/* Includes ------------------------------------------------------------------*/
/* emac and phy */
#include "bflb_emac.h"

#include "eth_phy.h"
#include "ephy_general.h"
#include "ephy_lan8720.h"

#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#if LWIP_IPV6
#include "lwip/ethip6.h"
#endif
#include "lwip/netifapi.h"
#include "netif/etharp.h"

#include "apeth_bridge_emac.h"

/* os */
#include <FreeRTOS.h>
#include "queue.h"

#define DBG_TAG "LWIP_EMAC"
#include "log.h"

// --------------------------- define ---------------------------
/* Network interface name */
#define IFNAME0 'e'
#define IFNAME1 'x'

// --------------------------- type ---------------------------
typedef struct _emac_ops_t {
    QueueHandle_t               tx_pool_queue;
    QueueHandle_t               rx_process_queue;
    TaskHandle_t                emac_rx_handle;
    struct bflb_device_s        *emac0;
    eth_phy_ctrl_t              phy_ctrl;
    eth_phy_init_cfg_t          phy_cfg;
    struct bflb_emac_config_s   emac_cfgdef;
} emac_ops_t;

typedef struct _emac_debug_t {
    uint32_t lwip_input;
    uint32_t lwip_input_memlimit;
    uint32_t lwip_linkoutput;

    uint32_t tx_push_cnt;
    uint32_t tx_success_cnt;
    uint32_t tx_error_cnt;
    uint64_t tx_total_size;

    uint32_t rx_push_cnt;
    uint32_t rx_success_cnt;
    uint32_t rx_error_cnt;
    uint32_t rx_busy_cnt;
    uint64_t rx_total_size;
} emac_debug_t;

// --------------------------- val ---------------------------
/* tx buff def */
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) emac_tx_buff[LWIP_EMAC_TX_BUFF_CNT][LWIP_EMAC_TX_BUFF_SIZE];
/* rx buff def */
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) emac_rx_buff[LWIP_EMAC_RX_BUFF_CNT][LWIP_EMAC_RX_BUFF_SIZE];
/* global val */
emac_ops_t s_emac_ops = {
    .tx_pool_queue = NULL,
    .rx_process_queue = NULL,
    .emac_rx_handle = NULL,
    .emac0 = NULL,
    .phy_ctrl = {0},
    .phy_cfg = { 
        .speed_mode = EPHY_SPEED_MODE_AUTO_NEGOTIATION,
        .local_auto_negotiation_ability = EPHY_ABILITY_100M_TX | EPHY_ABILITY_100M_FULL_DUPLEX,
    },
    .emac_cfgdef = {
        .mac_addr = { 0x18, 0xB9, 0x05, 0x12, 0x34, 0x56 },
        .clk_internal_mode = false,
#if defined(BL616L) || defined(BL616D)
        .md_clk_div = 79,
#else
        .md_clk_div = 39,
#endif
        .min_frame_len = LWIP_EMAC_FRAME_SIZE_MIN,
        .max_frame_len = LWIP_EMAC_FRAME_SIZE_MAX,
    }
};
/* global debug */
emac_debug_t s_emac_dbg;

// --------------------------- function ---------------------------
static void __emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc);
static void ethernetif_input(void *argument);
static err_t emac_low_level_output(struct netif *netif, struct pbuf *p);

void eth_emac_hw_config(emac_config_t *cfg)
{
    if (cfg) {
        memcpy(s_emac_ops.emac_cfgdef.mac_addr, cfg->mac_addr, sizeof(cfg->mac_addr));
    }
}

