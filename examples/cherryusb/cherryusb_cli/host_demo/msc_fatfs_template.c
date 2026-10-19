/**
 * @file msc_fatfs_template.c
 * @brief CherryUSB host MSC and FatFS throughput and integrity test.
 */

#include "bflb_mtimer.h"

#include "usbh_core.h"
#include "usbh_msc.h"

#include "ff.h"
#include "fatfs_diskio_register.h"

/** @brief Size in bytes of each FatFS test transfer. */
#define USBH_FATFS_TEST_BUF_SIZE (16 * 1024)

/** @brief Lifecycle states for the single-volume MSC FatFS demo. */
enum usbh_msc_state {
    USBH_MSC_STOPPED = 0,
    USBH_MSC_STARTING,
    USBH_MSC_RUNNING,
    USBH_MSC_STOPPING,
};

/** @brief Shared state for the MSC FatFS worker and disconnect path. */
struct usbh_msc_context {
    struct usbh_msc *msc_class;
    enum usbh_msc_state state;
    usb_osal_mutex_t state_mutex;
    usb_osal_sem_t stopped_sem;
    bool stop_waiting;
};

static struct usbh_msc_context g_msc_ctx = {
    .state = USBH_MSC_STOPPED,
};

FATFS fs;
FIL fnew;

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

/** @brief Data buffer used for file reads and writes. */
BYTE RW_Buffer[USBH_FATFS_TEST_BUF_SIZE] = { 0 };
/** @brief Reference data used to verify file contents. */
BYTE Check_Buffer[sizeof(RW_Buffer)] = { 0 };

/** @brief Lazily create synchronization objects for the MSC worker. */
static void usbh_msc_context_init(void)
{
    if (g_msc_ctx.state_mutex == NULL) {
        g_msc_ctx.state_mutex = usb_osal_mutex_create();
        g_msc_ctx.stopped_sem = usb_osal_sem_create(0);
    }
}

/** @brief Read the MSC lifecycle state under the state mutex. */
static enum usbh_msc_state usbh_msc_get_state(void)
{
    enum usbh_msc_state state;

    usb_osal_mutex_take(g_msc_ctx.state_mutex);
    state = g_msc_ctx.state;
    usb_osal_mutex_give(g_msc_ctx.state_mutex);
    return state;
}

/**
 * @brief Measure FatFS write/read throughput and verify the resulting file.
 * @note The MSC volume must already be mounted at `/usb`.
 */
