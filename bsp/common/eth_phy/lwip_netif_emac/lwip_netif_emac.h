#ifndef __LWIP_NETIF_EMAC_H__
#define __LWIP_NETIF_EMAC_H__

#include <stddef.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include "queue.h"
#include "task.h"

#include "bflb_emac.h"
#include "eth_phy.h"
#include "lwip/err.h"
#include "lwip/netif.h"

/* Exported types ------------------------------------------------------------*/
/** @brief Configuration and application-owned DMA buffer pools for one port. */
typedef struct {
    uint8_t port;              /**< EMAC data port (RMII TX/RX). */
    uint8_t mdio_port;         /**< EMAC port used for MDIO (PHY register) access. */
    uint8_t phy_scan_start;    /**< First PHY address to scan. */
    uint8_t phy_scan_end;      /**< Last PHY address to scan. */
    uint16_t mtu;              /**< lwIP network interface MTU. */
    uint16_t rx_stack_size;    /**< RX task stack depth. */
    TickType_t tx_buf_timeout; /**< TX buffer wait timeout. */
    size_t buffer_stride;      /**< Byte stride between adjacent DMA buffers. */
    /* phy cfg */
    eth_phy_init_cfg_t phy_cfg; /**< PHY initialization configuration. */
    /* emac cfg */
    struct bflb_emac_config_s emac_cfg; /**< EMAC initialization configuration. */
    void *tx_buffer;                    /**< TX DMA buffer pool address. */
    size_t tx_buffer_size;              /**< Total TX DMA buffer pool size. */
    void *rx_buffer;                    /**< RX DMA buffer pool address. */
    size_t rx_buffer_size;              /**< Total RX DMA buffer pool size. */
} lwip_emac_port_cfg_t;

/** @brief EMAC transfer statistics for one port. */
struct lwip_emac_debug_info_s {
    struct {
        uint32_t push_cnt;
        uint32_t success_cnt;
        uint32_t error_cnt;
        uint64_t total_size;
    } tx;
    struct {
        uint32_t push_cnt;
        uint32_t success_cnt;
        uint32_t error_cnt;
        uint32_t eamc_busy_cnt;
        uint32_t pbuf_busy_cnt;
        uint64_t total_size;
    } rx;
};

/** @brief Application-owned runtime context for one EMAC port. */
typedef struct lwip_emac_port_ctx_s {
    lwip_emac_port_cfg_t cfg;
    /* emac */
    struct bflb_device_s *emac_dev;
    /* eth phy */
    eth_phy_ctrl_t phy_ctrl;
    /* tx pool queue */
    QueueHandle_t tx_pool_queue;
    /* rx process queue */
    QueueHandle_t rx_process_queue;
    /* rx task  */
    TaskHandle_t rx_task_handle;
    bool mac_enabled;
    bool phy_powered;
    int speed_mode;
    int link_sta;
    char task_name[10]; /**< Application-configured RX task name. */
    char hostname[8];   /**< Application-configured lwIP hostname. */
    /* emac debug info */
    volatile struct lwip_emac_debug_info_s debug_info;
} lwip_emac_port_ctx_t;

/**
 * @brief Initialize an lwIP netif using its bound EMAC context.
 *
 * This function is intended to be passed as the init callback of
 * netifapi_netif_add(). Before it is called, netif->state must point to an
 * application-owned lwip_emac_port_ctx_t whose cfg and DMA buffer pools remain
 * valid for the lifetime of the network interface.
 *
 * @param[in,out] netif lwIP network interface.
 * @retval ERR_OK The network interface and its EMAC were initialized.
 * @retval ERR_IF EMAC device, PHY scan, or PHY initialization failed.
 */
err_t eth_emac_if_init(struct netif *netif);

/**
 * @brief Update one netif link state from its PHY.
 *
 * The netif must already have completed eth_emac_if_init(). Call this function
 * from task context; the current implementation may use netifapi functions.
 *
 * @param[in,out] netif lwIP network interface.
 */
void eth_link_state_update(struct netif *netif);

/**
 * @brief Shared EMAC IRQ event dispatcher.
 *
 * Routes DMA events to the per-port RX/TX queues and keeps the driver debug
 * counters. Used both by eth_emac_if_init() and by the application-level port
 * management module, so the RX/TX descriptor accounting lives in one place.
 *
 * @param[in,out] arg lwip_emac_port_ctx_t bound to the port.
 * @param[in] irq_event EMAC IRQ event, one of the EMAC_IRQ_EVENT_* values.
 * @param[in] trans_desc DMA descriptor associated with the event.
 */
void lwip_emac_irq_cb(void *arg, uint32_t irq_event, struct bflb_emac_trans_desc_s *trans_desc);

#endif
