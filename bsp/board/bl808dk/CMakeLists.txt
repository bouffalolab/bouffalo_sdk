sdk_add_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/fw_header.c)
sdk_set_linker_script(bl808_flash_${CPU_ID}.ld)
sdk_add_link_options(-ufw_header)

if(CONFIG_PSRAM)
sdk_add_compile_definitions(-DCONFIG_PSRAM)
endif()