static int emac_low_level_init(struct netif *netif)
{
    int ret;

    /* maximum transfer unit */
    netif->mtu = LWIP_EMAC_NETIF_MTU;

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    netif->hwaddr[0] = s_emac_ops.emac_cfgdef.mac_addr[0];
    netif->hwaddr[1] = s_emac_ops.emac_cfgdef.mac_addr[1];
    netif->hwaddr[2] = s_emac_ops.emac_cfgdef.mac_addr[2];
    netif->hwaddr[3] = s_emac_ops.emac_cfgdef.mac_addr[3];
    netif->hwaddr[4] = s_emac_ops.emac_cfgdef.mac_addr[4];
    netif->hwaddr[5] = s_emac_ops.emac_cfgdef.mac_addr[5];

    /* emac init */
    s_emac_ops.emac0 = bflb_device_get_by_name("emac0");
    if (s_emac_ops.emac0 == NULL) {
        LOG_E("device_get error\r\n");
        return -ERR_IF;
    }
    bflb_emac_init(s_emac_ops.emac0, &s_emac_ops.emac_cfgdef);
    bflb_emac_irq_attach(s_emac_ops.emac0, __emac_irq_cb, NULL);
    bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_SET_RX_PROMISCUOUS, true);

    /* scan eth_phy */
    ret = eth_phy_scan(&s_emac_ops.phy_ctrl, EPHY_ADDR_MIN, EPHY_ADDR_MAX);
    if (ret < 0) {
        return -ERR_IF;
    }
    /* eth_phy init */
    ret = eth_phy_init(&s_emac_ops.phy_ctrl, &s_emac_ops.phy_cfg);
    if (ret < 0) {
        return -ERR_IF;
    }

    /* LAN8720 Timing Adjustment: When in ref_clk input mode, invert the rx_clk. */
    if( (s_emac_ops.emac_cfgdef.clk_internal_mode == false) &&
        (s_emac_ops.phy_ctrl.phy_drv->phy_id == EPHY_LAN8720_ID)) {
        LOG_W("Invert rx_clk for LAN8720 Timing Adjustment.\r\n");
        bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_SET_MAC_RX_CLK_INVERT, true);
    }

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    #ifdef CONFIG_WIFI_ETH_BRIDGE
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
    #else
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    #endif
#if LWIP_IPV6
    netif->flags |= (NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6);
    netif->output_ip6 = ethip6_output;
#endif

    /* tx pool queue init */
    s_emac_ops.tx_pool_queue = xQueueCreate(LWIP_EMAC_TX_BUFF_CNT, sizeof(struct bflb_emac_trans_desc_s));

    /* rx process queue init */
    s_emac_ops.rx_process_queue = xQueueCreate(LWIP_EMAC_RX_BUFF_CNT, sizeof(struct bflb_emac_trans_desc_s));

    /* create the task that handles the ETH_MAC */
    LOG_I("[OS] Starting emac rx task...\r\n");
    xTaskCreate(ethernetif_input, (char *)"emac_rx_task", LWIP_EMAC_RX_STACK_SIZE, netif, osPriorityRealtime, &s_emac_ops.emac_rx_handle);

    netif_set_up(netif);

    return ERR_OK;
}

