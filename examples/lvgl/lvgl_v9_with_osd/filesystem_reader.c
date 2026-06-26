/**
 * @file filesystem_reader.c
 * @brief Read JPEG video frames pNNNN.jpg from SD card (FATFS).
 *
 * Producer side of a 2-buffer producer/consumer pipeline:
 *   empty_queue -> [read file] -> full_queue -> (image_switch_task decodes) -> empty_queue
 *
 * FPS-stability: FAT32 f_open() does a linear directory scan, so opening from one
 * flat folder of ~9500 files gets slower with the frame index and the reader falls
 * behind the decoder over time. The frames are instead split into 100-frame chunk
 * sub-folders (pic/<res>/CCC/pNNNN.jpg, CCC=(N-1)/100), so each open scans only a
 * ~100-file folder regardless of frame number -> constant per-frame open cost.
 * (Run tools/chunk_pics.sh once to reorganize a flat directory into this layout.)
 */

#include "filesystem_reader.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "board.h"
#include "fatfs_diskio_register.h"
#include "ff.h"
#include "lcd.h" 
#include "log.h"

#include <stdio.h>

/* SDH bus width selection.
 *
 * On DPI panels the SDH data lines IO43/IO44 (DAT3/DAT2) are wired to the DPI
 * R7/R6 signals, so the two peripherals cannot share those pins. When DPI is
 * active the SD card must run in 1-line mode (CLK/CMD/DAT0/DAT1 on IO45-48),
 * leaving IO43/44 free for the panel. Other interfaces keep 4-line mode.
 *
 * Define SDH_BUS_WIDTH_1LINE to 0 or 1 in the build to override the default.
 *
 * NOTE: this switch only controls which SDH pins this example muxes; it does
 * NOT touch the SD core. The core (bsp/common/sdh/sd/sdh_sd.c) still negotiates
 * a 4-bit transfer via ACMD6 whenever the card reports 4-bit support, so with
 * only this change the controller is still told to use 4 data lines while
 * IO43/44 are not driven. On most cards 1-line transfers over DAT0 still work
 * because DAT2/DAT3 are not sampled for 1-block reads, but for a guaranteed
 * 1-line link the ACMD6 negotiation in sdh_sd.c must also be suppressed. */
#ifndef SDH_BUS_WIDTH_1LINE
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI)
#define SDH_BUS_WIDTH_1LINE 1
#else
#define SDH_BUS_WIDTH_1LINE 0
#endif
#endif

#if SDH_BUS_WIDTH_1LINE
/* 1-line SDH GPIO init: bring up CLK/CMD/DAT0/DAT1 on IO45-48 and deliberately
 * leave IO43/IO44 (DAT3/DAT2) unconfigured so the DPI panel keeps R7/R6. This
 * replaces board_sdh_gpio_init(), which would claim all six SDH pins. */
static void sdh_gpio_init_1line(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_45, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_46, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_47, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_48, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}
#endif

/* Frames live under /sd/<res>/CCC/pNNNN.jpg. Resolution follows the active panel:
 * DSI is 720x1280 portrait, DPI standard is 800x480 landscape. Put a JPEG sequence
 * matching the panel under the corresponding folder on the SD card. 
 * (384x768 480x960) (576x1024 720x1280) (172x640 none) (700x420 800x480) (960x540 1024x600) */
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI)
#define FRAME_DIR        "/sd/1024x600"
#else
#define FRAME_DIR        "/sd/720x1280"
#endif
#define FRAMES_PER_CHUNK 100

/* JPEG source buffers in PSRAM (MJDEC reads from here after a dcache clean). */
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) static uint8_t jpg_buffers[BUFFER_COUNT][JPG_BUFFER_SIZE];

static jpg_buffer_t buffer_desc[BUFFER_COUNT];

static uint32_t frame_count; /* highest frame number present (frames play 1..frame_count) */

#if defined(CONFIG_FREERTOS)
static QueueHandle_t full_buffer_queue = NULL;
static QueueHandle_t empty_buffer_queue = NULL;
#endif

static FATFS fs;

int filesystem_init(void)
{
    FRESULT ret;

#if SDH_BUS_WIDTH_1LINE
    sdh_gpio_init_1line(); /* skip IO43/44 so DPI keeps R7/R6 */
#else
    board_sdh_gpio_init();
#endif
    fatfs_sdh_driver_register();

    ret = f_mount(&fs, "/sd", 1);
    if (ret == FR_NO_FILESYSTEM) {
        LOG_E("No filesystem found on SD card\r\n");
        return -1;
    }
    if (ret != FR_OK) {
        LOG_E("Failed to mount filesystem, error=%d\r\n", ret);
        return -1;
    }
    LOG_I("FileSystem type: %s\r\n", fs.fs_type == 1 ? "FAT12" :
                                     fs.fs_type == 2 ? "FAT16" :
                                     fs.fs_type == 3 ? "FAT32" :
                                     fs.fs_type == 4 ? "exFAT" : "unknown");
    return 0;
}

