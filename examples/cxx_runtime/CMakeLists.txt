cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# FreeRTOSConfig.h lives next to main.cpp.
sdk_add_include_directories(.)

sdk_set_main_file(main.cpp)

project(cxx_runtime)
