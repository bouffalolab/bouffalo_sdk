cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_WIFI_COEX_DEBUG_CLI)

# Only these PHYRF libraries export cmd_set_btble_combo/standalone.
if(("${CHIP}" STREQUAL "bl618dg") AND
   ((("${CPU_MODEL}" STREQUAL "b0") AND (NOT CONFIG_PHY_LPFW)) OR
    CONFIG_PHY_CLI))
sdk_add_compile_definitions(-DCONFIG_BL618DG_PHYRF_BTBLE_PATH_COMMANDS)
endif()

if(CONFIG_WIFI_GATEWAY)
sdk_add_compile_definitions(-DFHOST_BUFFER_COPY=1)
sdk_add_compile_definitions(-DFHOST_RX_BUF_CNT=48)
sdk_add_compile_definitions(-DFHOST_RX_BUF_SECTION=\".bss\")
sdk_add_compile_definitions(-DCONFIG_WIFI_GATEWAY)
sdk_add_compile_definitions(-DCFG_ENABLE_AP_PS)
#sdk_add_compile_definitions(-DLWIP_HEAP_SIZE=\(32*1024\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_TCP=\(30*60*1000\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_TCP_DISCON=\(2*20*1000\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_UDP=\(620*1000\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_ICMP=\(5*2*1000\))
endif()

target_sources(app PRIVATE
                   wifi_tcp_client.c
                   wifi_tcp_server.c)

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(wifi_bz_coexm)
