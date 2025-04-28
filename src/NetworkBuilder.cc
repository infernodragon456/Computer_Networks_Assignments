#include <omnetpp.h>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <unistd.h> // For getcwd
#include <cmath>    // For log2 and pow
#include <algorithm>
// By B22CS061 & B22CS062
using namespace omnetpp;

/**
 * NetworkBuilder module
 * 
 * Responsible for dynamically building the network topology based on configuration file.
 * This module reads the network configuration from a file (topo.txt) and:
 * 1. Creates the specified number of client nodes
 * 2. Sets up ring topology connections between clients
 * 3. Sets up chord finger table connections for efficient routing
 */
class NetworkBuilder : public cSimpleModule {
private:
    bool enabled;               // Whether the network builder is enabled
    std::string topoFileName;  // Path to the topology configuration file
    int numClients;            // Total number of clients in the network
    
    // Maps to store connection information
    std::map<int, std::vector<int>> clientConnections;  // Maps client ID to list of client IDs
    std::map<int, std::vector<int>> chordFingerTables;  // Maps client ID to its chord finger table

protected:
    virtual void initialize() override;     // Initialize the module and build the network
    virtual void handleMessage(cMessage *msg) override;   // Handle incoming messages
    void readTopoFile();                    // Read network topology from configuration file
    void setupNetwork();                    // Set up the network based on the topology
    void generateChordFingerTables();       // Generate Chord finger tables for O(logN) routing
};

Define_Module(NetworkBuilder);

/**
 * Initialize the NetworkBuilder module
 * 
 * This method looks for the configuration file, reads the network topology,
 * and sets up the network connections and properties.
 */
void NetworkBuilder::initialize() {
    // Check if the network builder is enabled
    enabled = par("enabled").boolValue();
    
    // If not enabled, do nothing
    if (!enabled) {
        EV_INFO << "NetworkBuilder is disabled. Skipping dynamic network setup." << std::endl;
        return;
    }
    
    // Try different paths to find the configuration file
    const char* possiblePaths[] = {
        "topo.txt",
        "./topo.txt",
        "../topo.txt",
        "../../topo.txt",
        "../../../topo.txt",
        "src/topo.txt",
        "./src/topo.txt",
        "../src/topo.txt"
    };
    
    bool fileFound = false;
    for (const char* path : possiblePaths) {
        std::ifstream testFile(path);
        if (testFile.is_open()) {
            topoFileName = path;
            testFile.close();
            fileFound = true;
            EV_INFO << "Found topology file at: " << topoFileName << std::endl;
            break;
        }
    }
    
    if (!fileFound) {
        // Output current working directory for debugging
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            EV_INFO << "Current working directory: " << cwd << std::endl;
        }
        EV_INFO << "Searching for topo.txt..." << std::endl;
        
        // Create a default ring topology in memory if file cannot be found
        EV_INFO << "Creating default ring topology in memory..." << std::endl;
        numClients = 10;
        
        // Default client to client connections (ring topology)
        for (int i = 0; i < numClients; i++) {
            // Connect to successor and predecessor in ring
            clientConnections[i].push_back((i + 1) % numClients);  // Successor
            clientConnections[i].push_back((i - 1 + numClients) % numClients);  // Predecessor
        }
        
        // Generate Chord finger tables
        generateChordFingerTables();
    } else {
        // Read topology file
        readTopoFile();
    }
    
    // Setup network
    setupNetwork();
}

/**
 * Generate Chord finger tables for each client
 * 
 * For each client i, the finger table contains log(N) entries
 * finger[i][k] = (i + 2^k) mod N, for 0 ≤ k < log(N)
 */
