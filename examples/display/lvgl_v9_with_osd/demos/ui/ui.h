#ifndef OSD_UI_H
#define OSD_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

LV_IMAGE_DECLARE(ui_img_sound_png);
LV_IMAGE_DECLARE(ui_img_stop_png);
LV_IMAGE_DECLARE(ui_img_fastforward_png);
LV_IMAGE_DECLARE(ui_img_bluetooth_png);
LV_IMAGE_DECLARE(ui_img_wifi_png);
LV_IMAGE_DECLARE(ui_img_comment_png);
LV_IMAGE_DECLARE(ui_img_speed_png);

void ui_init(void);
void ui_destroy(void);
/* Call from the LVGL task only. NULL hides the OTA message. */
void ui_set_ota_status(const char *text);

#ifdef __cplusplus
}
#endif

#endif
