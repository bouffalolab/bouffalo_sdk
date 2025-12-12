cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DconfigEXPECTED_IDLE_TIME_BEFORE_SLEEP=40)
sdk_add_compile_definitions(-DWPA_SUPPLICANT_CLEANUP_INTERVAL=1800)
sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_SOC_DEFAULT=10)
sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_AON_DEFAULT=10)

if (CONFIG_DCDC_ENABLE)
    sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=8)
else()
    sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=10)
endif()

sdk_add_compile_definitions(-DLP_FW_START_ADDR=0x63026800)
message(WARNING "lpfw bin load in WRAM(0x63026800)")

sdk_add_compile_definitions(-DAT_NET_TX_MAX_LEN=4096)

target_sources(app PRIVATE
                   app/src/app_usbwifi.c
                #    app/src/pm/app_pm.c
                #    app/src/pm/tickless.c
                   app/src/sample/app_sample.c
                   )

#sdio
if(CONFIG_HOSTROUTER)
    sdk_add_compile_definitions(-DBL_HOSTROUTER_ENABLE)
    sdk_add_compile_definitions(-DBL_SDIO_RX_MAX_PORT_NUM=4)
    sdk_add_compile_definitions(-DCONFIG_DUAL_STACK)
    #sdk_add_compile_definitions(-DCONFIG_SDIO_HIGH_PERFORMANCE)

    # if(CONFIG_SDIOWIFI_HEARTBEAT)
    #     sdk_add_compile_definitions(-DSDIOWIFI_HEARTBEAT=1)
    # endif()

    # if(CONFIG_SDIOWIFI_TTY)
    #     sdk_add_compile_definitions(-DSDIOWIFI_TTY=1)
    # endif()

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

sdk_add_compile_definitions(-DCONFIG_ATMODULE)
sdk_add_compile_definitions(-DCONFIG_ATMODULE_USB)

sdk_set_main_file(main.c)
sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

project(usb_wifi)
