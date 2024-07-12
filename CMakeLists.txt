cmake_minimum_required(VERSION 3.10)  # Minimum CMake version

project(MBHS)  # Project name

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build an executable from main.cpp and server.cpp
add_executable(MBHS 
    src/main.cpp
    src/server.cpp) 