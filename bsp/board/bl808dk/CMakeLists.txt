sdk_add_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)

if(CONFIG_IOT)
# target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/iot/fw_header.c)
sdk_set_linker_script(iot/bl808_flash_${CPU_ID}.ld)
# sdk_add_link_options(-ufw_header)
else()
# target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mcu/fw_header.c)
sdk_set_linker_script(mcu/bl808_flash_${CPU_ID}.ld)
# sdk_add_link_options(-ufw_header0)
# sdk_add_link_options(-ufw_header1)
endif()

if(CONFIG_PSRAM)
sdk_add_compile_definitions(-DCONFIG_PSRAM)
endif()