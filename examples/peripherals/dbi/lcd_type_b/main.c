#include "bflb_mtimer.h"
#include "board.h"

#include "lcd.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define LCD_TEST_W (100)
#define LCD_TEST_H (20)

lcd_color_t lcd_test_buff[LCD_TEST_W * LCD_TEST_H];

lcd_color_t lcd_test_color[] = {
    LCD_COLOR_RGB(0xff, 0xff, 0xff),

    LCD_COLOR_RGB(0xff, 0x00, 0x00),
    LCD_COLOR_RGB(0x00, 0xff, 0x00),
    LCD_COLOR_RGB(0x00, 0x00, 0xff),

    LCD_COLOR_RGB(0xff, 0xff, 0x00),
    LCD_COLOR_RGB(0x00, 0xff, 0xff),
    LCD_COLOR_RGB(0xff, 0x00, 0xff),
};

void color_fill_buff(lcd_color_t color)
{
    for (int i = 0; i < sizeof(lcd_test_buff) / sizeof(lcd_color_t); i++) {
        lcd_test_buff[i] = color;
    }
}

void flush_async_callback()
{
    static uint32_t cnt;
    cnt += 1;

    /* lcd async int come */
    LOG_I("lcd async int come, cnt: %d\r\n", cnt);
}

int main(void)
{
    int color_num = 0;

    board_init();

    LOG_I("DBI lcd test\r\n");

    /* lcd init */
    lcd_init();

    /* register async callback */
    lcd_async_callback_register(flush_async_callback);

    /* clean lcd */
    lcd_clear(LCD_COLOR_RGB(0x00, 0x00, 0x00));

    /* disp font */
    lcd_draw_str_ascii16(20, 20, LCD_COLOR_RGB(0xff, 0x00, 0x00), LCD_COLOR_RGB(0x00, 0x00, 0x00), (uint8_t *)"Hello World !", 100);
    lcd_draw_str_ascii16(20, 40, LCD_COLOR_RGB(0x00, 0xff, 0x00), LCD_COLOR_RGB(0x00, 0x00, 0x00), (uint8_t *)"dbi lcd test.", 100);

    lcd_draw_rectangle(0, 0, LCD_W - 1, LCD_H - 1, LCD_COLOR_RGB(0xff, 0x00, 0x00));
    lcd_draw_rectangle(5, 5, LCD_W - 6, LCD_H - 6, LCD_COLOR_RGB(0x00, 0xff, 0x00));
    lcd_draw_rectangle(10, 10, LCD_W - 11, LCD_H - 11, LCD_COLOR_RGB(0x00, 0x00, 0xff));

    while (1) {
        for (int i = 0; i < 4; i++) {
            /* fill buff */
            uint8_t n = (color_num + i) % (sizeof(lcd_test_color) / sizeof(lcd_color_t));
            color_fill_buff(lcd_test_color[n]);

            /* draw picture (DMA non-blocking) */
            lcd_draw_picture_nonblocking(30, (80 + (LCD_TEST_H + 5) * i), (30 + LCD_TEST_W - 1), (80 + (LCD_TEST_H + 5) * i + LCD_TEST_H - 1), lcd_test_buff);
            /* wait flush done */
            while (lcd_draw_is_busy()) {
            };
            LOG_I("lcd flush done\r\n");

            if (color_num >= sizeof(lcd_test_color) / sizeof(lcd_color_t)) {
                color_num = 0;
            }
        }

        color_num ++ ;
        bflb_mtimer_delay_ms(1000);
    }
}
