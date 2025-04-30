#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_dbi.h"
#include "bflb_sf_ctrl.h"
#include "hardware/dbi_reg.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "lcd.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "dbi_disp.h"

#if (0)
#define DISP_DBG(a, ...) printf("[DISP dbg]:" a, ##__VA_ARGS__)
#else
#define DISP_DBG(a, ...)
#endif

#if (1)
#define DISP_INFO(a, ...) printf("[DISP info]:" a, ##__VA_ARGS__)
#else
#define DISP_INFO(a, ...)
#endif

#define DISP_ERR(a, ...) printf("[DISP err]:" a, ##__VA_ARGS__)

#if (LCD_INFO_DISP_ENABLE)

#if (defined(CONFIG_SOLUTION_FUNC_MJPEG) && CONFIG_SOLUTION_FUNC_MJPEG)
#include "mjpeg.h"
#endif

#if (defined(CONFIG_SOLUTION_FUNC_DVP) && CONFIG_SOLUTION_FUNC_DVP)
#include "dvp.h"
#endif

#if (defined(CONFIG_SOLUTION_FUNC_DVP_JPEG) && CONFIG_SOLUTION_FUNC_DVP_JPEG)
#include "dvp_mjpeg.h"
#endif

#if ((defined(CONFIG_SOLUTION_FUNC_UVC_YUYV) && CONFIG_SOLUTION_FUNC_UVC_YUYV) || \
     (defined(CONFIG_SOLUTION_FUNC_UVC_JPEG) && CONFIG_SOLUTION_FUNC_UVC_JPEG) || \
     (defined(CONFIG_SOLUTION_FUNC_UAC_IN) && CONFIG_SOLUTION_FUNC_UAC_IN))
#include "usbh_uvc_uac.h"
#endif

#endif

uint32_t g_dbi_disp_total_frame_cnt = 0;

static uint16_t yuyv_out_queue_disp_id;

static struct bflb_device_s *dbi_hd;
static struct bflb_device_s *gpio;

static TaskHandle_t dbi_disp_handle;

static EventGroupHandle_t disp_event_group = NULL;

#if (LCD_TE_ENABLE)

#if (YUYV_FRAME_SHARE_EN || YUYV_FRAME_NUM < 2)
#error TE cannot use a single buff
#endif

/* LCD tear effect wire, period(10~20ms) */
static uint32_t lcd_te_period_us = 0;
static uint32_t lcd_te_last_us = 0;

