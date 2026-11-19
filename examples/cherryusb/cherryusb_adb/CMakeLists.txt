cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(./config_file)
sdk_add_include_directories(.)
sdk_set_main_file(main.c)

if(CONFIG_CHERRYUSB_DEVICE_ADB)
    target_sources(app PRIVATE ./adb_demo.c)
    target_sources(app PRIVATE ./adb_shell.c)
else()
    message(WARNING "CONFIG_CHERRYUSB_DEVICE_ADB is disabled, the ADB demo cannot link")
endif()

project(cherryusb_adb)
