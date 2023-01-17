cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_set_main_file(main.c)

target_sources(app PRIVATE mqueues_test.c semaphores_test.c)
sdk_add_include_directories(.)

project(posix)
