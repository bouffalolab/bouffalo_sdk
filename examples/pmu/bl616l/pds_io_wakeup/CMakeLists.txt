cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DPM_PDS_LDO18IO_POWER_DOWN=1)
sdk_add_compile_definitions(-DPM_PDS_LDO_SOC_LEVEL_DEFAULT=HBN_LDO_SOC_LEVEL_0P750V)
sdk_add_compile_definitions(-DPM_PDS_LDO_SYS_LEVEL_DEFAULT=HBN_LDO_SYS_LEVEL_1P250)

sdk_set_main_file(main.c)

project(pds_io_wakeup)
