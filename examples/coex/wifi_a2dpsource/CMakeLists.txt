cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

#sdk_add_compile_definitions(-DTICKLESS_DEBUG)
# sdk_add_compile_definitions(-DCONFIG_IRQ_USE_VECTOR)

# sdk_add_compile_definitions(-DLP_APP)
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
# sdk_add_compile_definitions(-DconfigUSE_TICKLESS_IDLE=1)
# sdk_add_compile_definitions(-Dconfig_CUSTOM_TICKLESS=1)
# sdk_add_compile_definitions(-DCFG_BL_WIFI_PS_ENABLE)

sdk_add_compile_definitions(-DCONFIG_RWNX_LWIP)
sdk_add_compile_definitions(-DconfigEXPECTED_IDLE_TIME_BEFORE_SLEEP=40)
sdk_add_compile_definitions(-DWPA_SUPPLICANT_CLEANUP_INTERVAL=1800)
sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_SOC_DEFAULT=10)
sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_AON_DEFAULT=10)

if (CONFIG_DCDC_ENABLE)
    sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=8)
else()
    sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=10)
endif()

target_sources(app PRIVATE
                   app/src/app_user.c
                   app/src/a2dpsource/app_a2dpsource.c
                   app/src/sample/app_sample.c
                   )

sdk_set_main_file(main.c)
sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

project(wifi_a2dpsource)

