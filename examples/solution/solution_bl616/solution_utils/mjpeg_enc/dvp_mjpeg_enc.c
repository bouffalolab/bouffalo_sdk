#include "bflb_mtimer.h"
#include "bflb_cam.h"
#include "bflb_mjpeg.h"
#include "bflb_l1c.h"
#include "bflb_core.h"

#include "board.h"
#include "image_sensor.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "dvp_mjpeg_enc.h"
#include "jpeg_head.h"

#if !defined(BL616) && !defined(BL616CL)
#error "This example code is only for BL616 and BL616CL"
#endif

#if (0)
#define DVP_JPEG_DBG(a, ...) printf("[DVP_JPEG dbg]:" a, ##__VA_ARGS__)
#else
#define DVP_JPEG_DBG(a, ...)
#endif

#if (1)
#define DVP_JPEG_INFO(a, ...) printf("[DVP_JPEG info]:" a, ##__VA_ARGS__)
#else
#define DVP_JPEG_INFO(a, ...)
#endif

#define DVP_JPEG_ERR(a, ...)  printf("[DVP_JPEG err]:" a, ##__VA_ARGS__)

#define DVP_ISR_NOTIFY_INDEX  (0)
#define JPEG_ISR_NOTIFY_INDEX (1)

#define JPEG_FRAME_COMP       (2)
#define JPEG_OVER_SIZE        (3)

#if IS_ENABLED(CONFIG_PSRAM)
#define DVP_BUFFER_ATTR ATTR_NOINIT_PSRAM_SECTION __ALIGNED(32)
#else
#define DVP_BUFFER_ATTR __ALIGNED(32)
#endif

uint32_t g_dvp_mjpeg_enc_total_frame_cnt = 0;

DVP_BUFFER_ATTR uint8_t dvp_jpeg_line_buff[DVP_JPEG_LINE_BUFF_SIZE]; /*  */

/***** ctrl *****/
static TaskHandle_t dvp_mjpeg_enc_task_hd;

static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;
static struct bflb_device_s *mjpeg_enc_dev;

static struct bflb_cam_config_s cam_config;
static struct image_sensor_config_s *sensor_config;
static struct bflb_mjpeg_config_s mjpeg_enc_cfg;

/* dvp isr */
static void cam_isr(int irq, void *arg)
{
    /* TODO: can not stop dvp */
    bflb_cam_stop(cam0);

    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);

    DVP_JPEG_DBG("cam_isr\r\n");

    /* to task */
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveIndexedFromISR(dvp_mjpeg_enc_task_hd, DVP_ISR_NOTIFY_INDEX, &pxHigherPriorityTaskWoken);
    if (pxHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
}

