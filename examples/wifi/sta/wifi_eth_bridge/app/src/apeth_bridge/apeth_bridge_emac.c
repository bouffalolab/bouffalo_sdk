/* Includes ------------------------------------------------------------------*/
/* emac and phy */
#include "bflb_emac.h"
#if defined(BL618DG)
#include "bflb_emac_v2.h"
#if !defined(CONFIG_PSRAM)
#error "BL618DG bridge RX buffers require CONFIG_PSRAM"
#endif
#endif

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
#include "lwip/stats.h"
#include "netif/etharp.h"

#include "apeth_bridge_emac.h"

#if LWIP_EMAC_RX_BUFF_CNT < 1 || (!defined(BL618DG) && LWIP_EMAC_RX_BUFF_CNT > EMAC_RX_BD_NUM_MAX)
#error "LWIP_EMAC_RX_BUFF_CNT must fit the EMAC RX descriptor ring"
#endif
#if LWIP_EMAC_TX_BUFF_CNT < 1 || LWIP_EMAC_TX_BUFF_CNT > EMAC_TX_BD_NUM_MAX
#error "LWIP_EMAC_TX_BUFF_CNT must fit the EMAC TX descriptor ring"
#endif

/* os */
#include <FreeRTOS.h>
#include "queue.h"
#include <stdlib.h>

#define DBG_TAG "LWIP_EMAC"
#include "log.h"

// --------------------------- define ---------------------------
/* Network interface name */
#define IFNAME0 'e'
#define IFNAME1 'x'

// --------------------------- type ---------------------------
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

typedef struct _emac_ops_t {
    QueueHandle_t               tx_pool_queue;
    QueueHandle_t               rx_process_queue;
    TaskHandle_t                emac_rx_handle;
    struct bflb_device_s        *dev;
    uint8_t                     port;
    bool                        initialized;
    char                        task_name[12];
    int                         speed_mode;
    int                         link_sta;
    int                         dhcp_state;
    int                         dhcp_link;
    emac_debug_t                debug;
    eth_phy_ctrl_t              phy_ctrl;
    eth_phy_init_cfg_t          phy_cfg;
    struct bflb_emac_config_s   emac_cfgdef;
} emac_ops_t;

// --------------------------- val ---------------------------
/* Both ports use identical counts/sizes, with separate ownership. */
#define EMAC_TX_BUFF_STRIDE ((LWIP_EMAC_TX_BUFF_SIZE + 31U) & ~31U)
/* tx buff def */
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) emac_tx_buff[LWIP_EMAC_PORT_COUNT][LWIP_EMAC_TX_BUFF_CNT][EMAC_TX_BUFF_STRIDE];
/* rx buff def */
#if defined(BL618DG)
/* Reserve through the PSRAM linker region, but only pass its non-cache alias
 * to DMA and the RX task. Pad the stride so every buffer starts on 32 bytes. */
#define EMAC_RX_BUFF_STRIDE ((LWIP_EMAC_RX_BUFF_SIZE + 31U) & ~31U)
static uint8_t ATTR_NOINIT_PSRAM_SECTION __ALIGNED(32) emac_rx_buff[LWIP_EMAC_PORT_COUNT][LWIP_EMAC_RX_BUFF_CNT][EMAC_RX_BUFF_STRIDE];
/* The demo supplies its own rings through the v2 API; public driver defaults
 * stay unchanged. Descriptors remain in non-cache OCRAM. */
static struct bflb_emac_v2_dma_desc_s ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32)
    emac_tx_bd[LWIP_EMAC_PORT_COUNT][EMAC_TX_BD_NUM_MAX],
    emac_rx_bd[LWIP_EMAC_PORT_COUNT][LWIP_EMAC_RX_BUFF_CNT];
_Static_assert(LWIP_EMAC_RX_BUFF_CNT <= UINT16_MAX, "RX ring count exceeds the v2 driver's index range");
_Static_assert(LWIP_EMAC_RX_BUFF_SIZE >= ETH_RX_BUFFER_SIZE, "RX buffer must hold the DMA receive size");
#else
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(32) emac_rx_buff[LWIP_EMAC_PORT_COUNT][LWIP_EMAC_RX_BUFF_CNT][LWIP_EMAC_RX_BUFF_SIZE];
#endif
static emac_ops_t emac_ports[LWIP_EMAC_PORT_COUNT];

