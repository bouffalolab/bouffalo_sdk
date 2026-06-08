#include "transport_usb.h"
#include "usb_backend.h"

#define DBG_TAG "NETHUB_USB"
#include "log.h"

struct mr_msg_ctrl_priv_s;
int nethub_usb_vchan_backend_init(struct mr_msg_ctrl_priv_s *msg_ctrl);

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

    return nethub_usb_vchan_backend_init(NULL);
}

static nethub_route_result_t transport_usb_output(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(arg);
    return nh_usb_backend_output(frame);
}

const struct nhif_ops nhusb_ops = {
    .type = NETHUB_CHANNEL_USB,
    .init = transport_usb_init,
    .deinit = NULL,
    .input = transport_usb_input,
    .output = transport_usb_output,
#if defined(CONFIG_NETHUB_LOWPOWER_ENABLE)
    .is_idle = nh_usb_backend_is_idle,
    .lowpower_prepare = nh_usb_backend_lowpower_prepare,
    .lowpower_resume = nh_usb_backend_lowpower_resume,
#endif
};
