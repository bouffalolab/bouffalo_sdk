#include <stdint.h>

#include "bflb_irq.h"
#include "bflb_dbi.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"

#include "hardware/dbi_reg.h"

#include "lcd.h"
#include "frame_queue.h"

#include <FreeRTOS.h>
#include <semphr.h>

#include "log.h"

#if !defined(LCD_DBI_INTERFACE_TYPE)
#error "The example code is only for LCD with DBI interface, please check your LCD config in menuconfig"
#endif

#if IS_ENABLED(CONFIG_PSRAM)
#define ATTR_FAST_CODE_SECTION ATTR_PSRAM_CODE_SECTION
#else
#define ATTR_FAST_CODE_SECTION ATTR_TCM_SECTION
#endif

static struct bflb_device_s *dbi_hd = NULL;

/*
    The CPU will directly write the converted data into the dbi fifo.
    The number of lines must be a multiple of LCD_BLOCK_NUM_A.
    The screen size must be larger than 480x320.
    The input pixels must be yuyv 640x480.
*/
#define LCD_BLOCK_NUM_A 10
ATTR_FAST_CODE_SECTION void yuyv422_640x480_to_yuv444_480x320_to_dbi_partition_fast(void *src, uint16_t y_start, uint16_t y_end)
{
    uint32_t dbi_send_buff[9]; /* 9-word, 12-pixel */
    uint8_t *p_src, *p_dst;

    uint32_t regval;
    uint32_t reg_base = dbi_hd->reg_base;

    uint16_t s_block_width, s_block_width_offset;
    uint16_t d_block_width;

    d_block_width = 480 / LCD_BLOCK_NUM_A;

    s_block_width = 640 / LCD_BLOCK_NUM_A;
    s_block_width_offset = s_block_width * (LCD_BLOCK_NUM_A - 1) * 2;

    bflb_irq_disable(dbi_hd->irq_num);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_YUV444);

    /* block loop */
    for (uint8_t bn = 0; bn < LCD_BLOCK_NUM_A; bn++) {
        lcd_draw_picture_blocking(d_block_width * bn, 0, d_block_width * (bn + 1) - 1, 320 - 1, NULL);

        p_src = (uint8_t *)src + bn * s_block_width * 2; /* column offset */
        p_dst = (void *)dbi_send_buff;

        /* column loop */
        for (uint16_t i = 0; i < 320; i++) {
            /* row loop */
            for (uint16_t j = 0; j < d_block_width; j += 12) {
                /* pixel 0,1,2 */
                p_dst[0] = p_src[0];
                p_dst[1] = p_src[1];
                p_dst[2] = p_src[3];

                p_dst[3] = p_src[2];
                p_dst[4] = p_src[1];
                p_dst[5] = p_src[3];

                p_dst[6] = p_src[4];
                p_dst[7] = p_src[5];
                p_dst[8] = p_src[7];

                /* pixel 3,4,5 */
                p_dst[9] = p_src[8];
                p_dst[10] = p_src[9];
                p_dst[11] = p_src[11];

                p_dst[12] = p_src[10];
                p_dst[13] = p_src[9];
                p_dst[14] = p_src[11];

                p_dst[15] = p_src[12];
                p_dst[16] = p_src[13];
                p_dst[17] = p_src[15];

                /* send to dbi fifo */
                do {
                    regval = getreg32(reg_base + DBI_FIFO_CONFIG_1_OFFSET);
                } while ((regval & DBI_TX_FIFO_CNT_MASK) >> DBI_TX_FIFO_CNT_SHIFT < 3);

                putreg32(dbi_send_buff[0], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[1], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[2], reg_base + DBI_FIFO_WDATA_OFFSET);

                /* pixel 6,7,8 */
                p_dst[18] = p_src[16];
                p_dst[19] = p_src[17];
                p_dst[20] = p_src[19];

                p_dst[21] = p_src[18];
                p_dst[22] = p_src[17];
                p_dst[23] = p_src[19];

                p_dst[24] = p_src[20];
                p_dst[25] = p_src[21];
                p_dst[26] = p_src[23];

                /* pixel 9,10,11 */
                p_dst[27] = p_src[24];
                p_dst[28] = p_src[25];
                p_dst[29] = p_src[27];

                p_dst[30] = p_src[26];
                p_dst[31] = p_src[25];
                p_dst[32] = p_src[27];

                p_dst[33] = p_src[28];
                p_dst[34] = p_src[29];
                p_dst[35] = p_src[31];

                p_src += (16 * 2);

                /* send to dbi fifo */
                do {
                    regval = getreg32(reg_base + DBI_FIFO_CONFIG_1_OFFSET);
                } while ((regval & DBI_TX_FIFO_CNT_MASK) >> DBI_TX_FIFO_CNT_SHIFT < 6);

                putreg32(dbi_send_buff[3], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[4], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[5], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[6], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[7], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[8], reg_base + DBI_FIFO_WDATA_OFFSET);
            }

            /* next row */
            p_src += s_block_width_offset;

            /*  */
            if (i % 2 == 1) {
                p_src += (640 * 2);
            }

            // bflb_mtimer_delay_ms(2);
        }

        /* Wait transfer complete */
        do {
            regval = bflb_dbi_get_intstatus(dbi_hd);
        } while ((regval & DBI_INTSTS_TC) == 0);
        bflb_dbi_int_clear(dbi_hd, DBI_INTCLR_TC);

        bflb_dbi_feature_control(dbi_hd, DBI_CMD_CLEAR_TX_FIFO, 0);
    }

    bflb_irq_enable(dbi_hd->irq_num);

