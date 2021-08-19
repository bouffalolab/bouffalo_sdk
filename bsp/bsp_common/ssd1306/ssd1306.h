#ifndef __SSD1306_h__
#define __SSD1306_h__

#include "drv_device.h"

#define OLED_CS_PIN  GPIO_PIN_10
#define OLED_DC_PIN  GPIO_PIN_22
#define OLED_RES_PIN GPIO_PIN_11

#define CS1_HIGH gpio_write(OLED_CS_PIN, 1)
#define CS1_LOW  gpio_write(OLED_CS_PIN, 0)
#define DC_HIGH  gpio_write(OLED_DC_PIN, 1)
#define DC_LOW   gpio_write(OLED_DC_PIN, 0)
#define RES_HIGH gpio_write(OLED_RES_PIN, 1)
#define RES_LOW  gpio_write(OLED_RES_PIN, 0)

extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
extern const unsigned char Hzk[][32];

void oled_init(void);
void oled_display_on(void);
void oled_display_off(void);
void oled_clear(void);
void oled_show(void);
void oled_draw_point(uint8_t x, uint8_t y, uint8_t mode);
void oled_draw_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);
void oled_draw_string(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size, uint8_t mode);
void oled_draw_chinese(uint8_t x, uint8_t y, uint8_t no, uint8_t mode);
void oled_draw_bmp(uint8_t x, uint8_t y, const unsigned char *image, uint8_t index, uint8_t mode);
void oled_draw_alpha(uint8_t x, uint8_t y, const unsigned char *image, uint8_t index, uint8_t mode);

#endif
