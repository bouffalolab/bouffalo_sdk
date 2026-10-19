#include "nh_wifi_backend.h"

#include "nh_common.h"

#include "lwip/netif.h"

#include <fhost.h>
#include <net_al.h>
#include <wifi_pkt_hooks.h>

extern int fhost_tx_start(net_al_if_t net_if, net_al_tx_t net_buf,
                          cb_fhost_tx cfm_cb, void *cfm_cb_arg);

static void nh_wifi_user_cfm_cb(uint32_t frame_id, bool acknowledged, void *arg)
{
    NETHUB_UNUSED(frame_id);
    NETHUB_UNUSED(acknowledged);
    NETHUB_UNUSED(arg);
}

static void *nh_wifi_fhost_input_hook(bool is_sta, void *pkt, void *arg)
{
    NETHUB_UNUSED(arg);
    return nh_wifi_bridge_handle_rx(is_sta, (struct pbuf *)pkt);
}

int nh_wifi_backend_init(void)
{
    bl_pkt_eth_input_hook_register(nh_wifi_fhost_input_hook, NULL);
    return 0;
}

nh_wifi_backend_tx_result_t nh_wifi_backend_tx(struct pbuf *p, bool is_sta)
{
    net_al_if_t net_if;

    if (is_sta) {
        net_if = fhost_env.vif[0].net_if;
    } else {
        net_if = fhost_env.vif[1].net_if;
    }

    if (net_if == NULL || !netif_is_up((struct netif *)net_if)) {
        pbuf_free(p);
        return NH_WIFI_BACKEND_TX_ERR_NETIF_DOWN;
    }

    if (fhost_tx_start(net_if, p, nh_wifi_user_cfm_cb, NULL) != 0) {
        pbuf_free(p);
        return NH_WIFI_BACKEND_TX_ERR_SEND;
    }

    return NH_WIFI_BACKEND_TX_OK;
}
