cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DWL_NIC=0)

get_target_property(SDK_INTF_LIB_LINK_OPTIONS sdk_intf_lib INTERFACE_LINK_OPTIONS)
list(REMOVE_ITEM SDK_INTF_LIB_LINK_OPTIONS -nostartfiles)
if(CONFIG_NEWLIB_STANDARD)
    list(REMOVE_ITEM SDK_INTF_LIB_LINK_OPTIONS --specs=nano.specs)
endif()

set_target_properties(sdk_intf_lib PROPERTIES INTERFACE_LINK_OPTIONS "")
foreach(item ${SDK_INTF_LIB_LINK_OPTIONS})
    sdk_add_link_options(${item})
endforeach()

sdk_add_include_directories(.)

target_sources(app PRIVATE otbr_cmd.c)

sdk_set_main_file(main.c)

project(openthread_eth_br)
