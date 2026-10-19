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

#include "lwip_netif_emac.h"

/* os */
#include <FreeRTOS.h>
#include "queue.h"
#include "task.h"

#include <stdbool.h>

#define DBG_TAG "LWIP_EMAC"
#include "log.h"

/* Network interface name */
#define IFNAME0 'e'
#define IFNAME1 'x'

void lwip_emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc);
static void ethernetif_input(void *argument);
static err_t emac_low_level_output(struct netif *netif, struct pbuf *p);

/**
 * @brief Initialize the EMAC hardware and DMA resources for one netif.
 *
 * @param[in,out] netif The lwIP network interface bound to an EMAC context.
 * @return ERR_OK on success, otherwise a lwIP error code.
 */
static int emac_low_level_init(struct netif *netif)
{
    lwip_emac_port_ctx_t *ctx = (lwip_emac_port_ctx_t *)netif->state;
    size_t buffer_stride = ctx->cfg.buffer_stride;
    size_t tx_buffer_count = ctx->cfg.tx_buffer_size / buffer_stride;
    size_t rx_buffer_count = ctx->cfg.rx_buffer_size / buffer_stride;
    int ret;

    /* maximum transfer unit */
    netif->mtu = ctx->cfg.mtu;

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    netif->hwaddr[0] = ctx->cfg.emac_cfg.mac_addr[0];
    netif->hwaddr[1] = ctx->cfg.emac_cfg.mac_addr[1];
    netif->hwaddr[2] = ctx->cfg.emac_cfg.mac_addr[2];
    netif->hwaddr[3] = ctx->cfg.emac_cfg.mac_addr[3];
    netif->hwaddr[4] = ctx->cfg.emac_cfg.mac_addr[4];
    netif->hwaddr[5] = ctx->cfg.emac_cfg.mac_addr[5];

    /* emac init */
    ctx->emac_dev = bsp_emac_get_device(ctx->cfg.port);
    if (ctx->emac_dev == NULL) {
        LOG_E("[EMAC%u] device_get error\r\n", ctx->cfg.port);
        return ERR_IF;
    }
    bflb_emac_init(ctx->emac_dev, &ctx->cfg.emac_cfg);
    bflb_emac_irq_attach(ctx->emac_dev, lwip_emac_irq_cb, ctx);
    // bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_SET_RX_PROMISCUOUS, false);

    /* scan eth_phy */
    ctx->phy_ctrl.mac_mdio_dev = bsp_emac_get_device(ctx->cfg.mdio_port);

    ret = eth_phy_scan(&ctx->phy_ctrl, ctx->cfg.phy_scan_start, ctx->cfg.phy_scan_end);
    if (ret < 0) {
        return ERR_IF;
    }
    /* eth_phy init */
    ret = eth_phy_init(&ctx->phy_ctrl, &ctx->cfg.phy_cfg);
    if (ret < 0) {
        return ERR_IF;
    }

    /* LAN8720 Timing Adjustment: When in ref_clk input mode, invert the rx_clk. */
    if ((ctx->cfg.emac_cfg.clk_internal_mode == false) && (ctx->phy_ctrl.phy_drv->phy_id == EPHY_LAN8720_ID)) {
        LOG_W("[EMAC%u] Invert rx_clk for LAN8720 Timing Adjustment.\r\n", ctx->cfg.port);
        bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_SET_MAC_RX_CLK_INVERT, true);
    }

    /* tx pool queue init */
    ctx->tx_pool_queue = xQueueCreate(tx_buffer_count, sizeof(struct bflb_emac_trans_desc_s));

    /* rx process queue init */
    ctx->rx_process_queue = xQueueCreate(rx_buffer_count, sizeof(struct bflb_emac_trans_desc_s));

    /* initialize tx buffer pool */
    for (size_t i = 0; i < tx_buffer_count; i++) {
        struct bflb_emac_trans_desc_s tx_desc = {
            .buff_addr = (uint8_t *)ctx->cfg.tx_buffer + i * buffer_stride,
        };
        xQueueSend(ctx->tx_pool_queue, &tx_desc, portMAX_DELAY);
    }

    /* initialize hardware rx descriptor queue */
    for (size_t i = 0; i < rx_buffer_count; i++) {
        struct bflb_emac_trans_desc_s rx_desc = {
            .buff_addr = (uint8_t *)ctx->cfg.rx_buffer + i * buffer_stride,
        };
        bflb_emac_queue_rx_push(ctx->emac_dev, &rx_desc);
        ctx->debug_info.rx.push_cnt += 1;
    }

    /* create the task that handles the ETH_MAC */
    LOG_I("[EMAC%u] [OS] Starting emac rx task...\r\n", ctx->cfg.port);
    xTaskCreate(ethernetif_input, ctx->task_name, ctx->cfg.rx_stack_size, netif, TCPIP_THREAD_PRIO,
                &ctx->rx_task_handle);

    /* Keep EMAC running across PHY link changes. Stopping the controller resets
     * its BD traversal position and requires a complete descriptor rebuild. */
    bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_SET_TX_EN, true);
    bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_SET_RX_EN, true);

    return ERR_OK;
}

