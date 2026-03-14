#include "transport_usb.h"

#define DBG_TAG "NETHUB_USB"
#include "log.h"

static int transport_usb_not_supported(const char *what)
{
    LOG_E("%s not implemented\r\n", what);
    return NETHUB_ERR_NOT_SUPPORTED;
}

static int transport_usb_init(void)
{
    return transport_usb_not_supported("usb host backend");
}

static nethub_route_result_t transport_usb_input(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(frame);
    NETHUB_UNUSED(arg);
    return NETHUB_ROUTE_ERROR;
}

static nethub_route_result_t transport_usb_output(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(frame);
    NETHUB_UNUSED(arg);
    return NETHUB_ROUTE_ERROR;
}

static int transport_usb_ctrlpath_upld_send(uint8_t *data_buff, uint32_t data_size)
{
    NETHUB_UNUSED(data_buff);
    NETHUB_UNUSED(data_size);
    return transport_usb_not_supported("usb ctrl upld");
}

static int transport_usb_ctrlpath_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg)
{
    NETHUB_UNUSED(dnld_cb);
    NETHUB_UNUSED(cbpri_arg);
    return transport_usb_not_supported("usb ctrl dnld");
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
