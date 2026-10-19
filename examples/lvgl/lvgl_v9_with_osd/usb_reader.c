/**
 * @file usb_reader.c
 * @brief USB-ACM JPEG video producer.
 *
 * The app owns the video DATA ACM endpoint (app_usb_composite); this reader
 * advances one non-blocking state machine from USB configuration through frame
 * header, JPEG payload, and rejected-payload drain.
 */

#include "usb_reader.h"

#include "app_usb_composite.h"
#include "dpi_manager.h"

#include "bflb_mtimer.h"
#include "board.h"
#include "compiler/compiler_ld.h"
#include "usb_config.h"
#include "usb_util.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <stdbool.h>
#include <string.h>

#define DBG_TAG "USBRD"
#include "log.h"

#define FRAME_MAGIC   0x55AA55AAUL
#define FRAME_HDR_LEN 12U
#define FRAME_MIN_LEN 4U
#define FRAME_MAX_LEN JPG_BUFFER_SIZE

#ifndef USB_READER_RX_CHUNK_SIZE
#define USB_READER_RX_CHUNK_SIZE 2048U
#endif
#ifndef USB_READER_STATS_LOG
#define USB_READER_STATS_LOG 1
#endif
#ifndef USB_READER_STATS_LOG_PERIOD_MS
#define USB_READER_STATS_LOG_PERIOD_MS 5000U
#endif

#define USB_READER_POLL_MS 20U

#define USB_READER_CFG_WAIT_MS 100U


/* 12-byte little-endian video frame header. Read straight off the wire (the
 * host emits it packed), so the struct must not gain padding. */
typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t length;
    uint16_t idx;
    uint16_t reserved;
} usb_frame_hdr_t;

ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t jpg_buffers[BUFFER_COUNT][JPG_BUFFER_SIZE];
static jpg_buffer_t buffer_desc[BUFFER_COUNT];

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t
    usb_rx_scratch[USB_ALIGN_UP(USB_READER_RX_CHUNK_SIZE, CONFIG_USB_ALIGN_SIZE)];

typedef struct {
    uint32_t rx_bytes;
    uint32_t frames_ok;
    uint32_t frames_dropped;
} usb_reader_stats_t;

static usb_reader_stats_t stats;

typedef enum {
    USB_READER_STATE_RECV_HEADER = 0,
    USB_READER_STATE_RECV_PAYLOAD,
    USB_READER_STATE_DRAIN_BAD_PAYLOAD,
} usb_reader_state_t;

typedef enum {
    USB_READER_RECV_WAIT = 0,
    USB_READER_RECV_DONE,
    USB_READER_RECV_ERROR,
} usb_reader_recv_status_t;

/* One OUT transfer can be in flight. The callback only publishes its result;
 * usb_reader_polling() owns all state and buffer transitions. */
typedef struct {
    TaskHandle_t task_handle;
    volatile bool configured;
    volatile bool read_pending;
    volatile uint32_t read_done_len;
    volatile bool reset_pending;
    usb_reader_state_t state;
    uint32_t rx_offset;
    uint32_t rx_request_len;
    usb_frame_hdr_t header;
    jpg_buffer_t *jpg_buffer;
} usb_reader_ctx_t;

static usb_reader_ctx_t reader = {
    .state = USB_READER_STATE_RECV_HEADER,
};

static void usb_reader_notify_task(void)
{
    if (reader.task_handle == NULL) {
        return;
    }

    if (xPortIsInsideInterrupt()) {
        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(reader.task_handle, &woken);
        portYIELD_FROM_ISR(woken);
    } else {
        xTaskNotifyGive(reader.task_handle);
    }
}

/* OUT-done fires in ISR context when the armed read completes. */
static void usb_reader_video_out_done_cb(void *arg, uint32_t len)
{
    (void)arg;
    reader.read_done_len = len;
    reader.read_pending = false;
    usb_reader_notify_task();
}

static void usb_reader_usb_event_cb(void *arg, int configured)
{
    (void)arg;
    reader.configured = (configured != 0);
    if (!configured) {
        reader.read_pending = false;
        reader.reset_pending = true;
    }
    usb_reader_notify_task();
}

static int usb_reader_start_video_out_read(uint8_t *data, uint32_t len)
{
    int ret;

    if (!reader.configured) {
        return -1;
    }

    reader.read_done_len = 0;
    reader.read_pending = true;
    ret = app_usb_video_start_out_read(data, len);
    if (ret != 0) {
        reader.read_pending = false;
    }
    return ret;
}

