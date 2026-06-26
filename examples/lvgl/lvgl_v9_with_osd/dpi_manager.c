/**
 * @file dpi_manager.c
 * @brief DPI(YUV-planar background) + OSD0(ARGB8888 overlay) + MJDEC.
 *
 * The DSI chain (PLL, clocks, controller, panel, HS mode) is brought up by
 * lcd_init() before dpi_manager_init() runs. This file owns only the video
 * pipeline: the MJDEC decoder, the ping-pong YUV background framebuffers, and
 * (for sub-panel video) the DMA2D centered composite.
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
#include "task.h"
#include "semphr.h"
#include "filesystem_reader.h"
#endif

#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI)
/* dpi_dev and osd_dev are defined in bl_mipi_dpi_v2.c (the BSP owns them). */
extern struct bflb_device_s *dpi_dev;
extern struct bflb_device_s *osd_dev;

static struct bflb_device_s *mjdec_dev = NULL;

/* Shared with bl_mipi_dpi_v2.c's osd_layer_isr (extern'd there):
 *  - mjdec_config holds the Y/UV addresses the ISR latches into the DPI scanout
 *  - pic_count is bumped by the ISR once per latched frame (the task waits on it)
 *  - dpi_mjdec_isr_enable_flag is raised by the MJDEC-done ISR, consumed by the
 *    OSD SEOF ISR so the background switches between frames (no mid-scan tear). */
volatile struct bflb_mjdec_config_s mjdec_config;
volatile uint32_t pic_count = 0;
uint8_t dpi_mjdec_isr_enable_flag = 0;

/* Double-buffered panel-sized YUV background (NV12/NV16: 2 B/px upper bound).
 * Decoded into ping-pong, scanned out by the DPI via the planar switch.
 * Layout per buffer: Y plane (LCD_PIXELS) immediately followed by the UV plane
 * (the planar switch is handed Y at base, UV at base+LCD_PIXELS). */
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t yuv_buffer_0[LCD_WIDTH * LCD_HEIGHT * 2];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t yuv_buffer_1[LCD_WIDTH * LCD_HEIGHT * 2];
static uint8_t *yuv_images[] = { yuv_buffer_0, yuv_buffer_1 };

#if (DPI_PIXEL_CLOCK_USE_SW_GPIO)
/**
 * @brief Configure pixel clock output using software GPIO
 *
 * Only used by LCD_DPI_STANDARD; other panels (e.g. JD9165BA) drive the panel
 * from the DPI peripheral's hardware PCLK pin instead (see DPI_PIXEL_CLOCK_USE_SW_GPIO).
 */
static void dpi_pixel_clock_output(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}
#endif

#if !VIDEO_FULLSCREEN
/* Sub-panel (centered) video. The MJDEC decodes the small frame into a compact
 * scratch (no panel-width stride), then DMA2D stride-copies it into the centered
 * window of the panel-sized buffer; the borders stay black. A separate DMA2D
 * channel + its own done semaphore mirror the DSI sub-panel path. */
static struct bflb_device_s *dma2d;
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t y_decode[VIDEO_WIDTH * VIDEO_HEIGHT];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t uv_decode[VIDEO_WIDTH * VIDEO_HEIGHT / 2];
#if defined(CONFIG_FREERTOS)
/* MJDEC-done: in centered mode the OSD ISR must NOT latch the compact scratch, so
 * the done-IRQ wakes the task via this semaphore instead of raising the flag; the
 * task raises the flag only after the composite, pointing it at the panel buffer. */
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

/* MJDEC done: just clear + raise the flag; the OSD SEOF ISR (in the BSP) does the
 * planar switch and bumps pic_count at the next frame boundary. In centered mode
 * the scratch is NOT the scan-out buffer, so we must not let the OSD ISR latch it:
 * wake the task instead and let it raise the flag after the composite. */
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

/* Blit one NV12 plane via DMA2D TRANSLATE: a compact w*h source copied into the
 * centered (dx,dy) window of a panel-stride (dst_stride) destination. Blocks on
 * the TC interrupt. NV12 is byte-addressed (Y and interleaved-UV both 1 B/px),
 * so pixel_data_width = 1 and 8-bit transfers handle any width/offset. */
