#include "bflb_mtimer.h"
#include "bflb_dpi.h"
#include "bflb_osd.h"
#include "board.h"

#define OSD_LAYER_MOON_WIDTH  80
#define OSD_LAYER_MOON_HEIGHT 96
#define OSD_LAYER_MOON_PIXEL  (OSD_LAYER_MOON_WIDTH * OSD_LAYER_MOON_HEIGHT)

static struct bflb_device_s *dpi;
static struct bflb_device_s *osd;

volatile int32_t blend_x_step = 2;
volatile int32_t blend_y_step = 2;

extern const uint8_t picture_moon_80x96_rgb565[];

uint16_t osd_layer_buffer[OSD_LAYER_MOON_PIXEL];

static struct bflb_osd_blend_config_s osd_blend_config = {
    .blend_format = OSD_BLEND_FORMAT_RGB565,
    .order_a = 3,
    .order_rv = 2,
    .order_gy = 1,
    .order_bu = 0,
    .coor = {
        .start_x = 0,
        .start_y = 0,
        .end_x = OSD_LAYER_MOON_WIDTH,
        .end_y = OSD_LAYER_MOON_HEIGHT,
    },
    .layer_buffer_addr = (uint32_t)osd_layer_buffer,
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
    bflb_osd_int_clear(osd);
    bflb_osd_blend_disable(osd);
    bflb_osd_blend_set_coordinate(osd, &osd_blend_config.coor);
    bflb_osd_blend_enable(osd);
    if ((osd_blend_config.coor.start_x + OSD_LAYER_MOON_WIDTH + blend_x_step) > 799) {
        blend_x_step = -2;
    } else if (osd_blend_config.coor.start_x == 0) {
        blend_x_step = 2;
    }
    if ((osd_blend_config.coor.start_y + OSD_LAYER_MOON_HEIGHT + blend_y_step) > 479) {
        blend_y_step = -2;
    } else if (osd_blend_config.coor.start_y == 0) {
        blend_y_step = 2;
    }
    osd_blend_config.coor.start_x += blend_x_step;
    osd_blend_config.coor.start_y += blend_y_step;
    osd_blend_config.coor.end_x = osd_blend_config.coor.start_x + OSD_LAYER_MOON_WIDTH;
    osd_blend_config.coor.end_y = osd_blend_config.coor.start_y + OSD_LAYER_MOON_HEIGHT;
}

void osd_buffer_init(void)
{
    memcpy(osd_layer_buffer, picture_moon_80x96_rgb565, sizeof(osd_layer_buffer));
    bflb_l1c_dcache_clean_range(osd_layer_buffer, sizeof(osd_layer_buffer));
}

int main(void)
{
    struct bflb_dpi_config_s dpi_config = {
        .width = 800,
        .height = 480,
        .hsw = 45,
        .hbp = 45,
        .hfp = 89,
        .vsw = 7,
        .vbp = 7,
        .vfp = 5,
        .interface = DPI_INTERFACE_24_PIN,
        .input_sel = DPI_INPUT_SEL_TEST_PATTERN_WITH_OSD,
        .test_pattern = DPI_TEST_PATTERN_RED,
        .data_format = DPI_DATA_FORMAT_TEST_PATTERN,
        .framebuffer_addr = 0,
    };

    board_init();
    board_dpi_gpio_init();

    dpi = bflb_device_get_by_name(BFLB_NAME_DPI);
    osd = bflb_device_get_by_name(BFLB_NAME_OSD0);

    printf("dpi init\r\n");
    bflb_dpi_init(dpi, &dpi_config);

    osd_buffer_init();

    bflb_osd_int_mask(osd, false);
    bflb_irq_attach(osd->irq_num, osd_layer0_isr, NULL);
    bflb_irq_enable(osd->irq_num);

    bflb_osd_blend_init(osd, &osd_blend_config);
    bflb_osd_blend_set_global_a(osd, true, 0x7f);
    bflb_osd_blend_enable(osd);

    /* draw rectangle 1 */
    bflb_osd_draw_init(osd, OSD_DRAW_LAYER_0, &osd_draw_config);
    bflb_osd_draw_enable(osd, OSD_DRAW_LAYER_0);

    /* draw rectangle 2 */
    osd_draw_config.border_thickness = 4;
    osd_draw_config.color_y = (OSD_DRAW_YUV_COLOR_GOLD >> 16) & 0xff;
    osd_draw_config.color_u = (OSD_DRAW_YUV_COLOR_GOLD >> 8) & 0xff;
    osd_draw_config.color_v = OSD_DRAW_YUV_COLOR_GOLD & 0xff;
    osd_draw_config.coor.start_x = 300;
    osd_draw_config.coor.start_y = 60;
    osd_draw_config.coor.end_x = 599;
    osd_draw_config.coor.end_y = 399;
    bflb_osd_draw_init(osd, OSD_DRAW_LAYER_1, &osd_draw_config);
    bflb_osd_draw_enable(osd, OSD_DRAW_LAYER_1);

    printf("dpi enable\r\n");
    bflb_dpi_enable(dpi);

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
