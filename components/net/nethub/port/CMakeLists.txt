# CMakeLists.txt for nethub port components

# Add source files
set(PORT_SOURCES
    nhsdiowifi.c
    port_sdio.c
    port_sta.c
    port_tcpipsta.c
    port_ap.c
    port_tcpipap.c
)

# Create static library
add_library(nethub_port STATIC ${PORT_SOURCES})

# Add include directories
target_include_directories(nethub_port
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Link nethub core library
target_link_libraries(nethub_port nethub)

# Set compile options
target_compile_options(nethub_port PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)