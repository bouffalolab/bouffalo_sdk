cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/pikapython/*.c")
file(GLOB_RECURSE sources_user "${CMAKE_CURRENT_SOURCE_DIR}/*user.c")

target_sources(app PRIVATE ${sources} ${sources_user})

sdk_add_include_directories(.)
sdk_add_include_directories(pikapython/pikascript-core)
sdk_add_include_directories(pikapython/pikascript-api)

sdk_add_compile_definitions(
    PIKA_CONFIG_ENABLE=1 
    # LWIP_DNS=1 
    LV_LVGL_H_INCLUDE_SIMPLE=1
    PIKASCRIPT=1
)

sdk_set_main_file(main.c)

project(pikapython)
