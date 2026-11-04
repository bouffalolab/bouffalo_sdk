cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

set(BLE_SHARED_DIR ${CMAKE_CURRENT_LIST_DIR}/../ble_usb_router/ble)

sdk_add_include_directories(.)
sdk_add_include_directories(${BLE_SHARED_DIR})

sdk_add_compile_definitions(
    -DCONFIG_BT_DEVICE_NAME=\"BLE-HID-SENDER\"
    -DUSB_ECM_BLE_PEER_DEVICE_NAME=\"USB-ECM-AUD\"
    -DUSB_ECM_BLE_ENABLE_ADV=0
    -DUSB_ECM_BLE_ENABLE_SCAN=1
    -DUSB_ECM_BLE_ENABLE_WRITE_RX=0
    -DUSB_ECM_BLE_ENABLE_WRITE_TX=1
)

sdk_set_main_file(main.c)

target_sources(app PRIVATE
    ${BLE_SHARED_DIR}/ble_common.c
    ${BLE_SHARED_DIR}/ble_notify.c
)

project(ble_hid_cli_sender)