ATTR_TCM_SECTION static void lcd_te_gpio_isr(int irq, void *arg)
{
    /* clear int */
    bflb_gpio_int_clear(gpio, LCD_TE_PIN);

    uint32_t mtimer_us = (uint32_t)bflb_mtimer_get_time_us();

    if (lcd_te_last_us == 0) {
        /* first, only recorded the timestamp */
        lcd_te_last_us = mtimer_us;
    } else if (lcd_te_period_us == 0) {
        /* second, update the TE-period  */
        lcd_te_period_us = mtimer_us - lcd_te_last_us;
        lcd_te_last_us = mtimer_us;
    } else {
        /* update the TE-period with smooth filtering */
        lcd_te_period_us = (mtimer_us - lcd_te_last_us) / 4 + lcd_te_period_us * 3 / 4;
        lcd_te_last_us = mtimer_us;
    }

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(dbi_disp_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    /* set event bit */
    // xEventGroupSetBitsFromISR(disp_event_group, DISP_EVENT_TE_TRIG, NULL);
}

int lcd_te_get_time_us(void)
{
    if (lcd_te_last_us == 0 || lcd_te_period_us == 0) {
        return -1;
    }

    uint32_t diff_us = (uint32_t)bflb_mtimer_get_time_us() - lcd_te_last_us;

    return (int)diff_us;
}

#define LCD_BLOCK_NUM_A 10

ATTR_TCM_SECTION void yuyv422_640x480_scaling_yuv444_480x320_dbi_display_fast(void *src, uint16_t y_start, uint16_t y_end)
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

    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_BGR_888);
    // bflb_dbi_feature_control(dbi_hd, DBI_CMD_OUTPUT_PIXEL_FORMAT, DBI_PIXEL_OUTPUT_FORMAT_RGB_888);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, true);

    /* block loop */
    for (uint8_t bn = 0; bn < LCD_BLOCK_NUM_A; bn++) {
        lcd_draw_picture_blocking(d_block_width * bn, 0, d_block_width * (bn + 1) - 1, 320 - 1, NULL);

        p_src = src + bn * s_block_width * 2; /* column offset */
        p_dst = (void *)dbi_send_buff;

        /* column loop */
        for (uint16_t i = 0; i < 320; i++) {
            /* row loop */
            for (uint16_t j = 0; j < d_block_width; j += 12) {
                /* pixel 0,1,2 */
                p_dst[0] = p_src[0];
                p_dst[1] = p_src[3];
                // p_dst[2] = p_src[1];

                // p_dst[3] = p_src[2];
                // p_dst[4] = p_src[3];
                *(uint32_t *)&p_dst[2] = *(uint32_t *)&p_src[1];
                p_dst[5] = p_src[1];

                p_dst[6] = p_src[4];
                p_dst[7] = p_src[7];
                p_dst[8] = p_src[5];

                /* pixel 3,4,5 */
                p_dst[9] = p_src[8];
                p_dst[10] = p_src[11];
                p_dst[11] = p_src[9];

                // p_dst[12] = p_src[10];
                // p_dst[13] = p_src[11];
                *(uint16_t *)&p_dst[12] = *(uint16_t *)&p_src[10];
                p_dst[14] = p_src[9];

                p_dst[15] = p_src[12];
                p_dst[16] = p_src[15];
                p_dst[17] = p_src[13];

                /* send to dbi fifo */
                do {
                    regval = getreg32(reg_base + DBI_FIFO_CONFIG_1_OFFSET);
                } while ((regval & DBI_TX_FIFO_CNT_MASK) >> DBI_TX_FIFO_CNT_SHIFT < 3);

                putreg32(dbi_send_buff[0], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[1], reg_base + DBI_FIFO_WDATA_OFFSET);
                putreg32(dbi_send_buff[2], reg_base + DBI_FIFO_WDATA_OFFSET);

                /* pixel 6,7,8 */
                p_dst[18] = p_src[16];
                p_dst[19] = p_src[19];
                // p_dst[20] = p_src[17];

                // p_dst[21] = p_src[18];
                // p_dst[22] = p_src[19];
                *(uint32_t *)&p_dst[20] = *(uint32_t *)&p_src[17];
                p_dst[23] = p_src[17];

                p_dst[24] = p_src[20];
                p_dst[25] = p_src[23];
                p_dst[26] = p_src[21];

                /* pixel 9,10,11 */
                p_dst[27] = p_src[24];
                p_dst[28] = p_src[27];
                p_dst[29] = p_src[25];

                // p_dst[30] = p_src[26];
                // p_dst[31] = p_src[27];
                *(uint16_t *)&p_dst[30] = *(uint16_t *)&p_src[26];
                p_dst[32] = p_src[25];

                p_dst[33] = p_src[28];
                p_dst[34] = p_src[31];
                p_dst[35] = p_src[29];

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

    bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, false);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
}

#define LCD_BLOCK_NUM_B 16

ATTR_TCM_SECTION void yuyv422_to_yuv444_dbi_display_fast(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, void *src)
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

    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_BGR_888);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, true);

    /* block loop */
    for (uint8_t bn = 0; bn < LCD_BLOCK_NUM_B; bn++) {
        x2 = x1 + block_width - 1;
        lcd_draw_picture_blocking(x1, y1, x2, y2, NULL);
        x1 += block_width;

        p_src = src + bn * block_width * 2; /* column offset */
        p_dst = (void *)dbi_send_buff;

        /* column loop */
        for (uint16_t i = 0; i < height; i++) {
            /* row loop */
            for (uint16_t j = 0; j < block_width; j += 8) {
                /*  */
                p_dst[0] = p_src[0];
                p_dst[1] = p_src[3];
                // *(uint32_t *)&p_dst[2] = *(uint32_t *)&p_src[1];
                p_dst[2] = p_src[1];
                p_dst[3] = p_src[2];
                p_dst[4] = p_src[3];
                p_dst[5] = p_src[1];

                p_dst[6] = p_src[4];
                p_dst[7] = p_src[7];
                // *(uint32_t *)&p_dst[8] = *(uint32_t *)&p_src[5];
                p_dst[8] = p_src[5];
                p_dst[9] = p_src[6];
                p_dst[10] = p_src[7];
                p_dst[11] = p_src[5];

                p_dst[12] = p_src[8];
                p_dst[13] = p_src[11];
                // *(uint32_t *)&p_dst[14] = *(uint32_t *)&p_src[9];
                p_dst[14] = p_src[9];
                p_dst[15] = p_src[10];
                p_dst[16] = p_src[11];
                p_dst[17] = p_src[9];

                p_dst[18] = p_src[12];
                p_dst[19] = p_src[15];
                // *(uint32_t *)&p_dst[20] = *(uint32_t *)&p_src[13];
                p_dst[20] = p_src[13];
                p_dst[21] = p_src[14];
                p_dst[22] = p_src[15];
                p_dst[23] = p_src[13];

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

    bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, false);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
}

#else

/*
    CPU-384M + DBI 40M RGB565 output : 10.91ms
    CPU-384M + DBI 40M RGB888 output : 11.57ms
    CPU-320M + DBI 40M RGB565 output : 12.54ms
    CPU-320M + DBI 40M RGB888 output : 12.54ms
    CPU-320M only conversion : 10.60ms
    CPU-384M only conversion : 9.29ms
    DBI-B 40M RGB888 output : 11.6ms
    DBI-B 40M RGB565 output : 7.7ms
*/
ATTR_TCM_SECTION void yuyv422_640x480_scaling_yuv444_480x320_dbi_display_fast(void *src, uint16_t y_start, uint16_t y_end)
{
    uint32_t dbi_send_buff[9];
    uint8_t *p_src, *p_dst;

    uint32_t regval;
    uint32_t reg_base = dbi_hd->reg_base;

    /* src to dst */
    y_start = y_start * 2 / 3;
    y_end = y_end * 2 / 3;

    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_BGR_888);
    // bflb_dbi_feature_control(dbi_hd, DBI_CMD_OUTPUT_PIXEL_FORMAT, DBI_PIXEL_OUTPUT_FORMAT_RGB_888);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, true);
    lcd_draw_picture_blocking(0, y_start, 480 - 1, y_end - 1, NULL);

    p_src = src;
    p_dst = (void *)dbi_send_buff;

    for (uint16_t i = y_start; i < y_end; i++) {
        /* 480 / 12 = 40 */
        for (uint16_t j = 0; j < 40; j++) {
            /* pixel 0,1,2 */
            p_dst[0] = p_src[0];
            p_dst[1] = p_src[3];
            // p_dst[2] = p_src[1];

            // p_dst[3] = p_src[2];
            // p_dst[4] = p_src[3];
            *(uint32_t *)&p_dst[2] = *(uint32_t *)&p_src[1];
            p_dst[5] = p_src[1];

            p_dst[6] = p_src[4];
            p_dst[7] = p_src[7];
            p_dst[8] = p_src[5];

            /* pixel 3,4,5 */
            p_dst[9] = p_src[8];
            p_dst[10] = p_src[11];
            p_dst[11] = p_src[9];

            // p_dst[12] = p_src[10];
            // p_dst[13] = p_src[11];
            *(uint16_t *)&p_dst[12] = *(uint16_t *)&p_src[10];
            p_dst[14] = p_src[9];

            p_dst[15] = p_src[12];
            p_dst[16] = p_src[15];
            p_dst[17] = p_src[13];

            /* send to dbi fifo */
            do {
                regval = getreg32(reg_base + DBI_FIFO_CONFIG_1_OFFSET);
            } while ((regval & DBI_TX_FIFO_CNT_MASK) >> DBI_TX_FIFO_CNT_SHIFT < 3);

            putreg32(dbi_send_buff[0], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[1], reg_base + DBI_FIFO_WDATA_OFFSET);
            putreg32(dbi_send_buff[2], reg_base + DBI_FIFO_WDATA_OFFSET);

            /* pixel 6,7,8 */
            p_dst[18] = p_src[16];
            p_dst[19] = p_src[19];
            // p_dst[20] = p_src[17];

            // p_dst[21] = p_src[18];
            // p_dst[22] = p_src[19];
            *(uint32_t *)&p_dst[20] = *(uint32_t *)&p_src[17];
            p_dst[23] = p_src[17];

            p_dst[24] = p_src[20];
            p_dst[25] = p_src[23];
            p_dst[26] = p_src[21];

            /* pixel 9,10,11 */
            p_dst[27] = p_src[24];
            p_dst[28] = p_src[27];
            p_dst[29] = p_src[25];

            // p_dst[30] = p_src[26];
            // p_dst[31] = p_src[27];
            *(uint16_t *)&p_dst[30] = *(uint16_t *)&p_src[26];
            p_dst[32] = p_src[25];

            p_dst[33] = p_src[28];
            p_dst[34] = p_src[31];
            p_dst[35] = p_src[29];

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

    bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, false);
#if (LCD_COLOR_DEPTH == 32)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
#elif (LCD_COLOR_DEPTH == 16)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
#endif
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_CLEAR_TX_FIFO, 0);
}