#if (LCD_COLOR_DEPTH == 16)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
#elif (LCD_COLOR_DEPTH == 32)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
#endif
}

/*
    The CPU will directly write the converted data into the dbi fifo.
    Must not exceed the size of the screen. 
    The number of lines must be a multiple of LCD_BLOCK_NUM_B.
    The number of columns must be a multiple of 8.
*/
#define LCD_BLOCK_NUM_B 16
ATTR_FAST_CODE_SECTION void yuyv422_to_yuv444_dbi_to_dbi_partition_fast(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, void *src)
{
    uint32_t dbi_send_buff[6]; /* 6-word, 8-pixel */
    uint8_t *p_src, *p_dst;

    uint32_t regval;
    uint32_t reg_base = dbi_hd->reg_base;

    uint16_t width, height, block_width, block_width_offset;

    width = (x2 - x1 + 1);
    height = (y2 - y1 + 1);
    block_width = width / LCD_BLOCK_NUM_B;
    block_width_offset = block_width * (LCD_BLOCK_NUM_B - 1) * 2;

    if (x2 < x1 || y2 < y1 || width % (8 * LCD_BLOCK_NUM_B)) {
        return;
    }

    bflb_irq_disable(dbi_hd->irq_num);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_YUV444);

    /* block loop */
    for (uint8_t bn = 0; bn < LCD_BLOCK_NUM_B; bn++) {
        x2 = x1 + block_width - 1;
        lcd_draw_picture_blocking(x1, y1, x2, y2, NULL);
        x1 += block_width;

        p_src = (uint8_t *)src + bn * block_width * 2; /* column offset */
        p_dst = (void *)dbi_send_buff;

        /* column loop */
        for (uint16_t i = 0; i < height; i++) {
            /* row loop */
            for (uint16_t j = 0; j < block_width; j += 8) {
                /*  */
                p_dst[0] = p_src[0];
                p_dst[1] = p_src[1];
                p_dst[2] = p_src[3];
                p_dst[3] = p_src[2];
                p_dst[4] = p_src[1];
                p_dst[5] = p_src[3];

                p_dst[6] = p_src[4];
                p_dst[7] = p_src[5];
                p_dst[8] = p_src[7];
                p_dst[9] = p_src[6];
                p_dst[10] = p_src[5];
                p_dst[11] = p_src[7];

                p_dst[12] = p_src[8];
                p_dst[13] = p_src[9];
                p_dst[14] = p_src[11];
                p_dst[15] = p_src[10];
                p_dst[16] = p_src[9];
                p_dst[17] = p_src[11];

                p_dst[18] = p_src[12];
                p_dst[19] = p_src[13];
                p_dst[20] = p_src[15];
                p_dst[21] = p_src[14];
                p_dst[22] = p_src[13];
                p_dst[23] = p_src[15];

                /* next 8 pixel */
                p_src += (8 * 2);

                /* send to dbi fifo */
                do {
                    regval = getreg32(reg_base + DBI_FIFO_CONFIG_1_OFFSET);
                } while ((regval & DBI_TX_FIFO_CNT_MASK) >> DBI_TX_FIFO_CNT_SHIFT < 6);

                putreg32(dbi_send_buff[0], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[1], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[2], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[3], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[4], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[5], reg_base + DBI_FIFO_WDATA_OFFSET);
            }

            p_src += block_width_offset; /* next row */

            // bflb_mtimer_delay_ms(2);
        }

        /* Wait transfer complete */
        do {
            regval = bflb_dbi_get_intstatus(dbi_hd);
        } while ((regval & DBI_INTSTS_TC) == 0);
        bflb_dbi_int_clear(dbi_hd, DBI_INTCLR_TC);

        bflb_dbi_feature_control(dbi_hd, DBI_CMD_CLEAR_TX_FIFO, 0);
    }

    bflb_irq_enable(dbi_hd->irq_num);

