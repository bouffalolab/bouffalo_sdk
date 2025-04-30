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
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
static void disp_flush_wait_cb(lv_disp_drv_t *disp_drv);
static void disp_rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area);
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

lv_disp_drv_t *lv_port_disp_init(void)
{
    /* Descriptor of a display buffer */
    static lv_disp_draw_buf_t draw_buf_dsc;
    /* Descriptor of a display driver */
    static lv_disp_drv_t disp_drv_dsc;

    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /* LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed your display drivers `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are three buffering configurations:
     * 1. Create ONE buffer with some rows:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer with some rows:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Create TWO screen-sized buffer:
     *      Similar to 2) but the buffer have to be screen sized. When LVGL is ready it will give the
     *      whole frame to display. This way you only need to change the frame buffer's address instead of
     *      copying the pixels.
     * */

    /* Initialize the display buffer */
    memset(&draw_buf_dsc, 0, sizeof(draw_buf_dsc));
    lv_disp_draw_buf_init(&draw_buf_dsc, draw_buf_1, draw_buf_2, DRAW_BUFF_SIZE / BYTE_PER_PIXEL);

    /* Register the display in LVGL */
    memset(&disp_drv_dsc, 0, sizeof(disp_drv_dsc));
    lv_disp_drv_init(&disp_drv_dsc); /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set a display buffer*/
    disp_drv_dsc.draw_buf = &draw_buf_dsc;

    /*Set the resolution of the display*/
    disp_drv_dsc.hor_res = LCD_W;
    disp_drv_dsc.ver_res = LCD_H;

    /*Used to copy the buffer's content to the display*/
    disp_drv_dsc.flush_cb = disp_flush;

    /* set flush_wait_cb */
    disp_drv_dsc.wait_cb = disp_flush_wait_cb;

    /* set rounder_cb */
    disp_drv_dsc.rounder_cb = disp_rounder_cb;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv_dsc);

    return &disp_drv_dsc;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Initialize your display and the required peripherals. */
void disp_init(void)
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

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
/* MCU LCD Common interface */
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    static uint8_t rotated_dir = 0;
    if (rotated_dir != disp_drv->rotated) {
        rotated_dir = disp_drv->rotated;
        lcd_set_dir(rotated_dir, 0);
    }

    if (disp_flush_enabled) {
        lcd_draw_picture_nonblocking(area->x1, area->y1, area->x2, area->y2, (lcd_color_t *)color_p);
    } else {
        lv_disp_flush_ready(disp_drv);
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
static void disp_flush_wait_cb(lv_disp_drv_t *disp_drv)
{
#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    xSemaphoreTake(disp_sem, portMAX_DELAY);
#else
    while (lcd_draw_is_busy()) {
    };
#endif

    lv_disp_flush_ready(disp_drv);
}

/* rounder_cb */
static void disp_rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
#if defined(LCD_PIXEL_ALIGN_X) && (LCD_PIXEL_ALIGN_X > 1)
    area->x1 = area->x1 & (~(LCD_PIXEL_ALIGN_X - 1));
    area->x2 = area->x2 | (LCD_PIXEL_ALIGN_X - 1);
#endif

#if defined(LCD_PIXEL_ALIGN_Y) && (LCD_PIXEL_ALIGN_Y > 1)
    area->y1 = area->y1 & (~(LCD_PIXEL_ALIGN_Y - 1));
    area->y2 = area->y2 | (LCD_PIXEL_ALIGN_Y - 1);
#endif
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
