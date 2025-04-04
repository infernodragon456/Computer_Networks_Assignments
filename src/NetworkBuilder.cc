#include <omnetpp.h>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <unistd.h> // For getcwd
// By B22CS061 & B22CS062
using namespace omnetpp;

/**
 * NetworkBuilder module
 * 
 * Responsible for dynamically building the network topology based on configuration file.
 * This module reads the network configuration from a file (config.txt) and:
 * 1. Creates the specified number of server and client nodes
 * 2. Sets up all connections between clients and servers
 * 3. Sets up all connections between clients and other clients
 * 4. Configures server properties (e.g., whether they're malicious)
 */
class NetworkBuilder : public cSimpleModule {
private:
    std::string topoFileName;  // Path to the topology configuration file
    int numServers;            // Total number of servers in the network
    int numClients;            // Total number of clients in the network
    
    // Maps to store connection information
    std::map<int, std::vector<int>> clientToServerConnections;  // Maps client ID to list of server IDs
    std::map<int, std::vector<int>> clientToClientConnections;  // Maps client ID to list of client IDs
    std::map<int, bool> serverMalicious;                        // Maps server ID to malicious flag

protected:
    virtual void initialize() override;     // Initialize the module and build the network
    virtual void handleMessage(cMessage *msg) override;   // Handle incoming messages
    void readTopoFile();                    // Read network topology from configuration file
    void setupNetwork();                    // Set up the network based on the topology
};

Define_Module(NetworkBuilder);

/**
 * Initialize the NetworkBuilder module
 * 
 * This method looks for the configuration file, reads the network topology,
 * and sets up the network connections and properties.
 */
void NetworkBuilder::initialize() {
    // Try different paths to find the configuration file
    const char* possiblePaths[] = {
        "config.txt",
        "./config.txt",
        "../config.txt",
        "../../config.txt",
        "../../../config.txt",
        "src/config.txt",
        "./src/config.txt",
        "../src/config.txt"
    };
    
    bool fileFound = false;
    for (const char* path : possiblePaths) {
        std::ifstream testFile(path);
        if (testFile.is_open()) {
            topoFileName = path;
            testFile.close();
            fileFound = true;
            EV_INFO << "Found configuration file at: " << topoFileName << std::endl;
            break;
        }
    }
    
    if (!fileFound) {
        // Output current working directory for debugging
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            EV_ERROR << "Current working directory: " << cwd << std::endl;
        }
        EV_ERROR << "Could not find configuration file (config.txt) in any of the common locations." << std::endl;
        
        // Create a default topology in memory if file cannot be found
        EV_INFO << "Creating default topology in memory..." << std::endl;
        numServers = 5;
        numClients = 3;
        
        // Default client to server connections
        for (int i = 0; i < numClients; i++) {
            for (int j = 0; j < numServers; j++) {
                clientToServerConnections[i].push_back(j);
            }
        }
        
        // Default client to client connections
        for (int i = 0; i < numClients; i++) {
            for (int j = 0; j < numClients; j++) {
                if (i != j) {
                    clientToClientConnections[i].push_back(j);
                }
            }
        }
        
        // Default all servers are honest except one
        for (int i = 0; i < numServers; i++) {
            serverMalicious[i] = (i == 3);  // Make server 3 malicious by default
        }
    } else {
        // Read topology file
        readTopoFile();
    }
    
    // Setup network
    setupNetwork();
}

/**
 * Handle incoming messages
 * 
 * The NetworkBuilder doesn't process messages; it only builds the network.
 */
void NetworkBuilder::handleMessage(cMessage *msg) {
    // This module doesn't handle messages, just delete them
    delete msg;
}

/**
 * Read the network topology from the configuration file
 * 
 * Parses the configuration file to extract:
 * - Number of servers and clients
 * - Client-server connections
 * - Client-client connections
 * - Server malicious flags
 */