#if (LCD_COLOR_DEPTH == 16)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
#elif (LCD_COLOR_DEPTH == 32)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
#endif
}

/*  
    The CPU will directly write the converted data into the dbi fifo.
    The screen size must be larger than 480x320.
    The input pixels must be yuyv 640x480.

    BL616:
    CPU-384M + DBI 40M RGB565 output : 10.91ms
    CPU-384M + DBI 40M RGB888 output : 11.57ms
    CPU-320M + DBI 40M RGB565 output : 12.54ms
    CPU-320M + DBI 40M RGB888 output : 12.54ms
    CPU-320M only conversion : 10.60ms
    CPU-384M only conversion : 9.29ms
    DBI-B 40M RGB888 output : 11.6ms
    DBI-B 40M RGB565 output : 7.7ms
*/
ATTR_FAST_CODE_SECTION void yuyv422_640x480_to_yuv444_480x320_to_dbi_fast(void *src, uint16_t y_start, uint16_t y_end)
{
    uint32_t dbi_send_buff[9];
    uint8_t *p_src, *p_dst;

    uint32_t regval;
    uint32_t reg_base = dbi_hd->reg_base;

    /* src to dst */
    y_start = y_start * 2 / 3;
    y_end = y_end * 2 / 3;

    bflb_irq_disable(dbi_hd->irq_num);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_YUV444);

    lcd_draw_picture_blocking(0, y_start, 480 - 1, y_end - 1, NULL);

    p_src = (uint8_t *)src;
    p_dst = (void *)dbi_send_buff;

    for (uint16_t i = y_start; i < y_end; i++) {
        /* 480 / 12 = 40 */
        for (uint16_t j = 0; j < 40; j++) {
            /* pixel 0,1,2 */
            p_dst[0] = p_src[0];
            p_dst[1] = p_src[1];
            p_dst[2] = p_src[3];

            p_dst[3] = p_src[2];
            p_dst[4] = p_src[1];
            p_dst[5] = p_src[3];

            p_dst[6] = p_src[4];
            p_dst[7] = p_src[5];
            p_dst[8] = p_src[7];

            /* pixel 3,4,5 */
            p_dst[9] = p_src[8];
            p_dst[10] = p_src[9];
            p_dst[11] = p_src[11];

            p_dst[12] = p_src[10];
            p_dst[13] = p_src[9];
            p_dst[14] = p_src[11];

            p_dst[15] = p_src[12];
            p_dst[16] = p_src[13];
            p_dst[17] = p_src[15];

            /* send to dbi fifo */
            do {
                regval = getreg32(reg_base + DBI_FIFO_CONFIG_1_OFFSET);
            } while ((regval & DBI_TX_FIFO_CNT_MASK) >> DBI_TX_FIFO_CNT_SHIFT < 3);

            putreg32(dbi_send_buff[0], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[1], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[2], reg_base + DBI_FIFO_WDATA_OFFSET);

            /* pixel 6,7,8 */
            p_dst[18] = p_src[16];
            p_dst[19] = p_src[17];
            p_dst[20] = p_src[19];

            p_dst[21] = p_src[18];
            p_dst[22] = p_src[17];
            p_dst[23] = p_src[19];

            p_dst[24] = p_src[20];
            p_dst[25] = p_src[21];
            p_dst[26] = p_src[23];

            /* pixel 9,10,11 */
            p_dst[27] = p_src[24];
            p_dst[28] = p_src[25];
            p_dst[29] = p_src[27];

            p_dst[30] = p_src[26];
            p_dst[31] = p_src[25];
            p_dst[32] = p_src[27];

            p_dst[33] = p_src[28];
            p_dst[34] = p_src[29];
            p_dst[35] = p_src[31];

            p_src += (16 * 2);

            /* send to dbi fifo */
            do {
                regval = getreg32(reg_base + DBI_FIFO_CONFIG_1_OFFSET);
            } while ((regval & DBI_TX_FIFO_CNT_MASK) >> DBI_TX_FIFO_CNT_SHIFT < 6);

            putreg32(dbi_send_buff[3], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[4], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[5], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[6], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[7], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[8], reg_base + DBI_FIFO_WDATA_OFFSET);
        }

        /* (320 / 480) = 2 / 3 */
        if (i % 2 == 1) {
            p_src += (640 * 2);
        }
    }

    /* Wait transfer complete */
    do {
        regval = bflb_dbi_get_intstatus(dbi_hd);
    } while ((regval & DBI_INTSTS_TC) == 0);
    bflb_dbi_int_clear(dbi_hd, DBI_INTCLR_TC);

    bflb_irq_enable(dbi_hd->irq_num);

