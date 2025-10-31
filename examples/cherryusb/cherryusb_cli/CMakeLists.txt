cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-D SHELL_THREAD_PRIO=20)

sdk_add_include_directories(.)
sdk_set_main_file(main.c)

if(CONFIG_CHERRYUSB_DEVICE)
    target_sources(app PRIVATE ./device_demo/usbd_cli.c)

    if(CONFIG_CHERRYUSB_DEVICE_CDC_ACM)
        target_sources(app PRIVATE ./device_demo/cdc_acm_template.c)
    endif()

    if(CONFIG_CHERRYUSB_DEVICE_CDC_ECM)
        target_sources(app PRIVATE ./device_demo/cdc_ecm_emac_template.c)
    endif()

    if(CONFIG_CHERRYUSB_DEVICE_CDC_RNDIS)
        target_sources(app PRIVATE ./device_demo/cdc_rndis_emac_template.c)
    endif()

    if(CONFIG_CHERRYUSB_DEVICE_HID)
        target_sources(app PRIVATE ./device_demo/hid_keyboard_template.c)
    endif()

    if(CONFIG_CHERRYUSB_DEVICE_MSC)
        target_sources(app PRIVATE ./device_demo/msc_ram_template.c)
    endif()

    if(CONFIG_CHERRYUSB_DEVICE_AUDIO)
        target_sources(app PRIVATE ./device_demo/uac_v1_mic_speaker_multichan_template.c)
    endif()

    if(CONFIG_CHERRYUSB_DEVICE_VIDEO)
        target_sources(app PRIVATE ./device_demo/uvc_static_mjpeg_template.c)
    endif()

    # composite device
    if(CONFIG_CHERRYUSB_DEVICE_CDC_ACM AND CONFIG_CHERRYUSB_DEVICE_HID)
        target_sources(app PRIVATE ./device_demo/cdc_acm_msc_template.c)
    endif()

    # eth emac
    if(CONFIG_CHERRYUSB_DEVICE_CDC_ECM OR CONFIG_CHERRYUSB_DEVICE_CDC_RNDIS)
        target_sources(app PRIVATE ./device_demo/eth_emac.c)
    endif()
endif()

if(CONFIG_CHERRYUSB_HOST)
    target_sources(app PRIVATE ./host_demo/usbh_cli.c)

    if(CONFIG_CHERRYUSB_HOST_CDC_ACM)
        target_sources(app PRIVATE ./host_demo/cdc_acm_template.c)
    endif()

    if(CONFIG_CHERRYUSB_HOST_CDC_ECM)
        target_sources(app PRIVATE ./host_demo/cdc_ecm_lwip_template.c)
    endif()

    if(CONFIG_CHERRYUSB_HOST_HID)
        target_sources(app PRIVATE ./host_demo/hid_template.c)
    endif()

    if(CONFIG_CHERRYUSB_HOST_MSC)
        target_sources(app PRIVATE ./host_demo/msc_fatfs_template.c)
    endif()
endif()

project(cherryusb)
