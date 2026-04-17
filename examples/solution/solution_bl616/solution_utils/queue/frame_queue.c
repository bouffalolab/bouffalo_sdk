#include "bflb_mtimer.h"
#include "bflb_l1c.h"
#include "bflb_irq.h"

#include <compiler/compiler_gcc.h>
#include <compiler/compiler_ld.h>

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "frame_queue.h"

#if IS_ENABLED(CONFIG_PSRAM)
#define FRAME_BUFFER_ATTR ATTR_NOINIT_PSRAM_SECTION __ALIGNED(32)
#else
#define FRAME_BUFFER_ATTR __ALIGNED(32)
#endif

/******************* img_raw OUT frame **********************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT)

FRAME_BUFFER_ATTR uint8_t img_raw_out_frame_buffer[CONFIG_IMG_RAW_OUT_FRAME_NUM][CONFIG_IMG_RAW_OUT_FRAME_SIZE]; /* RAW image data */

frame_queue_ctrl_t *g_img_raw_out_frame_ctrl = NULL;

int img_raw_out_frame_ctrl_init(void)
{
    int ret;
    printf("img_raw_out_frame_ctrl_init\r\n");

    /* fill info */
    pimg_raw_frame_t frame_desc[CONFIG_IMG_RAW_OUT_FRAME_NUM];
    for (uint16_t i = 0; i < CONFIG_IMG_RAW_OUT_FRAME_NUM; i++) {
        frame_desc[i].elem_base.frame_id = i;
        frame_desc[i].elem_base.frame_size = CONFIG_IMG_RAW_OUT_FRAME_SIZE;
        frame_desc[i].elem_base.frame_addr = img_raw_out_frame_buffer[i];
        frame_desc[i].x_start = 0;
        frame_desc[i].y_start = 0;
        frame_desc[i].x_end = 0;
        frame_desc[i].y_end = 0;
        frame_desc[i].format = IMG_RAW_FRAME_FORMAT_INVALID;
    }

    /* create ctrl */
    ret = frame_queue_create(&g_img_raw_out_frame_ctrl, CONFIG_IMG_RAW_OUT_FRAME_NUM, sizeof(pimg_raw_frame_t), (void *)frame_desc, "img_raw_out");

    if (ret < 0) {
        printf("img_raw out frame ctrl create failed\r\n");
    }

    return ret;
}
#endif

/******************* img_raw IN frame **********************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_IN)

FRAME_BUFFER_ATTR uint8_t img_raw_in_frame_buffer[CONFIG_IMG_RAW_IN_FRAME_NUM][CONFIG_IMG_RAW_IN_FRAME_SIZE]; /* RAW image data */

frame_queue_ctrl_t *g_img_raw_in_frame_ctrl = NULL;

int img_raw_in_frame_ctrl_init(void)
{
    int ret;
    printf("img_raw_in_frame_ctrl_init\r\n");

    /* fill info */
    pimg_raw_frame_t frame_desc[CONFIG_IMG_RAW_IN_FRAME_NUM];
    for (uint16_t i = 0; i < CONFIG_IMG_RAW_IN_FRAME_NUM; i++) {
        frame_desc[i].elem_base.frame_id = i;
        frame_desc[i].elem_base.frame_size = CONFIG_IMG_RAW_IN_FRAME_SIZE;
        frame_desc[i].elem_base.frame_addr = img_raw_in_frame_buffer[i];
        frame_desc[i].x_start = 0;
        frame_desc[i].y_start = 0;
        frame_desc[i].x_end = 0;
        frame_desc[i].y_end = 0;
        frame_desc[i].format = IMG_RAW_FRAME_FORMAT_INVALID;
    }

    /* create ctrl */
    ret = frame_queue_create(&g_img_raw_in_frame_ctrl, CONFIG_IMG_RAW_IN_FRAME_NUM, sizeof(pimg_raw_frame_t), (void *)frame_desc, "img_raw_in");

    if (ret < 0) {
        printf("img_raw in frame ctrl create failed\r\n");
    }

    return ret;
}
#endif

/******************** mjpeg OUT frame **********************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_OUT)
FRAME_BUFFER_ATTR uint8_t mjpeg_out_frame_buffer[CONFIG_MJPEG_OUT_FRAME_NUM][CONFIG_MJPEG_OUT_FRAME_SIZE]; /* Multi buffer queue */

frame_queue_ctrl_t *g_mjpeg_out_frame_ctrl = NULL;

int mjpeg_out_frame_ctrl_init(void)
{
    int ret;

    printf("mjpeg_out_frame_ctrl_init\r\n");

    /* fill info */
    jpeg_frame_t frame_desc[CONFIG_MJPEG_OUT_FRAME_NUM];
    for (uint16_t i = 0; i < CONFIG_MJPEG_OUT_FRAME_NUM; i++) {
        frame_desc[i].elem_base.frame_id = i;
        frame_desc[i].elem_base.frame_size = CONFIG_MJPEG_OUT_FRAME_SIZE;
        frame_desc[i].elem_base.frame_addr = mjpeg_out_frame_buffer[i];
        frame_desc[i].data_size = 0;
    }

    /* create ctrl */
    ret = frame_queue_create(&g_mjpeg_out_frame_ctrl, CONFIG_MJPEG_OUT_FRAME_NUM, sizeof(jpeg_frame_t), (void *)frame_desc, "mjpeg_out");

    if (ret < 0) {
        printf("mjpeg out frame ctrl create failed\r\n");
    }

    return ret;
}
#endif

