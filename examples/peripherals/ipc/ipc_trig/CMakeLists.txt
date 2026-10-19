cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

add_definitions(-DCONFIG_BOARD_FLASH_INIT_SKIP)

sdk_set_main_file(main.c)

project(ipc_trig)
