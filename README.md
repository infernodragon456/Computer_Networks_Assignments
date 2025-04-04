# Remote Execution Simulation with OMNeT++

By B22CS061 & B22CS062

This project simulates a distributed computing scenario where clients divide tasks into subtasks and send them to multiple servers. The system is designed to handle potentially malicious servers.

## Overview

The simulation models a scenario where:
1. Client nodes divide a task (finding maximum in array) into n subtasks
2. Each subtask is sent to n/2+1 servers
3. Servers process the subtasks (some may be malicious)
4. Clients collect results, determine correct results by majority voting
5. Clients rate servers and share ratings with other clients via gossip protocol
6. In the second round, clients select top-rated servers based on these ratings

## Project Structure

- `src/` - Source code directory
  - `RemoteExecution.ned` - Network definition
  - `RemoteExecution.msg` - Message definitions
  - `ServerNode.cc` - Server implementation
  - `ClientNode.cc` - Client implementation
  - `NetworkBuilder.cc` - Dynamically builds the network based on topology
- `config.txt` - Network topology configuration
- `omnetpp.ini` - Simulation configuration
- `outputfile.txt` - Output file for simulation results

## Prerequisites

- OMNeT++ 6.1 or later
- C++ compiler compatible with C++11 or later
- Make or CMake build system

## Detailed Build and Run Instructions

### 1. Setup Project Directory

```bash
# Create directories if they don't exist
mkdir -p build
```

### 2. Generate Message Classes

First, generate the necessary message classes from the message definitions:

```bash
cd src
opp_msgc RemoteExecution.msg
cd ..
```

### 3. Build the Project

#### Using Make

```bash
# Option 1: Using makemake and make
cd src
opp_makemake -f --deep
make
cd ..
```

#### Using CMake

```bash
# Option 2: Using CMake
cd build
cmake ..
make
cd ..
```

#### Using OMNeT++ IDE

If you're using the OMNeT++ IDE:
1. Launch OMNeT++ IDE
2. Go to File > Import > Existing Projects into Workspace
3. Select the root directory of this project
4. Click "Finish"
5. Right-click on the project in Project Explorer and select "Build Project"

### 4. Configure the Network

Edit the `config.txt` file to define your network topology:

```
NUM_SERVERS 5
NUM_CLIENTS 3

CLIENT_0_SERVERS 0,1,2,3,4
CLIENT_1_SERVERS 0,1,2,3,4
CLIENT_2_SERVERS 0,1,2,3,4

CLIENT_0_CLIENTS 1,2
CLIENT_1_CLIENTS 0,2
CLIENT_2_CLIENTS 0,1

SERVER_0_MALICIOUS 0
SERVER_1_MALICIOUS 0
SERVER_2_MALICIOUS 0
SERVER_3_MALICIOUS 1
SERVER_4_MALICIOUS 0
```

This configuration:
- Defines 5 servers and 3 clients
- Connects each client to all servers
- Creates a fully connected client network
- Sets server 3 as malicious

### 5. Run the Simulation

#### Command Line Execution

```bash
# Run from build directory
cd build
./RemoteExecution
```

Or using the OMNeT++ runtime:

```bash
# Run from project root
opp_run -m ./build/RemoteExecution
```

#### Using OMNeT++ IDE

1. Right-click on the project in Project Explorer
2. Select "Run As" > "OMNeT++ Simulation"
3. Select the `omnetpp.ini` configuration file
4. Click "Run"

### 6. View Results

The simulation results are written to:
- Console output
- `outputfile.txt` in the project root directory

## Configuration Details

### Network Topology (`config.txt`)

- `NUM_SERVERS`: Number of server nodes
- `NUM_CLIENTS`: Number of client nodes
- `CLIENT_X_SERVERS`: List of server IDs that client X connects to
- `CLIENT_X_CLIENTS`: List of client IDs that client X connects to
- `SERVER_X_MALICIOUS`: Whether server X is malicious (0 for honest, 1 for malicious)

### Simulation Settings (`omnetpp.ini`)

The `omnetpp.ini` file contains simulation parameters such as:
- Simulation duration
- Random number generator seeds
- Network builder settings

## Troubleshooting

### Common Issues

1. **Missing config.txt file**:
   - The simulation will create a default topology if the file is not found.
   - Ensure the file is in the correct location (project root or src directory).

2. **Gate Size Issues**:
   - If you encounter gate size errors, check your topology configuration.
   - Ensure you don't have conflicting connections.

3. **Build Errors**:
   - Make sure all dependencies are installed.
   - Ensure message classes are generated correctly.
   - Check for C++ syntax errors in the code. 