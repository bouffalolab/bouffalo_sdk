sdk_add_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_flash_psram.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/fw_header.c)

if(CONFIG_RF)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_rf.c)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wireless_regdb.c)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wireless_spur.c)
endif()

if(CONFIG_PSRAM_COPY_CODE)
    message(STATUS "LINK IN PSRAM")
    sdk_set_linker_script(bl616_chip_psram.ld)
elseif(CONFIG_LPAPP)
    message(STATUS "LINK IN LPAPP")
    sdk_set_linker_script(bl616_lp.ld)
elseif(CONFIG_WFA)
    message(STATUS "LINK IN WFA")
    sdk_set_linker_script(bl616_wfa_flash.ld)
    sdk_add_link_options(
    -L ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    message(STATUS "LINK IN FLASH")
    sdk_set_linker_script(bl616_flash.ld)
endif()

sdk_add_link_options(-ufw_header)

# fw tools board directory select
if(CONFIG_CUSTOMIZE_BOARD_DIR)
    set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_DIR ${CONFIG_CUSTOMIZE_BOARD_DIR})
else()
    set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_CONFIG config)