/* tx/rx done callback  */
void lwip_emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc)
{
    lwip_emac_port_ctx_t *ctx = (lwip_emac_port_ctx_t *)arg;
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    switch (irq_event) {
        case EMAC_IRQ_EVENT_RX_BUSY:
            LOG_W("[EMAC%u] rx busy\r\n", ctx->cfg.port);
            /* debug */
            ctx->debug_info.rx.eamc_busy_cnt++;
            break;

        case EMAC_IRQ_EVENT_RX_CTRL_FRAME:
            LOG_W("[EMAC%u] rx ctrl frame, drop.\r\n", ctx->cfg.port);
            bflb_emac_queue_rx_push(ctx->emac_dev, trans_desc);
            /* debug */
            ctx->debug_info.rx.success_cnt++;
            ctx->debug_info.rx.total_size += trans_desc->data_len;
            break;

        case EMAC_IRQ_EVENT_RX_ERR_FRAME:
            LOG_W("[EMAC%u] rx err frame sta %d, drop.\r\n", ctx->cfg.port, trans_desc->err_status);
            bflb_emac_queue_rx_push(ctx->emac_dev, trans_desc);
            /* debug */
            ctx->debug_info.rx.error_cnt++;
            break;

        case EMAC_IRQ_EVENT_RX_FRAME:
            if (xQueueSendFromISR(ctx->rx_process_queue, trans_desc, &pxHigherPriorityTaskWoken) != pdTRUE) {
                /* rx queue full: return the descriptor straight back to the DMA pool
                 * instead of dropping it, otherwise the descriptor would leak. */
                bflb_emac_queue_rx_push(ctx->emac_dev, trans_desc);
            } else {
                /* debug */
                ctx->debug_info.rx.success_cnt++;
                ctx->debug_info.rx.total_size += trans_desc->data_len;
            }
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            break;

        case EMAC_IRQ_EVENT_TX_FRAME:
            if (xQueueSendFromISR(ctx->tx_pool_queue, trans_desc, &pxHigherPriorityTaskWoken) != pdTRUE) {
                /* tx pool full: re-queue the descriptor back so the TX buffer is not leaked. */
                bflb_emac_queue_tx_push(ctx->emac_dev, trans_desc);
            } else {
                /* debug */
                ctx->debug_info.tx.success_cnt++;
                ctx->debug_info.tx.total_size += trans_desc->data_len;
            }
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            break;

        case EMAC_IRQ_EVENT_TX_ERR_FRAME:
            xQueueSendFromISR(ctx->tx_pool_queue, trans_desc, &pxHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            /* debug */
            if (trans_desc->err_status & (~EMAC_TX_STA_ERR_CS)) {
                LOG_W("[EMAC%u] tx err sta:%d\r\n", ctx->cfg.port, trans_desc->err_status);
                ctx->debug_info.tx.error_cnt++;
            } else {
                ctx->debug_info.tx.success_cnt++;
                ctx->debug_info.tx.total_size += trans_desc->data_len;
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
    lwip_emac_port_ctx_t *ctx = (lwip_emac_port_ctx_t *)netif->state;
    struct bflb_emac_trans_desc_s trans_desc;
    uint16_t byte_copy;

    if (p->tot_len > ctx->cfg.emac_cfg.max_frame_len) {
        LOG_E("[EMAC%u] tx tot_len size over\r\n", ctx->cfg.port);
        return ERR_BUF;
    }

    if (xQueueReceive(ctx->tx_pool_queue, &trans_desc, ctx->cfg.tx_buf_timeout) != pdPASS) {
        LOG_W("[EMAC%u] no tx buff\r\n", ctx->cfg.port);
        return ERR_MEM;
    }

    byte_copy = pbuf_copy_partial(p, trans_desc.buff_addr, p->tot_len, 0);
    if (byte_copy != p->tot_len) {
        LOG_E("[EMAC%u] tx copy failed\r\n", ctx->cfg.port);
        xQueueSend(ctx->tx_pool_queue, &trans_desc, 0);
        return ERR_BUF;
    }

    trans_desc.data_len = byte_copy;
    trans_desc.attr_flag = 0;
    trans_desc.err_status = 0;
    ctx->debug_info.tx.push_cnt += 1;
    bflb_emac_queue_tx_push(ctx->emac_dev, &trans_desc);

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
    lwip_emac_port_ctx_t *ctx = (lwip_emac_port_ctx_t *)netif->state;
    struct pbuf *p;
    struct bflb_emac_trans_desc_s trans_desc;

    while (1) {
        xQueueReceive(ctx->rx_process_queue, &trans_desc, portMAX_DELAY);

#if PBUF_POOL_SIZE > 0
        p = pbuf_alloc(PBUF_RAW, trans_desc.data_len, PBUF_POOL);
#else
        p = pbuf_alloc(PBUF_RAW, trans_desc.data_len, PBUF_RAM);
#endif

        if (p != NULL) {
            err_t copy_error = pbuf_take(p, trans_desc.buff_addr, trans_desc.data_len);

            /* The DMA buffer is no longer needed after pbuf_take(). */
            ctx->debug_info.rx.push_cnt += 1;
            bflb_emac_queue_rx_push(ctx->emac_dev, &trans_desc);

            if (copy_error != ERR_OK || netif->input(p, netif) != ERR_OK) {
                pbuf_free(p);
            }
        } else {
            ctx->debug_info.rx.pbuf_busy_cnt++;
            ctx->debug_info.rx.push_cnt += 1;
            bflb_emac_queue_rx_push(ctx->emac_dev, &trans_desc);
        }
    }
}

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
    lwip_emac_port_ctx_t *ctx;
    int ret;

    LWIP_ASSERT("netif != NULL", (netif != NULL));
    ctx = (lwip_emac_port_ctx_t *)netif->state;
    LWIP_ASSERT("netif->state != NULL", (ctx != NULL));

    ctx->speed_mode = 0;
    ctx->link_sta = 0;

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = ctx->hostname;
#endif
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = emac_low_level_output;
    /* device capabilities */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
    netif->flags |= (NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6);
#endif

    /* initialize the hardware */
    ret = emac_low_level_init(netif);
    if (ret < 0) {
        return ret;
    }

    /* set netif up */
    netif_set_up(netif);
    return ERR_OK;
}

/**
 * @brief Configure the EMAC speed and duplex mode for the negotiated PHY mode.
 *
 * @param[in] ctx EMAC port context.
 * @param[in] speed Negotiated PHY speed and duplex mode.
 */
static void emac_link_mode_config(lwip_emac_port_ctx_t *ctx, int speed)
{
    /* 10M/100M speed mode */
    if (speed == EPHY_SPEED_MODE_10M_HALF_DUPLEX || speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
#if (defined(EMAC_SPEED_10M_SUPPORT) && EMAC_SPEED_10M_SUPPORT)
        bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_SET_SPEED_10M, 0);
#else
        LOG_E("[EMAC%u] 10M speed mode not support, please check EMAC_SPEED_10M_SUPPORT config !!!\r\n", ctx->cfg.port);
#endif
    } else {
        bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_SET_SPEED_100M, 0);
    }

    /* full/half duplex mode */
    if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX || speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
        bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_SET_FULL_DUPLEX, true);
    } else {
        bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_SET_FULL_DUPLEX, false);
    }

    if (speed == EPHY_SPEED_MODE_10M_HALF_DUPLEX) {
        LOG_I("[EMAC%u] eth_phy speed: 10M_HALF_DUPLEX\r\n", ctx->cfg.port);
    } else if (speed == EPHY_SPEED_MODE_10M_FULL_DUPLEX) {
        LOG_I("[EMAC%u] eth_phy speed: 10M_FULL_DUPLEX\r\n", ctx->cfg.port);
    } else if (speed == EPHY_SPEED_MODE_100M_HALF_DUPLEX) {
        LOG_I("[EMAC%u] eth_phy speed: 100M_HALF_DUPLEX\r\n", ctx->cfg.port);
    } else if (speed == EPHY_SPEED_MODE_100M_FULL_DUPLEX) {
        LOG_I("[EMAC%u] eth_phy speed: 100M_FULL_DUPLEX\r\n", ctx->cfg.port);
    }
}

