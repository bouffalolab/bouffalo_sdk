
# enable usb ehci iso
sdk_add_compile_definitions(-D CONFIG_USB_EHCI_ISO)

sdk_add_include_directories(inc)

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
    sdk_generate_library()
    sdk_add_include_directories(src)
    sdk_library_add_sources(src/usb_hc_ehci_iso.c)
    sdk_library_add_sources(src/usbh_uac_stream.c)
    sdk_library_add_sources(src/usbh_uvc_stream.c)
else()
    sdk_add_static_library(lib/libusbh_uvc_uac_e907.a)
endif()
