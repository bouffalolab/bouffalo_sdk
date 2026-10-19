
if("${CHIP}" STREQUAL "bl618dg")
sdk_add_static_library(libspeex_bl618dg.a)
else()
sdk_add_static_library(libspeex.a)
endif()

sdk_add_include_directories(include)
