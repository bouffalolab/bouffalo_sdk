cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)
file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/kv_test/*.c")
target_sources(app PRIVATE ${sources})
sdk_add_link_options(-ucmd_kv_test)

sdk_set_main_file(main.c)

project(littlefs)
