#include "bflb_mtimer.h"
#include "bflb_irq.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "bflb_l1c.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "board.h"
#include "image_sensor.h"

#include "soln_fbq.h"

#include "soln_dvp.h"

#ifdef CONFIG_SOLN_VID_DVP_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_VID_DVP_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_DVP"
#include "log.h"

static uint32_t s_dvp_total_frame_count = 0;

uint32_t soln_dvp_get_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    uint32_t frame_count = s_dvp_total_frame_count;
    bflb_irq_restore(irq_flags);

    return frame_count;
}

void soln_dvp_clear_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_dvp_total_frame_count = 0;
    bflb_irq_restore(irq_flags);
}

static void dvp_record_frame(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_dvp_total_frame_count++;
    bflb_irq_restore(irq_flags);
}

/* cam ctrl */
static TaskHandle_t dvp_task_handle;

static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;

static struct bflb_cam_config_s cam_config;
static struct image_sensor_config_s *sensor_config;

static void cam_isr(int irq, void *arg)
{
    /* dvp cam stop */
    bflb_cam_stop(cam0);

    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);

    LOG_D("cam_isr\r\n");

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
    size_t frame_size;
    fbq_elem_t *pbuff_frame;

    frame_size = cam_config.resolution_x * cam_config.resolution_y * 2;

    vTaskDelay(10);

    while (1) {
        /* alloc new frame buff */
        ret = fbq_alloc(soln_fbq_vid_raw_local(), &pbuff_frame, 1000);
        if (ret != FBQ_OK) {
            LOG_W("local RAW alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        if (pbuff_frame->capacity < frame_size) {
            LOG_E("local RAW alloc size error: %d, need %d\r\n", pbuff_frame->capacity, frame_size);
            fbq_free(pbuff_frame);
            continue;
        }
        LOG_D("local RAW alloc: id %d, addr 0x%08X, size %d\r\n", pbuff_frame->id, pbuff_frame->data, pbuff_frame->capacity);

        bflb_l1c_dcache_invalidate_range(pbuff_frame->data, frame_size);

        cam_config.output_bufaddr = (uint32_t)pbuff_frame->data;
        cam_config.output_bufsize = pbuff_frame->capacity;

        /* reinitialize */
        bflb_cam_init(cam0, &cam_config);

        /* dvp cam start */
        bflb_cam_start(cam0);

        /* waiting dvp isr */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        LOG_D("dvp pop frame\r\n");

        uint8_t *buff_using;
        bflb_cam_get_frame_info(cam0, &buff_using);
        bflb_cam_pop_one_frame(cam0);

        // GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_D2XA);
        // GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_D2XB);
        /* check addr */
        // if ((void *)buff_using != pbuff_frame->data) {
        //     LOG_E("dvp addr err: 0x%08X->0x%08X\r\n", (uint32_t)pbuff_frame->data, (uint32_t)buff_using);
        // }

        bflb_l1c_dcache_invalidate_range(pbuff_frame->data, frame_size);

        soln_fbq_img_raw_ext_t *raw_ext = soln_fbq_img_raw_ext(pbuff_frame);
        raw_ext->x_start = 0;
        raw_ext->y_start = 0;
        raw_ext->x_end = cam_config.resolution_x - 1;
        raw_ext->y_end = cam_config.resolution_y - 1;
        raw_ext->format = IMG_RAW_FRAME_FORMAT_YUYV;
        pbuff_frame->size = frame_size;
        pbuff_frame->type_mask = SOLUTION_FBQ_TYPE_MASK_IMG_RAW_CAM;

        fbq_push_mask(soln_fbq_vid_raw_local(), pbuff_frame, FBQ_OUTPUT_ALL, 0);
        fbq_free(pbuff_frame);

        /* update dvp frame */
        dvp_record_frame();
    }

    /* Delete task */
    vTaskDelete(NULL);
}

int soln_dvp_cam_task_init(void)
{
    LOG_I("dvp_init\r\n");

    board_dvp_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    /* image_sensor init */
    if (image_sensor_scan(i2c0, &sensor_config)) {
        LOG_I("Sensor name: %s\r\n", sensor_config->name);
    } else {
        LOG_E("Error! Can't identify sensor!\r\n");
        return -1;
    }

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = (uint32_t)NULL;
    cam_config.output_bufsize = 0;

    LOG_I("dvp x: %d\r\n", cam_config.resolution_x);
    LOG_I("dvp y: %d\r\n", cam_config.resolution_y);

    uint32_t dvp_size = cam_config.resolution_x * cam_config.resolution_y * 2;
    if (dvp_size > CONFIG_SOLN_FBQ_VID_RAW_LOCAL_SIZE) {
        LOG_E("dvp size over: dvp:%d, buff:%d\r\n", dvp_size, CONFIG_SOLN_FBQ_VID_RAW_LOCAL_SIZE);
        return -2;
    }

    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, false);
    bflb_irq_attach(cam0->irq_num, cam_isr, NULL);
    bflb_irq_enable(cam0->irq_num);

    /* camer task */
    xTaskCreate(dvp_start_task, (char *)"dvp_task", 384, NULL, 30, &dvp_task_handle);

    return 0;
}
