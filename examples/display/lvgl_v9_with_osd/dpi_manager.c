/**
 * @file dpi_manager.c
 * @brief Video pipeline: MJDEC decode + ping-pong YUV background + (sub-panel) DMA2D composite.
 *        Main owns the display; image_switch_task waits for app_display readiness.
 *
 * Original DPI/DSI task, burst, timeout/recovery and composite paths are retained.
 * Only display ownership/latching moves to app_display: no legacy LCD hooks or
 * display register access, and a timeout NEVER releases a scan-out buffer.
 */
#include "dpi_manager.h"
#include "bflb_mjdec.h"
#include "bflb_dma2d.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bflb_irq.h"
#include "bflb_core.h"
#include "bl618dg_glb.h"
#include "board.h"
#include "log.h"
#include <string.h>

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "queue.h"
#include "task.h"
#include "semphr.h"
#endif

#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
#include "event_groups.h"
static EventGroupHandle_t video_pause_events;
static uint32_t video_ota_participants = VIDEO_OTA_PRODUCER | VIDEO_OTA_DECODER;
volatile bool g_video_pause_req;
volatile bool g_video_paused;
volatile bool g_video_resume_req;
#define video_pause_requested g_video_pause_req

void video_ota_configure_participants(uint32_t participants)
{
    video_ota_participants = participants;
}

void video_ota_checkpoint(uint32_t participant)
{
    while (video_pause_requested) {
        if (g_video_resume_req) {
            video_ota_resume();
            break;
        }
        /* Created before producer/decoder tasks, or lazily by video_ota_pause. */
        if (video_pause_events != NULL) {
            EventBits_t ready = xEventGroupSetBits(video_pause_events, participant);
            if ((ready & video_ota_participants) == video_ota_participants) {
                g_video_paused = true;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/* Pause cooperatively before any Flash write. SD and USB use the same producer bit. */
int video_ota_pause(void)
{
    const EventBits_t both = VIDEO_OTA_PRODUCER | VIDEO_OTA_DECODER;
    const EventBits_t required = video_ota_participants;
    if (video_pause_events == NULL) {
        video_pause_events = xEventGroupCreate();
        if (video_pause_events == NULL) {
            return -1;
        }
    }
    xEventGroupClearBits(video_pause_events, both);
    g_video_paused = false;
    g_video_resume_req = false;
    video_pause_requested = true;
    /* FreeRTOS requires a nonzero wait mask; no playback needs no task ACKs. */
    EventBits_t ready = required ?
        xEventGroupWaitBits(video_pause_events, required, pdFALSE, pdTRUE, pdMS_TO_TICKS(5000)) : 0;
    if ((ready & required) != required || !video_pause_requested) {
        video_ota_resume();
        return -1;
    }
    g_video_paused = true;
    return 0;
}

void video_ota_resume(void)
{
    video_pause_requested = false;
    g_video_paused = false;
    g_video_resume_req = false;
    if (video_pause_events != NULL) {
        xEventGroupClearBits(video_pause_events, VIDEO_OTA_PRODUCER | VIDEO_OTA_DECODER);
    }
}

static void video_display_wait_checkpoint(void)
{
    /* Present is called only after MJDEC and any DMA2D composite are stopped.
     * Pending scan-out ownership remains entirely within app_display. */
    video_ota_checkpoint(VIDEO_OTA_DECODER);
}
#endif

#if defined(CONFIG_FREERTOS)
static QueueHandle_t pool_queue;
static QueueHandle_t out_queue;
#endif

#if !VIDEO_INTERFACE_DPI
static struct bflb_device_s *mjdec;
#if defined(CONFIG_FREERTOS)
static SemaphoreHandle_t frame_done_sem;
#endif

/* Triple-buffered panel-sized Y/UV planar buffers (NV12: UV half height); the third
 * leaves one free for the decoder. app_display, not a local shadow, owns scan-out. */
#define VIDEO_BUF_COUNT 3
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64))) static uint8_t y_buffer[VIDEO_BUF_COUNT][LCD_PIXELS_MCU];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64))) static uint8_t uv_buffer[VIDEO_BUF_COUNT][LCD_PIXELS_MCU / 2];

