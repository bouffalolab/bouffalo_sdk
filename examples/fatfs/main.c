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

#define TEST_DATA_SIZE (1024 * 32)
#define TEST_RW_CNT    (1000)
#define TEST_CYCLE_NUM (5)

FATFS fs;
__attribute((aligned(64))) static uint32_t workbuf[FF_MAX_SS * 4];

MKFS_PARM fs_para = {
    .fmt = FM_EXFAT,     /* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
    .n_fat = 0,          /* Number of FATs */
    .align = 0,          /* Data area alignment (sector) */
    .n_root = 0,         /* Number of root directory entries */
    .au_size = 512 * 32, /* Cluster size (byte) */
};

int filesystem_init(void)
{
    FRESULT ret;

    board_sdh_gpio_init();

    fatfs_sdh_driver_register();

    ret = f_mount(&fs, "/sd", 1);

    if (ret == FR_NO_FILESYSTEM) {
        LOG_W("No filesystem yet, try to be formatted...\r\n");

        ret = f_mkfs("/sd", &fs_para, workbuf, sizeof(workbuf));
        if (ret != FR_OK) {
            LOG_E("fail to make filesystem %d\r\n", ret);
            return -1;
        }

        LOG_W("The formatting was successful. Try mount again\r\n");

        ret = f_mount(&fs, "/sd", 1);
    }

    if (ret == FR_OK) {
        LOG_D("Succeed to mount filesystem\r\n");
        LOG_I("FileSystem type: %s\r\n", fs.fs_type == 1 ? "FAT12" :
                                         fs.fs_type == 2 ? "FAT16" :
                                         fs.fs_type == 3 ? "FAT32" :
                                         fs.fs_type == 4 ? "exFAT" :
                                                           "unknown");
        LOG_I("   Volume  size: %d MByte\r\n", (uint32_t)((uint64_t)(fs.n_fatent - 2) * fs.csize * 512 / 1024 / 1024));
        LOG_I("   Cluster size: %d-sectors (%d-Byte)\r\n", fs.csize, fs.csize * 512);

    } else {
        LOG_E("fail to mount filesystem,error= %d\r\n", ret);
        LOG_E("SD card might fail to initialise.\r\n");
        return -1;
    }

    return 0;
}

#define SDU_DATA_CHECK 1

char test_data[512] =
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

__attribute((aligned(64))) BYTE RW_Buffer[TEST_DATA_SIZE] = { 0 };
#if SDU_DATA_CHECK
__attribute((aligned(64))) BYTE Check_Buffer[TEST_DATA_SIZE] = { 0 };
#endif

int fatfs_write_read_test()
{
    FRESULT ret;
    FIL fnew;
    UINT fnum;

    uint32_t time_node, i, j;

    /* fill test data to buff */
    for (uint32_t i = 0; i < TEST_DATA_SIZE; i++) {
        RW_Buffer[i] = test_data[i % sizeof(test_data)];
        Check_Buffer[i] = test_data[i % sizeof(test_data)];
    }

    /* write test */
    LOG_RI("\r\n");
    LOG_I("******************** be about to write test... **********************\r\n");
    ret = f_open(&fnew, "/sd/test_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (ret == FR_OK) {
        time_node = (uint32_t)bflb_mtimer_get_time_ms();
        /*write into file*/
        // ret = f_write(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < TEST_RW_CNT; i++) {
            ret = f_write(&fnew, RW_Buffer, TEST_DATA_SIZE, &fnum);
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
            LOG_I("Single data size:%d Byte, Write the number:%d, Total size:%d KB\r\n", TEST_DATA_SIZE, i, TEST_DATA_SIZE * i / 1024);
            LOG_I("Time:%dms, Write Speed:%d KB/s \r\n", time_node, (TEST_DATA_SIZE * i / 1024) * 1000 / time_node);
        } else {
            LOG_E("Fail to write files(%d) num:%d\n", ret, i);
            return -1;
        }
    } else {
        LOG_E("Fail to open or create files: %d.\r\n", ret);
        return -1;
    }

    /* read test */
    LOG_RI("\r\n");
    LOG_I("******************** be about to read test... **********************\r\n");
    ret = f_open(&fnew, "/sd/test_file.txt", FA_OPEN_EXISTING | FA_READ);
    if (ret == FR_OK) {
        time_node = (uint32_t)bflb_mtimer_get_time_ms();

        // ret = f_read(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < TEST_RW_CNT; i++) {
            ret = f_read(&fnew, RW_Buffer, TEST_DATA_SIZE, &fnum);
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
            LOG_I("Single data size:%d Byte, Read the number:%d, Total size:%d KB\r\n", TEST_DATA_SIZE, i, TEST_DATA_SIZE * i / 1024);
            LOG_I("Time:%dms, Read Speed:%d KB/s \r\n", time_node, (TEST_DATA_SIZE * i / 1024) * 1000 / time_node);
        } else {
            LOG_E("Fail to read file: (%d), num:%d\n", ret, i);
            return -1;
        }
    } else {
        LOG_E("Fail to open files.\r\n");
        return -1;
    }

    /* check data */
