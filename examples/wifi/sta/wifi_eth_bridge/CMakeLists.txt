cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

sdk_add_compile_definitions(-D${CONFIG_ETHERNET_PHY})

target_sources(app PRIVATE
                   app/src/app_apeth_bridge.c
                   app/src/sample/app_sample.c
                   app/src/apeth_bridge/apeth_bridge_emac.c
                   app/src/apeth_bridge/apeth_bridge_cli.c
                   )

sdk_set_main_file(main.c)

sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

project(wifi_eth_bridge)
