# SDIO WiFi application example - simplified version
cmake_minimum_required(VERSION 3.15)
project(sdiowifi_demo)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Add nethub header file paths
include_directories(${CMAKE_SOURCE_DIR}/../../include)

# Add forwarding rule configuration header file paths
include_directories(${CMAKE_SOURCE_DIR}/../../forwardrules)

# Collect source files
set(NETHUB_SOURCES
    ../../src/nethub.c
    ../../src/nh_forward.c
    ../../src/nhif_manager.c
)

set(SDIOWIFI_SOURCES
    ../../forwardrules/sdiowifi_rules.c
    main.c
)

# Create executable file
add_executable(sdiowifi_demo
    ${NETHUB_SOURCES}
    ${SDIOWIFI_SOURCES}
)

# Compilation options
target_compile_options(sdiowifi_demo PRIVATE -Wall -Wextra)