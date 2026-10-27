cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

if("${CHIP}" STREQUAL "bl602")
    sdk_add_compile_definitions(-DFHOST_IPERF_SEND_BUF_CNT=2)
endif()

if(CONFIG_BLUETOOTH)
    target_sources(app PRIVATE btble_cli_port.c)
    if(CONFIG_BT_STACK_CLI)
        target_sources(app PRIVATE btble_test_port.c)
    endif()
endif()

# Reduce BL602 Bluetooth task and RX buffer reservations.
if(CONFIG_BLUETOOTH AND "${CHIP}" STREQUAL "bl602")
    sdk_add_compile_definitions(-DCONFIG_BT_HCI_TX_STACK_SIZE=1536
                                -DCONFIG_BT_RX_BUF_COUNT=3)
endif()

sdk_add_include_directories(.)

sdk_set_main_file(main.c)
sdk_set_flash_erase(2)

project(wifi_ble)
