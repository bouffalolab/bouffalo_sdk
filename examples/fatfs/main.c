#include "bflb_mtimer.h"
#include "board.h"
#include "fatfs_diskio_register.h"
#include "ff.h"

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
#include <FreeRTOS.h>
#include "semphr.h"
#endif

#define DBG_TAG "MAIN"
#include "log.h"

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
static TaskHandle_t test_handle;
#endif

FATFS fs;
__attribute((aligned(64))) static uint32_t workbuf[4096];

MKFS_PARM fs_para = {
    .fmt = FM_FAT32,     /* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
    .n_fat = 1,          /* Number of FATs */
    .align = 0,          /* Data area alignment (sector) */
    .n_root = 1,         /* Number of root directory entries */
    .au_size = 512 * 32, /* Cluster size (byte) */
};

void filesystem_init(void)
{
    FRESULT ret;

    board_sdh_gpio_init();

    fatfs_sdh_driver_register();

    ret = f_mount(&fs, "/sd", 1);
    // ret = FR_NO_FILESYSTEM;

    if (ret == FR_NO_FILESYSTEM) {
        LOG_W("No filesystem yet, try to be formatted...\r\n");

        ret = f_mkfs("/sd", &fs_para, workbuf, sizeof(workbuf));

        if (ret != FR_OK) {
            LOG_F("fail to make filesystem %d\r\n", ret);
            _CALL_ERROR();
        }

        if (ret == FR_OK) {
            LOG_I("done with formatting.\r\n");
            LOG_I("first start to unmount.\r\n");
            ret = f_mount(NULL, "/sd", 1);
            LOG_I("then start to remount.\r\n");
        }
    } else if (ret != FR_OK) {
        LOG_F("fail to mount filesystem,error= %d\r\n", ret);
        LOG_F("SD card might fail to initialise.\r\n");
        _CALL_ERROR();
    } else {
        LOG_D("Succeed to mount filesystem\r\n");
    }

    if (ret == FR_OK) {
        LOG_I("FileSystem cluster size:%d-sectors (%d-Byte)\r\n", fs.csize, fs.csize * 512);
    }
}

#define SDU_DATA_CHECK 1

char test_data[] =
    "I've been reading books of old \r\n\
    The legends and the myths \r\n\
    Achilles and his gold \r\n\
    Hercules and his gifts \r\n\
    Spiderman's control \r\n\
    And Batman with his fists\r\n\
    And clearly I don't see myself upon that list\r\n\
    But she said, where'd you wanna go?\r\n\
    How much you wanna risk?\r\n\
    I'm not looking for somebody\r\n\
    With some superhuman gifts\r\n\
    Some superhero\r\n\
    Some fairytale bliss\r\n\
    Just something I can turn to\r\n\
    Somebody I can kiss\r\n\
    I want something just like this\r\n\r\n";

__attribute((aligned(64))) BYTE RW_Buffer[32 * 1024] = { 0 };
#if SDU_DATA_CHECK
__attribute((aligned(64))) BYTE Check_Buffer[sizeof(RW_Buffer)] = { 0 };
#endif

