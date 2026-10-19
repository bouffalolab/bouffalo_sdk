#ifndef _FILESYSTEM_READER_H
#define _FILESYSTEM_READER_H

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#endif

/* Max JPEG size per frame (720x1280 source frames are ~22KB; 64KB is safe) */
#define JPG_BUFFER_SIZE (128 * 1024)
#define BUFFER_COUNT    2

/* Upper bound on frame number (pNNNN.jpg). Used as the cap for the frame-count
 * probe in filesystem_count_frames(). */
#define MAX_FRAMES      10000

int filesystem_init(void);
int filesystem_reader_init(void);

#if defined(CONFIG_FREERTOS)
void filesystem_reader_task(void *param);
#endif

#endif /* _FILESYSTEM_READER_H */
