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
#include "lv_version.h"

/*********************
 *      INCLUDES
 *********************/
#include "lv_init.h"

#include "stdlib/lv_mem.h"
#include "stdlib/lv_string.h"
#include "stdlib/lv_sprintf.h"

#include "misc/lv_log.h"
#include "misc/lv_timer.h"
#include "misc/lv_math.h"
#include "misc/lv_array.h"
#include "misc/lv_async.h"
#include "misc/lv_anim_timeline.h"
#include "misc/lv_profiler_builtin.h"
#include "misc/lv_rb.h"
#include "misc/lv_utils.h"

#include "tick/lv_tick.h"

#include "core/lv_obj.h"
#include "core/lv_group.h"
#include "indev/lv_indev.h"
#include "core/lv_refr.h"
#include "display/lv_display.h"

#include "font/lv_font.h"
#include "font/lv_binfont_loader.h"
#include "font/lv_font_fmt_txt.h"

#include "widgets/animimage/lv_animimage.h"
#include "widgets/arc/lv_arc.h"
#include "widgets/bar/lv_bar.h"
#include "widgets/button/lv_button.h"
#include "widgets/buttonmatrix/lv_buttonmatrix.h"
#include "widgets/calendar/lv_calendar.h"
#include "widgets/canvas/lv_canvas.h"
#include "widgets/chart/lv_chart.h"
#include "widgets/checkbox/lv_checkbox.h"
#include "widgets/dropdown/lv_dropdown.h"
#include "widgets/image/lv_image.h"
#include "widgets/imagebutton/lv_imagebutton.h"
#include "widgets/keyboard/lv_keyboard.h"
#include "widgets/label/lv_label.h"
#include "widgets/led/lv_led.h"
#include "widgets/line/lv_line.h"
#include "widgets/list/lv_list.h"
#include "widgets/lottie/lv_lottie.h"
#include "widgets/menu/lv_menu.h"
#include "widgets/msgbox/lv_msgbox.h"
#include "widgets/roller/lv_roller.h"
#include "widgets/scale/lv_scale.h"
#include "widgets/slider/lv_slider.h"
#include "widgets/span/lv_span.h"
#include "widgets/spinbox/lv_spinbox.h"
#include "widgets/spinner/lv_spinner.h"
#include "widgets/switch/lv_switch.h"
#include "widgets/table/lv_table.h"
#include "widgets/tabview/lv_tabview.h"
#include "widgets/textarea/lv_textarea.h"
#include "widgets/tileview/lv_tileview.h"
#include "widgets/win/lv_win.h"

#include "others/snapshot/lv_snapshot.h"
#include "others/sysmon/lv_sysmon.h"
#include "others/monkey/lv_monkey.h"
#include "others/gridnav/lv_gridnav.h"
#include "others/fragment/lv_fragment.h"
#include "others/imgfont/lv_imgfont.h"
#include "others/observer/lv_observer.h"
#include "others/ime/lv_ime_pinyin.h"
#include "others/file_explorer/lv_file_explorer.h"

#include "libs/barcode/lv_barcode.h"
#include "libs/bin_decoder/lv_bin_decoder.h"
#include "libs/bmp/lv_bmp.h"
#include "libs/rle/lv_rle.h"
#include "libs/fsdrv/lv_fsdrv.h"
#include "libs/lodepng/lv_lodepng.h"
#include "libs/libpng/lv_libpng.h"
#include "libs/gif/lv_gif.h"
#include "libs/qrcode/lv_qrcode.h"
#include "libs/tjpgd/lv_tjpgd.h"
#include "libs/libjpeg_turbo/lv_libjpeg_turbo.h"
#include "libs/freetype/lv_freetype.h"
#include "libs/rlottie/lv_rlottie.h"
#include "libs/ffmpeg/lv_ffmpeg.h"
#include "libs/tiny_ttf/lv_tiny_ttf.h"

#include "layouts/lv_layout.h"

#include "draw/lv_draw.h"
#include "draw/lv_draw_buf.h"
#include "draw/lv_draw_vector.h"
#include "draw/sw/lv_draw_sw.h"

#include "themes/lv_theme.h"

#include "drivers/lv_drivers.h"

#include "lv_api_map_v8.h"
#include "lv_api_map_v9_0.h"
#include "lv_api_map_v9_1.h"

#if LV_USE_PRIVATE_API
#include "lvgl_private.h"
#endif

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
#define LV_VERSION_CHECK(x,y,z) (x == LVGL_VERSION_MAJOR && (y < LVGL_VERSION_MINOR || (y == LVGL_VERSION_MINOR && z <= LVGL_VERSION_PATCH)))

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

static inline const char * lv_version_info(void)
{
    return LVGL_VERSION_INFO;
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_H*/