/* Decode count only; panel-frame timing is available through app_display. */
static volatile uint32_t pic_count;

#if !VIDEO_FULLSCREEN
static struct bflb_device_s *dma2d; /* one channel; Y then UV run serially on it */
#if defined(CONFIG_FREERTOS)
static SemaphoreHandle_t dma2d_done_sem;
#endif
/* Sub-panel video: MJDEC decodes the small frame here (compact), then it's stride-copied
 * into the centered window of y/uv_buffer. Unused for full-screen (decoded into the fb). */
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64))) static uint8_t y_decode[VIDEO_WIDTH_MCU * VIDEO_HEIGHT_MCU];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64))) static uint8_t uv_decode[VIDEO_WIDTH_MCU * VIDEO_HEIGHT_MCU / 2];
#endif
#endif /* !VIDEO_INTERFACE_DPI */

#if defined(CONFIG_FREERTOS)
#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
static int video_pause_init(void)
{
    if (video_pause_events == NULL) {
        video_pause_events = xEventGroupCreate();
    }
    if (video_pause_events == NULL) {
        return -1;
    }
    app_display_set_wait_callback(video_display_wait_checkpoint);
    return 0;
}
#endif

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
/* Derive burst + transfer width from the transferred row byte count. */
static void dma2d_pick_burst_width(uint32_t src_x_start, uint32_t src_x_end, uint8_t pixel_width,
                                 uint8_t *burst, uint8_t *transfer_width)
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

/* Original SOF0 component/sample rules (gray, NV16 and NV12), shared by DPI and
 * DSI. Bound every marker before calling the LHAL table readers. Identification
 * remains separate from the display's NV12-only acceptance check below. */
static uint8_t parse_sof0(const uint8_t *header, size_t size, uint16_t *x, uint16_t *y,
                          uint8_t *format, uint16_t *skip)
{
    size_t pos = 2;
    bool found = false;
    if (size < 4 || header[0] != 0xff || header[1] != 0xd8 ||
        header[size - 2] != 0xff || header[size - 1] != 0xd9) {
        return MJDEC_ERR_HEADER_SOI;
    }
    while (pos + 4 <= size) {
        /* p points at the marker ID, as in the original parser. */
        const uint8_t *p = header + pos + 1;
        size_t len = ((size_t)p[1] << 8) | p[2];
        if (header[pos] != 0xff || len < 2 || len > size - pos - 2) {
            printf("error: header marker should be started by 0xFF.\r\n");
            return MJDEC_ERR_HEADER_MARKER;
        }
        if (p[0] == 0xc0) {
            if (found || len < 8 || p[3] != 8) {
                printf("accuracy should be 8.\r\n");
                return 0xF1;
            }
            *y = (p[4] << 8) | p[5];
            *x = (p[6] << 8) | p[7];
            if (len != (size_t)(8 + 3 * p[8])) {
                return MJDEC_ERR_HEADER_MARKER;
            }
            if (p[8] == 1) {
                if (p[9] == 1 && p[10] == 0x11 && p[11] == 0) {
                    *format = MJDEC_FORMAT_GRAY;
                } else {
                    printf("gray image error, id=0x%02X, sample=0x%02X, dqt=0x%02X.\r\n", p[9], p[10], p[11]);
                    return 0xF1;
                }
            } else if (p[8] == 3) {
                if (p[9] == 1 && p[11] == 0 && p[12] == 2 && p[13] == 0x11 && p[14] == 1 &&
                    p[15] == 3 && p[16] == 0x11 && p[17] == 1) {
                    if (p[10] == 0x21) {
                        *format = MJDEC_FORMAT_YUV422SP_NV16;
                    } else if (p[10] == 0x22) {
                        *format = MJDEC_FORMAT_YUV420SP_NV12;
                    } else {
                        printf("component: %02X %02X %02X, %02X %02X %02X, %02X %02X %02X\r\n",
                               p[9], p[10], p[11], p[12], p[13], p[14], p[15], p[16], p[17]);
                        printf("only support NV16 or NV12 format.\r\n");
                        return 0xF1;
                    }
                } else {
                    return 0xF1; /* Never leave format uninitialized on a bad component layout. */
                }
            } else {
                printf("only support gray or YCbCr format.\r\n");
                return 0xF2;
            }
            found = true;
        } else if (p[0] == 0xda) {
            size_t end = pos + len + 2;
            if (!found || end > UINT16_MAX || end >= size - 2 || len < 6 ||
                len != (size_t)(6 + 2 * p[3])) {
                return MJDEC_ERR_HEADER_MARKER;
            }
            *skip = (uint16_t)end;
            return 0;
        } else if ((p[0] >= 0xc1 && p[0] <= 0xcf && p[0] != 0xc4) || p[0] == 0xd9) {
            return 0xF2; /* Progressive/other SOF modes cannot be decoded by MJDEC. */
        }
        pos += len + 2;
    }
    return MJDEC_ERR_HEADER_MARKER;
}

