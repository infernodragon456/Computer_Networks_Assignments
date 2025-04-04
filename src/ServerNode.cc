#include <omnetpp.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include "RemoteExecution_m.h"

using namespace omnetpp;
// By B22CS061 & B22CS062
/**
 * ServerNode module
 * 
 * Represents a server in the remote execution network that:
 * 1. Receives subtasks from clients
 * 2. Processes the subtasks (finds maximum value in this implementation)
 * 3. May behave maliciously with a certain probability if configured to do so
 * 4. Returns results back to the client
 */
class ServerNode : public cSimpleModule {
private:
    std::string serverId;    // Unique identifier for this server
    bool isMalicious;        // Whether this server can act maliciously
    std::ofstream outFile;   // Output file for logging

protected:
    virtual void initialize() override;               // Initialize the server
    virtual void handleMessage(cMessage *msg) override;  // Process incoming messages
    int computeMaximum(const std::vector<int>& values);  // Find the maximum value in a vector
    int corruptResult(int result);                    // Deliberately corrupt a result (for malicious behavior)
};

Define_Module(ServerNode);

/**
 * Initialize the server node
 * 
 * Sets up server parameters and output file for logging
 */
void ServerNode::initialize() {
    serverId = par("id").stdstringValue();
    isMalicious = par("malicious").boolValue();
    
    // Setup output file
    std::string filename = "outputfile.txt";
    outFile.open(filename, std::ios::app);  // Open in append mode
    if (!outFile.is_open()) {
        EV_ERROR << "Failed to open output file: " << filename << std::endl;
    }
    
    EV_INFO << "Server " << serverId << " initialized. Malicious: " << (isMalicious ? "true" : "false") << std::endl;
    outFile << "Server " << serverId << " initialized. Malicious: " << (isMalicious ? "true" : "false") << std::endl;
}

/**
 * Handle incoming messages
 * 
 * Processes task messages, computes results, and sends them back to clients
 */
void ServerNode::handleMessage(cMessage *msg) {
    if (TaskMessage *taskMsg = dynamic_cast<TaskMessage *>(msg)) {
        EV_INFO << "Server " << serverId << " received task " << taskMsg->getTaskId() 
                << ", subtask " << taskMsg->getSubtaskId() << std::endl;
        outFile << "Server " << serverId << " received task " << taskMsg->getTaskId() 
                << ", subtask " << taskMsg->getSubtaskId() << std::endl;
        
        // Extract array values
        int arraySize = taskMsg->getValuesArraySize();
        std::vector<int> values(arraySize);
        for (int i = 0; i < arraySize; i++) {
            values[i] = taskMsg->getValues(i);
        }
        
        // Compute result
        int result = computeMaximum(values);
        
        // If malicious, potentially corrupt the result
        bool resultIsValid = true;
        if (isMalicious) {
            // 75% chance to be malicious for this task
            if (uniform(0, 1) < 0.75) {
                result = corruptResult(result);
                resultIsValid = false;
                EV_INFO << "Server " << serverId << " acting maliciously for task " << taskMsg->getTaskId() 
                        << ", subtask " << taskMsg->getSubtaskId() << std::endl;
                outFile << "Server " << serverId << " acting maliciously for task " << taskMsg->getTaskId() 
                        << ", subtask " << taskMsg->getSubtaskId() << std::endl;
            }
        }
        
        // Create and send result message
        ResultMessage *resultMsg = new ResultMessage();
        resultMsg->setSourceId(serverId.c_str());
        resultMsg->setDestinationId(taskMsg->getSourceId());
        resultMsg->setTimestamp(simTime().inUnit(SIMTIME_S));
        resultMsg->setTaskId(taskMsg->getTaskId());
        resultMsg->setSubtaskId(taskMsg->getSubtaskId());
        resultMsg->setResult(result);
        resultMsg->setIsValid(resultIsValid);
        
        EV_INFO << "Server " << serverId << " sending result " << result << " for task " << taskMsg->getTaskId() 
                << ", subtask " << taskMsg->getSubtaskId() << std::endl;
        outFile << "Server " << serverId << " sending result " << result << " for task " << taskMsg->getTaskId() 
                << ", subtask " << taskMsg->getSubtaskId() << std::endl;
        
        // Send to the gate that the task message came from
        send(resultMsg, "out", msg->getArrivalGate()->getIndex());
        
        delete msg;
    } else {
        delete msg;
    }
}

/**
 * Find the maximum value in a vector of integers
 * 
 * @param values Vector of integers
 * @return The maximum value, or 0 if the vector is empty
 */
int ServerNode::computeMaximum(const std::vector<int>& values) {
    if (values.empty()) return 0;
    return *std::max_element(values.begin(), values.end());
}

/**
 * Deliberately corrupt a result (for malicious behavior)
 * 
 * @param result The correct result
 * @return A corrupted (incorrect) result
 */
int ServerNode::corruptResult(int result) {
    // Simple corruption: return a different number
    return result - 10 - intuniform(1, 10);
} 