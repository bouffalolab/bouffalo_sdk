#include "bflb_mtimer.h"

#include "usbh_core.h"
#include "usbh_msc.h"

#include "ff.h"
#include "fatfs_diskio_register.h"

#define USBH_FATFS_TEST_BUF_SIZE (16 * 1024)

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

BYTE RW_Buffer[USBH_FATFS_TEST_BUF_SIZE] = { 0 };
BYTE Check_Buffer[sizeof(RW_Buffer)] = { 0 };

void fatfs_write_read_test()
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
            if (ret) {
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
            USB_LOG_INFO("Time:%dms, Write Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
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
            if (ret) {
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
            USB_LOG_INFO("Time:%dms, Read Speed:%d KB/s \r\n", time_node, ((sizeof(RW_Buffer) * i) >> 10) * 1000 / time_node);
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
                USB_LOG_INFO("Check Test Error! \r\n");
                USB_LOG_INFO("Data Error!  Num:%d/1024, Byte:%d/%d", i, j, sizeof(RW_Buffer));
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

static void usbh_msc_thread(CONFIG_USB_OSAL_THREAD_SET_ARGV)
{
    int ret;
    struct usbh_msc *msc_class = (struct usbh_msc *)CONFIG_USB_OSAL_THREAD_GET_ARGV;

    /* test with only one buffer, if you have more msc class, modify by yourself */
    static bool run_flag = false;
    if (run_flag == true) {
        USB_LOG_ERR("USBH ACM test already running\r\n");
        goto exit_del;
    }
    run_flag = true;

    ret = usbh_msc_scsi_init(msc_class);
    if (ret < 0) {
        USB_LOG_RAW("scsi_init error,ret:%d\r\n", ret);
        goto exit_del;
    }

#if 1
    /* read test: get the partition table */
    ret = usbh_msc_scsi_read10(msc_class, 0, RW_Buffer, 1);
    if (ret < 0) {
        USB_LOG_RAW("scsi_read10 error,ret:%d\r\n", ret);
        goto exit_del;
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

    ret = f_mount(&fs, "/usb", 1);
    if (FR_OK != ret) {
        USB_LOG_RAW("mount fail, res: %d\r\n", ret);
        goto exit_del;
    }

    fatfs_write_read_test();

    f_mount(NULL, "/usb", 1);

exit_del:
    run_flag = false;
    USB_LOG_RAW("USBH MSC FatFS speed test end\r\n");
    usb_osal_thread_delete(NULL);
}

void usbh_msc_run(struct usbh_msc *msc_class)
{
    usb_osal_thread_create("usbh_msc", 2048, CONFIG_USBHOST_PSC_PRIO - 1, usbh_msc_thread, msc_class);
}

void usbh_msc_stop(struct usbh_msc *msc_class)
{
}