/* Keep all parser formats, but reject unsupported scan-out layouts BEFORE MJDEC
 * writes: NV16 needs twice our UV allocation, and gray has no chroma plane. */
static int video_validate_format(uint16_t x, uint16_t y, uint8_t format)
{
    if (format != MJDEC_FORMAT_YUV420SP_NV12 || x != VIDEO_WIDTH || y != VIDEO_HEIGHT) {
        LOG_E("JPEG rejected: format=%u %ux%u; NV12 %ux%u required by app_display\r\n",
              format, x, y, (unsigned)VIDEO_WIDTH, (unsigned)VIDEO_HEIGHT);
        return -1;
    }
    return 0;
}

#if !VIDEO_INTERFACE_DPI
/* ---------- MJPEG header parse + MJDEC one-frame decode (DSI-agnostic) ---------- */

static int mjdec_decode_one_frame(struct bflb_device_s *dec, jpg_buffer_t *jpg, uint8_t *y_buf, uint8_t *uv_buf)
{
    uint16_t header_skip = 0;
    uint16_t pic_x = 0, pic_y = 0;
    uint8_t yuv_format = MJDEC_FORMAT_YUV420SP_NV12;
    uint8_t ret;
    struct bflb_mjdec_config_s mjdec_config = {0};

    ret = parse_sof0(jpg->data, jpg->size, &pic_x, &pic_y, &yuv_format, &header_skip);
    if (ret) {
        printf("parse header error!\r\n");
        return -1;
    }
    if (video_validate_format(pic_x, pic_y, yuv_format) != 0) {
        return -1;
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
    ret = bflb_mjdec_set_dqt_from_header(dec, jpg->data);
    if (ret) {
        printf("bflb_mjdec_set_dqt_from_header error, %d\r\n", ret);
        return -1;
    }
    ret = bflb_mjdec_set_dht_from_header(dec, jpg->data);
    if (ret) {
        printf("bflb_mjdec_set_dht_from_header error, %d\r\n", ret);
        return -1;
    }

    bflb_mjdec_start(dec);
    if (header_skip & 0x7) {
        bflb_mjdec_push_jpeg(dec, jpg->data);
    } else {
        bflb_mjdec_push_jpeg(dec, jpg->data + header_skip);
    }
    return 0;
}

static void mjdec_isr(int irq, void *arg)
{
    (void)irq;
    (void)arg;
    if (bflb_mjdec_get_intstatus(mjdec) & MJDEC_INTSTS_ONE_FRAME) {
        bflb_mjdec_int_clear(mjdec, MJDEC_INTCLR_ONE_FRAME);
        pic_count++;
        /* Decode-complete only. The task presents the composited panel frame. */
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
    /* MJDEC writes whole MCUs, so each source row is MCU-aligned (e.g. 172 -> 176).
     * Burst + transfer width follow the transferred row byte count, not panel timing. */
    uint8_t pixel_width = 1;
    uint32_t src_stride = MJDEC_MCU_ALIGN(w);
    uint8_t burst, transfer_width;

    dma2d_pick_burst_width(0, w, pixel_width, &burst, &transfer_width);

    /* Row span alone is insufficient for panels such as 258x960: a 32-bit
     * transfer also needs every row's source/destination address aligned. */
    uint32_t alignment = (uint32_t)src | ((uint32_t)dst + dy * dst_stride + dx) |
                         src_stride | dst_stride;
    if (transfer_width == DMA2D_DATA_WIDTH_32BIT && (alignment & 3U)) {
        transfer_width = DMA2D_DATA_WIDTH_16BIT;
    }
    if (transfer_width == DMA2D_DATA_WIDTH_16BIT && (alignment & 1U)) {
        transfer_width = DMA2D_DATA_WIDTH_8BIT;
    }

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
        .src_image_width = src_stride,
        .src_x_start = 0,
        .src_y_start = 0,
        .src_x_end = w, /* exclusive end: crop right MCU-padding columns */
        .src_y_end = h,
        .dst_image_addr = (uint32_t)dst,
        .dst_image_width = dst_stride,
        .dst_x_start = dx,
        .dst_y_start = dy,
    };

#if defined(CONFIG_FREERTOS)
    xSemaphoreTake(dma2d_done_sem, 0); /* drop stale completion after a stopped transfer */
#endif
    bflb_dma2d_image_geometric_transfor_calculate(dma2d, &cfg, &img, DMA2D_IMAGE_TRANSLATE);
    bflb_dma2d_channel_init(dma2d, &cfg);
    bflb_dma2d_channel_start(dma2d);
#if defined(CONFIG_FREERTOS)
    if (xSemaphoreTake(dma2d_done_sem, pdMS_TO_TICKS(400)) != pdTRUE) {
        bflb_dma2d_channel_stop(dma2d);
        bflb_dma2d_channel_tcint_clear(dma2d);
        return -1;
    }
#else
    while (bflb_dma2d_channel_isbusy(dma2d)) {
    }
#endif
    bflb_dma2d_channel_stop(dma2d);
    bflb_dma2d_channel_tcint_clear(dma2d);
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
    /* The whole DSI display side belongs to main/app_display. This owns only the
     * video pipeline: MJDEC, (sub-panel) DMA2D, and the YUV buffers. */

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
#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
    if (video_pause_init() != 0) {
        return -8;
    }
#endif
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
    bflb_l1c_dcache_clean_invalidate_range(y_decode, sizeof(y_decode));
    bflb_l1c_dcache_clean_invalidate_range(uv_decode, sizeof(uv_decode));
#endif
    /* [2] Background framebuffers start cleared (black); compose_centered only touches
     * the centered window, so the borders stay black. */
    LOG_I("[2] Clear video background framebuffers\r\n");
    memset(y_buffer, 0, sizeof(y_buffer));
    memset(uv_buffer, 0x80, sizeof(uv_buffer));
    /* CPU never touches these planes again. The display does not clean DMA
     * output; invalidation here removes any stale CPU copies before DMA owns it. */
    bflb_l1c_dcache_clean_invalidate_range(y_buffer, sizeof(y_buffer));
    bflb_l1c_dcache_clean_invalidate_range(uv_buffer, sizeof(uv_buffer));
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
    app_display_wait_ready();
    last_frame_wake = xTaskGetTickCount();
    t_stats = bflb_mtimer_get_time_ms();

    LOG_I("image switch task started, target=%u fps\r\n", (unsigned)VIDEO_TARGET_FPS);

    while (1) {
#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
        if (video_pause_requested) {
            video_ota_checkpoint(VIDEO_OTA_DECODER);
            /* Resume from now instead of catching up paused frame slots. */
            last_frame_wake = xTaskGetTickCount();
            t_stats = bflb_mtimer_get_time_ms();
            success = 0;
            timeout = 0;
        }
#endif
        if (frame_buffer_pop(&jpg_buffer, (TickType_t)100) != 0) {
            continue;
        }

        /* MJDEC reads the JPEG via DMA from PSRAM: make sure it is in memory. */
        bflb_l1c_dcache_clean_range(jpg_buffer->data, jpg_buffer->size);

        /* Back-pressure: wait until HW no longer owns the candidate (neither scanning
         * nor queued). The old 50ms escape is NOT ownership: app_display must keep
         * waiting if SEOF is missing. parity advances ONLY after a successful present,
         * so a bad JPEG/failed composite never selects our own last scanning slot. */
        app_display_wait_writable(y_buffer[parity]);

        /* Decode the frame. A corrupt JPEG can wedge MJDEC (done-IRQ never fires);
         * stop+pop the stuck frame, and after several timeouts reset the MJDEC block. */
        bool decoded = false;
        do {
            /* clear any stale completion from a previous timed-out decode */
            xSemaphoreTake(frame_done_sem, 0);
#if VIDEO_FULLSCREEN
            /* full-screen: decode straight into the candidate framebuffer */
            int ret = mjdec_decode_one_frame(mjdec, jpg_buffer, y_buffer[parity], uv_buffer[parity]);
#else
            /* sub-panel: decode into the compact scratch, composite afterwards */
            int ret = mjdec_decode_one_frame(mjdec, jpg_buffer, y_decode, uv_decode);
#endif
            if (ret) {
                break; /* Invalid format/dimensions/tables: hardware was never started. */
            }

            /* block (no busy-wait) until the done-IRQ fires or we time out */
            decoded = xSemaphoreTake(frame_done_sem, pdMS_TO_TICKS(500)) == pdTRUE;
            /* Stop on success too, before releasing the input or reusing the scratch. */
            bflb_mjdec_stop(mjdec);
            bflb_mjdec_pop_one_frame(mjdec);
            bflb_mjdec_int_clear(mjdec, MJDEC_INTCLR_ONE_FRAME);
            if (decoded) {
                break;
            }

            timeout++;
            reset_count++;

            /* still stuck after several retries: reset the whole MJDEC block */
            if (reset_count > 2) {
                GLB_MM_Software_Reset(GLB_MM_SW_MJDEC);
                reset_count = 0;
                printf("******  mjdec reset!  ******\r\n");
                /* give up on this (bad) frame and move on to the next one */
                break;
            }
        } while (1);

        if (decoded) {
            reset_count = 0;
#if !VIDEO_FULLSCREEN
            /* DMA2D does the centered composite entirely in PSRAM (CPU touches neither,
             * so no cache work); the init-time clean of the black borders still holds. */
            if (compose_centered(y_buffer[parity], uv_buffer[parity]) < 0) {
                timeout++;
                frame_buffer_release(jpg_buffer);
                continue; /* skip this frame, keep the same writable candidate */
            }
#endif
            /* app_display synchronously waits for FIRST/RETIRED. It must never return
             * an accepted frame on a timeout. Rejection leaves this candidate writable. */
            int ret = app_display_present(y_buffer[parity], uv_buffer[parity]);
            if (ret) {
                LOG_E("Base submit failed: %d; keeping writable candidate\r\n", ret);
                timeout++;
            } else {
                success++;
                parity = (parity + 1) % VIDEO_BUF_COUNT;
            }
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

#else /* VIDEO_INTERFACE_DPI : original double-buffer path */

static struct bflb_device_s *mjdec_dev = NULL;
/* Decode configuration no longer doubles as an ISR-to-display mailbox. */
static struct bflb_mjdec_config_s mjdec_config;

/* Double-buffered panel-sized YUV background. Per buffer: MCU-padded Y plane
 * then UV plane; app_display gets Y at base, UV at base+YUV_Y_PLANE_SIZE. */
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64))) static uint8_t yuv_buffer_0[YUV_BUFFER_SIZE];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64))) static uint8_t yuv_buffer_1[YUV_BUFFER_SIZE];
static uint8_t *yuv_images[] = { yuv_buffer_0, yuv_buffer_1 };