/**
 * @brief Synchronize the lwIP link state with the Ethernet PHY.
 *
 * The EMAC controller remains enabled across link changes because stopping it
 * resets the hardware BD traversal position. TX/RX queues and descriptors are
 * initialized once in emac_low_level_init() and are not rebuilt here.
 *
 * @param[in] netif lwIP network interface associated with the EMAC device.
 */
void eth_link_state_update(struct netif *netif)
{
    lwip_emac_port_ctx_t *ctx = (lwip_emac_port_ctx_t *)netif->state;
    int sta = eth_phy_ctrl(&ctx->phy_ctrl, EPHY_CMD_GET_LINK_STA, 0);
    int speed = eth_phy_ctrl(&ctx->phy_ctrl, EPHY_CMD_GET_SPEED_MODE, 0);

    if (sta != EPHY_LINK_STA_UP) {
        if (ctx->link_sta == EPHY_LINK_STA_UP) {
            LOG_W("[EMAC%u] Lwip Eth Emac LinkDown !!!\r\n", ctx->cfg.port);
            netifapi_netif_set_link_down(netif);
        }
        ctx->link_sta = 0;
        ctx->speed_mode = 0;
        return;
    }

    if (ctx->link_sta != EPHY_LINK_STA_UP) {
        LOG_W("[EMAC%u] Lwip Eth Emac LinkUp !!!\r\n", ctx->cfg.port);
        emac_link_mode_config(ctx, speed);
        ctx->speed_mode = speed;
        ctx->link_sta = sta;
        netifapi_netif_set_link_up(netif);
        return;
    }

    if (ctx->speed_mode != speed) {
        LOG_W("[EMAC%u] Lwip Eth Emac Speed Mode Has Changed !!!\r\n", ctx->cfg.port);
        netifapi_netif_set_link_down(netif);
        emac_link_mode_config(ctx, speed);
        ctx->speed_mode = speed;
        netifapi_netif_set_link_up(netif);
    }
}

