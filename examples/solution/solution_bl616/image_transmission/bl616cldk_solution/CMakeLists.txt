
# base: bsp/board/bl616dk
set(BOARD_BASE_PATH  ${BL_SDK_BASE}/bsp/board/bl616cldk)

sdk_add_include_directories(.)

# pinmux overlay
# target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio_overlay.c)

# Inherited bl616dk board base
add_subdirectory(${BOARD_BASE_PATH} bl616cldk)
