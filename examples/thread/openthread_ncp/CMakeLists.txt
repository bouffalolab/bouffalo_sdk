cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

string(TOUPPER "${CHIP}" upper_case_chip)
string(TOLOWER "${CHIP}" lower_case_chip)
sdk_add_compile_definitions(-DOPENTHREAD_CONFIG_PLATFORM_INFO=\"${upper_case_chip}\")

sdk_add_compile_definitions(-DOT_SERIAL_BUFFER_SIZE=1024)
sdk_add_compile_definitions(-DOT_SERIAL_LINE_SIZE=256)
sdk_add_compile_definitions(-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"openthread-core-proj-config.h\")
sdk_add_compile_definitions(-DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE=\"openthread-core-platform-config-check.h\")

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

target_sources(app PRIVATE board_${lower_case_chip}.c)

sdk_set_main_file(main.c)

project(openthread_ncp)
