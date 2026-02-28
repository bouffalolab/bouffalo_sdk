cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-D PM_PDS_RC32M_POWER_DOWN=0)

sdk_set_main_file(main.c)

project(pds_mini)

