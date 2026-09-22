cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

target_sources(app PRIVATE
                   wifi_p2p.c
                   wifi_tcp_client.c
                   wifi_tcp_server.c)

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(wifi_p2p)