void NetworkBuilder::readTopoFile() {
    std::ifstream file(topoFileName);
    if (!file.is_open()) {
        throw cRuntimeError("Failed to open topology file: %s", topoFileName.c_str());
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        
        if (key == "NUM_SERVERS") {
            iss >> numServers;
            EV_INFO << "Read number of servers: " << numServers << std::endl;
        }
        else if (key == "NUM_CLIENTS") {
            iss >> numClients;
            EV_INFO << "Read number of clients: " << numClients << std::endl;
        }
        else if (key.find("CLIENT_") == 0 && key.find("_SERVERS") != std::string::npos) {
            // Parse CLIENT_X_SERVERS
            int clientId = std::stoi(key.substr(7, key.find("_SERVERS") - 7));
            std::string serversStr;
            iss >> serversStr;
            
            std::stringstream ss(serversStr);
            std::string serverIdStr;
            while (std::getline(ss, serverIdStr, ',')) {
                clientToServerConnections[clientId].push_back(std::stoi(serverIdStr));
            }
            
            EV_INFO << "Client " << clientId << " connected to servers: ";
            for (int sId : clientToServerConnections[clientId]) {
                EV_INFO << sId << " ";
            }
            EV_INFO << std::endl;
        }
        else if (key.find("CLIENT_") == 0 && key.find("_CLIENTS") != std::string::npos) {
            // Parse CLIENT_X_CLIENTS
            int clientId = std::stoi(key.substr(7, key.find("_CLIENTS") - 7));
            std::string clientsStr;
            iss >> clientsStr;
            
            std::stringstream ss(clientsStr);
            std::string neighborIdStr;
            while (std::getline(ss, neighborIdStr, ',')) {
                clientToClientConnections[clientId].push_back(std::stoi(neighborIdStr));
            }
            
            EV_INFO << "Client " << clientId << " connected to clients: ";
            for (int cId : clientToClientConnections[clientId]) {
                EV_INFO << cId << " ";
            }
            EV_INFO << std::endl;
        }
        else if (key.find("SERVER_") == 0 && key.find("_MALICIOUS") != std::string::npos) {
            // Parse SERVER_X_MALICIOUS
            int serverId = std::stoi(key.substr(7, key.find("_MALICIOUS") - 7));
            int isMalicious;
            iss >> isMalicious;
            serverMalicious[serverId] = (isMalicious == 1);
            
            EV_INFO << "Server " << serverId << " malicious: " << serverMalicious[serverId] << std::endl;
        }
    }
    
    file.close();
}

/**
 * Set up the network based on the topology
 * 
 * This method:
 * 1. Sets network parameters
 * 2. Calculates required gate sizes for all modules
 * 3. Sets up client and server parameters
 * 4. Creates all connections between modules
 */
