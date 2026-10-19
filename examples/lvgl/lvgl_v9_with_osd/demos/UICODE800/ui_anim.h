#ifndef _UI_ANIM_H
#define _UI_ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Start the looping overlay animations: the side-icon green cycle and the
 * vertical slider sweep. Call once from ui_init() after the screen is loaded.
 * (The scrolling marquee label is animated by LVGL itself, so it needs no
 * driver here.) */
void ui_anim_start(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* _UI_ANIM_H */
