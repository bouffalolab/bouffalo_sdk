#include "ui.h"

#define STATUS_ICON_COUNT 4
#define SLIDER_SWEEP_STEPS 60

/* Portrait framebuffers are viewed in landscape, as in the original OSD UIs. */
static lv_obj_t *ui_root;
static lv_obj_t *volume_slider;
static lv_obj_t *marquee;
static lv_obj_t *ota_label;
static lv_obj_t *icons[8]; /* sound, stop, next, previous, bluetooth, WiFi, comment, speed */
static const lv_image_dsc_t * const icon_sources[] = {
    &ui_img_sound_png, &ui_img_stop_png,
    &ui_img_fastforward_png, &ui_img_fastforward_png,
    &ui_img_bluetooth_png, &ui_img_wifi_png,
    &ui_img_comment_png, &ui_img_speed_png,
};
static lv_display_t *ui_display;
static lv_timer_t *icon_timer;
static lv_timer_t *slider_timer;
static uint32_t active_icon;
static int32_t slider_step;
static int32_t slider_dir;

static const char marquee_text[] =
    "Bouffalo Lab (Nanjing) Co., Ltd. was established in Nanjing in 2016. "
    "It is a company focusing on the research and development of world-leading "
    "ultra-low power consumption, intelligent Internet of Things and edge "
    "computing SoCs, as well as providing overall solutions for intelligent "
    "cloud platforms. enterprise. At the same time, the company has complete "
    "multi-mode wireless connection technology, audio and video processing and "
    "artificial intelligence algorithm technology, which can fully realize the "
    "chip research and development of single-chip integration.   ";

static void ui_place(lv_obj_t *obj, int32_t x, int32_t y,
                     int32_t width, int32_t height, bool rotated)
{
    int32_t dx = x - width / 2;
    int32_t dy = y - height / 2;

    lv_obj_align(obj, LV_ALIGN_CENTER, rotated ? dy : dx, rotated ? -dx : dy);
}

static const lv_font_t *ui_font(int32_t short_side)
{
#if LV_FONT_MONTSERRAT_32
    if (short_side >= 600) {
        return &lv_font_montserrat_32;
    }
#endif
#if LV_FONT_MONTSERRAT_20
    if (short_side >= 300) {
        return &lv_font_montserrat_20;
    }
#endif
    return LV_FONT_DEFAULT;
}

static void ui_layout(void)
{
    int32_t physical_width = lv_display_get_horizontal_resolution(ui_display);
    int32_t physical_height = lv_display_get_vertical_resolution(ui_display);
    bool rotated = physical_height > physical_width;
    int32_t width = rotated ? physical_height : physical_width;
    int32_t height = rotated ? physical_width : physical_height;
    int32_t margin = LV_MAX(4, height / 40);
    int32_t status_size = LV_CLAMP(20, height / 12, 64);
    int32_t control_size = LV_CLAMP(24, height / 9, 80);
    int32_t thickness = LV_MAX(3, height / 80);
    const lv_font_t *font = ui_font(height);
    int32_t body_top = margin * 2 + font->line_height;
    int32_t body_bottom = height - margin * 2 - control_size;
    int32_t sound_y = body_bottom - status_size / 2;
    int32_t slider_top = body_top + status_size / 2;
    int32_t slider_bottom = sound_y - status_size / 2 - margin;
    int32_t status_top = body_top + status_size / 2;
    int32_t status_bottom = body_bottom - status_size / 2;

    for (uint32_t i = 0; i < sizeof(icons) / sizeof(icons[0]); i++) {
        int32_t size = i >= 1 && i <= 3 ? control_size : status_size;
        /* Balance the visible glyphs: speed has more transparent padding. */
        if (i == 7) {
            size = size * 180 / 100;
        } else if (i == 5 || i == 6) {
            size = size * 85 / 100;
        }
        uint32_t source_size = LV_MAX(icon_sources[i]->header.w, icon_sources[i]->header.h);
        lv_image_set_scale(icons[i], size * LV_SCALE_NONE / source_size);
        lv_image_set_rotation(icons[i], ((rotated ? 2700 : 0) + (i == 3 ? 1800 : 0)) % 3600);
    }

    ui_place(icons[0], margin + status_size / 2, sound_y, width, height, rotated);
    /* Playback order: previous, stop, next. */
    ui_place(icons[3], width / 4, height - margin - control_size / 2, width, height, rotated);
    ui_place(icons[1], width / 2, height - margin - control_size / 2, width, height, rotated);
    ui_place(icons[2], width * 3 / 4, height - margin - control_size / 2, width, height, rotated);

    for (uint32_t i = 0; i < STATUS_ICON_COUNT; i++) {
        ui_place(icons[i + 4], width - margin - status_size / 2,
                 status_top + (status_bottom - status_top) * i / (STATUS_ICON_COUNT - 1),
                 width, height, rotated);
    }

    /* Swap the slider's dimensions instead of rotating a large container/layer. */
    int32_t slider_length = LV_MAX(thickness * 2, slider_bottom - slider_top);
    lv_obj_set_size(volume_slider, rotated ? slider_length : thickness,
                    rotated ? thickness : slider_length);
    ui_place(volume_slider, margin + status_size / 2, (slider_top + slider_bottom) / 2,
             width, height, rotated);

    lv_obj_set_width(marquee, width - margin * 2);
    lv_obj_set_style_text_font(marquee, font, LV_PART_MAIN);
    lv_obj_set_style_transform_rotation(marquee, rotated ? 2700 : 0, LV_PART_MAIN);
    ui_place(marquee, width / 2, margin + font->line_height / 2, width, height, rotated);

    lv_obj_set_width(ota_label, width * 4 / 5);
    lv_obj_set_style_text_font(ota_label, font, LV_PART_MAIN);
    lv_obj_set_style_transform_rotation(ota_label, rotated ? 2700 : 0, LV_PART_MAIN);
    ui_place(ota_label, width / 2, height / 2, width, height, rotated);
}

