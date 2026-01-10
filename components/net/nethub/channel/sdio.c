/**
 * @file port_sdio.c
 * @brief SDIO interface port adaptation layer implementation
 */

#include <stdio.h>
#include <nethub.h>

#include <transportsdio.h>
#include "bflb_sdio_drv.h"

#include <bflb_tty.h>
#ifdef CONFIG_NETHUB_NETLINK
#include <bflb_netlink.h>
#endif

#include <wifi_mgmr.h>
#include <wifi_mgmr_ext.h>

#define DEBUG_DUMP_WIFITX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data
#define DEBUG_DUMP_WIFIRX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data

/* Debug output for example program */
#define USER_DBG(fmt, ...)     //printf("[NH_ERR] " fmt, ##__VA_ARGS__)
#if (DEBUG_DUMP_WIFITX_ENABLE | DEBUG_DUMP_WIFIRX_ENABLE)
#define USER_INFO(fmt, ...)    printf("[SDIO]" fmt, ##__VA_ARGS__)
#else
#define USER_INFO(...)
#endif

wifi_wifista_priv_t *g_sdio_ctx_p = NULL;

// ========================================= dnld  =========================================
/**
 * @brief WiFi download function - receives data from WiFi and forwards to nethub
 * @param ctx WiFi private context
 * @param buf Data buffer to download
 * @param buf_len Length of data buffer
 * @return 0 on success
 */
static int callback_wifi_dnld(struct wifi_wifista_priv *ctx,
        void *buf, uint32_t buf_len, void (*free_cb)(void *arg), void *cb_arg)
{
    nh_forward_result_t result;

    nh_skb_t skb;
    skb.data    = buf;
    skb.len     = buf_len;
    skb.free_cb = free_cb;
    skb.cb_arg  = cb_arg;
    skb.next    = NULL;

#if DEBUG_DUMP_WIFITX_ENABLE
    USER_INFO("@%p:%d\n", skb->data, skb->len);
#endif
    // nethub_transport_dnld(HRTRANSPORT_SDIO, &skb);
    result = nethub_process_input(&skb, NHIF_TYPE_SDIO);
    if (result == NH_FORWARD_STOP) {
        USER_DBG("Data consumed, forwarding stopped\n");
    } else if (result == NH_FORWARD_CONTINUE) {
        // USER_DBG("Data forwarding continues\n");
    } else {
        USER_DBG("Forwarding result abnormal: %d\n", result);
    }

    return 0;
}
/* SDIO interface input callback */
static nh_forward_result_t _sdio_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */

    return NH_FORWARD_CONTINUE;
}
// ========================================= dnld  =========================================

// ========================================= upld  =========================================
/* SDIO interface output callback */
static nh_forward_result_t _sdio_output_callback(nh_skb_t *skb, void *arg)
{
    int ret;
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
#if DEBUG_DUMP_WIFIRX_ENABLE
    USER_INFO("@%p:%d\n", skb->data, skb->len);
#endif

    ret = transportsdio_send_upld_skb(skb);

    if (ret != 0) {
        USER_DBG("ret:%d.\r\n", ret);
    }
    return NH_FORWARD_CONTINUE;
}

// ========================================= upld  =========================================
// ========================================= init  =========================================
/**
 * @brief Initialize SDIO transport layer
 * @return 0 on success, negative value on error
 */
static int _sdio_init(void)
{
    int res;
    bflb_msg_ctrl_ops_t *sdio_ops = NULL;

    USER_DBG("nh sdio init\r\n");

#ifdef USE_SDIO2_DRIVER
    sdio_ops = sdio2_driver_init();
#elif defined(USE_SDIO3_DRIVER)
    sdio_ops = sdio3_driver_init();
#else
    res = -1;
    goto __sdio_init_failed
#endif

    if (sdio_ops == NULL) {
        res = -2;
        goto __sdio_init_failed;
    }
    /* Initialize message controller */
    bflb_msg_ctrl_t *msg_ctrl = bflb_msg_ctrl_init(sdio_ops);
    if (msg_ctrl == NULL) {
        res = -3;
        goto __sdio_init_failed;
    }

    /* Initialize TTY device */
    bflb_tty_init(msg_ctrl, BL_MSG_TAG_TTY);

#ifdef CONFIG_NETHUB_NETLINK
    /* Initialize Netlink device */
    bflb_netlink_init(msg_ctrl, BL_MSG_TAG_NETLINK);
#endif

    g_sdio_ctx_p = malloc(sizeof(struct wifi_wifista_priv));
    if (!g_sdio_ctx_p) {
        res = -4;
        goto __sdio_init_failed;
    }
    memset(g_sdio_ctx_p, 0, sizeof(struct wifi_wifista_priv));

    wifi_mgmr_sta_mac_get(g_sdio_ctx_p->mac_addr);
    USER_DBG("%s Using MAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            __func__,
            g_sdio_ctx_p->mac_addr[0], g_sdio_ctx_p->mac_addr[1],
            g_sdio_ctx_p->mac_addr[2], g_sdio_ctx_p->mac_addr[3],
            g_sdio_ctx_p->mac_addr[4], g_sdio_ctx_p->mac_addr[5]);


    transportsdio_init(g_sdio_ctx_p, msg_ctrl, BL_MSG_TAG_ETH);
    transportsdio_set_wifi_send_cb(callback_wifi_dnld);

    return NH_OK;

__sdio_init_failed:
    printf("sdio_init ret:%d\r\n", res);
    return res;
}

/* SDIO interface operation structure */
const struct nhif_ops nhsdio_ops = {
    .type = NHIF_TYPE_SDIO,
    .init = _sdio_init,
    .deinit = NULL,
    .input = _sdio_input_callback,
    .output = _sdio_output_callback,
};
