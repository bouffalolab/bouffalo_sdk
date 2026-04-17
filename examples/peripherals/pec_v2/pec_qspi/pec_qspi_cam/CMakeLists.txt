cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE sensor/common.c)

sdk_set_main_file(main.c)

project(pec_qspi_cam)
