#include "hal_spi.h"
#include "hal_gpio.h"
#include "ssd1306.h"
#include "badapple.h"

void oled_draw_logo(uint8_t x, uint8_t y, uint8_t index, uint8_t mode);
void badapple_display(void);

int main(void)
{
    bflb_platform_init(0);
    ssd1306_init();

    ssd1306_draw_bmp(0, 0, (unsigned char *)logo, 0, 1);
    ssd1306_refresh_gram();
    bflb_platform_delay_ms(2000);
    ssd1306_draw_bmp(0, 0, (unsigned char *)logo, 1, 1);
    ssd1306_refresh_gram();
    bflb_platform_delay_ms(2000);
    BL_CASE_SUCCESS;
    while (1) {
        badapple_display();
    }
}

void badapple_display(void)
{
    uint8_t t;
    for (t = 0; t < 175; t++) {
        ssd1306_draw_bmp(0, 0, (unsigned char *)Image, t, 1);
        ssd1306_refresh_gram();
        bflb_platform_delay_ms(200);
    }
}