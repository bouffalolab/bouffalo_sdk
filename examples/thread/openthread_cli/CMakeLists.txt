cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

string(TOLOWER "${CHIP}" lower_case_chip)
string(TOUPPER "${CHIP}" upper_case_chip)

sdk_add_compile_definitions(-DOPENTHREAD_CONFIG_PLATFORM_INFO=\"${upper_case_chip}\")

if ("${CONFIG_LMAC154_DBG}")
    sdk_add_compile_definitions(-DAPP_PRINT=lmac154_dbg_trace)
else()
    sdk_add_compile_definitions(-DAPP_PRINT=printf)
endif()
sdk_add_compile_definitions(-DWL_NIC=0)

if ("${OT_SERIAL}" STREQUAL "SYS_CLI")
    sdk_add_compile_definitions(-DOT_SERIAL_SHELL)
elseif ("${OT_SERIAL}" STREQUAL "UART") 
    sdk_add_compile_definitions(-DOT_SERIAL_UART)
endif()

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

sdk_set_main_file(main.c)

if (CONFIG_SHELL) 
    target_sources(app PRIVATE app_cli.c)
endif()

project(openthread_cli)
