
cmake_minimum_required(VERSION 3.15)

# Project metadata
project(SmartParkingSystem
    VERSION 1.0.0
    DESCRIPTION "Automated Smart Parking Management System (DSA Task 1)"
    LANGUAGES CXX
)

# Enforce modern C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the executable target
add_executable(parking_system main.cpp)

# Enable compiler warnings
if(MSVC)
    target_compile_options(parking_system PRIVATE /W4 /WX)
else()
    target_compile_options(parking_system PRIVATE -Wall -Wextra -Wpedantic)
endif()