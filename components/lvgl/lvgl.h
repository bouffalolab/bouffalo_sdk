/**
 * @file lvgl.h
 * Include all LVGL related headers
 */

#ifndef LVGL_H
#define LVGL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************
 * CURRENT VERSION OF LVGL
 ***************************/
#define LVGL_VERSION_MAJOR 8
#define LVGL_VERSION_MINOR 3
#define LVGL_VERSION_PATCH 1
#define LVGL_VERSION_INFO  ""

/*********************
 *      INCLUDES
 *********************/

#include "misc/lv_log.h"
#include "misc/lv_timer.h"
#include "misc/lv_math.h"
#include "misc/lv_mem.h"
#include "misc/lv_async.h"
#include "misc/lv_anim_timeline.h"
#include "misc/lv_printf.h"

#include "hal/lv_hal.h"

#include "core/lv_obj.h"
#include "core/lv_group.h"
#include "core/lv_indev.h"
#include "core/lv_refr.h"
#include "core/lv_disp.h"
#include "core/lv_theme.h"

#include "font/lv_font.h"
#include "font/lv_font_loader.h"
#include "font/lv_font_fmt_txt.h"

#include "widgets/lv_arc.h"
#include "widgets/lv_btn.h"
#include "widgets/lv_img.h"
#include "widgets/lv_label.h"
#include "widgets/lv_line.h"
#include "widgets/lv_table.h"
#include "widgets/lv_checkbox.h"
#include "widgets/lv_bar.h"
#include "widgets/lv_slider.h"
#include "widgets/lv_btnmatrix.h"
#include "widgets/lv_dropdown.h"
#include "widgets/lv_roller.h"
#include "widgets/lv_textarea.h"
#include "widgets/lv_canvas.h"
#include "widgets/lv_switch.h"

#include "draw/lv_draw.h"

#include "lv_api_map.h"

/*-----------------
 * EXTRAS
 *----------------*/
#include "extra/lv_extra.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/** Gives 1 if the x.y.z version is supported in the current version
 * Usage:
 *
 * - Require v6
 * #if LV_VERSION_CHECK(6,0,0)
 *   new_func_in_v6();
 * #endif
 *
 *
 * - Require at least v5.3
 * #if LV_VERSION_CHECK(5,3,0)
 *   new_feature_from_v5_3();
 * #endif
 *
 *
 * - Require v5.3.2 bugfixes
 * #if LV_VERSION_CHECK(5,3,2)
 *   bugfix_in_v5_3_2();
 * #endif
 *
 */
#define LV_VERSION_CHECK(x, y, z) (x == LVGL_VERSION_MAJOR && (y < LVGL_VERSION_MINOR || (y == LVGL_VERSION_MINOR && z <= LVGL_VERSION_PATCH)))

/**
 * Wrapper functions for VERSION macros
 */

static inline int lv_version_major(void)
{
    return LVGL_VERSION_MAJOR;
}

static inline int lv_version_minor(void)
{
    return LVGL_VERSION_MINOR;
}

static inline int lv_version_patch(void)
{
    return LVGL_VERSION_PATCH;
}

static inline const char *lv_version_info(void)
{
    return LVGL_VERSION_INFO;
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_H*/