static void usb_reader_wait_for_event(TickType_t timeout_ticks)
{
    (void)ulTaskNotifyTake(pdTRUE, timeout_ticks);
}

static int usb_reader_start_next_chunk(uint32_t total_len)
{
    if (reader.rx_offset >= total_len) {
        return -1;
    }

    reader.rx_request_len = total_len - reader.rx_offset;
    if (reader.rx_request_len > USB_READER_RX_CHUNK_SIZE) {
        reader.rx_request_len = USB_READER_RX_CHUNK_SIZE;
    }

    return usb_reader_start_video_out_read(usb_rx_scratch, reader.rx_request_len);
}

static int usb_reader_consume_chunk(uint8_t *dst, uint32_t total_len)
{
    uint32_t done_len = reader.read_done_len;

    if (done_len == 0U || done_len > reader.rx_request_len ||
        done_len > (total_len - reader.rx_offset)) {
        return -1;
    }

    if (dst != NULL) {
        memcpy(dst + reader.rx_offset, usb_rx_scratch, done_len);
    }
    reader.rx_offset += done_len;
    reader.rx_request_len = 0;
    stats.rx_bytes += done_len;
    return 0;
}

static usb_reader_recv_status_t usb_reader_receive(uint8_t *dst, uint32_t total_len)
{
    if (reader.rx_request_len != 0U) {
        if (reader.read_pending) {
            return USB_READER_RECV_WAIT;
        }
        if (usb_reader_consume_chunk(dst, total_len) != 0) {
            return USB_READER_RECV_ERROR;
        }
    }

    if (reader.rx_offset >= total_len) {
        return USB_READER_RECV_DONE;
    }
    if (usb_reader_start_next_chunk(total_len) != 0) {
        return USB_READER_RECV_ERROR;
    }
    return USB_READER_RECV_WAIT;
}

static void usb_reader_reset_receive(usb_reader_state_t state)
{
    reader.state = state;
    reader.rx_offset = 0;
    reader.rx_request_len = 0;
}

static void usb_reader_release_buffer(void)
{
    (void)frame_buffer_release(reader.jpg_buffer);
    reader.jpg_buffer = NULL;
}

static bool usb_reader_header_is_valid(const usb_frame_hdr_t *header)
{
    return header->magic == FRAME_MAGIC && header->reserved == 0U &&
           header->length >= FRAME_MIN_LEN && header->length <= FRAME_MAX_LEN;
}

static void usb_reader_reset_stream(void)
{
    if (reader.jpg_buffer != NULL) {
        usb_reader_release_buffer();
    }

    reader.read_pending = false;
    reader.read_done_len = 0;
    usb_reader_reset_receive(USB_READER_STATE_RECV_HEADER);
}

static void usb_reader_process_completed_frame(void)
{
    uint8_t *jpeg_data;
    uint32_t jpeg_len;

    jpeg_data = reader.jpg_buffer->data;
    jpeg_len = reader.header.length;
    if (jpeg_data[0] != 0xFFU || jpeg_data[1] != 0xD8U || jpeg_data[jpeg_len - 2U] != 0xFFU ||
        jpeg_data[jpeg_len - 1U] != 0xD9U) {
        stats.frames_dropped++;
        usb_reader_release_buffer();
    } else if (frame_buffer_push(reader.jpg_buffer, (TickType_t)0) != 0) {
        stats.frames_dropped++;
        usb_reader_release_buffer();
    } else {
        stats.frames_ok++; /* handed off to the ready queue */
        reader.jpg_buffer = NULL;
    }
}

int usb_reader_init(void)
{
#if defined(CONFIG_FREERTOS)
    if (frame_buffer_pool_init(buffer_desc, jpg_buffers[0], BUFFER_COUNT, JPG_BUFFER_SIZE) != 0) {
        LOG_E("Failed to create frame queues\r\n");
        return -1;
    }

    usb_reader_reset_stream();
    app_usb_video_register_callbacks(usb_reader_video_out_done_cb, usb_reader_usb_event_cb, NULL);

    LOG_I("USB CDC-ACM video reader init done\r\n");
    return 0;
#else
    return -1;
#endif
}