#if (LCD_COLOR_DEPTH == 16)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
#elif (LCD_COLOR_DEPTH == 32)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
#endif
}

/* 
    The CPU will directly write the converted data into the dbi fifo. 
    Must not exceed the size of the screen. 
    The number of rows and columns must all be multiples of 8.
*/
ATTR_FAST_CODE_SECTION void yuyv422_to_yuv444_to_dbi_fast(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, void *src)
{
    uint32_t dbi_send_buff[6]; /* 6-word, 8-pixel */
    uint8_t *p_src, *p_dst;

    uint32_t regval;
    uint32_t reg_base = dbi_hd->reg_base;

    uint16_t width, height;

    if (x2 < x1 || y2 < y1 || (x2 - x1 + 1) % 8) {
        return;
    }

    width = (x2 - x1 + 1);
    height = (y2 - y1 + 1);

    bflb_irq_disable(dbi_hd->irq_num);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_YUV444);

    lcd_draw_picture_blocking(x1, y1, x2, y2, NULL);

    p_src = (uint8_t *)src;
    p_dst = (void *)dbi_send_buff;

    for (uint16_t i = 0; i < height; i++) {
        /*  */
        p_src = (uint8_t *)src + i * width * 2;

        for (uint16_t j = 0; j < width; j += 8) {
            /*  */
            p_dst[0] = p_src[0];
            p_dst[1] = p_src[1];
            p_dst[2] = p_src[3];
            p_dst[3] = p_src[2];
            p_dst[4] = p_src[1];
            p_dst[5] = p_src[3];

            p_dst[6] = p_src[4];
            p_dst[7] = p_src[5];
            p_dst[8] = p_src[7];
            p_dst[9] = p_src[6];
            p_dst[10] = p_src[5];
            p_dst[11] = p_src[7];

            p_dst[12] = p_src[8];
            p_dst[13] = p_src[9];
            p_dst[14] = p_src[11];
            p_dst[15] = p_src[10];
            p_dst[16] = p_src[9];
            p_dst[17] = p_src[11];

            p_dst[18] = p_src[12];
            p_dst[19] = p_src[13];
            p_dst[20] = p_src[15];
            p_dst[21] = p_src[14];
            p_dst[22] = p_src[13];
            p_dst[23] = p_src[15];

            /* next 8 pixel */
            p_src += (8 * 2);

            /* send to dbi fifo */
            do {
                regval = getreg32(reg_base + DBI_FIFO_CONFIG_1_OFFSET);
            } while ((regval & DBI_TX_FIFO_CNT_MASK) >> DBI_TX_FIFO_CNT_SHIFT < 6);

            putreg32(dbi_send_buff[0], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[1], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[2], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[3], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[4], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[5], reg_base + DBI_FIFO_WDATA_OFFSET);
        }
    }

    /* Wait transfer complete */
    do {
        regval = bflb_dbi_get_intstatus(dbi_hd);
    } while ((regval & DBI_INTSTS_TC) == 0);
    bflb_dbi_int_clear(dbi_hd, DBI_INTCLR_TC);

    bflb_irq_enable(dbi_hd->irq_num);

