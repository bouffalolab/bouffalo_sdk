#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_netdev.h"

#include "msg_ctrl_sdio.h"
#include "netdev_wifi.h"
#include "tty_atcmd.h"

#include "transport_sdio.h"
#include "virtualchan.h"

#include <nethub.h>
#include <nethub_types.h>

#define DBG_TAG "SDIO_TRANS_PORT"
#include "log.h"

static int transport_sdio_init(void)
{
    int ret;

    /* sdio mr init */
    ret = msg_ctrl_sdio_init();
    if (ret < 0 || g_msg_sdio_ctrl == NULL) {
        LOG_E("msg_ctrl_sdio_init failed\r\n");
        return ret;
    }

    /* netdev sta init */
    ret = netdev_wifi_init(g_msg_sdio_ctrl);
    if (ret < 0 || g_netdev_wifi_priv == NULL) {
        LOG_E("netdev_wifi_init failed\r\n");
        return ret;
    }

    /* virtualchan */
    ret = nethub_vchan_init(g_msg_sdio_ctrl);
    if (ret < 0 || g_virtualchan_priv == NULL) {
        LOG_E("tty_atcmd_init failed\r\n");
        return ret;
    }

    /* tty */
    ret = tty_atcmd_init(g_msg_sdio_ctrl);
    if (ret < 0 || g_tty_atcmd_priv == NULL) {
        LOG_E("tty_atcmd_init failed\r\n");
        return ret;
    }

    return 0;
}

static nh_forward_result_t _sdio_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg); /* Avoid unused parameter warning */

    return NH_FORWARD_CONTINUE;
}

nh_forward_result_t _sdio_output_callback(nh_skb_t *skb, void *arg)
{
    netdev_wifi_upld(skb, arg); /* netdev_sta.c */

    return NH_FORWARD_CONTINUE;
}

/* SDIO interface operation structure */
struct nhif_ops nhsdio_ops = {
    .type = NHIF_TYPE_SDIO,
    .init = transport_sdio_init,
    .deinit = NULL,
    .input = _sdio_input_callback,
    .output = _sdio_output_callback,
};

// fixme
int nethub_ctrlpath_upld_send(uint8_t *data_buff, uint32_t data_size)
{
    return tty_atcmd_upld_send(data_buff, data_size);
}
int nethub_ctrlpath_dnld_register(nethub_ctrlpath_callback_t dnld_cb, void *cbpri_arg)
{
    return tty_atcmd_dnld_recv_register((atcmd_dnld_cb_t)dnld_cb, cbpri_arg);
}
