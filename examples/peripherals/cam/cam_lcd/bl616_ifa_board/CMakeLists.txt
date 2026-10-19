# base: bsp/board/bl616dk
set(BOARD_BASE_PATH ${BL_SDK_BASE}/bsp/board/bl616dk)

sdk_add_include_directories(.)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio_overlay.c)

# Inherit the default bl616dk board implementation.
add_subdirectory(${BOARD_BASE_PATH} bl616dk)
