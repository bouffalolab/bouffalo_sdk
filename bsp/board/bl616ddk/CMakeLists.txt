sdk_add_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_flash_psram.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/fw_header.c)

if(CONFIG_RF)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_rf.c)
endif()

if ("${CPU_MODEL}" STREQUAL "a0")
if("${CPU_ID}" STREQUAL "ap")
sdk_set_linker_script_macro(bl616d_common.ld.in)
elseif("${CPU_ID}" STREQUAL "np")
sdk_set_linker_script_macro(bl616d_common.ld.in)
elseif("${CPU_ID}" STREQUAL "lp")
endif()
else()
if("${CPU_ID}" STREQUAL "ap")
sdk_set_linker_script_macro(bl616d_ram_ap_b0.ld)
endif()
endif()

sdk_add_link_options(-ufw_header)

# fw tools board directory select
if(CONFIG_CUSTOMIZE_BOARD_DIR)
    set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_DIR ${CONFIG_CUSTOMIZE_BOARD_DIR})
else()
    set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_CONFIG config)
