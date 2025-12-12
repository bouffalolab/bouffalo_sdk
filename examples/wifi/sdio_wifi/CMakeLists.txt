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
sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_SOC_DEFAULT=10)
sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_AON_DEFAULT=10)
sdk_add_compile_definitions(-DCONFIG_LWIP_PBUF_CUSTOM_EXT_ENABLE)

if (CONFIG_DCDC_ENABLE)
    sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=8)
else()
    sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=10)
endif()

target_sources(app PRIVATE
                   app/src/app_sdiowifi.c
                   app/src/pm/app_pm.c
                   app/src/pm/tickless.c
                   app/src/sample/app_sample.c
                   )

#sdio
if(CONFIG_HOSTROUTER)
    sdk_add_compile_definitions(-DBL_HOSTROUTER_ENABLE)
    sdk_add_compile_definitions(-DBL_SDIO_RX_MAX_PORT_NUM=4)
    sdk_add_compile_definitions(-DCONFIG_DUAL_STACK)
    sdk_add_compile_definitions(-DCONFIG_SDIO_HIGH_PERFORMANCE)
 
    if(CONFIG_SDIOWIFI_HEARTBEAT)
        sdk_add_compile_definitions(-DSDIOWIFI_HEARTBEAT=1)
    endif()

    if(CONFIG_SDIOWIFI_TTY)
        sdk_add_compile_definitions(-DSDIOWIFI_TTY=1)
    endif()

    if(CONFIG_SDIO_TEST_NO_HOST)
        sdk_add_compile_definitions(-DSDIO_TEST_NO_HOST=1)
    endif()

    if(CONFIG_DUAL_ETH)
        sdk_add_compile_definitions(-DCFG_DUAL_ETH=1)
    endif()

    target_sources(app PRIVATE
                    app/src/rnm_user_ext/app_rnm_user.c
                    )
endif()

sdk_set_main_file(main.c)
sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

project(sdio_wifi)
