#include "bflb_mtimer.h"
#include "bflb_irq.h"
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

#include "soln_fbq.h"

#include "soln_dvp_mjpeg_enc.h"
#include "soln_jpeg_head.h"

#ifdef CONFIG_SOLN_VID_DVP_JPEG_ENC_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_VID_DVP_JPEG_ENC_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_DVP_MJPEG"
#include "log.h"

#if !defined(BL616) && !defined(BL616CL)
#error "This example code is only for BL616 and BL616CL"
#endif

#define DVP_ISR_NOTIFY_INDEX  (0)
#define JPEG_ISR_NOTIFY_INDEX (1)

#define JPEG_FRAME_COMP       (2)
#define JPEG_OVER_SIZE        (3)

#if IS_ENABLED(CONFIG_PSRAM)
#define DVP_BUFFER_ATTR ATTR_NOINIT_PSRAM_SECTION __ALIGNED(32)
#else
#define DVP_BUFFER_ATTR __ALIGNED(32)
#endif

static uint32_t s_dvp_mjpeg_enc_total_frame_count = 0;

uint32_t soln_dvp_mjpeg_enc_get_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    uint32_t frame_count = s_dvp_mjpeg_enc_total_frame_count;
    bflb_irq_restore(irq_flags);

    return frame_count;
}

void soln_dvp_mjpeg_enc_clear_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_dvp_mjpeg_enc_total_frame_count = 0;
    bflb_irq_restore(irq_flags);
}

static void dvp_mjpeg_enc_record_frame(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_dvp_mjpeg_enc_total_frame_count++;
    bflb_irq_restore(irq_flags);
}

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

    LOG_D("cam_isr\r\n");

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

    LOG_D("mjpeg_isr 0x%08X\r\n", intstatus);

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
static int dvp_mjpeg_enc_init(uint8_t quality)
{
    uint32_t jpg_head_buf[800 / 4] = { 0 };
    uint32_t jpg_head_len;

    LOG_I("dvp_mjpeg_enc_init\r\n");

    /* dvp init */
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
    cam_config.with_mjpeg = true;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = (uint32_t)dvp_jpeg_line_buff;
    cam_config.output_bufsize = DVP_JPEG_ROW_NUM * cam_config.resolution_x * 2;

    LOG_I("dvp x: %d\r\n", cam_config.resolution_x);
    LOG_I("dvp y: %d\r\n", cam_config.resolution_y);

    if (cam_config.resolution_x > DVP_JPEG_W) {
        LOG_E("dvp line_buff size over: dvp:%d, buff:%d\r\n", cam_config.resolution_x, DVP_JPEG_W);
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
    jpg_head_len = soln_jpeg_head_create(YUV_MODE_422, quality, cam_config.resolution_x, cam_config.resolution_y, (uint8_t *)jpg_head_buf);
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
    fbq_elem_t *mjpeg_enc_frame;

    void *jpeg_dest;
    uint32_t jpeg_buff_size;

    uint8_t *jpeg_pic;
    uint32_t jpeg_len;

    vTaskDelay(100);

    /* clear */
    // ulTaskNotifyTake(pdTRUE, 0);

    while (1) {
        /* allco frame buff */
        ret = fbq_alloc(soln_fbq_vid_jpeg_local(), &mjpeg_enc_frame, 1000);
        if (ret != FBQ_OK) {
            LOG_D("jpeg buff alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }
        mjpeg_enc_frame->type_mask = SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_JPEG_ENC;

        /* get info */
        jpeg_dest = mjpeg_enc_frame->data;
        jpeg_buff_size = mjpeg_enc_frame->capacity;

        LOG_D("jpeg alloc: id %d, addr 0x%08X, size %d\r\n", mjpeg_enc_frame->id, jpeg_dest, jpeg_buff_size);

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
                LOG_E("mjpeg output address error! 0x%08X -> 0x%08X \r\n", (uint32_t)jpeg_pic, (uint32_t)jpeg_dest);
            } else if (jpeg_len <= jpeg_buff_size) {
                LOG_D("jpeg size %d\r\n", jpeg_len);
                bflb_l1c_dcache_invalidate_range(mjpeg_enc_frame->data, jpeg_len);
                mjpeg_enc_frame->size = jpeg_len;
                (void)fbq_push_mask(soln_fbq_vid_jpeg_local(), mjpeg_enc_frame, FBQ_OUTPUT_ALL, 0);
            } else {
                LOG_E("got mjpeg frame, but jpeg over size\r\n");
            }
        } else {
            if (mjpeg_enc_status == JPEG_OVER_SIZE) {
                LOG_W("mjpeg over size, frame drop\r\n");
            } else {
                LOG_E("mjpeg unknown error %d\r\n", mjpeg_enc_status);
            }
        }

        fbq_free(mjpeg_enc_frame);

        dvp_mjpeg_enc_record_frame();
    }
}

/* mjpeg enc task init */
int soln_dvp_mjpeg_enc_task_init(uint8_t quality)
{
    int ret;

    LOG_I("soln_dvp_mjpeg_enc_task_init\r\n");

    ret = dvp_mjpeg_enc_init(quality);
    if (ret < 0) {
        return ret;
    }

    /* creat record  */
    xTaskCreate(dvp_mjpeg_enc_task, (char *)"dvp_mjpeg_enc_task", 384, NULL, 30, &dvp_mjpeg_enc_task_hd);

    return 0;
}