#ifdef CONFIG_SHELL
#include <shell.h>

int lwip_emac_info_cmd(int argc, char **argv)
{
    struct netif *netif;

    NETIF_FOREACH(netif)
    {
        lwip_emac_port_ctx_t *ctx;
        uint32_t tx_db_avail;
        uint32_t rx_db_avail;

        if (netif->linkoutput != emac_low_level_output) {
            continue;
        }

        ctx = (lwip_emac_port_ctx_t *)netif->state;
        tx_db_avail = bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_GET_TX_DB_AVAILABLE, 0);
        rx_db_avail = bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_GET_RX_DB_AVAILABLE, 0);

        LOG_I("[EMAC%u] TX: success cnt:%d, error cnt:%d, total size:%lldByte\r\n", ctx->cfg.port,
              ctx->debug_info.tx.success_cnt, ctx->debug_info.tx.error_cnt, ctx->debug_info.tx.total_size);
        LOG_I("[EMAC%u]     push_cnt:%d, tx_db available:%d, tx_bd_ptr:%d\r\n", ctx->cfg.port,
              ctx->debug_info.tx.push_cnt, tx_db_avail,
              bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_GET_TX_BD_PTR, 0));

        LOG_I("[EMAC%u] RX: success cnt:%d, error cnt:%d, total size:%lldByte\r\n", ctx->cfg.port,
              ctx->debug_info.rx.success_cnt, ctx->debug_info.rx.error_cnt, ctx->debug_info.rx.total_size);
        LOG_I("[EMAC%u]     push_cnt:%d, rx_db available:%d, rx_bd_ptr:%d\r\n", ctx->cfg.port,
              ctx->debug_info.rx.push_cnt, rx_db_avail,
              bflb_emac_feature_control(ctx->emac_dev, EMAC_CMD_GET_RX_BD_PTR, 0));
        LOG_I("[EMAC%u]     emac busy cnt:%d, pbuf busy cnt:%d\r\n", ctx->cfg.port, ctx->debug_info.rx.eamc_busy_cnt,
              ctx->debug_info.rx.pbuf_busy_cnt);
        LOG_RI("\r\n");
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(lwip_emac_info_cmd, lwip_emac_info, put netif emac info);
#endif
