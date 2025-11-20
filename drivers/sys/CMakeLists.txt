sdk_generate_library()

sdk_add_include_directories(.)

sdk_library_add_sources(${CHIP}/${CHIP}_sys.c)

string(TOLOWER "${CHIP}" CHIP_LOWER)
if(CHIP_LOWER MATCHES "^bl616(l|d)?$")
  sdk_library_add_sources(${CHIP}/${CHIP}_lp.c)
endif()

if("${CHIP}" STREQUAL "bl702")
    if(CONFIG_PM)
        sdk_library_add_sources(bl702/bl702_xip_recovery.c bl702/bl702_lp_debug.c)
    endif()
endif()

if("${CHIP}" STREQUAL "bl616")
    sdk_library_add_sources(bl616/bl616_xip_recovery.c)
    if(CONFIG_WIFI6)
        sdk_library_add_sources(bl616/bl616_pm_wifi.c)
    endif()
endif()


if("${CHIP}" STREQUAL "bl616l")
    sdk_library_add_sources(bl616l/bl616l_xip_recovery.c)
endif()

sdk_add_include_directories(${CHIP})
