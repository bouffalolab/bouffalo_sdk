/**
 * @file display_default_module.h
 * @brief The panel module of the default display, selected by the Kconfig choices of default/Kconfig: DEFAULT_MODULE,
 *        a pointer to the module description, and the resolution of the screen: DISPLAY_DEFAULT_WIDTH / HEIGHT.
 *
 * Included by display_default_gram.c, display_default_video.c and (video) display_default.h. Two parts:
 * - GRAM: a screen with frame memory (SPI / DBI / QSPI), the choice DISPLAY_DEFAULT_MODULE; it gives the resolution
 *   (DISPLAY_DEFAULT_GRAM_WIDTH / HEIGHT);
 * - VIDEO: a video screen (MIPI DSI / RGB), the choice DISPLAY_DEFAULT_VIDEO_MODULE; it also gives the size of the
 *   frames (DISPLAY_DEFAULT_VIDEO_WIDTH / HEIGHT), written next to the modules: the host test
 *   tests/host/test_default_video.c checks them against the modules.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_DEFAULT_MODULE_H
#define DISPLAY_DEFAULT_MODULE_H

#if defined(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM)
/* ---------------------------------------------------------------------------------------------- GRAM */
/* DISPLAY_DEFAULT_GRAM_WIDTH / HEIGHT: the resolution of the module at rotation 0, in pixels; not defined for the
 * custom module (the project defines it) */
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7789_240X320) || defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7789_240X240)
#include "panel/gram_st7789/display_st7789.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7796_320X480)
#include "panel/gram_st7796/display_st7796.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ILI9341_240X320)
#include "panel/gram_ili9341/display_ili9341.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ILI9488_320X480)
#include "panel/gram_ili9488/display_ili9488.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_GC9A01_240X240)
#include "panel/gram_gc9a01/display_gc9a01.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_NT35510_480X800)
#include "panel/gram_nt35510/display_nt35510.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_GC9307_240X320)
#include "panel/gram_gc9307/display_gc9307.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ILI9486_320X480)
#include "panel/gram_ili9486/display_ili9486.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_JD9853_240X320)
#include "panel/gram_jd9853/display_jd9853.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7735_240X320)
#include "panel/gram_st7735/display_st7735.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST77922_320X480) || defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST77926_320X480)
#include "panel/gram_st77922/display_st77922.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_SPD2010_412X412)
#include "panel/gram_spd2010/display_spd2010.h"
#endif

