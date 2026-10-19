/**
 * @file usbd_cli.c
 * @brief Shell commands for selecting and controlling CherryUSB device demos.
 */

#include "bflb_mtimer.h"
#include "board.h"

#include "usbd_core.h"
#include "usbd_hid.h"

#include "FreeRTOS.h"
#include "task.h"

#include "shell.h"

#define DBG_TAG "USBD_CLI"
#include "log.h"

/** @brief Stack depth used for shell-managed USB device tasks. */
#define USBD_TASK_STACK_SIZE 1024

/** @brief Callback used to deinitialize the active USB device demo. */
typedef void (*shell_usbd_stop_cb_t)(void);

#ifdef CONFIG_CHERRYUSB_HOST
extern volatile bool usbh_run_flag;
#endif
volatile bool usbd_run_flag = false;

static TaskHandle_t usbd_cli_handle = NULL;
static shell_usbd_stop_cb_t shell_usbd_stop_cb = NULL;

/* check if usb device task is running */
/**
 * @brief Claim device mode and optionally create a demo task.
 * @param[in] pxTaskCode FreeRTOS task entry, or NULL for taskless demos.
 * @param[in] pcName Task name when `pxTaskCode` is non-NULL.
 * @param[in] stop_cb Demo-specific deinitialization callback.
 * @retval 0 Device mode was claimed and the optional task was created.
 * @retval -1 Another USB device or host demo is already active.
 */
int usbd_test_run(TaskFunction_t pxTaskCode, const char *pcName, shell_usbd_stop_cb_t stop_cb)
{
    if (usbd_run_flag) {
        LOG_W("usb device already running\r\n");
        LOG_W("please stop it first (cmd: usbd_stop)\r\n");
        return -1;
    }

#ifdef CONFIG_CHERRYUSB_HOST
    if (usbh_run_flag) {
        LOG_W("usb host already running\r\n");
        LOG_W("please stop it first (cmd: usbh_stop)\r\n");
        return -1;
    }
#endif

    usbd_run_flag = true;
    shell_usbd_stop_cb = stop_cb;

    if (pxTaskCode) {
        LOG_I("Create USB device %s task...\r\n", pcName);
        xTaskCreate(pxTaskCode, pcName, USBD_TASK_STACK_SIZE, NULL, 15, &usbd_cli_handle);
    }

    return 0;
}

/* start usb device test task */
/**
 * @brief Stop the active USB device demo.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 The active demo is stopped or no demo was running.
 */
int shell_usbd_stop(int argc, char **argv)
{
    if (usbd_run_flag == false) {
        LOG_W("usb device not running\r\n");
        return 0;
    }

    if (usbd_cli_handle) {
        vTaskDelete(usbd_cli_handle);
        usbd_cli_handle = NULL;
        LOG_I("usbd task stop\r\n");
    }

    if (shell_usbd_stop_cb) {
        shell_usbd_stop_cb();
        shell_usbd_stop_cb = NULL;
    }

    LOG_I("usb device test stop\r\n");
    usbd_run_flag = false;
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_stop, usbd_stop, stop usb device test task);

/***********************************************************************************/
/* USB device cdc_acm test */
#ifdef CONFIG_CHERRYUSB_DEVICE_CDC_ACM

extern void usbd_cdc_acm_init(void);
extern void usbd_cdc_acm_deinit(void);

/** @brief Deinitialize the CDC ACM demo. */
static void usbd_cdc_acm_stop(void)
{
    usbd_cdc_acm_deinit();
}

/** @brief Start the CDC ACM demo from the shell.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_cdc_acm_test(int argc, char **argv)
{
    if (usbd_test_run(NULL, NULL, usbd_cdc_acm_stop) < 0) {
        return 0;
    }

    usbd_cdc_acm_init();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_cdc_acm_test, usbd_cdc_acm_test, usbd cdc_acm test.);

extern volatile uint32_t acm_stats_rx_bytes;
extern volatile uint32_t acm_stats_tx_bytes;
extern volatile uint32_t acm_stats_rx_overflow;
extern volatile uint32_t acm_stats_rx_cnt;
extern volatile uint32_t acm_stats_tx_cnt;

/** @brief Print CDC ACM loopback statistics.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_cdc_acm_stats(int argc, char **argv)
{
    if (usbd_run_flag == false) {
        LOG_W("usb device not running\r\n");
        return 0;
    }

    LOG_I("=== CDC ACM Statistics ===\r\n");
    LOG_I("  RX bytes:    %u\r\n", acm_stats_rx_bytes);
    LOG_I("  TX bytes:    %u\r\n", acm_stats_tx_bytes);
    LOG_I("  RX packets:  %u\r\n", acm_stats_rx_cnt);
    LOG_I("  TX packets:  %u\r\n", acm_stats_tx_cnt);
    LOG_I("  RX overflow: %u\r\n", acm_stats_rx_overflow);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_cdc_acm_stats, usbd_cdc_acm_stats, show cdc_acm stress test statistics.);

#endif /* CONFIG_CHERRYUSB_DEVICE_CDC_ACM */

