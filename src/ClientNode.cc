#include <omnetpp.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <fstream>
#include <ctime>
#include <unordered_map>
#include <random>
#include <cmath>
#include "RemoteExecution_m.h"
// By B22CS061 & B22CS062
using namespace omnetpp;

class ClientNode : public cSimpleModule {
private:
    // Client identification
    std::string clientId;
    int clientNumericId;
    int numClients;
    std::vector<int> connectedClientIds;
    std::vector<int> fingerTable;     // Chord finger table for efficient routing
    std::map<int, int> clientToGateMap; // Maps clientID to gate index
    
    // Task management
    int currentTaskId;
    std::vector<int> taskArray;       // Array for the current task
    int subtaskCount;                 // Number of subtasks to divide task into
    bool isExecutingTask;             // Flag to indicate if client is currently executing a task
    
    // Subtask tracking
    struct SubtaskInfo {
        int taskId;
        int subtaskId;
        std::vector<int> values;
        int result;
        bool completed;
    };
    std::map<int, SubtaskInfo> mySubtasks;  // Subtasks assigned to this client (subtaskId -> info)
    std::map<int, std::map<int, int>> taskResults;  // taskId -> (subtaskId -> result)
    std::map<int, int> pendingResults;      // taskId -> count of pending subtasks
    
    // Gossip protocol
    std::map<std::string, bool> messageLog;  // Message hash -> Seen before?
    int receivedGossipCount;               // Count of unique gossip messages received
    
    // Output handling
    std::ofstream outFile;
    
    // Simulation control
    simtime_t gossipInterval;
    cMessage *gossipTimer;
    cMessage *taskTimer;
    
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    
    // Task management
    void initiateTask();
    void executeTask();
    void generateRandomArray(int size);
    std::vector<std::vector<int>> divideArray(int numSubtasks);
    void sendSubtaskToDestination(int taskId, int subtaskId, const std::vector<int>& values);
    int findNextHopToDestination(int destinationId);
    void processTaskResults(int taskId);
    
    // Subtask execution
    int findMaxElement(const std::vector<int>& values);
    void handleSubtask(TaskMessage *msg);
    void sendSubtaskResult(int taskId, int subtaskId, int result, const char* destinationId);
    
    // Gossip protocol
    void startGossiping();
    void sendGossipToClients();
    void processGossipMessage(GossipMessage *msg);
    std::string generateGossipContent();
    std::string hashMessage(const std::string& message);
};

Define_Module(ClientNode);

void ClientNode::initialize() {
    clientId = par("id").stdstringValue();
    clientNumericId = par("clientId").intValue();
    numClients = par("numClients").intValue();
    
    // Parse connected clients and finger table
    std::string connectedClientsStr = par("connectedClients").stdstringValue();
    std::string fingerTableStr = par("chordFingerTable").stdstringValue();
    
    EV_INFO << "Client " << clientId << " initialized with parameters:" << std::endl;
    EV_INFO << "  connectedClients = " << connectedClientsStr << std::endl;
    EV_INFO << "  chordFingerTable = " << fingerTableStr << std::endl;
    
    // Parse comma-separated client connections 
    std::stringstream ss_clients(connectedClientsStr);
    std::string clientIdStr;
    while (std::getline(ss_clients, clientIdStr, ',')) {
        if (!clientIdStr.empty()) {
            connectedClientIds.push_back(std::stoi(clientIdStr));
        }
    }
    
    // Parse comma-separated finger table entries
    std::stringstream ss_fingers(fingerTableStr);
    std::string fingerIdStr;
    while (std::getline(ss_fingers, fingerIdStr, ',')) {
        if (!fingerIdStr.empty()) {
            fingerTable.push_back(std::stoi(fingerIdStr));
        }
    }
    
    // Discover all gates and map them to client IDs
    int numGates = gateSize("out");
    EV_INFO << "Client " << clientId << " has " << numGates << " out gates" << std::endl;
    
    // Create a mapping between gate index and connected client ID
    // This will be different from what's in parameters since we defined connections in NED
    for (int i = 0; i < numGates; i++) {
        cGate *outGate = gate("out", i);
        if (outGate && outGate->isConnected()) {
            cModule *targetModule = outGate->getPathEndGate()->getOwnerModule();
            if (targetModule) {
                std::string targetName = targetModule->getName();
                if (targetName == "client") {
                    int targetIndex = targetModule->getIndex();
                    clientToGateMap[targetIndex] = i;
                    EV_INFO << "  Gate " << i << " is connected to client " << targetIndex << std::endl;
                }
            }
        }
    }
    
    // Initialize other variables
    currentTaskId = 0;
    subtaskCount = numClients * 2;  // Ensure x > N as per assignment
    isExecutingTask = false;
    receivedGossipCount = 0;
    
    // Setup output file
    std::string filename = "outputfile.txt";
    outFile.open(filename, std::ios::app);  // Open in append mode to allow multiple nodes to write
    if (!outFile.is_open()) {
        EV_ERROR << "Failed to open output file: " << filename << std::endl;
    }
    
    // Log initialization
    EV_INFO << "Client " << clientId << " (ID " << clientNumericId << ") initialized with " 
            << clientToGateMap.size() << " direct connections" << std::endl;
    outFile << "Client " << clientId << " (ID " << clientNumericId << ") initialized with " 
            << clientToGateMap.size() << " direct connections" << std::endl;
    
    // Setup timers
    gossipInterval = 5.0;
    gossipTimer = new cMessage("gossipTimer");
    taskTimer = new cMessage("taskTimer");
    
    // Only client 0 initiates a task at the start
    if (clientNumericId == 0) {
        scheduleAt(simTime() + 1.0, taskTimer);
    }
}

