cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# sdk_add_compile_definitions(-DBL_WIFI_LP_FW)
sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_DEFAULT=10)
# sdk_add_compile_definitions(-DLPFW_LOG_EN=0)

sdk_set_main_file(main.c)
sdk_add_include_directories(.)

project(hbn_rtc)
