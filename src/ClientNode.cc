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
#include "RemoteExecution_m.h"
// By B22CS061 & B22CS062
using namespace omnetpp;

class ClientNode : public cSimpleModule {
private:
    // Client identification
    std::string clientId;
    int numServers;
    std::vector<int> connectedServerIds;
    std::vector<int> connectedClientIds;
    
    // Task management
    int currentTaskId;
    std::vector<int> taskArray;  // Array for the current task
    int taskRound;               // Current round (0 for first, 1 for second)
    
    // Server ratings and results tracking
    std::map<int, double> serverRatings;  // Server ID -> Rating
    std::map<int, std::map<int, std::set<int>>> subtaskResults;  // TaskID -> (SubtaskID -> Set of results)
    
    // Store actual result messages for later reference
    std::map<int, std::map<int, std::map<int, std::pair<bool, int>>>> serverResponses;  // TaskID -> (SubtaskID -> (ServerID -> (IsValid, Result)))
    
    // Gossip protocol
    std::map<std::string, bool> messageLog;  // Message hash -> Seen before?
    
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
    void scheduleTask();
    void executeTask();
    void generateRandomArray(int size);
    std::vector<std::vector<int>> divideArray(int numSubtasks);
    std::vector<int> selectServersForSubtask();
    void sendSubtaskToServer(int serverId, int subtaskId, const std::vector<int>& values);
    void processTaskResults(int taskId);
    
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
    numServers = par("numServers").intValue();
    
    // Parse connected servers and clients
    std::string connectedServersStr = par("connectedServers").stdstringValue();
    std::string connectedClientsStr = par("connectedClients").stdstringValue();
    
    // Parse comma-separated values
    std::stringstream ss_servers(connectedServersStr);
    std::string serverIdStr;
    while (std::getline(ss_servers, serverIdStr, ',')) {
        connectedServerIds.push_back(std::stoi(serverIdStr));
    }
    
    std::stringstream ss_clients(connectedClientsStr);
    std::string clientIdStr;
    while (std::getline(ss_clients, clientIdStr, ',')) {
        connectedClientIds.push_back(std::stoi(clientIdStr));
    }
    
    // Initialize other variables
    currentTaskId = 0;
    taskRound = 0;
    
    // Initialize server ratings to 0.5 (neutral)
    for (int i = 0; i < numServers; i++) {
        serverRatings[i] = 0.5;
    }
    
    // Setup output file
    std::string filename = "outputfile.txt";
    outFile.open(filename, std::ios::app);  // Open in append mode to allow multiple nodes to write
    if (!outFile.is_open()) {
        EV_ERROR << "Failed to open output file: " << filename << std::endl;
    }
    
    // Log initialization
    EV_INFO << "Client " << clientId << " initialized with " << connectedServerIds.size() 
            << " server connections and " << connectedClientIds.size() << " client connections" << std::endl;
    outFile << "Client " << clientId << " initialized with " << connectedServerIds.size() 
            << " server connections and " << connectedClientIds.size() << " client connections" << std::endl;
    
    // Setup timers
    gossipInterval = 5.0;
    gossipTimer = new cMessage("gossipTimer");
    taskTimer = new cMessage("taskTimer");
    
    // Schedule first task with a random delay
    scheduleAt(simTime() + exponential(1.0), taskTimer);
}

