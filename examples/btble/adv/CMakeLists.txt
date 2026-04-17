cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)

# No additional source files needed for simple beacon
# All functionality is in main.c

sdk_set_main_file(main.c)

project(adv)
