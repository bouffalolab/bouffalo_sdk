cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# Define
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

target_sources(app PRIVATE
           ota_tls_config.c
           )

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(wifi_ota_by_http)
