cmake_minimum_required(VERSION 3.15)

find_package(
    bouffalo_sdk
    REQUIRED
    HINTS
    $ENV{BL_SDK_BASE}
)

sdk_add_include_directories(.)
sdk_add_include_directories(config_file)

# User 
# sdk_add_compile_definitions(-D CONFIG_CLI_CMD_ENABLE)
# sdk_add_compile_definitions(-DBL616_DHCP_DEBUG)
# sdk_add_compile_definitions(-DCONFIG_IRQ_USE_VECTOR)

add_subdirectory(../solution_utils solution_utils)

if(CONFIG_WIFI6)
    target_sources(app PRIVATE wifi_start.c)
endif()

sdk_set_main_file(main.c)

project(image_transmission)
