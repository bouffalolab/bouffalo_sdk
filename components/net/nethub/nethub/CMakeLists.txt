cmake_minimum_required(VERSION 3.15)

project(nethub)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)
include_directories(${CMAKE_SOURCE_DIR})

# Add compile definitions to enable configuration files
add_definitions(-DHAVE_CONFIG_H)

# Source files
file(GLOB_RECURSE SOURCES "src/*.c")

# Create static library
add_library(nethub STATIC ${SOURCES})

# Examples
add_subdirectory(examples)

# Tests
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Install header files
install(DIRECTORY include/ DESTINATION include)