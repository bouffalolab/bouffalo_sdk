#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// #include "usbd_core.h"
#include "bflb_mtimer.h"
#include <bflb_core.h>
#include "board.h"
#include <shell.h>

#include <assert.h>
#include <stdio.h>

//#include "usb.h"
#include "utils.h"

#include "bflb_mtimer.h"
#include "bflb_emac.h"

// #include "usbd_core.h"
// #include "usbd_cdc.h"
// #include "usbd_cdc_ecm.h"
#include "lwip/netif.h"
// #include "eth_phy.h"
// #include "ephy_general.h"

#include <wifi_pkt_hooks.h>
#include <net_pkt_filter.h>
#include <net_al.h>
#include <fhost.h>

#include <nxspi.h>
#include <nxspi_net.h>


spinet_t g_spinet;
char     s_buf[NXBD_DNLD_ITEMS*TX_PBUF_PAYLOAD_LEN+31] __attribute__((section("SHAREDRAM")));
nettrans_desc_t s_dnmsg_desc[NXBD_DNLD_ITEMS] __attribute__((section(".nocache_ram")));// up+dn

static void custom_free(struct pbuf *p)
{
    nettrans_desc_t *msg = (nettrans_desc_t *)p;

    //printf("++++++++++++++++++++custom_free:%p\r\n", msg);
    if (msg) {
        while (xQueueSend(g_spinet.dnfq, &msg, portMAX_DELAY) != pdPASS);
    }
}

int portwifi_eth_tx(nettrans_desc_t *msg, bool is_sta)
{
    struct pbuf *p = (struct pbuf *)msg;
    net_al_if_t *net_if;

    if (!p) {
        printf("alloc error.\r\n");
        pbuf_free(p);
    }

    if (is_sta) {
        net_if = fhost_env.vif[0].net_if;
    } else {
        net_if = fhost_env.vif[1].net_if;
    }
    if (!net_if) {
    	return -1;
    }


    if (netif_is_up((struct netif *)net_if) && (0 != fhost_tx_start(net_if, p, NULL, NULL))) {
        printf("tx error.\r\n");
        pbuf_free(p);
    }
}

int spinet_dn_task(void *arg)
{
    BaseType_t result;
    int ret;
    struct pbuf *p;

    while (1) {
        g_spinet.dbg_dntask_mode = 1;
        // get buf
        result = xQueueReceive(g_spinet.dnfq, &g_spinet.dnmsg, portMAX_DELAY);
        if (result != pdPASS) {
            printf("result:%d\r\n", result);
            return -2;  // Timeout or error in receiving the message
        }

        // init pbuf
        g_spinet.dnmsg->pbuf.custom_free_function = custom_free;
        p = pbuf_alloced_custom(PBUF_RAW_TX, TX_PBUF_FRAME_LEN,
                (PBUF_ALLOC_FLAG_DATA_CONTIGUOUS | PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP),
                &g_spinet.dnmsg->pbuf, g_spinet.dnmsg->payload_buf, TX_PBUF_PAYLOAD_LEN);

        // recv buf
        g_spinet.dbg_dntask_mode = 2;
#if 0
        ret = usbd_cdc_ecm_receive_packet((uint8_t *)g_spinet.dnmsg->pbuf.pbuf.payload, (TX_PBUF_FRAME_LEN));
        if (0 != ret) {
            //printf("usbd_cdc_ecm_receive ret:%d\r\n", ret);
            vTaskDelay(200);
            while (xQueueSend(g_spinet.dnfq, &g_spinet.dnmsg, portMAX_DELAY) != pdPASS);
            continue;
        }
#else
        // recv from spinet
        ret = nxspi_read(NXSPI_TYPE_NET, (uint8_t *)g_spinet.dnmsg->pbuf.pbuf.payload, (TX_PBUF_FRAME_LEN), -1);
        //printf("net ret:%d\r\n", ret);
        if (0 > ret) {
            //printf("usbd_cdc_ecm_receive ret:%d\r\n", ret);
            //vTaskDelay(200);
            while (xQueueSend(g_spinet.dnfq, &g_spinet.dnmsg, portMAX_DELAY) != pdPASS);
            continue;
        }
#if 0
        {
            int i;
            uint8_t *ptmp = g_spinet.dnmsg->pbuf.pbuf.payload;
            printf("NetRx[%d]:", ret);
            for (i = 0; i < ret; i++) {
                printf(" %02X", ptmp[i]);
            }
            printf("\r\n");
        }
#endif
#endif
        g_spinet.dbg_dntask_mode = 3;

        g_spinet.read_cnt++;
        g_spinet.read_bytes += ret;

        if (g_spinet.netstream == SPINET_NETSTREAM_TO_LOCAL) {
            pbuf_free(p);
            continue;
        }
        // update pbuf
        portwifi_eth_tx(g_spinet.dnmsg, 1);
        g_spinet.dnmsg = NULL;
    }
}

