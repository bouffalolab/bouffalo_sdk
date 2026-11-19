/** @file lv_port_disp_default.h
 *  Display port for CONFIG_BSP_DISPLAY's default display.
 */
#ifndef LV_PORT_DISP_DEFAULT_H
#define LV_PORT_DISP_DEFAULT_H

#ifndef CONFIG_LVGL8_PORT_DISP
#error "CONFIG_LVGL8_PORT_DISP is not enabled"
#endif

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

lv_disp_drv_t *lv_port_disp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_PORT_DISP_DEFAULT_H */