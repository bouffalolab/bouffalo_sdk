#include "usbh_core.h"
#include "usbh_hid.h"

static volatile int hid_in_nbytes;
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t hid_buffer[128];

void usbh_hid_in_callback(void *arg, int nbytes)
{
    // struct usbh_hid *hid_class = (struct usbh_hid *)arg;

    hid_in_nbytes = nbytes;
}

static void usbh_hid_thread(CONFIG_USB_OSAL_THREAD_SET_ARGV)
{
    int ret;
    uint32_t input_cnt = 0;
    uint32_t hid_interval_ms;
    struct usbh_hid *hid_class = (struct usbh_hid *)CONFIG_USB_OSAL_THREAD_GET_ARGV;

    /* test with only one buffer, if you have more hid class, modify by yourself */
    static bool run_flag = false;
    if (run_flag == true) {
        USB_LOG_ERR("USBH HID test already running\r\n");
        goto exit_del;
    }
    run_flag = true;

    if (hid_class->hport->speed == USB_SPEED_HIGH) {
        /* HS */
        hid_interval_ms = (0x01 << (hid_class->intin->bInterval - 1)) * 125 / 1000;
    } else {
        /* LS/FS */
        hid_interval_ms = hid_class->intin->bInterval;
    }
    USB_LOG_RAW("USBH HID test start, hid_interval: %dms\r\n", hid_class->intin->bInterval);

    while (1) {
        /* Suggest you to use timer for int transfer and use ep interval */
        /* data in (sync, timeout:1000ms) */
        usbh_int_urb_fill(&hid_class->intin_urb, hid_class->hport, hid_class->intin, hid_buffer, hid_class->intin->wMaxPacketSize, 1000, usbh_hid_in_callback, hid_class);
        ret = usbh_submit_urb(&hid_class->intin_urb);
        if (ret < 0) {
            if (ret == -USB_ERR_TIMEOUT) {
                continue;
            } else {
                USB_LOG_ERR("hid submit urb error,ret:%d\r\n", ret);
                goto exit_del;
            }
        }
        input_cnt++;

        if (hid_in_nbytes < 0) {
            USB_LOG_ERR("hid in error,ret:%d\r\n", hid_in_nbytes);
            continue;
        } else if (hid_in_nbytes == 0) {
            USB_LOG_ERR("hid in no data\r\n");
            continue;
        }

        USB_LOG_RAW("HID input[%d] %dByte: ", input_cnt, hid_in_nbytes);
        for (int i = 0; i < hid_in_nbytes; i++) {
            USB_LOG_RAW("0x%02x ", hid_buffer[i]);
        }
        USB_LOG_RAW("\r\n");

        /* wait for next interval */
        usb_osal_msleep(hid_interval_ms);
    }

exit_del:
    run_flag = false;
    USB_LOG_RAW("USBH HID test end\r\n");
    usb_osal_thread_delete(NULL);
}

void usbh_hid_run(struct usbh_hid *hid_class)
{
    usb_osal_thread_create("usbh_hid", 2048, CONFIG_USBHOST_PSC_PRIO - 1, usbh_hid_thread, hid_class);
}

void usbh_hid_stop(struct usbh_hid *hid_class)
{
}