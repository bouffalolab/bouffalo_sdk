#ifndef _BL_MIPI_DBI_PORT_H
#define _BL_MIPI_DBI_PORT_H

#include "../lcd_conf.h"

#if (defined(LCD_DBI_INTERFACE_TYPE) && (LCD_DBI_INTERFACE_TYPE == 1))

/* Do not modify this file ! */

/* Optional pixel format */
#define LCD_DBI_LCD_PIXEL_FORMAT_RGB565   1
#define LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888 2

typedef struct {
    uint32_t pixel_format;
    uint32_t clock_freq; /* No use */

#if (LCD_DBI_WORK_MODE == 4)
    uint8_t cmd_wire_dual_en;
    uint8_t addr_wire_dual_en;
    uint8_t data_wire_dual_en;
#endif
} lcd_dbi_init_t;

int lcd_dbi_init(lcd_dbi_init_t *dbi_parra);

#if (LCD_DBI_WORK_MODE == 4)
int lcd_dbi_ex_qspi_addr_cfg(uint8_t addr_byte_size, uint32_t addr_val);
#endif

int lcd_dbi_transmit_cmd_para(uint8_t cmd, uint32_t *para, size_t para_num);
int lcd_dbi_transmit_cmd_pixel_sync(uint8_t cmd, uint32_t *pixel, size_t pixel_num);
int lcd_dbi_transmit_cmd_pixel_fill_sync(uint8_t cmd, uint32_t pixel_val, size_t pixel_num);

int lcd_dbi_is_busy(void);
int lcd_dbi_async_callback_enable(bool enable);
int lcd_dbi_async_callback_register(void (*callback)(void));
int lcd_dbi_transmit_cmd_pixel_async(uint8_t cmd, uint32_t *pixel, size_t pixel_num);
int lcd_dbi_transmit_cmd_pixel_fill_async(uint8_t cmd, uint32_t pixel_val, size_t pixel_num);

#endif
#endif