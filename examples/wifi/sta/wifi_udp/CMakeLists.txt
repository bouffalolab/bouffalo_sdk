cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User

target_sources(app PRIVATE
                   wifi_udp_echo.c)

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(wifi_udp)