static int dma2d_blit_plane(const uint8_t *src, uint8_t *dst, uint32_t dst_stride,
                             uint32_t w, uint32_t h, uint32_t dx, uint32_t dy)
{
    struct bflb_dma2d_channel_config_s cfg = {
        .next_lli_addr = 0,
        .control = {
            .bits = {
                .transfer_size = 0, /* 0 -> DMA2D 2D-loop mode (not plain DMA) */
                .src_burst = DMA2D_BURST_INCR16,
                .dst_burst = DMA2D_BURST_INCR16,
                .src_incr = 1,
                .dst_incr = 1,
                .int_enable = 1,
            },
        },
    };
    struct bflb_dma2d_image_s img = {
        .transfer_data_width = DMA2D_DATA_WIDTH_32BIT,
        .pixel_data_width = 1,
        .src_image_addr = (uint32_t)src,
        .src_image_width = w, /* compact source: stride == width */
        .src_x_start = 0,
        .src_y_start = 0,
        .src_x_end = w, /* end coords are exclusive (width/height) */
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

/* Composite the decoded frame into the centered window of a panel buffer via
 * DMA2D (all in PSRAM, no CPU access -> no dcache maintenance). The Y plane sits
 * at the buffer base; the UV plane follows at +LCD_PIXELS (NV12: half height). */
static int compose_centered(uint8_t *yuv_dst)
{
    uint8_t *y_dst = yuv_dst;
    uint8_t *uv_dst = yuv_dst + LCD_PIXELS;
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
    /* The DPI controller + OSD1 blend layer were brought up by lcd_init() ->
     * the selected panel's _dpi_init() -> bl_mipi_dpi_v2_init() (OSD layer switch
     * mode). This owns only the video pipeline: MJDEC + the YUV background buffers. */
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
    osd_dev = bflb_device_get_by_name(BFLB_NAME_OSD1);
    if (osd_dev == NULL) {
        return -4;
    }

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
        memset(yuv_images[i], 0, LCD_PIXELS);              /* Y plane -> black */
        memset(yuv_images[i] + LCD_PIXELS, 0x80, LCD_PIXELS); /* UV plane -> neutral */
        bflb_l1c_dcache_clean_range(yuv_images[i], LCD_WIDTH * LCD_HEIGHT * 2);
    }
#endif

#if (DPI_PIXEL_CLOCK_USE_SW_GPIO)
    dpi_pixel_clock_output();
#endif

    return 0;
}

#if defined(CONFIG_FREERTOS)
void image_switch_task(void *param)
{
    volatile uint32_t *pcount;
    uint32_t last_count = 0;
    jpg_buffer_t *jpg_buffer;
    QueueHandle_t full_queue = filesystem_get_full_queue();
    QueueHandle_t empty_queue = filesystem_get_empty_queue();
    uint8_t yuv_idx = 0;
    uint32_t reset_count = 0;
    uint32_t timeout_count = 0, success_count = 0;
    uint64_t time_stats;

    (void)param;
    vTaskDelay(100);
    pcount = &pic_count;
    time_stats = bflb_mtimer_get_time_ms();

    LOG_I("image switch task started (DPI)\r\n");

    while (1) {
        if (xQueueReceive(full_queue, &jpg_buffer, (TickType_t)100) != pdTRUE) {
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
                                           yuv_images[yuv_idx] + LCD_PIXELS);
            }
            /* Wait for the OSD SEOF ISR to latch the decoded frame (pic_count++). */
            uint64_t t_start = bflb_mtimer_get_time_ms();
            while (*pcount == last_count) {
                if (bflb_mtimer_get_time_ms() - t_start >= 50) {
                    dpi_mjdec_stop();
                    timeout_count++;
                    reset_count++;
                    break;
                }
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
        /* Centered sub-panel: decode into the compact scratch (the MJDEC-done ISR
         * wakes us via frame_done_sem WITHOUT arming the OSD latch), DMA2D-composite
         * into the centered window of the panel buffer, then point the OSD latch at
         * the panel buffer and arm it so the SEOF ISR scans the composited frame. */
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
            xQueueSend(empty_queue, &jpg_buffer, (TickType_t)0);
            continue;
        }
        dpi_mjdec_stop();
        reset_count = 0;

        if (compose_centered(yuv_images[yuv_idx]) < 0) {
            timeout_count++;                       
            xQueueSend(empty_queue, &jpg_buffer, (TickType_t)0);
            continue;                        /* skip */
        }
        /* Hand the composited panel buffer to the OSD SEOF ISR and arm the latch. */
        mjdec_config.output_bufaddr0 = (uint32_t)yuv_images[yuv_idx];
        mjdec_config.output_bufaddr1 = (uint32_t)(yuv_images[yuv_idx] + LCD_PIXELS);
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

        xQueueSend(empty_queue, &jpg_buffer, (TickType_t)0);
        yuv_idx = (yuv_idx + 1) % 2;
        success_count++;

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
#if !VIDEO_FULLSCREEN
static struct bflb_device_s *dma2d; /* one channel; Y then UV run serially on it */
#endif

/* Triple-buffer scan-out tracking, maintained by the SEOF ISR (mipi_dsi_v2.c) so
 * the decode task knows which Y buffers the hardware still owns. The swap latches
 * one frame late, so two are in flight: scanning now ([0]) and latched to scan next
 * ([1]). The decode task must write neither -- it picks the one remaining free
 * buffer. 0 = none yet. */
volatile uint32_t dpi_busy_y[2];

/* Background video: TRIPLE-buffered Y/UV planar buffers (NV12: UV is half height),
 * panel-sized and scanned out by the DPI.
 * The third buffer always leaves exactly one free for the decoder. */
#define VIDEO_BUF_COUNT 3
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t y_buffer[VIDEO_BUF_COUNT][LCD_PIXELS];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t uv_buffer[VIDEO_BUF_COUNT][LCD_PIXELS / 2];

#if !VIDEO_FULLSCREEN
/* Sub-panel video: MJDEC decodes the small frame here (compact, no panel-width
 * stride), then the pixels are stride-copied into the centered window of
 * y/uv_buffer. Not needed for full-screen video (decoded straight into the fb). */
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t y_decode[VIDEO_WIDTH * VIDEO_HEIGHT];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t uv_decode[VIDEO_WIDTH * VIDEO_HEIGHT / 2];
#endif

/* OSD overlay: full-screen ARGB8888 canvases are owned by main.c (double
 * buffered for LVGL); dpi_manager only points the blend hardware at them. */

/* set by the decode task before each decode, consumed by the MJDEC done ISR */
static volatile uint32_t pic_count;

/* Pending DPI background frame, latched by the SEOF ISR at the next frame boundary
 * (between frames, so the background never tears mid-scanout). These hold the
 * *display* address (the panel-sized planar buffer the DPI scans out), set after
 * the frame is composited; mipi_dsi_v2.c reads them via extern. */
volatile uint32_t dpi_show_bufaddr0;
volatile uint32_t dpi_show_bufaddr1;
volatile uint8_t dpi_show_pending;

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
        /* Decode-complete only. The task raises dpi_show_pending; the back-pressure
         * check at the top of the loop (the dpi_busy_y shadow) keeps it off buffers
         * the scanner still owns, so a buffer is never overwritten mid-scanout. */
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

/* Blit one NV12 plane via DMA2D TRANSLATE: a compact w*h source copied into the
 * centered (dx,dy) window of a panel-stride (dst_stride) destination. Blocks on
 * the TC interrupt. NV12 is byte-addressed (Y and interleaved-UV both 1 B/px),
 * so pixel_data_width = 1 and 8-bit transfers handle any width/offset. */
static int dma2d_blit_plane(const uint8_t *src, uint8_t *dst, uint32_t dst_stride,
                             uint32_t w, uint32_t h, uint32_t dx, uint32_t dy)
{
    struct bflb_dma2d_channel_config_s cfg = {
        .next_lli_addr = 0,
        .control = {
            .bits = {
                .transfer_size = 0, /* 0 -> DMA2D 2D-loop mode (not plain DMA) */
                .src_burst = DMA2D_BURST_INCR16,
                .dst_burst = DMA2D_BURST_INCR16,
                .src_incr = 1,
                .dst_incr = 1,
                .int_enable = 1,
            },
        },
    };
    struct bflb_dma2d_image_s img = {
        .transfer_data_width = DMA2D_DATA_WIDTH_32BIT,
        .pixel_data_width = 1,
        .src_image_addr = (uint32_t)src,
        .src_image_width = w, /* compact source: stride == width */
        .src_x_start = 0,
        .src_y_start = 0,
        .src_x_end = w, /* end coords are exclusive (width/height) */
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
    /* The whole DSI display side (panel link + DPI layer + OSD overlay + OSD
     * interrupt) was already brought up by lcd_init() -> the panel's _dsi_init().
     * This function owns only the video pipeline: MJDEC, (sub-panel) DMA2D, and
     * the YUV background framebuffers it switches into the DPI layer. */

    /* [1] MJDEC */
    LOG_I("[1] Init MJDEC\r\n");
    mjdec = bflb_device_get_by_name("mjdec");
    if (mjdec == NULL) {
        return -2;
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

    /* [2] Background framebuffers start cleared (black). compose_centered only
     * touches the centered window, so the borders stay black for the program's
     * life. */
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
    QueueHandle_t full_queue = filesystem_get_full_queue();
    QueueHandle_t empty_queue = filesystem_get_empty_queue();
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
        if (xQueueReceive(full_queue, &jpg_buffer, (TickType_t)100) != pdTRUE) {
            continue;
        }

        /* MJDEC reads the JPEG via DMA from PSRAM: make sure it is in memory */
        bflb_l1c_dcache_clean_range(jpg_buffer->data, jpg_buffer->size);

        /* Back-pressure: pick the next candidate buffer (round-robin), then wait
         * until the hardware no longer owns it -- it must match none of the two
         * in-flight buffers (dpi_busy_y[0/1]) nor a frame submitted but not yet
         * latched (dpi_show_bufaddr0 while dpi_show_pending). With 3 buffers one is
         * always free, so this normally returns at once; the wait only covers the
         * window before the next SEOF rotates the set. Bounded so a missing SEOF
         * can't wedge the task. */
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

        /* Decode the frame. A corrupt JPEG can wedge the MJDEC state machine so the
         * done-IRQ never fires; recover by stopping+popping the stuck frame, and
         * after a run of consecutive timeouts do a full MJDEC hardware reset. */
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
            if (xSemaphoreTake(frame_done_sem, pdMS_TO_TICKS(50)) == pdTRUE) {
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
            /* DMA2D does the centered composite entirely in PSRAM (reads y/uv_decode,
             * writes the panel buffers as a bus master). The CPU touches neither, so
             * no cache maintenance here; the init-time clean of the black borders
             * still holds since DMA2D only overwrites the centered window. */
            // compose_centered(y_buffer[parity], uv_buffer[parity]);
            if (compose_centered(y_buffer[parity], uv_buffer[parity]) < 0) {
                timeout++;                      
                xQueueSend(empty_queue, &jpg_buffer, (TickType_t)0);
                continue;                        /* skip this frame */
            }
#endif
            /* Hand the freshly decoded/composited buffer to the SEOF ISR; the swap
             * latches at the next frame boundary. Non-blocking -- back-pressure at
             * the top of the loop (the dpi_busy_y check) keeps the decoder off the
             * buffers the scanner still owns, so we don't wait on the swap here. */
            dpi_show_bufaddr0 = (uint32_t)y_buffer[parity];
            dpi_show_bufaddr1 = (uint32_t)uv_buffer[parity];
            dpi_show_pending = 1;
        }

        xQueueSend(empty_queue, &jpg_buffer, (TickType_t)0);

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
