/**
 * @file dpi_manager.c
 * @brief Video pipeline: MJDEC decode + ping-pong YUV background + (sub-panel) DMA2D composite.
 *        The DSI/DPI display side is already up (lcd_init()) before dpi_manager_init() runs.
 */

#include "dpi_manager.h"
#include "bflb_dpi.h"
#include "bflb_osd.h"
#include "bflb_mjdec.h"
#include "bflb_dma2d.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bflb_irq.h"
#include "bflb_core.h"
#include "bl618dg_glb.h" /* GLB_MM_Software_Reset for the MJDEC recovery path */
#include "board.h"
#include "log.h"
#include "lcd.h"           /* _LCD_FUNC_DEFINE(get_timing) -> active panel timing */
#include "mipi_dsi_v2.h"   /* mipi_dsi_v2_timing_t */
#include <string.h>

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "queue.h"
#include "task.h"
#include "semphr.h"
#endif

#if defined(CONFIG_FREERTOS)
static QueueHandle_t pool_queue;
static QueueHandle_t out_queue;

int frame_buffer_pool_init(jpg_buffer_t *buffers, uint8_t *data, uint32_t buffer_count, size_t buffer_size)
{
    pool_queue = xQueueCreate(buffer_count, sizeof(jpg_buffer_t *));
    if (pool_queue == NULL) {
        return -1;
    }

    out_queue = xQueueCreate(buffer_count, sizeof(jpg_buffer_t *));
    if (out_queue == NULL) {
        vQueueDelete(pool_queue);
        pool_queue = NULL;
        return -1;
    }

    for (uint32_t i = 0; i < buffer_count; i++) {
        buffers[i].data = data + i * buffer_size;
        buffers[i].size = 0;
        frame_buffer_release(&buffers[i]);
    }

    return 0;
}

int frame_buffer_get(jpg_buffer_t **buffer, TickType_t timeout)
{
    return xQueueReceive(pool_queue, buffer, timeout) == pdTRUE ? 0 : -1;
}

int frame_buffer_push(jpg_buffer_t *buffer, TickType_t timeout)
{
    return xQueueSend(out_queue, &buffer, timeout) == pdTRUE ? 0 : -1;
}

int frame_buffer_pop(jpg_buffer_t **buffer, TickType_t timeout)
{
    return xQueueReceive(out_queue, buffer, timeout) == pdTRUE ? 0 : -1;
}

int frame_buffer_release(jpg_buffer_t *buffer)
{
    return xQueueSend(pool_queue, &buffer, 0) == pdTRUE ? 0 : -1;
}
#endif

#if !VIDEO_FULLSCREEN
/* Derive burst + transfer width from the transferred row byte count */
static void dma2d_pick_burst_width(uint32_t src_x_start, uint32_t src_x_end, uint8_t pixel_width, uint8_t *burst, uint8_t *transfer_width)
{
    uint32_t span_bytes = (src_x_end - src_x_start) * pixel_width;

    *burst = (span_bytes % 16 == 0) ? DMA2D_BURST_INCR16 : DMA2D_BURST_INCR4;

    if (span_bytes % 4 == 0) {
        *transfer_width = DMA2D_DATA_WIDTH_32BIT;
    } else if (span_bytes % 2 == 0) {
        *transfer_width = DMA2D_DATA_WIDTH_16BIT;
    } else {
        *transfer_width = DMA2D_DATA_WIDTH_8BIT;
    }
}
#endif

#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI)
#include "bl_mipi_dpi_v2.h"

/* dpi_dev and osd_dev are defined in bl_mipi_dpi_v2.c (the BSP owns them). */
extern struct bflb_device_s *dpi_dev;
extern struct bflb_device_s *osd_dev;

static struct bflb_device_s *mjdec_dev = NULL;

/* The registered OSD SEOF callback latches mjdec_config's Y/UV addresses,
 * pic_count counts latched frames, and dpi_mjdec_isr_enable_flag arms the latch. */
volatile struct bflb_mjdec_config_s mjdec_config;
volatile uint32_t pic_count = 0;
uint8_t dpi_mjdec_isr_enable_flag = 0;

/* Double-buffered panel-sized YUV background (NV12/NV16: <=2 B/px). Per buffer: Y plane
 * (YUV_Y_PLANE_SIZE = MCU-padded, see dpi_manager.h) then UV plane; the planar switch gets
 * Y at base, UV at base+YUV_Y_PLANE_SIZE. */
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t yuv_buffer_0[YUV_BUFFER_SIZE];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t yuv_buffer_1[YUV_BUFFER_SIZE];
static uint8_t *yuv_images[] = { yuv_buffer_0, yuv_buffer_1 };

/* Strong override of the BSP's weak base-layer swap (bl_mipi_dpi_v2.c). Invoked
 * from the OSD SEOF ISR: latch the freshly decoded YUV frame into the DPI base
 * layer at the frame boundary, so the video background switches between frames. */
