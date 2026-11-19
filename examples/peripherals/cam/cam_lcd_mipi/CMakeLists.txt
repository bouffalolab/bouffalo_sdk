cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

if(NOT "${CHIP}" STREQUAL "bl618dg" OR NOT "${CPU_ID}" STREQUAL "ap")
	message(FATAL_ERROR "cam_lcd_mipi requires CHIP=bl618dg CPU_ID=ap")
endif()

sdk_add_include_directories(.)
sdk_set_main_file(main.c)

project(cam_lcd_mipi)
