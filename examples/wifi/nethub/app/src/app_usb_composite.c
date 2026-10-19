#include "app_usb_composite.h"

#include "nethub_usb_device.h"

#include <stdbool.h>
#include <stdint.h>

#define DBG_TAG "APP_USB"
#include "log.h"

#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_acm.h"
#include "usbd_cdc_ecm.h"

#ifndef CONFIG_USBDEV_ADVANCE_DESC
#error "NetHub USB composite expects CONFIG_USBDEV_ADVANCE_DESC"
#endif

#define APP_USB_BUSID                0U
#define APP_USB_VID                  0xFFFFU
#define APP_USB_PID                  0xFFFFU
#define APP_USB_MAX_POWER            100U
#define APP_USB_MAC_STRING_INDEX     4U
#define APP_USB_INTERFACE_COUNT      0x04U

#define APP_USB_ECM_IN_EP            0x81U
#define APP_USB_ECM_OUT_EP           0x02U
#define APP_USB_ECM_INT_EP           0x85U
#define APP_USB_CMD_ACM_IN_EP        0x83U
#define APP_USB_CMD_ACM_OUT_EP       0x04U
#define APP_USB_CMD_ACM_INT_EP       0x86U

#ifdef CONFIG_USB_HS
#define APP_USB_CDC_MAX_MPS 512U
#else
#define APP_USB_CDC_MAX_MPS 64U
#endif

#define APP_USB_CONFIG_DESCRIPTOR_SZ (9U + CDC_ECM_DESCRIPTOR_LEN + CDC_ACM_DESCRIPTOR_LEN)

static bool g_app_usb_configured;
static bool g_app_usb_started;
static struct usbd_interface g_app_ecm_intf0;
static struct usbd_interface g_app_ecm_intf1;
static struct usbd_interface g_app_cmd_acm_intf0;
static struct usbd_interface g_app_cmd_acm_intf1;
static nethub_usb_device_cdc_cbs_t g_app_ecm_cbs;
static nethub_usb_device_cdc_cbs_t g_app_cmd_acm_cbs;

static struct usbd_endpoint g_app_cmd_acm_out_ep = {
    .ep_addr = APP_USB_CMD_ACM_OUT_EP,
};

static struct usbd_endpoint g_app_cmd_acm_in_ep = {
    .ep_addr = APP_USB_CMD_ACM_IN_EP,
};

static const uint8_t g_app_usb_device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, APP_USB_VID, APP_USB_PID, 0x0100, 0x01),
};

static const uint8_t g_app_usb_config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(APP_USB_CONFIG_DESCRIPTOR_SZ, APP_USB_INTERFACE_COUNT, 0x01,
                               USB_CONFIG_BUS_POWERED, APP_USB_MAX_POWER),
    CDC_ECM_DESCRIPTOR_INIT(0x00, APP_USB_ECM_INT_EP, APP_USB_ECM_OUT_EP, APP_USB_ECM_IN_EP,
                            APP_USB_CDC_MAX_MPS, APP_USB_MAC_STRING_INDEX),
    CDC_ACM_DESCRIPTOR_INIT(0x02, APP_USB_CMD_ACM_INT_EP, APP_USB_CMD_ACM_OUT_EP, APP_USB_CMD_ACM_IN_EP,
                            APP_USB_CDC_MAX_MPS, USB_STRING_PRODUCT_INDEX),
};

#ifdef CONFIG_USB_HS
static const uint8_t g_app_usb_device_qualifier_descriptor[] = {
    0x0A, USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
};
#endif

static const uint8_t *app_usb_device_descriptor_cb(uint8_t speed)
{
    (void)speed;
    return g_app_usb_device_descriptor;
}

static const uint8_t *app_usb_config_descriptor_cb(uint8_t speed)
{
    (void)speed;
    return g_app_usb_config_descriptor;
}