/* mjpeg isr */
static void mjpeg_enc_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg_enc_dev);

    /* stop mjpeg */
    bflb_mjpeg_stop(mjpeg_enc_dev);

    DVP_JPEG_DBG("mjpeg_isr 0x%08X\r\n", intstatus);

    if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
        /* clear frame isr */
        bflb_mjpeg_int_clear(mjpeg_enc_dev, MJPEG_INTCLR_ONE_FRAME);
        /* clear over size isr */
        bflb_mjpeg_int_clear(mjpeg_enc_dev, 1 << 10);

        /* notify to task */
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyIndexedFromISR(dvp_mjpeg_enc_task_hd, JPEG_ISR_NOTIFY_INDEX, JPEG_FRAME_COMP, eSetValueWithOverwrite, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else if (intstatus & (1 << 6)) {
        /* over size error */
        /* clear over size isr */
        bflb_mjpeg_int_clear(mjpeg_enc_dev, 1 << 10);

        /* notify to task */
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyIndexedFromISR(dvp_mjpeg_enc_task_hd, JPEG_ISR_NOTIFY_INDEX, JPEG_OVER_SIZE, eSetValueWithOverwrite, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    }
}

/* dvp mjpeg enc init */
int dvp_mjpeg_enc_init(uint8_t quality)
{
    uint32_t jpg_head_buf[800 / 4] = { 0 };
    uint32_t jpg_head_len;

    DVP_JPEG_INFO("dvp_mjpeg_enc_init\r\n");

    /* dvp init */
    board_dvp_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    /* image_sensor init */
    if (image_sensor_scan(i2c0, &sensor_config)) {
        DVP_JPEG_INFO("Sensor name: %s\r\n", sensor_config->name);
    } else {
        DVP_JPEG_ERR("Error! Can't identify sensor!\r\n");
        return -1;
    }

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = true;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = (uint32_t)dvp_jpeg_line_buff;
    cam_config.output_bufsize = DVP_JPEG_ROW_NUM * cam_config.resolution_x * 2;

    DVP_JPEG_INFO("dvp x: %d\r\n", cam_config.resolution_x);
    DVP_JPEG_INFO("dvp y: %d\r\n", cam_config.resolution_y);

    if (cam_config.resolution_x > DVP_JPEG_W) {
        DVP_JPEG_ERR("dvp line_buff size over: dvp:%d, buff:%d\r\n", cam_config.resolution_x, DVP_JPEG_W);
        return -2;
    }

    bflb_cam_init(cam0, &cam_config);
    // bflb_cam_start(cam0);

    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, false);
    bflb_irq_attach(cam0->irq_num, cam_isr, NULL);
    bflb_irq_enable(cam0->irq_num);

    /* jpeg init */
    mjpeg_enc_dev = bflb_device_get_by_name("mjpeg");

    mjpeg_enc_cfg.format = MJPEG_FORMAT_YUV422_YUYV;
    mjpeg_enc_cfg.quality = quality;
    mjpeg_enc_cfg.rows = DVP_JPEG_ROW_NUM;
    mjpeg_enc_cfg.resolution_x = cam_config.resolution_x;
    mjpeg_enc_cfg.resolution_y = cam_config.resolution_y;
    mjpeg_enc_cfg.input_bufaddr0 = (uint32_t)dvp_jpeg_line_buff;
    mjpeg_enc_cfg.input_bufaddr1 = (uint32_t)NULL;
    mjpeg_enc_cfg.output_bufaddr = (uint32_t)NULL;
    mjpeg_enc_cfg.output_bufsize = 0;
    mjpeg_enc_cfg.input_yy_table = NULL; /* use default table */
    mjpeg_enc_cfg.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg_enc_dev, &mjpeg_enc_cfg);
    jpg_head_len = JpegHeadCreate(YUV_MODE_422, quality, cam_config.resolution_x, cam_config.resolution_y, (uint8_t *)jpg_head_buf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg_enc_dev, (uint8_t *)jpg_head_buf, jpg_head_len);

    /* TODO: */
    uint32_t reg_temp = getreg32(mjpeg_enc_dev->reg_base + 0x00);
    putreg32((reg_temp | (1 << 3)), (mjpeg_enc_dev->reg_base + 0x00)); /* enable over_size int */

    bflb_mjpeg_tcint_mask(mjpeg_enc_dev, false);
    reg_temp = getreg32(mjpeg_enc_dev->reg_base + 0x1C);
    putreg32((reg_temp | (1 << 2)), (mjpeg_enc_dev->reg_base + 0x1C)); /* enable over_size int */
    bflb_irq_attach(mjpeg_enc_dev->irq_num, mjpeg_enc_isr, NULL);
    bflb_irq_enable(mjpeg_enc_dev->irq_num);

    return 0;
}

