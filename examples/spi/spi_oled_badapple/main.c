#include "hal_spi.h"
#include "hal_gpio.h"
#include "ssd1306.h"
#include "badapple.h"

void oled_draw_logo(uint8_t x, uint8_t y, uint8_t index, uint8_t mode);
void badapple_display(void);

int main(void)
{
    bflb_platform_init(0);
    oled_init();

    oled_draw_bmp(0, 0, (unsigned char *)logo, 0, 1);
    oled_show();
    bflb_platform_delay_ms(2000);
    oled_draw_bmp(0, 0, (unsigned char *)logo, 1, 1);
    oled_show();
    bflb_platform_delay_ms(2000);

    while (1)
        badapple_display();

    BL_CASE_SUCCESS;
}

void badapple_display(void)
{
    uint8_t t;
    for (t = 0; t < 175; t++) {
        oled_draw_bmp(0, 0, (unsigned char *)Image, t, 1);
        oled_show();
        bflb_platform_delay_ms(200);
    }
}