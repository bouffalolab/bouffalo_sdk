
if("${CHIP}" STREQUAL "bl618dg")
sdk_add_static_library(libamrnb_bl618dg.a)
else()
sdk_add_static_library(libamrnb.a)
endif()

sdk_add_include_directories(include)