static const uint8_t *app_usb_device_quality_descriptor_cb(uint8_t speed)
{
    (void)speed;
#ifdef CONFIG_USB_HS
    return g_app_usb_device_qualifier_descriptor;
#else
    return NULL;
#endif
}

static const char *app_usb_string_descriptor_cb(uint8_t speed, uint8_t index)
{
    static const char langid[] = { 0x09, 0x04 };

    (void)speed;

    switch (index) {
        case USB_STRING_LANGID_INDEX:
            return langid;
        case USB_STRING_MFC_INDEX:
            return "BouffaloLab";
        case USB_STRING_PRODUCT_INDEX:
            return "Bouffalo Nethub ECM";
        case USB_STRING_SERIAL_INDEX:
            return "0000000001";
        case APP_USB_MAC_STRING_INDEX:
            return nethub_usb_device_ecm_mac_string();
        default:
            return NULL;
    }
}

static const struct usb_descriptor g_app_usb_descriptor = {
    .device_descriptor_callback = app_usb_device_descriptor_cb,
    .config_descriptor_callback = app_usb_config_descriptor_cb,
    .device_quality_descriptor_callback = app_usb_device_quality_descriptor_cb,
    .string_descriptor_callback = app_usb_string_descriptor_cb,
};

static int app_usb_ecm_start_out_read(uint8_t *data, uint32_t len)
{
    return usbd_cdc_ecm_start_read(data, len);
}

static int app_usb_ecm_start_in_write(uint8_t *data, uint32_t len)
{
    return usbd_cdc_ecm_start_write(data, len);
}

static int app_usb_ecm_write_interrupt_in(uint8_t *data, uint32_t len)
{
    if (data == NULL || len == 0U) {
        return usbd_cdc_ecm_set_connect(false, NULL);
    }

    if (len != (sizeof(uint32_t) * 2U)) {
        return -1;
    }

    return usbd_cdc_ecm_set_connect(true, (uint32_t *)data);
}

static int app_usb_cmd_acm_start_out_read(uint8_t *data, uint32_t len)
{
    return usbd_ep_start_read(APP_USB_BUSID, APP_USB_CMD_ACM_OUT_EP, data, len);
}

static int app_usb_cmd_acm_start_in_write(uint8_t *data, uint32_t len)
{
    return usbd_ep_start_write(APP_USB_BUSID, APP_USB_CMD_ACM_IN_EP, data, len);
}

static void app_usb_cmd_acm_out_done_cb(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;

    if (g_app_cmd_acm_cbs.out_done_cb != NULL) {
        g_app_cmd_acm_cbs.out_done_cb(nbytes);
    }
}

static void app_usb_cmd_acm_in_done_cb(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    uint16_t mps;

    mps = usbd_get_ep_mps(busid, ep);
    if (nbytes != 0U && mps != 0U && (nbytes % mps) == 0U && usbd_ep_start_write(busid, ep, NULL, 0U) == 0) {
        return;
    }

    if (g_app_cmd_acm_cbs.in_done_cb != NULL) {
        g_app_cmd_acm_cbs.in_done_cb(nbytes);
    }
}

/* CherryUSB ECM completion callbacks are application-owned after the backend split. */
void usbd_cdc_ecm_data_send_done(uint32_t len)
{
    if (g_app_ecm_cbs.in_done_cb != NULL) {
        g_app_ecm_cbs.in_done_cb(len);
    }
}

void usbd_cdc_ecm_data_recv_done(uint32_t len)
{
    if (g_app_ecm_cbs.out_done_cb != NULL) {
        g_app_ecm_cbs.out_done_cb(len);
    }
}

