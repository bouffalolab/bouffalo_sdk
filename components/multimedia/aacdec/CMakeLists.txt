
if("${CHIP}" STREQUAL "bl618dg")
sdk_add_static_library(libaacdec_bl618dg.a)
else()
sdk_add_static_library(libaacdec.a)
endif()

sdk_add_include_directories(include)
