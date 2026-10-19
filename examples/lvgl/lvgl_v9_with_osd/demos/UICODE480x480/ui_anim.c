// UICODE480x480 (480x480 (ST7701P DPI)) Custom animations: right-side icon green cycle + slider sweep
// Adapted from UICODE800, icon IDs Image5/6/7/8 = bt/wifi/comment/speed

#include "ui.h"
#include "ui_anim.h"

/* Green recolor applied to PNG icon (full-cover recolor = solid green tint) */
#define ICON_GREEN          lv_color_hex(0x00C853)
#define ICON_GREEN_MS       1000 /* how long each icon stays green */

/* The four icons to cycle through, in order (right-side column): Image5(bt), Image6(wifi), Image7(comment), Image8(speed) */
static lv_obj_t **icon_seq[] = {
    &ui_Image5, /* bluetooth */
    &ui_Image6, /* wifi */
    &ui_Image7, /* comment */
    &ui_Image8, /* speed */
};
#define ICON_COUNT (sizeof(icon_seq) / sizeof(icon_seq[0]))

static void icon_set_green(lv_obj_t *icon, bool on)
{
    if (icon == NULL) {
        return;
    }
    lv_obj_set_style_image_recolor(icon, ICON_GREEN, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor_opa(icon, on ? LV_OPA_COVER : LV_OPA_TRANSP,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
}

/* Timer callback: every ICON_GREEN_MS, turn off previous icon and light up next */
static void icon_cycle_timer_cb(lv_timer_t *t)
{
    (void)t;
    static uint32_t idx = 0;

    icon_set_green(*icon_seq[idx], false);
    idx = (idx + 1) % ICON_COUNT;
    icon_set_green(*icon_seq[idx], true);
}

/* --- Slider sweep ---
 * Driven by a low-frequency timer to avoid per-frame CPU overhead and PSRAM bandwidth */
#define SLIDER_REFRESH_MS   200 /* ms between slider updates (higher = less CPU) */
#define SLIDER_SWEEP_STEPS  60  /* steps for one min->max sweep */

typedef struct {
    lv_obj_t **ref;
    int32_t    step;
    int8_t     dir;  /* +1 rising, -1 falling */
} slider_sweep_t;

static void slider_sweep_timer_cb(lv_timer_t *t)
{
    slider_sweep_t *s = lv_timer_get_user_data(t);
    lv_obj_t *sl = *s->ref;
    if (sl == NULL) {
        return;
    }
    int32_t min = lv_slider_get_min_value(sl);
    int32_t max = lv_slider_get_max_value(sl);
    int32_t val = min + (int32_t)(((int64_t)(max - min) * s->step) / SLIDER_SWEEP_STEPS);
    lv_slider_set_value(sl, val, LV_ANIM_OFF);

    s->step += s->dir;
    if (s->step >= SLIDER_SWEEP_STEPS) {
        s->step = SLIDER_SWEEP_STEPS;
        s->dir = -1;
    } else if (s->step <= 0) {
        s->step = 0;
        s->dir = +1;
    }
}

static void slider_sweep_start(lv_obj_t **slider_ref, int32_t phase_step)
{
    static slider_sweep_t states[1];
    static uint32_t n = 0;
    if (n >= 1) {
        return;
    }
    slider_sweep_t *s = &states[n++];
    s->ref = slider_ref;
    s->step = phase_step % (SLIDER_SWEEP_STEPS + 1);
    s->dir = +1;
    lv_timer_create(slider_sweep_timer_cb, SLIDER_REFRESH_MS, s);
}

void ui_anim_start(void)
{
    /* Init: restore all icons to original color, then light the first one */
    for (uint32_t i = 0; i < ICON_COUNT; i++) {
        icon_set_green(*icon_seq[i], false);
    }
    icon_set_green(*icon_seq[0], true);
    lv_timer_create(icon_cycle_timer_cb, ICON_GREEN_MS, NULL);

    /* Sweep slider min->max->min via low-rate timer */
    slider_sweep_start(&ui_Slider1, 0);
}