void fatfs_write_read_test()
{
    FRESULT ret;
    FIL fnew;
    UINT fnum;

    uint32_t time_node, i, j;

    /* full test data to buff */
    for (uint32_t cnt = 0; cnt < (sizeof(RW_Buffer) / sizeof(test_data)); cnt++) {
        memcpy(&RW_Buffer[cnt * sizeof(test_data)], test_data, sizeof(test_data));
#if SDU_DATA_CHECK
        memcpy(&Check_Buffer[cnt * sizeof(test_data)], test_data, sizeof(test_data));
#endif
    }

    /* write test */
    LOG_RI("\r\n");
    LOG_I("******************** be about to write test... **********************\r\n");
    ret = f_open(&fnew, "/sd/test_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (ret == FR_OK) {
        time_node = (uint32_t)bflb_mtimer_get_time_ms();
        /*write into file*/
        // ret = f_write(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < 1024; i++) {
            ret = f_write(&fnew, RW_Buffer, sizeof(RW_Buffer), &fnum);
            if (ret) {
                break;
            }
        }

        /* close file */
        ret |= f_close(&fnew);
        /* get time */
        time_node = (uint32_t)bflb_mtimer_get_time_ms() - time_node;

        if (ret == FR_OK) {
            LOG_I("Write Test Succeed! \r\n");
            LOG_I("Single data size:%d Byte, Write the number:%d, Total size:%d KB\r\n", sizeof(RW_Buffer), i, sizeof(RW_Buffer) * i >> 10);
            LOG_I("Time:%dms, Write Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
        } else {
            LOG_F("Fail to write files(%d) num:%d\n", ret, i);
            return;
        }
    } else {
        LOG_F("Fail to open or create files: %d.\r\n", ret);
        return;
    }

    /* read test */
    LOG_RI("\r\n");
    LOG_I("******************** be about to read test... **********************\r\n");
    ret = f_open(&fnew, "/sd/test_file.txt", FA_OPEN_EXISTING | FA_READ);
    if (ret == FR_OK) {
        time_node = (uint32_t)bflb_mtimer_get_time_ms();

        // ret = f_read(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < 1024; i++) {
            ret = f_read(&fnew, RW_Buffer, sizeof(RW_Buffer), &fnum);
            if (ret) {
                break;
            }
        }
        /* close file */
        ret |= f_close(&fnew);
        /* get time */
        time_node = (uint32_t)bflb_mtimer_get_time_ms() - time_node;

        if (ret == FR_OK) {
            LOG_I("Read Test Succeed! \r\n");
            LOG_I("Single data size:%dByte, Read the number:%d, Total size:%d KB\r\n", sizeof(RW_Buffer), i, sizeof(RW_Buffer) * i >> 10);
            LOG_I("Time:%dms, Read Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
        } else {
            LOG_F("Fail to read file: (%d), num:%d\n", ret, i);
            return;
        }
    } else {
        LOG_F("Fail to open files.\r\n");
        return;
    }

    /* check data */
#if SDU_DATA_CHECK
    LOG_RI("\r\n");
    LOG_I("******************** be about to check test... **********************\r\n");
    ret = f_open(&fnew, "/sd/test_file.txt", FA_OPEN_EXISTING | FA_READ);
    if (ret == FR_OK) {
        // ret = f_read(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < 1024; i++) {
            memset(RW_Buffer, 0x55, sizeof(RW_Buffer));
            ret = f_read(&fnew, RW_Buffer, sizeof(RW_Buffer), &fnum);
            if (ret) {
                break;
            }
            for (j = 0; j < sizeof(RW_Buffer); j++) {
                if (RW_Buffer[j] != Check_Buffer[j]) {
                    break;
                }
            }
            if (j < sizeof(RW_Buffer)) {
                break;
            }
        }
        /* close file */
        ret |= f_close(&fnew);

        if (ret == FR_OK) {
            if (i < 1024 || j < sizeof(RW_Buffer)) {
                LOG_I("Check Test Error! \r\n");
                LOG_I("Data Error!  Num:%d/1024, Byte:%d/%d", i, j, sizeof(RW_Buffer));
            } else {
                LOG_I("Check Test Succeed! \r\n");
                LOG_I("All Data Is Good! \r\n");
            }

        } else {
            LOG_F("Fail to read file: (%d), num:%d\n", ret, i);
            return;
        }
    } else {
        LOG_F("Fail to open files.\r\n");
        return;
    }
#endif
}

void fatfs_test_main(void *param)
{
    (void)param;

    filesystem_init();

    fatfs_write_read_test();

#if defined(CONFIG_NEWLIB) && CONFIG_NEWLIB && defined(CONFIG_NEWLIB_FATFS) && CONFIG_NEWLIB_FATFS
    FILE *fp;
    fp = fopen("/sd/hellotest.txt", "w+");
    fprintf(fp, "hello world\r\n");
    fclose(fp);
#endif

    while (1) {
        bflb_mtimer_delay_ms(200);
    }
}

int main(void)
{
    board_init();

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
    xTaskCreate(fatfs_test_main, (char *)"test_task", 1024, NULL, configMAX_PRIORITIES - 2, &test_handle);
    vTaskStartScheduler();
#else
    fatfs_test_main(NULL);
#endif
}
