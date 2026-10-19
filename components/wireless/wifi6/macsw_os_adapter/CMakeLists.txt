sdk_generate_library()

sdk_library_add_sources(src/rtos_al.c)
sdk_library_add_sources(src/platform_bouffalo_sdk.c)
if(CHIP STREQUAL "bl618dg")
    sdk_library_add_sources(src/platform_chip_bl618dg.c)
else()
    sdk_library_add_sources(src/platform_chip_default.c)
endif()

sdk_add_include_directories(include)