ATTR_TCM_SECTION void yuyv422_to_yuv444_dbi_display_fast(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, void *src)
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

    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_BGR_888);
    // bflb_dbi_feature_control(dbi_hd, DBI_CMD_OUTPUT_PIXEL_FORMAT, DBI_PIXEL_OUTPUT_FORMAT_RGB_888);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, true);
    lcd_draw_picture_blocking(x1, y1, x2, y2, NULL);

    p_src = src;
    p_dst = (void *)dbi_send_buff;

    for (uint16_t i = 0; i < height; i++) {
        /*  */
        p_src = src + i * width * 2;

        for (uint16_t j = 0; j < width; j += 8) {
            /*  */
            p_dst[0] = p_src[0];
            p_dst[1] = p_src[3];
            // *(uint32_t *)&p_dst[2] = *(uint32_t *)&p_src[1];
            p_dst[2] = p_src[1];
            p_dst[3] = p_src[2];
            p_dst[4] = p_src[3];
            p_dst[5] = p_src[1];

            p_dst[6] = p_src[4];
            p_dst[7] = p_src[7];
            // *(uint32_t *)&p_dst[8] = *(uint32_t *)&p_src[5];
            p_dst[8] = p_src[5];
            p_dst[9] = p_src[6];
            p_dst[10] = p_src[7];
            p_dst[11] = p_src[5];

            p_dst[12] = p_src[8];
            p_dst[13] = p_src[11];
            // *(uint32_t *)&p_dst[14] = *(uint32_t *)&p_src[9];
            p_dst[14] = p_src[9];
            p_dst[15] = p_src[10];
            p_dst[16] = p_src[11];
            p_dst[17] = p_src[9];

            p_dst[18] = p_src[12];
            p_dst[19] = p_src[15];
            // *(uint32_t *)&p_dst[20] = *(uint32_t *)&p_src[13];
            p_dst[20] = p_src[13];
            p_dst[21] = p_src[14];
            p_dst[22] = p_src[15];
            p_dst[23] = p_src[13];

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

    bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, false);
#if (LCD_COLOR_DEPTH == 32)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_NRGB_8888);
#elif (LCD_COLOR_DEPTH == 16)
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
#endif
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_CLEAR_TX_FIFO, 0);
}

