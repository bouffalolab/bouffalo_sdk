cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_DEFAULT=10)

sdk_set_main_file(main.c)

project(pds_io_wakeup)