#if defined(CONFIG_FREERTOS)
/* All modes wake the task on decode-complete; only app_display can latch a frame. */
static SemaphoreHandle_t frame_done_sem;
#endif

#if !VIDEO_FULLSCREEN
/* Sub-panel (centered) video: MJDEC decodes the small frame into a compact scratch, then
 * DMA2D stride-copies it into the centered window of the panel buffer (borders stay black). */
static struct bflb_device_s *dma2d;
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64))) static uint8_t y_decode[VIDEO_WIDTH_MCU * VIDEO_HEIGHT_MCU];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64))) static uint8_t uv_decode[VIDEO_WIDTH_MCU * VIDEO_HEIGHT_MCU / 2];
#if defined(CONFIG_FREERTOS)
static SemaphoreHandle_t dma2d_done_sem;
#endif
#endif

/* Retain the DPI parser entry point; both interfaces use the original SOF0 rules
 * with the same size-bounded walk, including gray and NV16 identification. */
static uint8_t dpi_parse_sof0(uint8_t *header, size_t size, uint16_t *x, uint16_t *y,
                             uint8_t *format, uint16_t *skip)
{
    return parse_sof0(header, size, x, y, format, skip);
}

static int dpi_mjdec_decode_one_frame(jpg_buffer_t *jpg, uint8_t *y_buf, uint8_t *uv_buf)
{
    uint16_t header_skip = 0;
    uint16_t pic_x = 0, pic_y = 0;
    uint8_t yuv_format = MJDEC_FORMAT_YUV420SP_NV12;
    uint8_t ret = 0;

    ret = dpi_parse_sof0(jpg->data, jpg->size, &pic_x, &pic_y, &yuv_format, &header_skip);
    if (ret) {
        printf("parse header error!\r\n");
        return -1;
    }
    if (video_validate_format(pic_x, pic_y, yuv_format) != 0) {
        return -1;
    }
    mjdec_config.format = yuv_format;
    mjdec_config.swap_enable = true;
    mjdec_config.resolution_x = pic_x;
    mjdec_config.resolution_y = pic_y;
    mjdec_config.head_size = (header_skip & 0x7) ? header_skip : 0;
    mjdec_config.output_bufaddr0 = (uint32_t)y_buf;
    mjdec_config.output_bufaddr1 = (uint32_t)uv_buf;

    bflb_mjdec_init(mjdec_dev, &mjdec_config);

    bflb_mjdec_feature_control(mjdec_dev, MJDEC_CMD_SET_READ_BURST, MJDEC_BURST_INCR4);
    bflb_mjdec_feature_control(mjdec_dev, MJDEC_CMD_SET_WRITE_BURST, MJDEC_BURST_INCR4);

    bflb_mjdec_tcint_mask(mjdec_dev, false);
    ret = bflb_mjdec_set_dqt_from_header(mjdec_dev, jpg->data);
    if (ret) {
        printf("bflb_mjdec_set_dqt_from_header error, %d\r\n", ret);
        return -1;
    }
    ret = bflb_mjdec_set_dht_from_header(mjdec_dev, jpg->data);
    if (ret) {
        printf("bflb_mjdec_set_dht_from_header error, %d\r\n", ret);
        return -1;
    }

    bflb_mjdec_start(mjdec_dev);
    if (header_skip & 0x7) {
        bflb_mjdec_push_jpeg(mjdec_dev, jpg->data);
    } else {
        bflb_mjdec_push_jpeg(mjdec_dev, jpg->data + header_skip);
    }
    return 0;
}

