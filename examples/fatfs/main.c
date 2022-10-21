#include "bflb_mtimer.h"
#include "board.h"
#include "vlibc_stdio.h"
#include "ff.h"
#include "log.h"

FATFS fs;
__attribute((aligned(8))) static uint32_t workbuf[4096];

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

    extern void fatfs_sdh_driver_register(void);
    fatfs_sdh_driver_register();

    ret = f_mount(&fs, "sd:/", 1);

    if (ret == FR_NO_FILESYSTEM) {
        LOG_W("No filesystem yet, try to be formatted...\r\n");

        ret = f_mkfs("sd:/", &fs_para, workbuf, sizeof(workbuf));

        if (ret != FR_OK) {
            LOG_F("fail to make filesystem\r\n");
            _CALL_ERROR();
        }

        if (ret == FR_OK) {
            LOG_I("done with formatting.\r\n");
            LOG_I("first start to unmount.\r\n");
            ret = f_mount(NULL, "sd:/", 1);
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

int main(void)
{
    board_init();

    filesystem_init();

    vlibc_file_t *fp = vlibc_fopen("sd:/test.txt", "w");

    if (fp == NULL) {
        LOG_I("open file error\r\n");
    } else {
        LOG_I("open file success\r\n");
        for (uint8_t i = 0; i < 10; i++) {
            vlibc_fprintf(fp, "hello world\r\n");
        }

        vlibc_fclose(fp);

        LOG_I("close file success\r\n");
    }

    while (1) {
        bflb_mtimer_delay_ms(200);
    }
}
