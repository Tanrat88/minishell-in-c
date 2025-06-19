#!/bin/bash

# Check if Python3 is installed
if ! command -v python3 &> /dev/null; then
    echo "Python3 is not installed. Installing..."
    sudo apt-get update
    sudo apt-get install -y python3 python3-tk
fi

# Compile the shell if needed
make

# Start the GUI
python3 shell_gui.py 