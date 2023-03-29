cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)
# add_subdirectory(${BL_SDK_BASE}/components/usb/usbh_uvc_uac usbh_uvc_uac)
# sdk_add_static_library(libusbh_uvc_uac.a)

target_sources(app PRIVATE usb_host.c)
sdk_set_main_file(main.c)

project(usbhost)