void NetworkBuilder::generateChordFingerTables() {
    int m = static_cast<int>(ceil(log2(numClients))); // Number of bits needed to represent numClients
    
    for (int i = 0; i < numClients; i++) {
        for (int k = 0; k < m; k++) {
            int fingerNode = (i + static_cast<int>(pow(2, k))) % numClients;
            chordFingerTables[i].push_back(fingerNode);
            
            // Add this connection to the client connections if not already present
            if (std::find(clientConnections[i].begin(), clientConnections[i].end(), fingerNode) == clientConnections[i].end()) {
                clientConnections[i].push_back(fingerNode);
            }
        }
    }
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
 * - Number of clients
 * - Client-client connections
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
        
        if (key == "NUM_CLIENTS") {
            iss >> numClients;
            EV_INFO << "Read number of clients: " << numClients << std::endl;
        }
        else if (key.find("CLIENT_") == 0 && key.find("_CLIENTS") != std::string::npos) {
            // Parse CLIENT_X_CLIENTS
            int clientId = std::stoi(key.substr(7, key.find("_CLIENTS") - 7));
            std::string clientsStr;
            iss >> clientsStr;
            
            std::stringstream ss(clientsStr);
            std::string neighborIdStr;
            while (std::getline(ss, neighborIdStr, ',')) {
                clientConnections[clientId].push_back(std::stoi(neighborIdStr));
            }
            
            EV_INFO << "Client " << clientId << " connected to clients: ";
            for (int cId : clientConnections[clientId]) {
                EV_INFO << cId << " ";
            }
            EV_INFO << std::endl;
        }
    }
    
    file.close();
    
    // If no connections were specified in the file, create a default ring topology
    if (clientConnections.empty()) {
        for (int i = 0; i < numClients; i++) {
            // Connect to successor and predecessor in ring
            clientConnections[i].push_back((i + 1) % numClients);  // Successor
            clientConnections[i].push_back((i - 1 + numClients) % numClients);  // Predecessor
        }
    }
    
    // Generate Chord finger tables
    generateChordFingerTables();
}

/**
 * Set up the network based on the topology
 * 
 * This method:
 * 1. Sets network parameters
 * 2. Calculates required gate sizes for all modules
 * 3. Sets up client parameters
 * 4. Creates all connections between modules
 */
void NetworkBuilder::setupNetwork() {
    // Get the network module
    cModule *network = getParentModule();
    
    // Set parameters for the network
    network->par("numClients").setIntValue(numClients);
    
    // Get client modules
    cModule **clients = new cModule*[numClients];
    
    // Pre-calculate gate counts for each client
    std::vector<int> clientOutGateCount(numClients);
    std::vector<int> clientInGateCount(numClients);
    
    for (int i = 0; i < numClients; i++) {
        // Count gates needed for client connections
        int clientGateCount = clientConnections[i].size();
        
        // Total gates needed for this client
        clientOutGateCount[i] = clientGateCount;
        clientInGateCount[i] = clientGateCount; // Same for in-gates
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
            
            // Set client connections as parameter
            std::stringstream ss;
            bool first = true;
            for (int clientId : clientConnections[i]) {
                if (!first) ss << ",";
                ss << clientId;
                first = false;
            }
            clients[i]->par("connectedClients").setStringValue(ss.str());
            
            // Set chord finger table as parameter
            ss.str("");
            ss.clear();
            first = true;
            for (int fingerId : chordFingerTables[i]) {
                if (!first) ss << ",";
                ss << fingerId;
                first = false;
            }
            clients[i]->par("chordFingerTable").setStringValue(ss.str());
        } else {
            EV_ERROR << "Client module " << i << " not found." << std::endl;
        }
    }
    
    // Now create all connections
    // Track used gate indices for each module
    std::vector<int> clientOutGateIndex(numClients, 0);
    std::vector<int> clientInGateIndex(numClients, 0);
    
    // Create connections between clients
    for (int i = 0; i < numClients; i++) {
        if (!clients[i]) continue;
        
        for (int j = 0; j < clientConnections[i].size(); j++) {
            int neighborId = clientConnections[i][j];
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
    
    delete[] clients;
} 