cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# Define
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

target_sources(app PRIVATE
                   wifi_ota_test.c
                   )

sdk_add_include_directories(.)

if((${CHIP} STREQUAL "bl616"))
    sdk_add_compile_definitions(-DCHIP_BL616)
endif()

sdk_set_main_file(main.c)

project(wifi_ota)
