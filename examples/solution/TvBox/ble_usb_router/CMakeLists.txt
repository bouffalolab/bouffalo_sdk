cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

set(CHERRYUSB_CLI_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../cherryusb/cherryusb_cli)

sdk_add_include_directories(. ble usb)
sdk_add_include_directories(${CHERRYUSB_CLI_DIR})
sdk_add_include_directories(${CHERRYUSB_CLI_DIR}/config_file)
sdk_add_include_directories(${CHERRYUSB_CLI_DIR}/device_demo)

sdk_add_compile_definitions(
    -DCONFIG_BT_DEVICE_NAME=\"USB-ECM-AUD\"
    -DUSB_ECM_BLE_ENABLE_ADV=1
    -DUSB_ECM_BLE_ENABLE_SCAN=0
    -DUSB_ECM_BLE_ENABLE_WRITE_RX=1
    -DUSB_ECM_BLE_ENABLE_WRITE_TX=0
)

sdk_set_main_file(main.c)

target_sources(app PRIVATE
    usb/usb_hid_bridge.c
    ble/ble_common.c
    ble/ble_remote.c
    usb/usb_composite.c
    ${CHERRYUSB_CLI_DIR}/device_demo/eth_emac.c
)

project(ble_usb_router)
