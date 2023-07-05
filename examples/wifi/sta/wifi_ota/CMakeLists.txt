cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# Define
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

target_sources(app PRIVATE
                   utils_sha256.c
                   partition.c
                   wifi_ota_test.c
                   ${CMAKE_SOURCE_DIR}/port/${CHIP}/bflb_port_boot2.c
                   )

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/port/${CHIP})
include_directories(${CMAKE_SOURCE_DIR}/port/${CHIP})
else()
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/port/${CHIP} is not exist")
endif()
sdk_add_include_directories(.)

if((${CHIP} STREQUAL "bl616"))
    sdk_add_compile_definitions(-DCHIP_BL616)
endif()

sdk_set_main_file(main.c)

project(wifi_ota)