static void dpi_mjdec_stop(void)
{
    bflb_mjdec_stop(mjdec_dev);
    bflb_mjdec_pop_one_frame(mjdec_dev);
    bflb_mjdec_int_clear(mjdec_dev, MJDEC_INTCLR_ONE_FRAME);
}

/* MJDEC done: clear + wake the task. The task presents only complete frames;
 * neither the decoder ISR nor this module touches the display controller. */
static void dpi_mjdec_isr(int irq, void *arg)
{
    (void)irq;
    (void)arg;
    if (bflb_mjdec_get_intstatus(mjdec_dev) & MJDEC_INTSTS_ONE_FRAME) {
        bflb_mjdec_int_clear(mjdec_dev, MJDEC_INTCLR_ONE_FRAME);
#if defined(CONFIG_FREERTOS)
        BaseType_t hp_woken = pdFALSE;
        xSemaphoreGiveFromISR(frame_done_sem, &hp_woken);
        portYIELD_FROM_ISR(hp_woken);
#endif
    }
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
    /* MJDEC writes whole MCUs, so each source row is MCU-aligned (e.g. 172 -> 176).
     * Burst + transfer width follow the transferred row byte count, not panel timing. */
    uint8_t pixel_width = 1;
    uint32_t src_stride = MJDEC_MCU_ALIGN(w);
    uint8_t burst, transfer_width;

    dma2d_pick_burst_width(0, w, pixel_width, &burst, &transfer_width);

    /* Row span alone is insufficient for panels such as 258x960: a 32-bit
     * transfer also needs every row's source/destination address aligned. */
    uint32_t alignment = (uint32_t)src | ((uint32_t)dst + dy * dst_stride + dx) |
                         src_stride | dst_stride;
    if (transfer_width == DMA2D_DATA_WIDTH_32BIT && (alignment & 3U)) {
        transfer_width = DMA2D_DATA_WIDTH_16BIT;
    }
    if (transfer_width == DMA2D_DATA_WIDTH_16BIT && (alignment & 1U)) {
        transfer_width = DMA2D_DATA_WIDTH_8BIT;
    }

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
        .src_image_width = src_stride,
        .src_x_start = 0,
        .src_y_start = 0,
        .src_x_end = w, /* exclusive end: crop right MCU-padding columns */
        .src_y_end = h,
        .dst_image_addr = (uint32_t)dst,
        .dst_image_width = dst_stride,
        .dst_x_start = dx,
        .dst_y_start = dy,
    };