void ClientNode::handleMessage(cMessage *msg) {
    if (msg == taskTimer) {
        initiateTask();
    }
    else if (msg == gossipTimer) {
        sendGossipToClients();
        
        // Schedule next gossip
        scheduleAt(simTime() + gossipInterval, gossipTimer);
    }
    else if (TaskMessage *taskMsg = dynamic_cast<TaskMessage *>(msg)) {
        // Process incoming task message
        // Check if this task is meant for us or needs to be forwarded
        const char* destId = taskMsg->getDestinationId();
        std::string destIdStr(destId);
        
        if (destIdStr == clientId) {
            // Task is for this client, process it
            handleSubtask(taskMsg);
        } else {
            // Task needs to be forwarded to another client
            int destClientId = std::stoi(destIdStr.substr(6)); // Extract ID from "clientX"
            
            // Check for routing loops: if this message has already passed through
            // this node, it's stuck in a loop
            if (taskMsg->getHopCount() > numClients) {
                EV_ERROR << "Client " << clientId << " detected routing loop for task " 
                         << taskMsg->getTaskId() << " subtask " << taskMsg->getSubtaskId()
                         << " to " << destId << ", dropping message" << std::endl;
                delete msg;
                return;
            }
            
            // Increment hop count
            taskMsg->setHopCount(taskMsg->getHopCount() + 1);
            
            int nextHop = findNextHopToDestination(destClientId);
            
            // Update forwarding path
            EV_INFO << "Client " << clientId << " forwarding task " << taskMsg->getTaskId() 
                    << " subtask " << taskMsg->getSubtaskId() 
                    << " to client " << nextHop << std::endl;
            outFile << "Client " << clientId << " forwarding task " << taskMsg->getTaskId() 
                    << " subtask " << taskMsg->getSubtaskId() 
                    << " to client " << nextHop << std::endl;
            
            // Find the gate index for the next hop
            auto gateIt = clientToGateMap.find(nextHop);
            if (gateIt != clientToGateMap.end()) {
                int gateIndex = gateIt->second;
                send(taskMsg, "out", gateIndex);
            } else {
                EV_ERROR << "Client " << clientId << " could not find gate for client " << nextHop << std::endl;
                outFile << "Client " << clientId << " could not find gate for client " << nextHop << std::endl;
                delete taskMsg;
            }
        }
    }
    else if (ResultMessage *resultMsg = dynamic_cast<ResultMessage *>(msg)) {
        // Process result from another client
        const char* destId = resultMsg->getDestinationId();
        std::string destIdStr(destId);
        
        if (destIdStr == clientId) {
            // Result is for this client
            int taskId = resultMsg->getTaskId();
            int subtaskId = resultMsg->getSubtaskId();
            int result = resultMsg->getResult();
            const char* sourceId = resultMsg->getSourceId();
            
            EV_INFO << "Client " << clientId << " received result " << result 
                    << " for task " << taskId << ", subtask " << subtaskId 
                    << " from client " << sourceId << std::endl;
            outFile << "Client " << clientId << " received result " << result 
                    << " for task " << taskId << ", subtask " << subtaskId 
                    << " from client " << sourceId << std::endl;
            
            // Store the result
            taskResults[taskId][subtaskId] = result;
            pendingResults[taskId]--;
            
            // Check if we have all results for this task
            if (pendingResults[taskId] <= 0) {
                // Process the task results
                processTaskResults(taskId);
                
                // Start gossiping after completing a task
                startGossiping();
                
                // Mark task as complete
                isExecutingTask = false;
            }
        } else {
            // Result needs to be forwarded to another client
            int destClientId = std::stoi(destIdStr.substr(6)); // Extract ID from "clientX"
            int nextHop = findNextHopToDestination(destClientId);
            
            // Find the gate index for the next hop
            int gateIndex = -1;
            for (int i = 0; i < connectedClientIds.size(); i++) {
                if (connectedClientIds[i] == nextHop) {
                    gateIndex = i;
                    break;
                }
            }
            
            if (gateIndex != -1) {
                send(resultMsg, "out", gateIndex);
            } else {
                EV_ERROR << "Client " << clientId << " could not find gate for client " << nextHop << std::endl;
                outFile << "Client " << clientId << " could not find gate for client " << nextHop << std::endl;
                delete resultMsg;
            }
        }
    }
    else if (GossipMessage *gossipMsg = dynamic_cast<GossipMessage *>(msg)) {
        processGossipMessage(gossipMsg);
        delete msg;
    }
    else {
        delete msg;
    }
}

