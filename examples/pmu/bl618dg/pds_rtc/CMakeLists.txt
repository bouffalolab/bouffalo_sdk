cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

#sdk_add_compile_definitions(-D CONFIG_BOARD_FLASH_INIT_SKIP)
#sdk_add_compile_definitions(-D PM_PDS_FLASH_POWER_OFF=0)
#sdk_add_compile_definitions(-D PM_PDS_LDO_SOC_LEVEL_DEFAULT=AON_LDO_SOC_LEVEL_0P800V)
#sdk_add_compile_definitions(-D PM_PDS_DCDC_SYS_LEVEL_DEFAULT=AON_DCDC_SYS_LEVEL_1P150V)
#sdk_add_compile_definitions(-D PM_PDS_LDO18_AON_LEVEL_DEFAULT=AON_LDO18_AON_LEVEL_1P75V)


sdk_set_main_file(main.c)

project(pds_rtc)