#if defined(CONFIG_FREERTOS)
    xSemaphoreTake(dma2d_done_sem, 0); /* drop stale completion after a stopped transfer */
#endif
    bflb_dma2d_image_geometric_transfor_calculate(dma2d, &cfg, &img, DMA2D_IMAGE_TRANSLATE);
    bflb_dma2d_channel_init(dma2d, &cfg);
    bflb_dma2d_channel_start(dma2d);

#if defined(CONFIG_FREERTOS)
    if (xSemaphoreTake(dma2d_done_sem, pdMS_TO_TICKS(200)) != pdTRUE) {
        bflb_dma2d_channel_stop(dma2d);
        bflb_dma2d_channel_tcint_clear(dma2d);
        return -1;
    }
#else
    while (bflb_dma2d_channel_isbusy(dma2d)) {
    }
#endif
    bflb_dma2d_channel_stop(dma2d);
    bflb_dma2d_channel_tcint_clear(dma2d);
    return 0;
}

/* Composite the decoded frame into the centered window of a panel buffer via DMA2D
 * (all in PSRAM -> no dcache work). Y at base, UV at +YUV_Y_PLANE_SIZE. */
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
    /* The DPI controller + OSD blend layer belong to main/app_display. This owns
     * only the video pipeline: MJDEC + the YUV background buffers. */
    LOG_I("[1] Init MJDEC (DPI)\r\n");
    mjdec_dev = bflb_device_get_by_name("mjdec");
    if (mjdec_dev == NULL) {
        return -2;
    }
