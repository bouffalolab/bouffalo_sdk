
# base: bsp/board/bl616dk
set(BOARD_BASE_PATH  ${BL_SDK_BASE}/bsp/board/bl616ldk)

sdk_add_include_directories(.)

# pinmux overlay
# target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio_overlay.c)

# fw tools board directory select
# if(NOT CONFIG_CUSTOMIZE_BOARD_DIR)
#     set(CONFIG_CUSTOMIZE_BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# endif()

# fw tools board config select
# if(NOT CONFIG_CUSTOMIZE_BOARD_CONFIG)
#     set(CONFIG_CUSTOMIZE_BOARD_CONFIG config)
# endif()

# Inherited bl616dk board base
add_subdirectory(${BOARD_BASE_PATH} bl616ldk)

# select custom ld script
if(CONFIG_PSRAM_COPY_CODE)
    message(STATUS "OVERLAY: LINK IN PSRAM")
    sdk_add_compile_definitions(-D CONFIG_PSRAM_COPY_CODE)
    sdk_set_linker_script(bl616l_chip_psram.ld)
elseif(CONFIG_LPAPP)
    message(STATUS "OVERLAY: LINK IN LPAPP")
    sdk_set_linker_script(bl616l_lp.ld)
else()
    message(STATUS "OVERLAY: LINK IN FLASH")
    sdk_set_linker_script(bl616l_flash.ld)
endif()
