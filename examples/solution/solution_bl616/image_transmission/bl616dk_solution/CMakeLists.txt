
# base: bsp/board/bl616dk
set(BOARD_BASE_PATH  ${BL_SDK_BASE}/bsp/board/bl616dk)

sdk_add_include_directories(.)

# pinmux overlay
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio_overlay.c)

# fw tools board directory select
# if(NOT CONFIG_CUSTOMIZE_BOARD_DIR)
#     set(CONFIG_CUSTOMIZE_BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# endif()

# fw tools board config select
# if(NOT CONFIG_CUSTOMIZE_BOARD_CONFIG)
#     set(CONFIG_CUSTOMIZE_BOARD_CONFIG config)
# endif()

# Inherited bl616dk board base
add_subdirectory(${BOARD_BASE_PATH} bl616dk)