/***********************************************************************************/
/* USB device cdc_ecm emac test */
#ifdef CONFIG_CHERRYUSB_DEVICE_CDC_ECM

extern void usbd_ecm_emac_init(void);
extern void usbd_ecm_emac_deinit(void);

/** @brief Deinitialize the CDC ECM-to-EMAC demo. */
static void usbd_cdc_ecm_stop(void)
{
    usbd_ecm_emac_deinit();
}

/** @brief Start the CDC ECM-to-EMAC demo from the shell.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_cdc_ecm_test(int argc, char **argv)
{
    if (usbd_test_run(NULL, NULL, usbd_cdc_ecm_stop) < 0) {
        return 0;
    }

    usbd_ecm_emac_init();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_cdc_ecm_test, usbd_cdc_ecm_test, usbd cdc_ecm test.);
#endif /* CONFIG_CHERRYUSB_DEVICE_CDC_ECM */

/***********************************************************************************/
/* USB device cdc_rndis emac test */
#ifdef CONFIG_CHERRYUSB_DEVICE_CDC_RNDIS

extern void usbd_rndis_emac_init(void);
extern void usb_rndis_emac_deinit(void);

/** @brief Deinitialize the RNDIS-to-EMAC demo. */
static void usbd_cdc_rndis_stop(void)
{
    usb_rndis_emac_deinit();
}

/** @brief Start the RNDIS-to-EMAC demo from the shell.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_cdc_rndis_test(int argc, char **argv)
{
    if (usbd_test_run(NULL, NULL, usbd_cdc_rndis_stop) < 0) {
        return 0;
    }
    usbd_rndis_emac_init();

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(shell_usbd_cdc_rndis_test, usbd_cdc_rndis_test, usbd cdc_rndis test.);
#endif /* CONFIG_CHERRYUSB_DEVICE_CDC_RNDIS */

/***********************************************************************************/
/* USB device hid keyboard test */
#ifdef CONFIG_CHERRYUSB_DEVICE_HID

extern void hid_keyboard_init(uint8_t busid, uintptr_t reg_base);
extern void hid_keyboard_test(uint8_t busid, uint8_t key_val);
static volatile bool hid_keyboard_pause_flag;
static volatile uint8_t hid_keyboard_key_val;

/** @brief Periodically send keyboard usage codes while the demo is active.
 * @param[in] param Unused FreeRTOS task parameter.
 * @note This task runs until deleted by `shell_usbd_stop()`.
 */
static void usbd_hid_keyboard_task(void *param)
{
    LOG_I("usbd_hid_keyboard_task run\r\n");

    hid_keyboard_init(0, 0);
    LOG_I("hid_keyboard_init done\r\n");

    while (1) {
        if (hid_keyboard_pause_flag == true) {
            if (hid_keyboard_key_val != HID_KBD_USAGE_NONE) {
                hid_keyboard_test(0, HID_KBD_USAGE_NONE);
                hid_keyboard_key_val = HID_KBD_USAGE_NONE;
            }
        } else {
            hid_keyboard_key_val += 1;
            if (hid_keyboard_key_val > HID_KBD_USAGE_ENTER) {
                hid_keyboard_key_val = HID_KBD_USAGE_A;
            }
            hid_keyboard_test(0, hid_keyboard_key_val);
        }
        vTaskDelay(100);
    }
}

/** @brief Deinitialize the HID keyboard device. */
static void usbd_hid_keyboard_stop(void)
{
    usbd_deinitialize(0);
}

/** @brief Start the HID keyboard demo from the shell.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_hid_keyboard_test(int argc, char **argv)
{
    hid_keyboard_pause_flag = false;
    hid_keyboard_key_val = HID_KBD_USAGE_ERRUNDEF;
    usbd_test_run(usbd_hid_keyboard_task, "usbd_hid_keyboard", usbd_hid_keyboard_stop);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_hid_keyboard_test, usbd_hid_keyboard_test, usbd hid_keyboard test.);

/** @brief Toggle HID keyboard report generation.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_hid_keyboard_pause(int argc, char **argv)
{
    if (usbd_run_flag == false || shell_usbd_stop_cb != usbd_hid_keyboard_stop) {
        LOG_W("usbd HID not running\r\n");
        return 0;
    }

    hid_keyboard_key_val = HID_KBD_USAGE_ERRUNDEF;

    if (hid_keyboard_pause_flag == false) {
        hid_keyboard_pause_flag = true;
        LOG_I("hid keyboard pause\r\n");
    } else {
        hid_keyboard_pause_flag = false;
        LOG_I("hid keyboard renew\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_hid_keyboard_pause, usbd_hid_keyboard_pause, usbd hid_keyboard pause / renew.);
#endif /* CONFIG_CHERRYUSB_DEVICE_HID */

