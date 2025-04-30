cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)

sdk_add_compile_definitions(-D${CONFIG_ETHERNET_PHY})
sdk_add_compile_definitions(-DEMAC_RX_DATA_ZERO_COPY)


target_sources(app PRIVATE cdc_ecm_template.c)
sdk_set_main_file(main.c)

project(usbd_cdc_ecm)
