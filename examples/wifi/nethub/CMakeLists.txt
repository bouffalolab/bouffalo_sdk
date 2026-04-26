cmake_minimum_required(VERSION 3.15)

#include("$ENV{BL_SDK_BASE}/cmake/auto_config.cmake")

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DTICKLESS_DEBUG=4)
sdk_add_compile_definitions(-DPM_PDS_LDO18IO_POWER_DOWN)
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
sdk_add_compile_definitions(-DconfigUSE_TICKLESS_IDLE=1)
sdk_add_compile_definitions(-Dconfig_CUSTOM_TICKLESS=1)
sdk_add_compile_definitions(-DCFG_BL_WIFI_PS_ENABLE)
sdk_add_compile_definitions(-DCONFIG_RWNX_LWIP)
sdk_add_compile_definitions(-DconfigEXPECTED_IDLE_TIME_BEFORE_SLEEP=10)
sdk_add_compile_definitions(-DWPA_SUPPLICANT_CLEANUP_INTERVAL=1800)
sdk_add_compile_definitions(-DBL_RA_EN=1)
sdk_add_compile_definitions(-DPRINTF_EN)

if (CONFIG_LPAPP)
sdk_add_compile_definitions(-DLP_APP)
endif()

if(CONFIG_LPM)
sdk_add_compile_definitions(-DCFG_LPM=1)
sdk_add_compile_definitions(-DRC_EZ23Q4=1)
sdk_add_compile_definitions(-DRC_EZ23Q4_EDCA=1)
sdk_add_compile_definitions(-DTX_RETRY_ENABLE=1)
endif()

target_sources(app PRIVATE
                   app/src/app_user.c
                   )

# Virtual channel demo (only compile if CONFIG_MR_VIRTUALCHAN is defined)
if(CONFIG_MR_VIRTUALCHAN)
target_sources(app PRIVATE app/src/virtualchan/app_vchan.c)
endif()


if (CONFIG_WL80211)
    target_sources(app PRIVATE app/src/wifi/app_wifi_wl80211.c)
else ()
    target_sources(app PRIVATE app/src/wifi/app_wifi_fhost.c)
    if (CONFIG_LPAPP)
    target_sources(app PRIVATE app/src/wifi/app_wifi_cli.c)
    endif()
endif()

sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

sdk_set_main_file(main.c)

project(nethub)
