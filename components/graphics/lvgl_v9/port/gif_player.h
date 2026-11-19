#ifndef LV_GIF_PLAYER_H
#define LV_GIF_PLAYER_H

#include "lvgl.h"

#ifndef GIF_PLAYER_LVGL_RGB565A8
#define GIF_PLAYER_LVGL_RGB565A8 0
#endif

/* Uses the display initialized by the normal LVGL port. */
#if LV_USE_GIF
bool gif_player_start(lv_obj_t * parent, const lv_image_dsc_t * source, const char * name);
void gif_player_stop(void);
#endif /* LV_USE_GIF */

#endif
