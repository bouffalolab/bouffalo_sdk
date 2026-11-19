/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#ifndef BOARD_BL618DG_ORION_H
#define BOARD_BL618DG_ORION_H

#include "bflb_rtc.h"
#include "board_gpio.h"
#include "board_flash_psram.h"

#if IS_ENABLED(CONFIG_COMPAT_V2)
void board_init(void) __attribute__((deprecated("handled by SYSINIT; will be removed in Bouffalo SDK v3")));
#endif /* IS_ENABLED(CONFIG_COMPAT_V2) */
enum bflb_rtc_32k_clk_type board_get_rtc_32k_clk_type(void);

#ifdef CPU_AP
void boot_up_np(void);
void boot_up_lp(uint32_t address);
#endif

#endif
