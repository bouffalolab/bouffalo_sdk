sdk_add_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_flash_psram.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/fw_header.c)

if(CONFIG_GCC_COMPILE_LTO)
    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/board_flash_psram.c
        ${CMAKE_CURRENT_SOURCE_DIR}/board.c
        TARGET_DIRECTORY app
        PROPERTIES COMPILE_FLAGS "-fno-lto"
    )
endif()

if(CONFIG_RF)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_rf.c)
endif()

if(CONFIG_CUSTOM_LINKSCRIPT)
message(STATUS "LINK IN CUSTOM_LINKSCRIPT")
else()
if("${CPU_ID}" STREQUAL "lp")
if("${CPU_MODEL}" STREQUAL "b0")
message(STATUS "LINK IN bl618dg_ram_lp_b0.ld")
sdk_set_linker_script(bl618dg_ram_lp_b0.ld)
sdk_add_link_options(-Wl,--no-relax)
endif()
else()
message(STATUS "LINK IN bl618dg_common.ld")
sdk_set_linker_script_macro(bl618dg_common.ld.in)
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
