sdk_generate_library()

sdk_add_include_directories(.)

sdk_library_add_sources(${CHIP}/${CHIP}_sys.c)
sdk_library_add_sources(${CHIP}/${CHIP}_lp.c)

if("${CHIP}" STREQUAL "bl702")
    if(CONFIG_PM)
        sdk_library_add_sources(bl702/bl702_xip_recovery.c bl702/bl702_lp_debug.c)
    endif()
endif()

if("${CHIP}" STREQUAL "bl616l")
    sdk_library_add_sources(bl616l/bl616l_xip_recovery.c)
endif()

sdk_add_include_directories(${CHIP})