void NetworkBuilder::setupNetwork() {
    // Get the network module
    cModule *network = getParentModule();
    
    // Set parameters for the network
    network->par("numServers").setIntValue(numServers);
    network->par("numClients").setIntValue(numClients);
    
    // Get server and client modules
    cModule **servers = new cModule*[numServers];
    cModule **clients = new cModule*[numClients];
    
    // Initialize all modules and set parameters first
    for (int i = 0; i < numServers; i++) {
        servers[i] = network->getSubmodule("server", i);
        if (servers[i]) {
            // Set malicious parameter
            servers[i]->par("malicious").setBoolValue(serverMalicious[i]);
        } else {
            EV_ERROR << "Server module " << i << " not found." << std::endl;
        }
    }
    
    // Pre-calculate gate counts for each client
    std::vector<int> clientOutGateCount(numClients);
    std::vector<int> clientInGateCount(numClients);
    
    for (int i = 0; i < numClients; i++) {
        // Count gates needed for server connections
        int serverGateCount = clientToServerConnections[i].size();
        
        // Count gates needed for client connections
        int clientGateCount = clientToClientConnections[i].size();
        
        // Total gates needed for this client
        clientOutGateCount[i] = serverGateCount + clientGateCount;
        clientInGateCount[i] = serverGateCount + clientGateCount; // Same for in-gates
    }
    
    // Pre-calculate gate counts for each server
    std::vector<int> serverInGateCount(numServers, 0);
    std::vector<int> serverOutGateCount(numServers, 0);
    
    for (int i = 0; i < numClients; i++) {
        for (int serverId : clientToServerConnections[i]) {
            if (serverId >= 0 && serverId < numServers) {
                serverInGateCount[serverId]++;
                serverOutGateCount[serverId]++;
            }
        }
    }
    
    // Set up clients with parameters and gate sizes
    for (int i = 0; i < numClients; i++) {
        clients[i] = network->getSubmodule("client", i);
        if (clients[i]) {
            // Set gate sizes first
            if (clientOutGateCount[i] > 0) {
                clients[i]->setGateSize("out", clientOutGateCount[i]);
                EV_INFO << "Set client " << i << " out gate size to " << clientOutGateCount[i] << std::endl;
            }
            
            if (clientInGateCount[i] > 0) {
                clients[i]->setGateSize("in", clientInGateCount[i]);
                EV_INFO << "Set client " << i << " in gate size to " << clientInGateCount[i] << std::endl;
            }
            
            // Set server connections as parameter
            std::stringstream ss;
            bool first = true;
            for (int serverId : clientToServerConnections[i]) {
                if (!first) ss << ",";
                ss << serverId;
                first = false;
            }
            clients[i]->par("connectedServers").setStringValue(ss.str());
            
            // Set client connections as parameter
            ss.str("");
            ss.clear();
            first = true;
            for (int clientId : clientToClientConnections[i]) {
                if (!first) ss << ",";
                ss << clientId;
                first = false;
            }
            clients[i]->par("connectedClients").setStringValue(ss.str());
        } else {
            EV_ERROR << "Client module " << i << " not found." << std::endl;
        }
    }
    
    // Set up server gate sizes
    for (int i = 0; i < numServers; i++) {
        if (servers[i]) {
            if (serverInGateCount[i] > 0) {
                servers[i]->setGateSize("in", serverInGateCount[i]);
                EV_INFO << "Set server " << i << " in gate size to " << serverInGateCount[i] << std::endl;
            }
            
            if (serverOutGateCount[i] > 0) {
                servers[i]->setGateSize("out", serverOutGateCount[i]);
                EV_INFO << "Set server " << i << " out gate size to " << serverOutGateCount[i] << std::endl;
            }
        }
    }
    
    // Now create all connections
    // Track used gate indices for each module
    std::vector<int> clientOutGateIndex(numClients, 0);
    std::vector<int> clientInGateIndex(numClients, 0);
    std::vector<int> serverInGateIndex(numServers, 0);
    std::vector<int> serverOutGateIndex(numServers, 0);
    
    // Create connections between clients and servers
    for (int i = 0; i < numClients; i++) {
        if (!clients[i]) continue;
        
        for (int j = 0; j < clientToServerConnections[i].size(); j++) {
            int serverId = clientToServerConnections[i][j];
            if (serverId < 0 || serverId >= numServers || !servers[serverId]) continue;
            
            // Get next available gates
            cGate *clientOutGate = clients[i]->gate("out", clientOutGateIndex[i]++);
            cGate *serverInGate = servers[serverId]->gate("in", serverInGateIndex[serverId]++);
            cGate *serverOutGate = servers[serverId]->gate("out", serverOutGateIndex[serverId]++);
            cGate *clientInGate = clients[i]->gate("in", clientInGateIndex[i]++);
            
            // Set channel type for client->server connection
            cDatarateChannel *channelToServer = cDatarateChannel::create("channelToServer");
            channelToServer->setDatarate(100000000); // 100 Mbps
            channelToServer->setDelay(0.01); // 10 ms
            
            // Set channel type for server->client connection
            cDatarateChannel *channelToClient = cDatarateChannel::create("channelToClient");
            channelToClient->setDatarate(100000000); // 100 Mbps
            channelToClient->setDelay(0.01); // 10 ms
            
            // Connect gates with separate channels
            clientOutGate->connectTo(serverInGate, channelToServer);
            serverOutGate->connectTo(clientInGate, channelToClient);
            
            EV_INFO << "Created connection between client " << i << " and server " << serverId << std::endl;
        }
        
        // Create connections between clients
        for (int j = 0; j < clientToClientConnections[i].size(); j++) {
            int neighborId = clientToClientConnections[i][j];
            if (neighborId < 0 || neighborId >= numClients || !clients[neighborId]) continue;
            
            // Get next available gates
            cGate *clientOutGate = clients[i]->gate("out", clientOutGateIndex[i]++);
            cGate *neighborInGate = clients[neighborId]->gate("in", clientInGateIndex[neighborId]++);
            
            // Set channel type for client->client connection
            cDatarateChannel *channelToClient = cDatarateChannel::create("channelClientToClient");
            channelToClient->setDatarate(100000000); // 100 Mbps
            channelToClient->setDelay(0.01); // 10 ms
            
            // Connect gates with the channel
            clientOutGate->connectTo(neighborInGate, channelToClient);
            
            EV_INFO << "Created connection from client " << i << " to client " << neighborId << std::endl;
        }
    }
    
    delete[] servers;
    delete[] clients;
} 