static void resolution_changed_cb(lv_event_t *event)
{
    (void)event;
    ui_layout();
}

static void icon_cycle_cb(lv_timer_t *timer)
{
    (void)timer;
    lv_obj_set_style_image_recolor_opa(icons[active_icon + 4], LV_OPA_TRANSP, LV_PART_MAIN);
    active_icon = (active_icon + 1) % STATUS_ICON_COUNT;
    lv_obj_set_style_image_recolor_opa(icons[active_icon + 4], LV_OPA_COVER, LV_PART_MAIN);
}

static void slider_sweep_cb(lv_timer_t *timer)
{
    (void)timer;
    slider_step += slider_dir;
    if (slider_step >= SLIDER_SWEEP_STEPS || slider_step <= 0) {
        slider_dir = -slider_dir;
    }
    lv_slider_set_value(volume_slider, 100 * slider_step / SLIDER_SWEEP_STEPS, LV_ANIM_OFF);
}

static void ui_deleted_cb(lv_event_t *event)
{
    (void)event;
    lv_timer_delete(icon_timer);
    lv_timer_delete(slider_timer);
    lv_display_remove_event_cb_with_user_data(ui_display, resolution_changed_cb, NULL);
    icon_timer = NULL;
    slider_timer = NULL;
    ui_root = NULL;
    volume_slider = NULL;
    marquee = NULL;
    ota_label = NULL;
    ui_display = NULL;
    for (uint32_t i = 0; i < sizeof(icons) / sizeof(icons[0]); i++) {
        icons[i] = NULL;
    }
}

void ui_init(void)
{
    ui_destroy();
    ui_display = lv_display_get_default();
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_TRANSP, LV_PART_MAIN);
    ui_root = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(ui_root);
    lv_obj_set_size(ui_root, lv_pct(100), lv_pct(100));
    lv_obj_remove_flag(ui_root, LV_OBJ_FLAG_SCROLLABLE);

    for (uint32_t i = 0; i < sizeof(icons) / sizeof(icons[0]); i++) {
        icons[i] = lv_image_create(ui_root);
        lv_image_set_src(icons[i], icon_sources[i]);
        lv_obj_add_flag(icons[i], LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_image_recolor(icons[i], lv_color_hex(0x00C853), LV_PART_MAIN);
    }

    volume_slider = lv_slider_create(ui_root);
    lv_slider_set_range(volume_slider, 0, 100);
    lv_obj_set_style_pad_all(volume_slider, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(volume_slider, 0, LV_PART_KNOB);
    lv_obj_set_style_shadow_width(volume_slider, 0, LV_PART_KNOB);

    marquee = lv_label_create(ui_root);
    lv_label_set_long_mode(marquee, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_label_set_text_static(marquee, marquee_text);
    lv_obj_set_style_text_color(marquee, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_anim_duration(marquee, 128000, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_x(marquee, lv_pct(50), LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_y(marquee, lv_pct(50), LV_PART_MAIN);

    ota_label = lv_label_create(ui_root);
    lv_label_set_long_mode(ota_label, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_style_text_align(ota_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(ota_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(ota_label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ota_label, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ota_label, 8, LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_x(ota_label, lv_pct(50), LV_PART_MAIN);
    lv_obj_set_style_transform_pivot_y(ota_label, lv_pct(50), LV_PART_MAIN);
    lv_obj_add_flag(ota_label, LV_OBJ_FLAG_HIDDEN);

    ui_layout();
    active_icon = 0;
    slider_step = 0;
    slider_dir = 1;
    lv_obj_set_style_image_recolor_opa(icons[4], LV_OPA_COVER, LV_PART_MAIN);
    icon_timer = lv_timer_create(icon_cycle_cb, 1000, NULL);
    /* Keep the original low update rate to limit OSD redraw bandwidth. */
    slider_timer = lv_timer_create(slider_sweep_cb, 200, NULL);
    lv_obj_add_event_cb(ui_root, ui_deleted_cb, LV_EVENT_DELETE, NULL);
    lv_display_add_event_cb(ui_display, resolution_changed_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
}

void ui_destroy(void)
{
    if (ui_root != NULL) {
        lv_obj_delete(ui_root);
    }
}

void ui_set_ota_status(const char *text)
{
    if (ota_label == NULL) {
        return;
    }
    if (text == NULL) {
        lv_obj_add_flag(ota_label, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_label_set_text(ota_label, text);
    lv_obj_remove_flag(ota_label, LV_OBJ_FLAG_HIDDEN);
}
