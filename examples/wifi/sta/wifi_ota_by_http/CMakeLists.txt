cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# Define

target_sources(app PRIVATE
           ota_tls_config.c
           )

sdk_add_include_directories(.)

sdk_set_main_file(main.c)
sdk_set_flash_erase(2)

project(wifi_ota_by_http)