/******************** mjpeg IN frame **********************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_IN)

FRAME_BUFFER_ATTR uint8_t mjpeg_in_frame_buffer[CONFIG_MJPEG_IN_FRAME_NUM][CONFIG_MJPEG_IN_FRAME_SIZE]; /* Multi buffer queue */

frame_queue_ctrl_t *g_mjpeg_in_frame_ctrl = NULL;

int mjpeg_in_frame_ctrl_init(void)
{
    int ret;

    printf("mjpeg_in_frame_ctrl_init\r\n");

    /* fill info */
    jpeg_frame_t frame_desc[CONFIG_MJPEG_IN_FRAME_NUM];
    for (uint16_t i = 0; i < CONFIG_MJPEG_IN_FRAME_NUM; i++) {
        frame_desc[i].elem_base.frame_id = i;
        frame_desc[i].elem_base.frame_size = CONFIG_MJPEG_IN_FRAME_SIZE;
        frame_desc[i].elem_base.frame_addr = mjpeg_in_frame_buffer[i];
        frame_desc[i].data_size = 0;
    }

    /* create ctrl */
    ret = frame_queue_create(&g_mjpeg_in_frame_ctrl, CONFIG_MJPEG_IN_FRAME_NUM, sizeof(jpeg_frame_t), (void *)frame_desc, "mjpeg_in");

    if (ret < 0) {
        printf("mjpeg in frame ctrl create failed\r\n");
    }

    return ret;
}

#endif

/******************* audio input frame  **********************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_IN)

FRAME_BUFFER_ATTR uint8_t auadc_frame_buffer[CONFIG_AUDIO_IN_FRAME_NUM][CONFIG_AUDIO_IN_FRAME_SIZE]; /* Multi buffer queue */

frame_queue_ctrl_t *g_audio_in_frame_ctrl = NULL;

int auadc_in_frame_ctrl_init(void)
{
    int ret;

    printf("auadc_in_frame_ctrl_init\r\n");

    /* fill info */
    audio_in_frame_t frame_desc[CONFIG_AUDIO_IN_FRAME_NUM];
    for (uint16_t i = 0; i < CONFIG_AUDIO_IN_FRAME_NUM; i++) {
        frame_desc[i].elem_base.frame_id = i;
        frame_desc[i].elem_base.frame_size = CONFIG_AUDIO_IN_FRAME_SIZE;
        frame_desc[i].elem_base.frame_addr = auadc_frame_buffer[i];
        frame_desc[i].data_size = 0;
        frame_desc[i].frame_mode = 0;
    }

    /* create ctrl */
    ret = frame_queue_create(&g_audio_in_frame_ctrl, CONFIG_AUDIO_IN_FRAME_NUM, sizeof(audio_in_frame_t), (void *)frame_desc, "audio_in");

    if (ret < 0) {
        printf("audio input frame ctrl create failed\r\n");
    }

    return ret;
}

#endif

/******************* audio output frame  **********************/
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_OUT)

FRAME_BUFFER_ATTR uint8_t audac_frame_buffer[CONFIG_AUDIO_OUT_FRAME_NUM][CONFIG_AUDIO_OUT_FRAME_SIZE]; /* Multi buffer queue */

frame_queue_ctrl_t *g_audio_out_frame_ctrl = NULL;

int auadc_out_frame_ctrl_init(void)
{
    int ret;

    printf("auadc_out_frame_ctrl_init\r\n");

    /* fill info */
    audio_out_frame_t frame_desc[CONFIG_AUDIO_OUT_FRAME_NUM];
    for (uint16_t i = 0; i < CONFIG_AUDIO_OUT_FRAME_NUM; i++) {
        frame_desc[i].elem_base.frame_id = i;
        frame_desc[i].elem_base.frame_size = CONFIG_AUDIO_OUT_FRAME_SIZE;
        frame_desc[i].elem_base.frame_addr = audac_frame_buffer[i];
        frame_desc[i].data_size = 0;
        frame_desc[i].frame_mode = 0;
    }

    /* create ctrl */
    ret = frame_queue_create(&g_audio_out_frame_ctrl, CONFIG_AUDIO_OUT_FRAME_NUM, sizeof(audio_out_frame_t), (void *)frame_desc, "audio_out");

    if (ret < 0) {
        printf("audio output frame ctrl create failed\r\n");
    }

    return ret;
}

#endif

/******************* all frame init  **********************/
int frame_ctrl_init_all(void)
{
    int ret;
    (void)ret;

    printf("frame_ctrl_init_all\r\n");

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT)
    ret = img_raw_out_frame_ctrl_init();
    if (ret < 0) {
        return ret;
    }

#endif

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_IN)
    ret = img_raw_in_frame_ctrl_init();
    if (ret < 0) {
        return ret;
    }

#endif

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_OUT)
    ret = mjpeg_out_frame_ctrl_init();
    if (ret < 0) {
        return ret;
    }

#endif

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_IN)
    ret = mjpeg_in_frame_ctrl_init();
    if (ret < 0) {
        return ret;
    }

#endif

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_IN)
    ret = auadc_in_frame_ctrl_init();
    if (ret < 0) {
        return ret;
    }
#endif

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
    ret = auadc_out_frame_ctrl_init();
    if (ret < 0) {
        return ret;
    }
#endif

    return 0;
}
