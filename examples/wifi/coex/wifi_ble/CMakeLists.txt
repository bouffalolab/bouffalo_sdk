cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

if(CONFIG_BLUETOOTH)
target_sources(app PRIVATE
                   btble_cli_port.c
                   btble_test_port.c)
endif()

sdk_add_include_directories(.)

sdk_set_main_file(main.c)
sdk_set_flash_erase(2)

project(wifi_ble)
