#ifndef __FRAME_QUEUE_H__
#define __FRAME_QUEUE_H__

#include "stdint.h"
#include "frame_queue_ctrl.h"

/******************* yuyv frame cfg **********************/
#if (CONFIG_SOLUTION_QUEUE_YUYV)
/* frame pool cfg */
#define YUYV_FRAME_SHARE_EN          (0)
#define YUYV_FRAME_SIZE              (CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH * CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT * 2)
#define YUYV_FRAME_NUM               (2)

/* jpeg out_queue cfg */
#define YUYV_FRAME_STREAM_JPEG_ID    (0)
#define YUYV_FRAME_STREAM_JPEG_DEPTH (1)

/* dbi_display out_queue cfg */
#define YUYV_FRAME_STREAM_DISP_ID    (1)
#define YUYV_FRAME_STREAM_DISP_DEPTH (1)

typedef struct {
    frame_elem_t elem_base;

    uint16_t x_start;
    uint16_t y_start;
    uint16_t x_end;
    uint16_t y_end;
} pyuyv_frame_t;

extern frame_queue_ctrl_t *g_yuyv_frame_ctrl;

int yuyv_frame_ctrl_init(void);

#endif

/******************* jpeg frame cfg **********************/
#if (CONFIG_SOLUTION_QUEUE_MJPEG)

/* frame pool cfg */
#define MJPEG_FRAME_SIZE                  (150 * 1024)
#define MJPEG_FRAME_NUM                   (6)

/* sd avi out_queue cfg */
#define MJPEG_FRAME_STREAM_SD_AVI_ID      (0)
#define MJPEG_FRAME_STREAM_SD_AVI_DEPTH   (2)
/* sd jpeg out_queue cfg */
#define MJPEG_FRAME_STREAM_SD_JPEG_ID     (1)
#define MJPEG_FRAME_STREAM_SD_JPEG_DEPTH  (2)
/* wifi rtc out_queue cfg */
#define MJPEG_FRAME_STREAM_WIFI_RTC_ID    (2)
#define MJPEG_FRAME_STREAM_WIFI_RTC_DEPTH (1)

typedef struct {
    frame_elem_t elem_base;

    uint32_t data_size;
} jpeg_frame_t;

extern frame_queue_ctrl_t *g_jpeg_frame_ctrl;

int jpeg_frame_ctrl_init(void);

#endif

/******************* audio input frame cfg **********************/
#if (CONFIG_SOLUTION_QUEUE_AUDIO_IN)

/* frame pool cfg */
#define AUDIO_IN_FRAME_SIZE                  (1024)
#define AUDIO_IN_FRAME_NUM                   (20)

/* audio lookback out_queue cfg */
#define AUDIO_IN_FRAME_STREAM_LOOKBACK_ID    (0)
#define AUDIO_IN_FRAME_STREAM_LOOKBACK_DEPTH (20)

/* audio agora_rtc out_queue cfg */
#define AUDIO_IN_FRAME_STREAM_AGORA_ID       (1)
#define AUDIO_IN_FRAME_STREAM_AGORA_DEPTH    (10)

typedef struct {
    frame_elem_t elem_base;

    uint16_t data_size;
    uint16_t frame_mode;
} audio_in_frame_t;

extern frame_queue_ctrl_t *g_audio_in_frame_ctrl;

int auadc_in_frame_ctrl_init(void);

#endif

/******************* audio output frame cfg **********************/
#if (CONFIG_SOLUTION_QUEUE_AUDIO_OUT)

/* frame pool cfg */
#define AUDIO_OUT_FRAME_SIZE               (1024)
#define AUDIO_OUT_FRAME_NUM                (20)

/* audio audac out_queue cfg */
#define AUDIO_OUT_FRAME_STREAM_AUDAC_ID    (0)
#define AUDIO_OUT_FRAME_STREAM_AUDAC_DEPTH (20)

/* audio i2s out_queue cfg */
#define AUDIO_OUT_FRAME_STREAM_I2S_ID      (1)
#define AUDIO_OUT_FRAME_STREAM_I2S_DEPTH   (20)

/* audio sd .avi out_queue cfg */
#define AUDIO_OUT_FRAME_STREAM_SD_AVI_ID      (2)
#define AUDIO_OUT_FRAME_STREAM_SD_AVI_DEPTH   (10)

typedef struct {
    frame_elem_t elem_base;

    uint16_t data_size;
    uint16_t frame_mode;
} audio_out_frame_t;

extern frame_queue_ctrl_t *g_audio_out_frame_ctrl;

int auadc_out_frame_ctrl_init(void);

#endif

/******************* all frame init  **********************/
int frame_ctrl_init_all(void);

#endif /* __DVP_MJPEG_H__ */
