cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/demos/*.c")

sdk_add_include_directories(.)
target_sources(app PRIVATE ${sources})
sdk_set_main_file(main.c)
sdk_set_linker_script_macro(bl616d_common_lvgl.ld)

# Add DPI manager source files
target_sources(app PRIVATE dpi_manager.c)

# Add filesystem reader source files
target_sources(app PRIVATE filesystem_reader.c)

project(lvgl8)
