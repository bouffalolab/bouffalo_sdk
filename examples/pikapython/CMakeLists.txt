cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/pikapython/*.c")

target_sources(app PRIVATE ${sources})

sdk_add_include_directories(.)
sdk_add_include_directories(pikapython/pikascript-core)
sdk_add_include_directories(pikapython/pikascript-api)

sdk_set_main_file(main.c)

set(APP_LDFLAGS_HEAD "-Wl,--whole-archive")
set(APP_LDFLAGS_TAIL "-Wl,--no-whole-archive")

project(pikapython)
