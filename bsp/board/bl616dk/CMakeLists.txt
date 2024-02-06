sdk_add_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/fw_header.c)

if(CONFIG_RF)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_rf.c)
endif()

if(CONFIG_LINK_IN_PSRAM)
message("LINK IN PSRAM")
sdk_add_compile_definitions(-DCONFIG_PSRAM_COPY_CODE)
sdk_set_linker_script(bl616_chip_psram.ld)
else()
message("LINK IN FLASH")
sdk_set_linker_script(bl616_flash.ld)
endif()

if(CONFIG_PSRAM)
sdk_add_compile_definitions(-DCONFIG_PSRAM)
endif()
if(CONFIG_CONSOLE_WO)
sdk_add_compile_definitions(-DCONFIG_CONSOLE_WO)
endif()

sdk_add_compile_definitions_ifdef(CONFIG_PSRAM_COPY_CODE -DCONFIG_PSRAM_COPY_CODE)

sdk_add_link_options(-ufw_header)
