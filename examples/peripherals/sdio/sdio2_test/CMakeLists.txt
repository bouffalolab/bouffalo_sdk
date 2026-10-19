cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# sdk_add_compile_definitions(-D SDIO2_DEBUG)
sdk_add_compile_definitions(-D SDIO2_MAX_PORT_NUM=4)

sdk_set_main_file(main.c)

project(sdio2_test)
