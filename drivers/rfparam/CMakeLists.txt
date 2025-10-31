sdk_generate_library()

if("${CHIP}" STREQUAL "bl602")
sdk_library_add_sources(Src/${CHIP}/rfparam_adapter.c)
else()
sdk_library_add_sources(Src/rfparam_adapter.c)
endif()
sdk_library_add_sources(Src/rfparam_rftlv.c)


sdk_add_include_directories(Inc)

if(CONFIG_RFPARAM_TLV_FIXED_ADDRESS)
    sdk_add_compile_definitions(-D CONFIG_RFPARAM_TLV_FIXED_ADDRESS)
endif()

