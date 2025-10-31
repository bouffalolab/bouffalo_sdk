cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DPM_PDS_LDO18IO_POWER_DOWN=1)

sdk_set_main_file(main.c)

project(pds_timer)
