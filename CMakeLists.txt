cmake_minimum_required(VERSION 3.10)
project(asdKeypad_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_MINIMUM_VERSION "5.15.0")
find_package(Qt5 ${QT_MINIMUM_VERSION} COMPONENTS Core Gui Widgets SerialPort REQUIRED)

add_executable(asdKeypad_cpp
    src/main.cpp
    src/MainWindow.cpp
    src/SerialCommunication.cpp
    src/KeypressCommands.cpp
    src/MockSerialCommunication.cpp
    src/AutoKeypress.cpp
    src/Colors.h
    src/SetPriceDialog.cpp
)

target_link_libraries(asdKeypad_cpp
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
)