#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7789_240X320)
#define DEFAULT_MODULE              &display_module_st7789_240x320
#define DISPLAY_DEFAULT_GRAM_WIDTH  240
#define DISPLAY_DEFAULT_GRAM_HEIGHT 320
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7789_240X240)
#define DEFAULT_MODULE              &display_module_st7789_240x240
#define DISPLAY_DEFAULT_GRAM_WIDTH  240
#define DISPLAY_DEFAULT_GRAM_HEIGHT 240
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7796_320X480)
#define DEFAULT_MODULE              &display_module_st7796_320x480
#define DISPLAY_DEFAULT_GRAM_WIDTH  320
#define DISPLAY_DEFAULT_GRAM_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_ILI9341_240X320)
#define DEFAULT_MODULE              &display_module_ili9341_240x320
#define DISPLAY_DEFAULT_GRAM_WIDTH  240
#define DISPLAY_DEFAULT_GRAM_HEIGHT 320
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_ILI9488_320X480)
#define DEFAULT_MODULE              &display_module_ili9488_320x480
#define DISPLAY_DEFAULT_GRAM_WIDTH  320
#define DISPLAY_DEFAULT_GRAM_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_GC9A01_240X240)
#define DEFAULT_MODULE              &display_module_gc9a01_240x240
#define DISPLAY_DEFAULT_GRAM_WIDTH  240
#define DISPLAY_DEFAULT_GRAM_HEIGHT 240
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_NT35510_480X800)
#define DEFAULT_MODULE              &display_module_nt35510_480x800
#define DISPLAY_DEFAULT_GRAM_WIDTH  480
#define DISPLAY_DEFAULT_GRAM_HEIGHT 800
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_GC9307_240X320)
#define DEFAULT_MODULE              &display_module_gc9307_240x320
#define DISPLAY_DEFAULT_GRAM_WIDTH  240
#define DISPLAY_DEFAULT_GRAM_HEIGHT 320
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_ILI9486_320X480)
#define DEFAULT_MODULE              &display_module_ili9486_320x480
#define DISPLAY_DEFAULT_GRAM_WIDTH  320
#define DISPLAY_DEFAULT_GRAM_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_JD9853_240X320)
#define DEFAULT_MODULE              &display_module_jd9853_240x320
#define DISPLAY_DEFAULT_GRAM_WIDTH  240
#define DISPLAY_DEFAULT_GRAM_HEIGHT 320
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7735_240X320)
#define DEFAULT_MODULE              &display_module_st7735_240x320
#define DISPLAY_DEFAULT_GRAM_WIDTH  240
#define DISPLAY_DEFAULT_GRAM_HEIGHT 320
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST77922_320X480)
#define DEFAULT_MODULE              &display_module_st77922_320x480
#define DISPLAY_DEFAULT_GRAM_WIDTH  320
#define DISPLAY_DEFAULT_GRAM_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST77926_320X480)
#define DEFAULT_MODULE              &display_module_st77926_320x480
#define DISPLAY_DEFAULT_GRAM_WIDTH  320
#define DISPLAY_DEFAULT_GRAM_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_SPD2010_412X412)
#define DEFAULT_MODULE              &display_module_spd2010_412x412
#define DISPLAY_DEFAULT_GRAM_WIDTH  412
#define DISPLAY_DEFAULT_GRAM_HEIGHT 412
#elif defined(CONFIG_DISPLAY_DEFAULT_MODULE_CUSTOM)
#include "panel/dcs/display_panel_dcs.h"
extern const display_dcs_module_t display_default_custom_module;
#define DEFAULT_MODULE &display_default_custom_module
#else
#error "select a panel module in default/Kconfig"
#endif

#elif defined(CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO)
/* ---------------------------------------------------------------------------------------------- VIDEO */
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_EK79007_2LANE) || defined(CONFIG_DISPLAY_DEFAULT_VIDEO_EK79007_4LANE)
#include "panel/video_ek79007/display_ek79007.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_AXS15231B_172X640)
#include "panel/video_axs15231b/display_axs15231b.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_AXS15231E_258X960)
#include "panel/video_axs15231e/display_axs15231e.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FL7707N_720X720)
#include "panel/video_fl7707n/display_fl7707n.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9806E_480X854)
#include "panel/video_ili9806e/display_ili9806e.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9881C_KD050020) || defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9881C_KD050023W4)
#include "panel/video_ili9881c/display_ili9881c.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_JD9365TX_720X1280)
#include "panel/video_jd9365tx/display_jd9365tx.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7102_KD027HVF) || defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7102_YH494)
#include "panel/video_st7102/display_st7102.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701S_DSI_480X480) || \
    defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701S_RGB_480X480)
#include "panel/video_st7701s/display_st7701s.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_GC9503V_480X480)
#include "panel/video_gc9503v/display_gc9503v.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701P_480X480)
#include "panel/video_st7701p/display_st7701p.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_JD9165BA_1024X600)
#include "panel/video_jd9165ba/display_jd9165ba.h"
#endif
#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_800X480) || defined(CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_1024X600)
#include "panel/video_standard/display_video_standard.h"
#endif

