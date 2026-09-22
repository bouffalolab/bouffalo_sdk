cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

set(WIFI_P2P_COMMON_DIR ${CMAKE_CURRENT_LIST_DIR}/../wifi_p2p)

sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
sdk_add_compile_definitions(-DHIGH_PERFORMANCE_PBUF=1)

if(CHIP STREQUAL "bl618dg")
    # OOSEQ is capped at 16 pbufs; reserve RX buffers for gap recovery.
    sdk_add_compile_definitions(-DFHOST_RX_BUF_CNT=24)
endif()

target_sources(app PRIVATE
                   ${WIFI_P2P_COMMON_DIR}/wifi_p2p.c
                   ${WIFI_P2P_COMMON_DIR}/wifi_tcp_client.c
                   ${WIFI_P2P_COMMON_DIR}/wifi_tcp_server.c)

sdk_add_include_directories(.)

sdk_set_main_file(${WIFI_P2P_COMMON_DIR}/main.c)

project(wifi_p2p_high_perf)