void ClientNode::handleMessage(cMessage *msg) {
    if (msg == taskTimer) {
        executeTask();
        
        // Schedule next task if we haven't completed both rounds
        if (taskRound < 2) {
            scheduleAt(simTime() + 20.0, taskTimer);
        }
    }
    else if (msg == gossipTimer) {
        sendGossipToClients();
        
        // Schedule next gossip
        scheduleAt(simTime() + gossipInterval, gossipTimer);
    }
    else if (ResultMessage *resultMsg = dynamic_cast<ResultMessage *>(msg)) {
        // Process result from server
        int taskId = resultMsg->getTaskId();
        int subtaskId = resultMsg->getSubtaskId();
        int result = resultMsg->getResult();
        int senderId = std::stoi(resultMsg->getSourceId() + 6); // Extract server ID from "serverX"
        bool isValid = resultMsg->isValid();
        
        EV_INFO << "Client " << clientId << " received result " << result 
                << " for task " << taskId << ", subtask " << subtaskId 
                << " from server " << senderId << std::endl;
        outFile << "Client " << clientId << " received result " << result 
                << " for task " << taskId << ", subtask " << subtaskId 
                << " from server " << senderId << std::endl;
        
        // Store the result
        subtaskResults[taskId][subtaskId].insert(result);
        serverResponses[taskId][subtaskId][senderId] = std::make_pair(isValid, result);
        
        // Check if we have all results for this task
        if (serverResponses[taskId].size() == taskArray.size() / 2) {
            bool allComplete = true;
            for (const auto& subtask : serverResponses[taskId]) {
                if (subtask.second.size() < (numServers / 2 + 1)) {
                    allComplete = false;
                    break;
                }
            }
            
            if (allComplete) {
                // Process the task results
                processTaskResults(taskId);
                
                // Start gossiping after completing a task
                if (!gossipTimer->isScheduled()) {
                    scheduleAt(simTime() + 1.0, gossipTimer);
                }
            }
        }
        
        delete msg;
    }
    else if (GossipMessage *gossipMsg = dynamic_cast<GossipMessage *>(msg)) {
        processGossipMessage(gossipMsg);
        delete msg;
    }
    else {
        delete msg;
    }
}

void ClientNode::executeTask() {
    // Generate a random array for the task
    generateRandomArray(20 + taskRound * 10); // Larger array for second round
    
    EV_INFO << "Client " << clientId << " executing task " << currentTaskId 
            << " in round " << taskRound << " with array size " << taskArray.size() << std::endl;
    outFile << "Client " << clientId << " executing task " << currentTaskId 
            << " in round " << taskRound << " with array size " << taskArray.size() << std::endl;
    
    // Divide the array into n subtasks
    std::vector<std::vector<int>> subtasks = divideArray(numServers);
    
    // Send each subtask to n/2+1 servers
    for (int subtaskId = 0; subtaskId < subtasks.size(); subtaskId++) {
        std::vector<int> servers;
        
        if (taskRound == 0) {
            // First round: random selection
            servers = selectServersForSubtask();
        } else {
            // Second round: select top-rated servers
            std::vector<std::pair<int, double>> sortedRatings;
            for (const auto& rating : serverRatings) {
                sortedRatings.push_back(rating);
            }
            
            std::sort(sortedRatings.begin(), sortedRatings.end(), 
                     [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
                         return a.second > b.second;
                     });
            
            // Take top n/2+1 servers
            for (int i = 0; i < numServers / 2 + 1 && i < sortedRatings.size(); i++) {
                servers.push_back(sortedRatings[i].first);
            }
        }
        
        // Send the subtask to each selected server
        for (int serverId : servers) {
            sendSubtaskToServer(serverId, subtaskId, subtasks[subtaskId]);
        }
    }
    
    currentTaskId++;
}

void ClientNode::generateRandomArray(int size) {
    taskArray.resize(size);
    for (int i = 0; i < size; i++) {
        taskArray[i] = intuniform(1, 1000);
    }
}

std::vector<std::vector<int>> ClientNode::divideArray(int numSubtasks) {
    std::vector<std::vector<int>> result;
    
    // Ensure each subtask has at least 2 elements
    if (taskArray.size() / numSubtasks < 2) {
        numSubtasks = taskArray.size() / 2;
    }
    
    int elementsPerSubtask = taskArray.size() / numSubtasks;
    int remainingElements = taskArray.size() % numSubtasks;
    
    int startIdx = 0;
    for (int i = 0; i < numSubtasks; i++) {
        int endIdx = startIdx + elementsPerSubtask + (i < remainingElements ? 1 : 0);
        std::vector<int> subtask(taskArray.begin() + startIdx, taskArray.begin() + endIdx);
        result.push_back(subtask);
        startIdx = endIdx;
    }
    
    return result;
}