void ClientNode::initiateTask() {
    // Only start a new task if we're not already executing one
    if (!isExecutingTask) {
        isExecutingTask = true;
        executeTask();
    } else {
        // Reschedule task execution for later
        scheduleAt(simTime() + 5.0, taskTimer);
    }
}

void ClientNode::executeTask() {
    // Generate a random array for the task
    generateRandomArray(20 + 10 * (currentTaskId % 3)); // Vary array size
    
    EV_INFO << "Client " << clientId << " executing task " << currentTaskId 
            << " with array size " << taskArray.size() << std::endl;
    outFile << "Client " << clientId << " executing task " << currentTaskId 
            << " with array size " << taskArray.size() << std::endl;
    
    // Determine number of subtasks - ensure x > N and each subtask has at least 2 elements
    int maxSubtasks = taskArray.size() / 2;  // Each subtask needs at least 2 elements
    subtaskCount = std::min(numClients * 2, maxSubtasks);  // Ensure x > N
    
    EV_INFO << "Client " << clientId << " dividing task into " << subtaskCount << " subtasks" << std::endl;
    
    // Divide the array into subtasks
    std::vector<std::vector<int>> subtasks = divideArray(subtaskCount);
    
    // Initialize pending count
    pendingResults[currentTaskId] = subtasks.size();
    
    // Display subtask distribution information
    EV_INFO << "Client " << clientId << " distribution of subtasks:" << std::endl;
    for (int subtaskId = 0; subtaskId < subtasks.size(); subtaskId++) {
        int targetClientId = subtaskId % numClients;
        EV_INFO << "  Subtask " << subtaskId << " -> Client " << targetClientId 
                << " (size: " << subtasks[subtaskId].size() << ")" << std::endl;
    }
    
    // Send each subtask to the appropriate client
    for (int subtaskId = 0; subtaskId < subtasks.size(); subtaskId++) {
        sendSubtaskToDestination(currentTaskId, subtaskId, subtasks[subtaskId]);
    }
    
    currentTaskId++;
}

void ClientNode::generateRandomArray(int size) {
    taskArray.resize(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 1000);
    
    for (int i = 0; i < size; i++) {
        taskArray[i] = distrib(gen);
    }
}

std::vector<std::vector<int>> ClientNode::divideArray(int numSubtasks) {
    std::vector<std::vector<int>> result;
    
    // Ensure each subtask has at least 2 elements
    int maxPossibleSubtasks = taskArray.size() / 2;
    if (numSubtasks > maxPossibleSubtasks) {
        EV_INFO << "Warning: Reducing subtask count from " << numSubtasks 
                << " to " << maxPossibleSubtasks << " to ensure min 2 elements per subtask" << std::endl;
        numSubtasks = maxPossibleSubtasks;
    }
    
    // Distribute array elements evenly among subtasks
    int elementsPerSubtask = taskArray.size() / numSubtasks;
    int remainingElements = taskArray.size() % numSubtasks;
    
    EV_INFO << "Dividing array of size " << taskArray.size() << " into " 
            << numSubtasks << " subtasks with ~" << elementsPerSubtask << " elements each" << std::endl;
    
    int startIdx = 0;
    for (int i = 0; i < numSubtasks; i++) {
        int extraElement = (i < remainingElements) ? 1 : 0;
        int endIdx = startIdx + elementsPerSubtask + extraElement;
        
        std::vector<int> subtask(taskArray.begin() + startIdx, taskArray.begin() + endIdx);
        result.push_back(subtask);
        
        EV_INFO << "  Subtask " << i << ": " << subtask.size() << " elements [" 
                << startIdx << ":" << (endIdx-1) << "]" << std::endl;
        
        startIdx = endIdx;
    }
    
    return result;
}

