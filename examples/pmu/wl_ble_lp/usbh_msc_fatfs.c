#include "bflb_mtimer.h"

#include "usbh_core.h"
#include "usbh_msc.h"

#include "ff.h"
#include "fatfs_diskio_register.h"

#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"
#include "bl616_glb.h"

FATFS fs;
FIL fnew;

const char test_data[512] =
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

USB_NOCACHE_RAM_SECTION BYTE Write_Buffer[sizeof(test_data)*10] = { 0 };
USB_NOCACHE_RAM_SECTION BYTE Read_Buffer[sizeof(Write_Buffer)] = { 0 };

void fatfs_write_read_test()
{
    FRESULT ret;
    UINT fnum;

    uint32_t time_node, i, j;

    /* full test data to buff */
    for (uint32_t cnt = 0; cnt < (sizeof(Write_Buffer) / sizeof(test_data)); cnt++) {
        memcpy(&Write_Buffer[cnt * sizeof(test_data)], test_data, sizeof(test_data));
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
            ret = f_write(&fnew, Write_Buffer, sizeof(Write_Buffer), &fnum);
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
            USB_LOG_INFO("Single data size:%d Byte, Write the number:%d, Total size:%d KB\r\n", sizeof(Write_Buffer), i, sizeof(Write_Buffer) * i >> 10);
            USB_LOG_INFO("Time:%dms, Write Speed:%d KB/s \r\n", time_node, ((sizeof(Write_Buffer) * i) >> 10) * 1000 / time_node);
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
            ret = f_read(&fnew, Read_Buffer, sizeof(Read_Buffer), &fnum);
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
            USB_LOG_INFO("Single data size:%dByte, Read the number:%d, Total size:%d KB\r\n", sizeof(Read_Buffer), i, sizeof(Read_Buffer) * i >> 10);
            USB_LOG_INFO("Time:%dms, Read Speed:%d KB/s \r\n", time_node, ((sizeof(Read_Buffer) * i) >> 10) * 1000 / time_node);
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
            memset(Read_Buffer, 0x55, sizeof(Read_Buffer));
            ret = f_read(&fnew, Read_Buffer, sizeof(Read_Buffer), &fnum);
            if (ret) {
                break;
            }
            for (j = 0; j < sizeof(Read_Buffer); j++) {
                if (Read_Buffer[j] != Write_Buffer[j]) {
                    break;
                }
            }
            if (j < sizeof(Read_Buffer)) {
                break;
            }
        }
        /* close file */
        ret |= f_close(&fnew);

        if (ret == FR_OK) {
            if (i < 1024 || j < sizeof(Read_Buffer)) {
                USB_LOG_INFO("Check Test Error! \r\n");
                USB_LOG_INFO("Data Error!  Num:%d/1024, Byte:%d/%d", i, j, sizeof(Read_Buffer));
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

static usb_osal_thread_t usbh_msc_handle = NULL;

static void usbh_msc_thread(CONFIG_USB_OSAL_THREAD_SET_ARGV)
{
    int ret;
    struct usbh_msc *msc_class = (struct usbh_msc *)CONFIG_USB_OSAL_THREAD_GET_ARGV;

    ret = usbh_msc_scsi_init(msc_class);
    if (ret < 0) {
        USB_LOG_RAW("scsi_init error,ret:%d\r\n", ret);
        goto exit_del;
    }

    fatfs_usbh_driver_register(msc_class);

    uint32_t timeout = 0;
    uint32_t time_start = 0;
    while (1) {
        xTaskNotifyWait(0, 0, &timeout, portMAX_DELAY);

        USB_LOG_RAW("\r\n");
        USB_LOG_INFO("Will run usbh msc test for %lu seconds\r\n", timeout);

        time_start = (uint32_t)bflb_mtimer_get_time_ms();
        while ((uint32_t)bflb_mtimer_get_time_ms() - time_start < timeout * 1000) {
            ret = f_mount(&fs, "/usb", 1);
            if (FR_OK != ret) {
                USB_LOG_RAW("mount fail, res: %d\r\n", ret);
                goto exit_del;
            }

            fatfs_write_read_test();

            f_mount(NULL, "/usb", 1);
        }

        USB_LOG_RAW("\r\n");
        USB_LOG_INFO("Test done\r\n");
    }

exit_del:
    USB_LOG_RAW("USBH MSC FatFS speed test end\r\n");
    usb_osal_thread_delete(NULL);
    usbh_msc_handle = NULL;
}

void usbh_msc_run(struct usbh_msc *msc_class)
{
    //printf("\r\n%s\r\n\r\n", __func__);
    usbh_msc_handle = usb_osal_thread_create("usbh_msc", 2048, CONFIG_USBHOST_PSC_PRIO - 1, usbh_msc_thread, msc_class);
}

void usbh_msc_stop(struct usbh_msc *msc_class)
{
    //printf("\r\n%s\r\n\r\n", __func__);
    if (usbh_msc_handle) {
        usb_osal_thread_delete(usbh_msc_handle);
        usbh_msc_handle = NULL;
    }
}

int usbh_init(int argc, char **argv)
{
    *(volatile uint32_t *)(GLB_BASE + GLB_CGEN_CFG1_OFFSET) |= 1U << 13;
    *(volatile uint32_t *)(GLB_BASE + GLB_CGEN_CFG2_OFFSET) |= 1U << 19;

    GLB_Set_USB_CLK_From_WIFIPLL(1);

    usbh_initialize(0, USB_BASE);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(usbh_init, usbh_init, usbh init.);

int usbh_msc_test(int argc, char **argv)
{
    uint32_t value = 10;

    if (argc > 1) {
        sscanf(argv[1], "%lu", &value);
    }

    if (usbh_msc_handle) {
        xTaskNotify(usbh_msc_handle, value, eSetValueWithOverwrite);
    } else {
        USB_LOG_ERR("Please plug in usb msc device and initialize usb host\r\n");
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(usbh_msc_test, usbh_msc_test, usbh msc test.);
