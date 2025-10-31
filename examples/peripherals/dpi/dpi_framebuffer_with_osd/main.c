#include "bflb_mtimer.h"
#include "bflb_dpi.h"
#include "bflb_osd.h"
#include "board.h"

#define LCD_WIDTH                       800
#define LCD_HEIGHT                      480
#define LCD_PIXEL                       (LCD_WIDTH * LCD_HEIGHT)

#define OSD_BLEND_LAYER0_MOON_WIDTH     80
#define OSD_BLEND_LAYER0_MOON_HEIGHT    96
#define OSD_BLEND_LAYER0_MOON_PIXEL     (OSD_BLEND_LAYER0_MOON_WIDTH * OSD_BLEND_LAYER0_MOON_HEIGHT)

#define OSD_BLEND_LAYER1_BALLOON_WIDTH  96
#define OSD_BLEND_LAYER1_BALLOON_HEIGHT 96
#define OSD_BLEND_LAYER1_BALLOON_PIXEL  (OSD_BLEND_LAYER1_BALLOON_WIDTH * OSD_BLEND_LAYER1_BALLOON_HEIGHT)

static struct bflb_device_s *dpi;
static struct bflb_device_s *osd0;
static struct bflb_device_s *osd1;

static volatile int32_t blend0_x_step = 2;
static volatile int32_t blend0_y_step = 2;
static volatile int32_t blend1_x_step = 3;
static volatile int32_t blend1_y_step = 3;
static volatile uint8_t replace_index = 0;

extern const uint8_t picture_moon_80x96_rgb565[];
extern const uint32_t balloon_palette_256[];
extern const uint8_t picture_balloon_96x96_8bit[];

ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint16_t framebuffer_1[LCD_PIXEL];
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint16_t framebuffer_2[LCD_PIXEL];
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint16_t framebuffer_3[LCD_PIXEL];

ATTR_NOINIT_PSRAM_SECTION uint16_t osd0_layer_buffer[OSD_BLEND_LAYER0_MOON_PIXEL];
ATTR_NOINIT_PSRAM_SECTION uint8_t osd1_layer_buffer[OSD_BLEND_LAYER1_BALLOON_PIXEL];

static struct bflb_osd_blend_config_s osd0_blend_config = {
    .blend_format = OSD_BLEND_FORMAT_RGB565,
    .order_a = 3,
    .order_rv = 2,
    .order_gy = 1,
    .order_bu = 0,
    .coor = {
        .start_x = 0,
        .start_y = 0,
        .end_x = OSD_BLEND_LAYER0_MOON_WIDTH,
        .end_y = OSD_BLEND_LAYER0_MOON_HEIGHT,
    },
    .layer_buffer_addr = (uint32_t)osd0_layer_buffer,
};

static struct bflb_osd_blend_config_s osd1_blend_config = {
    .blend_format = OSD_BLEND_FORMAT_ARGB_PALETTE_8BIT,
    .order_a = 0,
    .order_rv = 1,
    .order_gy = 2,
    .order_bu = 3,
    .coor = {
        .start_x = 0,
        .start_y = 0,
        .end_x = OSD_BLEND_LAYER1_BALLOON_WIDTH,
        .end_y = OSD_BLEND_LAYER1_BALLOON_HEIGHT,
    },
    .layer_buffer_addr = (uint32_t)osd1_layer_buffer,
};

struct bflb_osd_blend_replace_s osd1_blend_replace = {
    .replace_between = true,
    .target_value = 0,
    .min_value = 0,
    .max_value = 0,
};

static struct bflb_osd_draw_config_s osd_draw_config = {
    .is_solid = false,
    .border_thickness = 2,
    .color_y = (OSD_DRAW_YUV_COLOR_BLUE >> 16) & 0xff,
    .color_u = (OSD_DRAW_YUV_COLOR_BLUE >> 8) & 0xff,
    .color_v = OSD_DRAW_YUV_COLOR_BLUE & 0xff,
    .coor = {
        .start_x = 200,
        .start_y = 100,
        .end_x = 499,
        .end_y = 299,
    },
};

