cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE gc9307_dbi.c)
sdk_set_main_file(main.c)

project(cam_dbi)
