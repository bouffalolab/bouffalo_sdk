#ifndef _FILESYSTEM_READER_H
#define _FILESYSTEM_READER_H

#include <stdint.h>
#include <stddef.h>

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "queue.h"
#endif

/* Max JPEG size per frame (720x1280 source frames are ~22KB; 64KB is safe) */
#define JPG_BUFFER_SIZE (64 * 1024)
#define BUFFER_COUNT    2

/* Upper bound on frame number (pNNNN.jpg). Used as the cap for the frame-count
 * probe in filesystem_count_frames(). */
#define MAX_FRAMES      10000

typedef struct {
    uint8_t *data;
    size_t size;
    uint16_t image_idx;
} jpg_buffer_t;

int filesystem_init(void);
int filesystem_reader_init(void);

#if defined(CONFIG_FREERTOS)
QueueHandle_t filesystem_get_full_queue(void);
QueueHandle_t filesystem_get_empty_queue(void);
void filesystem_reader_task(void *param);
#endif

#endif /* _FILESYSTEM_READER_H */
