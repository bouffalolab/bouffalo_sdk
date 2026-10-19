cmake_minimum_required(VERSION 3.15)

include("$ENV{BL_SDK_BASE}/cmake/auto_config.cmake")

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DCONFIG_BOOT_NP_ASYNC)

sdk_set_main_file(main.c)

project(ipc_trig)
