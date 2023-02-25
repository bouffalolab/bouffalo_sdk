#ifndef _BL_MIPI_DBI_PORT_H
#define _BL_MIPI_DBI_PORT_H

#include "../lcd_conf.h"

#if (defined(LCD_DBI_INTERFACE_TYPE) && (LCD_DBI_INTERFACE_TYPE == 2))

/* Do not modify this file ! */

/* Optional pixel format */
#define LCD_DBI_LCD_PIXEL_FORMAT_RGB565   1
#define LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888 2

typedef struct {
    uint16_t pixel_format;
    uint32_t clock_freq; /* No use */
} lcd_mipi_dbi_init_t;

int pec_dbi_b_init(lcd_mipi_dbi_init_t *dbi_parra);

int pec_dbi_b_transmit_cmd_para(uint8_t cmd, uint32_t *para, size_t para_num);
int pec_dbi_b_transmit_cmd_pixel_sync(uint8_t cmd, uint32_t *pixel, size_t pixel_num);
int pec_dbi_b_transmit_cmd_pixel_fill_sync(uint8_t cmd, uint32_t pixel_val, size_t pixel_num);

int pec_dbi_b_dma_is_busy(void);
int pec_dbi_b_async_callback_register(void (*callback)(void));
int pec_dbi_b_async_callback_enable(bool enable);
int pec_dbi_b_transmit_cmd_pixel_async(uint8_t cmd, uint32_t *pixel, size_t pixel_num);
int pec_dbi_b_transmit_cmd_pixel_fill_async(uint8_t cmd, uint32_t pixel_val, size_t pixel_num);

#endif
#endif