#if defined(CONFIG_FREERTOS)
static int usb_reader_polling(void)
{
    usb_reader_recv_status_t recv_status;

    if (reader.reset_pending) {
        reader.reset_pending = false;
        usb_reader_reset_stream();
    }

    if (!reader.configured) {
        usb_reader_wait_for_event(pdMS_TO_TICKS(USB_READER_CFG_WAIT_MS));
        return 0;
    }

polling_continue:

    switch (reader.state) {
        case USB_READER_STATE_RECV_HEADER:
            recv_status = usb_reader_receive((uint8_t *)&reader.header, FRAME_HDR_LEN);
            if (recv_status == USB_READER_RECV_WAIT) {
                usb_reader_wait_for_event(pdMS_TO_TICKS(USB_READER_CFG_WAIT_MS));
                break;
            }
            if (recv_status == USB_READER_RECV_ERROR) {
                usb_reader_reset_stream();
                usb_reader_wait_for_event(pdMS_TO_TICKS(USB_READER_POLL_MS));
                break;
            }

            if (!usb_reader_header_is_valid(&reader.header)) {
                stats.frames_dropped++;
                LOG_W("bad video header: magic=0x%08lx len=%lu resv=0x%04x, drain payload\r\n",
                      (unsigned long)reader.header.magic, (unsigned long)reader.header.length,
                      (unsigned int)reader.header.reserved);
                usb_reader_reset_receive(USB_READER_STATE_DRAIN_BAD_PAYLOAD);
            } else {
                usb_reader_reset_receive(USB_READER_STATE_RECV_PAYLOAD);
            }
            goto polling_continue;

        case USB_READER_STATE_RECV_PAYLOAD:
            if (reader.jpg_buffer == NULL) {
                if (frame_buffer_get(&reader.jpg_buffer, pdMS_TO_TICKS(USB_READER_POLL_MS)) != 0) {
                    break;
                }
                reader.jpg_buffer->size = reader.header.length;
            }

            recv_status = usb_reader_receive(reader.jpg_buffer->data, reader.header.length);
            if (recv_status == USB_READER_RECV_WAIT) {
                usb_reader_wait_for_event(pdMS_TO_TICKS(USB_READER_CFG_WAIT_MS));
                break;
            }
            if (recv_status == USB_READER_RECV_ERROR) {
                usb_reader_reset_stream();
                usb_reader_wait_for_event(pdMS_TO_TICKS(USB_READER_POLL_MS));
                break;
            }

            usb_reader_process_completed_frame();
            usb_reader_reset_receive(USB_READER_STATE_RECV_HEADER);
            goto polling_continue;

        case USB_READER_STATE_DRAIN_BAD_PAYLOAD:
            recv_status = usb_reader_receive(NULL, reader.header.length);
            if (recv_status == USB_READER_RECV_WAIT) {
                usb_reader_wait_for_event(pdMS_TO_TICKS(USB_READER_CFG_WAIT_MS));
                break;
            }
            if (recv_status == USB_READER_RECV_ERROR) {
                usb_reader_reset_stream();
                usb_reader_wait_for_event(pdMS_TO_TICKS(USB_READER_POLL_MS));
                break;
            }

            usb_reader_reset_receive(USB_READER_STATE_RECV_HEADER);
            goto polling_continue;

        default:
            usb_reader_reset_stream();
            break;
    }

    return 0;
}

void usb_reader_task(void *param)
{
#if USB_READER_STATS_LOG
    uint64_t t_stats = bflb_mtimer_get_time_ms();
    uint32_t last_rx_bytes = 0;
    uint32_t last_frames_ok = 0;
    uint32_t last_frames_dropped = 0;
#endif

    (void)param;
    LOG_I("usb_reader_task waiting for USB host video stream...\r\n");

    reader.task_handle = xTaskGetCurrentTaskHandle();

    while (1) {
        usb_reader_polling();

#if USB_READER_STATS_LOG
        {
            uint64_t now = bflb_mtimer_get_time_ms();
            if (now - t_stats >= USB_READER_STATS_LOG_PERIOD_MS) {
                uint32_t dt = (uint32_t)(now - t_stats);
                uint32_t rx_bytes = stats.rx_bytes;
                uint32_t frames_ok = stats.frames_ok;
                uint32_t frames_dropped = stats.frames_dropped;

                LOG_I("usb stream: rx=%lu KB/s fps=%lu drop=%lu\r\n",
                      (unsigned long)((rx_bytes - last_rx_bytes) * 1000U / dt / 1024U),
                      (unsigned long)((frames_ok - last_frames_ok) * 1000U / dt),
                      (unsigned long)(frames_dropped - last_frames_dropped));

                t_stats = now;
                last_rx_bytes = rx_bytes;
                last_frames_ok = frames_ok;
                last_frames_dropped = frames_dropped;
            }
        }
#endif
    }
}
#endif
