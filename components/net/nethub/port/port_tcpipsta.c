/**
 * @file port_tcpipsta.c
 * @brief TCPIPSTA interface port adaptation layer implementation
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
#include "nethub_types.h"

#define DEBUG_DUMP_WIFITX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data
#define DEBUG_DUMP_WIFIRX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data

/* External NETHUB statistics variable declaration */
extern nethub_statistics_t g_nethub_stats;

/* Debug output for example program */
#ifdef CONFIG_NETHUB_DEBUG
#define EXAMPLE_DEBUG LOG_D//(fmt, ...) printf("[PORTTCPIPSTA] " fmt, ##__VA_ARGS__)
#else
#define EXAMPLE_DEBUG(fmt, ...)
#endif

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

    /* Statistics: UPLD data flows through tcpipsta interface */
    g_nethub_stats.upld_flow_through++;
#if DEBUG_DUMP_WIFIRX_ENABLE
    EXAMPLE_DEBUG("@%p:%d\n", skb->data, skb->len);
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