#if (LCD_COLOR_DEPTH == 16)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
#elif (LCD_COLOR_DEPTH == 32)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
#endif
}

/****************************************************************************************************************** */
/* DMA */
static SemaphoreHandle_t disp_sem = NULL;

/* flush done isr */
static void disp_flush_done_cb(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(disp_sem, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/* display dbi init */
void disp_dbi_init(void)
{
    /* lcd init */
    lcd_init();

    lcd_clear(LCD_COLOR_RGB(0x00, 0X00, 0X00));

    dbi_hd = bflb_device_get_by_name(BFLB_NAME_DBI);

    disp_sem = xSemaphoreCreateBinary();
    lcd_async_callback_register(disp_flush_done_cb);
}

/* 
    1: RGB565, 2: RGB888, 3: NRGB8888, 4: YUYV, 5: YUV,
*/
static int disp_pixel_format_switch(uint32_t pixel_format)
{
    if (pixel_format == IMG_RAW_FRAME_FORMAT_RGB565) {
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
    } else if (pixel_format == IMG_RAW_FRAME_FORMAT_RGB888) {
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_888);
    } else if (pixel_format == IMG_RAW_FRAME_FORMAT_NRGB8888) {
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
    }
#if defined(BL616CL)
    else if (pixel_format == IMG_RAW_FRAME_FORMAT_YUYV) {
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_YUYV);
    }
#endif
#if defined(BL616CL) || defined(BL616)
    else if (pixel_format == IMG_RAW_FRAME_FORMAT_YUV) {
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_YUV444);
    }
#endif
    else {
        LOG_W("Unsupported pixel format: %u\r\n", pixel_format);
        return -1;
    }

    return 0;
}

/* flush with dma to dbi, blocking until flush done */
void disp_flush_dma_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, void *picture, uint32_t pixel_format)
{
    if (disp_pixel_format_switch(pixel_format) < 0) {
        return;
    }

    xSemaphoreTake(disp_sem, 0);

    lcd_draw_picture_nonblocking(x1, y1, x2, y2, picture);

    xSemaphoreTake(disp_sem, portMAX_DELAY);

#if (LCD_COLOR_DEPTH == 16)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
#elif (LCD_COLOR_DEPTH == 32)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
#endif
}

/****************************************************************************************************************** */
/* TE */

#ifndef CONFIG_DISP_TX_TIME_US
/* 320x480 RGB888 @ 27MB/s -> 320*480*3/27000 ~= 17067us */
#define CONFIG_DISP_TX_TIME_US 17000U
#endif