void osd_layer0_isr(int irq, void *arg)
{
    bflb_osd_int_clear(osd0);
    bflb_osd_blend_disable(osd0);
    bflb_osd_blend_set_coordinate(osd0, &osd0_blend_config.coor);
    bflb_osd_blend_enable(osd0);
    if ((osd0_blend_config.coor.start_x + OSD_BLEND_LAYER0_MOON_WIDTH + blend0_x_step) > 799) {
        blend0_x_step = -2;
    } else if (osd0_blend_config.coor.start_x == 0) {
        blend0_x_step = 2;
    }
    if ((osd0_blend_config.coor.start_y + OSD_BLEND_LAYER0_MOON_HEIGHT + blend0_y_step) > 479) {
        blend0_y_step = -2;
    } else if (osd0_blend_config.coor.start_y == 0) {
        blend0_y_step = 2;
    }
    osd0_blend_config.coor.start_x += blend0_x_step;
    osd0_blend_config.coor.start_y += blend0_y_step;
    osd0_blend_config.coor.end_x = osd0_blend_config.coor.start_x + OSD_BLEND_LAYER0_MOON_WIDTH;
    osd0_blend_config.coor.end_y = osd0_blend_config.coor.start_y + OSD_BLEND_LAYER0_MOON_HEIGHT;
}

void osd_layer1_isr(int irq, void *arg)
{
    bflb_osd_int_clear(osd1);
    bflb_osd_blend_disable(osd1);
    bflb_osd_blend_set_coordinate(osd1, &osd1_blend_config.coor);
    bflb_osd_blend_enable(osd1);
    if ((osd1_blend_config.coor.start_x + OSD_BLEND_LAYER1_BALLOON_WIDTH + blend1_x_step) > 799) {
        blend1_x_step = -3;
    } else if (osd1_blend_config.coor.start_x == 0) {
        blend1_x_step = 3;
    }
    if ((osd1_blend_config.coor.start_y + OSD_BLEND_LAYER1_BALLOON_HEIGHT + blend1_y_step) > 479) {
        blend1_y_step = -3;
    } else if (osd1_blend_config.coor.start_y == 0) {
        blend1_y_step = 3;
    }
    osd1_blend_config.coor.start_x += blend1_x_step;
    osd1_blend_config.coor.start_y += blend1_y_step;
    osd1_blend_config.coor.end_x = osd1_blend_config.coor.start_x + OSD_BLEND_LAYER1_BALLOON_WIDTH;
    osd1_blend_config.coor.end_y = osd1_blend_config.coor.start_y + OSD_BLEND_LAYER1_BALLOON_HEIGHT;
}

void buffer_init(void)
{
    uint32_t i;

    /* RED */
    for (i = 0; i < LCD_PIXEL / 3; i++) {
        framebuffer_1[i] = 0x1f << 11;
    }
    /* GREE */
    for (; i < LCD_PIXEL / 3 * 2; i++) {
        framebuffer_1[i] = 0x3f << 5;
    }
    /* BLUE */
    for (; i < LCD_PIXEL; i++) {
        framebuffer_1[i] = 0x1f;
    }
    bflb_l1c_dcache_clean_range(framebuffer_1, sizeof(framebuffer_1));

    for (i = 0; i < LCD_PIXEL / 3; i++) {
        framebuffer_2[i] = 0x3f << 5;
    }
    for (; i < LCD_PIXEL / 3 * 2; i++) {
        framebuffer_2[i] = 0x1f;
    }
    for (; i < LCD_PIXEL; i++) {
        framebuffer_2[i] = 0x1f << 11;
    }
    bflb_l1c_dcache_clean_range(framebuffer_2, sizeof(framebuffer_2));

    for (i = 0; i < LCD_PIXEL / 3; i++) {
        framebuffer_3[i] = 0x1f;
    }
    for (; i < LCD_PIXEL / 3 * 2; i++) {
        framebuffer_3[i] = 0x1f << 11;
    }
    for (; i < LCD_PIXEL; i++) {
        framebuffer_3[i] = 0x3f << 5;
    }
    bflb_l1c_dcache_clean_range(framebuffer_3, sizeof(framebuffer_3));

    memcpy(osd0_layer_buffer, picture_moon_80x96_rgb565, sizeof(osd0_layer_buffer));
    bflb_l1c_dcache_clean_range(osd0_layer_buffer, sizeof(osd0_layer_buffer));

    memcpy(osd1_layer_buffer, picture_balloon_96x96_8bit, sizeof(osd1_layer_buffer));
    bflb_l1c_dcache_clean_range(osd1_layer_buffer, sizeof(osd1_layer_buffer));
}

