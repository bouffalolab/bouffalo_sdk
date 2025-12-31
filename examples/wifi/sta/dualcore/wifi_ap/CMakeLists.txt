cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DCONFIG_BOOT_NP_ASYNC)

sdk_add_include_directories(.)

sdk_add_include_directories(${CMAKE_SOURCE_DIR}/../../../../../components/utils/wifi_ota)

# Add flash_rpc_master.c for dual-core OTA (AP executes Flash commands for NP)
if(CONFIG_WIFI_OTA_DUAL_CORE)
    sdk_add_include_directories(${CMAKE_SOURCE_DIR}/../../../../../components/utils/wifi_ota/rpc)
    sdk_add_include_directories(${CMAKE_SOURCE_DIR}/../../../../../drivers/soc/bl616d/std/include)
    target_sources(app PRIVATE
        ${CMAKE_SOURCE_DIR}/../../../../../components/utils/wifi_ota/rpc/flash_rpc_master.c
    )
endif()

sdk_set_main_file(main.c)

project(wifi)

