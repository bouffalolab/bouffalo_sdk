#include "transport_usb.h"
#include "usb_backend.h"

#define DBG_TAG "NETHUB_USB"
#include "log.h"

static nethub_route_result_t transport_usb_input(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(frame);
    NETHUB_UNUSED(arg);
    return NETHUB_ROUTE_CONTINUE;
}

static int transport_usb_init(void)
{
    return nh_usb_backend_init();
}

static nethub_route_result_t transport_usb_output(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(arg);
    return nh_usb_backend_output(frame);
}

static int transport_usb_ctrlpath_upld_send(uint8_t *data_buff, uint32_t data_size)
{
    return nh_usb_backend_ctrl_upld_send(data_buff, data_size);
}

static int transport_usb_ctrlpath_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg)
{
    return nh_usb_backend_ctrl_dnld_register(dnld_cb, cbpri_arg);
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
