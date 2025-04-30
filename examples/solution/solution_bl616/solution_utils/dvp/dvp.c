#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "bl616_glb.h"
#include "bflb_l1c.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "board.h"
#include "image_sensor.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "dvp.h"

#if (0)
#define DVP_DBG(a, ...) printf("[DVP dbg]:" a, ##__VA_ARGS__)
#else
#define DVP_DBG(a, ...)
#endif

#if (1)
#define DVP_INFO(a, ...) printf("[DVP info]:" a, ##__VA_ARGS__)
#else
#define DVP_INFO(a, ...)
#endif

#define DVP_ERR(a, ...) printf("[DVP err]:" a, ##__VA_ARGS__)

uint32_t g_dvp_total_frame_cnt = 0;

/* cam ctrl */
static TaskHandle_t dvp_task_handle;

static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;

static struct bflb_cam_config_s cam_config;
static struct image_sensor_config_s *sensor_config;

ATTR_PSRAM_CODE_SECTION static void cam_isr(int irq, void *arg)
{
    /* dvp cam stop */
    bflb_cam_stop(cam0);

    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);

    DVP_DBG("cam_isr\r\n");

    /* to task */
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(dvp_task_handle, &pxHigherPriorityTaskWoken);
    if (pxHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
}

static void dvp_start_task(void *pvParameters)
{
    int ret;
    pyuyv_frame_t pbuff_frame;

    vTaskDelay(2);

    /* clear */
    // ulTaskNotifyTake(pdTRUE, 0);

    while (1) {
        /* alloc new frame buff */
        ret = frame_queue_alloc_lock(g_yuyv_frame_ctrl, (frame_elem_t *)&pbuff_frame, 1000);
        if (ret < 0) {
            DVP_INFO("yuyv alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        DVP_DBG("yuyv alloc: id %d, addr 0x%08X, size %d\r\n", pbuff_frame.elem_base.frame_id, pbuff_frame.elem_base.frame_addr, pbuff_frame.elem_base.frame_size);

        cam_config.output_bufaddr = (uint32_t)pbuff_frame.elem_base.frame_addr;
        cam_config.output_bufsize = pbuff_frame.elem_base.frame_size;

        /* reinitialize */
        bflb_cam_init(cam0, &cam_config);

        /* dvp cam start */
        bflb_cam_start(cam0);

        /* waiting dvp isr */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        DVP_DBG("dvp pop frame\r\n");

        uint8_t *buff_using;
        bflb_cam_get_frame_info(cam0, &buff_using);
        bflb_cam_pop_one_frame(cam0);

        // GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_D2XA);
        // GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_D2XB);

        /* check addr */
        if ((void *)buff_using != pbuff_frame.elem_base.frame_addr) {
            DVP_ERR("dvp addr err: 0x%08X->0x%08X\r\n", (uint32_t)pbuff_frame.elem_base.frame_addr, (uint32_t)buff_using);
        }

        /*  */
        pbuff_frame.x_start = 0;
        pbuff_frame.y_start = 0;
        pbuff_frame.x_end = cam_config.resolution_x - 1;
        pbuff_frame.y_end = cam_config.resolution_y - 1;

        /* push all */
        for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
            ret = frame_queue_push(g_yuyv_frame_ctrl, (frame_elem_t *)&pbuff_frame, i, 0);
        }

        /* unlock */
        frame_queue_free_unlock(g_yuyv_frame_ctrl, (frame_elem_t *)&pbuff_frame);

        /* update dvp frame */
        g_dvp_total_frame_cnt++;
    }

    /* Delete task */
    vTaskDelete(NULL);
}

int dvp_cam_task_init(void)
{
    DVP_INFO("dvp_init\r\n");

    board_dvp_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    /* image_sensor init */
    if (image_sensor_scan(i2c0, &sensor_config)) {
        DVP_INFO("Sensor name: %s\r\n", sensor_config->name);
    } else {
        DVP_ERR("Error! Can't identify sensor!\r\n");
        return -1;
    }

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = (uint32_t)NULL;
    cam_config.output_bufsize = 0;

    DVP_INFO("dvp x: %d\r\n", cam_config.resolution_x);
    DVP_INFO("dvp y: %d\r\n", cam_config.resolution_y);

    uint32_t dvp_size = cam_config.resolution_x * cam_config.resolution_y * 2;
    if (dvp_size > YUYV_FRAME_SIZE) {
        DVP_ERR("dvp size over: dvp:%d, buff:%d\r\n", dvp_size, YUYV_FRAME_SIZE);
        return -2;
    }

    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, false);
    bflb_irq_attach(cam0->irq_num, cam_isr, NULL);
    bflb_irq_enable(cam0->irq_num);

    /* camer task */
    xTaskCreate(dvp_start_task, (char *)"dvp_start_task", 1024, NULL, 20, &dvp_task_handle);

    return 0;
}