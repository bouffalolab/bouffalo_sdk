cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User

target_sources(app PRIVATE
                   wifi_http_client.c
                   )

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(wifi_http)