/* mjpeg enc task with dvp line input */
static void dvp_mjpeg_enc_task(void *pvParameters)
{
    int ret;
    static jpeg_frame_t mjpeg_enc_frame;

    void *jpeg_dest;
    uint32_t jpeg_buff_size;

    uint8_t *jpeg_pic;
    uint32_t jpeg_len;

    vTaskDelay(100);

    /* clear */
    // ulTaskNotifyTake(pdTRUE, 0);

    while (1) {
        /* allco frame buff */
        ret = frame_queue_alloc_lock(g_mjpeg_out_frame_ctrl, (frame_elem_t *)&mjpeg_enc_frame, 1000);
        if (ret < 0) {
            DVP_JPEG_DBG("jpeg buff alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        /* get info */
        jpeg_dest = mjpeg_enc_frame.elem_base.frame_addr;
        jpeg_buff_size = mjpeg_enc_frame.elem_base.frame_size;

        DVP_JPEG_DBG("jpeg alloc: id %d, addr 0x%08X, size %d\r\n", mjpeg_enc_frame.elem_base.frame_id, jpeg_dest, jpeg_buff_size);

        /* start mjpeg enc */
        bflb_mjpeg_update_input_output_buff(mjpeg_enc_dev, dvp_jpeg_line_buff, NULL, jpeg_dest, jpeg_buff_size);
        // bflb_mjpeg_sw_run(mjpeg_enc_dev, 1);
        bflb_mjpeg_start(mjpeg_enc_dev);

        /* start dvp input */
        bflb_cam_start(cam0);

        /* waiting dvp isr */
        ulTaskNotifyTakeIndexed(DVP_ISR_NOTIFY_INDEX, pdTRUE, portMAX_DELAY);

        /* waiting jpeg isr */
        int mjpeg_enc_status = ulTaskNotifyTakeIndexed(JPEG_ISR_NOTIFY_INDEX, pdTRUE, portMAX_DELAY);

        if (mjpeg_enc_status == JPEG_FRAME_COMP) {
            /* save info */
            jpeg_len = bflb_mjpeg_get_frame_info(mjpeg_enc_dev, &jpeg_pic);
            bflb_mjpeg_pop_one_frame(mjpeg_enc_dev);

            /* check address */
            if ((void *)jpeg_pic != jpeg_dest) {
                DVP_JPEG_ERR("mjpeg output address error! 0x%08X -> 0x%08X \r\n", (uint32_t)jpeg_pic, (uint32_t)jpeg_dest);
                return;
            }

            DVP_JPEG_DBG("jpeg size %d\r\n", jpeg_len);

            bflb_l1c_dcache_invalidate_range(mjpeg_enc_frame.elem_base.frame_addr, mjpeg_enc_frame.elem_base.frame_size);

            if (jpeg_len <= jpeg_buff_size) {
                /* update frame len */
                mjpeg_enc_frame.data_size = jpeg_len;
                /* try send to out_queue */
                for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
                    ret = frame_queue_push(g_mjpeg_out_frame_ctrl, (frame_elem_t *)&mjpeg_enc_frame, i, 0);
                }
            } else {
                DVP_JPEG_ERR("got mjpeg frame ,but jpeg over size\r\n");
            }
        } else {
            if (mjpeg_enc_status == JPEG_OVER_SIZE) {
                DVP_JPEG_INFO("mjpeg over size, frame drop\r\n");
            } else {
                DVP_JPEG_ERR("mjpeg unkown error %d\r\n", mjpeg_enc_status);
            }
        }

        /* unlock jpeg */
        frame_queue_free_unlock(g_mjpeg_out_frame_ctrl, (frame_elem_t *)&mjpeg_enc_frame);

        g_dvp_mjpeg_enc_total_frame_cnt++;
    }
}

/* mjpeg enc task init */
int dvp_mjpeg_enc_task_init(uint8_t quality)
{
    int ret;

    DVP_JPEG_INFO("dvp_mjpeg_enc_task_init\r\n");

    ret = dvp_mjpeg_enc_init(quality);
    if (ret < 0) {
        return ret;
    }

    /* creat record  */
    xTaskCreate(dvp_mjpeg_enc_task, (char *)"dvp_mjpeg_enc_task", 384, NULL, 30, &dvp_mjpeg_enc_task_hd);

    return 0;
}
