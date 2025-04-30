sdk_generate_library()

sdk_library_add_sources(Src/rfparam_adapter.c)
sdk_library_add_sources(Src/rfparam_rftlv.c)

sdk_add_include_directories(Inc)

if(CONFIG_RFPARAM_TLV_FIXED_ADDRESS)
    sdk_add_compile_definitions(-D CONFIG_RFPARAM_TLV_FIXED_ADDRESS)
endif()

if("${CHIP}" STREQUAL "bl616")
    sdk_add_compile_definitions(-D RFPARAM_BL616)
endif()