static const eth_phy_init_cfg_t default_phy_cfg = {
    .speed_mode = EPHY_SPEED_MODE_AUTO_NEGOTIATION,
    .local_auto_negotiation_ability = EPHY_ABILITY_100M_TX | EPHY_ABILITY_100M_FULL_DUPLEX,
};
static const struct bflb_emac_config_s default_emac_cfg = {
    .clk_internal_mode = false,
#if defined(BL616CL) || defined(BL618DG)
    .md_clk_div = 79,
#else
    .md_clk_div = 39,
#endif
    .min_frame_len = LWIP_EMAC_FRAME_SIZE_MIN,
    .max_frame_len = LWIP_EMAC_FRAME_SIZE_MAX,
};

// --------------------------- function ---------------------------
static void __emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc);
static void ethernetif_input(void *argument);
static err_t emac_low_level_output(struct netif *netif, struct pbuf *p);

#if defined(BL618DG)
static int emac_bridge_bd_init(emac_ops_t *ctx)
{
    /* Called once after MAC init, before link-up or any buffers are queued.
     * The driver has already started TX DMA. Wait for it to stop before
     * changing ring bases, and consume the resulting stop interrupt before
     * restarting it, otherwise the driver's stop recovery can retrigger it. */
    struct bflb_device_s *dev = ctx->dev;
    uintptr_t flags = bflb_irq_save();
    uint32_t interrupt_mask = bflb_emac_v2_get_interrupt_mask(dev);
    bflb_emac_v2_disable_interrupt_all(dev);
    bflb_emac_v2_disable_dma_tx(dev);
    bflb_emac_v2_disable_dma_rx(dev);

    for (uint32_t timeout_us = 10000; ; timeout_us--) {
        uint32_t status = bflb_emac_v2_get_interrupt_status(dev);
        if ((status & (EMAC_V2_DMA_TX_STATE | EMAC_V2_DMA_RX_STATE)) == 0) {
            break;
        }
        if (timeout_us == 0) {
            bflb_irq_restore(flags);
            LOG_E("EMAC DMA stop timeout, status:0x%08lx\r\n", (unsigned long)status);
            return -1;
        }
        bflb_mtimer_delay_us(1);
    }

    int ret = bflb_emac_v2_dma_desc_init(dev, emac_tx_bd[ctx->port], NULL, EMAC_TX_BD_NUM_MAX,
                                       emac_rx_bd[ctx->port], NULL, LWIP_EMAC_RX_BUFF_CNT);
    /* No frames have been submitted yet; clear the startup DMA status. */
    bflb_emac_v2_clear_interrupt(dev, EMAC_V2_INT_ENABLE_CFG);
    if (ret == 0) {
        bflb_emac_v2_enable_dma_tx(dev);
        bflb_emac_v2_enable_interrupt(dev, interrupt_mask);
    }
    bflb_irq_restore(flags);
    return ret;
}
#endif

static void *emac_rx_buffer_get(emac_ops_t *ctx, unsigned int index)
{
#if defined(BL618DG)
    return bflb_get_no_cache_addr(emac_rx_buff[ctx->port][index]);
#else
    return emac_rx_buff[ctx->port][index];
#endif
}

