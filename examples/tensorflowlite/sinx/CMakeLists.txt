cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE constants.cc main_functions.cc model.cc output_handler.cc)

sdk_set_main_file(main.c)

project(tensorflowlite_sinx)