#if defined(CONFIG_FREERTOS)
    frame_done_sem = xSemaphoreCreateBinary();
    if (frame_done_sem == NULL) {
        return -5;
    }
#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
    if (video_pause_init() != 0) {
        return -8;
    }
#endif
#endif
    bflb_irq_attach(mjdec_dev->irq_num, dpi_mjdec_isr, NULL);
    bflb_irq_enable(mjdec_dev->irq_num);

    /* Reset before start */
    GLB_MM_Software_Reset(GLB_MM_SW_MJDEC);

#if !VIDEO_FULLSCREEN
    /* Centered sub-panel video: DMA2D for the composite + done semaphore. */
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
    bflb_l1c_dcache_clean_invalidate_range(y_decode, sizeof(y_decode));
    bflb_l1c_dcache_clean_invalidate_range(uv_decode, sizeof(uv_decode));
#endif

    /* Clear both panel buffers to black: Y=0, UV=0x80 (neutral chroma). The
     * composite only overwrites the centered window, so the borders stay black.
     * Do this also in full-screen mode to remove any dirty CPU cache lines before DMA. */
    LOG_I("[2] Clear video background framebuffers\r\n");
    for (int i = 0; i < 2; i++) {
        memset(yuv_images[i], 0, YUV_Y_PLANE_SIZE);
        memset(yuv_images[i] + YUV_Y_PLANE_SIZE, 0x80,
               YUV_BUFFER_SIZE - YUV_Y_PLANE_SIZE);
        bflb_l1c_dcache_clean_invalidate_range(yuv_images[i], YUV_BUFFER_SIZE);
    }
    return 0;
}

