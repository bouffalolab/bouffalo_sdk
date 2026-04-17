#ifndef __FRAME_QUEUE_H__
#define __FRAME_QUEUE_H__

#include "stdint.h"
#include "frame_queue_ctrl.h"

/******************* img_raw frame cfg **********************/
typedef enum {
    IMG_RAW_FRAME_FORMAT_INVALID = 0,

    IMG_RAW_FRAME_FORMAT_RGB565,
    IMG_RAW_FRAME_FORMAT_RGB888,
    IMG_RAW_FRAME_FORMAT_NRGB8888,
    IMG_RAW_FRAME_FORMAT_YUYV,
    IMG_RAW_FRAME_FORMAT_YUV,
} img_raw_frame_format_t;

typedef struct {
    frame_elem_t elem_base;

    uint16_t x_start;
    uint16_t y_start;
    uint16_t x_end;
    uint16_t y_end;
    img_raw_frame_format_t format;
} pimg_raw_frame_t;

/************ img_raw out frame cfg ************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT)
/* out frame pool cfg */
#ifndef CONFIG_IMG_RAW_OUT_FRAME_SIZE
#define CONFIG_IMG_RAW_OUT_FRAME_SIZE (CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH * CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT * 2)
#endif
#ifndef CONFIG_IMG_RAW_OUT_FRAME_NUM
#define CONFIG_IMG_RAW_OUT_FRAME_NUM (2)
#endif

/* img_raw out_queue cfg */
#define IMG_RAW_OUT_FRAME_STREAM_JPEG_ID    (0)
#define IMG_RAW_OUT_FRAME_STREAM_JPEG_DEPTH (1)

/* dbi_display out_queue cfg */
#define IMG_RAW_OUT_FRAME_STREAM_DISP_ID    (1)
#define IMG_RAW_OUT_FRAME_STREAM_DISP_DEPTH (1)

extern frame_queue_ctrl_t *g_img_raw_out_frame_ctrl;
int img_raw_out_frame_ctrl_init(void);
#endif

/************ img_raw in frame cfg ************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_IN)
/* in frame pool cfg */
#ifndef CONFIG_IMG_RAW_IN_FRAME_SIZE
#define CONFIG_IMG_RAW_IN_FRAME_SIZE (CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH * CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT * 2)
#endif
#ifndef CONFIG_IMG_RAW_IN_FRAME_NUM
#define CONFIG_IMG_RAW_IN_FRAME_NUM (2)
#endif

/* img_raw in_queue cfg */
#define IMG_RAW_IN_FRAME_STREAM_JPEG_ID    (0)
#define IMG_RAW_IN_FRAME_STREAM_JPEG_DEPTH (1)

/* dbi_display in_queue cfg */
#define IMG_RAW_IN_FRAME_STREAM_DISP_ID    (1)
#define IMG_RAW_IN_FRAME_STREAM_DISP_DEPTH (1)

extern frame_queue_ctrl_t *g_img_raw_in_frame_ctrl;
int img_raw_in_frame_ctrl_init(void);
#endif

/******************* jpeg frame cfg **********************/
typedef struct {
    frame_elem_t elem_base;

    uint32_t data_size;
} jpeg_frame_t;

/************ mjpeg out frame cfg ************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_OUT)
/* out frame pool cfg */
#ifndef CONFIG_MJPEG_OUT_FRAME_SIZE
#define CONFIG_MJPEG_OUT_FRAME_SIZE (128 * 1024)
#endif
#ifndef CONFIG_MJPEG_OUT_FRAME_NUM
#define CONFIG_MJPEG_OUT_FRAME_NUM (6)
#endif

/* sd avi out_queue cfg */
#define MJPEG_OUT_FRAME_STREAM_SD_ID          (0)
#define MJPEG_OUT_FRAME_STREAM_SD_DEPTH       (2)
/* sd jpeg out_queue cfg */
#define MJPEG_OUT_FRAME_STREAM_SD_ID          (0)
#define MJPEG_OUT_FRAME_STREAM_SD_DEPTH       (2)
/* wifi rtc out_queue cfg */
#define MJPEG_OUT_FRAME_STREAM_WIFI_RTC_ID    (2)
#define MJPEG_OUT_FRAME_STREAM_WIFI_RTC_DEPTH (1)

extern frame_queue_ctrl_t *g_mjpeg_out_frame_ctrl;
int mjpeg_out_frame_ctrl_init(void);
#endif

/************ mjpeg in frame cfg ************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_IN)
/* in frame pool cfg */
#ifndef CONFIG_MJPEG_IN_FRAME_SIZE
#define CONFIG_MJPEG_IN_FRAME_SIZE (128 * 1024)
#endif
#ifndef CONFIG_MJPEG_IN_FRAME_NUM
#define CONFIG_MJPEG_IN_FRAME_NUM (6)
#endif

/* sd avi in_queue cfg */
#define MJPEG_IN_FRAME_STREAM_SD_ID     (0)
#define MJPEG_IN_FRAME_STREAM_SD_DEPTH  (2)
/* sd jpeg in_queue cfg */
#define MJPEG_IN_FRAME_STREAM_SD_ID     (0)
#define MJPEG_IN_FRAME_STREAM_SD_DEPTH  (2)
/* dec in_queue cfg */
#define MJPEG_IN_FRAME_STREAM_DEC_ID    (2)
#define MJPEG_IN_FRAME_STREAM_DEC_DEPTH (5)

extern frame_queue_ctrl_t *g_mjpeg_in_frame_ctrl;
int mjpeg_in_frame_ctrl_init(void);
#endif

/******************* audio input frame cfg **********************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_IN)

/* frame pool cfg */
#ifndef CONFIG_AUDIO_IN_FRAME_SIZE
#define CONFIG_AUDIO_IN_FRAME_SIZE (1024)
#endif
#ifndef CONFIG_AUDIO_IN_FRAME_NUM
#define CONFIG_AUDIO_IN_FRAME_NUM (20)
#endif

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
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_OUT)

/* frame pool cfg */
#ifndef CONFIG_AUDIO_OUT_FRAME_SIZE
#define CONFIG_AUDIO_OUT_FRAME_SIZE (1024)
#endif
#ifndef CONFIG_AUDIO_OUT_FRAME_NUM
#define CONFIG_AUDIO_OUT_FRAME_NUM (20)
#endif

/* audio audac out_queue cfg */
#define AUDIO_OUT_FRAME_STREAM_AUDAC_ID    (0)
#define AUDIO_OUT_FRAME_STREAM_AUDAC_DEPTH (20)

/* audio i2s out_queue cfg */
#define AUDIO_OUT_FRAME_STREAM_I2S_ID      (1)
#define AUDIO_OUT_FRAME_STREAM_I2S_DEPTH   (20)

/* audio sd .avi out_queue cfg */
#define AUDIO_OUT_FRAME_STREAM_SD_ID       (2)
#define AUDIO_OUT_FRAME_STREAM_SD_DEPTH    (10)

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

#endif /* __FRAME_QUEUE_H__ */
