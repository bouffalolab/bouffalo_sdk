#ifndef __MJPEG_SW_BUFFER_H__
#define __MJPEG_SW_BUFFER_H__

#include <string.h>
#include <stdint.h>
typedef enum {
    H264_MAIN,
    H264_SUB,
    MJPEG,
    RAW,
    YUV,
    BLANK,
}img_mode;

typedef struct mjpeg_sw_fifo_item {
    uint16_t height;
    uint16_t width;
    uint32_t length;
    uint8_t quality;
    uint8_t _pad0[7];
    uint8_t data[];
} mjpeg_sw_fifo_item_t;

#define MJPEG_SW_FIFO_CAP 4
#ifdef BL808
#define MJPEG_SW_FIFO_ITEM_PAYLOAD_SIZE_MAX (1 * 1024 * 1024)
#else
#define MJPEG_SW_FIFO_ITEM_PAYLOAD_SIZE_MAX (1 * 200 * 1024)
#endif

#define MJPEG_SW_FIFO_ITEM_SIZE_MAX (sizeof(mjpeg_sw_fifo_item_t) + MJPEG_SW_FIFO_ITEM_PAYLOAD_SIZE_MAX)
#define MJPEG_SW_FIFO_BUFFER_SIZE (MJPEG_SW_FIFO_CAP * MJPEG_SW_FIFO_ITEM_SIZE_MAX)

#define DEFAULT_MJPEG_Q_USED_MAX_IDX 3 // 50
#define DEFAULT_MJPEG_Q_USED_MIN_IDX 2 // 25

#define DEFAULT_MJPEG_Q_UPGRADE_THRESHOLD_SIZE   (15 * 1024)
#define DEFAULT_MJPEG_Q_DOWNGRADE_THRESHOLD_SIZE (45 * 1024)

int mjpeg_sw_fifo_init(uint16_t width, uint16_t height);
int mjpeg_sw_fifo_uninit();
int mjpeg_sw_fifo_config(uint8_t q_max, uint8_t q_min,
        uint32_t q_upgrade_threshold_size, uint32_t q_downgrade_threshold_size);

int mjpeg_sw_fifo_frame_get(uint32_t *frames, uint8_t **ptr1, uint32_t *len1, uint8_t **ptr2, uint32_t *len2,
        uint8_t *quality, uint16_t *width, uint16_t *height, int pic_mode);
int mjpeg_sw_fifo_frame_pop(void);

#endif
