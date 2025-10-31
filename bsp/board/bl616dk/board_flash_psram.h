#ifndef _BOARD_FLASH_PSRAM_H_
#define _BOARD_FLASH_PSRAM_H_

#if __has_include("board_flash_psram_overlay.h")
#include "board_flash_psram_overlay.h"
#else

#include "stdio.h"

/* flash */
void board_set_flash_80m(void);
void bl_show_flashinfo(void);

/* psram */
uint32_t board_psram_x8_init(void);

#endif
#endif
