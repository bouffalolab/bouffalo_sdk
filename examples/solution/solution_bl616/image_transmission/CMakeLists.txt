cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)

# User
sdk_add_compile_definitions(-D CONFIG_CLI_CMD_ENABLE)
#sdk_add_compile_definitions(-D BL616_DHCP_DEBUG)
# sdk_add_compile_definitions(-D CONFIG_IRQ_USE_VECTOR)

add_subdirectory(../solution_utils solution_utils)

sdk_set_main_file(main.c)

project(image_trans)
