// Network definition for Remote Execution simulation
// By B22CS061 & B22CS062

package src;

// Channel definition for connections
channel Channel extends ned.DatarateChannel {
    delay = 10ms;
    datarate = 100Mbps;
}

// Client node definition
simple ClientNode {
    parameters:
        string id;
        int numClients @mutable; // Total number of clients in the network
        int clientId @mutable;   // Numeric ID of this client
        string connectedClients @mutable; // List of client IDs this client is connected to
        string chordFingerTable @mutable; // Chord finger table for efficient routing
        bool enabled = default(true);     // Enable/disable flag
        @display("i=device/pc;p=100,200");
    gates:
        input in[]; // Will be dynamically sized
        output out[]; // Will be dynamically sized
}

// Network builder module
simple NetworkBuilder {
    parameters:
        bool enabled = default(true); // Enable/disable flag for dynamic network building
        @display("i=block/network2");
}

// Network definition
network RemoteExecutionNetwork {
    parameters:
        int numClients @mutable = default(10); // Mark as mutable to allow runtime changes
        @display("bgb=500,500");
    submodules:
        networkBuilder: NetworkBuilder {
            @display("p=50,50");
        }
        client[numClients]: ClientNode {
            id = default("client" + string(index));
            clientId = default(index);
            numClients = parent.numClients; // Use parent.numClients to reference the network parameter
            @display("p=200,300,ring,150,150");
        }
    connections allowunconnected:
        // Ring topology connections (forward)
        client[0].out++ --> Channel --> client[1].in++;
        client[1].out++ --> Channel --> client[2].in++;
        client[2].out++ --> Channel --> client[3].in++;
        client[3].out++ --> Channel --> client[4].in++;
        client[4].out++ --> Channel --> client[5].in++;
        client[5].out++ --> Channel --> client[6].in++;
        client[6].out++ --> Channel --> client[7].in++;
        client[7].out++ --> Channel --> client[8].in++;
        client[8].out++ --> Channel --> client[9].in++;
        client[9].out++ --> Channel --> client[0].in++;
        
        // Ring topology connections (backward)
        client[0].out++ --> Channel --> client[9].in++;
        client[1].out++ --> Channel --> client[0].in++;
        client[2].out++ --> Channel --> client[1].in++;
        client[3].out++ --> Channel --> client[2].in++;
        client[4].out++ --> Channel --> client[3].in++;
        client[5].out++ --> Channel --> client[4].in++;
        client[6].out++ --> Channel --> client[5].in++;
        client[7].out++ --> Channel --> client[6].in++;
        client[8].out++ --> Channel --> client[7].in++;
        client[9].out++ --> Channel --> client[8].in++;
        
        // Chord finger table connections for client[0]
        client[0].out++ --> Channel --> client[2].in++;
        client[0].out++ --> Channel --> client[4].in++;
        client[0].out++ --> Channel --> client[8].in++;
        
        // Chord finger table connections for client[1]
        client[1].out++ --> Channel --> client[3].in++;
        client[1].out++ --> Channel --> client[5].in++;
        client[1].out++ --> Channel --> client[9].in++;
        
        // Chord finger table connections for client[2]
        client[2].out++ --> Channel --> client[4].in++;
        client[2].out++ --> Channel --> client[6].in++;
        client[2].out++ --> Channel --> client[0].in++;
        
        // Chord finger table connections for client[3]
        client[3].out++ --> Channel --> client[5].in++;
        client[3].out++ --> Channel --> client[7].in++;
        client[3].out++ --> Channel --> client[1].in++;
        
        // Chord finger table connections for client[4]
        client[4].out++ --> Channel --> client[6].in++;
        client[4].out++ --> Channel --> client[8].in++;
        client[4].out++ --> Channel --> client[2].in++;
        
        // Chord finger table connections for client[5]
        client[5].out++ --> Channel --> client[7].in++;
        client[5].out++ --> Channel --> client[9].in++;
        client[5].out++ --> Channel --> client[3].in++;
        
        // Chord finger table connections for client[6]
        client[6].out++ --> Channel --> client[8].in++;
        client[6].out++ --> Channel --> client[0].in++;
        client[6].out++ --> Channel --> client[4].in++;
        
        // Chord finger table connections for client[7]
        client[7].out++ --> Channel --> client[9].in++;
        client[7].out++ --> Channel --> client[1].in++;
        client[7].out++ --> Channel --> client[5].in++;
        
        // Chord finger table connections for client[8]
        client[8].out++ --> Channel --> client[0].in++;
        client[8].out++ --> Channel --> client[2].in++;
        client[8].out++ --> Channel --> client[6].in++;
        
        // Chord finger table connections for client[9]
        client[9].out++ --> Channel --> client[1].in++;
        client[9].out++ --> Channel --> client[3].in++;
        client[9].out++ --> Channel --> client[7].in++;
} 