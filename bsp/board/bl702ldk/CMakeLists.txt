sdk_add_include_directories(.)

if(NOT CONFIG_SYSINIT_CUSTOM_BOARD_INIT)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)
endif()
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/fw_header.c)

sdk_add_compile_definitions_ifdef(CONFIG_BLUETOOTH -DCFG_BLUETOOTH_ENABLED)
sdk_add_compile_definitions_ifdef(CONFIG_LMAC154 -DCFG_M154_ENABLED)

sdk_set_linker_script(bl702l_flash.ld)

sdk_add_link_options(-ufw_header)