std::vector<int> ClientNode::selectServersForSubtask() {
    // Select n/2+1 random servers from the connected servers
    int serversNeeded = numServers / 2 + 1;
    std::vector<int> selectedServers;
    
    // If we have fewer connected servers than needed, use all of them
    if (connectedServerIds.size() <= serversNeeded) {
        return connectedServerIds;
    }
    
    // Randomly select servers
    std::vector<int> serverIndices(connectedServerIds.size());
    std::iota(serverIndices.begin(), serverIndices.end(), 0);
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(serverIndices.begin(), serverIndices.end(), g);
    
    for (int i = 0; i < serversNeeded; i++) {
        selectedServers.push_back(connectedServerIds[serverIndices[i]]);
    }
    
    return selectedServers;
}

void ClientNode::sendSubtaskToServer(int serverId, int subtaskId, const std::vector<int>& values) {
    TaskMessage *taskMsg = new TaskMessage();
    taskMsg->setSourceId(clientId.c_str());
    taskMsg->setDestinationId(("server" + std::to_string(serverId)).c_str());
    taskMsg->setTimestamp(simTime().inUnit(SIMTIME_S));
    taskMsg->setTaskId(currentTaskId);
    taskMsg->setSubtaskId(subtaskId);
    
    // Set the values array
    taskMsg->setValuesArraySize(values.size());
    for (int i = 0; i < values.size(); i++) {
        taskMsg->setValues(i, values[i]);
    }
    
    // Find the gate index for this server
    int gateIndex = -1;
    for (int i = 0; i < connectedServerIds.size(); i++) {
        if (connectedServerIds[i] == serverId) {
            gateIndex = i;
            break;
        }
    }
    
    if (gateIndex != -1) {
        EV_INFO << "Client " << clientId << " sending subtask " << subtaskId 
                << " of task " << currentTaskId << " to server " << serverId << std::endl;
        outFile << "Client " << clientId << " sending subtask " << subtaskId 
                << " of task " << currentTaskId << " to server " << serverId << std::endl;
        
        send(taskMsg, "out", gateIndex);
    } else {
        EV_ERROR << "Client " << clientId << " could not find gate for server " << serverId << std::endl;
        outFile << "Client " << clientId << " could not find gate for server " << serverId << std::endl;
        delete taskMsg;
    }
}

