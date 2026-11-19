cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User

sdk_add_compile_definitions(-D${CONFIG_ETHERNET_PHY})

# WiFi TX requires shared RAM; copy bridge pbufs allocated in PSRAM before TX.
sdk_add_compile_definitions(-DFHOST_BUFFER_COPY=1)

if(CHIP STREQUAL "bl618dg")
    # Two physical EMAC ports and one Wi-Fi port share this bridge.
    sdk_add_compile_definitions(-DNETIFD_BRIDGE_MAX_PORTS=3)
endif()

if(CONFIG_BSP_LWIP_EMAC_START OR CONFIG_BSP_LWIP_NETIF_EMAC)
    message(FATAL_ERROR "wifi_eth_bridge uses apeth_bridge_emac.c for all ports; disable BSP lwIP EMAC drivers")
endif()

target_sources(app PRIVATE
                   app/src/app_apeth_bridge.c
                   app/src/sample/app_sample.c
                   app/src/apeth_bridge/apeth_bridge_emac.c
                   app/src/apeth_bridge/apeth_bridge_cli.c
                   app/src/station_stats/wifi_station_stats.c
                   # Link the Ethernet bridge into this example.
                   app/src/apeth_bridge/apeth_bridge_wds.c
                   )

if(NOT CONFIG_WIFI_WDS)
    message(FATAL_ERROR "wifi_eth_bridge requires CONFIG_WIFI_WDS=y. MACSW_SELECT=bridge links the WDS library. Three-address mode is runtime enable=0.")
endif()
if(NOT CONFIG_FHOST)
    message(FATAL_ERROR "wifi_eth_bridge requires FHOST")
endif()
if(CONFIG_WL80211)
    message(FATAL_ERROR "wifi_eth_bridge cannot be built with WL80211")
endif()
if(NOT CONFIG_NETIFD_NANO)
    message(FATAL_ERROR "wifi_eth_bridge creates the bridge through NETIFD_NANO")
endif()

sdk_set_main_file(main.c)

sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

project(wifi_eth_bridge)
