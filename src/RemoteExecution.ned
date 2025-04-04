// Network definition for Remote Execution simulation
// By B22CS061 & B22CS062

package src;

// Channel definition for connections
channel Channel extends ned.DatarateChannel {
    delay = 10ms;
    datarate = 100Mbps;
}

// Server node definition
simple ServerNode {
    parameters:
        string id;
        bool malicious @mutable = default(false); // Mark as mutable to allow runtime changes
        @display("i=device/server;p=100,100");
    gates:
        input in[]; // Will be dynamically sized
        output out[]; // Will be dynamically sized
}

// Client node definition
simple ClientNode {
    parameters:
        string id;
        int numServers @mutable; // Total number of servers in the network
        string connectedServers @mutable; // List of server IDs this client is connected to
        string connectedClients @mutable; // List of client IDs this client is connected to
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
        int numServers @mutable = default(5); // Mark as mutable to allow runtime changes
        int numClients @mutable = default(3); // Mark as mutable to allow runtime changes
        @display("bgb=500,500");
    submodules:
        networkBuilder: NetworkBuilder {
            @display("p=50,50");
        }
        server[numServers]: ServerNode {
            id = default("server" + string(index));
            @display("p=200,100,m,10,20");
        }
        client[numClients]: ClientNode {
            id = default("client" + string(index));
            numServers = parent.numServers; // Use parent.numServers to reference the network parameter
            @display("p=200,300,m,10,20");
        }
    connections allowunconnected:
        // Connections will be established dynamically in the NetworkBuilder
} 