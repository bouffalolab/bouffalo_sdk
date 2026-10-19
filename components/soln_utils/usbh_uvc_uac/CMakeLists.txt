# USB host UVC/UAC streaming

# enable usb ehci iso
sdk_add_compile_definitions(-D CONFIG_USB_EHCI_ISO)

# Public headers, plus usb_uvc_uac_config_template.h: the cherryusb
# configuration this module is built against. The application's usb_config.h
# must include it so that both sides agree on cherryusb struct layout.
sdk_add_include_directories(inc)

# Private source when available, otherwise lib/libusbh_uvc_uac_${CHIP}.a
if(SOLN_PRIVATE_SOURCE_ENABLED)
	if(NOT SOLN_PRIVATE_USBH_UVC_UAC_SOURCES)
		message(FATAL_ERROR "soln_utils/soln_private/sources.cmake does not define UVC/UAC sources")
	endif()

	sdk_generate_library(usbh_uvc_uac)
	sdk_library_add_sources(${SOLN_PRIVATE_USBH_UVC_UAC_SOURCES})
	sdk_add_private_include_directories(${SOLN_PRIVATE_USBH_UVC_UAC_INCLUDES})
else()
	set(SOLN_USBH_UVC_UAC_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib/libusbh_uvc_uac_${CHIP}.a)
	if(NOT EXISTS ${SOLN_USBH_UVC_UAC_LIBRARY})
		message(FATAL_ERROR "Missing UVC/UAC prebuilt library for CHIP=${CHIP}: ${SOLN_USBH_UVC_UAC_LIBRARY}")
	endif()
	sdk_add_static_library(${SOLN_USBH_UVC_UAC_LIBRARY})
endif()
