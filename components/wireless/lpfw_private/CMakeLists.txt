
if("${CHIP}" STREQUAL "bl618dg")
sdk_add_static_library(liblpfw_private_bl618dg.a)
else()
sdk_add_static_library(liblpfw_private.a)
endif()