#if SDU_DATA_CHECK
    LOG_RI("\r\n");
    LOG_I("******************** be about to check test... **********************\r\n");
    ret = f_open(&fnew, "/sd/test_file.txt", FA_OPEN_EXISTING | FA_READ);
    if (ret == FR_OK) {
        // ret = f_read(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < TEST_RW_CNT; i++) {
            memset(RW_Buffer, 0x55, TEST_DATA_SIZE);
            ret = f_read(&fnew, RW_Buffer, TEST_DATA_SIZE, &fnum);
            if (ret) {
                break;
            }
            for (j = 0; j < TEST_DATA_SIZE; j++) {
                if (RW_Buffer[j] != Check_Buffer[j]) {
                    break;
                }
            }
            if (j < TEST_DATA_SIZE) {
                break;
            }
        }
        /* close file */
        ret |= f_close(&fnew);

        if (ret == FR_OK) {
            if (i < TEST_RW_CNT || j < TEST_DATA_SIZE) {
                LOG_I("Check Test Error! \r\n");
                LOG_I("Data Error!  Num:%d/%d, Byte:%d/%d, 0x%X(%c)->0x%X(%c) \r\n",
                      i, TEST_RW_CNT, j, TEST_DATA_SIZE, Check_Buffer[j], Check_Buffer[j], RW_Buffer[j], RW_Buffer[j]);

                uint32_t n = j & (~511);
                DBG_VALUE(n);
                LOG_I("Error data offset: 0x%04X\r\n", j - n);

                char *check_data = (char *)&Check_Buffer[n];
                DBG_HEXDUMP(check_data, 1024);

                char *read_data = (char *)&RW_Buffer[n];
                DBG_HEXDUMP(read_data, 1024);

                return -1;
            } else {
                LOG_I("Check Test Succeed! \r\n");
                LOG_I("All Data Is Good! \r\n\r\n");
            }

        } else {
            LOG_E("Fail to read file: (%d), num:%d\n", ret, i);
            return -1;
        }
    } else {
        LOG_E("Fail to open files.\r\n");
        return -1;
    }
#endif

    return 0;
}

void fatfs_test_main(void *param)
{
    (void)param;

    for (int test_cnt = 1; test_cnt <= TEST_CYCLE_NUM; test_cnt++) {
        LOG_W("test_cnt: %d\r\n", test_cnt);

        if (filesystem_init() < 0) {
            break;
        }
        if (fatfs_write_read_test() < 0) {
            break;
        }

#if defined(CONFIG_NEWLIB) && CONFIG_NEWLIB && defined(CONFIG_NEWLIB_FATFS) && CONFIG_NEWLIB_FATFS
        FILE *fp;
        fp = fopen("/sd/hellotest.txt", "w+");
        fprintf(fp, "hello world\r\n");
        fclose(fp);
#endif

        f_unmount("/sd");

        bflb_mtimer_delay_ms(1000);
    }

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
