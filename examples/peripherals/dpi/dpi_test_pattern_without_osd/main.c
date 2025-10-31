#include "bflb_mtimer.h"
#include "bflb_dpi.h"
#include "board.h"

static struct bflb_device_s *dpi;

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
        .input_sel = DPI_INPUT_SEL_TEST_PATTERN_WITHOUT_OSD,
        .test_pattern = DPI_TEST_PATTERN_RED,
        .data_format = DPI_DATA_FORMAT_TEST_PATTERN,
        .framebuffer_addr = 0,
    };

    board_init();
    board_dpi_gpio_init();

    dpi = bflb_device_get_by_name("dpi");

    printf("dpi init\r\n");
    bflb_dpi_init(dpi, &dpi_config);

    printf("dpi enable\r\n");
    bflb_dpi_enable(dpi);

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
