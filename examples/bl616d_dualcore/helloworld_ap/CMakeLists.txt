cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DCONFIG_BOOT_NP_ASYNC)

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(helloworld)
