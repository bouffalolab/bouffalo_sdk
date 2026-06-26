cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

set(BLE_SHARED_DIR ${CMAKE_CURRENT_LIST_DIR}/../usb_ecm_audio-notify)

sdk_add_include_directories(.)
sdk_add_include_directories(${BLE_SHARED_DIR})

sdk_add_compile_definitions(
    -DCONFIG_BT_DEVICE_NAME=\"BLE-HID-SENDER\"
    -DUSB_ECM_BLE_LOCAL_DEVICE_NAME=\"BLE-HID-SENDER\"
    -DUSB_ECM_BLE_PEER_DEVICE_NAME=\"USB-ECM-AUD\"
    -DUSB_ECM_BLE_ENABLE_ADV=0
    -DUSB_ECM_BLE_ENABLE_SCAN=1
    -DUSB_ECM_BLE_CONNECT_BY_ADDR_ORDER=0
    -DUSB_ETHERNET_BLE_AUTO_TEST_ENABLE=0
)

sdk_set_main_file(main.c)

target_sources(app PRIVATE
    ${BLE_SHARED_DIR}/usb_ecm_ble.c
)

project(ble_hid_cli_sender)
