/**
 * @file filesystem_reader.c
 * @brief Producer: read JPEG frames from SD (FATFS) into the empty->full buffer queue.
 *
 * Frames are split into 100-file chunk folders (<res>/CCC/pNNNN.jpg, CCC=(N-1)/100) so
 * each FAT32 f_open() scans only ~100 entries -> constant per-frame cost (else a flat
 * folder slows down with the frame index). Run tools/chunk_pics.sh once to lay this out.
 */

#include "filesystem_reader.h"
#include "dpi_manager.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "board.h"
#include "fatfs_diskio_register.h"
#include "ff.h"
#include "log.h"
#include "video_config.h" /* customer knobs: FRAME_DIR, FRAMES_PER_CHUNK */

#include <stdio.h>

#if defined(CONFIG_FREERTOS)
#include "task.h"
#endif

/* On DPI, SDH DAT3/DAT2 (IO43/44) clash with the panel's R7/R6, so SD runs 1-line
 * (IO45-48) there and 4-line elsewhere; override via SDH_BUS_WIDTH_1LINE. NOTE: this
 * only picks which pins are muxed -- SD driver/main must also negotiate the matching
 * bus width (suppress ACMD6 for a strict 1-line link). */
#ifndef SDH_BUS_WIDTH_1LINE
#if VIDEO_INTERFACE_DPI
#define SDH_BUS_WIDTH_1LINE 1
#else
#define SDH_BUS_WIDTH_1LINE 0
#endif
#endif

#if SDH_BUS_WIDTH_1LINE
/* Mux SDH CLK/CMD/DAT0/DAT1 on IO45-48 only, leaving IO43/44 for the DPI panel
 * (replaces board_sdh_gpio_init(), which would claim all six SDH pins). */
static void sdh_gpio_init_1line(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_45, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_46, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_47, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_48, GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}
#endif

/* FRAME_DIR and FRAMES_PER_CHUNK now live in video_config.h (customer-editable).
 * JPEG source buffers in PSRAM (MJDEC reads from here after a dcache clean). */
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64))) static uint8_t jpg_buffers[BUFFER_COUNT][JPG_BUFFER_SIZE];
_Static_assert(JPG_BUFFER_SIZE % 64U == 0, "each JPEG slot must be cache64 aligned");
_Static_assert(MAX_FRAMES > 0, "MAX_FRAMES must be positive");

static jpg_buffer_t buffer_desc[BUFFER_COUNT];

static uint32_t frame_count; /* highest frame number present (frames play 1..frame_count) */

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

/* Count playable frames: they're contiguous from 1, so binary-search "does frame N
 * open?" to find the last one in O(log n) opens instead of scanning all. */
int filesystem_count_frames(void)
{
    char path[sizeof(FRAME_DIR) + 32];
    FIL file;
    uint64_t t0 = bflb_mtimer_get_time_ms();

    /* frame 1 must exist */
    frame_path(path, sizeof(path), 1);
    if (f_open(&file, path, FA_READ) != FR_OK) {
        LOG_E("first frame missing: %s\r\n", path);
        return -1;
    }
    f_close(&file);

    /* exponential probe to bracket the end, then binary search */
    uint32_t lo = 1, hi = MAX_FRAMES > 1 ? 2 : 1;
    while (hi < MAX_FRAMES) {
        frame_path(path, sizeof(path), hi);
        if (f_open(&file, path, FA_READ) != FR_OK) {
            break;
        }
        f_close(&file);
        lo = hi;
        hi = hi > MAX_FRAMES / 2 ? MAX_FRAMES : hi * 2;
    }
    /* invariant: lo opens, hi does not (or is the cap) */
    while (lo + 1 < hi) {
        uint32_t mid = lo + (hi - lo) / 2;
        frame_path(path, sizeof(path), mid);
        if (f_open(&file, path, FA_READ) == FR_OK) {
            f_close(&file);
            lo = mid;
        } else {
            hi = mid;
        }
    }
    /* Include the cap if it exists (the original search excluded it). */
    frame_path(path, sizeof(path), hi);
    if (hi == MAX_FRAMES && f_open(&file, path, FA_READ) == FR_OK) {
        f_close(&file);
        lo = hi;
    }
    frame_count = lo;
    LOG_I("counted %lu frames in %lu ms\r\n", (unsigned long)frame_count,
          (unsigned long)(bflb_mtimer_get_time_ms() - t0));
    return (int)frame_count;
}

int filesystem_reader_init(void)
{
#if defined(CONFIG_FREERTOS)
    if (frame_buffer_pool_init(buffer_desc, jpg_buffers[0], BUFFER_COUNT, JPG_BUFFER_SIZE) != 0) {
        LOG_E("Failed to create buffer queues\r\n");
        return -1;
    }
    return 0;
#else
    return -1;
#endif
}

#if defined(CONFIG_FREERTOS)
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
#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
            video_ota_checkpoint(VIDEO_OTA_PRODUCER);
#endif
            vTaskDelay(1000);
        }
    }

    /* one-time scan: count how many frames exist (binary search of f_open probes) */
    if (filesystem_count_frames() < 0) {
        LOG_E("frame count failed, reader task idle\r\n");
        while (1) {
#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
            video_ota_checkpoint(VIDEO_OTA_PRODUCER);
#endif
            vTaskDelay(1000);
        }
    }

    LOG_I("Reading " FRAME_DIR "/CCC/pNNNN.jpg ...\r\n");

    while (1) {
#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
        video_ota_checkpoint(VIDEO_OTA_PRODUCER);
#endif
        if (frame_buffer_get(&buffer, (TickType_t)100) != 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        /* open from the per-frame chunk folder (only ~100 entries to scan) */
        char path[sizeof(FRAME_DIR) + 32];
        frame_path(path, sizeof(path), img_idx);
        res = f_open(&file, path, FA_READ);
        if (res != FR_OK) {
            /* missing frame: wrap to the start of the sequence */
            img_idx = 1;
            frame_buffer_release(buffer);
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        FSIZE_t bytes = f_size(&file);
        bytes_read = 0;
        if (!bytes || bytes > JPG_BUFFER_SIZE) {
            LOG_E("Invalid JPEG size: %s (%lu bytes, limit %u)\r\n",
                  path, (unsigned long)bytes, (unsigned)JPG_BUFFER_SIZE);
            res = FR_INVALID_PARAMETER;
        } else {
            res = f_read(&file, buffer->data, (UINT)bytes, &bytes_read);
        }
        f_close(&file);

        if (res == FR_OK && bytes_read > 0 && bytes_read == bytes) {
            buffer->size = bytes_read;
            if (frame_buffer_push(buffer, (TickType_t)10) != 0) {
                frame_buffer_release(buffer);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        } else {
            LOG_E("Failed to read frame %lu, error=%d (%u/%lu bytes)\r\n",
                  (unsigned long)img_idx, res, bytes_read, (unsigned long)bytes);
            frame_buffer_release(buffer);
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        if (++img_idx > frame_count) {
            img_idx = 1; /* loop the video */
        }
    }
}
#endif