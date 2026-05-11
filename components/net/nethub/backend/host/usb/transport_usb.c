#include "transport_usb.h"
#include "usb_backend.h"
#include "nethub_vchan.h"

#define DBG_TAG "NETHUB_USB"
#include "log.h"

struct mr_msg_ctrl_priv_s;
int nethub_vchan_backend_init(struct mr_msg_ctrl_priv_s *msg_ctrl);

static nethub_ctrl_rx_cb_t g_transport_usb_ctrl_dnld_cb;
static void *g_transport_usb_ctrl_dnld_arg;

static int transport_usb_ctrlpath_dnld_dispatch(void *arg, uint8_t *data_buff, uint16_t data_size)
{
    (void)arg;

    if (g_transport_usb_ctrl_dnld_cb == NULL) {
        return NETHUB_OK;
    }

    return g_transport_usb_ctrl_dnld_cb(g_transport_usb_ctrl_dnld_arg, data_buff, (uint32_t)data_size);
}

static nethub_route_result_t transport_usb_input(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(frame);
    NETHUB_UNUSED(arg);
    return NETHUB_ROUTE_CONTINUE;
}

static int transport_usb_init(void)
{
    int ret;

    ret = nh_usb_backend_init();
    if (ret < 0) {
        return ret;
    }

    return nethub_vchan_backend_init(NULL);
}

static nethub_route_result_t transport_usb_output(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(arg);
    return nh_usb_backend_output(frame);
}

static int transport_usb_ctrlpath_upld_send(uint8_t *data_buff, uint32_t data_size)
{
    if (data_size > NETHUB_VCHAN_MAX_DATA_LEN) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    return nethub_vchan_at_send(data_buff, (uint16_t)data_size);
}

static int transport_usb_ctrlpath_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg)
{
    g_transport_usb_ctrl_dnld_cb = dnld_cb;
    g_transport_usb_ctrl_dnld_arg = cbpri_arg;

    if (dnld_cb == NULL) {
        return nethub_vchan_at_recv_register(NULL, NULL);
    }

    return nethub_vchan_at_recv_register(transport_usb_ctrlpath_dnld_dispatch, NULL);
}

const struct nhif_ops nhusb_ops = {
    .type = NETHUB_CHANNEL_USB,
    .init = transport_usb_init,
    .deinit = NULL,
    .input = transport_usb_input,
    .output = transport_usb_output,
};

const nh_ctrlpath_ops_t nhusb_ctrlpath_ops = {
    .upld_send = transport_usb_ctrlpath_upld_send,
    .dnld_register = transport_usb_ctrlpath_dnld_register,
};
