/**
 * @file lv_port_disp_templ.c
 *
 */

#include "lcd.h"

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
/* MCU LCD Common interface */
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_SPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI)

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>

#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI)
#include "bflb_dbi.h"
#endif

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#define DBG_TAG "LV_DISP"
#include "log.h"

/*********************
 *      DEFINES
 *********************/
#define BYTE_PER_PIXEL (LV_COLOR_DEPTH / 8)

#ifndef LCD_LVGL_CUSTOM_DRAW_BUFF
/* default buff def */
#ifdef CONFIG_PSRAM
#define DRAW_BUFF_ATTR __ALIGNED(64) ATTR_NOINIT_PSRAM_SECTION
#define DRAW_BUFF_SIZE (LCD_W * LCD_H / 4 * BYTE_PER_PIXEL)
#else
#define DRAW_BUFF_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION
#define DRAW_BUFF_SIZE (LCD_W * LCD_H / 8 * BYTE_PER_PIXEL)
#endif

#else
/* custom buffer def */
#define DRAW_BUFF_SIZE LCD_LVGL_CUSTOM_DRAW_BUFF_SIZE
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void disp_flush_wait_cb(lv_display_t *disp);
static void disp_resolution_cb(lv_event_t *e);
static void disp_rounder_cb(lv_event_t *e);
#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
static void disp_flush_done_cb(void);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#ifndef LCD_LVGL_CUSTOM_DRAW_BUFF
/* default buff def */
static uint8_t DRAW_BUFF_ATTR draw_buf_1[DRAW_BUFF_SIZE]; /* A buffer */
static uint8_t DRAW_BUFF_ATTR draw_buf_2[DRAW_BUFF_SIZE]; /* An other buffer */
#else
/* custom buffer def */
static LCD_LVGL_CUSTOM_DRAW_BUFF(1, draw_buf_1);
static LCD_LVGL_CUSTOM_DRAW_BUFF(2, draw_buf_2);
#endif

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
SemaphoreHandle_t disp_sem = NULL;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t *lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t *disp = lv_display_create(LCD_W, LCD_H);
    lv_display_set_flush_cb(disp, disp_flush);

    /* 
     * Two buffers for partial rendering
     * In flush_cb DMA or similar hardware should be used to update the display in the background.*/
    lv_display_set_buffers(disp, draw_buf_1, draw_buf_2, DRAW_BUFF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* set flush_wait_cb */
    lv_display_set_flush_wait_cb(disp, disp_flush_wait_cb);

    /* set LV_EVENT_SIZE_CHANGED event */
    lv_display_add_event_cb(disp, disp_resolution_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    /* set LV_EVENT_INVALIDATE_AREA event */
    lv_display_add_event_cb(disp, disp_rounder_cb, LV_EVENT_INVALIDATE_AREA, NULL);

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    lcd_init();
    lcd_clear(LCD_COLOR_RGB(0x00, 0X00, 0X00));

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    disp_sem = xSemaphoreCreateBinary();

    lcd_async_callback_register(disp_flush_done_cb);
#endif

#if (LCD_INTERFACE_TYPE != LCD_INTERFACE_DBI)

    if (LV_COLOR_DEPTH != LCD_COLOR_DEPTH) {
        LOG_E("LCD and LVGL have different color depth, LCD:%d, LVGL:%d\r\n", LCD_COLOR_DEPTH, LV_COLOR_DEPTH);
        LOG_E("Please set LV_COLOR_DEPTH to LCD_COLOR_DEPTH\r\n");
        return;
    }
#else

    if (LV_COLOR_DEPTH != LCD_COLOR_DEPTH) {
        LOG_W("LCD and LVGL have different color depth, LCD:%d, LVGL:%d\r\n", LCD_COLOR_DEPTH, LV_COLOR_DEPTH);
        struct bflb_device_s *dbi_hd = bflb_device_get_by_name("dbi");

        if (LV_COLOR_DEPTH == 16) {
            /* RGB565/BGR565 */
            if (!LCD_RGB_ORDER_MODE) {
                bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
                LOG_W("Switch the input color of DBI to DBI_PIXEL_INPUT_FORMAT_RGB_565\r\n");
            } else {
                bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_BGR_565);
                LOG_W("Switch the input color of DBI to DBI_PIXEL_INPUT_FORMAT_BGR_565\r\n");
            }
        } else if (LV_COLOR_DEPTH == 24) {
            /* RGB888/BGR888 */
            if (!LCD_RGB_ORDER_MODE) {
                bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_888);
                LOG_W("Switch the input color of DBI to DBI_PIXEL_INPUT_FORMAT_RGB_888\r\n");
            } else {
                bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_BGR_888);
                LOG_W("Switch the input color of DBI to DBI_PIXEL_INPUT_FORMAT_BGR_888\r\n");
            }
        } else if (LV_COLOR_DEPTH == 32) {
            /* NRGB888/NBGR8888 */
            if (!LCD_RGB_ORDER_MODE) {
                bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
                LOG_W("Switch the input color of DBI to DBI_PIXEL_INPUT_FORMAT_NRGB_8888\r\n");
            } else {
                bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NBGR_8888);
                LOG_W("Switch the input color of DBI to DBI_PIXEL_INPUT_FORMAT_NBGR_8888\r\n");
            }
        } else {
            LOG_E("Unsupported LVLG pixel format: %d\r\n", LV_COLOR_DEPTH);
        }
    }

#endif
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map)
{
    if (disp_flush_enabled) {
        lcd_draw_picture_nonblocking(area->x1, area->y1, area->x2, area->y2, (lcd_color_t *)px_map);
    } else {
        lv_display_flush_ready(disp_drv);
    }
}

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
/* flush done isr */
static void disp_flush_done_cb(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(disp_sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#endif

/* flush_wait_cb */
static void disp_flush_wait_cb(lv_display_t *disp)
{
#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    xSemaphoreTake(disp_sem, portMAX_DELAY);
#else
    while (lcd_draw_is_busy()) {
    };
#endif

    lv_display_flush_ready(disp);
}

/* resolution cb */
static void disp_resolution_cb(lv_event_t *e)
{
    lv_display_t *disp = (lv_display_t *)lv_event_get_target(e);
    lv_display_rotation_t rotation = lv_display_get_rotation(disp);

    while (lcd_draw_is_busy()) {
    };
    lcd_set_dir(rotation, 0);
}

void disp_rounder_cb(lv_event_t *e)
{
    lv_area_t *a = lv_event_get_param(e);
    (void)a;

#if defined(LCD_PIXEL_ALIGN_X) && (LCD_PIXEL_ALIGN_X > 1)
    a->x1 = a->x1 & (~(LCD_PIXEL_ALIGN_X - 1));
    a->x2 = a->x2 | (LCD_PIXEL_ALIGN_X - 1);
#endif

#if defined(LCD_PIXEL_ALIGN_Y) && (LCD_PIXEL_ALIGN_Y > 1)
    a->y1 = a->y1 & (~(LCD_PIXEL_ALIGN_Y - 1));
    a->y2 = a->y2 | (LCD_PIXEL_ALIGN_Y - 1);
#endif
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
