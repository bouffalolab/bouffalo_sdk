
if("${CHIP}" STREQUAL "bl618dg")
sdk_add_static_library(libopus_bl618dg.a)
else()
sdk_add_static_library(libopus.a)
endif()

sdk_add_include_directories(include)
