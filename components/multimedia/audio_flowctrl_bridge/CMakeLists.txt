
if("${CHIP}" STREQUAL "bl618dg")
sdk_add_static_library(libaudio_flowctrl_bridge_bl618dg.a)
else()
sdk_add_static_library(libaudio_flowctrl_bridge.a)
endif()

sdk_add_include_directories(include)
