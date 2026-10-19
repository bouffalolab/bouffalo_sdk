#ifndef _UI_ANIM_H
#define _UI_ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Start the looping UI animations: the side-icon green cycle and the vertical slider sweep.
 * Call once from ui_init(). (The scrolling marquee is animated by LVGL itself.) */
void ui_anim_start(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* _UI_ANIM_H */
