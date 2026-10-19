#ifndef _BL_SPI_HARD_4_H
#define _BL_SPI_HARD_4_H

#include "../lcd_conf.h"

#if (defined(LCD_SPI_INTERFACE_TYPE) && (LCD_SPI_INTERFACE_TYPE == 1))

/* Do not modify this file ! */

/* Optional pixel format */
#define LCD_SPI_LCD_PIXEL_FORMAT_RGB565   1
#define LCD_SPI_LCD_PIXEL_FORMAT_NRGB8888 2

#define LCD_SPI_HARD_4_CS_HIGH            bflb_gpio_set(gpio, LCD_SPI_HARD_4_PIN_CS)
#define LCD_SPI_HARD_4_CS_LOW             bflb_gpio_reset(gpio, LCD_SPI_HARD_4_PIN_CS)
#define LCD_SPI_HARD_4_DC_HIGH            bflb_gpio_set(gpio, LCD_SPI_HARD_4_PIN_DC)
#define LCD_SPI_HARD_4_DC_LOW             bflb_gpio_reset(gpio, LCD_SPI_HARD_4_PIN_DC)

typedef struct {
    uint32_t pixel_format;
    uint32_t clock_freq;

} lcd_spi_hard_4_init_t;

int lcd_spi_hard_4_init(lcd_spi_hard_4_init_t *spi_parra);

int lcd_spi_hard_4_transmit_cmd_para(uint8_t cmd, uint32_t *para, size_t para_num);
int lcd_spi_hard_4_transmit_cmd_pixel_sync(uint8_t cmd, uint32_t *pixel, size_t pixel_num);
int lcd_spi_hard_4_transmit_cmd_pixel_fill_sync(uint8_t cmd, uint32_t pixel_val, size_t pixel_num);

int lcd_spi_hard_4_is_busy(void);
int lcd_spi_hard_4_async_callback_enable(bool enable);
int lcd_spi_hard_4_async_callback_register(void (*callback)(void));
int lcd_spi_hard_4_transmit_cmd_pixel_async(uint8_t cmd, uint32_t *pixel, size_t pixel_num);
int lcd_spi_hard_4_transmit_cmd_pixel_fill_async(uint8_t cmd, uint32_t pixel_val, size_t pixel_num);

#endif
#endif