/* tx/rx done callback  */
static void __emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    switch (irq_event) {
        case EMAC_IRQ_EVENT_RX_BUSY:
            /* debug */
            s_emac_dbg.rx_busy_cnt++;
            if (!(s_emac_dbg.rx_busy_cnt%100)) {
                LOG_W("emac irq rx busy cnt:%d\r\n", s_emac_dbg.rx_busy_cnt);
            }
            break;

        case EMAC_IRQ_EVENT_RX_CTRL_FRAME:
            LOG_W("rx ctrl frame, drop.\r\n");
            trans_desc->attr_flag = 0;
            trans_desc->err_status = 0;
            bflb_emac_queue_rx_push(s_emac_ops.emac0, trans_desc);
            /* debug */
            s_emac_dbg.rx_push_cnt++;
            s_emac_dbg.rx_success_cnt++;
            s_emac_dbg.rx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_RX_ERR_FRAME:
            LOG_W("rx err frame sta %d, drop.\r\n", trans_desc->err_status);
            trans_desc->attr_flag = 0;
            trans_desc->err_status = 0;
            bflb_emac_queue_rx_push(s_emac_ops.emac0, trans_desc);
            /* debug */
            s_emac_dbg.rx_push_cnt++;
            s_emac_dbg.rx_error_cnt++;
            break;

        case EMAC_IRQ_EVENT_RX_FRAME:
            xQueueSendFromISR(s_emac_ops.rx_process_queue, trans_desc, &pxHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            /* debug */
            s_emac_dbg.rx_success_cnt++;
            s_emac_dbg.rx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_TX_FRAME:
            xQueueSendFromISR(s_emac_ops.tx_pool_queue, trans_desc, &pxHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            /* debug */
            s_emac_dbg.tx_success_cnt++;
            s_emac_dbg.tx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_TX_ERR_FRAME:
            xQueueSendFromISR(s_emac_ops.tx_pool_queue, trans_desc, &pxHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            /* debug */
            if (trans_desc->err_status & (~EMAC_TX_STA_ERR_CS)) {
                LOG_W("tx err sta:%d\r\n", trans_desc->err_status);
                s_emac_dbg.tx_error_cnt++;
            } else {
                s_emac_dbg.tx_success_cnt++;
                s_emac_dbg.tx_total_size += trans_desc->data_len;
            }
            break;

        default:
            break;
    }
}

/**
  * @brief This function should do the actual transmission of the packet. The packet is
  * contained in the pbuf that is passed to the function. This pbuf
  * might be chained.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
  * @return ERR_OK if the packet could be sent
  *         an err_t value if the packet couldn't be sent
  *
  * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
  *       strange results. You might consider waiting for space in the DMA queue
  *       to become available since the stack doesn't retry to send a packet
  *       dropped because of memory failure (except for the TCP timers).
  */
static err_t emac_low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    struct bflb_emac_trans_desc_s trans_desc;
    uint16_t byte_copy = 0;

    s_emac_dbg.lwip_linkoutput++;
    if (p->tot_len > LWIP_EMAC_TX_BUFF_SIZE) {
        LOG_E("tx tot_len size over\r\n");
        return ERR_BUF;
    }

    if (xQueueReceive(s_emac_ops.tx_pool_queue, &trans_desc, LWIP_EMAC_GET_TXBUF_TIMEOUT) == pdFALSE) {
        LOG_W("no tx buff\r\n");
        return ERR_MEM;
    }

    for (q = p; q != NULL; q = q->next) {
        /* size check */
        if (byte_copy + q->len > LWIP_EMAC_TX_BUFF_SIZE) {
            xQueueSend(s_emac_ops.tx_pool_queue, &trans_desc, 0);
            LOG_E("tx size over\r\n");
            return ERR_BUF;
        }
        /* data copy */
        memcpy(trans_desc.buff_addr + byte_copy, q->payload, q->len);
        byte_copy += q->len;
    }

    trans_desc.data_len = byte_copy;
    trans_desc.attr_flag = 0;
    trans_desc.err_status = 0;

    s_emac_dbg.tx_push_cnt++;
    bflb_emac_queue_tx_push(s_emac_ops.emac0, &trans_desc);

    return ERR_OK;
}

/**
  * @brief This function is the ethernetif_input task, it is processed when a packet
  * is ready to be read from the interface. It uses the function low_level_input()
  * that should handle the actual reception of bytes from the network
  * interface. Then the type of the received packet is determined and
  * the appropriate input function is called.
  *
  * @param netif the lwip network interface structure for this ethernetif
  */
static void ethernetif_input(void *argument)
{
    struct netif *netif = (struct netif *)argument;
    struct pbuf *p, *q;
    struct bflb_emac_trans_desc_s trans_desc;
    uint32_t byte_copy;

    while (1) {
        if (xQueueReceive(s_emac_ops.rx_process_queue, &trans_desc, portMAX_DELAY) == pdPASS) {
#ifdef CONFIG_WIFI_ETH_BRIDGE
            p = pbuf_alloc(PBUF_RAW_TX, trans_desc.data_len, PBUF_POOL);
#else
            p = pbuf_alloc(PBUF_RAW, trans_desc.data_len, PBUF_POOL);
#endif

            if (p != NULL) {
                #if 1
                byte_copy = 0;
                for (q = p; q != NULL; q = q->next) {
                    memcpy(q->payload, trans_desc.buff_addr + byte_copy, q->len);
                    byte_copy += q->len;
                }
                #else
                memcpy(p->payload, trans_desc.buff_addr, trans_desc.data_len);
                #endif
                /* gpt code */
                if (netif->input(p, netif) != ERR_OK) {
                    printf("netif->input err, free emac package\r\n");
                    pbuf_free(p);
                }  else {
                    s_emac_dbg.lwip_input++;
                }
            } else {
                s_emac_dbg.lwip_input_memlimit++;
                if (!(s_emac_dbg.lwip_input_memlimit%100)) {
                    printf("Drop the emac package_cnt:%d, because of pbuf alloc null.\r\n",
                        s_emac_dbg.lwip_input_memlimit);
                }
            }

            s_emac_dbg.rx_push_cnt += 1;
            bflb_emac_queue_rx_push(s_emac_ops.emac0, &trans_desc);
        }
    }
}

/**
  * @brief Should allocate a pbuf and transfer the bytes of the incoming
  * packet from the interface into the pbuf.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return a pbuf filled with the received packet (including MAC header)
  *         NULL on memory error
  */

/**
  * @brief Should be called at the beginning of the program to set up the
  * network interface. It calls the function low_level_init() to do the
  * actual setup of the hardware.
  *
  * This function should be passed as a parameter to netif_add().
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return ERR_OK if the loopif is initialized
  *         ERR_MEM if private data couldn't be allocated
  *         any other err_t on error
  */
err_t eth_emac_if_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = emac_low_level_output;

    /* for global debug */
    memset(&s_emac_dbg, 0, sizeof(s_emac_dbg));

    /* initialize the hardware */
    return emac_low_level_init(netif);
}

