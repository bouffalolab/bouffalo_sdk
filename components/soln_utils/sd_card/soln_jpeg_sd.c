#include <string.h>
#include <stdlib.h>

#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#include <FreeRTOS.h>
#include <task.h>

#include "board.h"

#include "fatfs_diskio_register.h"
#include "ff.h"

#include "soln_jpeg_sd.h"

#ifdef CONFIG_SOLN_SD_JPEG_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_SD_JPEG_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_SD_JPEG"
#include "log.h"

FATFS g_fatfs_fs = { 0 };

/* recursively deletes all the contents of the folder */
int soln_filedir_delete(char *path)
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

#if FF_USE_LFN
        sprintf((char *)file_path, "%s/%s", path, (*fno.altname) ? fno.altname : fno.fname);
#else
        sprintf((char *)file_path, "%s/%s", path, fno.fname);
#endif

        if (fno.fattrib & AM_DIR) {
            /* Delete folders recursively */
            res = soln_filedir_delete(file_path);
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

int soln_fatfs_sd_init(void)
{
    FRESULT ret;
    static int sd_ready = 0;

    if (sd_ready) {
        return 0;
    }

    LOG_I("soln_fatfs_sd_init\r\n");

    /* init sdcard and fatfs */
    board_sdh_gpio_init();
    fatfs_sdh_driver_register();

    /* mount filesystem */
    ret = f_mount(&g_fatfs_fs, "/sd", 1);

    if (ret == FR_OK) {
        LOG_I("succeed to mount filesystem\r\n");
        LOG_I("fileSystem cluster size:%d-sectors (%d-Byte)\r\n", g_fatfs_fs.csize, g_fatfs_fs.csize * 512);

    } else if (ret == FR_NO_FILESYSTEM) {
        LOG_E("sdcard No filesystem or do not support!!! Please format the sd card\r\n");
        return -1;

    } else {
        LOG_E("fail to mount filesystem, error= %d\r\n", ret);
        LOG_E("sdcard might fail to initialise.\r\n");
        return -2;
    }

    sd_ready = 1;

    return 0;
}

int soln_save_jpeg_to_sdcard_init(void)
{
    LOG_I("soln_save_jpeg_to_sdcard_init\r\n");
#if 0
    xTaskCreate(save_jpeg_to_sdcard, (char *)"save_jpeg_to_sdcard", 1024, NULL, 15, &save_jpeg);
#endif
    return 0;
}