void bl_mipi_dpi_v2_osd0_base_layer_swap(void)
{
    if (dpi_mjdec_isr_enable_flag) {
        bflb_dpi_framebuffer_planar_switch(dpi_dev, mjdec_config.output_bufaddr0, mjdec_config.output_bufaddr1);
        pic_count++;
        dpi_mjdec_isr_enable_flag = 0;
    }
}

#if !VIDEO_FULLSCREEN
/* Sub-panel (centered) video: MJDEC decodes the small frame into a compact scratch, then
 * DMA2D stride-copies it into the centered window of the panel buffer (borders stay black). */
static struct bflb_device_s *dma2d;
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t y_decode[VIDEO_WIDTH_MCU * VIDEO_HEIGHT_MCU];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t uv_decode[VIDEO_WIDTH_MCU * VIDEO_HEIGHT_MCU / 2];
#if defined(CONFIG_FREERTOS)
/* Centered mode: the OSD ISR must not latch the compact scratch, so MJDEC-done wakes the
 * task via this sem; the task raises the flag only after the composite, aimed at the panel buffer. */
static SemaphoreHandle_t frame_done_sem;
static SemaphoreHandle_t dma2d_done_sem;
#endif
#endif

static uint8_t dpi_parse_sof0(uint8_t *header, uint16_t *x, uint16_t *y, uint8_t *format, uint16_t *skip)
{
    uint32_t len;
    uint8_t *p = header;

    if (p[0] != 0xFF || p[1] != 0xD8) {
        return MJDEC_ERR_HEADER_SOI;
    }
    p += 2;
    while (1) {
        if (p[0] != 0xFF) {
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
                        printf("only support NV16 or NV12 format.\r\n");
                        return 0xF1;
                    }
                }
            } else {
                printf("only support gray or YCbCr format.\r\n");
                return 0xF2;
            }
        } else if (p[0] == 0xDA) {
            *skip = (uint16_t)(p + len + 1 - header);
            break;
        }
        p = p + len + 1;
    }
    return 0;
}