int main(void)
{
    struct bflb_dpi_config_s dpi_config = {
        .width = LCD_WIDTH,
        .height = LCD_HEIGHT,
        .hsw = 45,
        .hbp = 45,
        .hfp = 89,
        .vsw = 7,
        .vbp = 7,
        .vfp = 5,
        .interface = DPI_INTERFACE_24_PIN,
        .input_sel = DPI_INPUT_SEL_FRAMEUFFER_WITH_OSD,
        .test_pattern = DPI_TEST_PATTERN_NULL,
        .data_format = DPI_DATA_FORMAT_RGB565,
        .framebuffer_addr = (uint32_t)framebuffer_1,
    };
    uint32_t i;

    board_init();
    board_dpi_gpio_init();

    dpi = bflb_device_get_by_name(BFLB_NAME_DPI);
    osd0 = bflb_device_get_by_name(BFLB_NAME_OSD0);
    osd1 = bflb_device_get_by_name(BFLB_NAME_OSD1);

    printf("dpi init\r\n");
    bflb_dpi_init(dpi, &dpi_config);

    buffer_init();

    bflb_osd_int_mask(osd0, false);
    bflb_irq_attach(osd0->irq_num, osd_layer0_isr, NULL);
    bflb_irq_enable(osd0->irq_num);

    bflb_osd_int_mask(osd1, false);
    bflb_irq_attach(osd1->irq_num, osd_layer1_isr, NULL);
    bflb_irq_enable(osd1->irq_num);

    /* init blend 0 layer */
    bflb_osd_blend_init(osd0, &osd0_blend_config);
    bflb_osd_blend_set_global_a(osd0, true, 0x7f);
    bflb_osd_blend_enable(osd0);

    /* init blend 1 layer */
    bflb_osd_blend_init(osd1, &osd1_blend_config);
    for (i = 0; i < 256; i++) {
        bflb_osd_blend_set_palette_color(osd1, i, balloon_palette_256[i] | 0xbf);
    }
    /* balloon background color index is 0xff, set alpha of index 0xff to 0 */
    bflb_osd_blend_set_palette_color(osd1, 0xff, balloon_palette_256[0xff] | 0);
    bflb_osd_blend_enable(osd1);

    /* init draw 0 layer 0 */
    bflb_osd_draw_init(osd0, OSD_DRAW_LAYER_0, &osd_draw_config);
    bflb_osd_draw_enable(osd0, OSD_DRAW_LAYER_0);

    /* init draw 0 layer 1 */
    osd_draw_config.border_thickness = 4;
    osd_draw_config.color_y = (OSD_DRAW_YUV_COLOR_GOLD >> 16) & 0xff;
    osd_draw_config.color_u = (OSD_DRAW_YUV_COLOR_GOLD >> 8) & 0xff;
    osd_draw_config.color_v = OSD_DRAW_YUV_COLOR_GOLD & 0xff;
    osd_draw_config.coor.start_x = 300;
    osd_draw_config.coor.start_y = 60;
    osd_draw_config.coor.end_x = 599;
    osd_draw_config.coor.end_y = 399;
    bflb_osd_draw_init(osd0, OSD_DRAW_LAYER_1, &osd_draw_config);
    bflb_osd_draw_enable(osd0, OSD_DRAW_LAYER_1);

    printf("dpi enable\r\n");
    bflb_dpi_enable(dpi);

    while (1) {
        bflb_mtimer_delay_ms(1000);

        /* balloon color index is 0xb1 and 0xf6, remap to other index */
        osd1_blend_replace.target_value = replace_index % 255;
        replace_index += 20;
        osd1_blend_replace.min_value = 0xb1;
        osd1_blend_replace.max_value = 0xf6;
        bflb_osd_blend_replace_palette_index(osd1, true, &osd1_blend_replace);

        if (bflb_dpi_get_framebuffer_using(dpi) == (uint32_t)framebuffer_1) {
            bflb_dpi_framebuffer_switch(dpi, (uint32_t)framebuffer_2);
        } else if (bflb_dpi_get_framebuffer_using(dpi) == (uint32_t)framebuffer_2) {
            bflb_dpi_framebuffer_switch(dpi, (uint32_t)framebuffer_3);
        } else {
            bflb_dpi_framebuffer_switch(dpi, (uint32_t)framebuffer_1);
        }
    }
}