/* Build the chunked path for a frame: FRAME_DIR/CCC/pNNNN.jpg. */
static void frame_path(char *buf, size_t buflen, uint32_t frame_num)
{
    uint32_t chunk = (frame_num - 1) / FRAMES_PER_CHUNK;
    snprintf(buf, buflen, FRAME_DIR "/%03lu/p%04lu.jpg", (unsigned long)chunk, (unsigned long)frame_num);
}

/* Count playable frames by probing how far the pNNNN sequence goes. Frames are
 * contiguous from 1, so a binary search over "does frame N open?" finds the last
 * one in O(log n) opens instead of scanning all of them. */
int filesystem_count_frames(void)
{
    char path[48];
    FIL fil;
    uint64_t t0 = bflb_mtimer_get_time_ms();

    /* frame 1 must exist */
    frame_path(path, sizeof(path), 1);
    if (f_open(&fil, path, FA_READ) != FR_OK) {
        LOG_E("first frame missing: %s\r\n", path);
        return -1;
    }
    f_close(&fil);

    /* exponential probe to bracket the end, then binary search */
    uint32_t lo = 1, hi = 2;
    while (hi < MAX_FRAMES) {
        frame_path(path, sizeof(path), hi);
        if (f_open(&fil, path, FA_READ) != FR_OK) {
            break;
        }
        f_close(&fil);
        lo = hi;
        hi *= 2;
    }
    if (hi > MAX_FRAMES) {
        hi = MAX_FRAMES;
    }
    /* invariant: lo opens, hi does not (or is the cap) */
    while (lo + 1 < hi) {
        uint32_t mid = lo + (hi - lo) / 2;
        frame_path(path, sizeof(path), mid);
        if (f_open(&fil, path, FA_READ) == FR_OK) {
            f_close(&fil);
            lo = mid;
        } else {
            hi = mid;
        }
    }

    frame_count = lo;
    LOG_I("counted %lu frames in %lu ms\r\n", (unsigned long)frame_count,
          (unsigned long)(bflb_mtimer_get_time_ms() - t0));
    return (int)frame_count;
}
int filesystem_reader_init(void)
{
#if defined(CONFIG_FREERTOS)
    full_buffer_queue = xQueueCreate(BUFFER_COUNT, sizeof(jpg_buffer_t *));
    empty_buffer_queue = xQueueCreate(BUFFER_COUNT, sizeof(jpg_buffer_t *));
    if (full_buffer_queue == NULL || empty_buffer_queue == NULL) {
        LOG_E("Failed to create buffer queues\r\n");
        return -1;
    }
    for (int i = 0; i < BUFFER_COUNT; i++) {
        buffer_desc[i].data = jpg_buffers[i];
        buffer_desc[i].size = 0;
        buffer_desc[i].image_idx = 0;
        jpg_buffer_t *buf_ptr = &buffer_desc[i];
        xQueueSend(empty_buffer_queue, &buf_ptr, (TickType_t)0);
    }
    return 0;
#else
    return -1;
#endif
}

#if defined(CONFIG_FREERTOS)
QueueHandle_t filesystem_get_full_queue(void)
{
    return full_buffer_queue;
}

QueueHandle_t filesystem_get_empty_queue(void)
{
    return empty_buffer_queue;
}

void filesystem_reader_task(void *param)
{
    FIL file;
    FRESULT res;
    UINT bytes_read;
    jpg_buffer_t *buffer;
    uint32_t img_idx = 1;

    (void)param;

    if (filesystem_init() < 0) {
        LOG_E("filesystem init failed, reader task idle\r\n");
        while (1) {
            vTaskDelay(1000);
        }
    }

    /* one-time scan: count how many frames exist (binary search of f_open probes) */
    if (filesystem_count_frames() < 0) {
        LOG_E("frame count failed, reader task idle\r\n");
        while (1) {
            vTaskDelay(1000);
        }
    }

    LOG_I("Reading " FRAME_DIR "/CCC/pNNNN.jpg ...\r\n");

    while (1) {
        if (xQueueReceive(empty_buffer_queue, &buffer, (TickType_t)100) != pdTRUE) {
            vTaskDelay(1);
            continue;
        }

        /* open from the per-frame chunk folder (only ~100 entries to scan) */
        char path[48];
        frame_path(path, sizeof(path), img_idx);
        res = f_open(&file, path, FA_READ);
        if (res != FR_OK) {
            /* missing frame: wrap to the start of the sequence */
            img_idx = 1;
            xQueueSend(empty_buffer_queue, &buffer, (TickType_t)0);
            continue;
        }

        res = f_read(&file, buffer->data, JPG_BUFFER_SIZE, &bytes_read);
        f_close(&file);

        if (res == FR_OK && bytes_read > 0) {
            buffer->size = bytes_read;
            buffer->image_idx = (uint16_t)img_idx;
            if (xQueueSend(full_buffer_queue, &buffer, (TickType_t)10) != pdTRUE) {
                xQueueSend(empty_buffer_queue, &buffer, (TickType_t)0);
            }
        } else {
            LOG_E("Failed to read frame %lu, error=%d\r\n", (unsigned long)img_idx, res);
            xQueueSend(empty_buffer_queue, &buffer, (TickType_t)0);
        }

        if (++img_idx > frame_count) {
            img_idx = 1; /* loop the video */
        }
    }
}
#endif