/**
  * @brief
  * @retval None
  */
void eth_link_state_update(struct netif *netif)
{
    static int speed_mode;
    static int link_sta;

    int sta = eth_phy_ctrl(&s_emac_ops.phy_ctrl, EPHY_CMD_GET_LINK_STA, 0);
    int speed = eth_phy_ctrl(&s_emac_ops.phy_ctrl, EPHY_CMD_GET_SPEED_MODE, 0);

    if (sta == EPHY_LINK_STA_UP && link_sta != EPHY_LINK_STA_UP) {
        LOG_W("Lwip Eth Emac LinkUp !!!\r\n");
        /* tx queue reinit */
        xQueueReset(s_emac_ops.tx_pool_queue);
        for (int i = 0; i < LWIP_EMAC_TX_BUFF_CNT; i++) {
            struct bflb_emac_trans_desc_s tx_desc = {
                .buff_addr = emac_tx_buff[i],
            };
            xQueueSend(s_emac_ops.tx_pool_queue, &tx_desc, portMAX_DELAY);
        }
        /* rx queue reinit */
        xQueueReset(s_emac_ops.rx_process_queue);
        for (int i = 0; i < LWIP_EMAC_RX_BUFF_CNT; i++) {
            struct bflb_emac_trans_desc_s rx_desc = {
                .buff_addr = emac_rx_buff[i],
            };
            bflb_emac_queue_rx_push(s_emac_ops.emac0, &rx_desc);
            s_emac_dbg.rx_push_cnt += 1;
        }

        /* enable full duplex mode */
        if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX || speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_SET_FULL_DUPLEX, true);
        } else {
            bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_SET_FULL_DUPLEX, false);
        }
        /* enable tx and rx */
        bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_SET_TX_EN, true);
        bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_SET_RX_EN, true);

        netif_set_link_up(netif);

    } else if (sta != EPHY_LINK_STA_UP && link_sta == EPHY_LINK_STA_UP) {
        LOG_W("Lwip Eth Emac LinkDown !!!\r\n");
        netif_set_link_down(netif);

        /* disable tx and rx, and clean tx/rx bd */
        bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_SET_TX_EN, false);
        bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_SET_RX_EN, false);
        bflb_emac_bd_ctrl_clean(s_emac_ops.emac0);

        speed_mode = 0;
    }
    link_sta = sta;

    if (link_sta != EPHY_LINK_STA_UP) {
        return;
    }

    if (speed != speed_mode) {
        speed_mode = speed;
        if (speed_mode == EPHY_SPEED_MODE_10M_HALF_DUPLEX) {
            LOG_I("eth_phy speed: 10M_HALF_DUPLEX\r\n");
        } else if (speed_mode == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
            LOG_I("eth_phy speed: 10M_FULL_DUPLEX\r\n");
        } else if (speed_mode == EPHY_SPEED_MODE_100M_HALF_DUPLEX) {
            LOG_I("eth_phy speed: 100M_HALF_DUPLEX\r\n");
        } else if (speed_mode == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
            LOG_I("eth_phy speed: 100M_FULL_DUPLEX\r\n");
        }
    }
}

#if LWIP_DHCP

