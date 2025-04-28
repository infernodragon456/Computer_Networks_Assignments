#!/bin/bash
# Script to build and run the OMNeT++ simulation

# Clear any previous output
rm -f outputfile.txt

# Clean the build directory
make clean

# Build the simulation
make

# Check if build was successful
if [ $? -ne 0 ]; then
    echo "Build failed. Please check for errors."
    exit 1
fi

# Run the simulation
./src/RemoteExecution -c General -u Cmdenv

# Display results
echo "Simulation completed. Results saved to outputfile.txt"
echo "Contents of outputfile.txt:"
cat outputfile.txt 