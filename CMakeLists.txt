cmake_minimum_required(VERSION 3.10)
project(asdKeypad_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Try Qt6 first, fall back to Qt5 if Qt6 is not available
find_package(Qt6 COMPONENTS Core Gui Widgets SerialPort QUIET)
if (Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
else()
    set(QT_MINIMUM_VERSION "5.15.0")
    find_package(Qt5 ${QT_MINIMUM_VERSION} COMPONENTS Core Gui Widgets SerialPort REQUIRED)
    set(QT_VERSION_MAJOR 5)
endif()

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
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
)