int spinet_up_task(void *arg)
{
    BaseType_t result;

    while (1) {
        result = xQueueReceive(g_spinet.upvq, &g_spinet.upmsg, portMAX_DELAY);
        if (result != pdPASS) {
            printf("result:%d\r\n", result);
            return -2;  // Timeout or error in receiving the message
        }

        nxspi_write(NXSPI_TYPE_NET, (uint8_t *)(uintptr_t)g_spinet.upmsg->payload, g_spinet.upmsg->len, -1);
        g_spinet.write_cnt++;
        g_spinet.write_bytes += g_spinet.upmsg->len;

        pbuf_free(g_spinet.upmsg);
    }

    return 0;
}

static int spinet_queue_init(void)
{
    int i;

    // reset global
    memset(&g_spinet, 0, sizeof(spinet_t));

    // init queue sem
    g_spinet.dnfq = xQueueCreate(NXBD_DNLD_ITEMS + 1, sizeof(nettrans_desc_t *));
    g_spinet.upvq = xQueueCreate(NXBD_UPLD_ITEMS + 1, sizeof(nettrans_desc_t *));
    if (!g_spinet.dnfq || !g_spinet.upvq) {
        printf("failed to create queue\r\n");
        return -1;
    }

    // init sem
    for (i = 0; i < NXBD_DNLD_ITEMS; i++) {
        nettrans_desc_t *msg;
#if 0
        struct pbuf_custom *p;

        p = &(dnmsg_desc[i].pbuf);
        p->custom_free_function = custom_free;
        p = pbuf_alloced_custom(PBUF_RAW_TX, TX_PBUF_FRAME_LEN,
                (PBUF_ALLOC_FLAG_DATA_CONTIGUOUS | PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP),
                &p->pbuf, p->payload_buf, TX_PBUF_PAYLOAD_LEN);
#endif
        s_dnmsg_desc[i].payload_buf = s_buf + TX_PBUF_PAYLOAD_LEN*i;
        msg = &(s_dnmsg_desc[i]);
        xQueueSend(g_spinet.dnfq, &msg, portMAX_DELAY);
    }
    for (i = 0; i < NXBD_UPLD_ITEMS; i++) {
    }

    return 0;
}

int dual_stack_peer_input(void *pkt, void *arg)
{
    static int s_cnt = 0;
    trans_desc_t msg;

    //struct usbwifi_rx_env_tag *env = &g_usbwifi.rx_env;
    struct pbuf *p = (struct pbuf *)pkt;
    while (xQueueSend(g_spinet.upvq, &p, portMAX_DELAY) != pdPASS);

    return 0;
}

static void *eth_input_hook(bool is_sta, void *pkt, void *arg)
{
    struct pbuf *p = (struct pbuf *)pkt;

    if (npf_is_8021X(p)) {
        // The packet is an 802.1X protocol packet.
        return p;
    }

    if (g_spinet.netstream == SPINET_NETSTREAM_TO_LOCAL) {
        g_spinet.local_pktcnt++;
        //printf("local handle:%d\r\n", g_spinet.local_pktcnt);
        return p;
    } else if (g_spinet.netstream == SPINET_NETSTREAM_TO_HOST) {
        g_spinet.host_pktcnt++;
        //printf("host  handle:%d\r\n", g_spinet.host_pktcnt);
        // XXX distinguish STA/AP
        int ret = dual_stack_peer_input(p, NULL);
        if (ret) {
            pbuf_free(p);
            return NULL;
        }
    } else {
        g_spinet.oth_pktcnt++;
        printf("oth    handle:%d\r\n", g_spinet.oth_pktcnt);
        pbuf_free(p);
    }

    return NULL;
}

static int spinet_wifi_init(void)
{
    bflb_pkt_eth_input_hook_register(eth_input_hook, NULL);//&ctx->rx_env);
    return 0;
}

void spinet_init(void)
{
    // init 0
    memset(&g_spinet, 0, sizeof(spinet_t));

    spinet_queue_init();
    // spinet_usb_init();
    spinet_wifi_init();

    xTaskCreate(spinet_dn_task, (char *)"nxspidn", 256, NULL, 25, NULL);
    xTaskCreate(spinet_up_task, (char *)"nxspiup", 256, NULL, 25, NULL);

    return;
}
