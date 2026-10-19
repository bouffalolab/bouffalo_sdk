sdk_generate_library()

if("${CHIP}" STREQUAL "bl602")
sdk_library_add_sources(Src/${CHIP}/rfparam_adapter.c)
else()
sdk_library_add_sources(Src/rfparam_adapter.c)
endif()
if("${CHIP}" STREQUAL "bl616cl")
sdk_library_add_sources(Src/rfparam_bl616cl_flash_otp.c)
endif()
sdk_library_add_sources(Src/rfparam_rftlv.c)


sdk_add_include_directories(Inc)
