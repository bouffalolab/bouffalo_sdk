cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

#sdk_add_compile_definitions(-DCONFIG_BOARD_FLASH_INIT_SKIP)
#sdk_add_compile_definitions(-DPM_HBN_FLASH_POWER_OFF=0)

sdk_set_main_file(main.c)

project(hbn_io_wakeup)
