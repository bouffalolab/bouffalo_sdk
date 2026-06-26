// Custom (non-SquareLine) animations for the UICODE720 overlay.
//
// Driven from ui_init() via ui_anim_start(). Kept in a separate file so the
// SquareLine-generated sources stay untouched and re-exportable.
//
//   - the four side icons (wifi, bluetooth, comment, speed) light up GREEN one
//     after another, each staying green for 1 s, then return to their original
//     look; the cycle repeats forever.
//   - the two sliders sweep from their minimum to their maximum value and back,
//     forever.

#include "ui.h"
#include "ui_anim.h"

/* Green recolor applied to a PNG icon (full-cover recolor = solid green tint). */
#define ICON_GREEN          lv_color_hex(0x00C853)
#define ICON_GREEN_MS       1000 /* how long each icon stays green */

/* The four icons to cycle through, in order. */
static lv_obj_t **icon_seq[] = {
    &ui_Image5, /* wifi */
    &ui_Image6, /* bluetooth */
    &ui_Image7, /* comment */
    &ui_Image8, /* speed */
};
#define ICON_COUNT (sizeof(icon_seq) / sizeof(icon_seq[0]))

static void icon_set_green(lv_obj_t *icon, bool on)
{
    if (icon == NULL) {
        return;
    }
    /* recolor_opa 0 = original PNG colors; COVER = fully tinted to recolor. */
    lv_obj_set_style_image_recolor(icon, ICON_GREEN, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor_opa(icon, on ? LV_OPA_COVER : LV_OPA_TRANSP,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
}

/* Periodic timer: every ICON_GREEN_MS, turn the previous icon off and the next
 * one on, walking the sequence in a loop. */
static void icon_cycle_timer_cb(lv_timer_t *t)
{
    (void)t;
    static uint32_t idx = 0;

    icon_set_green(*icon_seq[idx], false);     /* restore current */
    idx = (idx + 1) % ICON_COUNT;              /* advance */
    icon_set_green(*icon_seq[idx], true);      /* light up next */
}

/* --- Slider sweep -------------------------------------------------------
 * Driven by a low-frequency timer rather than an lv_anim. An lv_anim recomputes
 * and redraws the slider every display refresh (~30 fps); each redraw makes
 * disp_flush() clean the WHOLE ARGB8888 canvas out of cache (LCD_PIXELS*4 bytes
 * - 3.6 MB at 720p), which dominates CPU on a big panel and also makes the long
 * slider strobe/ghost in the DIRECT double buffer when it moves fast.
 *
 * With a timer we redraw exactly 1000/SLIDER_REFRESH_MS times per second, so
 * raising SLIDER_REFRESH_MS trades smoothness for CPU headroom directly. The
 * sweep walks min->max->min in SLIDER_SWEEP_STEPS steps each way. */
#define SLIDER_REFRESH_MS   200 /* ms between slider updates (higher = less CPU) */
#define SLIDER_SWEEP_STEPS  60 /* steps for one min->max sweep */

typedef struct {
    lv_obj_t **ref;  /* &ui_SliderX, read lazily so it is NULL-safe */
    int32_t    step; /* 0..SLIDER_SWEEP_STEPS */
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
    static slider_sweep_t states[2];
    static uint32_t n = 0;
    if (n >= 2) {
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
    /* Start with all icons in their original (un-tinted) state, then light the
     * first one so the cycle has a visible head start. */
    for (uint32_t i = 0; i < ICON_COUNT; i++) {
        icon_set_green(*icon_seq[i], false);
    }
    icon_set_green(*icon_seq[0], true);
    lv_timer_create(icon_cycle_timer_cb, ICON_GREEN_MS, NULL);

    /* Sweep the remaining slider min->max->min via a low-rate timer. (The other
     * slider was replaced by the scrolling marquee label, which LVGL animates on
     * its own, so there is nothing to sweep for it here.) */
    slider_sweep_start(&ui_Slider1, 0);
}
