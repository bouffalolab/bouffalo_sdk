cmake_minimum_required(VERSION 3.15)

include("$ENV{BL_SDK_BASE}/cmake/auto_config.cmake")

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE ../format.c)
sdk_add_include_directories(..)

sdk_set_main_file(main.c)

project(mjdec_pingpang)
