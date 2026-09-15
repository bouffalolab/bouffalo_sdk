cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(SHELL_EXEC_THREAD_STACK_SIZE=4096)

sdk_set_main_file(main.c)

sdk_add_flash_partition_image(
    PARTITION media
    FILE ${CMAKE_BINARY_DIR}/build_out/littlefs.bin)

project(mquickjs)