static void dpi_mjdec_decode_one_frame(uint8_t *jpg, uint8_t *y_buf, uint8_t *uv_buf)
{
    uint16_t header_skip;
    uint16_t pic_x = 0, pic_y = 0;
    uint8_t yuv_format = MJDEC_FORMAT_YUV420SP_NV12;
    uint8_t ret = 0;

    ret = dpi_parse_sof0(jpg, &pic_x, &pic_y, &yuv_format, &header_skip);
    if (ret) {
        printf("parse header error!\r\n");
        return;
    }
    mjdec_config.format = yuv_format;
    mjdec_config.swap_enable = true;
    mjdec_config.resolution_x = pic_x;
    mjdec_config.resolution_y = pic_y;
    mjdec_config.head_size = (header_skip & 0x7) ? header_skip : 0;
    mjdec_config.output_bufaddr0 = (uint32_t)y_buf;
    mjdec_config.output_bufaddr1 = (uint32_t)uv_buf;

    bflb_mjdec_init(mjdec_dev, (struct bflb_mjdec_config_s *)&mjdec_config);
    
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

static void dpi_mjdec_stop(void)
{
    bflb_mjdec_stop(mjdec_dev);
    bflb_mjdec_pop_one_frame(mjdec_dev);
    bflb_mjdec_int_clear(mjdec_dev, MJDEC_INTCLR_ONE_FRAME);
}

/* MJDEC done: clear + raise the flag; the registered OSD SEOF callback does the
 * planar switch and bumps pic_count. Centered mode wakes the task instead. */
static void dpi_mjdec_isr(int irq, void *arg)
{
    (void)irq;
    (void)arg;
    bflb_mjdec_int_clear(mjdec_dev, MJDEC_INTCLR_ONE_FRAME);
#if VIDEO_FULLSCREEN
    dpi_mjdec_isr_enable_flag = 1;
#elif defined(CONFIG_FREERTOS)
    BaseType_t hp_woken = pdFALSE;
    xSemaphoreGiveFromISR(frame_done_sem, &hp_woken);
    portYIELD_FROM_ISR(hp_woken);
#endif
}

#if !VIDEO_FULLSCREEN
/* DMA2D transfer-complete: wake the task waiting on dma2d_done_sem. */
static void dma2d_done_cb(void *arg)
{
    (void)arg;
#if defined(CONFIG_FREERTOS)
    BaseType_t hp_woken = pdFALSE;
    xSemaphoreGiveFromISR(dma2d_done_sem, &hp_woken);
    portYIELD_FROM_ISR(hp_woken);
#endif
}

/* Blit one NV12 plane via DMA2D TRANSLATE: compact w*h source -> centered (dx,dy) window of a
 * panel-stride dest. Blocks on TC. NV12 is byte-addressed, so pixel_data_width=1 handles any offset. */
static int dma2d_blit_plane(const uint8_t *src, uint8_t *dst, uint32_t dst_stride,
                             uint32_t w, uint32_t h, uint32_t dx, uint32_t dy)
{
    /* NV12 is byte-addressed, so pixel_data_width=1 handles any (dx,dy) offset. MJDEC writes whole
     * MCUs, so each source row is MCU-aligned (e.g. 172 -> 176). */
    uint8_t pixel_width = 1;
    uint32_t src_stride = MJDEC_MCU_ALIGN(w);
    uint8_t burst, transfer_width;

    /* Burst + transfer width are derived here (no longer taken from the panel driver) from the
     * transferred row byte count (src_x_end - src_x_start) * pixel_width (= w * 1 for NV12):
     * 16-aligned byte span -> INCR16 else INCR4; byte span divisible by 4/2/1 -> 32/16/8-bit. */
    dma2d_pick_burst_width(0, w, pixel_width, &burst, &transfer_width);

    struct bflb_dma2d_channel_config_s cfg = {
        .next_lli_addr = 0,
        .control = {
            .bits = {
                .transfer_size = 0, /* 0 -> DMA2D 2D-loop mode (not plain DMA) */
                .src_burst = burst,
                .dst_burst = burst,
                .src_incr = 1,
                .dst_incr = 1,
                .int_enable = 1,
            },
        },
    };
    struct bflb_dma2d_image_s img = {
        .transfer_data_width = transfer_width,
        .pixel_data_width = pixel_width,
        .src_image_addr = (uint32_t)src,
        .src_image_width = src_stride, /* source row stride: MJDEC writes MCU-aligned (e.g. 172 -> 176) */
        .src_x_start = 0,
        .src_y_start = 0,
        .src_x_end = w, /* exclusive end: crop to width, dropping the right MCU-padding cols */
        .src_y_end = h,
        .dst_image_addr = (uint32_t)dst,
        .dst_image_width = dst_stride,
        .dst_x_start = dx,
        .dst_y_start = dy,
    };

    bflb_dma2d_image_geometric_transfor_calculate(dma2d, &cfg, &img, DMA2D_IMAGE_TRANSLATE);
    bflb_dma2d_channel_init(dma2d, &cfg);
    bflb_dma2d_channel_start(dma2d);

#if defined(CONFIG_FREERTOS)
    if (xSemaphoreTake(dma2d_done_sem, pdMS_TO_TICKS(200)) != pdTRUE) {
        bflb_dma2d_channel_tcint_clear(dma2d);
        bflb_dma2d_channel_stop(dma2d);
        return -1;   
    }
#else
    while (bflb_dma2d_channel_isbusy(dma2d)) {
    }
    bflb_dma2d_channel_tcint_clear(dma2d);
#endif
    bflb_dma2d_channel_stop(dma2d);
    return 0;
}

/* Composite the decoded frame into the centered window of a panel buffer via DMA2D (all in
 * PSRAM -> no dcache work). Y at the buffer base, UV at +LCD_PIXELS (NV12: half height). */
static int compose_centered(uint8_t *yuv_dst)
{
    uint8_t *y_dst = yuv_dst;
    uint8_t *uv_dst = yuv_dst + YUV_Y_PLANE_SIZE;
    if (dma2d_blit_plane(y_decode, y_dst, LCD_WIDTH,
                         VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_OFFSET_X, VIDEO_OFFSET_Y) < 0)
        return -1;
    if (dma2d_blit_plane(uv_decode, uv_dst, LCD_WIDTH,
                         VIDEO_WIDTH, VIDEO_HEIGHT / 2, VIDEO_OFFSET_X, VIDEO_OFFSET_Y / 2) < 0)
        return -1;
    return 0;
}
#endif /* !VIDEO_FULLSCREEN */

int dpi_manager_init(void)
{
    /* The DPI controller + OSD0 blend layer are already up (lcd_init() -> the panel's
     * _dpi_init()). This owns only the video pipeline: MJDEC + the YUV background buffers. */
    LOG_I("[1] Init MJDEC (DPI)\r\n");
    mjdec_dev = bflb_device_get_by_name("mjdec");
    if (mjdec_dev == NULL) {
        return -2;
    }
    bflb_irq_attach(mjdec_dev->irq_num, dpi_mjdec_isr, NULL);
    bflb_irq_enable(mjdec_dev->irq_num);

    /* Reset before start */
    GLB_MM_Software_Reset(GLB_MM_SW_MJDEC);

    /* dpi_dev / osd_dev are created by the BSP; fetch handles for completeness. */
    dpi_dev = bflb_device_get_by_name(BFLB_NAME_DPI);
    if (dpi_dev == NULL) {
        return -3;
    }
    osd_dev = bflb_device_get_by_name(BFLB_NAME_OSD0);
    if (osd_dev == NULL) {
        return -4;
    }
    /* The base-layer swap is wired via the weak-override bl_mipi_dpi_v2_osd0_base_layer_swap()
     * above; no explicit callback registration needed. */

#if !VIDEO_FULLSCREEN
    /* Centered sub-panel video: DMA2D for the composite + done semaphores. */
    LOG_I("[1b] Init DMA2D (centered composite)\r\n");
    dma2d = bflb_device_get_by_name(BFLB_NAME_DMA2D_CH0);
    if (dma2d == NULL) {
        return -6;
    }
#if defined(CONFIG_FREERTOS)
    frame_done_sem = xSemaphoreCreateBinary();
    dma2d_done_sem = xSemaphoreCreateBinary();
    if (frame_done_sem == NULL || dma2d_done_sem == NULL) {
        return -7;
    }
#endif
    bflb_dma2d_channel_irq_attach(dma2d, dma2d_done_cb, NULL);

    /* Clear both panel buffers to black: Y=0, UV=0x80 (neutral chroma). The
     * composite only overwrites the centered window, so the borders stay black. */
    LOG_I("[2] Clear video background framebuffers\r\n");
    for (int i = 0; i < 2; i++) {
        memset(yuv_images[i], 0, YUV_Y_PLANE_SIZE);              /* Y plane -> black */
        memset(yuv_images[i] + YUV_Y_PLANE_SIZE, 0x80,
               YUV_BUFFER_SIZE - YUV_Y_PLANE_SIZE);              /* UV plane -> neutral */
        bflb_l1c_dcache_clean_range(yuv_images[i], YUV_BUFFER_SIZE);
    }
#endif
    return 0;
}

#if defined(CONFIG_FREERTOS)
void image_switch_task(void *param)
{
    volatile uint32_t *pcount;
    uint32_t last_count = 0;
    jpg_buffer_t *jpg_buffer;
    uint8_t yuv_idx = 0;
    uint32_t reset_count = 0;
    uint32_t timeout_count = 0, success_count = 0;
    uint64_t time_stats;
    const TickType_t frame_period_ticks = pdMS_TO_TICKS(VIDEO_FRAME_PERIOD_MS);
    TickType_t last_frame_wake;

    (void)param;
    vTaskDelay(100);
    pcount = &pic_count;
    time_stats = bflb_mtimer_get_time_ms();
    last_frame_wake = xTaskGetTickCount();

    LOG_I("image switch task started (DPI), target=%u fps\r\n", (unsigned)VIDEO_TARGET_FPS);

    while (1) {
        if (frame_buffer_pop(&jpg_buffer, (TickType_t)100) != 0) {
            continue;
        }

        /* MJDEC reads the JPEG via DMA from PSRAM: make sure it is in memory */
        bflb_l1c_dcache_clean_range(jpg_buffer->data, jpg_buffer->size);

#if VIDEO_FULLSCREEN
        /* Full-screen: decode straight into the panel-sized scan-out buffer; the
         * MJDEC-done ISR raises the flag so the OSD SEOF ISR latches it. */
        do {
            if (!dpi_mjdec_isr_enable_flag) {
                dpi_mjdec_decode_one_frame(jpg_buffer->data, yuv_images[yuv_idx],
                                           yuv_images[yuv_idx] + YUV_Y_PLANE_SIZE);
            }
            /* Wait for the OSD SEOF ISR to latch the decoded frame (pic_count++).
             * vTaskDelay(1) yields to the lower-priority lvgl_task while we wait a
             * full panel frame (~16ms) for the latch -- a bare spin here at MAX-2
             * starves the OSD/LVGL render (matches the centered path's wait). */
            uint64_t t_start = bflb_mtimer_get_time_ms();
            while (*pcount == last_count) {
                if (bflb_mtimer_get_time_ms() - t_start >= 50) {
                    dpi_mjdec_stop();
                    timeout_count++;
                    reset_count++;
                    break;
                }
                vTaskDelay(1);
            }
            if (reset_count > 2) {
                dpi_mjdec_stop();
                GLB_MM_Software_Reset(GLB_MM_SW_MJDEC);
                reset_count = 0;
                printf("******  mjdec reset!  ******\r\n");
            }
        } while (*pcount == last_count);

        last_count = *pcount;
        reset_count = 0;
        dpi_mjdec_stop();
#else
        /* Centered sub-panel: decode into the compact scratch (MJDEC-done wakes us via
         * frame_done_sem without arming the OSD latch), DMA2D-composite into the panel buffer,
         * then aim the OSD latch at it and arm so the SEOF ISR scans the composited frame. */
        xSemaphoreTake(frame_done_sem, 0); /* drop any stale completion */
        dpi_mjdec_decode_one_frame(jpg_buffer->data, y_decode, uv_decode);
        if (xSemaphoreTake(frame_done_sem, pdMS_TO_TICKS(500)) != pdTRUE) {
            /* decode wedged: drop the stuck frame, recover, skip this frame */
            dpi_mjdec_stop();
            timeout_count++;
            if (++reset_count > 2) {
                GLB_MM_Software_Reset(GLB_MM_SW_MJDEC);
                reset_count = 0;
                printf("******  mjdec reset!  ******\r\n");
            }
            frame_buffer_release(jpg_buffer);
            continue;
        }
        dpi_mjdec_stop();
        reset_count = 0;

        if (compose_centered(yuv_images[yuv_idx]) < 0) {
            timeout_count++;                       
            frame_buffer_release(jpg_buffer);
            continue;                        /* skip */
        }
        /* Hand the composited panel buffer to the OSD SEOF ISR and arm the latch. */
        mjdec_config.output_bufaddr0 = (uint32_t)yuv_images[yuv_idx];
        mjdec_config.output_bufaddr1 = (uint32_t)(yuv_images[yuv_idx] + YUV_Y_PLANE_SIZE);
        dpi_mjdec_isr_enable_flag = 1;
        /* Wait for the SEOF latch so we don't overwrite a buffer still queued. */
        uint64_t t_swap = bflb_mtimer_get_time_ms();
        while (*pcount == last_count) {
            if (bflb_mtimer_get_time_ms() - t_swap >= 50) {
                break; /* SEOF lagging; proceed rather than stall the pipeline */
            }
            vTaskDelay(1);
        }
        last_count = *pcount;
#endif

        frame_buffer_release(jpg_buffer);
        yuv_idx = (yuv_idx + 1) % 2;
        success_count++;

        /* Frame-rate cap: sleep until the next VIDEO_TARGET_FPS slot, else the decoder runs
         * flat-out and starves the LVGL OSD render of CPU/PSRAM-bus bandwidth. */
        if (frame_period_ticks > 0) {
            vTaskDelayUntil(&last_frame_wake, frame_period_ticks);
        }

        if (bflb_mtimer_get_time_ms() - time_stats >= 2000) {
            uint64_t dt = bflb_mtimer_get_time_ms() - time_stats;
            printf("video: %lu fps (ok=%lu timeout=%lu)\r\n", (unsigned long)(success_count * 1000 / dt),
                   (unsigned long)success_count, (unsigned long)timeout_count);
            success_count = 0;
            timeout_count = 0;
            time_stats = bflb_mtimer_get_time_ms();
        }
    }
}
#endif

#else /* LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI : original triple-buffer path */

static struct bflb_device_s *mjdec;
static struct bflb_device_s *dpi_bg; /* DPI background scan-out layer, for the base-layer swap */
#if !VIDEO_FULLSCREEN
static struct bflb_device_s *dma2d; /* one channel; Y then UV run serially on it */
#endif

/* Triple-buffer scan-out tracking (consumed by the base-layer swap below): the swap latches
 * one frame late, so two Y buffers are in flight (scanning [0], next [1]); the decode task
 * uses the third. 0 = none yet. */
volatile uint32_t dpi_busy_y[2];

/* Triple-buffered panel-sized Y/UV planar buffers (NV12: UV half height); the third
 * always leaves one free for the decoder. */
#define VIDEO_BUF_COUNT 3
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t y_buffer[VIDEO_BUF_COUNT][LCD_PIXELS_MCU];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t uv_buffer[VIDEO_BUF_COUNT][LCD_PIXELS_MCU / 2];

#if !VIDEO_FULLSCREEN
/* Sub-panel video: MJDEC decodes the small frame here (compact), then it's stride-copied
 * into the centered window of y/uv_buffer. Unused for full-screen (decoded into the fb). */
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t y_decode[VIDEO_WIDTH_MCU * VIDEO_HEIGHT_MCU];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t uv_decode[VIDEO_WIDTH_MCU * VIDEO_HEIGHT_MCU / 2];
#endif

/* OSD overlay: full-screen ARGB8888 canvases are owned by main.c (double
 * buffered for LVGL); dpi_manager only points the blend hardware at them. */

/* set by the decode task before each decode, consumed by the MJDEC done ISR */
static volatile uint32_t pic_count;

/* Pending DPI background frame, latched by the base-layer swap between frames (no mid-scan
 * tear). Hold the display address of the panel-sized buffer; the decode task raises
 * dpi_show_pending when a frame is ready and the swap clears it. */
volatile uint32_t dpi_show_bufaddr0;
volatile uint32_t dpi_show_bufaddr1;
volatile uint8_t dpi_show_pending;

/* Strong override of the BSP's weak base-layer swap (mipi_dsi_v2.c). Invoked from the OSD
 * SEOF ISR: latch the pending video frame into the DPI background layer here so it switches
 * between frames, not mid-scanout, then rotate the in-flight set so the decode task can tell
 * which Y buffers the scanner still owns. */
void mipi_dsi_v2_osd0_base_layer_swap(void)
{
    if (dpi_show_pending && dpi_bg != NULL) {
        bflb_dpi_framebuffer_planar_switch(dpi_bg, dpi_show_bufaddr0, dpi_show_bufaddr1);
        /* Rotate the in-flight set: what was "next" ([1]) is now scanning ([0]),
         * and the just-latched buffer becomes "next". */
        dpi_busy_y[0] = dpi_busy_y[1];
        dpi_busy_y[1] = dpi_show_bufaddr0;
        dpi_show_pending = 0;
    }
}

#if defined(CONFIG_FREERTOS)
/* MJDEC done notification: ISR gives, decode task blocks on take (no busy-wait). */
static SemaphoreHandle_t frame_done_sem;
#if !VIDEO_FULLSCREEN
/* DMA2D transfer-complete notification, same ISR-give / task-take pattern. */
static SemaphoreHandle_t dma2d_done_sem;
#endif
#endif

/* ---------- MJPEG header parse + MJDEC one-frame decode (DSI-agnostic) ---------- */

static uint8_t parse_sof0(uint8_t *header, uint16_t *x, uint16_t *y, uint8_t *format, uint16_t *skip)
{
    uint32_t len;
    uint8_t *p = header;

    if (p[0] != 0xFF || p[1] != 0xD8) {
        return MJDEC_ERR_HEADER_SOI;
    }
    p += 2;
    while (1) {
        if (p[0] != 0xFF) {
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
            *skip = (uint16_t)(p + len + 1 - header);
            break;
        }
        p = p + len + 1;
    }
    return 0;
}

static void mjdec_decode_one_frame(struct bflb_device_s *dec, uint8_t *jpg, uint8_t *y_buf, uint8_t *uv_buf)
{
    uint16_t header_skip;
    uint16_t pic_x = 0, pic_y = 0;
    uint8_t yuv_format = MJDEC_FORMAT_YUV420SP_NV12;
    uint8_t ret = 0;
    struct bflb_mjdec_config_s mjdec_config;

    ret = parse_sof0(jpg, &pic_x, &pic_y, &yuv_format, &header_skip);
    if (ret) {
        printf("parse header error!\r\n");
        return;
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
    mjdec_config.output_bufaddr0 = (uint32_t)y_buf;
    mjdec_config.output_bufaddr1 = (uint32_t)uv_buf;

    bflb_mjdec_init(dec, &mjdec_config);

    bflb_mjdec_feature_control(dec, MJDEC_CMD_SET_READ_BURST, MJDEC_BURST_INCR16);
    bflb_mjdec_feature_control(dec, MJDEC_CMD_SET_WRITE_BURST, MJDEC_BURST_INCR16);

    bflb_mjdec_tcint_mask(dec, false);
    ret = bflb_mjdec_set_dqt_from_header(dec, jpg);
    if (ret) {
        printf("bflb_mjdec_set_dqt_from_header error, %d\r\n", ret);
    }
    ret = bflb_mjdec_set_dht_from_header(dec, jpg);
    if (ret) {
        printf("bflb_mjdec_set_dht_from_header error, %d\r\n", ret);
    }

    bflb_mjdec_start(dec);
    if (header_skip & 0x7) {
        bflb_mjdec_push_jpeg(dec, jpg);
    } else {
        bflb_mjdec_push_jpeg(dec, jpg + header_skip);
    }
}

static void mjdec_isr(int irq, void *arg)
{
    (void)irq;
    (void)arg;
    uint32_t intstatus = bflb_mjdec_get_intstatus(mjdec);
    if (intstatus & MJDEC_INTSTS_ONE_FRAME) {
        bflb_mjdec_int_clear(mjdec, MJDEC_INTCLR_ONE_FRAME);
        pic_count++;
        /* Decode-complete only. The task raises dpi_show_pending; the loop-top back-pressure
         * (dpi_busy_y shadow) keeps it off buffers the scanner still owns. */
#if defined(CONFIG_FREERTOS)
        BaseType_t hp_woken = pdFALSE;
        xSemaphoreGiveFromISR(frame_done_sem, &hp_woken);
        portYIELD_FROM_ISR(hp_woken);
#endif
    }
}

#if !VIDEO_FULLSCREEN
/* DMA2D transfer-complete: wake the decode task waiting on dma2d_done_sem. */
static void dma2d_done_cb(void *arg)
{
    (void)arg;
#if defined(CONFIG_FREERTOS)
    BaseType_t hp_woken = pdFALSE;
    xSemaphoreGiveFromISR(dma2d_done_sem, &hp_woken);
    portYIELD_FROM_ISR(hp_woken);
#endif
}

/* Blit one NV12 plane via DMA2D TRANSLATE: compact w*h source -> centered (dx,dy) window of a
 * panel-stride dest. Blocks on TC. NV12 is byte-addressed, so pixel_data_width=1 handles any offset. */
static int dma2d_blit_plane(const uint8_t *src, uint8_t *dst, uint32_t dst_stride,
                             uint32_t w, uint32_t h, uint32_t dx, uint32_t dy)
{
    /* NV12 is byte-addressed, so pixel_data_width=1 handles any (dx,dy) offset. MJDEC writes whole
     * MCUs, so each source row is MCU-aligned (e.g. 172 -> 176). */
    uint8_t pixel_width = 1;
    uint32_t src_stride = MJDEC_MCU_ALIGN(w);
    uint8_t burst, transfer_width;

    /* Burst + transfer width are derived here (no longer taken from the panel driver) from the
     * transferred row byte count (src_x_end - src_x_start) * pixel_width (= w * 1 for NV12):
     * 16-aligned byte span -> INCR16 else INCR4; byte span divisible by 4/2/1 -> 32/16/8-bit. */
    dma2d_pick_burst_width(0, w, pixel_width, &burst, &transfer_width);

    struct bflb_dma2d_channel_config_s cfg = {
        .next_lli_addr = 0,
        .control = {
            .bits = {
                .transfer_size = 0, /* 0 -> DMA2D 2D-loop mode (not plain DMA) */
                .src_burst = burst,
                .dst_burst = burst,
                .src_incr = 1,
                .dst_incr = 1,
                .int_enable = 1,
            },
        },
    };
    struct bflb_dma2d_image_s img = {
        .transfer_data_width = transfer_width,
        .pixel_data_width = pixel_width,
        .src_image_addr = (uint32_t)src,
        .src_image_width = src_stride, /* source row stride: MJDEC writes MCU-aligned */
        .src_x_start = 0,
        .src_y_start = 0,
        .src_x_end = w, /* exclusive end: crop to width, dropping the right MCU-padding cols */
        .src_y_end = h,
        .dst_image_addr = (uint32_t)dst,
        .dst_image_width = dst_stride,
        .dst_x_start = dx,
        .dst_y_start = dy,
    };

    bflb_dma2d_image_geometric_transfor_calculate(dma2d, &cfg, &img, DMA2D_IMAGE_TRANSLATE);
    bflb_dma2d_channel_init(dma2d, &cfg);
    bflb_dma2d_channel_start(dma2d);

#if defined(CONFIG_FREERTOS)
    if (xSemaphoreTake(dma2d_done_sem, pdMS_TO_TICKS(400)) != pdTRUE) {
        bflb_dma2d_channel_tcint_clear(dma2d);
        bflb_dma2d_channel_stop(dma2d);
        return -1;
    }
#else
    while (bflb_dma2d_channel_isbusy(dma2d)) {
    }
    bflb_dma2d_channel_tcint_clear(dma2d);
#endif
    bflb_dma2d_channel_stop(dma2d);
    return 0;
}

/* Composite the decoded frame into the centered window of the panel buffers via
 * DMA2D (all in PSRAM, no CPU access -> no dcache maintenance). */
static int compose_centered(uint8_t *y_dst, uint8_t *uv_dst)
{
    if (dma2d_blit_plane(y_decode, y_dst, LCD_WIDTH,
                         VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_OFFSET_X, VIDEO_OFFSET_Y) < 0)
        return -1;
    if (dma2d_blit_plane(uv_decode, uv_dst, LCD_WIDTH,
                         VIDEO_WIDTH, VIDEO_HEIGHT / 2, VIDEO_OFFSET_X, VIDEO_OFFSET_Y / 2) < 0)
        return -1;
    return 0;
}
#endif

int dpi_manager_init(void)
{
    /* The whole DSI display side is already up (lcd_init() -> the panel's _dsi_init()).
     * This owns only the video pipeline: MJDEC, (sub-panel) DMA2D, and the YUV buffers. */

    /* [1] MJDEC */
    LOG_I("[1] Init MJDEC\r\n");
    mjdec = bflb_device_get_by_name("mjdec");
    if (mjdec == NULL) {
        return -2;
    }
    /* DPI background layer handle used by the base-layer swap (mipi_dsi_v2_osd0_base_layer_swap). */
    dpi_bg = bflb_device_get_by_name("dpi");
    if (dpi_bg == NULL) {
        return -3;
    }
#if defined(CONFIG_FREERTOS)
    frame_done_sem = xSemaphoreCreateBinary();
    if (frame_done_sem == NULL) {
        return -5;
    }
#endif
    bflb_irq_attach(mjdec->irq_num, mjdec_isr, NULL);
    bflb_irq_enable(mjdec->irq_num);

    /* Reset before start */
    GLB_MM_Software_Reset(GLB_MM_SW_MJDEC);

#if !VIDEO_FULLSCREEN
    /* [1b] DMA2D for the centered composite (sub-panel video only). One channel
     * is enough: Y and UV blits run serially within compose_centered. */
    LOG_I("[1b] Init DMA2D (centered composite)\r\n");
    dma2d = bflb_device_get_by_name(BFLB_NAME_DMA2D_CH0);
    if (dma2d == NULL) {
        return -6;
    }
#if defined(CONFIG_FREERTOS)
    dma2d_done_sem = xSemaphoreCreateBinary();
    if (dma2d_done_sem == NULL) {
        return -7;
    }
#endif
    bflb_dma2d_channel_irq_attach(dma2d, dma2d_done_cb, NULL);
#endif

    /* [2] Background framebuffers start cleared (black); compose_centered only touches
     * the centered window, so the borders stay black. */
    LOG_I("[2] Clear video background framebuffers\r\n");
    memset(y_buffer, 0, sizeof(y_buffer));
    memset(uv_buffer, 0x80, sizeof(uv_buffer)); /* neutral chroma -> black border */
    bflb_l1c_dcache_clean_range(y_buffer, sizeof(y_buffer));
    bflb_l1c_dcache_clean_range(uv_buffer, sizeof(uv_buffer));

    return 0;
}

#if defined(CONFIG_FREERTOS)
void image_switch_task(void *param)
{
    jpg_buffer_t *jpg_buffer;
    const TickType_t frame_period_ticks = pdMS_TO_TICKS(VIDEO_FRAME_PERIOD_MS);
    TickType_t last_frame_wake;
    int parity = 0;
    uint32_t success = 0, timeout = 0;
    uint32_t reset_count = 0;
    uint64_t t_stats;

    (void)param;
    vTaskDelay(100);
    last_frame_wake = xTaskGetTickCount();
    t_stats = bflb_mtimer_get_time_ms();

    LOG_I("image switch task started, target=%u fps\r\n", (unsigned)VIDEO_TARGET_FPS);

    while (1) {
        if (frame_buffer_pop(&jpg_buffer, (TickType_t)100) != 0) {
            continue;
        }

        /* MJDEC reads the JPEG via DMA from PSRAM: make sure it is in memory */
        bflb_l1c_dcache_clean_range(jpg_buffer->data, jpg_buffer->size);

        /* Back-pressure: round-robin to the next candidate buffer, then wait until the HW
         * no longer owns it (not in dpi_busy_y[0/1], nor a pending-but-unlatched frame). With
         * 3 buffers one is always free, so this usually returns at once; bounded so a missing
         * SEOF can't wedge the task. */
        parity = (parity + 1) % VIDEO_BUF_COUNT;
        {
            uint32_t cand_y = (uint32_t)y_buffer[parity];
            uint64_t t_bp = bflb_mtimer_get_time_ms();
            while (cand_y == dpi_busy_y[0] || cand_y == dpi_busy_y[1] ||
                   (dpi_show_pending && cand_y == dpi_show_bufaddr0)) {
                if (bflb_mtimer_get_time_ms() - t_bp >= 50) {
                    break; /* SEOF lagging; proceed rather than stall the pipeline */
                }
                vTaskDelay(1);
            }
        }

        /* Decode the frame. A corrupt JPEG can wedge the MJDEC (done-IRQ never fires); recover by
         * stopping+popping the stuck frame, and after several timeouts do a full MJDEC reset. */
        bool decoded = false;
        do {
            /* clear any stale completion from a previous timed-out decode */
            xSemaphoreTake(frame_done_sem, 0);
#if VIDEO_FULLSCREEN
            /* full-screen: decode straight into the candidate framebuffer */
            mjdec_decode_one_frame(mjdec, jpg_buffer->data, y_buffer[parity], uv_buffer[parity]);
#else
            /* sub-panel: decode into the compact scratch, composite afterwards */
            mjdec_decode_one_frame(mjdec, jpg_buffer->data, y_decode, uv_decode);
#endif

            /* block (no busy-wait) until the done-IRQ fires or we time out */
            if (xSemaphoreTake(frame_done_sem, pdMS_TO_TICKS(500)) == pdTRUE) {
                decoded = true;
                break;
            }

            /* timeout: stop + drop the half-decoded frame stuck in the FIFO */
            bflb_mjdec_stop(mjdec);
            bflb_mjdec_pop_one_frame(mjdec);
            bflb_mjdec_int_clear(mjdec, MJDEC_INTCLR_ONE_FRAME);
            timeout++;
            reset_count++;

            /* still stuck after several retries: reset the whole MJDEC block */
            if (reset_count > 2) {
                bflb_mjdec_stop(mjdec);
                bflb_mjdec_pop_one_frame(mjdec);
                GLB_MM_Software_Reset(GLB_MM_SW_MJDEC);
                reset_count = 0;
                printf("******  mjdec reset!  ******\r\n");
                /* give up on this (bad) frame and move on to the next one */
                break;
            }
        } while (1);

        if (decoded) {
            reset_count = 0;
            success++;

#if !VIDEO_FULLSCREEN
            /* DMA2D does the centered composite entirely in PSRAM (CPU touches neither, so no
             * cache work); the init-time clean of the black borders still holds. */
            if (compose_centered(y_buffer[parity], uv_buffer[parity]) < 0) {
                timeout++;
                frame_buffer_release(jpg_buffer);
                continue;                        /* skip this frame */
            }
#endif
            /* Hand the buffer to the SEOF ISR (swap latches next frame boundary). Non-blocking:
             * the loop-top back-pressure keeps the decoder off buffers the scanner still owns. */
            dpi_show_bufaddr0 = (uint32_t)y_buffer[parity];
            dpi_show_bufaddr1 = (uint32_t)uv_buffer[parity];
            dpi_show_pending = 1;
        }

        frame_buffer_release(jpg_buffer);

        if (frame_period_ticks > 0) {
            vTaskDelayUntil(&last_frame_wake, frame_period_ticks);
        }

        if (bflb_mtimer_get_time_ms() - t_stats >= 2000) {
            uint64_t dt = bflb_mtimer_get_time_ms() - t_stats;
            printf("video: %lu fps (ok=%lu timeout=%lu)\r\n", (unsigned long)(success * 1000 / dt),
                   (unsigned long)success, (unsigned long)timeout);
            success = 0;
            timeout = 0;
            t_stats = bflb_mtimer_get_time_ms();
        }
    }
}
#endif

#endif /* LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI / DSI */
