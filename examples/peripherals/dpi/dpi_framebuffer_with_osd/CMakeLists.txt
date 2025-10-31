cmake_minimum_required(VERSION 3.15)

include("$ENV{BL_SDK_BASE}/cmake/auto_config.cmake")

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE picture_moon.c)
target_sources(app PRIVATE balloon_palette_8_bit.c)

sdk_set_main_file(main.c)

project(dpi_framebuffer_with_osd)