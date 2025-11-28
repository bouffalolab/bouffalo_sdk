cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

sdk_add_subdirectory_ifdef(CONFIG_BFLB_BLESYNC bflb_blesync)

project(smartconfig_ble)
