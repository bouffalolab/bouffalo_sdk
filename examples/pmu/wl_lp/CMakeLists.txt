cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CONFIG_PSRAM_RETENTION AND NOT CONFIG_PSRAM)
    message(FATAL_ERROR
        "CONFIG_PSRAM_RETENTION=y requires CONFIG_PSRAM=y. "
        "Enable CONFIG_PSRAM or disable CONFIG_PSRAM_RETENTION in wl_lp.")
endif()

# PSRAM retention uses PDS1 on BL616 only; all other chips keep their default.
# Configure CONFIG_PSRAM_RETENTION in defconfig, not in LPFW.
if(CONFIG_PSRAM_RETENTION AND "${CHIP}" STREQUAL "bl616")
    sdk_add_compile_definitions(-DCONFIG_PSRAM_RETENTION=1)
endif()

#sdk_add_compile_definitions(-DTICKLESS_DEBUG)
# sdk_add_compile_definitions(-DCONFIG_IRQ_USE_VECTOR)
sdk_add_compile_definitions(-DCONFIG_RWNX_LWIP)
sdk_add_compile_definitions(-DconfigEXPECTED_IDLE_TIME_BEFORE_SLEEP=40)
sdk_add_compile_definitions(-DWPA_SUPPLICANT_CLEANUP_INTERVAL=1800)
sdk_add_compile_definitions(-DBL_RA_EN=1)

target_sources(app PRIVATE
                   wl_lp_app.c
                   )

sdk_set_main_file(main.c)
sdk_add_include_directories(.)

project(wl_lp)
