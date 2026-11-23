cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_WIFI_COEX_DEBUG_CLI)

if(CHIP STREQUAL "bl618dg")
    sdk_add_compile_definitions(-DFHOST_IPERF_SEND_BUF_CNT=63)
    sdk_add_compile_definitions(-DHIGH_PERFORMANCE_PBUF=1)
    sdk_add_compile_definitions(-DFHOST_RX_BUF_CNT=50)
endif()

# Only these PHYRF libraries export cmd_set_btble_combo/standalone.
if(("${CHIP}" STREQUAL "bl618dg") AND
   ((("${CPU_MODEL}" STREQUAL "b0") AND (NOT CONFIG_PHY_LPFW)) OR
    CONFIG_PHY_CLI))
sdk_add_compile_definitions(-DCONFIG_BL618DG_PHYRF_BTBLE_PATH_COMMANDS)
endif()

target_sources(app PRIVATE
                   wifi_tcp_client.c
                   wifi_tcp_server.c)

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(wifi_bz_coexm)