#if defined(CONFIG_DISPLAY_DEFAULT_VIDEO_EK79007_2LANE)
#define DEFAULT_MODULE               &display_module_ek79007_1024x600_2lane
#define DISPLAY_DEFAULT_VIDEO_WIDTH  1024
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 600
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_EK79007_4LANE)
#define DEFAULT_MODULE               &display_module_ek79007_1024x600_4lane
#define DISPLAY_DEFAULT_VIDEO_WIDTH  1024
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 600
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_AXS15231B_172X640)
#define DEFAULT_MODULE               &display_module_axs15231b_172x640
#define DISPLAY_DEFAULT_VIDEO_WIDTH  172
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 640
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_AXS15231E_258X960)
#define DEFAULT_MODULE               &display_module_axs15231e_258x960
#define DISPLAY_DEFAULT_VIDEO_WIDTH  260
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 960
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FL7707N_720X720)
#define DEFAULT_MODULE               &display_module_fl7707n_720x720
#define DISPLAY_DEFAULT_VIDEO_WIDTH  720
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 720
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9806E_480X854)
#define DEFAULT_MODULE               &display_module_ili9806e_480x854
#define DISPLAY_DEFAULT_VIDEO_WIDTH  480
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 854
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9881C_KD050020)
#define DEFAULT_MODULE               &display_module_ili9881c_720x1280_kd050020
#define DISPLAY_DEFAULT_VIDEO_WIDTH  720
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 1280
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9881C_KD050023W4)
#define DEFAULT_MODULE               &display_module_ili9881c_720x1280_kd050023w4
#define DISPLAY_DEFAULT_VIDEO_WIDTH  720
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 1280
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_JD9365TX_720X1280)
#define DEFAULT_MODULE               &display_module_jd9365tx_720x1280
#define DISPLAY_DEFAULT_VIDEO_WIDTH  720
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 1280
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7102_KD027HVF)
#define DEFAULT_MODULE               &display_module_st7102_320x320_kd027hvf
#define DISPLAY_DEFAULT_VIDEO_WIDTH  320
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 320
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7102_YH494)
#define DEFAULT_MODULE               &display_module_st7102_480x960_yh494
#define DISPLAY_DEFAULT_VIDEO_WIDTH  480
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 960
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701S_DSI_480X480)
#define DEFAULT_MODULE               &display_module_st7701s_dsi_480x480
#define DISPLAY_DEFAULT_VIDEO_WIDTH  480
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_GC9503V_480X480)
#define DEFAULT_MODULE               &display_module_gc9503v_480x480
#define DISPLAY_DEFAULT_VIDEO_WIDTH  480
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701P_480X480)
#define DEFAULT_MODULE               &display_module_st7701p_480x480
#define DISPLAY_DEFAULT_VIDEO_WIDTH  480
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701S_RGB_480X480)
#define DEFAULT_MODULE               &display_module_st7701s_rgb_480x480
#define DISPLAY_DEFAULT_VIDEO_WIDTH  480
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_JD9165BA_1024X600)
#define DEFAULT_MODULE               &display_module_jd9165ba_1024x600
#define DISPLAY_DEFAULT_VIDEO_WIDTH  1024
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 600
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_800X480)
#define DEFAULT_MODULE               &display_module_rgb_800x480
#define DISPLAY_DEFAULT_VIDEO_WIDTH  800
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 480
#elif defined(CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_1024X600)
#define DEFAULT_MODULE               &display_module_rgb_1024x600
#define DISPLAY_DEFAULT_VIDEO_WIDTH  1024
#define DISPLAY_DEFAULT_VIDEO_HEIGHT 600
#else
#error "select a video panel in default/Kconfig"
#endif

#elif defined(CONFIG_DISPLAY_DEFAULT)
#error "select the type of the default display in default/Kconfig"
#endif /* GRAM / VIDEO */

/* The resolution of the default display, whatever its type (not defined for the custom GRAM module) */
#if defined(DISPLAY_DEFAULT_GRAM_WIDTH)
#define DISPLAY_DEFAULT_WIDTH  DISPLAY_DEFAULT_GRAM_WIDTH
#define DISPLAY_DEFAULT_HEIGHT DISPLAY_DEFAULT_GRAM_HEIGHT
#elif defined(DISPLAY_DEFAULT_VIDEO_WIDTH)
#define DISPLAY_DEFAULT_WIDTH  DISPLAY_DEFAULT_VIDEO_WIDTH
#define DISPLAY_DEFAULT_HEIGHT DISPLAY_DEFAULT_VIDEO_HEIGHT
#endif

#endif /* DISPLAY_DEFAULT_MODULE_H */