/***********************************************************************************/
/* USB device msc test */
#ifdef CONFIG_CHERRYUSB_DEVICE_MSC

extern void msc_ram_init(uint8_t busid, uintptr_t reg_base);

/** @brief Deinitialize the RAM-disk MSC device. */
static void usbd_msc_stop(void)
{
    usbd_deinitialize(0);
}

/** @brief Start the RAM-disk MSC demo from the shell.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_msc_test(int argc, char **argv)
{
    if (usbd_test_run(NULL, NULL, usbd_msc_stop) < 0) {
        return 0;
    }

    msc_ram_init(0, 0);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_msc_test, usbd_msc_test, usbd msc test.);
#endif /* CONFIG_CHERRYUSB_DEVICE_MSC */

/***********************************************************************************/
/* USB device audio test */
#ifdef CONFIG_CHERRYUSB_DEVICE_AUDIO

void uac_v1_init(uint8_t busid, uintptr_t reg_base);
void uac_v1_test_polling(uint8_t busid);

/** @brief Initialize and continuously service the UAC 1.0 demo.
 * @param[in] param Unused FreeRTOS task parameter.
 * @note This task runs until deleted by `shell_usbd_stop()`.
 */
static void usbd_uac_v1_task(void *param)
{
    LOG_I("usbd_uac_v1_task run\r\n");

    uac_v1_init(0, 0);
    LOG_I("uac_v1_init done\r\n");

    while (1) {
        uac_v1_test_polling(0);
    }
}

/** @brief Deinitialize the UAC 1.0 device. */
static void usbd_uac_v1_stop(void)
{
    usbd_deinitialize(0);
}

/** @brief Start the UAC 1.0 demo from the shell.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_uac_v1_test(int argc, char **argv)
{
    usbd_test_run(usbd_uac_v1_task, "usbd_uac_v1", usbd_uac_v1_stop);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_uac_v1_test, usbd_uac_v1_test, usbd uac_v1 test.);
#endif /* CONFIG_CHERRYUSB_DEVICE_AUDIO */

/***********************************************************************************/
/* USB device video test */
#ifdef CONFIG_CHERRYUSB_DEVICE_VIDEO

extern void uvc_mjpeg_init(uint8_t busid, uintptr_t reg_base);
extern void uvc_mjpeg_test_polling(uint8_t busid);

/** @brief Initialize and periodically service the static MJPEG UVC demo.
 * @param[in] param Unused FreeRTOS task parameter.
 * @note This task runs until deleted by `shell_usbd_stop()`.
 */
static void usbd_uvc_mjpeg_task(void *param)
{
    LOG_I("usbd_uvc_mjpeg_task run\r\n");

    uvc_mjpeg_init(0, 0);
    LOG_I("uvc_mjpeg_init done\r\n");

    while (1) {
        uvc_mjpeg_test_polling(0);

        vTaskDelay(20);
    }
}

/** @brief Deinitialize the UVC device. */
static void usbd_uvc_mjpeg_stop(void)
{
    usbd_deinitialize(0);
}

/** @brief Start the static MJPEG UVC demo from the shell.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_uvc_mjpeg_test(int argc, char **argv)
{
    usbd_test_run(usbd_uvc_mjpeg_task, "usbd_uvc_mjpeg", usbd_uvc_mjpeg_stop);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_uvc_mjpeg_test, usbd_uvc_mjpeg_test, usbd uvc_mjpeg test.);
#endif /* CONFIG_CHERRYUSB_DEVICE_VIDEO */

/***********************************************************************************/
/* USB composite device acm_cdc msc test */
/* this test will send data to host when DTR is set */
#if (defined(CONFIG_CHERRYUSB_DEVICE_CDC_ACM) && defined(CONFIG_CHERRYUSB_DEVICE_MSC))

extern void usbd_acm_msc_init(void);
extern void usbd_acm_msc_deinit(void);

/** @brief Deinitialize the composite CDC ACM and MSC device. */
static void usbd_cdc_acm_msc_stop(void)
{
    usbd_acm_msc_deinit();
}

/** @brief Start the composite CDC ACM and MSC demo from the shell.
 * @param[in] argc Shell argument count; unused.
 * @param[in] argv Shell argument vector; unused.
 * @retval 0 Command processing completed.
 */
int shell_usbd_cdc_acm_msc_test(int argc, char **argv)
{
    if (usbd_test_run(NULL, NULL, usbd_cdc_acm_msc_stop) < 0) {
        return 0;
    }

    usbd_acm_msc_init();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_usbd_cdc_acm_msc_test, usbd_cdc_acm_msc_test, usbd composite cdc_acm msc test.);

#endif /* CONFIG_CHERRYUSB_DEVICE_CDC_ACM && CONFIG_CHERRYUSB_DEVICE_MSC */
