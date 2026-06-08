cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DTICKLESS_DEBUG=4)
sdk_add_compile_definitions(-DPM_PDS_LDO18IO_POWER_DOWN)
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
sdk_add_compile_definitions(-DCONFIG_RWNX_LWIP)
sdk_add_compile_definitions(-DconfigEXPECTED_IDLE_TIME_BEFORE_SLEEP=500)
sdk_add_compile_definitions(-DWPA_SUPPLICANT_CLEANUP_INTERVAL=1800)
sdk_add_compile_definitions(-DBL_RA_EN=1)

sdk_add_compile_definitions(-D PRINTF_EN)



target_sources(app PRIVATE
                   wl_lp_app.c
                   )

sdk_set_main_file(main.c)
sdk_add_include_directories(.)

project(wl_lp)
