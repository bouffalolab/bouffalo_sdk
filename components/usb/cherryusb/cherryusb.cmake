#
# Copyright (c) 2024, sakumisu
#
# SPDX-License-Identifier: Apache-2.0
#

# cmake-format: off

# set(CONFIG_CHERRYUSB_DEVICE 1)
# set(CONFIG_CHERRYUSB_DEVICE_CDC 1)
# set(CONFIG_CHERRYUSB_DEVICE_HID 1)
# set(CONFIG_CHERRYUSB_DEVICE_MSC 1)
# set(CONFIG_CHERRYUSB_DEVICE_AUDIO 1)
# set(CONFIG_CHERRYUSB_DEVICE_VIDEO 1)
# set(CONFIG_CHERRYUSB_DEVICE_DWC2_ST 1)

# set(CONFIG_CHERRYUSB_HOST 1)
# set(CONFIG_CHERRYUSB_HOST_CDC_ACM 1)
# set(CONFIG_CHERRYUSB_HOST_CDC_ECM 1)
# set(CONFIG_CHERRYUSB_HOST_CDC_NCM 1)
# set(CONFIG_CHERRYUSB_HOST_HID 1)
# set(CONFIG_CHERRYUSB_HOST_MSC 1)
# set(CONFIG_CHERRYUSB_HOST_VIDEO 1)
# set(CONFIG_CHERRYUSB_HOST_AUDIO 1)
# set(CONFIG_CHERRYUSB_HOST_CDC_RNDIS 1)
# set(CONFIG_CHERRYUSB_HOST_BLUETOOTH 1)
# set(CONFIG_CHERRYUSB_HOST_ASIX 1)
# set(CONFIG_CHERRYUSB_HOST_RTL8152 1)
# set(CONFIG_CHERRYUSB_HOST_DWC2_ST 1)
# set(CONFIG_CHERRYUSB_HOST_XBOX 1)
# set(CONFIG_CHERRYUSB_HOST_CH34X 1)
# set(CONFIG_CHERRYUSB_HOST_CP210X 1)
# set(CONFIG_CHERRYUSB_HOST_PL2303 1)
# set(CONFIG_CHERRYUSB_HOST_FTDI 1)

# set(CONFIG_CHERRYUSB_OSAL "freertos")
# cmake-format: on

list(
    APPEND
    cherryusb_incs
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/common
    ${CMAKE_CURRENT_LIST_DIR}/core
    ${CMAKE_CURRENT_LIST_DIR}/class/hub
    ${CMAKE_CURRENT_LIST_DIR}/class/cdc
    ${CMAKE_CURRENT_LIST_DIR}/class/hid
    ${CMAKE_CURRENT_LIST_DIR}/class/msc
    ${CMAKE_CURRENT_LIST_DIR}/class/audio
    ${CMAKE_CURRENT_LIST_DIR}/class/video
    ${CMAKE_CURRENT_LIST_DIR}/class/wireless
    ${CMAKE_CURRENT_LIST_DIR}/class/midi
    ${CMAKE_CURRENT_LIST_DIR}/class/adb
    ${CMAKE_CURRENT_LIST_DIR}/class/dfu
    ${CMAKE_CURRENT_LIST_DIR}/class/vendor/net
    ${CMAKE_CURRENT_LIST_DIR}/class/vendor/serial
    ${CMAKE_CURRENT_LIST_DIR}/class/vendor/wifi
    ${CMAKE_CURRENT_LIST_DIR}/class/vendor/xbox
    ${CMAKE_CURRENT_LIST_DIR}/class/aoa
)

if(CONFIG_CHERRYUSB_DEVICE)
    list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/core/usbd_core.c)
    if(CONFIG_CHERRYUSB_DEVICE_CDC_ACM)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/cdc/usbd_cdc_acm.c)
    endif()
    if(CONFIG_CHERRYUSB_DEVICE_HID)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/hid/usbd_hid.c)
    endif()
    if(CONFIG_CHERRYUSB_DEVICE_MSC)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/msc/usbd_msc.c)
    endif()
    if(CONFIG_CHERRYUSB_DEVICE_AUDIO)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/audio/usbd_audio.c)
    endif()
    if(CONFIG_CHERRYUSB_DEVICE_VIDEO)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/video/usbd_video.c)
    endif()
    if(CONFIG_CHERRYUSB_DEVICE_CDC_ECM)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/cdc/usbd_cdc_ecm.c)
    endif()
    if(CONFIG_CHERRYUSB_DEVICE_CDC_NCM)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/cdc/usbd_cdc_ncm.c)
    endif()
    if(CONFIG_CHERRYUSB_DEVICE_CDC_RNDIS)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/wireless/usbd_rndis.c)
    endif()
    if(CONFIG_CHERRYUSB_DEVICE_DFU)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/dfu/usbd_dfu.c)
    endif()
    if(CONFIG_CHERRYUSB_DEVICE_ADB)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/adb/usbd_adb.c)
    endif()

endif()

if(CONFIG_CHERRYUSB_HOST)
    list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/core/usbh_core.c
         ${CMAKE_CURRENT_LIST_DIR}/class/hub/usbh_hub.c
    )

    if(CONFIG_CHERRYUSB_HOST_CDC_ACM)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/cdc/usbh_cdc_acm.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_CDC_ECM)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/cdc/usbh_cdc_ecm.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_CDC_RNDIS)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/wireless/usbh_rndis.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_CDC_NCM)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/cdc/usbh_cdc_ncm.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_HID)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/hid/usbh_hid.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_MSC)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/msc/usbh_msc.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_VIDEO)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/video/usbh_video.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_AUDIO)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/audio/usbh_audio.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_BLUETOOTH)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/wireless/usbh_bluetooth.c)
    endif()

    if(CONFIG_CHERRYUSB_HOST_ASIX)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/vendor/net/usbh_asix.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_RTL8152)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/vendor/net/usbh_rtl8152.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_CH34X)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/vendor/serial/usbh_ch34x.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_CP210X)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/vendor/serial/usbh_cp210x.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_FTDI)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/vendor/serial/usbh_ftdi.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_PL2303)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/vendor/serial/usbh_pl2303.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_XBOX)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/vendor/xbox/usbh_xbox.c)
    endif()
    if(CONFIG_CHERRYUSB_HOST_AOA)
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/class/aoa/usbh_aoa.c)
    endif()

    # ehci port
    list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/port/ehci/usb_hc_ehci.c)
endif()

if(CONFIG_CHERRYUSB_DEVICE AND CONFIG_CHERRYUSB_HOST)
    list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/core/usbotg_core.c)
endif()

if(DEFINED CONFIG_CHERRYUSB_OSAL)
    if("${CONFIG_CHERRYUSB_OSAL}" STREQUAL "freertos")
        list(APPEND cherryusb_srcs ${CMAKE_CURRENT_LIST_DIR}/osal/usb_osal_freertos.c)
    endif()
endif()

