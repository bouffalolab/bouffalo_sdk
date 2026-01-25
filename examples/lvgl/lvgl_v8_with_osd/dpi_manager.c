/**
 * @file dpi_manager.c
 * @brief DPI Manager - Encapsulates DPI and MJDEC initialization and control logic
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "bl616d_glb.h"
#include "dpi_manager.h"
#include "bflb_dpi.h"
#include "bflb_mjdec.h"
#include "bflb_osd.h"
#include "bflb_gpio.h"
#include "bflb_wdg.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "filesystem_reader.h"
#include "log.h"

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "task.h"
#include "semphr.h"
#endif

/* DPI and MJDEC device pointers */
static struct bflb_device_s *mjdec_dev = NULL;
/* dpi_dev and osd_dev are defined in bl_mipi_dpi_v2.c */
extern struct bflb_device_s *dpi_dev;
extern struct bflb_device_s *osd_dev;

/* External watchdog device */
extern struct bflb_device_s *wdg;

struct bflb_mjdec_config_s mjdec_config;

/* Picture count flag for MJDEC ISR */
volatile uint32_t pic_count = 0;

/**
 * @brief Configure pixel clock output using software GPIO
 */
static void dpi_pixel_clock_output(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

/**
 * @brief Parse JPEG SOF0 header to get image info
 */
uint8_t dpi_parse_sof0(uint8_t *header, uint16_t *x, uint16_t *y, uint8_t *format, uint16_t *skip)
{
    uint32_t len;
    uint8_t *p = header;

    if (p[0] != 0xFF || p[1] != 0xD8) {
        /* not jpeg header */
        return MJDEC_ERR_HEADER_SOI;
    }
    p += 2;
    while (1) {
        if (p[0] != 0xFF) {
            /* JPEG header format error */
            printf("error: header marker should be started by 0xFF.\r\n");
            return MJDEC_ERR_HEADER_MARKER;
        }
        p++;
        len = ((p[1] << 8) | p[2]);
        if (p[0] == 0xC0) {
            if (p[3] != 8) {
                printf("accuracy should be 8.\r\n");
                return 0xF1;
            }
            *y = (p[4] << 8) | p[5];
            *x = (p[6] << 8) | p[7];
            if (p[8] == 1) {
                if (p[9] == 1 && p[10] == 0x11 && p[11] == 0) {
                    *format = MJDEC_FORMAT_GRAY;
                } else {
                    printf("gray image error, id=0x%02X, sample=0x%02X, dqt=0x%02X.\r\n", p[9], p[10], p[11]);
                    return 0xF1;
                }
            } else if (p[8] == 3) {
                if (p[9] == 1 && p[11] == 0 && p[12] == 2 && p[13] == 0x11 && p[14] == 1 && p[15] == 3 &&
                    p[16] == 0x11 && p[17] == 1) {
                    if (p[10] == 0x21) {
                        *format = MJDEC_FORMAT_YUV422SP_NV16;
                    } else if (p[10] == 0x22) {
                        *format = MJDEC_FORMAT_YUV420SP_NV12;
                    } else {
                        printf("component: %02X %02X %02X, %02X %02X %02X, %02X %02X %02X\r\n", p[9], p[10], p[11],
                               p[12], p[13], p[14], p[15], p[16], p[17]);
                        printf("only support NV16 or NV12 format.\r\n");
                        return 0xF1;
                    }
                }
            } else {
                printf("only support gray or YCbCr format.\r\n");
                return 0xF2;
            }
        } else if (p[0] == 0xDA) {
            /* start of scan or EOI or other invalid marker */
            *skip = (uint16_t)(p + len + 1 - header);
            break;
        }
        p = p + len + 1;
    }
    return 0;
}

/* External reference to the OSD ISR enable flag from bl_mipi_dpi_v2.c */
volatile uint8_t dpi_mjdec_isr_enable_flag = 0;
static volatile uint32_t osd_addr = 0;

/**
 * @brief MJDEC interrupt handler
 */
void dpi_mjdec_isr(int irq, void *arg)
{
    bflb_mjdec_int_clear(mjdec_dev, MJDEC_INTCLR_ONE_FRAME);
    dpi_mjdec_isr_enable_flag = 1;
}

/**
 * @brief Decode one JPEG frame using MJDEC
 */
void dpi_mjdec_decode_one_frame(uint8_t *jpg, uint8_t *output)
{
    uint16_t header_skip;
    uint16_t pic_x, pic_y;
    uint8_t yuv_format;
    uint8_t ret = 0;

    ret = dpi_parse_sof0(jpg, &pic_x, &pic_y, &yuv_format, &header_skip);
    if (ret) {
        printf("parse header error!\r\n");
    }
    mjdec_config.format = yuv_format;
    mjdec_config.swap_enable = true;
    mjdec_config.resolution_x = pic_x;
    mjdec_config.resolution_y = pic_y;
    if (header_skip & 0x7) {
        mjdec_config.head_size = header_skip;
    } else {
        mjdec_config.head_size = 0;
    }
    mjdec_config.output_bufaddr0 = (uint32_t)(&output[0]);
    mjdec_config.output_bufaddr1 = (uint32_t)(&output[pic_x * pic_y]);

    bflb_mjdec_init(mjdec_dev, &mjdec_config);
    bflb_mjdec_feature_control(mjdec_dev, MJDEC_CMD_SET_READ_BURST, MJDEC_BURST_INCR4);
    bflb_mjdec_feature_control(mjdec_dev, MJDEC_CMD_SET_WRITE_BURST, MJDEC_BURST_INCR4);
    bflb_mjdec_tcint_mask(mjdec_dev, false);
    ret = bflb_mjdec_set_dqt_from_header(mjdec_dev, jpg);
    if (ret) {
        printf("bflb_mjdec_set_dqt_from_header error, %d\r\n", ret);
    }
    ret = bflb_mjdec_set_dht_from_header(mjdec_dev, jpg);
    if (ret) {
        printf("bflb_mjdec_set_dht_from_header error, %d\r\n", ret);
    }

    bflb_mjdec_start(mjdec_dev);
    if (header_skip & 0x7) {
        bflb_mjdec_push_jpeg(mjdec_dev, jpg);
    } else {
        bflb_mjdec_push_jpeg(mjdec_dev, jpg + header_skip);
    }
}

/**
 * @brief Stop MJDEC and pop one frame
 */
void dpi_mjdec_stop(void)
{
    bflb_mjdec_stop(mjdec_dev);
    bflb_mjdec_pop_one_frame(mjdec_dev);
    bflb_mjdec_int_clear(mjdec_dev, MJDEC_INTCLR_ONE_FRAME);
}

/**
 * @brief Get picture count flag pointer
 */
volatile uint32_t *dpi_mjdec_get_pic_count(void)
{
    return &pic_count;
}

/**
 * @brief Initialize MJDEC only (DPI is initialized by lcd_init)
 *
 * Note: This function should be called AFTER lcd_init() since lcd_init()
 * will initialize the DPI hardware for YUV planar output when
 * LCD_DPI_V2_USE_OSD_LAYER_SWITCH is defined. The DPI configuration is
 * handled by bl_mipi_dpi_v2_init() in the BSP layer using parameters from
 * lcd_conf_user.h (STANDARD_DPI_W, STANDARD_DPI_H, timing params, etc.)
 *
 * @return int 0 on success, negative value on error
 */
int dpi_manager_init(void)
{
    /* Get MJDEC device */
    mjdec_dev = bflb_device_get_by_name("mjdec");
    if (mjdec_dev == NULL) {
        return -2;
    }

    /* Attach MJDEC interrupt */
    bflb_irq_attach(mjdec_dev->irq_num, dpi_mjdec_isr, NULL);
    bflb_irq_enable(mjdec_dev->irq_num);

    /* Get DPI device (for use with MJDEC framebuffer switching) */
    dpi_dev = bflb_device_get_by_name(BFLB_NAME_DPI);
    if (dpi_dev == NULL) {
        return -3;
    }

    /* Get OSD device */
    osd_dev = bflb_device_get_by_name(BFLB_NAME_OSD1);
    if (osd_dev == NULL) {
        return -4;
    }

#if (DPI_PIXEL_CLOCK_USE_SW_GPIO)
    dpi_pixel_clock_output();
#endif

    return 0;
}

#if defined(CONFIG_FREERTOS)

/* External YUV buffers from main.c */
extern uint8_t *yuv_images[];

/**
 * @brief Image switch task - processes JPEG buffers from filesystem
 * @param param Task parameter (unused)
 */
void image_switch_task(void *param)
{
    volatile uint32_t *pic_count;
    uint32_t last_count = 0;
    jpg_buffer_t *jpg_buffer;
    QueueHandle_t full_queue;
    QueueHandle_t empty_queue;
    uint8_t yuv_idx = 0;
    uint32_t reset_count = 0;

    /* Statistics counters */
    uint32_t timeout_count = 0;
    uint32_t success_count = 0;
    uint64_t time_stats;
    /* Runtime tracking */
    uint64_t start_time_ms;

    (void)param;

    vTaskDelay(100);

    pic_count = dpi_mjdec_get_pic_count();
    time_stats = bflb_mtimer_get_time_ms();
    start_time_ms = bflb_mtimer_get_time_ms();

    full_queue = filesystem_get_full_queue();
    empty_queue = filesystem_get_empty_queue();

    LOG_I("Image switch task started\r\n");

    while (1) {
        if (xQueueReceive(full_queue, &jpg_buffer, (TickType_t)100) == pdTRUE) {
            vTaskDelay(20);
            /* Decode JPEG */
            do {
                if (!dpi_mjdec_isr_enable_flag) {
                    dpi_mjdec_decode_one_frame(jpg_buffer->data, yuv_images[yuv_idx]);
                }
                /* Wait for decode complete */
                uint64_t time_start = bflb_mtimer_get_time_ms();
                while (*pic_count == last_count) {
                    if (bflb_mtimer_get_time_ms() - time_start >= 6) {
                        dpi_mjdec_stop();
                        timeout_count++;
                        reset_count++;
                        break;
                    }
                }
                if (reset_count > 10) {
                    dpi_mjdec_stop();
                    /* Reset mjdec */
                    GLB_MM_Software_Reset(GLB_MM_SW_MJDEC);
                    reset_count = 0;
                    printf("******  mjdec reset!  ******\r\n");
                }

            } while (*pic_count == last_count);

            last_count = *pic_count;
            reset_count = 0;

            dpi_mjdec_stop();
            xQueueSend(empty_queue, &jpg_buffer, (TickType_t)0);
            yuv_idx = (yuv_idx + 1) % 2;

            success_count++;

            /* Print statistics every 2 seconds */
            if (bflb_mtimer_get_time_ms() - time_stats >= 2000) {
                bflb_wdg_reset_countervalue(wdg);
                uint64_t elapsed_ms = bflb_mtimer_get_time_ms() - time_stats;
                uint64_t total_elapsed_ms = bflb_mtimer_get_time_ms() - start_time_ms;
                uint32_t hours = total_elapsed_ms / 3600000;
                uint32_t minutes = (total_elapsed_ms % 3600000) / 60000;
                uint32_t seconds = (total_elapsed_ms % 60000) / 1000;
                printf("Runtime: %02lu:%02lu:%02lu, Success: %d, Timeout: %d, FPS: %llu\r\n", hours, minutes, seconds,
                       success_count, timeout_count, success_count * 1000 / elapsed_ms);
                timeout_count = 0;
                success_count = 0;
                time_stats = bflb_mtimer_get_time_ms();
            }
        } else {
            printf("receive error!\r\n");
        }
    }
}
#endif
