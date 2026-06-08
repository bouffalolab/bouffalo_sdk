#include "nethub_vchan.h"
#include "usb_backend.h"
#include "nh_profile.h"
#include "nethub_defs.h"
#include "nethub.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * USB ACM framing: magic(A5 5A) + len_le16 + payload + checksum_le32.
 * Invalid magic/len/checksum drops one byte and resyncs on the stream.
 */
#define NETHUB_VCHAN_USB_MAGIC0       0xA5U
#define NETHUB_VCHAN_USB_MAGIC1       0x5AU
#define NETHUB_VCHAN_USB_CHECKSUM     0xA5A55A5AU
#define NETHUB_VCHAN_USB_MAGIC0_OFF   0U
#define NETHUB_VCHAN_USB_MAGIC1_OFF   1U
#define NETHUB_VCHAN_USB_LEN_OFF      2U
#define NETHUB_VCHAN_USB_HDR_LEN      4U
#define NETHUB_VCHAN_USB_CHECKSUM_LEN 4U
#define NETHUB_VCHAN_USB_FRAME_MAX \
    (NETHUB_VCHAN_USB_HDR_LEN + NETHUB_VCHAN_MAX_DATA_LEN + NETHUB_VCHAN_USB_CHECKSUM_LEN)
#define NETHUB_VCHAN_USB_RX_TYPE      NETHUB_VCHAN_TYPE_AT

static nethub_vchan_recv_cb_t g_nethub_vchan_usb_recv_cb[NETHUB_VCHAN_TYPE_MAX];
static void *g_nethub_vchan_usb_recv_arg[NETHUB_VCHAN_TYPE_MAX];
static uint8_t g_nethub_vchan_usb_rx_buf[NETHUB_VCHAN_USB_FRAME_MAX];
static uint16_t g_nethub_vchan_usb_rx_len;
static bool g_nethub_vchan_usb_initialized;

static int nethub_vchan_usb_type_is_valid(nethub_vchan_type_t type)
{
    return (type > NETHUB_VCHAN_TYPE_INVALID) && (type < NETHUB_VCHAN_TYPE_MAX);
}

static uint16_t nethub_vchan_usb_get_le16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t nethub_vchan_usb_get_le32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void nethub_vchan_usb_put_le16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)(value & 0xffU);
    p[1] = (uint8_t)((value >> 8) & 0xffU);
}

static void nethub_vchan_usb_put_le32(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)(value & 0xffU);
    p[1] = (uint8_t)((value >> 8) & 0xffU);
    p[2] = (uint8_t)((value >> 16) & 0xffU);
    p[3] = (uint8_t)((value >> 24) & 0xffU);
}

static void nethub_vchan_usb_frame_build(uint8_t *frame,
                                         const void *payload,
                                         uint16_t payload_len)
{
    frame[NETHUB_VCHAN_USB_MAGIC0_OFF] = NETHUB_VCHAN_USB_MAGIC0;
    frame[NETHUB_VCHAN_USB_MAGIC1_OFF] = NETHUB_VCHAN_USB_MAGIC1;
    nethub_vchan_usb_put_le16(&frame[NETHUB_VCHAN_USB_LEN_OFF], payload_len);
    memcpy(&frame[NETHUB_VCHAN_USB_HDR_LEN], payload, payload_len);
    nethub_vchan_usb_put_le32(&frame[NETHUB_VCHAN_USB_HDR_LEN + payload_len],
                              NETHUB_VCHAN_USB_CHECKSUM);
}

static void nethub_vchan_usb_dispatch(nethub_vchan_type_t type,
                                      uint8_t *payload,
                                      uint16_t payload_len)
{
    nethub_vchan_recv_cb_t cb = g_nethub_vchan_usb_recv_cb[type];
    void *arg = g_nethub_vchan_usb_recv_arg[type];

    if (cb != NULL) {
        cb(arg, payload, payload_len);
    }
}

static void nethub_vchan_usb_stream_drop(uint16_t len)
{
    if (len >= g_nethub_vchan_usb_rx_len) {
        g_nethub_vchan_usb_rx_len = 0U;
        return;
    }

    memmove(g_nethub_vchan_usb_rx_buf,
            &g_nethub_vchan_usb_rx_buf[len],
            g_nethub_vchan_usb_rx_len - len);
    g_nethub_vchan_usb_rx_len -= len;
}

