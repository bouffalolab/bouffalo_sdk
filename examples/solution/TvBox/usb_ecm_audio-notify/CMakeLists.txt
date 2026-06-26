cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

set(CHERRYUSB_CLI_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../cherryusb/cherryusb_cli)

sdk_add_include_directories(.)
sdk_add_include_directories(${CHERRYUSB_CLI_DIR})
sdk_add_include_directories(${CHERRYUSB_CLI_DIR}/device_demo)
sdk_add_include_directories(bl616cl_solution)

function(usb_ecm_audio_bool01 out var default_value)
    if(DEFINED ${var})
        set(value ${${var}})
    else()
        set(value ${default_value})
    endif()

    if("${value}" STREQUAL "y" OR "${value}" STREQUAL "Y" OR
       "${value}" STREQUAL "1" OR "${value}" STREQUAL "ON" OR
       "${value}" STREQUAL "TRUE")
        set(${out} 1 PARENT_SCOPE)
    else()
        set(${out} 0 PARENT_SCOPE)
    endif()
endfunction()

usb_ecm_audio_bool01(USB_ETHERNET_HID_ENABLE_VALUE CONFIG_USB_ETHERNET_HID_ENABLE 1)
usb_ecm_audio_bool01(USB_ETHERNET_UAC_ENABLE_VALUE CONFIG_USB_ETHERNET_UAC_ENABLE 1)
usb_ecm_audio_bool01(USB_ETHERNET_UAC_SPEAKER_ENABLE_VALUE CONFIG_USB_ETHERNET_UAC_SPEAKER_ENABLE 1)
usb_ecm_audio_bool01(USB_ETHERNET_UAC_MIC_ENABLE_VALUE CONFIG_USB_ETHERNET_UAC_MIC_ENABLE 1)

sdk_add_compile_definitions(
    -DCONFIG_BT_DEVICE_NAME=\"USB-ECM-AUD\"
    -DUSB_ETHERNET_BLE_ENABLE=1
    -DUSB_ETHERNET_HID_ENABLE=${USB_ETHERNET_HID_ENABLE_VALUE}
    -DUSB_ETHERNET_UAC_ENABLE=${USB_ETHERNET_UAC_ENABLE_VALUE}
    -DUSB_ETHERNET_UAC_SPEAKER_ENABLE=${USB_ETHERNET_UAC_SPEAKER_ENABLE_VALUE}
    -DUSB_ETHERNET_UAC_MIC_ENABLE=${USB_ETHERNET_UAC_MIC_ENABLE_VALUE}
)

sdk_set_main_file(main.c)

target_sources(app PRIVATE
    ble_hid_bridge.c
    usb_ecm_ble.c
    usb_ecm_uac_device.c
    bl616cl_solution/board_gpio_overlay.c
    ${CHERRYUSB_CLI_DIR}/device_demo/eth_emac.c
)

project(usb_ecm_audio_notify)
