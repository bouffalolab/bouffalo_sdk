cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

if (CONFIG_USBH_MSC)
target_sources(app PRIVATE
    usbh_msc_fatfs.c
              )
endif()

target_sources(app PRIVATE
    wl_ble_lp_app.c
              )

sdk_add_compile_definitions(-DCFG_BL_WIFI_PS_ENABLE)
sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(ble_lp)
