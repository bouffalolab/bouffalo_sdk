
sdk_add_include_directories(.)

sdk_add_compile_definitions(-D CONFIG_USB_EHCI_ISO)

if(CONFIG_SOLUTION_BUILD_FROM_LIB_MODE)
    sdk_add_static_library(libusbh_uvc_uac.a)
else()
    sdk_generate_library(usbh_uvc_uac)

    sdk_library_add_sources(usb_hc_ehci_iso.c usbh_uvc_stream.c usbh_uac_stream.c)
endif()
