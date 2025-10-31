sdk_generate_library()

## These include paths would be exported to project level
sdk_add_include_directories(bl60x_wifi_driver/include)

sdk_add_private_include_directories(bl60x_wifi_driver)

## This component's src
sdk_library_add_sources(bl60x_wifi_driver/ipc_host.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_cmds.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_irqs.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_main.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_mod_params.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_msg_rx.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_msg_tx.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_platform.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_rx.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_tx.c)
sdk_library_add_sources(bl60x_wifi_driver/bl_utils.c)
sdk_library_add_sources(bl60x_wifi_driver/stateMachine.c)
sdk_library_add_sources(bl60x_wifi_driver/wifi.c)
sdk_library_add_sources(bl60x_wifi_driver/wifi_mgmr.c)
sdk_library_add_sources(bl60x_wifi_driver/wifi_mgmr_api.c)
sdk_library_add_sources(bl60x_wifi_driver/wifi_mgmr_cli.c)
sdk_library_add_sources(bl60x_wifi_driver/wifi_mgmr_ext.c)
sdk_library_add_sources(bl60x_wifi_driver/wifi_mgmr_profile.c)
sdk_library_add_sources(bl60x_wifi_driver/wifi_netif.c)
sdk_library_add_sources(bl60x_wifi_driver/wifi_mgmr_event.c)
sdk_library_add_sources(bl60x_wifi_driver/wifi_pkt_hooks.c)

sdk_add_compile_definitions(-DBL_CHIP_NAME="${CHIP}")

if(("${CHIP}" STREQUAL "bl602"))
    sdk_add_compile_definitions(-DCFG_TXDESC=2)
endif()

if(NOT DEFINED CONFIG_VIRT_DEV_MAX)
    set(CONFIG_VIRT_DEV_MAX 2)
endif()
sdk_add_compile_definitions(-DCFG_VIRT_DEV_MAX=${CONFIG_VIRT_DEV_MAX})

if(NOT DEFINED CONFIG_STA_MAX)
    set(CONFIG_STA_MAX 5)
endif()
sdk_add_compile_definitions(-DCFG_STA_MAX=${CONFIG_STA_MAX})

if(CONFIG_BL_IOT_FW_STATISTIC EQUAL 1)
sdk_add_compile_definitions(-DCFG_BL_STATISTIC)
endif()

if(CONFIG_NETBUS_WIFI_ENABLE EQUAL 1)
sdk_add_compile_definitions(-DCFG_NETBUS_WIFI_ENABLE)
endif()

if(CONFIG_CHIP_NAME EQUAL BL602)
sdk_add_compile_definitions(-DCFG_CHIP_BL602)
endif()

if(CONFIG_WIFI_PKT_INPUT_HOOK EQUAL 1)
sdk_add_compile_definitions(-D PKT_INPUT_HOOK)
endif()

if(CONFIG_WIFI_PKT_OUTPUT_HOOK EQUAL 1)
sdk_add_compile_definitions(-D PKT_OUTPUT_HOOK)
endif()

