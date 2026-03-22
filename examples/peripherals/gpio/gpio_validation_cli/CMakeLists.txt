cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DSHELL_CMD_SIZE=512)

sdk_generate_library()
sdk_library_add_sources(pca9698.c)
sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(gpio_validation_cli)