#define DHCP_OFF              (uint8_t)0
#define DHCP_START            (uint8_t)1
#define DHCP_WAIT_ADDRESS     (uint8_t)2
#define DHCP_ADDRESS_ASSIGNED (uint8_t)3
#define DHCP_LINK_DOWN        (uint8_t)4

#define MAX_DHCP_TRIES        (4)

void dhcp_sta_update(struct netif *netif)
{
    static int dhcp_sta = DHCP_LINK_DOWN;
    static int link_sta;

    struct dhcp *dhcp;
    ip4_addr_t ip_zero;

    uint8_t iptxt[20];

    switch (dhcp_sta) {
        case DHCP_OFF:
            /* dhcp disable */
            break;

        case DHCP_START:
            /* dhcp start */
            ip4_addr_set_zero(&ip_zero);
            netifapi_netif_set_addr(netif, &ip_zero, &ip_zero, &ip_zero);
            netifapi_dhcp_start(netif);
            dhcp_sta = DHCP_WAIT_ADDRESS;
            LOG_I("State: Looking for DHCP server ...\r\n");
            break;

        case DHCP_WAIT_ADDRESS:
            /* check link sta */
            link_sta = netif_is_link_up(netif);
            if (!link_sta) {
                dhcp_sta = DHCP_LINK_DOWN;
                break;
            }
            /* wait and retry */
            if (dhcp_supplied_address(netif)) {
                dhcp_sta = DHCP_ADDRESS_ASSIGNED;
                snprintf((char *)iptxt, sizeof(iptxt), "%s", ip4addr_ntoa(netif_ip4_addr(netif)));
                LOG_I("IP address assigned by a DHCP server: %s\r\n", iptxt);
            } else {
                dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
                if (dhcp->tries > MAX_DHCP_TRIES) {
                    link_sta = netif_is_link_up(netif);
                    dhcp_sta = DHCP_LINK_DOWN;
                    LOG_E("DHCP Timeout !! \r\n");
                }
            }
            break;

        case DHCP_ADDRESS_ASSIGNED:
            link_sta = netif_is_link_up(netif);
            if (!link_sta) {
                dhcp_sta = DHCP_LINK_DOWN;
            }
            break;

        case DHCP_LINK_DOWN:
            if (netif_is_link_up(netif) && !link_sta) {
                dhcp_sta = DHCP_START;
            }
            break;

        default:
            LOG_E("dhcp sta err:%d\r\n", dhcp_sta);
            break;
    }
}

#endif

#ifdef CONFIG_SHELL
#include <shell.h>

int emac_status_cmd(int argc, char **argv)
{
    uint32_t tx_db_avail = bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_GET_TX_DB_AVAILABLE, 0);
    uint32_t rx_db_avail = bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_GET_RX_DB_AVAILABLE, 0);

    LOG_I("EmacTX irq TxCnt:%d(LwipOut-%d)\r\n",
            s_emac_dbg.tx_success_cnt,
            s_emac_dbg.lwip_linkoutput);// lwip task -> linkoutput
    LOG_I("           ErrCnt:%d, TotCnt:%lld, PushCnt:%d, Avail:%d, BdPtr:%d\r\n",
            s_emac_dbg.tx_error_cnt,
            s_emac_dbg.tx_total_size,
            s_emac_dbg.tx_push_cnt,
            tx_db_avail,
            bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_GET_TX_BD_PTR, 0));

    LOG_I("EmacRX thd RxCnt:%d(LwipInput-%d MemLimit:%d), Busy:%d\r\n",
            s_emac_dbg.rx_success_cnt,
            s_emac_dbg.lwip_input,
            s_emac_dbg.lwip_input_memlimit,
            s_emac_dbg.rx_busy_cnt);
    LOG_I("           ErrCnt:%d, TotCnt:%lld, PushCnt:%d, Avail:%d, BdPtr:%d\r\n",
            s_emac_dbg.rx_error_cnt,
            s_emac_dbg.rx_total_size,
            s_emac_dbg.rx_push_cnt,
            rx_db_avail,
            bflb_emac_feature_control(s_emac_ops.emac0, EMAC_CMD_GET_RX_BD_PTR, 0));

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(emac_status_cmd, emac_status, put netif emac info);

#endif
