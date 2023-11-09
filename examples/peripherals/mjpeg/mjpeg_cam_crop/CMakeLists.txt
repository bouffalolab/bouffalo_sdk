cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE jpeg_head.c)
sdk_set_main_file(main.c)

project(mjpeg_cam_crop)
