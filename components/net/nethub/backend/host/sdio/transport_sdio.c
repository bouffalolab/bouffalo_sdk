#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_netdev.h"

#include "transport_sdio.h"
#include "msg_ctrl_sdio.h"
#include "nh_vchan_backend.h"
#include "netdev_wifi.h"
#include "tty_atcmd.h"
#include "nethub_vchan.h"

#define DBG_TAG "NETHUB_SDIO"
#include "log.h"

static nethub_ctrl_rx_cb_t g_transport_sdio_ctrl_dnld_cb;
static void *g_transport_sdio_ctrl_dnld_arg;

static int transport_sdio_ctrlpath_dnld_dispatch(void *arg, uint8_t *data_buff, uint16_t data_size)
{
    NETHUB_UNUSED(arg);

    if (g_transport_sdio_ctrl_dnld_cb == NULL) {
        return NETHUB_OK;
    }

    return g_transport_sdio_ctrl_dnld_cb(g_transport_sdio_ctrl_dnld_arg, data_buff, data_size);
}

static int transport_sdio_ctrlpath_upld_send(uint8_t *data_buff, uint32_t data_size)
{
#ifdef CONFIG_NETHUB_AT_USE_VCHAN
    return nethub_vchan_at_send(data_buff, (uint16_t)data_size);
#else
    return tty_atcmd_upld_send(data_buff, (uint16_t)data_size);
#endif
}

static int transport_sdio_ctrlpath_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg)
{
    g_transport_sdio_ctrl_dnld_cb = dnld_cb;
    g_transport_sdio_ctrl_dnld_arg = cbpri_arg;

#ifdef CONFIG_NETHUB_AT_USE_VCHAN
    if (dnld_cb == NULL) {
        return nethub_vchan_at_recv_register(NULL, NULL);
    }

    return nethub_vchan_at_recv_register(transport_sdio_ctrlpath_dnld_dispatch, NULL);
#else
    if (dnld_cb == NULL) {
        return tty_atcmd_dnld_recv_register(NULL, NULL);
    }

    return tty_atcmd_dnld_recv_register(transport_sdio_ctrlpath_dnld_dispatch, NULL);
#endif
}

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

    ret = nethub_vchan_backend_init(g_msg_sdio_ctrl);
    if (ret < 0) {
        LOG_E("nethub_vchan_backend_init failed\r\n");
        return ret < 0 ? ret : -1;
    }

#ifndef CONFIG_NETHUB_AT_USE_VCHAN
    ret = tty_atcmd_init(g_msg_sdio_ctrl);
    if (ret < 0 || g_tty_atcmd_priv == NULL) {
        LOG_E("tty_atcmd_init failed\r\n");
        return ret < 0 ? ret : -1;
    }
#endif

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

const struct nhif_ops nhsdio_ops = {
    .type = NETHUB_CHANNEL_SDIO,
    .init = transport_sdio_init,
    .deinit = NULL,
    .input = sdio_input_callback,
    .output = sdio_output_callback,
};

const nh_ctrlpath_ops_t nhsdio_ctrlpath_ops = {
    .upld_send = transport_sdio_ctrlpath_upld_send,
    .dnld_register = transport_sdio_ctrlpath_dnld_register,
};