#if defined(CONFIG_FREERTOS)
void image_switch_task(void *param)
{
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
    app_display_wait_ready();
    last_count = app_display_get_scan_frame_count();
    time_stats = bflb_mtimer_get_time_ms();
    last_frame_wake = xTaskGetTickCount();

    LOG_I("image switch task started (DPI), target=%u fps\r\n", (unsigned)VIDEO_TARGET_FPS);

    while (1) {
#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
        if (video_pause_requested) {
            video_ota_checkpoint(VIDEO_OTA_DECODER);
            /* Resume from now instead of catching up paused frame slots. */
            last_frame_wake = xTaskGetTickCount();
            time_stats = bflb_mtimer_get_time_ms();
            last_count = app_display_get_scan_frame_count();
            success_count = 0;
            timeout_count = 0;
        }
#endif
        if (frame_buffer_pop(&jpg_buffer, (TickType_t)100) != 0) {
            continue;
        }

        /* MJDEC reads the JPEG via DMA from PSRAM: make sure it is in memory. */
        bflb_l1c_dcache_clean_range(jpg_buffer->data, jpg_buffer->size);
        /* A 50ms display wait is never permission to overwrite scanning/queued data. */
        app_display_wait_writable(yuv_images[yuv_idx]);

#if VIDEO_FULLSCREEN
        /* Full-screen: decode straight into the panel-sized writable buffer. Keep
         * the original 50ms decoder wait/retry/reset, not its unsafe display reuse. */
        bool decoded = false;
        do {
            xSemaphoreTake(frame_done_sem, 0);
            if (dpi_mjdec_decode_one_frame(jpg_buffer, yuv_images[yuv_idx],
                                          yuv_images[yuv_idx] + YUV_Y_PLANE_SIZE) != 0) {
                break;
            }
            decoded = xSemaphoreTake(frame_done_sem, pdMS_TO_TICKS(50)) == pdTRUE;
            dpi_mjdec_stop();
            if (decoded) {
                break;
            }
            timeout_count++;
            reset_count++;
            if (reset_count > 2) {
                GLB_MM_Software_Reset(GLB_MM_SW_MJDEC);
                reset_count = 0;
                printf("******  mjdec reset!  ******\r\n");
                break; /* Bad input cannot prevent the OTA checkpoint forever. */
            }
        } while (1);
        if (!decoded) {
            frame_buffer_release(jpg_buffer);
            continue;
        }
        reset_count = 0;
#else
        /* Centered sub-panel: decode into compact scratch, DMA2D-composite into
         * the writable panel buffer, then present the completed composite. */
        xSemaphoreTake(frame_done_sem, 0); /* drop any stale completion */
        if (dpi_mjdec_decode_one_frame(jpg_buffer, y_decode, uv_decode) != 0) {
            frame_buffer_release(jpg_buffer);
            continue;
        }
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
            continue; /* skip, keep the same writable candidate */
        }
#endif

        /* Only app_display owns SEOF latching and FIRST/RETIRED completion.
         * Its synchronous wait must retain both planes if the IRQ is missing. */
        uint64_t t_swap = bflb_mtimer_get_time_ms();
        int ret = app_display_present(yuv_images[yuv_idx],
                                      yuv_images[yuv_idx] + YUV_Y_PLANE_SIZE);
        frame_buffer_release(jpg_buffer);
        if (ret != 0) {
            LOG_E("Base submit failed: %d; keeping writable candidate\r\n", ret);
            timeout_count++;
            continue;
        }
        uint32_t count = app_display_get_scan_frame_count();
        if (bflb_mtimer_get_time_ms() - t_swap >= 50 && count == last_count) {
            LOG_W("DPI present waited >=50ms without scan counter advance\r\n");
        }
        last_count = count; /* diagnostic only: never substitutes for ownership */
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

#endif /* VIDEO_INTERFACE_DPI / DSI */