cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

#sdk_add_compile_definitions(-DTICKLESS_DEBUG)
# sdk_add_compile_definitions(-DCONFIG_IRQ_USE_VECTOR)
sdk_add_compile_definitions(-DLP_APP)
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
sdk_add_compile_definitions(-DconfigUSE_TICKLESS_IDLE=1)
sdk_add_compile_definitions(-Dconfig_CUSTOM_TICKLESS=1)
sdk_add_compile_definitions(-DCFG_BL_WIFI_PS_ENABLE)
sdk_add_compile_definitions(-DCONFIG_RWNX_LWIP)
sdk_add_compile_definitions(-DconfigEXPECTED_IDLE_TIME_BEFORE_SLEEP=40)
sdk_add_compile_definitions(-DWPA_SUPPLICANT_CLEANUP_INTERVAL=1800)
sdk_add_compile_definitions(-DBL_RA_EN=1)
sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_SOC_DEFAULT=10)
sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_AON_DEFAULT=10)

if (CONFIG_LPM)
    sdk_add_compile_definitions(-DCFG_LPM=1)
    sdk_add_compile_definitions(-DRC_EZ23Q4=1)
    sdk_add_compile_definitions(-DRC_EZ23Q4_EDCA=1)
    sdk_add_compile_definitions(-DTX_RETRY_ENABLE=1)
endif()

if (CONFIG_DCDC_ENABLE)
    sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=8)
else()
    sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=10)
endif()

target_sources(app PRIVATE
                   tickless.c
                   wl_lp_app.c
                   )

sdk_set_main_file(main.c)
sdk_add_include_directories(.)

project(wl_lp)
