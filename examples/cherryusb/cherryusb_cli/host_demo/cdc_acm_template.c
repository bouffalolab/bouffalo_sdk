#include "bflb_mtimer.h"

#include "usbh_core.h"
#include "usbh_cdc_acm.h"

#include "ring_buffer.h"

#define USBH_ACM_IN_BUFF_SIZE   (4 * 1024)
#define USBH_ACM_OUT_BUFF_SIZE  (2 * 1024)

#define USBH_CDC_ACM_TEST_TIME  (2 * 1000) // 2s

static volatile uint32_t acm_in_size = 0;
static volatile uint32_t acm_out_size = 0;
static volatile bool acm_out_busy_flag = false;
static volatile bool acm_in_busy_flag = false;

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[USBH_ACM_IN_BUFF_SIZE];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[USBH_ACM_OUT_BUFF_SIZE];

static usb_osal_sem_t usbh_cdc_acm_sem = NULL;

void usbh_cdc_acm_out_callback(void *arg, int nbytes)
{
    // struct usbh_cdc_acm *cdc_acm_class = (struct usbh_cdc_acm *)arg;
    // USB_LOG_RAW("acm_out nbytes:%d\r\n", nbytes);
    if (nbytes < 0) {
        USB_LOG_ERR("USBH bulk out error,ret:%d\r\n", nbytes);
        return;
    }
    acm_out_size += nbytes;
    acm_out_busy_flag = false;

    usb_osal_sem_give(usbh_cdc_acm_sem);
}

void usbh_cdc_acm_in_callback(void *arg, int nbytes)
{
    // struct usbh_cdc_acm *cdc_acm_class = (struct usbh_cdc_acm *)arg;
    // USB_LOG_RAW("acm_in nbytes:%d\r\n", nbytes);
    if (nbytes < 0) {
        USB_LOG_ERR("USBH bulk in error,ret:%d\r\n", nbytes);
        return;
    }
    acm_in_size += nbytes;
    acm_in_busy_flag = false;

    usb_osal_sem_give(usbh_cdc_acm_sem);
}

static void usbh_cdc_acm_thread(CONFIG_USB_OSAL_THREAD_SET_ARGV)
{
    int ret;
    uint32_t time_ms;
    struct usbh_cdc_acm *cdc_acm_class = (struct usbh_cdc_acm *)CONFIG_USB_OSAL_THREAD_GET_ARGV;

    /* test with only one buffer, if you have more cdc acm class, modify by yourself */
    static bool run_flag = false;
    if (run_flag == true) {
        USB_LOG_ERR("USBH ACM test already running\r\n");
        usb_osal_thread_delete(NULL);
        return;
    }
    run_flag = true;

    USB_LOG_RAW("USBH ACM speed test start\r\n");

    usbh_cdc_acm_sem = usb_osal_sem_create(0);
    usb_osal_msleep(10);

    /* set data rate and bits */
    struct cdc_line_coding line_coding = {
        .dwDTERate = 2000000,
        .bDataBits = 8,
        .bParityType = 0,
        .bCharFormat = 0,
    };
    ret = usbh_cdc_acm_set_line_coding(cdc_acm_class, &line_coding);
    if (ret < 0) {
        USB_LOG_ERR("set line_coding error,ret:%d\r\n", ret);
    }
    /* set dtr/rts  */
    ret = usbh_cdc_acm_set_line_state(cdc_acm_class, true, false);
    if (ret < 0) {
        USB_LOG_ERR("set dtr/rts error,ret:%d\r\n", ret);
    }
    usb_osal_msleep(10);

    acm_in_size = 0;
    acm_out_size = 0;
    acm_out_busy_flag = false;
    acm_in_busy_flag = false;
    time_ms = bflb_mtimer_get_time_ms();

    while ((bflb_mtimer_get_time_ms() - time_ms) < USBH_CDC_ACM_TEST_TIME) {
        /* data out (async) */
        if (acm_out_busy_flag == false) {
            acm_out_busy_flag = true;
            usbh_bulk_urb_fill(&cdc_acm_class->bulkout_urb, cdc_acm_class->hport, cdc_acm_class->bulkout, write_buffer,
                               sizeof(write_buffer), 0, usbh_cdc_acm_out_callback, cdc_acm_class);
            ret = usbh_submit_urb(&cdc_acm_class->bulkout_urb);
            if (ret < 0) {
                USB_LOG_RAW("bulk out error,ret:%d\r\n", ret);
                goto exit_del;
            }
        }
        /* data in (async) */
        if (acm_in_busy_flag == false) {
            acm_in_busy_flag = true;
            usbh_bulk_urb_fill(&cdc_acm_class->bulkin_urb, cdc_acm_class->hport, cdc_acm_class->bulkin, read_buffer,
                               sizeof(read_buffer), 0, usbh_cdc_acm_in_callback, cdc_acm_class);
            ret = usbh_submit_urb(&cdc_acm_class->bulkin_urb);
            if (ret < 0) {
                USB_LOG_RAW("bulk in error,ret:%d\r\n", ret);
                goto exit_del;
            }
        }

        usb_osal_sem_take(usbh_cdc_acm_sem, 10);
    }
    time_ms = bflb_mtimer_get_time_ms() - time_ms;

    /* set dtr/rts  */
    usb_osal_msleep(5);
    ret = usbh_cdc_acm_set_line_state(cdc_acm_class, false, false);
    if (ret < 0) {
        USB_LOG_ERR("set dtr/rts error,ret:%d\r\n", ret);
    }

    USB_LOG_RAW("USBH ACM test time: %d\r\n", time_ms);
    USB_LOG_RAW("USBH ACM out size: %d\r\n", acm_out_size);
    USB_LOG_RAW("USBH ACM in size: %d\r\n", acm_in_size);
    USB_LOG_RAW("USBH ACM out speed: %lld KB/S\r\n", (uint64_t)acm_out_size * 1000 / 1024 / time_ms);
    USB_LOG_RAW("USBH ACM in speed: %lld KB/S\r\n", (uint64_t)acm_in_size * 1000 / 1024 / time_ms);
    USB_LOG_RAW("USBH ACM total speed: %lld KB/S\r\n", (uint64_t)(acm_out_size + acm_in_size) / 1024 * 1000 / time_ms);

exit_del:
    usb_osal_sem_delete(usbh_cdc_acm_sem);
    usbh_cdc_acm_sem = NULL;
    run_flag = false;
    USB_LOG_WRN("USBH ACM speed test end\r\n");
    usb_osal_thread_delete(NULL);
}

void usbh_cdc_acm_run(struct usbh_cdc_acm *cdc_acm_class)
{
    usb_osal_thread_create("usbh_cdc", 2048, CONFIG_USBHOST_PSC_PRIO - 1, usbh_cdc_acm_thread, cdc_acm_class);
}

void usbh_cdc_acm_stop(struct usbh_cdc_acm *cdc_acm_class)
{
}
