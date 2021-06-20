/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "lv_port_disp.h"
#include "bsp_il9341.h"

#include "hal_spi.h"
#include "hal_dma.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
void LCD_Init(void);

static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
#if LV_USE_GPU
static void gpu_blend(lv_disp_drv_t *disp_drv, lv_color_t *dest, const lv_color_t *src, uint32_t length, lv_opa_t opa);
static void gpu_fill(lv_disp_drv_t *disp_drv, lv_color_t *dest_buf, lv_coord_t dest_width,
                     const lv_area_t *fill_area, lv_color_t color);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static lv_disp_drv_t Disp_Drv; /*Descriptor of a display driver*/
static lv_disp_drv_t *p_disp_drv_cb;

void lv_port_disp_init(void)
{
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

    /* Example for 1) */
    // static lv_disp_buf_t draw_buf_dsc_1;
    // static lv_color_t draw_buf_1[LV_HOR_RES_MAX * 30];                          /*A buffer for 10 rows*/
    // lv_disp_buf_init(&draw_buf_dsc_1, draw_buf_1, NULL, LV_HOR_RES_MAX * 30);   /*Initialize the display buffer*/

    // /* Example for 2) */
    static lv_disp_buf_t draw_buf_dsc_2;
    static lv_color_t draw_buf_2_1[LV_HOR_RES_MAX * 20];                                /*A buffer for 10 rows*/
    static lv_color_t draw_buf_2_2[LV_HOR_RES_MAX * 20];                                /*An other buffer for 10 rows*/
    lv_disp_buf_init(&draw_buf_dsc_2, draw_buf_2_1, draw_buf_2_2, LV_HOR_RES_MAX * 20); /*Initialize the display buffer*/

    // /* Example for 3) *
    // static lv_disp_buf_t draw_buf_dsc_3;
    // static lv_color_t draw_buf_3_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*A screen sized buffer*/
    // static lv_color_t draw_buf_3_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*An other screen sized buffer*/
    // lv_disp_buf_init(&draw_buf_dsc_3, draw_buf_3_1, draw_buf_3_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_init(&Disp_Drv); /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    Disp_Drv.hor_res = 240;
    Disp_Drv.ver_res = 320;
    /* hardware rotation */
    Disp_Drv.sw_rotate = 0;
    /*  rotation */
    Disp_Drv.rotated = LV_DISP_ROT_NONE;

    /*Used to copy the buffer's content to the display*/
    Disp_Drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    Disp_Drv.buffer = &draw_buf_dsc_2;

#if LV_USE_GPU
    /*Optionally add functions to access the GPU. (Only in buffered mode, LV_VDB_SIZE != 0)*/

    /*Blend two color array using opacity*/
    disp_drv.gpu_blend_cb = gpu_blend;

    /*Fill a memory array with a color*/
    disp_drv.gpu_fill_cb = gpu_fill;
#endif

    /*Finally register the driver*/
    lv_disp_drv_register(&Disp_Drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
struct device *lcd_spi;
struct device *lcd_dma_tx;
struct device *lcd_dma_rx;

void flush_callback(struct device *dev, void *args, uint32_t size, uint32_t event)
{
    device_control(lcd_spi, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
    lv_disp_flush_ready(p_disp_drv_cb);
    CS1_HIGH;
}

/* Initialize your display and the required peripherals. */
void disp_init(void)
{
    /*You code here*/
    LCD_Init();
    lcd_spi = device_find("spi0");
    lcd_dma_tx = device_find("dma0_ch3");
    lcd_dma_rx = device_find("dma0_ch4");
    device_set_callback(lcd_dma_tx, flush_callback);
}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    static uint8_t rotated_dir = 0;
    uint32_t length = (area->y2 - area->y1 + 1) * (area->x2 - area->x1 + 1);

    if (rotated_dir != disp_drv->rotated) {
        rotated_dir = disp_drv->rotated;
        LCD_Set_Dir(rotated_dir);
    }

    LCD_Set_Addr(area->x1, area->y1, area->x2, area->y2);

    device_control(lcd_spi, DEVICE_CTRL_TX_DMA_RESUME, NULL);
    device_control(lcd_dma_tx, DEVICE_CTRL_SET_INT, NULL);
    CS1_LOW;
    DC_HIGH;
    dma_reload(lcd_dma_tx, (uint32_t)color_p, (uint32_t)DMA_ADDR_SPI_TDR, length * 2);
    dma_channel_start(lcd_dma_tx);

    p_disp_drv_cb = disp_drv;
    return;

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    //lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/
#if LV_USE_GPU

/* If your MCU has hardware accelerator (GPU) then you can use it to blend to memories using opacity
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_blend(lv_disp_drv_t *disp_drv, lv_color_t *dest, const lv_color_t *src, uint32_t length, lv_opa_t opa)
{
    /*It's an example code which should be done by your GPU*/
    uint32_t i;

    for (i = 0; i < length; i++) {
        dest[i] = lv_color_mix(dest[i], src[i], opa);
    }
}

/* If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_fill(lv_disp_drv_t *disp_drv, lv_color_t *dest_buf, lv_coord_t dest_width,
                     const lv_area_t *fill_area, lv_color_t color)
{
    /*It's an example code which should be done by your GPU*/
    int32_t x, y;
    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

    for (y = fill_area->y1; y <= fill_area->y2; y++) {
        for (x = fill_area->x1; x <= fill_area->x2; x++) {
            dest_buf[x] = color;
        }

        dest_buf += dest_width; /*Go to the next line*/
    }
}

#endif /*LV_USE_GPU*/

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
