#!/bin/bash

# Script to compile and run the Remote Execution simulation
# By B22CS061 & B22CS062
# Create results directory if it doesn't exist
mkdir -p results

# Generate message files
echo "Generating message files..."
opp_msgc src/RemoteExecution.msg

# Compile the project
echo "Compiling project..."
make

# Run the simulation
echo "Running simulation..."
./remoteexecution

echo "Simulation complete. Results are in the 'results' directory." 