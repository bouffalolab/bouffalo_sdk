#include "bflb_mtimer.h"
#include "bflb_dpi.h"
#include "board.h"

#define LCD_WIDTH       800
#define LCD_HEIGHT      480
#define LCD_PIXEL       (LCD_WIDTH * LCD_HEIGHT)

/* DPI_DATA_FORMAT_RGB888/DPI_DATA_FORMAT_NRGB8888/DPI_DATA_FORMAT_RGB565 */
#define DATA_FORMAT_USE DPI_DATA_FORMAT_RGB565

static struct bflb_device_s *dpi;

#if (DATA_FORMAT_USE == DPI_DATA_FORMAT_RGB888)
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint8_t framebuffer_1[LCD_PIXEL * 3];
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint8_t framebuffer_2[LCD_PIXEL * 3];
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint8_t framebuffer_3[LCD_PIXEL * 3];
#elif (DATA_FORMAT_USE == DPI_DATA_FORMAT_NRGB8888)
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint32_t framebuffer_1[LCD_PIXEL];
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint32_t framebuffer_2[LCD_PIXEL];
#else /* DPI_DATA_FORMAT_RGB565 */
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint16_t framebuffer_1[LCD_PIXEL];
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint16_t framebuffer_2[LCD_PIXEL];
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(4))) uint16_t framebuffer_3[LCD_PIXEL];
#endif

void framebuffer_init(void)
{
    uint32_t i;

#if (DATA_FORMAT_USE == DPI_DATA_FORMAT_RGB888)
    /* RED */
    for (i = 0; i < LCD_PIXEL / 3; i++) {
        framebuffer_1[3 * i] = 0;
        framebuffer_1[3 * i + 1] = 0;
        framebuffer_1[3 * i + 2] = 0xff;
    }
    /* GREE */
    for (; i < LCD_PIXEL / 3 * 2; i++) {
        framebuffer_1[3 * i] = 0;
        framebuffer_1[3 * i + 1] = 0xff;
        framebuffer_1[3 * i + 2] = 0;
    }
    /* BLUE */
    for (; i < LCD_PIXEL; i++) {
        framebuffer_1[3 * i] = 0xff;
        framebuffer_1[3 * i + 1] = 0;
        framebuffer_1[3 * i + 2] = 0;
    }
    bflb_l1c_dcache_clean_range(framebuffer_1, sizeof(framebuffer_1));

    for (i = 0; i < LCD_PIXEL / 3; i++) {
        framebuffer_2[3 * i] = 0;
        framebuffer_2[3 * i + 1] = 0xff;
        framebuffer_2[3 * i + 2] = 0;
    }
    for (; i < LCD_PIXEL / 3 * 2; i++) {
        framebuffer_2[3 * i] = 0xff;
        framebuffer_2[3 * i + 1] = 0;
        framebuffer_2[3 * i + 2] = 0;
    }
    for (; i < LCD_PIXEL; i++) {
        framebuffer_2[3 * i] = 0;
        framebuffer_2[3 * i + 1] = 0;
        framebuffer_2[3 * i + 2] = 0xff;
    }
    bflb_l1c_dcache_clean_range(framebuffer_2, sizeof(framebuffer_2));

    for (i = 0; i < LCD_PIXEL / 3; i++) {
        framebuffer_3[3 * i] = 0xff;
        framebuffer_3[3 * i + 1] = 0;
        framebuffer_3[3 * i + 2] = 0;
    }
    for (; i < LCD_PIXEL / 3 * 2; i++) {
        framebuffer_3[3 * i] = 0;
        framebuffer_3[3 * i + 1] = 0;
        framebuffer_3[3 * i + 2] = 0xff;
    }
    for (; i < LCD_PIXEL; i++) {
        framebuffer_3[3 * i] = 0;
        framebuffer_3[3 * i + 1] = 0xff;
        framebuffer_3[3 * i + 2] = 0;
    }
    bflb_l1c_dcache_clean_range(framebuffer_3, sizeof(framebuffer_3));
#elif (DATA_FORMAT_USE == DPI_DATA_FORMAT_NRGB8888)
    /* RED */
    for (i = 0; i < LCD_PIXEL / 3; i++) {
        framebuffer_1[i] = 0x00ff0000;
    }
    /* GREE */
    for (; i < LCD_PIXEL / 3 * 2; i++) {
        framebuffer_1[i] = 0x0000ff00;
    }
    /* BLUE */
    for (; i < LCD_PIXEL; i++) {
        framebuffer_1[i] = 0x000000ff;
    }
    bflb_l1c_dcache_clean_range(framebuffer_1, sizeof(framebuffer_1));

    for (i = 0; i < LCD_PIXEL / 3; i++) {
        framebuffer_2[i] = 0x0000ff00;
    }
    for (; i < LCD_PIXEL / 3 * 2; i++) {
        framebuffer_2[i] = 0x000000ff;
    }
    for (; i < LCD_PIXEL; i++) {
        framebuffer_2[i] = 0x00ff0000;
    }
    bflb_l1c_dcache_clean_range(framebuffer_2, sizeof(framebuffer_2));
#else /* DPI_DATA_FORMAT_RGB565 */
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
#endif
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
        .input_sel = DPI_INPUT_SEL_FRAMEBUFFER_WITHOUT_OSD,
        .test_pattern = DPI_TEST_PATTERN_NULL,
        .data_format = DATA_FORMAT_USE,
        .framebuffer_addr = (uint32_t)framebuffer_1,
    };

    board_init();
    board_dpi_gpio_init();

    framebuffer_init();

    dpi = bflb_device_get_by_name("dpi");

    printf("dpi init\r\n");
    bflb_dpi_init(dpi, &dpi_config);

    printf("dpi enable\r\n");
    bflb_dpi_enable(dpi);

    while (1) {
        bflb_mtimer_delay_ms(1000);
        if (bflb_dpi_get_framebuffer_using(dpi) == (uint32_t)framebuffer_1) {
            bflb_dpi_framebuffer_switch(dpi, (uint32_t)framebuffer_2);
#if (DATA_FORMAT_USE != DPI_DATA_FORMAT_NRGB8888)
        } else if (bflb_dpi_get_framebuffer_using(dpi) == (uint32_t)framebuffer_2) {
            bflb_dpi_framebuffer_switch(dpi, (uint32_t)framebuffer_3);
#endif
        } else {
            bflb_dpi_framebuffer_switch(dpi, (uint32_t)framebuffer_1);
        }
    }
}