static void fatfs_write_read_test(void)
{
    FRESULT ret;
    UINT fnum;

    uint32_t time_node, i, j;

    /* full test data to buff */
    for (uint32_t cnt = 0; cnt < (sizeof(RW_Buffer) / sizeof(test_data)); cnt++) {
        memcpy(&RW_Buffer[cnt * sizeof(test_data)], test_data, sizeof(test_data));
        memcpy(&Check_Buffer[cnt * sizeof(test_data)], test_data, sizeof(test_data));
    }

    /* write test */
    USB_LOG_RAW("\r\n");
    USB_LOG_INFO("******************** be about to write test... **********************\r\n");
    ret = f_open(&fnew, "/usb/test_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (ret == FR_OK) {
        time_node = (uint32_t)bflb_mtimer_get_time_ms();
        /*write into file*/
        // ret = f_write(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < 1024; i++) {
            ret = f_write(&fnew, RW_Buffer, sizeof(RW_Buffer), &fnum);
            if (ret != FR_OK || fnum != sizeof(RW_Buffer)) {
                if (ret == FR_OK) {
                    ret = FR_DISK_ERR;
                }
                break;
            }
        }

        /* close file */
        ret |= f_close(&fnew);
        /* get time */
        time_node = (uint32_t)bflb_mtimer_get_time_ms() - time_node;

        if (ret == FR_OK) {
            USB_LOG_INFO("Write Test Succeed! \r\n");
            USB_LOG_INFO("Single data size:%d Byte, Write the number:%d, Total size:%d KB\r\n", sizeof(RW_Buffer), i, sizeof(RW_Buffer) * i >> 10);
            if (time_node > 0) {
                USB_LOG_INFO("Time:%dms, Write Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
            }
        } else {
            USB_LOG_ERR("Fail to write files(%d) num:%d\n", ret, i);
            return;
        }
    } else {
        USB_LOG_ERR("Fail to open or create files: %d.\r\n", ret);
        return;
    }

    /* read test */
    USB_LOG_RAW("\r\n");
    USB_LOG_INFO("******************** be about to read test... **********************\r\n");
    ret = f_open(&fnew, "/usb/test_file.txt", FA_OPEN_EXISTING | FA_READ);
    if (ret == FR_OK) {
        time_node = (uint32_t)bflb_mtimer_get_time_ms();

        // ret = f_read(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < 1024; i++) {
            ret = f_read(&fnew, RW_Buffer, sizeof(RW_Buffer), &fnum);
            if (ret != FR_OK || fnum != sizeof(RW_Buffer)) {
                if (ret == FR_OK) {
                    ret = FR_DISK_ERR;
                }
                break;
            }
        }
        /* close file */
        ret |= f_close(&fnew);
        /* get time */
        time_node = (uint32_t)bflb_mtimer_get_time_ms() - time_node;

        if (ret == FR_OK) {
            USB_LOG_INFO("Read Test Succeed! \r\n");
            USB_LOG_INFO("Single data size:%dByte, Read the number:%d, Total size:%d KB\r\n", sizeof(RW_Buffer), i, sizeof(RW_Buffer) * i >> 10);
            if (time_node > 0) {
                USB_LOG_INFO("Time:%dms, Read Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
            }
        } else {
            USB_LOG_ERR("Fail to read file: (%d), num:%d\n", ret, i);
            return;
        }
    } else {
        USB_LOG_ERR("Fail to open files.\r\n");
        return;
    }

    /* check data */
    USB_LOG_RAW("\r\n");
    USB_LOG_INFO("******************** be about to check test... **********************\r\n");
    ret = f_open(&fnew, "/usb/test_file.txt", FA_OPEN_EXISTING | FA_READ);
    if (ret == FR_OK) {
        // ret = f_read(&fnew, RW_Buffer, 1024, &fnum);
        for (i = 0; i < 1024; i++) {
            memset(RW_Buffer, 0x55, sizeof(RW_Buffer));
            ret = f_read(&fnew, RW_Buffer, sizeof(RW_Buffer), &fnum);
            if (ret != FR_OK || fnum != sizeof(RW_Buffer)) {
                if (ret == FR_OK) {
                    ret = FR_DISK_ERR;
                }
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
                USB_LOG_INFO("Check Test Error! \r\n");
                USB_LOG_INFO("Data Error!  Num:%d/1024, Byte:%d/%d", i, j, sizeof(RW_Buffer));
                return;
            } else {
                USB_LOG_INFO("Check Test Succeed! \r\n");
                USB_LOG_INFO("All Data Is Good! \r\n");
            }

        } else {
            USB_LOG_ERR("Fail to read file: (%d), num:%d\n", ret, i);
            return;
        }
    } else {
        USB_LOG_ERR("Fail to open files.\r\n");
        return;
    }
}

/**
 * @brief Initialize an MSC device, mount it, and run the FatFS test.
 * @note The MSC class instance is supplied through the USB OSAL thread
 *       argument macro. The thread unmounts the volume and deletes itself.
 */
