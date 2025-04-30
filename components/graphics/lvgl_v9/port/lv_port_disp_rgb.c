/**
 * @file lv_port_disp_templ.c
 *
 */

#include "lcd.h"

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "lv_display_private.h"
#include <stdbool.h>

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define BYTE_PER_PIXEL (LV_COLOR_DEPTH / 8)

#ifndef LCD_LVGL_CUSTOM_DRAW_BUFF
/* default buff def */
#ifdef CONFIG_PSRAM
#define DRAW_BUFF_ATTR __ALIGNED(64) ATTR_NOINIT_PSRAM_SECTION
#else
#define DRAW_BUFF_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION
#endif
#define DRAW_BUFF_SIZE (LCD_W * LCD_H * BYTE_PER_PIXEL)

#else
/* custom buffer def */
#define DRAW_BUFF_SIZE LCD_LVGL_CUSTOM_DRAW_BUFF_SIZE
#endif

/* Triple buffer mode, An additional video memory is required, for better performance */
#ifndef LCD_LVGL_RGB_TRIPLE_BUFF_MODE
#define LCD_LVGL_RGB_TRIPLE_BUFF_MODE 1
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void rgb_screen_frame_callback(void);

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

#if LCD_LVGL_RGB_TRIPLE_BUFF_MODE
/* Third frame buffer */
#ifndef LCD_LVGL_CUSTOM_DRAW_BUFF
static uint8_t DRAW_BUFF_ATTR draw_buf_3[DRAW_BUFF_SIZE];
#else
static LCD_LVGL_CUSTOM_DRAW_BUFF(3, draw_buf_3);
#endif
static volatile void *last_disp_buff_p = (void *)draw_buf_3;
static volatile void *last_lvgl_flush_p = NULL;
static lv_display_t *displpay_drv_p = NULL;

#elif defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
/* freeRTOS */
SemaphoreHandle_t disp_sem = NULL;

#else
static volatile uint8_t swap_flag = 0;
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
     * Two buffers screen sized buffer for double buffering.
     * Both LV_DISPLAY_RENDER_MODE_DIRECT and LV_DISPLAY_RENDER_MODE_FULL works, see their comments*/
    lv_display_set_buffers(disp, draw_buf_1, draw_buf_2, DRAW_BUFF_SIZE, LV_DISPLAY_RENDER_MODE_FULL);

#if LCD_LVGL_RGB_TRIPLE_BUFF_MODE
    displpay_drv_p = disp;
#endif

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
#if LCD_LVGL_RGB_TRIPLE_BUFF_MODE
    lcd_clear((lcd_color_t *)draw_buf_3, LCD_COLOR_RGB(0x00, 0x00, 0x00));
    lcd_init((lcd_color_t *)draw_buf_3);
    last_disp_buff_p = draw_buf_3;
#else
    lcd_clear((lcd_color_t *)draw_buf_2, LCD_COLOR_RGB(0x00, 0x00, 0x00));
    lcd_init((lcd_color_t *)draw_buf_2);

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    disp_sem = xSemaphoreCreateBinary();
#endif

#endif

    lcd_frame_callback_register(FRAME_INT_TYPE_SWAP, rgb_screen_frame_callback);
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
    if (disp_flush_enabled == false) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    if (px_map == (void *)lcd_get_screen_using()) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

#if LCD_LVGL_RGB_TRIPLE_BUFF_MODE
    /* Triple buffer mode */
    /* lock */
    uintptr_t flag = bflb_irq_save();
    lcd_screen_switch((lcd_color_t *)px_map);
    last_lvgl_flush_p = px_map;
    /* unlock */
    bflb_irq_restore(flag);

#elif defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    lcd_screen_switch((lcd_color_t *)px_map);
    xSemaphoreTake(disp_sem, portMAX_DELAY);

#else
    swap_flag = 1;
    lcd_screen_switch((lcd_color_t *)px_map);
    while (swap_flag) {
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
    }
#endif

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp_drv);
}

/* lcd_screen_switch done cb */
static void rgb_screen_frame_callback(void)
{
#if LCD_LVGL_RGB_TRIPLE_BUFF_MODE
    /* Triple buffer mode */
    void *screen_using = lcd_get_screen_using();

    if (screen_using != (void *)last_disp_buff_p) {
        if (last_lvgl_flush_p == displpay_drv_p->buf_1->data) {
            displpay_drv_p->buf_1->data = (void *)last_disp_buff_p;
            last_disp_buff_p = screen_using;
        } else if (last_lvgl_flush_p == displpay_drv_p->buf_2->data) {
            displpay_drv_p->buf_2->data = (void *)last_disp_buff_p;
            last_disp_buff_p = screen_using;
        }
    }

#elif defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    /* freeRTOS */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(disp_sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

#else
    swap_flag = 0;
#endif
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
