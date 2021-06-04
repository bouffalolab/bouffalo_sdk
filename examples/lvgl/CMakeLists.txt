set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_LIBS fatfs lvgl)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/lvgl_flash.ld)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/il9341 ${BSP_COMMON_DIR}/lvgl ${BSP_COMMON_DIR}/touch ${BSP_COMMON_DIR}/spi_sd)
file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.c")
set(TARGET_REQUIRED_SRCS ${sources} ${BSP_COMMON_DIR}/il9341/bsp_il9341.c ${BSP_COMMON_DIR}/lvgl/lv_port_disp.c ${BSP_COMMON_DIR}/lvgl/lv_port_indev.c ${BSP_COMMON_DIR}/lvgl/lv_port_fs.c 
${BSP_COMMON_DIR}/touch/xpt2046.c ${BSP_COMMON_DIR}/touch/touch.c ${BSP_COMMON_DIR}/fatfs/fatfs_spi_sd.c ${BSP_COMMON_DIR}/spi_sd/bsp_spi_sd.c)
set(mains main.c)
generate_bin()


