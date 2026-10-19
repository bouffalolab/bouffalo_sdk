cmake_minimum_required(VERSION 3.15)

include("$ENV{BL_SDK_BASE}/cmake/auto_config.cmake")

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DPM_PDS_RC32M_POWER_DOWN=0)

sdk_set_main_file(main.c)

project(pds_lp_wakeup)