#ifndef CONFIG_DISP_TE_BOUNDARY_US
#define CONFIG_DISP_TE_BOUNDARY_US 300U
#endif

static volatile uint32_t disp_te_last_us = 0;
static volatile uint32_t disp_te_period_us = 0;

/* te signal gpio isr, calculate the te period */
static void disp_te_gpio_isr(uint8_t pin)
{
    uint32_t now_us = (uint32_t)bflb_mtimer_get_time_us();
    if (disp_te_last_us == 0U) {
        disp_te_last_us = now_us;
        return;
    }

    uint32_t sample_us = now_us - disp_te_last_us;

    if (sample_us < 1000 || sample_us > 100000) {
        LOG_W("TE signal abnormal, sample_us: %u\r\n", sample_us);
        return;
    }

    if (disp_te_period_us == 0U) {
        disp_te_period_us = sample_us;
        disp_te_last_us = now_us;
    } else {
        disp_te_period_us = (disp_te_period_us * 7 + sample_us) / 8;
        disp_te_last_us = now_us;
    }
}

/* Get the minimum delay time to meet TE timing requirement, 0 means no need to delay. */
static uint32_t disp_te_get_min_delay_us(void)
{
    uint32_t te_last_us = disp_te_last_us;
    uint32_t te_period_us = disp_te_period_us;
    uint32_t w_s, w_e;
    uint32_t now_us = (uint32_t)bflb_mtimer_get_time_us();

    if (disp_te_last_us == 0 || disp_te_period_us == 0) {
        LOG_W("TE timing is not ready, te_last_us: %u, te_period_us: %u\r\n", te_last_us, te_period_us);
        return 0U;
    }

    /* calculate the valid window for TE, which is [w_s, w_e] after the start of TE period */
    if (CONFIG_DISP_TX_TIME_US < te_period_us) {
        w_s = te_period_us - CONFIG_DISP_TX_TIME_US;
        w_e = te_period_us;
    } else if (CONFIG_DISP_TX_TIME_US < te_period_us * 2) {
        w_s = 0;
        w_e = te_period_us * 2 - CONFIG_DISP_TX_TIME_US;
    } else {
        LOG_W("The LCD interface is too slow and does not meet the requirements of TE.\r\n");
        return 0U;
    }

    if (w_e - w_s <= 1000) {
        LOG_W("TE timing is too tight, w_s: %u, w_e: %u\r\n", w_s, w_e);
        return 0U;
    }

    /* add 500us margin */
    w_s += CONFIG_DISP_TE_BOUNDARY_US;
    w_e -= CONFIG_DISP_TE_BOUNDARY_US;

    /* calculate the phase of current time in TE period */
    uint32_t phase_us = (now_us - te_last_us) % te_period_us;
    uint32_t delay_us;
    if (phase_us > w_s && phase_us < w_e) {
        delay_us = 0;
    } else {
        if (phase_us < w_s) {
            delay_us = w_s - phase_us;
        } else {
            delay_us = w_e + te_period_us - phase_us;
        }
    }

    // LOG_I("TE timing, te_period_us: %u, w_s: %u, w_e: %u, phase_us: %u, delay_us: %u\r\n",
    //       te_period_us, w_s, w_e, phase_us, delay_us);

    return delay_us;
}

/* te signal gpio init */
void disp_te_init(uint8_t pin)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, pin, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN);
    bflb_gpio_int_init(gpio, pin, GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE);
    bflb_gpio_int_mask(gpio, pin, false);

    bflb_gpio_irq_attach(pin, disp_te_gpio_isr);
    bflb_irq_enable(gpio->irq_num);
}

void disp_te_delay(void)
{
    uint32_t delay_us = disp_te_get_min_delay_us();
    if (delay_us > 0) {
        vTaskDelay(pdMS_TO_TICKS((delay_us + 999) / 1000));
    }
}