void ClientNode::processTaskResults(int taskId) {
    EV_INFO << "Client " << clientId << " processing results for task " << taskId << std::endl;
    outFile << "Client " << clientId << " processing results for task " << taskId << std::endl;
    
    // For each subtask, determine the correct result by majority
    std::map<int, int> subtaskCorrectResults;  // SubtaskID -> Correct result
    
    for (const auto& subtaskPair : subtaskResults[taskId]) {
        int subtaskId = subtaskPair.first;
        const std::set<int>& results = subtaskPair.second;
        
        // Count occurrences of each result
        std::map<int, int> resultCounts;
        for (int result : results) {
            resultCounts[result]++;
        }
        
        // Find the result with the most occurrences
        int maxCount = 0;
        int correctResult = 0;
        
        for (const auto& countPair : resultCounts) {
            if (countPair.second > maxCount) {
                maxCount = countPair.second;
                correctResult = countPair.first;
            }
        }
        
        subtaskCorrectResults[subtaskId] = correctResult;
        
        EV_INFO << "Client " << clientId << " determined correct result for task " << taskId 
                << ", subtask " << subtaskId << " is " << correctResult << std::endl;
        outFile << "Client " << clientId << " determined correct result for task " << taskId 
                << ", subtask " << subtaskId << " is " << correctResult << std::endl;
    }
    
    // Find the final result (maximum of all subtask results)
    int finalResult = 0;
    for (const auto& resultPair : subtaskCorrectResults) {
        finalResult = std::max(finalResult, resultPair.second);
    }
    
    EV_INFO << "Client " << clientId << " final result for task " << taskId << " is " << finalResult << std::endl;
    outFile << "Client " << clientId << " final result for task " << taskId << " is " << finalResult << std::endl;
    
    // Update server ratings based on their responses
    for (const auto& subtaskPair : serverResponses[taskId]) {
        int subtaskId = subtaskPair.first;
        int correctResult = subtaskCorrectResults[subtaskId];
        
        for (const auto& serverPair : subtaskPair.second) {
            int serverId = serverPair.first;
            // Get result from the map (isValid not used here)
            int result = serverPair.second.second;
            
            // Check if server provided correct result
            if (result == correctResult) {
                serverRatings[serverId] += 0.1;  // Increase rating for correct results
                EV_INFO << "Client " << clientId << " increasing rating for server " << serverId << std::endl;
                outFile << "Client " << clientId << " increasing rating for server " << serverId << std::endl;
            } else {
                serverRatings[serverId] -= 0.1;  // Decrease rating for incorrect results
                EV_INFO << "Client " << clientId << " decreasing rating for server " << serverId << std::endl;
                outFile << "Client " << clientId << " decreasing rating for server " << serverId << std::endl;
            }
            
            // Ensure ratings stay within bounds
            serverRatings[serverId] = std::max(0.0, std::min(1.0, serverRatings[serverId]));
        }
    }
    
    // Increment task round after processing
    taskRound++;
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
        
        // Find the gate index for this client
        int clientGateOffset = connectedServerIds.size(); // Client gates start after server gates
        send(gossipMsg, "out", clientGateOffset + i);
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
    
    // Parse the gossip content to update server ratings
    // Format: <timestamp>:<clientIP>:<serverScores>
    std::stringstream ss(content);
    std::string timestampStr, clientIP, scoresStr;
    
    std::getline(ss, timestampStr, ':');
    std::getline(ss, clientIP, ':');
    std::getline(ss, scoresStr, ':');
    
    // Parse the scores (assuming format is "serverId1=score1,serverId2=score2,...")
    std::stringstream scoresStream(scoresStr);
    std::string scorePair;
    
    while (std::getline(scoresStream, scorePair, ',')) {
        std::stringstream pairStream(scorePair);
        std::string serverIdStr, scoreStr;
        
        std::getline(pairStream, serverIdStr, '=');
        std::getline(pairStream, scoreStr);
        
        int serverId = std::stoi(serverIdStr);
        double score = std::stod(scoreStr);
        
        // Update our rating for this server (simple average)
        serverRatings[serverId] = (serverRatings[serverId] + score) / 2.0;
    }
    
    // Forward to all other clients if this is the first time
    if (firstTime) {
        for (int i = 0; i < connectedClientIds.size(); i++) {
            int targetClientId = connectedClientIds[i];
            int sourceClientId = std::stoi(msg->getSourceId() + 6); // Extract ID from "clientX"
            
            // Don't send back to the sender
            if (targetClientId != sourceClientId) {
                GossipMessage *forwardMsg = new GossipMessage();
                forwardMsg->setSourceId(clientId.c_str());
                forwardMsg->setDestinationId(("client" + std::to_string(targetClientId)).c_str());
                forwardMsg->setTimestamp(simTime().inUnit(SIMTIME_S));
                forwardMsg->setContent(content.c_str());
                forwardMsg->setFirstTime(false);
                
                // Find the gate index for this client
                int clientGateOffset = connectedServerIds.size(); // Client gates start after server gates
                send(forwardMsg, "out", clientGateOffset + i);
            }
        }
    }
}

std::string ClientNode::generateGossipContent() {
    // Format: <timestamp>:<clientIP>:<serverScores>
    std::stringstream ss;
    
    // Add timestamp
    ss << simTime().inUnit(SIMTIME_S) << ":";
    
    // Add client ID
    ss << clientId << ":";
    
    // Add server scores
    bool first = true;
    for (const auto& rating : serverRatings) {
        if (!first) {
            ss << ",";
        }
        ss << rating.first << "=" << rating.second;
        first = false;
    }
    
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