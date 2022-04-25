set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_LIBS usb_stack tflite)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/tflite_camera_psram.ld)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/usb ${BSP_COMMON_DIR}/psram ${BSP_COMMON_DIR}/image_sensor ${BSP_COMMON_DIR}/mcu_lcd ${CMAKE_CURRENT_SOURCE_DIR}/vww)
file(GLOB_RECURSE ref
"${CMAKE_CURRENT_SOURCE_DIR}/vww/*.cc"
)
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/usb/usb_dc.c ${BSP_COMMON_DIR}/psram/bsp_sf_psram.c ${BSP_COMMON_DIR}/image_sensor/bsp_image_sensor.c ${BSP_COMMON_DIR}/mcu_lcd/mcu_lcd.c ${BSP_COMMON_DIR}/mcu_lcd/font.c ${BSP_COMMON_DIR}/mcu_lcd/ili9341.c ${ref})
set(mains main.c)
generate_bin()


