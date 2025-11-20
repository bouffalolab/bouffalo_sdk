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

#include "bflb_sdio_drv.h"
#include "bflb_msg_ctrl.h"
#include "bflb_frame_buff_ctrl.h"
#include "bflb_tty.h"
#include <transportsdio.h>
#include <netpacket_filter.h>
#include <lwip/pbuf.h>
#include "lwip/netif.h"
#include <log.h>

#include <net_al.h>
#include <fhost.h>

#define DEBUG_DUMP_WIFITX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data
#define DEBUG_DUMP_WIFIRX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data

/* Debug output for example program */
#ifdef CONFIG_NETHUB_DEBUG
#define EXAMPLE_DEBUG       LOG_D//printf("[PORTTCPIPAP] " fmt, ##__VA_ARGS__)
#else
#define EXAMPLE_DEBUG(fmt, ...)
#endif

/* TCPIPAP interface input callback */
static nh_forward_result_t tcpipap_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    EXAMPLE_DEBUG("TCPIPAP INPUT received data: %s (simulated TCP/IP AP processing)\n", skb->data);

    nh_forward_result_t result;
    EXAMPLE_DEBUG("TCPIPAP INPUT received data: %s (simulated TCP/IP AP processing)\n", skb->data);

    result = nethub_process_input(skb, NHIF_TYPE_TCPIPSTA);
    if (result == NH_FORWARD_STOP) {
        EXAMPLE_DEBUG("Data consumed, forwarding stopped\n");
    } else if (result == NH_FORWARD_CONTINUE) {
        EXAMPLE_DEBUG("Data forwarding continues\n");
    } else {
        EXAMPLE_DEBUG("Forwarding result abnormal: %d\n", result);
    }

    return NH_FORWARD_CONTINUE;
}

/* TCPIPAP interface output callback */
static nh_forward_result_t tcpipap_output_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */

    EXAMPLE_DEBUG("TCPIPAP OUTPUT send data: %s\n", skb->data);
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
