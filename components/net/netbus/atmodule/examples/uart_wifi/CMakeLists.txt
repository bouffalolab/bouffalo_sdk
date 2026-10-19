cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DAT_NET_TX_MAX_LEN=6144)

set(UART_WIFI_LPAPP_SUPPORTED_CHIPS bl616 bl616cl bl618dg)

if (CONFIG_LPAPP AND NOT CONFIG_WL80211 AND "${CHIP}" IN_LIST UART_WIFI_LPAPP_SUPPORTED_CHIPS)
    sdk_add_compile_definitions(-DPM_PDS_LDO18IO_POWER_DOWN=1)
    sdk_add_compile_definitions(-DPM_PDS_LDO_SOC_LEVEL_DEFAULT=HBN_LDO_SOC_LEVEL_0P750V)
    sdk_add_compile_definitions(-DPM_PDS_LDO_SYS_LEVEL_DEFAULT=HBN_LDO_SYS_LEVEL_1P250V)
    target_sources(app PRIVATE
                       app/src/app_uartwifi.c
                       app/src/atmodule/app_atmodule.c
                       app/src/pm/app_pm.c
                       )
else ()
    if(NOT CONFIG_PSRAM_COPY_CODE)
        sdk_add_compile_definitions(-DCFG_LINK_ALL_IN_FLASH=1)
        sdk_add_compile_definitions(-DCONFIG_HIGH_PERFORMANCE=1)
    endif()

    target_sources(app PRIVATE
                       app/src/app_uartwifi.c
                       app/src/atmodule/app_atmodule.c
                       )
endif()

if (CONFIG_WL80211)
    target_sources(app PRIVATE app/src/wifi/app_wifi_wl80211.c)
else ()
    target_sources(app PRIVATE app/src/wifi/app_wifi_fhost.c)
endif()
target_sources(app PRIVATE boards/${CHIP}/board_gpio_overlay.c)

sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)
sdk_add_include_directories(./boards/${CHIP})

sdk_set_main_file(main.c)

sdk_add_flash_partition_image(
    PARTITION media
    FILE ${CMAKE_BINARY_DIR}/build_out/littlefs.bin)
sdk_set_flash_erase(2)

project(uart_wifi)
