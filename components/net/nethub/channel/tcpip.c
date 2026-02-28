/**
 * @file port_tcpipap.c
 * @brief TCPIPAP interface port adaptation layer implementation
 */

#include <stdio.h>
#include "nhsdiowifi.h"

#include "bflb_core.h"
#include "bflb_mtimer.h"

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"

#include <netpacket_filter.h>
#include <lwip/pbuf.h>
#include "lwip/netif.h"
#include <log.h>

#ifdef CONFIG_WL80211
#else
#include <net_al.h>
#include <fhost.h>
#endif

#define DEBUG_DUMP_WIFITX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data
#define DEBUG_DUMP_WIFIRX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data

/* Debug output for example program */
#define USER_DBG(fmt, ...)     //printf("[TCPIP] " fmt, ##__VA_ARGS__)
#if (DEBUG_DUMP_WIFITX_ENABLE | DEBUG_DUMP_WIFIRX_ENABLE)
#define USER_INFO(fmt, ...)    printf("[TCPIP]" fmt, ##__VA_ARGS__)
#else
#define USER_INFO(...)
#endif


/* =================================== ap start =================================== */
/* TCPIPAP interface input callback */
static nh_forward_result_t tcpipap_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    USER_DBG("TCPIPAP INPUT received data: %s (simulated TCP/IP AP processing)\n", skb->data);

    nh_forward_result_t result;
    USER_DBG("TCPIPAP INPUT received data: %s (simulated TCP/IP AP processing)\n", skb->data);

    result = nethub_process_input(skb, NHIF_TYPE_TCPIPSTA);
    if (result == NH_FORWARD_STOP) {
        USER_DBG("Data consumed, forwarding stopped\n");
    } else if (result == NH_FORWARD_CONTINUE) {
        USER_DBG("Data forwarding continues\n");
    } else {
        USER_DBG("Forwarding result abnormal: %d\n", result);
    }

    return NH_FORWARD_CONTINUE;
}

/* TCPIPAP interface output callback */
static nh_forward_result_t tcpipap_output_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */

    USER_DBG("TCPIPAP OUTPUT send data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* TCPIPAP interface operation structure */
const struct nhif_ops nhtcpipap_ops = {
    .type = NHIF_TYPE_TCPIPAP,
    .init = NULL,
    .deinit = NULL,
    .input = tcpipap_input_callback,
    .output = tcpipap_output_callback,
};
/* =================================== ap end =================================== */

/* =================================== sta start =================================== */

/* TCPIPSTA interface input callback */
static nh_forward_result_t tcpipsta_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    NH_UNUSED(skb);  /* Avoid unused parameter warning */

    return NH_FORWARD_CONTINUE;
}

/* TCPIPSTA interface output callback */
static nh_forward_result_t tcpipsta_output_callback(nh_skb_t *skb, void *arg)
{
    nh_forward_result_t result;

    NH_UNUSED(arg);  /* Avoid unused parameter warning */


#if DEBUG_DUMP_WIFIRX_ENABLE
    USER_INFO("@%p:%d\n", skb->data, skb->len);
#endif
    //result = nethub_process_input(skb, NHIF_TYPE_TCPIPSTA);
    return NH_FORWARD_CONTINUE;
}

/* TCPIPSTA interface operation structure */
const struct nhif_ops nhtcpipsta_ops = {
    .type = NHIF_TYPE_TCPIPSTA,
    .init = NULL,
    .deinit = NULL,
    .input = tcpipsta_input_callback,
    .output = tcpipsta_output_callback,
};
/* =================================== sta end =================================== */