void ClientNode::sendSubtaskToDestination(int taskId, int subtaskId, const std::vector<int>& values) {
    // Determine which client should handle this subtask: subtaskId % numClients
    int targetClientId = subtaskId % numClients;
    
    // Find the next hop in the path to the target client
    int nextHop = findNextHopToDestination(targetClientId);
    
    // Create task message
    TaskMessage *taskMsg = new TaskMessage();
    taskMsg->setSourceId(clientId.c_str());
    taskMsg->setDestinationId(("client" + std::to_string(targetClientId)).c_str());
    taskMsg->setTimestamp(simTime().inUnit(SIMTIME_S));
    taskMsg->setTaskId(taskId);
    taskMsg->setSubtaskId(subtaskId);
    
    // Set the values array
    taskMsg->setValuesArraySize(values.size());
    for (int i = 0; i < values.size(); i++) {
        taskMsg->setValues(i, values[i]);
    }
    
    // Find the gate index for the next hop using our gate mapping
    auto gateIt = clientToGateMap.find(nextHop);
    if (gateIt != clientToGateMap.end()) {
        int gateIndex = gateIt->second;
        
        EV_INFO << "Client " << clientId << " sending subtask " << subtaskId 
                << " of task " << taskId << " to client " << targetClientId 
                << " via next hop " << nextHop << " (gate " << gateIndex << ")" << std::endl;
        outFile << "Client " << clientId << " sending subtask " << subtaskId 
                << " of task " << taskId << " to client " << targetClientId 
                << " via next hop " << nextHop << std::endl;
        
        send(taskMsg, "out", gateIndex);
    } else {
        EV_ERROR << "Client " << clientId << " could not find gate for client " << nextHop 
                << " (available: ";
        for (const auto& pair : clientToGateMap) {
            EV_ERROR << pair.first << " ";
        }
        EV_ERROR << ")" << std::endl;
        outFile << "Client " << clientId << " could not find gate for client " << nextHop << std::endl;
        delete taskMsg;
    }
}

int ClientNode::findNextHopToDestination(int destinationId) {
    // If this message is for ourselves, no need to route
    if (destinationId == clientNumericId) {
        return clientNumericId;
    }
    
    // If we're directly connected to the destination, route directly
    if (std::find(connectedClientIds.begin(), connectedClientIds.end(), destinationId) != connectedClientIds.end()) {
        EV_INFO << "Client " << clientId << " is directly connected to destination " << destinationId << std::endl;
        return destinationId;
    }
    
    // Calculate clockwise distance in the ring
    int clockwiseDistance = (destinationId - clientNumericId + numClients) % numClients;
    
    // If target is our successor, route directly through the ring
    if (clockwiseDistance == 1) {
        int successor = (clientNumericId + 1) % numClients;
        EV_INFO << "Client " << clientId << " routing to successor " << successor << std::endl;
        return successor;
    }
    
    // Use Chord finger table for O(log N) routing
    // Find the closest preceding node in our finger table
    int bestFinger = -1;
    int bestDistance = numClients + 1;
    
    EV_INFO << "Client " << clientId << " using finger table to route to " << destinationId 
            << ", fingers: ";
    for (int finger : fingerTable) {
        EV_INFO << finger << " ";
    }
    EV_INFO << std::endl;
    
    for (int fingerNode : fingerTable) {
        // Calculate distance from finger to destination (clockwise)
        int fingerToDestDistance = (destinationId - fingerNode + numClients) % numClients;
        
        // The finger must be before the destination in clockwise direction
        if (fingerToDestDistance > 0 && fingerToDestDistance < bestDistance) {
            bestDistance = fingerToDestDistance;
            bestFinger = fingerNode;
        }
    }
    
    // If we found a suitable finger, use it
    if (bestFinger != -1) {
        EV_INFO << "Client " << clientId << " routing to " << destinationId 
                << " via finger " << bestFinger << std::endl;
        return bestFinger;
    }
    
    // Last resort: Just route to our successor in the ring
    int successor = (clientNumericId + 1) % numClients;
    EV_INFO << "Client " << clientId << " (fallback) routing to successor " << successor << std::endl;
    return successor;
}