static void nethub_vchan_usb_stream_process(void)
{
    while (g_nethub_vchan_usb_rx_len >= 2U) {
        uint16_t payload_len;
        uint16_t frame_len;

        if (g_nethub_vchan_usb_rx_buf[NETHUB_VCHAN_USB_MAGIC0_OFF] != NETHUB_VCHAN_USB_MAGIC0 ||
            g_nethub_vchan_usb_rx_buf[NETHUB_VCHAN_USB_MAGIC1_OFF] != NETHUB_VCHAN_USB_MAGIC1) {
            nethub_vchan_usb_stream_drop(1U);
            continue;
        }

        if (g_nethub_vchan_usb_rx_len < NETHUB_VCHAN_USB_HDR_LEN) {
            return;
        }

        payload_len = nethub_vchan_usb_get_le16(&g_nethub_vchan_usb_rx_buf[NETHUB_VCHAN_USB_LEN_OFF]);
        if (payload_len == 0U ||
            payload_len > NETHUB_VCHAN_MAX_DATA_LEN) {
            nethub_vchan_usb_stream_drop(1U);
            continue;
        }

        frame_len = (uint16_t)(NETHUB_VCHAN_USB_HDR_LEN + payload_len + NETHUB_VCHAN_USB_CHECKSUM_LEN);
        if (g_nethub_vchan_usb_rx_len < frame_len) {
            return;
        }

        if (nethub_vchan_usb_get_le32(&g_nethub_vchan_usb_rx_buf[NETHUB_VCHAN_USB_HDR_LEN + payload_len]) !=
            NETHUB_VCHAN_USB_CHECKSUM) {
            nethub_vchan_usb_stream_drop(1U);
            continue;
        }

        (void)nethub_set_active_host_link(NETHUB_CHANNEL_USB);
        nethub_vchan_usb_dispatch(NETHUB_VCHAN_USB_RX_TYPE,
                                  &g_nethub_vchan_usb_rx_buf[NETHUB_VCHAN_USB_HDR_LEN],
                                  payload_len);
        nethub_vchan_usb_stream_drop(frame_len);
    }
}

static int nethub_vchan_usb_dnld_cb(void *arg, uint8_t *data_buff, uint32_t data_size)
{
    uint32_t i;

    (void)arg;

    if (data_buff == NULL || data_size == 0U) {
        return NETHUB_OK;
    }

    for (i = 0U; i < data_size; i++) {
        if (g_nethub_vchan_usb_rx_len >= sizeof(g_nethub_vchan_usb_rx_buf)) {
            nethub_vchan_usb_stream_drop(1U);
        }
        g_nethub_vchan_usb_rx_buf[g_nethub_vchan_usb_rx_len++] = data_buff[i];
        nethub_vchan_usb_stream_process();
    }

    return NETHUB_OK;
}

struct mr_msg_ctrl_priv_s;

int nethub_usb_vchan_backend_init(struct mr_msg_ctrl_priv_s *msg_ctrl)
{
    int ret;

    (void)msg_ctrl;

    g_nethub_vchan_usb_rx_len = 0U;
    g_nethub_vchan_usb_initialized = false;
    ret = nh_usb_backend_acm_recv_register(nethub_vchan_usb_dnld_cb, NULL);
    if (ret == NETHUB_OK) {
        g_nethub_vchan_usb_initialized = true;
    }

    return ret;
}

static int nethub_usb_vchan_recv_register(nethub_vchan_type_t type,
                                          nethub_vchan_recv_cb_t recv_cb,
                                          void *arg)
{
    if (!nethub_vchan_usb_type_is_valid(type)) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    g_nethub_vchan_usb_recv_cb[type] = recv_cb;
    g_nethub_vchan_usb_recv_arg[type] = arg;

    return NETHUB_OK;
}

static int nethub_usb_vchan_send(nethub_vchan_type_t type, const void *data, uint16_t len)
{
    uint8_t *frame;
    uint16_t frame_len;
    int ret;

    if (!g_nethub_vchan_usb_initialized) {
        return NETHUB_ERR_NOT_INITIALIZED;
    }

    if (!nethub_vchan_usb_type_is_valid(type) ||
        data == NULL ||
        len == 0U ||
        len > NETHUB_VCHAN_MAX_DATA_LEN) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    frame_len = (uint16_t)(NETHUB_VCHAN_USB_HDR_LEN + len + NETHUB_VCHAN_USB_CHECKSUM_LEN);
    frame = (uint8_t *)malloc(frame_len);
    if (frame == NULL) {
        return NETHUB_ERR_NO_MEMORY;
    }

    nethub_vchan_usb_frame_build(frame, data, len);
    ret = nh_usb_backend_acm_send(frame, frame_len);
    free(frame);

    return (ret < 0) ? ret : NETHUB_OK;
}

const nh_vchan_ops_t nhusb_vchan_ops = {
    .send = nethub_usb_vchan_send,
    .recv_register = nethub_usb_vchan_recv_register,
};
