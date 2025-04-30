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
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
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
static lv_disp_draw_buf_t *draw_buf_p = NULL;

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

    /*Initialize the display buffer*/
    memset(&draw_buf_dsc, 0, sizeof(draw_buf_dsc));
    lv_disp_draw_buf_init(&draw_buf_dsc, draw_buf_1, draw_buf_2, DRAW_BUFF_SIZE / BYTE_PER_PIXEL);

    /*Basic initialization*/
    memset(&disp_drv_dsc, 0, sizeof(disp_drv_dsc));
    lv_disp_drv_init(&disp_drv_dsc);

    /*Set up the functions to access to your display*/

    /*Set a display buffer*/
    disp_drv_dsc.draw_buf = (lv_disp_draw_buf_t *)&draw_buf_dsc;

    /*Set the resolution of the display*/
    disp_drv_dsc.hor_res = LCD_W;
    disp_drv_dsc.ver_res = LCD_H;

    /* hardware rotation */
    disp_drv_dsc.sw_rotate = 1;

    /* full refresh mode */
    disp_drv_dsc.full_refresh = 1;

    /*Used to copy the buffer's content to the display*/
    disp_drv_dsc.flush_cb = disp_flush;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv_dsc);

#if LCD_LVGL_RGB_TRIPLE_BUFF_MODE
    draw_buf_p = &draw_buf_dsc;
#endif

    return &disp_drv_dsc;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Initialize your display and the required peripherals. */
void disp_init(void)
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

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    if (disp_flush_enabled == false) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    if (color_p == (void *)lcd_get_screen_using()) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

#if LCD_LVGL_RGB_TRIPLE_BUFF_MODE
    /* Triple buffer mode */
    /* lock */
    uintptr_t flag = bflb_irq_save();
    lcd_screen_switch((lcd_color_t *)color_p);
    last_lvgl_flush_p = color_p;
    /* unlock */
    bflb_irq_restore(flag);

#elif defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    lcd_screen_switch((lcd_color_t *)color_p);
    xSemaphoreTake(disp_sem, portMAX_DELAY);

#else
    swap_flag = 1;
    lcd_screen_switch((lcd_color_t *)color_p);
    while (swap_flag) {
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
    }
#endif

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/* lcd_screen_switch done cb */
void rgb_screen_frame_callback(void)
{
#if LCD_LVGL_RGB_TRIPLE_BUFF_MODE
    /* Triple buffer mode */
    void *screen_using = lcd_get_screen_using();

    if (screen_using != (void *)last_disp_buff_p) {
        if (last_lvgl_flush_p == draw_buf_p->buf1) {
            draw_buf_p->buf1 = (void *)last_disp_buff_p;
            last_disp_buff_p = screen_using;
        } else if (last_lvgl_flush_p == draw_buf_p->buf2) {
            draw_buf_p->buf2 = (void *)last_disp_buff_p;
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

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