#endif

static ATTR_TCM_SECTION void dbi_disp_lcd_task(void *pvParameters)
{
    static uint64_t refresh_time_last;
    static uint16_t cnt = 0;

    int ret;
    pyuyv_frame_t disp_src_frame;
    char str_buff_total[128], str_buff[64];

    lcd_draw_str_ascii16(10, 10, LCD_COLOR_RGB(0x80, 0x80, 0xff), LCD_COLOR_RGB(0, 0, 0), (void *)"Wireless picture transmission test @BouffaloLab", 100);
    lcd_draw_str_ascii16(10, 30, LCD_COLOR_RGB(0xff, 0xff, 0xff), LCD_COLOR_RGB(0, 0, 0), (void *)"lcd init ok...", 100);
    lcd_draw_str_ascii16(10, 50, LCD_COLOR_RGB(0, 0xff, 0), LCD_COLOR_RGB(0, 0, 0), (void *)"wait disp frame src...", 100);

    vTaskDelay(100);

    /* enable display */
    xEventGroupSetBits(disp_event_group, DISP_EVENT_ENABLE_DISP);

    // disp_supend:

    /* wait resume dislpay */
    xEventGroupWaitBits(disp_event_group, DISP_EVENT_ENABLE_DISP, pdFALSE, pdTRUE, portMAX_DELAY);

    /* drop the first frame */
    ret = frame_queue_output_pop(g_yuyv_frame_ctrl, (frame_elem_t *)&disp_src_frame, yuyv_out_queue_disp_id, 0);
    if (ret >= 0) {
        frame_queue_output_free(g_yuyv_frame_ctrl, (frame_elem_t *)&disp_src_frame);
    }

    while (1) {
        /* get frame */
        ret = frame_queue_output_pop(g_yuyv_frame_ctrl, (frame_elem_t *)&disp_src_frame, yuyv_out_queue_disp_id, 1000);

        /* supend display */
        // if ((xEventGroupGetBits(disp_event_group) & DISP_EVENT_ENABLE_DISP) == 0) {
        //     if (ret >= 0) {
        //         frame_queue_output_free(g_yuyv_frame_ctrl, (frame_elem_t *)&disp_src_frame);
        //     }
        //     goto disp_supend;
        // }

        if (ret < 0) {
            DISP_INFO("yuyv pop timeout: %d, continue wait...\r\n", ret);
            continue;
        }

        DISP_DBG("yuyv pop: id %d, addr 0x%08X,\r\n", disp_src_frame.elem_base.frame_id, disp_src_frame.elem_base.frame_addr);

        uint32_t frame_size = (disp_src_frame.x_end - disp_src_frame.x_start + 1) * (disp_src_frame.y_end - disp_src_frame.y_start) * 2;
        bflb_l1c_dcache_invalidate_range(disp_src_frame.elem_base.frame_addr, frame_size);

#if (LCD_TE_ENABLE)

#if (defined(LCD_DBI_ILI9488) || defined(LCD_DBI_ILI9486))
        uint32_t mtimer_us = (uint32_t)bflb_mtimer_get_time_us();
        uint32_t te_time_us = mtimer_us - lcd_te_last_us;

        if (te_time_us < 2000) {
            vTaskDelay(2);
        } else if (te_time_us > 5 * 1000) {
            ulTaskNotifyTake(pdTRUE, 0);
            while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == 0) {
                DISP_ERR("lcd TE timeout\r\n");
            }
            vTaskDelay(2);
        }

#elif defined(LCD_DBI_NT35510)
        ulTaskNotifyTake(pdTRUE, 0);
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == 0) {
            DISP_ERR("lcd TE timeout\r\n");
        }
