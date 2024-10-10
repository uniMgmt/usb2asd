#!/bin/bash

# Use the existing DISPLAY variable
echo "Using DISPLAY=$DISPLAY"

echo "Starting asdKeypad_cpp..."
./build/asdKeypad_cpp

echo "asdKeypad_cpp exited."