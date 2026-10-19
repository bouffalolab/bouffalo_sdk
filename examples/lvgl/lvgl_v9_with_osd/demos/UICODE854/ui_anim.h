// Custom (non-SquareLine) animations for the UICODE854 overlay. See ui_anim.c.
#ifndef _UI_ANIM_H
#define _UI_ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Start the looping overlay animations (side-icon green cycle + slider sweep).
 * Call once after the screen has been built/loaded (end of ui_init()). */
void ui_anim_start(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* _UI_ANIM_H */