#endif

#endif

        refresh_time_last = bflb_mtimer_get_time_us();

        /* scaling and disp frame */
#if (defined(LCD_DBI_INTERFACE_TYPE) || defined(LCD_DBI_WORK_MODE))

#if ((LCD_W >= 480) && (LCD_H >= 640))
        yuyv422_to_yuv444_dbi_display_fast(disp_src_frame.x_start, disp_src_frame.y_start, disp_src_frame.x_end, disp_src_frame.y_end, disp_src_frame.elem_base.frame_addr);
#elif ((LCD_W >= 320) && (LCD_H >= 480))
        yuyv422_640x480_scaling_yuv444_480x320_dbi_display_fast(disp_src_frame.elem_base.frame_addr, disp_src_frame.y_start, disp_src_frame.y_end + 1);
#endif

#else

#warning "only support DBI interface"
#endif

        /* free frame */
        frame_queue_output_free(g_yuyv_frame_ctrl, (frame_elem_t *)&disp_src_frame);
        DISP_DBG("yuyv free: id %d, \r\n", disp_src_frame.elem_base.frame_id);

#if (LCD_INFO_DISP_ENABLE)
        /* disp info */
        sprintf(str_buff_total, "fps ");
        extern uint32_t g_dbi_disp_fps;
        sprintf(str_buff, "lcd:%2d", g_dbi_disp_fps);
        strcat(str_buff_total, str_buff);

#if (defined(CONFIG_SOLUTION_FUNC_DVP) && CONFIG_SOLUTION_FUNC_DVP)
        extern uint32_t g_dvp_fps;
        sprintf(str_buff, ", dvp:%2d", g_dvp_fps);
        strcat(str_buff_total, str_buff);
#endif

