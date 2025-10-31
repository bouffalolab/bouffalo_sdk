#include <string.h>
#include <stdlib.h>

#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#include <FreeRTOS.h>
#include <task.h>

#include "board.h"

#include "fatfs_diskio_register.h"
#include "ff.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "jpeg_sd.h"

FATFS g_fatfs_fs = { 0 };

#if (1)
#define SD_JPEG_INFO(a, ...) printf("[SD_JPEG info]:" a, ##__VA_ARGS__)
#else
#define SD_JPEG_INFO(a, ...)
#endif
#define SD_JPEG_ERR(a, ...) printf("[SD_JPEG err]:" a, ##__VA_ARGS__)

#if 0
static uint16_t jpeg_out_queue_jpeg_id;
static TaskHandle_t save_jpeg;
#endif

/* recursively deletes all the contents of the folder */
int filedir_delete(char *path)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    TCHAR file_path[FF_MAX_LFN + 2] = { 0 };

    res = f_opendir(&dir, (const TCHAR *)path);

    /* Traversal folder */
    while ((res == FR_OK) && (FR_OK == f_readdir(&dir, &fno))) {
        if (0 == strlen(fno.fname)) {
            /* End of folder traversal */
            break;
        }

        if (0 == strcmp(fno.fname, ".") || 0 == strcmp(fno.fname, "..")) {
            /* Special symbol */
            continue;
        }

        memset(file_path, 0, sizeof(file_path));

#if _USE_LFN
        sprintf((char *)file_path, "%s/%s", path, (*fno.altname) ? fno.altname : fno.fname);
#else
        sprintf((char *)file_path, "%s/%s", path, fno.fname);
#endif

        if (fno.fattrib & AM_DIR) {
            /* Delete folders recursively */
            res = filedir_delete(file_path);
        } else {
            /* Delete file */
            res = f_unlink(file_path);
        }
    }

    /* Delete oneself */
    if (res == FR_OK) {
        res = f_unlink((const TCHAR *)path);
        return res;
    }

    return res;
}

#if 0
static void save_jpeg_to_sdcard(void *pvParameters)
{
    jpeg_frame_t jpeg_frame;
    uint32_t save_cnt = 0, error_cnt = 0;
    FRESULT ret;
    UINT fnum;

    static FIL fnew = { 0 };
    char str_buff[64];

    if (fatfs_sd_init() < 0) {
        SD_JPEG_ERR("sd_card init failed\r\n");
        vTaskSuspend(NULL);
    }

    SD_JPEG_INFO("rm -rf /sd/jpeg_test ...");
    /* delete dir */
    filedir_delete("/sd/jpeg_test");
    SD_JPEG_INFO("rm OK!\r\n");

    /* create dir */
    f_mkdir("/sd/jpeg_test");

    /* create output queue */
    jpeg_out_queue_jpeg_id = MJPEG_FRAME_STREAM_SD_JPEG_ID;
    if (frame_queue_output_create(g_jpeg_frame_ctrl, &jpeg_out_queue_jpeg_id, MJPEG_FRAME_STREAM_SD_JPEG_DEPTH) < 0) {
        SD_JPEG_ERR("jpeg frame sd_jpeg out queue create failed\r\n");
        vTaskSuspend(NULL);
    } else {
        SD_JPEG_INFO("jpeg frame sd_avi out queue ID: %d\r\n", jpeg_out_queue_jpeg_id);
    }

    while (1) {
        /* pop frame */
        if (frame_queue_output_pop(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame, jpeg_out_queue_jpeg_id, 1000) < 0) {
            SD_JPEG_INFO("jpeg pop timeout, continue wait...\r\n");
            continue;
        }

        save_cnt++;

        /* invalid dcache */
        bflb_l1c_dcache_invalidate_range(jpeg_frame.elem_base.frame_addr, jpeg_frame.data_size + 31);

        ret = f_open(&fnew, str_buff, FA_CREATE_NEW | FA_WRITE);

        if (ret == FR_OK) {
            ret = f_write(&fnew, jpeg_frame.elem_base.frame_addr, jpeg_frame.data_size, &fnum);

            if (ret != FR_OK) {
                SD_JPEG_ERR("fatfs write failed, cnt %d, code %d\r\n", save_cnt, ret);
                error_cnt++;
            }

            f_close(&fnew);

        } else {
            SD_JPEG_ERR("fatfs open failed, cnt %d, code %d\r\n", save_cnt, ret);
            error_cnt++;
        }

        /* free frame */
        frame_queue_output_free(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame);

        if (save_cnt % 50 == 0) {
            SD_JPEG_INFO("sd cnt:%d, err_cnt:%d\r\n", save_cnt, error_cnt);
        }

        vTaskDelay(2);
    }
}
#endif

int fatfs_sd_init(void)
{
    FRESULT ret;
    static int sd_ready = 0;

    if (sd_ready) {
        return 0;
    }

    SD_JPEG_INFO("fatfs_sd_init\r\n");

    /* init sdcard and fatfs */
    board_sdh_gpio_init();
    fatfs_sdh_driver_register();

    /* mount filesystem */
    ret = f_mount(&g_fatfs_fs, "/sd", 1);

    if (ret == FR_OK) {
        SD_JPEG_INFO("succeed to mount filesystem\r\n");
        SD_JPEG_INFO("fileSystem cluster size:%d-sectors (%d-Byte)\r\n", g_fatfs_fs.csize, g_fatfs_fs.csize * 512);

    } else if (ret == FR_NO_FILESYSTEM) {
        SD_JPEG_ERR("sdcard No filesystem or do not support!!! Please format the sd card\r\n");
        return -1;

    } else {
        SD_JPEG_ERR("fail to mount filesystem, error= %d\r\n", ret);
        SD_JPEG_ERR("sdcard might fail to initialise.\r\n");
        return -2;
    }

    sd_ready = 1;

    return 0;
}

int save_jpeg_to_sdcard_init(void)
{
    SD_JPEG_INFO("save_jpeg_to_sdcard_init\r\n");
#if 0
    xTaskCreate(save_jpeg_to_sdcard, (char *)"save_jpeg_to_sdcard", 1024, NULL, 15, &save_jpeg);
#endif
    return 0;
}
