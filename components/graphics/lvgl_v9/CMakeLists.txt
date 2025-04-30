sdk_generate_library()

sdk_add_compile_definitions(-D CONFIG_LVGL9)

sdk_add_compile_definitions(-D LV_KCONFIG_IGNORE)
sdk_add_compile_definitions(-D LV_CONF_INCLUDE_SIMPLE)
sdk_add_compile_definitions(-D LV_LVGL_H_INCLUDE_SIMPLE)

file(GLOB_RECURSE SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/display/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/draw/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/font/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/indev/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/layouts/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/osal/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/others/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stdlib/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/themes/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tick/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.c
)
sdk_library_add_sources(${SOURCES})
sdk_library_add_sources(lv_init.c)

if(CONFIG_LVGL9_PORT_DISP)
    sdk_add_compile_definitions(-D CONFIG_LVGL9_PORT_DISP)
    sdk_library_add_sources(port/lv_port_disp_mcu.c)
    sdk_library_add_sources(port/lv_port_disp_rgb.c)
endif()

if(CONFIG_LVGL9_PORT_FS)
    sdk_add_compile_definitions(-D CONFIG_LVGL9_PORT_FS)
    sdk_library_add_sources(port/lv_port_fs.c)
endif()

if(CONFIG_LVGL9_PORT_INDEV)
    sdk_add_compile_definitions(-D CONFIG_LVGL9_PORT_INDEV)
    sdk_library_add_sources(port/lv_port_indev.c)
endif()

sdk_add_include_directories(.)
sdk_add_include_directories(core/)
sdk_add_include_directories(display/)
sdk_add_include_directories(draw/)
sdk_add_include_directories(drivers/)
sdk_add_include_directories(font/)
sdk_add_include_directories(indev/)
sdk_add_include_directories(layouts/)
sdk_add_include_directories(libs/)
sdk_add_include_directories(misc/)
sdk_add_include_directories(osal/)
sdk_add_include_directories(others/)
sdk_add_include_directories(stdlib/)
sdk_add_include_directories(themes/)
sdk_add_include_directories(tick/)
sdk_add_include_directories(widgets/)
sdk_add_include_directories(port/)
