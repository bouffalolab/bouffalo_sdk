#ifndef _BOARD_FLASH_PSRAM_H_
#define _BOARD_FLASH_PSRAM_H_

#if __has_include("board_flash_psram_overlay.h")
#include "board_flash_psram_overlay.h"
#else

#include "stdio.h"

/* flash */
uint32_t board_set_flash_hs(uint8_t flash_clock);
void bl_show_flashinfo(void);

/* psram */
uint32_t board_psram_x8_init(void);
uint32_t board_psram_size_get(void);

#endif
#endif
