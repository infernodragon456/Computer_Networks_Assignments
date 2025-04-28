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
        @display("i=device/pc;p=100,200");
    gates:
        input in[]; // Will be dynamically sized
        output out[]; // Will be dynamically sized
}

// Network builder module
simple NetworkBuilder {
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
        // Connections will be established dynamically in the NetworkBuilder
} 