void ClientNode::handleSubtask(TaskMessage *msg) {
    int taskId = msg->getTaskId();
    int subtaskId = msg->getSubtaskId();
    
    EV_INFO << "Client " << clientId << " processing subtask " << subtaskId 
            << " of task " << taskId << std::endl;
    outFile << "Client " << clientId << " processing subtask " << subtaskId 
            << " of task " << taskId << std::endl;
    
    // Extract values from the message
    int numValues = msg->getValuesArraySize();
    std::vector<int> values(numValues);
    for (int i = 0; i < numValues; i++) {
        values[i] = msg->getValues(i);
    }
    
    // Perform computation (find maximum element)
    int result = findMaxElement(values);
    
    // Create a subtask record
    SubtaskInfo info;
    info.taskId = taskId;
    info.subtaskId = subtaskId;
    info.values = values;
    info.result = result;
    info.completed = true;
    mySubtasks[subtaskId] = info;
    
    // Send result back to the source
    sendSubtaskResult(taskId, subtaskId, result, msg->getSourceId());
    
    delete msg;
}

int ClientNode::findMaxElement(const std::vector<int>& values) {
    // Find the maximum element in the array
    return *std::max_element(values.begin(), values.end());
}

void ClientNode::sendSubtaskResult(int taskId, int subtaskId, int result, const char* destinationId) {
    // Create result message
    ResultMessage *resultMsg = new ResultMessage();
    resultMsg->setSourceId(clientId.c_str());
    resultMsg->setDestinationId(destinationId);
    resultMsg->setTimestamp(simTime().inUnit(SIMTIME_S));
    resultMsg->setTaskId(taskId);
    resultMsg->setSubtaskId(subtaskId);
    resultMsg->setResult(result);
    resultMsg->setIsValid(true); // Always valid (no malicious clients)
    
    // Extract numeric client ID from destination
    std::string destStr(destinationId);
    int destClientId = std::stoi(destStr.substr(6)); // Extract ID from "clientX"
    
    // Find next hop to destination
    int nextHop = findNextHopToDestination(destClientId);
    
    // Find gate index
    int gateIndex = -1;
    for (int i = 0; i < connectedClientIds.size(); i++) {
        if (connectedClientIds[i] == nextHop) {
            gateIndex = i;
            break;
        }
    }
    
    if (gateIndex != -1) {
        EV_INFO << "Client " << clientId << " sending result " << result 
                << " for task " << taskId << ", subtask " << subtaskId 
                << " to " << destinationId << " via next hop " << nextHop << std::endl;
        outFile << "Client " << clientId << " sending result " << result 
                << " for task " << taskId << ", subtask " << subtaskId 
                << " to " << destinationId << " via next hop " << nextHop << std::endl;
        
        send(resultMsg, "out", gateIndex);
    } else {
        EV_ERROR << "Client " << clientId << " could not find gate for client " << nextHop << std::endl;
        outFile << "Client " << clientId << " could not find gate for client " << nextHop << std::endl;
        delete resultMsg;
    }
}

void ClientNode::processTaskResults(int taskId) {
    EV_INFO << "Client " << clientId << " processing results for task " << taskId << std::endl;
    outFile << "Client " << clientId << " processing results for task " << taskId << std::endl;
    
    // Display all subtask results
    for (const auto& resultPair : taskResults[taskId]) {
        int subtaskId = resultPair.first;
        int result = resultPair.second;
        
        EV_INFO << "Client " << clientId << " received result for task " << taskId 
                << ", subtask " << subtaskId << ": " << result << std::endl;
        outFile << "Client " << clientId << " received result for task " << taskId 
                << ", subtask " << subtaskId << ": " << result << std::endl;
    }
    
    // Compute final result (maximum of all subtask results)
    int finalResult = 0;
    for (const auto& resultPair : taskResults[taskId]) {
        finalResult = std::max(finalResult, resultPair.second);
    }
    
    EV_INFO << "Client " << clientId << " final result for task " << taskId << " is " << finalResult << std::endl;
    outFile << "Client " << clientId << " final result for task " << taskId << " is " << finalResult << std::endl;
}

