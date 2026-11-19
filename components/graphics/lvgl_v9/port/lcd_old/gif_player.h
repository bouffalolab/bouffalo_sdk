#ifndef LV_GIF_PLAYER_H
#define LV_GIF_PLAYER_H

#include "lvgl.h"

#define GIF_PLAYER_RGB555A1_OSD 1
#define GIF_PLAYER_RGB565_DIRECT 2
#define GIF_PLAYER_LVGL 3

#ifndef GIF_PLAYER_OUTPUT
#define GIF_PLAYER_OUTPUT GIF_PLAYER_LVGL
#endif

#ifndef GIF_PLAYER_LVGL_RGB565A8
#define GIF_PLAYER_LVGL_RGB565A8 0
#endif

/* Hardware modes own the panel and cannot share its layer with another player.
 * LVGL mode uses the display initialized by the normal LVGL port. */
#if LV_USE_GIF
bool gif_player_start(lv_obj_t * parent, const lv_image_dsc_t * source, const char * name);
void gif_player_stop(void);
#endif /* LV_USE_GIF */

#endif
