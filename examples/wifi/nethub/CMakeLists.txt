cmake_minimum_required(VERSION 3.15)

#include("$ENV{BL_SDK_BASE}/cmake/auto_config.cmake")

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_link_options(-L${CMAKE_CURRENT_SOURCE_DIR})

sdk_add_compile_definitions(-DTICKLESS_DEBUG=4)
sdk_add_compile_definitions(-DPM_PDS_LDO18IO_POWER_DOWN)
sdk_add_compile_definitions(-DCONFIG_RWNX_LWIP)
sdk_add_compile_definitions(-DconfigEXPECTED_IDLE_TIME_BEFORE_SLEEP=40)
sdk_add_compile_definitions(-DWPA_SUPPLICANT_CLEANUP_INTERVAL=1800)
sdk_add_compile_definitions(-DBL_RA_EN=1)
sdk_add_compile_definitions(-DPRINTF_EN)

target_sources(app PRIVATE
                   app/src/app_user.c
                   )

if(CONFIG_NETHUB_PROFILE_USB)
target_sources(app PRIVATE
                   app/src/app_usb_composite.c
                   )
endif()

# Virtual channel demo (only compile if CONFIG_MR_VIRTUALCHAN is defined)
if(CONFIG_MR_VIRTUALCHAN)
target_sources(app PRIVATE app/src/virtualchan/app_vchan.c)
endif()


if (CONFIG_WL80211)
    target_sources(app PRIVATE app/src/wifi/app_wifi_wl80211.c)
else ()
    target_sources(app PRIVATE app/src/wifi/app_wifi_fhost.c)
endif()

if (CONFIG_LPAPP)
    target_sources(app PRIVATE app/src/wifi/app_wifi_cli.c)
endif()

sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

sdk_set_main_file(main.c)
sdk_set_flash_erase(2)

project(nethub)
