sdk_generate_library()

sdk_add_compile_definitions(-D CONFIG_LVGL8)

sdk_add_compile_definitions(-D LV_KCONFIG_IGNORE)
sdk_add_compile_definitions(-D LV_CONF_INCLUDE_SIMPLE)

file(GLOB_RECURSE SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/draw/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/font/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.c
)
sdk_library_add_sources(${SOURCES})

if(CONFIG_LVGL8_PORT_DISP)
    sdk_add_compile_definitions(-D CONFIG_LVGL8_PORT_DISP)
    sdk_library_add_sources(port/lv_port_disp_mcu.c)
    sdk_library_add_sources(port/lv_port_disp_rgb.c)

endif()

if(CONFIG_LVGL8_PORT_FS)
    sdk_add_compile_definitions(-D CONFIG_LVGL8_PORT_FS)
    sdk_library_add_sources(port/lv_port_fs.c)
endif()

if(CONFIG_LVGL8_PORT_INDEV)
    sdk_add_compile_definitions(-D CONFIG_LVGL8_PORT_INDEV)
    sdk_library_add_sources(port/lv_port_indev.c)
endif()

sdk_add_include_directories(.)
sdk_add_include_directories(port/)
