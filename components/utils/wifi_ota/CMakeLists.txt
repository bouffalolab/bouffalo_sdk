# Add common include directories (used by both RPC and wifi_ota)
sdk_add_include_directories(../partition)

# Add partition component (required by flash operations and wifi_ota)
sdk_library_add_sources(../partition/partition.c)

# Add wifi_ota component when FHOST is enabled (all WiFi demos)
if(CONFIG_FHOST)
    # Add source files to utils libraries
    sdk_library_add_sources(src/wifi_ota.c)
    sdk_library_add_sources(src/wifi_ota_shell.c)

    # Add include directories
    sdk_add_include_directories(include)
    sdk_add_include_directories(src)

    # Set compile definitions for this component
    sdk_add_compile_definitions(-DWIFI_OTA_BUFFER_SIZE=512)
endif()
