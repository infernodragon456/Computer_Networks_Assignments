# P2P Remote Execution Simulation

This simulation implements a peer-to-peer (P2P) distributed remote execution system using OMNeT++. The system allows clients to distribute computational tasks across a network of nodes in a ring topology, with optimized message routing for efficient communication.

## Overview

The simulation creates a network of N client nodes arranged in a ring topology. Each client has a unique ID starting from 0. When a client needs to execute a task (finding the maximum element in an array), it divides the task into x subtasks, where x > N and each subtask has an ID i. Each subtask with ID i is sent to the client with ID i%N.

The network uses Chord-like finger tables to achieve O(log N) message routing complexity, which is an optimization over the basic ring topology that would require O(N) message exchanges in the worst case.

## Network Topology

The network topology is defined in `topo.txt` and includes:
- Number of client nodes
- Client-to-client connections (ring topology)
- Chord finger tables for efficient routing

The NetworkBuilder module dynamically creates the network based on this configuration, establishing all necessary connections and generating the Chord finger tables.

## Task Execution Process

1. A client initiates a task by generating a random array
2. The array is divided into x subtasks, each containing at least 2 elements
3. Each subtask is assigned to a client based on the formula: clientID = subtaskID % numClients
4. Subtasks are routed through the network using Chord finger tables
5. Each client processes its assigned subtasks (finding the maximum element)
6. Results are sent back to the initiating client
7. The initiating client consolidates the results to find the final answer

## Gossip Protocol

After completing a task, clients participate in a gossip protocol to share information:

1. A client generates a gossip message in the format: <timestamp>:<clientID>:<clientNumericID>
2. The message is sent to all directly connected clients
3. Upon receiving a new gossip message, a client forwards it to all other connected clients
4. Once a client has received gossip messages from all N clients, it terminates

## Building and Running

To build and run the simulation:

1. Make sure you have OMNeT++ installed
2. Clone this repository
3. Run the provided script:
   ```
   ./run.sh
   ```

This will compile the simulation, run it, and display the results.

## Configuration

You can modify the network by editing `topo.txt`:
- Change the number of clients (NUM_CLIENTS)
- Define custom connections between clients

The simulation parameters can be modified in `omnetpp.ini`.

## Implementation Details

- **NetworkBuilder.cc**: Creates the network topology, including Chord finger tables
- **ClientNode.cc**: Implements client behavior, task execution, routing, and gossip protocol
- **RemoteExecution.ned**: Network definition
- **RemoteExecution.msg**: Message type definitions

## Routing Algorithm

The simulation uses a Chord-like routing algorithm that achieves O(log N) message complexity:

1. Each node maintains a finger table with log(N) entries
2. The finger[i][k] entry points to node (i + 2^k) mod N
3. When routing to a destination, a node forwards to the finger that most closely precedes the destination
4. This allows messages to reach any destination in at most O(log N) hops 