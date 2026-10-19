cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)
sdk_add_include_directories(config_file)

add_subdirectory(tcp)
add_subdirectory(udp)
add_subdirectory(http_server)

sdk_set_main_file(main.c)

project(lwip_emac)
