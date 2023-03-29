sdk_generate_library()

sdk_library_add_sources(Src/rfparam_adapter.c)
sdk_library_add_sources(Src/rfparam_rftlv.c)

sdk_add_include_directories(Inc)

if("${CHIP}" STREQUAL "bl616")
sdk_add_compile_definitions(-DRFPARAM_BL616)
elseif("${CHIP}" STREQUAL "wb03")
sdk_add_compile_definitions(-DRFPARAM_WB03)
endif()
