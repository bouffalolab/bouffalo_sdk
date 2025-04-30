sdk_add_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/fw_header.c)

sdk_add_compile_definitions_ifdef(CONFIG_BLUETOOTH -DCFG_BLUETOOTH_ENABLED)
sdk_add_compile_definitions_ifdef(CONFIG_LMAC154 -DCFG_M154_ENABLED)

if(CONFIG_PM)
sdk_set_linker_script(bl702_flash_lp.ld)
else()
sdk_set_linker_script(bl702_flash.ld)
endif()

if(CONFIG_PSRAM)
sdk_add_compile_definitions(-DCONFIG_PSRAM)
endif()

sdk_add_link_options(-ufw_header)