static void usbh_msc_thread(CONFIG_USB_OSAL_THREAD_SET_ARGV)
{
    struct usbh_msc_context *ctx = (struct usbh_msc_context *)CONFIG_USB_OSAL_THREAD_GET_ARGV;
    struct usbh_msc *msc_class = ctx->msc_class;
    bool driver_registered = false;
    bool mounted = false;
    bool stop_waiting;
    int ret;

    if (usbh_msc_get_state() != USBH_MSC_STARTING) {
        goto exit;
    }

    ret = usbh_msc_scsi_init(msc_class);
    if (ret < 0) {
        USB_LOG_RAW("scsi_init error,ret:%d\r\n", ret);
        goto exit;
    }

    if (msc_class->blocksize != FF_MIN_SS || FF_MIN_SS != FF_MAX_SS) {
        USB_LOG_ERR("Unsupported MSC block size:%u, FatFS sector size:%u\r\n",
                    msc_class->blocksize, FF_MIN_SS);
        goto exit;
    }

#if 1
    /* read test: get the partition table */
    ret = usbh_msc_scsi_read10(msc_class, 0, RW_Buffer, 1);
    if (ret < 0) {
        USB_LOG_RAW("scsi_read10 error,ret:%d\r\n", ret);
        goto exit;
    }

    // for (uint32_t i = 0; i < 512; i++) {
    //     if (i % 16 == 0) {
    //         USB_LOG_RAW("\r\n");
    //     }
    //     USB_LOG_RAW("%02x ", RW_Buffer[i]);
    // }
    // USB_LOG_RAW("\r\n");
#endif

    fatfs_usbh_driver_register(msc_class);
    driver_registered = true;

    ret = f_mount(&fs, "/usb", 1);
    if (FR_OK != ret) {
        USB_LOG_RAW("mount fail, res: %d\r\n", ret);
        goto exit;
    }
    mounted = true;

    usb_osal_mutex_take(ctx->state_mutex);
    if (ctx->state == USBH_MSC_STARTING) {
        ctx->state = USBH_MSC_RUNNING;
    }
    usb_osal_mutex_give(ctx->state_mutex);

    if (usbh_msc_get_state() == USBH_MSC_RUNNING) {
        fatfs_write_read_test();
    }

exit:
    usb_osal_mutex_take(ctx->state_mutex);
    if (ctx->state == USBH_MSC_STARTING || ctx->state == USBH_MSC_RUNNING) {
        ctx->state = USBH_MSC_STOPPING;
    }
    usb_osal_mutex_give(ctx->state_mutex);

    if (mounted) {
        f_mount(NULL, "/usb", 1);
    }
    if (driver_registered) {
        fatfs_usbh_driver_unregister(msc_class);
    }

    usb_osal_mutex_take(ctx->state_mutex);
    ctx->msc_class = NULL;
    stop_waiting = ctx->stop_waiting;
    if (stop_waiting == false) {
        ctx->state = USBH_MSC_STOPPED;
    }
    usb_osal_mutex_give(ctx->state_mutex);

    USB_LOG_RAW("USBH MSC FatFS speed test end\r\n");
    if (stop_waiting) {
        usb_osal_sem_give(ctx->stopped_sem);
    }
    usb_osal_thread_delete(NULL);
}

/**
 * @brief Start the MSC FatFS test thread.
 * @param[in] msc_class MSC host class instance to test.
 */
void usbh_msc_run(struct usbh_msc *msc_class)
{
    usbh_msc_context_init();

    usb_osal_mutex_take(g_msc_ctx.state_mutex);
    if (g_msc_ctx.state != USBH_MSC_STOPPED) {
        usb_osal_mutex_give(g_msc_ctx.state_mutex);
        USB_LOG_ERR("USBH MSC FatFS test already running\r\n");
        return;
    }

    usb_osal_sem_reset(g_msc_ctx.stopped_sem);
    g_msc_ctx.msc_class = msc_class;
    g_msc_ctx.stop_waiting = false;
    g_msc_ctx.state = USBH_MSC_STARTING;
    usb_osal_mutex_give(g_msc_ctx.state_mutex);

    usb_osal_thread_create("usbh_msc", 2048, CONFIG_USBHOST_PSC_PRIO - 1,
                           usbh_msc_thread, &g_msc_ctx);
}

/**
 * @brief Handle a request to stop the MSC FatFS test.
 * @param[in] msc_class MSC host class instance being disconnected.
 * @note This function blocks until FatFS is unmounted and the class reference is released.
 */
void usbh_msc_stop(struct usbh_msc *msc_class)
{
    if (g_msc_ctx.state_mutex == NULL) {
        return;
    }

    usb_osal_mutex_take(g_msc_ctx.state_mutex);
    if (g_msc_ctx.state == USBH_MSC_STOPPED ||
        g_msc_ctx.msc_class != msc_class) {
        usb_osal_mutex_give(g_msc_ctx.state_mutex);
        return;
    }

    g_msc_ctx.stop_waiting = true;
    g_msc_ctx.state = USBH_MSC_STOPPING;
    usb_osal_mutex_give(g_msc_ctx.state_mutex);

    usb_osal_sem_take(g_msc_ctx.stopped_sem, USB_OSAL_WAITING_FOREVER);

    usb_osal_mutex_take(g_msc_ctx.state_mutex);
    g_msc_ctx.stop_waiting = false;
    g_msc_ctx.state = USBH_MSC_STOPPED;
    usb_osal_mutex_give(g_msc_ctx.state_mutex);
}