static void app_usb_register_functions(void)
{
    usbd_add_interface(APP_USB_BUSID,
                       usbd_cdc_ecm_init_intf(&g_app_ecm_intf0, APP_USB_ECM_INT_EP, APP_USB_ECM_OUT_EP,
                                              APP_USB_ECM_IN_EP));
    usbd_add_interface(APP_USB_BUSID,
                       usbd_cdc_ecm_init_intf(&g_app_ecm_intf1, APP_USB_ECM_INT_EP, APP_USB_ECM_OUT_EP,
                                              APP_USB_ECM_IN_EP));

    usbd_add_interface(APP_USB_BUSID, usbd_cdc_acm_init_intf(APP_USB_BUSID, &g_app_cmd_acm_intf0));
    usbd_add_interface(APP_USB_BUSID, usbd_cdc_acm_init_intf(APP_USB_BUSID, &g_app_cmd_acm_intf1));
    g_app_cmd_acm_out_ep.ep_cb = app_usb_cmd_acm_out_done_cb;
    g_app_cmd_acm_in_ep.ep_cb = app_usb_cmd_acm_in_done_cb;
    usbd_add_endpoint(APP_USB_BUSID, &g_app_cmd_acm_out_ep);
    usbd_add_endpoint(APP_USB_BUSID, &g_app_cmd_acm_in_ep);
}

static void app_usb_event_cb(uint8_t busid, uint8_t event)
{
    (void)busid;

    if (g_app_ecm_cbs.event_cb != NULL) {
        g_app_ecm_cbs.event_cb(event);
    }
}

int app_usb_composite_configure(void)
{
    const nethub_usb_device_ops_t usb_ops = {
        .init = app_usb_composite_start,
        .deinit = app_usb_composite_stop,
    };
    const nethub_usb_device_cdc_ops_t ecm_ops = {
        .start_out_read = app_usb_ecm_start_out_read,
        .start_in_write = app_usb_ecm_start_in_write,
        .write_interrupt_in = app_usb_ecm_write_interrupt_in,
    };
    const nethub_usb_device_cdc_ops_t cmd_acm_ops = {
        .start_out_read = app_usb_cmd_acm_start_out_read,
        .start_in_write = app_usb_cmd_acm_start_in_write,
        .write_interrupt_in = NULL,
    };
    int ret;

    if (g_app_usb_configured) {
        return 0;
    }

    ret = nethub_usb_device_init(&usb_ops);
    if (ret != NETHUB_OK) {
        LOG_E("NetHub USB device init failed: %d\r\n", ret);
        return -1;
    }

    ret = nethub_usb_device_cdc_ecm_init(&ecm_ops, &g_app_ecm_cbs, 0U);
    if (ret != NETHUB_OK) {
        LOG_E("NetHub USB ECM init failed: %d\r\n", ret);
        return -1;
    }

    ret = nethub_usb_device_cdc_acm_cmd_init(&cmd_acm_ops, &g_app_cmd_acm_cbs);
    if (ret != NETHUB_OK) {
        LOG_E("NetHub USB CMD ACM init failed: %d\r\n", ret);
        return -1;
    }

    g_app_usb_configured = true;
    return 0;
}

int app_usb_composite_start(void)
{
    int ret;

    if (g_app_usb_started) {
        return 0;
    }

    if (!g_app_usb_configured) {
        LOG_E("USB composite is not configured\r\n");
        return -1;
    }

    usbd_desc_register(APP_USB_BUSID, &g_app_usb_descriptor);
    app_usb_register_functions();

    ret = usbd_initialize(APP_USB_BUSID, 0, app_usb_event_cb);
    if (ret != 0) {
        LOG_E("usbd_initialize failed: %d\r\n", ret);
        return -1;
    }

    g_app_usb_started = true;
    LOG_I("USB composite started: ECM if=0/1, CMD ACM if=2/3\r\n");
    return 0;
}

int app_usb_composite_stop(void)
{
    int ret;

    if (!g_app_usb_started) {
        return 0;
    }

    ret = usbd_deinitialize(APP_USB_BUSID);
    if (ret == 0) {
        g_app_usb_started = false;
    }

    return ret;
}