void ClientNode::startGossiping() {
    // Schedule first gossip message
    if (!gossipTimer->isScheduled()) {
        scheduleAt(simTime() + 1.0, gossipTimer);
    }
}

void ClientNode::sendGossipToClients() {
    std::string content = generateGossipContent();
    std::string hash = hashMessage(content);
    
    // Add to our own message log
    messageLog[hash] = true;
    
    // Send to all connected clients
    for (int i = 0; i < connectedClientIds.size(); i++) {
        GossipMessage *gossipMsg = new GossipMessage();
        gossipMsg->setSourceId(clientId.c_str());
        gossipMsg->setDestinationId(("client" + std::to_string(connectedClientIds[i])).c_str());
        gossipMsg->setTimestamp(simTime().inUnit(SIMTIME_S));
        gossipMsg->setContent(content.c_str());
        gossipMsg->setFirstTime(true);
        
        EV_INFO << "Client " << clientId << " sending gossip to client " << connectedClientIds[i] 
                << ": " << content << std::endl;
        outFile << "Client " << clientId << " sending gossip to client " << connectedClientIds[i] 
                << ": " << content << std::endl;
        
        send(gossipMsg, "out", i);
    }
}

void ClientNode::processGossipMessage(GossipMessage *msg) {
    std::string content = msg->getContent();
    std::string hash = hashMessage(content);
    bool firstTime = msg->getFirstTime();
    
    // Log the receipt of this gossip message
    EV_INFO << "Client " << clientId << " received gossip from " << msg->getSourceId() 
            << ": " << content << std::endl;
    outFile << "Client " << clientId << " received gossip from " << msg->getSourceId() 
            << ": " << content << " at time " << simTime() << std::endl;
    
    // Check if we've seen this message before
    if (messageLog.find(hash) != messageLog.end()) {
        // We've seen this message, ignore it
        return;
    }
    
    // Add to message log
    messageLog[hash] = true;
    receivedGossipCount++;
    
    // If we've received gossip from all clients, terminate
    if (receivedGossipCount >= numClients) {
        EV_INFO << "Client " << clientId << " received gossip from all clients, terminating" << std::endl;
        outFile << "Client " << clientId << " received gossip from all clients, terminating" << std::endl;
        
        // Cancel any pending events
        if (gossipTimer->isScheduled()) {
            cancelEvent(gossipTimer);
        }
        if (taskTimer->isScheduled()) {
            cancelEvent(taskTimer);
        }
    }
    
    // Forward to all other clients if this is the first time
    if (firstTime) {
        for (int i = 0; i < connectedClientIds.size(); i++) {
            int targetClientId = connectedClientIds[i];
            const char* sourceId = msg->getSourceId();
            std::string sourceStr(sourceId);
            int sourceClientId = std::stoi(sourceStr.substr(6)); // Extract ID from "clientX"
            
            // Don't send back to the sender
            if (targetClientId != sourceClientId) {
                GossipMessage *forwardMsg = new GossipMessage();
                forwardMsg->setSourceId(clientId.c_str());
                forwardMsg->setDestinationId(("client" + std::to_string(targetClientId)).c_str());
                forwardMsg->setTimestamp(simTime().inUnit(SIMTIME_S));
                forwardMsg->setContent(content.c_str());
                forwardMsg->setFirstTime(false);
                
                send(forwardMsg, "out", i);
            }
        }
    }
}

std::string ClientNode::generateGossipContent() {
    // Format: <timestamp>:<clientIP>:<clientID>
    std::stringstream ss;
    
    // Add timestamp
    ss << simTime().inUnit(SIMTIME_S) << ":";
    
    // Add client ID
    ss << clientId << ":";
    
    // Add numeric client ID
    ss << clientNumericId;
    
    return ss.str();
}

std::string ClientNode::hashMessage(const std::string& message) {
    // Simple hash function for demonstration
    std::hash<std::string> hasher;
    return std::to_string(hasher(message));
}

void ClientNode::finish() {
    // Clean up
    if (gossipTimer && gossipTimer->isScheduled()) {
        cancelEvent(gossipTimer);
    }
    delete gossipTimer;
    
    if (taskTimer && taskTimer->isScheduled()) {
        cancelEvent(taskTimer);
    }
    delete taskTimer;
    
    // Close output file
    if (outFile.is_open()) {
        outFile.close();
    }
} 