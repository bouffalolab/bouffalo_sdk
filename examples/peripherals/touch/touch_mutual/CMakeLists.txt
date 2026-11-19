cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

if(NOT "${CHIP}" STREQUAL "bl618dg" OR NOT "${CPU_ID}" STREQUAL "ap")
    message(FATAL_ERROR "This touch example supports BL618DG AP only")
endif()

target_sources(app PRIVATE ../touch_common.c)
sdk_add_include_directories(..)
sdk_set_main_file(main.c)

project(touch_mutual)