#if (defined(CONFIG_SOLUTION_FUNC_MJPEG) && CONFIG_SOLUTION_FUNC_MJPEG)
        extern uint32_t g_mjpeg_fps;
        sprintf(str_buff, ", mjpeg:%2d", g_mjpeg_fps);
        strcat(str_buff_total, str_buff);
#endif

#if (defined(CONFIG_SOLUTION_FUNC_DVP_JPEG) && CONFIG_SOLUTION_FUNC_DVP_JPEG)
        extern uint32_t g_dvp_mjpeg_fps;
        sprintf(str_buff, "dvp_mjpeg: %2d, ", g_dvp_mjpeg_fps);
        strcat(str_buff_total, str_buff);
#endif

#if ((defined(CONFIG_SOLUTION_FUNC_UVC_YUYV) && CONFIG_SOLUTION_FUNC_UVC_YUYV) || \
     (defined(CONFIG_SOLUTION_FUNC_UVC_JPEG) && CONFIG_SOLUTION_FUNC_UVC_JPEG))
        extern uint32_t g_uvc_fps;
        sprintf(str_buff, ", uvc:%2d", g_uvc_fps);
        strcat(str_buff_total, str_buff);
#endif

        lcd_draw_str_ascii16(4, lcd_max_y - 20, LCD_COLOR_RGB(0xA0, 0xA0, 0xff), LCD_COLOR_RGB(0x60, 0x60, 0x60), (void *)str_buff_total, 64);
#endif

        g_dbi_disp_total_frame_cnt++;

        if (cnt++ >= 100) {
            cnt = 0;
            uint32_t lcd_refresh_period_us = (uint32_t)(bflb_mtimer_get_time_us() - refresh_time_last);
#if (LCD_TE_ENABLE)
            DISP_INFO("te_period:%d, lcd_refresh:%d\r\n", lcd_te_period_us, lcd_refresh_period_us);
#else
            DISP_INFO("lcd_refresh:%d\r\n", lcd_refresh_period_us);
#endif
        }
    }
}

int dbi_disp_task_init(void)
{
    DISP_INFO("dbi_disp_init\r\n");

    gpio = bflb_device_get_by_name("gpio");

#ifdef LCD_BACKLIGHT_PWM_PIN
    /* lcd pwm */
    bflb_gpio_init(gpio, LCD_BACKLIGHT_PWM_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_reset(gpio, LCD_BACKLIGHT_PWM_PIN);
#endif

#if (defined(LCD_DBI_WORK_MODE) && (LCD_DBI_WORK_MODE == 3))
    /* flash/lcd switch */
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_reset(gpio, GPIO_PIN_29);

#if defined(CONFIG_PSRAM_COPY_CODE)
    /* dbi 8080 mode, psram mode, close xip */
    bflb_sf_ctrl_disable();
#endif
#endif

    /* lcd init */
    lcd_init();

    if (disp_event_group == NULL) {
        disp_event_group = xEventGroupCreate();
    }

#if (LCD_TE_ENABLE)
    /* TE gpio interrupt init */
    bflb_gpio_int_init(gpio, LCD_TE_PIN, GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE);
    bflb_gpio_int_mask(gpio, LCD_TE_PIN, false);
    bflb_irq_attach(gpio->irq_num, lcd_te_gpio_isr, gpio);
    bflb_irq_enable(gpio->irq_num);
#endif

    dbi_hd = bflb_device_get_by_name("dbi");
    lcd_set_dir(1, 0);

    lcd_clear(LCD_COLOR_RGB(0, 0, 0));

    /* create yuyv output queue */
    yuyv_out_queue_disp_id = YUYV_FRAME_STREAM_DISP_ID;
    if (frame_queue_output_create(g_yuyv_frame_ctrl, &yuyv_out_queue_disp_id, YUYV_FRAME_STREAM_DISP_DEPTH) < 0) {
        DISP_ERR("yuyv frame display out queue create failed\r\n");
        return -1;
    } else {
        DISP_INFO("yuyv frame display out queue ID: %d\r\n", yuyv_out_queue_disp_id);
    }

    /* dbi display task */
    xTaskCreate(dbi_disp_lcd_task, (char *)"disp_lcd_task", 512, NULL, 4, &dbi_disp_handle);

    return 0;
}