static int emac_low_level_init(struct netif *netif)
{
    emac_ops_t *ctx = netif->state;
    int ret;
    err_t error = ERR_IF;

    /* maximum transfer unit */
    netif->mtu = LWIP_EMAC_NETIF_MTU;

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    netif->hwaddr[0] = ctx->emac_cfgdef.mac_addr[0];
    netif->hwaddr[1] = ctx->emac_cfgdef.mac_addr[1];
    netif->hwaddr[2] = ctx->emac_cfgdef.mac_addr[2];
    netif->hwaddr[3] = ctx->emac_cfgdef.mac_addr[3];
    netif->hwaddr[4] = ctx->emac_cfgdef.mac_addr[4];
    netif->hwaddr[5] = ctx->emac_cfgdef.mac_addr[5];

    /* emac init */
    ctx->dev = bsp_emac_get_device(ctx->port);
    if (ctx->dev == NULL) {
        LOG_E("device_get error\r\n");
        return ERR_IF;
    }
    ret = bflb_emac_init(ctx->dev, &ctx->emac_cfgdef);
    if (ret != 0) {
        return ERR_IF;
    }
#if defined(BL618DG)
    ret = emac_bridge_bd_init(ctx);
    if (ret != 0) {
        bflb_emac_deinit(ctx->dev);
        return ERR_IF;
    }
#endif
    ret = bflb_emac_irq_attach(ctx->dev, __emac_irq_cb, ctx);
    if (ret != 0) {
        goto init_failed;
    }
#if !defined(BL618DG)
    bflb_emac_feature_control(ctx->dev, EMAC_CMD_SET_RX_PROMISCUOUS, true);
#endif

    /* scan eth_phy */
    ctx->phy_ctrl.mac_mdio_dev = bsp_emac_get_device(ctx->port);
    if (ctx->phy_ctrl.mac_mdio_dev == NULL) {
        LOG_E("mdio device_get error\r\n");
        goto init_failed;
    }

    ret = eth_phy_scan(&ctx->phy_ctrl, EPHY_ADDR_MIN, EPHY_ADDR_MAX);
    if (ret < 0) {
        goto init_failed;
    }
    /* eth_phy init */
    ret = eth_phy_init(&ctx->phy_ctrl, &ctx->phy_cfg);
    if (ret < 0) {
        goto init_failed;
    }

    /* LAN8720 Timing Adjustment: When in ref_clk input mode, invert the rx_clk. */
    if( (ctx->emac_cfgdef.clk_internal_mode == false) &&
        (ctx->phy_ctrl.phy_drv->phy_id == EPHY_LAN8720_ID)) {
        LOG_W("Invert rx_clk for LAN8720 Timing Adjustment.\r\n");
        bflb_emac_feature_control(ctx->dev, EMAC_CMD_SET_MAC_RX_CLK_INVERT, true);
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
    ctx->tx_pool_queue = xQueueCreate(LWIP_EMAC_TX_BUFF_CNT, sizeof(struct bflb_emac_trans_desc_s));

    /* rx process queue init */
    ctx->rx_process_queue = xQueueCreate(LWIP_EMAC_RX_BUFF_CNT, sizeof(struct bflb_emac_trans_desc_s));
    if (ctx->tx_pool_queue == NULL || ctx->rx_process_queue == NULL) {
        error = ERR_MEM;
        goto init_failed;
    }

    /* Initialize buffer ownership once. Link changes must not rebuild a live
     * DMA ring or discard buffers still queued for the RX task. */
    for (int i = 0; i < LWIP_EMAC_TX_BUFF_CNT; i++) {
        struct bflb_emac_trans_desc_s tx_desc = {
            .buff_addr = emac_tx_buff[ctx->port][i],
        };
        xQueueSend(ctx->tx_pool_queue, &tx_desc, portMAX_DELAY);
    }
    for (int i = 0; i < LWIP_EMAC_RX_BUFF_CNT; i++) {
        struct bflb_emac_trans_desc_s rx_desc = {
            .buff_addr = emac_rx_buffer_get(ctx, i),
        };
        if (bflb_emac_queue_rx_push(ctx->dev, &rx_desc) != 0) {
            LOG_E("EMAC%u failed to submit RX buffer %d\r\n", ctx->port, i);
            goto init_failed;
        }
        ctx->debug.rx_push_cnt++;
    }

    /* create the task that handles the ETH_MAC */
    LOG_I("[OS] Starting emac rx task...\r\n");
    ret = xTaskCreate(ethernetif_input, ctx->task_name, LWIP_EMAC_RX_STACK_SIZE, netif, 27, &ctx->emac_rx_handle);
    if (ret != pdPASS) {
        error = ERR_MEM;
        goto init_failed;
    }

    ctx->initialized = true;
    netif_set_up(netif);
    return ERR_OK;

init_failed:
    bflb_emac_deinit(ctx->dev);
    if (ctx->tx_pool_queue) {
        vQueueDelete(ctx->tx_pool_queue);
        ctx->tx_pool_queue = NULL;
    }
    if (ctx->rx_process_queue) {
        vQueueDelete(ctx->rx_process_queue);
        ctx->rx_process_queue = NULL;
    }
    return error;
}

/* tx/rx done callback  */
static void __emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc)
{
    emac_ops_t *ctx = arg;
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    switch (irq_event) {
        case EMAC_IRQ_EVENT_RX_BUSY:
            /* debug */
            ctx->debug.rx_busy_cnt++;
            if (!(ctx->debug.rx_busy_cnt%100)) {
                LOG_W("emac irq rx busy cnt:%d\r\n", ctx->debug.rx_busy_cnt);
            }
            break;

        case EMAC_IRQ_EVENT_RX_CTRL_FRAME:
            LOG_W("rx ctrl frame, drop.\r\n");
            trans_desc->attr_flag = 0;
            trans_desc->err_status = 0;
            bflb_emac_queue_rx_push(ctx->dev, trans_desc);
            /* debug */
            ctx->debug.rx_push_cnt++;
            ctx->debug.rx_success_cnt++;
            ctx->debug.rx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_RX_ERR_FRAME:
            LOG_W("rx err frame sta %d, drop.\r\n", trans_desc->err_status);
            trans_desc->attr_flag = 0;
            trans_desc->err_status = 0;
            bflb_emac_queue_rx_push(ctx->dev, trans_desc);
            /* debug */
            ctx->debug.rx_push_cnt++;
            ctx->debug.rx_error_cnt++;
            break;

        case EMAC_IRQ_EVENT_RX_FRAME:
            if (xQueueSendFromISR(ctx->rx_process_queue, trans_desc, &pxHigherPriorityTaskWoken) != pdPASS) {
                /* Return dropped buffers so queue pressure cannot drain the RX ring. */
                bflb_emac_queue_rx_push(ctx->dev, trans_desc);
                ctx->debug.rx_push_cnt++;
                ctx->debug.rx_error_cnt++;
            } else {
                ctx->debug.rx_success_cnt++;
                ctx->debug.rx_total_size += trans_desc->data_len;
            }
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            break;

        case EMAC_IRQ_EVENT_TX_FRAME:
            xQueueSendFromISR(ctx->tx_pool_queue, trans_desc, &pxHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            /* debug */
            ctx->debug.tx_success_cnt++;
            ctx->debug.tx_total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_TX_ERR_FRAME:
            xQueueSendFromISR(ctx->tx_pool_queue, trans_desc, &pxHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            /* debug */
            if (trans_desc->err_status & (~EMAC_TX_STA_ERR_CS)) {
                LOG_W("tx err sta:%d\r\n", trans_desc->err_status);
                ctx->debug.tx_error_cnt++;
            } else {
                ctx->debug.tx_success_cnt++;
                ctx->debug.tx_total_size += trans_desc->data_len;
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
    emac_ops_t *ctx = netif->state;
    struct pbuf *q;
    struct bflb_emac_trans_desc_s trans_desc;
    uint16_t byte_copy = 0;

    ctx->debug.lwip_linkoutput++;
    if (p->tot_len > LWIP_EMAC_TX_BUFF_SIZE) {
        LOG_E("tx tot_len size over\r\n");
        return ERR_BUF;
    }

    if (xQueueReceive(ctx->tx_pool_queue, &trans_desc, LWIP_EMAC_GET_TXBUF_TIMEOUT) == pdFALSE) {
        LOG_W("no tx buff\r\n");
        return ERR_MEM;
    }

    for (q = p; q != NULL; q = q->next) {
        /* size check */
        if (byte_copy + q->len > LWIP_EMAC_TX_BUFF_SIZE) {
            xQueueSend(ctx->tx_pool_queue, &trans_desc, 0);
            LOG_E("tx size over\r\n");
            return ERR_BUF;
        }
        /* data copy */
        memcpy((uint8_t *)trans_desc.buff_addr + byte_copy, q->payload, q->len);
        byte_copy += q->len;
    }

    trans_desc.data_len = byte_copy;
    trans_desc.attr_flag = 0;
    trans_desc.err_status = 0;

    ctx->debug.tx_push_cnt++;
    if (bflb_emac_queue_tx_push(ctx->dev, &trans_desc) != 0) {
        xQueueSend(ctx->tx_pool_queue, &trans_desc, 0);
        ctx->debug.tx_error_cnt++;
        return ERR_IF;
    }

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
    emac_ops_t *ctx = netif->state;
    struct pbuf *p, *q;
    struct bflb_emac_trans_desc_s trans_desc;
    uint32_t byte_copy;

    while (1) {
        if (xQueueReceive(ctx->rx_process_queue, &trans_desc, portMAX_DELAY) == pdPASS) {
#ifdef CONFIG_WIFI_ETH_BRIDGE
            p = pbuf_alloc(PBUF_RAW_TX, trans_desc.data_len, PBUF_POOL);
#else
            p = pbuf_alloc(PBUF_RAW, trans_desc.data_len, PBUF_POOL);
#endif

            if (p != NULL) {
                #if 1
                byte_copy = 0;
                for (q = p; q != NULL; q = q->next) {
                    memcpy(q->payload, (uint8_t *)trans_desc.buff_addr + byte_copy, q->len);
                    byte_copy += q->len;
                }
                #else
                memcpy(p->payload, trans_desc.buff_addr, trans_desc.data_len);
                #endif
                /* The pbuf owns an independent copy. Return the DMA buffer
                 * before bridge/Wi-Fi input can retain or block on the pbuf. */
                ctx->debug.rx_push_cnt += 1;
                bflb_emac_queue_rx_push(ctx->dev, &trans_desc);
                if (netif->input(p, netif) != ERR_OK) {
                    printf("netif->input err, free emac package\r\n");
                    pbuf_free(p);
                }  else {
                    ctx->debug.lwip_input++;
                }
            } else {
                /* Keep the DMA ring supplied even when the lwIP pool is full. */
                ctx->debug.rx_push_cnt += 1;
                bflb_emac_queue_rx_push(ctx->dev, &trans_desc);
                ctx->debug.lwip_input_memlimit++;
                if (!(ctx->debug.lwip_input_memlimit%100)) {
                    printf("Drop the emac package_cnt:%d, because of pbuf alloc null.\r\n",
                        ctx->debug.lwip_input_memlimit);
                }
            }

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
static err_t eth_emac_if_init(struct netif *netif)
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

    /* initialize the hardware */
    return emac_low_level_init(netif);
}

err_t eth_emac_netif_add(struct netif *netif, uint8_t port, const emac_config_t *cfg)
{
    emac_ops_t *ctx;

    if (!netif || !cfg || port >= LWIP_EMAC_PORT_COUNT)
        return ERR_ARG;
    ctx = &emac_ports[port];
    if (ctx->initialized)
        return ERR_USE;
    memset(ctx, 0, sizeof(*ctx));
    ctx->port = port;
    ctx->phy_cfg = default_phy_cfg;
    ctx->emac_cfgdef = default_emac_cfg;
    ctx->dhcp_state = 4; /* DHCP_LINK_DOWN */
    memcpy(ctx->emac_cfgdef.mac_addr, cfg->mac_addr, sizeof(cfg->mac_addr));
    snprintf(ctx->task_name, sizeof(ctx->task_name), "emac%u_rx", port);
    return netifapi_netif_add(netif, NULL, NULL, NULL, ctx, eth_emac_if_init, tcpip_input);
}

/* Startup rollback only: no bridge owns this netif yet. */
void eth_emac_netif_remove(struct netif *netif)
{
    emac_ops_t *ctx = netif->state;

    if (!ctx || !ctx->initialized)
        return;
    bflb_emac_deinit(ctx->dev);
    vTaskDelete(ctx->emac_rx_handle);
    vQueueDelete(ctx->rx_process_queue);
    vQueueDelete(ctx->tx_pool_queue);
    netifapi_netif_remove(netif);
    ctx->initialized = false;
}

static void emac_link_mode_config(emac_ops_t *ctx, int speed)
{
    /* EMAC v2 defaults to 10M after reset; match the negotiated PHY speed
     * before enabling reception, including when the link is renegotiated. */
    if (speed == EPHY_SPEED_MODE_10M_HALF_DUPLEX || speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
#if EMAC_SPEED_10M_SUPPORT
        bflb_emac_feature_control(ctx->dev, EMAC_CMD_SET_SPEED_10M, 0);
#else
        LOG_E("10M speed mode not supported\r\n");
#endif
    } else {
        bflb_emac_feature_control(ctx->dev, EMAC_CMD_SET_SPEED_100M, 0);
    }

    if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX || speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
        bflb_emac_feature_control(ctx->dev, EMAC_CMD_SET_FULL_DUPLEX, true);
    } else {
        bflb_emac_feature_control(ctx->dev, EMAC_CMD_SET_FULL_DUPLEX, false);
    }

    if (speed == EPHY_SPEED_MODE_10M_HALF_DUPLEX) {
        LOG_I("eth_phy speed: 10M_HALF_DUPLEX\r\n");
    } else if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
        LOG_I("eth_phy speed: 10M_FULL_DUPLEX\r\n");
    } else if (speed == EPHY_SPEED_MODE_100M_HALF_DUPLEX) {
        LOG_I("eth_phy speed: 100M_HALF_DUPLEX\r\n");
    } else if (speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
        LOG_I("eth_phy speed: 100M_FULL_DUPLEX\r\n");
    }
}

void eth_link_state_update(struct netif *netif)
{
    emac_ops_t *ctx = netif->state;

    int sta = eth_phy_ctrl(&ctx->phy_ctrl, EPHY_CMD_GET_LINK_STA, 0);
    int speed = eth_phy_ctrl(&ctx->phy_ctrl, EPHY_CMD_GET_SPEED_MODE, 0);

    if (sta == EPHY_LINK_STA_UP &&
        (speed < EPHY_SPEED_MODE_10M_HALF_DUPLEX || speed > EPHY_SPEED_MODE_100M_FULL_DUPLEX)) {
        return; /* Wait for PHY negotiation to report a valid speed. */
    }
#if !defined(EMAC_SPEED_10M_SUPPORT) || !EMAC_SPEED_10M_SUPPORT
    if (sta == EPHY_LINK_STA_UP &&
        (speed == EPHY_SPEED_MODE_10M_HALF_DUPLEX || speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX)) {
        return;
    }
#endif
    if (sta != EPHY_LINK_STA_UP) {
        if (ctx->link_sta == EPHY_LINK_STA_UP) {
            LOG_W("EMAC%u LinkDown\r\n", ctx->port);
            netifapi_netif_set_link_down(netif);
        }
        /* Keep MAC/DMA and buffer ownership intact across cable unplug/replug. */
        ctx->link_sta = sta;
        ctx->speed_mode = 0;
        return;
    }

    if (ctx->link_sta != EPHY_LINK_STA_UP) {
        LOG_W("EMAC%u LinkUp\r\n", ctx->port);
        emac_link_mode_config(ctx, speed);
        ctx->speed_mode = speed;

        /* enable tx and rx */
        bflb_emac_feature_control(ctx->dev, EMAC_CMD_SET_TX_EN, true);
        bflb_emac_feature_control(ctx->dev, EMAC_CMD_SET_RX_EN, true);
        netifapi_netif_set_link_up(netif);
    } else if (speed != ctx->speed_mode) {
        netifapi_netif_set_link_down(netif);
        emac_link_mode_config(ctx, speed);
        ctx->speed_mode = speed;
        netifapi_netif_set_link_up(netif);
    }
    ctx->link_sta = sta;
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
    emac_ops_t *ctx = netif->state;

    struct dhcp *dhcp;
    ip4_addr_t ip_zero;

    uint8_t iptxt[20];

    switch (ctx->dhcp_state) {
        case DHCP_OFF:
            /* dhcp disable */
            break;

        case DHCP_START:
            /* dhcp start */
            ip4_addr_set_zero(&ip_zero);
            netifapi_netif_set_addr(netif, &ip_zero, &ip_zero, &ip_zero);
            netifapi_dhcp_start(netif);
            ctx->dhcp_state = DHCP_WAIT_ADDRESS;
            LOG_I("State: Looking for DHCP server ...\r\n");
            break;

        case DHCP_WAIT_ADDRESS:
            /* check link sta */
            ctx->dhcp_link = netif_is_link_up(netif);
            if (!ctx->dhcp_link) {
                ctx->dhcp_state = DHCP_LINK_DOWN;
                break;
            }
            /* wait and retry */
            if (dhcp_supplied_address(netif)) {
                ctx->dhcp_state = DHCP_ADDRESS_ASSIGNED;
                snprintf((char *)iptxt, sizeof(iptxt), "%s", ip4addr_ntoa(netif_ip4_addr(netif)));
                LOG_I("IP address assigned by a DHCP server: %s\r\n", iptxt);
            } else {
                dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
                if (dhcp->tries > MAX_DHCP_TRIES) {
                    ctx->dhcp_link = netif_is_link_up(netif);
                    ctx->dhcp_state = DHCP_LINK_DOWN;
                    LOG_E("DHCP Timeout !! \r\n");
                }
            }
            break;

        case DHCP_ADDRESS_ASSIGNED:
            ctx->dhcp_link = netif_is_link_up(netif);
            if (!ctx->dhcp_link) {
                ctx->dhcp_state = DHCP_LINK_DOWN;
            }
            break;

        case DHCP_LINK_DOWN:
            if (netif_is_link_up(netif) && !ctx->dhcp_link) {
                ctx->dhcp_state = DHCP_START;
            }
            break;

        default:
            LOG_E("dhcp sta err:%d\r\n", ctx->dhcp_state);
            break;
    }
}

#endif

#ifdef CONFIG_SHELL
#include <shell.h>

static void emac_port_status(emac_ops_t *ctx)
{
    LOG_I("EMAC%u initialized:%u link:%u RX:%u TX:%u\r\n", ctx->port, ctx->initialized,
          ctx->link_sta == EPHY_LINK_STA_UP, LWIP_EMAC_RX_BUFF_CNT, LWIP_EMAC_TX_BUFF_CNT);
    if (!ctx->initialized)
        return;
    LOG_I("TX nocache:%p buffers:%u size:%u stride:%u\r\n",
          emac_tx_buff[ctx->port][0], (unsigned)LWIP_EMAC_TX_BUFF_CNT,
          (unsigned)LWIP_EMAC_TX_BUFF_SIZE, (unsigned)EMAC_TX_BUFF_STRIDE);
#if defined(BL618DG)
    LOG_I("RX PSRAM nocache:%p buffers:%u size:%u stride:%u\r\n",
          emac_rx_buffer_get(ctx, 0), (unsigned)LWIP_EMAC_RX_BUFF_CNT,
          (unsigned)LWIP_EMAC_RX_BUFF_SIZE, (unsigned)EMAC_RX_BUFF_STRIDE);
#endif
    uint32_t tx_db_avail = bflb_emac_feature_control(ctx->dev, EMAC_CMD_GET_TX_DB_AVAILABLE, 0);
    uint32_t rx_db_avail = bflb_emac_feature_control(ctx->dev, EMAC_CMD_GET_RX_DB_AVAILABLE, 0);

#if MEMP_STATS
    const struct stats_mem *pool = lwip_stats.memp[MEMP_PBUF_POOL];
    if (pool != NULL) {
        LOG_I("PBUF_POOL total:%u used:%u free:%u max:%u alloc_fail:%u\r\n",
              (unsigned)pool->avail, (unsigned)pool->used,
              (unsigned)(pool->avail - pool->used), (unsigned)pool->max,
              (unsigned)pool->err);
    }
#endif

    LOG_I("EmacTX irq TxCnt:%d(LwipOut-%d)\r\n",
            ctx->debug.tx_success_cnt,
            ctx->debug.lwip_linkoutput);// lwip task -> linkoutput
    LOG_I("           ErrCnt:%d, TotCnt:%lld, PushCnt:%d, Avail:%d, BdPtr:%d\r\n",
            ctx->debug.tx_error_cnt,
            ctx->debug.tx_total_size,
            ctx->debug.tx_push_cnt,
            tx_db_avail,
            bflb_emac_feature_control(ctx->dev, EMAC_CMD_GET_TX_BD_PTR, 0));

    LOG_I("EmacRX thd RxCnt:%d(LwipInput-%d MemLimit:%d), Busy:%d\r\n",
            ctx->debug.rx_success_cnt,
            ctx->debug.lwip_input,
            ctx->debug.lwip_input_memlimit,
            ctx->debug.rx_busy_cnt);
    LOG_I("           ErrCnt:%d, TotCnt:%lld, PushCnt:%d, Avail:%d, BdPtr:%d\r\n",
            ctx->debug.rx_error_cnt,
            ctx->debug.rx_total_size,
            ctx->debug.rx_push_cnt,
            rx_db_avail,
            bflb_emac_feature_control(ctx->dev, EMAC_CMD_GET_RX_BD_PTR, 0));

}

int emac_status_cmd(int argc, char **argv)
{
    unsigned int first = 0, end = LWIP_EMAC_PORT_COUNT;

    if (argc == 2) {
        char *tail;
        unsigned long port = strtoul(argv[1], &tail, 10);
        if (argv[1][0] == '\0' || *tail || port >= LWIP_EMAC_PORT_COUNT)
            goto usage;
        first = port;
        end = first + 1;
    } else if (argc != 1) {
        goto usage;
    }
    for (unsigned int port = first; port < end; port++)
        emac_port_status(&emac_ports[port]);
    return 0;
usage:
    printf("usage: emac_status [port: 0..%u]\r\n", LWIP_EMAC_PORT_COUNT - 1);
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(emac_status_cmd, emac_status, put netif emac info);

#endif
