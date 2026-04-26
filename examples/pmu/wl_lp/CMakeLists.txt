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

if("${CHIP}" STREQUAL "bl616")
    if (CONFIG_DCDC_ENABLE)
        sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=8)
    else()
        sdk_add_compile_definitions(-DPM_PDS_LDO_LEVEL_RT_DEFAULT=10)
    endif()
endif()

if("${CHIP}" STREQUAL "bl616cl")

    if(CONFIG_PSRAM)
        sdk_add_compile_definitions(-DPM_PDS_LDO18IO_POWER_DOWN=0)
    else()
        sdk_add_compile_definitions(-DPM_PDS_LDO18IO_POWER_DOWN=1)
    endif()

    set(BL616CL_LDO_SOC_LP_ENABLE 1)
    set(BL616CL_LDO_SYS_LP_ENABLE 1)
    set(BL616CL_LDO_SOC_LEVEL_DEFAULT HBN_LDO_SOC_LEVEL_0P700V)
    set(BL616CL_LDO_SYS_LEVEL_DEFAULT HBN_LDO_SYS_LEVEL_1P300V)
    set(BL616CL_DCDC_CTRL_IF 0)
    set(BL616CL_POWER_SUPPLY_MODE BL616CL_POWER_SUPPLY_INTERNAL_LDO)

    if(CONFIG_BL616CL_POWER_SUPPLY_EXT_DCDC_SOC AND CONFIG_BL616CL_POWER_SUPPLY_EXT_DCDC_SYS)
        set(BL616CL_POWER_SUPPLY_MODE BL616CL_POWER_SUPPLY_EXT_DCDC_SOC_SYS)
    elseif(CONFIG_BL616CL_POWER_SUPPLY_EXT_DCDC_SOC)
        set(BL616CL_POWER_SUPPLY_MODE BL616CL_POWER_SUPPLY_EXT_DCDC_SOC)
    elseif(CONFIG_BL616CL_POWER_SUPPLY_EXT_DCDC_SYS)
        set(BL616CL_POWER_SUPPLY_MODE BL616CL_POWER_SUPPLY_EXT_DCDC_SYS)
    endif()

    if(CONFIG_BL616CL_POWER_SUPPLY_EXT_DCDC_SOC)
        set(BL616CL_LDO_SOC_LEVEL_DEFAULT HBN_LDO_SOC_LEVEL_0P650V)
        set(BL616CL_DCDC_CTRL_PIN ${CONFIG_BL616CL_DCDC_CTRL_PIN})

        if(CONFIG_BL616CL_DCDC_CTRL_USE_SYS AND CONFIG_BL616CL_DCDC_CTRL_USE_STD)
            message(FATAL_ERROR "BL616CL DCDC ctrl interface cannot enable SYS and STD at the same time")
        elseif(CONFIG_BL616CL_DCDC_CTRL_USE_SYS)
            message(FATAL_ERROR "BL616CL DCDC ctrl interface not support SYS")
            set(BL616CL_DCDC_CTRL_IF 1)
            sdk_add_compile_definitions(-DBL_EXT_DCDC_OUTPUT_CTRL_PIN=${BL616CL_DCDC_CTRL_PIN})
        elseif(CONFIG_BL616CL_DCDC_CTRL_USE_STD)
            set(BL616CL_DCDC_CTRL_IF 2)
            sdk_add_compile_definitions(-DPM_EXT_DCDC_SOC_CTRL_PIN=12)
        endif()
    endif()

    if(CONFIG_BL616CL_POWER_SUPPLY_EXT_DCDC_SYS)
        set(BL616CL_LDO_SYS_LEVEL_DEFAULT HBN_LDO_SYS_LEVEL_1P200V)
    endif()

    sdk_add_compile_definitions(-DBL616CL_POWER_SUPPLY_MODE=${BL616CL_POWER_SUPPLY_MODE})
    sdk_add_compile_definitions(-DBL616CL_DCDC_CTRL_IF=${BL616CL_DCDC_CTRL_IF})
    sdk_add_compile_definitions(-DPM_PDS_LDO_SOC_LOWPOWER_ENABLE=${BL616CL_LDO_SOC_LP_ENABLE})
    sdk_add_compile_definitions(-DPM_PDS_LDO_SYS_LOWPOWER_ENABLE=${BL616CL_LDO_SYS_LP_ENABLE})
    sdk_add_compile_definitions(-DPM_PDS_LDO_SOC_LEVEL_DEFAULT=${BL616CL_LDO_SOC_LEVEL_DEFAULT})
    sdk_add_compile_definitions(-DPM_PDS_LDO_SYS_LEVEL_DEFAULT=${BL616CL_LDO_SYS_LEVEL_DEFAULT})
endif()


target_sources(app PRIVATE
                   wl_lp_app.c
                   )

sdk_set_main_file(main.c)
sdk_add_include_directories(.)

project(wl_lp)
