# asdKeypad C++ Port

This project is a C++ port of the asdKeypad VB.NET application, which simulates a vending machine keypad and communicates with a USART Comm-port from a VCP FTDI chip-set.

## VB.NET Program Overview
The original VB.NET application for the asdKeypad program was developed to simulate a vending machine keypad, sending keypress commands through a serial port (using an FTDI chip to create a virtual comm port). The keypresses were represented by specific byte sequences sent over the serial connection.

The application supported functionalities such as simulating specific keypresses (1 through 9, 0, *, and #), handling errors, and logging actions. It also included a basic GUI with a keypad, port connection options, and status indicators. The main functions of the VB.NET program were as follows:

	•	Simulated keypress functionality, sending specific byte commands to a VMC (Vending Machine Controller).
	•	Ability to clear errors and adjust settings via the interface.
	•	Logging and error handling.

## C++ Port Overview

This C++ port replicates the core functionality of the original VB.NET program using the Qt framework for the GUI and serial communication. The main features that have been implemented so far include:

	•	Basic UI: The keypad UI has been recreated using Qt, including buttons for digits 0-9, *, and #.
	•	Serial Communication: The application interacts with the serial port using Qt’s serial port module, allowing it to send and receive data from the VMC.
	•	Mock Serial Communication: A mock serial communication module has been implemented to simulate sending and receiving data without hardware, allowing easier testing and development.
	•	Keypress Commands: The keypress functionality has been ported, sending specific byte sequences when the keypad buttons are pressed.
	•	Auto Keypress Feature: This newly implemented feature simulates repeated keypresses automatically over a timed interval.
	•	Logging: Logs actions and errors in the application for easy debugging and feedback.

## Project Structure

asdKeypad_cpp/
├── src/
│   ├── AutoKeypress.cpp
│   ├── AutoKeypress.h
│   ├── Colors.h
│   ├── KeypressCommands.cpp
│   ├── KeypressCommands.h
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   ├── MockSerialCommunication.cpp
│   ├── MockSerialCommunication.h
│   ├── SerialCommunication.cpp
│   ├── SerialCommunication.h
│   └── main.cpp
├── CMakeLists.txt
└── README.md

## Dependencies

	•	Qt 5 or 6 (for GUI and serial port communication)
	•	CMake (for build system)

## Build Instructions

	1.	Install Qt and CMake
	2.	Create a build directory: mkdir build && cd build
	3.	Run CMake: cmake ..
	4.	Build the project: make or cmake --build .

## Current Features

	•	Basic MainWindow implementation with keypad UI
	•	Serial communication setup and mock implementation
	•	Auto Keypress feature for repeated keypress simulation
	•	*Keypress command functions ported (1-9, 0, , #)
	•	Logging functionality for actions and errors
	•	Basic menu with options for clearing logs and setting the default port

## Next Steps and TODOs

	1.	Improve Serial Communication Error Handling: Enhance the user feedback when errors occur during serial communication, such as failures to connect to the port or send data.
	2.	About Dialog: Create an About dialog, replicating the functionality of the original VB.NET program, which provides information about the application.
	3.	Price Setting Functionality: Port the price-setting feature from the VB.NET version, where specific sequences of keypresses are sent to configure the vending machine prices.
	4.	Visual Indicator for Serial Connection: Implement a visual indicator in the UI to show the status of the serial connection (connected/disconnected).
	5.	Test with Actual Hardware: Once all features are implemented, test the program with actual hardware (FTDI chipset and VMC) to ensure correct functionality.
	6.	Finalize Documentation: Once all features are complete and tested, update this README with detailed usage instructions and any additional setup steps required.

## Usage

Currently, the application can be run using the provided run_with_xvfb.sh script:

./run_with_xvfb.sh

