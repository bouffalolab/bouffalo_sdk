#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_netdev.h"

#include "transport_sdio.h"
#include "msg_ctrl_sdio.h"
#include "nh_vchan_backend.h"
#include "netdev_wifi.h"

#if defined(CONFIG_NETHUB_LOWPOWER_ENABLE)
#include "mr_sdio_drv.h"
#endif

#define DBG_TAG "NETHUB_SDIO"
#include "log.h"

static int transport_sdio_init(void)
{
    int ret;

    ret = msg_ctrl_sdio_init();
    if (ret < 0 || g_msg_sdio_ctrl == NULL) {
        LOG_E("msg_ctrl_sdio_init failed\r\n");
        return ret < 0 ? ret : -1;
    }

    ret = netdev_wifi_init(g_msg_sdio_ctrl);
    if (ret < 0 || g_netdev_wifi_priv == NULL) {
        LOG_E("netdev_wifi_init failed\r\n");
        return ret < 0 ? ret : -1;
    }

    ret = nethub_sdio_vchan_backend_init(g_msg_sdio_ctrl);
    if (ret < 0) {
        LOG_E("nethub_vchan_backend_init failed\r\n");
        return ret < 0 ? ret : -1;
    }

    return NETHUB_OK;
}

static nethub_route_result_t sdio_input_callback(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(frame);
    NETHUB_UNUSED(arg);
    return NETHUB_ROUTE_CONTINUE;
}

static nethub_route_result_t sdio_output_callback(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(arg);
    netdev_wifi_upld(frame, NULL);
    return NETHUB_ROUTE_CONTINUE;
}

#if defined(CONFIG_NETHUB_LOWPOWER_ENABLE)
static bool transport_sdio_lp_is_idle(void)
{
    return true;
}

static int transport_sdio_lp_prepare(void)
{
    return mr_sdio_drv_lowpower_prepare();
}

static int transport_sdio_lp_resume(void)
{
    return mr_sdio_drv_lowpower_restore();
}
#endif

const struct nhif_ops nhsdio_ops = {
    .type = NETHUB_CHANNEL_SDIO,
    .init = transport_sdio_init,
    .deinit = NULL,
    .input = sdio_input_callback,
    .output = sdio_output_callback,
#if defined(CONFIG_NETHUB_LOWPOWER_ENABLE)
    .is_idle = transport_sdio_lp_is_idle,
    .lowpower_prepare = transport_sdio_lp_prepare,
    .lowpower_resume = transport_sdio_lp_resume,
#endif
};
