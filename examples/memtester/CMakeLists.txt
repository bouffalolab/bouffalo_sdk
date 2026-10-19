cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_subdirectory_ifdef(CONFIG_MEMTESTER memtester)

sdk_set_main_file(main.c)

project(memtester)
