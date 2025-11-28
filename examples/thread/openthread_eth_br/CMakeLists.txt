cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DCONFIG_OTBR_REST=${CONFIG_OTBR_REST})

if(CONFIG_LMAC154_LOG)
sdk_add_compile_definitions(-DAPP_PRINT=lmac154_log)
else()
sdk_add_compile_definitions(-DAPP_PRINT=printf)
endif()
sdk_add_compile_definitions(-DCONFIG_LMAC154_LOG=${CONFIG_LMAC154_LOG})

if (CONFIG_SHELL AND CONFIG_LMAC154_CMD) 
sdk_add_compile_definitions(-DCONFIG_LMAC154_CMD=1)
endif()

sdk_add_include_directories(.)

target_sources(app PRIVATE otbr_cmd.c)

sdk_set_main_file(main.c)

project(openthread_eth_br)
