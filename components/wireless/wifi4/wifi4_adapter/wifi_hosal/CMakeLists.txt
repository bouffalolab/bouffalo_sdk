sdk_generate_library()

## These include paths would be exported to project level
sdk_add_include_directories(include)

sdk_library_add_sources(wifi_hosal.c)


if(("${CHIP}" STREQUAL "bl602"))
    sdk_library_add_sources(port/wifi_hosal_bl602.c)
endif()

