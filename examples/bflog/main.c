#include "bflb_mtimer.h"
#include "board.h"
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

static bflog_direct_file_time_t bflog_direct_filetime;
static bflog_direct_file_size_t bflog_direct_filesize;

void bflog_restart(void)
{
    void *record = (void *)&__bflog_recorder;

    void *direct0 = (void *)&bflog_direct_filetime;
    void *direct1 = (void *)&bflog_direct_filesize;

    bflog_direct_create(direct0, BFLOG_DIRECT_TYPE_FILE_TIME, BFLOG_DIRECT_COLOR_DISABLE);
    bflog_direct_init_file_time(direct0, "sd:/log/common", 60, 42, NULL, NULL);

    bflog_direct_create(direct1, BFLOG_DIRECT_TYPE_FILE_SIZE, BFLOG_DIRECT_COLOR_DISABLE);
    bflog_direct_init_file_size(direct1, "sd:/log/error", 128 * 1024, 16, NULL, NULL);
    bflog_direct_control(direct1, BFLOG_DIRECT_CMD_LEVEL, BFLOG_LEVEL_WARN);

    /*!< suspend record */
    bflog_suspend_s(record);

    bflog_append_s(record, direct0);
    bflog_append_s(record, direct1);

    bflog_direct_resume_s(direct0);
    bflog_direct_resume_s(direct1);

    /*!< resume record */
    bflog_resume_s(record);
}

int main(void)
{
    board_init();

    filesystem_init();

    bflog_restart();

    while (1) {
        LOG_F("hello world\r\n");
        LOG_E("hello world\r\n");
        LOG_W("hello world\r\n");
        LOG_I("hello world\r\n");
        LOG_D("hello world\r\n");
        LOG_T("hello world\r\n");
        bflb_mtimer_delay_ms(200);
    }
}
