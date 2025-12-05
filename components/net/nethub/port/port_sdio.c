/**
 * @file port_sdio.c
 * @brief SDIO interface port adaptation layer implementation
 */

#include <stdio.h>
#include "nhsdiowifi.h"


#include "bflb_core.h"
#include "bflb_mtimer.h"

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bflb_sdio_drv.h"

#include "bflb_sdio2.h"
#include "bflb_sdio3.h"

#include "bflb_msg_ctrl.h"
#include "bflb_frame_buff_ctrl.h"
#include "bflb_tty.h"
#include <transportsdio.h>
#include <netpacket_filter.h>
#include <nethub_types.h>
#include <nethub.h>
#include <log.h>

#define DEBUG_DUMP_WIFITX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data
#define DEBUG_DUMP_WIFIRX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data

/* Debug output for example program */
#ifdef CONFIG_NETHUB_DEBUG
#define EXAMPLE_DEBUG       LOG_D//printf("[PORTSDIO] " fmt, ##__VA_ARGS__)
#else
#define EXAMPLE_DEBUG(fmt, ...)
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
static int _wifi_dnld(
        struct wifi_wifista_priv *ctx,
        void *buf,
        uint32_t buf_len,
        void (*free_cb)(void *ctx, void *arg),
        void *cb_arg)
{
    nh_forward_result_t result;
    nh_skb_t skb;

    skb.data    = buf;
    skb.len     = buf_len;
    skb.free_cb = free_cb;
    skb.cb_priv = ctx;
    skb.cb_arg  = cb_arg;
    skb.next    = NULL;
#if 0
    {
        uint8_t *vbuf = (uint8_t *)buf;
        printf(" --- %s, line:%d, len:%d, data[%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X]\r\n",
            __func__, __LINE__, buf_len,
            vbuf[0],vbuf[1],vbuf[2],vbuf[3],vbuf[4],vbuf[5],
            vbuf[6],vbuf[7],vbuf[8],vbuf[9],vbuf[10],vbuf[11]);
    }
#endif
    // nethub_transport_dnld(HRTRANSPORT_SDIO, &skb);
    result = nethub_process_input(&skb, NHIF_TYPE_SDIO);
    if (result == NH_FORWARD_STOP) {
        EXAMPLE_DEBUG("Data consumed, forwarding stopped\n");
    } else if (result == NH_FORWARD_CONTINUE) {
        // EXAMPLE_DEBUG("Data forwarding continues\n");
    } else {
        EXAMPLE_DEBUG("Forwarding result abnormal: %d\n", result);
    }

    return 0;
}
/* SDIO interface input callback */
static nh_forward_result_t _sdio_input_callback(nh_skb_t *skb, void *arg)
{
    int ret;

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
    printf("@%p:%d\n", skb->data, skb->len);
#endif

    ret = transportsdio_send_upld_skb(skb);

    if (ret != 0) {
        EXAMPLE_DEBUG("ret:%d.\r\n", ret);
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
    bflb_msg_ctrl_ops_t *sdio_ops = NULL;

    EXAMPLE_DEBUG("nh sdio init\n");

#ifdef USE_SDIO2_DRIVER
    sdio_ops = sdio2_driver_init();
#elif defined(USE_SDIO3_DRIVER)
    sdio_ops = sdio3_driver_init();
#else
    printf("Error: No SDIO driver defined for this chip\n");
    return -1;
#endif

    if (sdio_ops == NULL) {
        printf("Error: SDIO driver init failed\n");
        return -1;
    }
    /* Initialize message controller */
    bflb_msg_ctrl_t *msg_ctrl = bflb_msg_ctrl_init(sdio_ops);
    if (msg_ctrl == NULL) {
        printf("Error: bl_msg_ctrl_init failed\n");
        return -2;
    }

    /* Initialize Ethernet */
    // app_wifista_init(msg_ctrl);
    /* Initialize TTY device */
    bflb_tty_init(msg_ctrl, BL_MSG_TAG_TTY);

    g_sdio_ctx_p = malloc(sizeof(struct wifi_wifista_priv));
    if (!g_sdio_ctx_p) {
        EXAMPLE_DEBUG("Failed to alloc.\r\n", __func__, __LINE__);
        return -3;
    }
    memset(g_sdio_ctx_p, 0, sizeof(struct wifi_wifista_priv));

    wifi_mgmr_sta_mac_get(g_sdio_ctx_p->mac_addr);
    EXAMPLE_DEBUG("%s Using MAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            __func__,
            g_sdio_ctx_p->mac_addr[0], g_sdio_ctx_p->mac_addr[1],
            g_sdio_ctx_p->mac_addr[2], g_sdio_ctx_p->mac_addr[3],
            g_sdio_ctx_p->mac_addr[4], g_sdio_ctx_p->mac_addr[5]);

    // hr_transport_register(HRTRANSPORT_SDIO, &_sdio_ops);

    transportsdio_init(g_sdio_ctx_p, msg_ctrl, BL_MSG_TAG_ETH);
    transportsdio_set_wifi_send_cb(_wifi_dnld);

    return NH_OK;
}

/* SDIO interface operation structure */
const struct nhif_ops nhsdio_ops = {
    .type = NHIF_TYPE_SDIO,
    .init = _sdio_init,
    .deinit = NULL,
    .input = _sdio_input_callback,
    .output = _sdio_output_callback,
};
