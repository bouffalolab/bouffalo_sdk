/**
 * @file hd_eth_netdev.c
 * @brief Host Driver Ethernet Network Device Driver
 * @author mlwang
 * @date 2025-07-17
 * @details This module implements a virtual Ethernet network device that
 *          operates over SDIO transport. It provides standard Linux network
 *          interface functionality including packet transmission, reception,
 *          statistics, and network stack integration with message-based
 *          communication protocol and comprehensive flow control management.
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/rtnetlink.h>

#include "hd_eth_netdev.h"
#include "hd_msg_ctrl.h"
#include "hd_debugfs.h"

#define ETH_DRV_NAME "hd_eth"

static int hd_eth_netdev_daemon_thread(void *data);

/**
 * @brief Update transmit statistics
 * @param[in] priv Private data structure
 * @param[in] packets Packet count: positive=success, negative=error, zero=dropped
 * @param[in] bytes Byte count for successful packets (ignored for errors/drops)
 * @retval None
 * @details Centralized transmit statistics update function
 */
static inline void hd_eth_update_tx_stats(struct hd_sdio_eth_priv *priv, int packets, uint32_t bytes)
{
    spin_lock(&priv->stats_lock);
    if (packets > 0) {
        priv->stats.tx_packets += packets;
        priv->stats.tx_bytes += bytes;
    } else if (packets < 0) {
        priv->stats.tx_errors += (-packets);
    } else {
        priv->stats.tx_dropped++;
    }
    spin_unlock(&priv->stats_lock);
}

/**
 * @brief Update receive statistics
 * @param[in] priv Private data structure
 * @param[in] packets Packet count: positive=success, negative=error
 * @param[in] bytes Byte count for successful packets (ignored for errors)
 * @retval None
 * @details Centralized receive statistics update function
 */
static inline void hd_eth_update_rx_stats(struct hd_sdio_eth_priv *priv, int packets, uint32_t bytes)
{
    spin_lock(&priv->stats_lock);
    if (packets > 0) {
        priv->stats.rx_packets += packets;
        priv->stats.rx_bytes += bytes;
    } else if (packets < 0) {
        priv->stats.rx_errors += (-packets);
    }
    spin_unlock(&priv->stats_lock);
}

/**
 * @brief Receive network data and pass to network device
 * @param[in] netdev Network device structure
 * @param[in] skb Socket buffer containing received data
 * @retval 0 Success
 * @details This function processes received network packets by removing
 *          message headers and forwarding them to the Linux network stack.
 *          Updates device statistics and handles protocol recognition.
 */
static int hd_eth_upld_recv_net_data(struct net_device *netdev, struct sk_buff *skb)
{
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);

    /* Remove message header from socket buffer */
    skb_pull(skb, sizeof(struct hd_eth_msg_packt));

    if (skb->len == 0) {
        ETH_NETDEV_INFO(priv, "received empty skb after header removal\n");
        dev_kfree_skb_any(skb);
        return 0;
    }

    /* Prepare socket buffer for network stack */
    skb->dev = netdev;
    skb->protocol = eth_type_trans(skb, netdev);

    /* Update receive statistics */
    hd_eth_update_rx_stats(priv, 1, skb->len);

    /* Forward to network stack */
    netif_rx(skb);

    return 0;
}

/**
 * @brief Update the network queue status based on flow control credits
 * @param[in] netdev Network device structure
 * @retval 0 Success or early exit
 * @retval -1 Exceeded max cycles, forced exit
 * @details This function checks the available flow control credits and
 *          starts or stops the network queue accordingly. It prevents
 *          infinite loops by limiting the number of cycles.
 */
static int hd_eth_queue_status_update(struct net_device *netdev)
{
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);
    unsigned long flags;
    int cycle_count = 0;
    uint8_t credit_limit_cnt;

    bool netif_stopped;
    bool queue_flag;

    while (cycle_count < 100) { // Limit cycles to prevent infinite loop

        spin_lock_irqsave(&priv->credit_lock, flags);

        netif_stopped = netif_queue_stopped(netdev);

        /* Wake up network queue if we have credits available */
        if ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) > 0 && netif_stopped) {
            queue_flag = true;
        } else if ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) <= 0 && !netif_stopped) {
            queue_flag = false;
        } else {
            spin_unlock_irqrestore(&priv->credit_lock, flags);
            return 0;
        }
        credit_limit_cnt = priv->credit_limit_cnt;
        spin_unlock_irqrestore(&priv->credit_lock, flags);

        if (queue_flag) {
            ETH_NETDEV_DBG(priv, "Credit available, cycle:%d, waking up queue: %d\n", cycle_count, credit_limit_cnt);
            netif_wake_queue(netdev);
        } else {
            ETH_NETDEV_DBG(priv, "Credit limit reached, cycle:%d, stopping queue: %d\n", cycle_count, credit_limit_cnt);
            netif_stop_queue(netdev);
        }

        cycle_count++;
    }

    /* This point should theoretically never be reached */
    ETH_NETDEV_ERR(priv, "Queue status update exceeded max cycles, forcing exit\n");
    return -1;
}

/**
 * @brief Update the flow control credit limit
 * @param[in] netdev Network device structure
 * @param[in] credit_limit_new New credit limit value
 * @retval 0 Always returns 0
 * @details This function updates the credit limit for flow control and
 *          wakes up the network queue if new credits are available.
 */
static int hd_eth_credit_limit_update(struct net_device *netdev, uint8_t credit_limit_new)
{
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);
    unsigned long flags;
    bool should_wake = false;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if ((int8_t)(credit_limit_new - priv->credit_limit_cnt) > 0) {
        /* Wake up network queue if we have credits available */
        if ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) <= 0 &&
            (int8_t)(credit_limit_new - priv->credit_consumed_cnt) > 0) {
            should_wake = true;
        }
        ETH_NETDEV_DBG(priv, "Credit limit update: %d -> %d\n", priv->credit_limit_cnt, credit_limit_new);
        priv->credit_limit_cnt = credit_limit_new;
    } else {
        ETH_NETDEV_INFO(priv, "Credit limit update error: current %d, new %d\n", priv->credit_limit_cnt, credit_limit_new);
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    if (should_wake) {
        hd_eth_queue_status_update(netdev);
    }

    return 0;
}

/**
 * @brief Consume a flow control credit for packet transmission
 * @param[in] netdev Network device structure
 * @retval 0 Success, credit consumed
 * @retval -1 No credits available
 * @details This function decrements the consumed credit count and stops
 *          the network queue if no credits remain.
 */
static int hd_eth_credit_get(struct net_device *netdev)
{
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);
    unsigned long flags;
    bool should_stop = false;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) <= 0) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return -1;
    }

    priv->credit_consumed_cnt++;

    if ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) <= 0) {
        should_stop = true;
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    if (should_stop) {
        hd_eth_queue_status_update(netdev);
    }

    return 0;
}

/**
 * @brief Return a flow control credit after failed transmission
 * @param[in] netdev Network device structure
 * @retval 0 Always returns 0
 * @details This function returns a consumed credit and wakes up the
 *          network queue if credits become available.
 */
static int hd_eth_credit_put(struct net_device *netdev)
{
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);
    unsigned long flags;
    uint8_t credit_consumed_old;
    bool should_wake = false;

    spin_lock_irqsave(&priv->credit_lock, flags);

    credit_consumed_old = priv->credit_consumed_cnt;
    priv->credit_consumed_cnt -= 1;

    if (((int8_t)(credit_consumed_old - priv->credit_consumed_cnt) <= 0) &&
        ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) > 0)) {
        should_wake = true;
        ETH_NETDEV_DBG(priv, "Flow control credit returned, wakeup consumed: %d \n", priv->credit_consumed_cnt);
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    if (should_wake) {
        hd_eth_queue_status_update(netdev);
    }

    return 0;
}

/**
 * @brief Socket buffer receive callback for message controller
 * @param[in] skb Socket buffer containing received data
 * @param[in] success Reception success flag (always true for received messages)
 * @param[in] arg Network device argument
 * @retval Always returns 0
 * @details This callback is registered with the message controller
 *          to handle received Ethernet packets from the device.
 *          Data validation and trimming are already done in msg_ctrl.
 */
static int hd_eth_upld_recv_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct net_device *netdev = (struct net_device *)arg;
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);
    struct hd_eth_msg_packt *eth_msg_packt;

    /* msg_ctrl has already validated packet size and trimmed excess data */
    eth_msg_packt = (struct hd_eth_msg_packt *)skb->data;

    /* Fast path for data packets in RUN state to avoid waking up the kthread */
    if (priv->eth_status == HD_ETH_HSTA_DEVICE_RUN &&
        (eth_msg_packt->flag == HD_ETH_FLAG_UPLD_DATA || eth_msg_packt->flag == HD_ETH_FLAG_CREDIT_UPDATE)) {
        /* Update flow control, must be protected with lock */
        if (eth_msg_packt->credit_update_flag) {
            hd_eth_credit_limit_update(netdev, eth_msg_packt->credit_limit_cnt);
        }

        if (eth_msg_packt->flag == HD_ETH_FLAG_UPLD_DATA) {
            hd_eth_upld_recv_net_data(netdev, skb);
        } else {
            dev_kfree_skb_any(skb);
        }

        return 0;
    }

    /* Forward to thread for processing */
    skb_queue_tail(&priv->thread_skb_head, skb);
    wake_up_interruptible(&priv->waitq);

    return 0;
}

/**
 * @brief Socket buffer send completion callback for message controller
 * @param[in] skb Socket buffer that was transmitted
 * @param[in] success Transmission success flag
 * @param[in] arg Network device argument
 * @retval 0 SKB processed and freed by callback
 * @details This callback is called when a dnld (download/send) operation
 *          completes. It handles transmission statistics and flow control
 *          with precise success/failure tracking, then frees the SKB.
 */
static int hd_eth_dnld_send_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct net_device *netdev = (struct net_device *)arg;
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);
    struct hd_eth_msg_packt *eth_msg_packt;
    uint32_t data_len;

    /* Extract data length from message packet */
    eth_msg_packt = (struct hd_eth_msg_packt *)skb->data;
    /* Calculate actual network data length (excluding eth message header fields) */
    data_len = eth_msg_packt->msg_packt.len - (sizeof(struct hd_eth_msg_packt) - sizeof(struct hd_msg_packt));

    if (eth_msg_packt->flag == HD_ETH_FLAG_DNLD_DATA) {
        if (success) {
            /* Update success statistics */
            hd_eth_update_tx_stats(priv, 1, data_len);
        } else {
            /* Transmission failed, return credit and update error statistics */
            hd_eth_credit_put(netdev);
            hd_eth_update_tx_stats(priv, -1, 0);
            ETH_NETDEV_ERR(priv, "failed to send DNLD_DATA skb, returning credit\n");
        }
    }

    dev_kfree_skb_any(skb);
    return 0;
}

/**
 * @brief Open network device (ndo_open callback)
 * @param[in] netdev Network device structure
 * @retval 0 Success
 * @retval <0 Error code
 * @details This function initializes the network device when brought up.
 *          It sets up private data structures, creates daemon thread,
 *          registers message callbacks, and prepares the device for
 *          network operations with proper state initialization.
 */
static int hd_eth_netdev_open(struct net_device *netdev)
{
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);

    ETH_NETDEV_INFO(priv, "device %s opened\n", netdev->name);

    netif_carrier_off(netdev);
    netif_stop_queue(netdev);

    /* Initialize private data */
    priv->eth_status = HD_ETH_HSTA_RESET;
    priv->credit_consumed_cnt = 0;
    priv->credit_limit_cnt = 0;
    priv->thread_condition = false;
    skb_queue_head_init(&priv->thread_skb_head);
    init_waitqueue_head(&priv->waitq);

    spin_lock(&priv->stats_lock);
    memset(&priv->stats, 0, sizeof(priv->stats));
    spin_unlock(&priv->stats_lock);

    /* Create network daemon thread */
    priv->daemon_thread = kthread_create(hd_eth_netdev_daemon_thread, netdev, "%s_daemon", netdev->name);
    if (IS_ERR(priv->daemon_thread)) {
        ETH_NETDEV_ERR(priv, "failed to create daemon thread: %ld\n", PTR_ERR(priv->daemon_thread));
        return PTR_ERR(priv->daemon_thread);
    }

    hd_msg_cb_register(priv->msg_ctrl, priv->msg_tag, hd_eth_upld_recv_cb, netdev, hd_eth_dnld_send_cb, netdev);

    wake_up_process(priv->daemon_thread);
    return 0;
}

/**
 * @brief Stop network device (ndo_stop callback)
 * @param[in] netdev Network device structure
 * @retval 0 Success
 * @details This function stops the network device when brought down.
 *          It disables carrier and queue, stops daemon thread,
 *          unregisters message callbacks, cleans up resources,
 *          and resets device state for proper shutdown.
 */
static int hd_eth_netdev_stop(struct net_device *netdev)
{
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);

    netif_carrier_off(netdev);
    netif_stop_queue(netdev);

    ETH_NETDEV_INFO(priv, "device %s stopped\n", netdev->name);

    /* Step 1: First unregister message controller callbacks to stop new messages */
    hd_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);

    /* Step 2: Stop daemon thread after callbacks are unregistered */
    if (priv->daemon_thread) {
        priv->thread_condition = true;
        wake_up_interruptible(&priv->waitq);
        kthread_stop(priv->daemon_thread);
        priv->daemon_thread = NULL;
    }

    /* Step 3: Clean up skb queue after thread is stopped */
    skb_queue_purge(&priv->thread_skb_head);

    return 0;
}

/**
 * @brief Transmit network packet (ndo_start_xmit callback)
 * @param[in] skb Socket buffer containing packet data
 * @param[in] netdev Network device structure
 * @retval NETDEV_TX_OK Success or handled error
 * @retval NETDEV_TX_BUSY Flow control triggered, retry needed
 * @details This function transmits network packets through the message
 *          controller. It handles flow control, adds message headers,
 *          validates packet size, manages credit consumption, and
 *          updates transmission statistics with proper error handling.
 */
static netdev_tx_t hd_eth_netdev_start_xmit(struct sk_buff *skb, struct net_device *netdev)
{
    int ret;
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);
    struct hd_eth_msg_packt *eth_msg_packt;

    /* 1. Perform all skb-related checks and preparation work first */
    if (priv->eth_status != HD_ETH_HSTA_DEVICE_RUN) {
        ETH_NETDEV_ERR(priv, "Device not in RUN state, dropping packet\n");
        hd_eth_update_tx_stats(priv, 0, 0); /* 0 = dropped */
        dev_kfree_skb_any(skb);
        return NETDEV_TX_OK;
    }

    /* Check skb length */
    if (skb->len + sizeof(struct hd_eth_msg_packt) > priv->msg_ctrl->msg_dnld_max_size) {
        ETH_NETDEV_ERR(priv, "skb length %d exceeds max dnld size %d\n", skb->len, priv->msg_ctrl->msg_dnld_max_size);
        hd_eth_update_tx_stats(priv, 0, 0); /* 0 = dropped */
        dev_kfree_skb_any(skb);
        return NETDEV_TX_OK;
    }

    /* Check header space, reallocate if insufficient */
    if (skb_headroom(skb) < sizeof(struct hd_eth_msg_packt)) {
        struct sk_buff *new_skb = skb_realloc_headroom(skb, sizeof(struct hd_eth_msg_packt));
        if (!new_skb) {
            ETH_NETDEV_ERR(priv, "Failed to realloc headroom for tx packet\n");
            hd_eth_update_tx_stats(priv, 0, 0); /* 0 = dropped */
            dev_kfree_skb_any(skb);
            return NETDEV_TX_OK;
        }
        dev_kfree_skb_any(skb);
        skb = new_skb;
    }

    /* 2. After all checks pass, perform flow control check and send (atomic operation) */
    if (hd_eth_credit_get(netdev) < 0) {
        /* Flow control triggered, stop queue and return busy */
        ETH_NETDEV_DBG(priv, "Flow control credit exhausted\n");
        hd_eth_queue_status_update(netdev);
        return NETDEV_TX_BUSY;
    }

    /* 3. Prepare message header and send */
    eth_msg_packt = (struct hd_eth_msg_packt *)skb_push(skb, sizeof(struct hd_eth_msg_packt));
    eth_msg_packt->msg_packt.tag = priv->msg_tag;
    eth_msg_packt->msg_packt.sub_tag = 0;
    eth_msg_packt->msg_packt.len = skb->len - sizeof(struct hd_msg_packt); /* Length is pure data packet length */
    eth_msg_packt->flag = HD_ETH_FLAG_DNLD_DATA;
    eth_msg_packt->credit_update_flag = false;

    ret = hd_msg_ctrl_send(priv->msg_ctrl, skb);
    if (ret < 0) {
        /* Message sending failed, roll back credit consumption */
        hd_eth_credit_put(netdev);

        ETH_NETDEV_ERR(priv, "failed to send skb: %d\n", ret);
        dev_kfree_skb_any(skb);
        /* Update error statistics */
        hd_eth_update_tx_stats(priv, -1, 0); /* -1 = error */
    }

    return NETDEV_TX_OK;
}

/**
 * @brief Get network device statistics (ndo_get_stats64 callback)
 * @param[in] netdev Network device structure
 * @param[out] stats Statistics structure to populate
 * @details This function retrieves comprehensive network device statistics
 *          including packet counts, byte counts, error rates, and
 *          transmission metrics with proper locking for thread safety.
 */
static void hd_eth_netdev_get_stats64(struct net_device *netdev, struct rtnl_link_stats64 *stats)
{
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);

    spin_lock(&priv->stats_lock);
    memcpy(stats, &priv->stats, sizeof(struct rtnl_link_stats64));
    spin_unlock(&priv->stats_lock);
}

/* Network device operations function set */
static const struct net_device_ops hd_eth_netdev_ops = {
    .ndo_open = hd_eth_netdev_open,
    .ndo_stop = hd_eth_netdev_stop,
    .ndo_start_xmit = hd_eth_netdev_start_xmit,
    .ndo_get_stats64 = hd_eth_netdev_get_stats64,
};

/**
 * @brief Send command packet to the device
 * @param[in] priv Private data structure
 * @param[in] flag Command flag to set in the packet
 * @retval 0 Success
 * @retval <0 Error code
 * @details This function prepares and sends a command packet to the device
 *          using the message controller. It allocates a socket buffer,
 *          fills in the command details, and sends it through the message
 *          control interface.
 */
static int hd_eth_send_cmd_packet(struct hd_sdio_eth_priv *priv, uint8_t flag)
{
    int ret;
    struct sk_buff *skb_send;
    struct hd_eth_msg_packt *eth_msg_packt;

    skb_send = dev_alloc_skb(sizeof(struct hd_eth_msg_packt));
    if (!skb_send) {
        ETH_NETDEV_ERR(priv, "failed to allocate skb for cmd\n");
        return -ENOMEM;
    }

    eth_msg_packt = (struct hd_eth_msg_packt *)skb_put(skb_send, sizeof(struct hd_eth_msg_packt));
    eth_msg_packt->msg_packt.tag = priv->msg_tag;
    eth_msg_packt->msg_packt.sub_tag = 0;
    eth_msg_packt->msg_packt.len = sizeof(struct hd_eth_msg_packt) - sizeof(struct hd_msg_packt);
    eth_msg_packt->flag = flag;

    ret = hd_msg_ctrl_send(priv->msg_ctrl, skb_send);
    if (ret < 0) {
        dev_kfree_skb_any(skb_send);
        return ret;
    }

    return 0;
}

/**
 * @brief Network device daemon thread for state machine management
 * @param[in] data Network device structure pointer
 * @retval 0 Thread completed successfully
 * @details This daemon thread manages the Ethernet device state machine,
 *          processes control messages, handles device initialization
 *          handshake, manages flow control updates, and maintains
 *          communication protocol with the remote device.
 */
static int hd_eth_netdev_daemon_thread(void *data)
{
    int ret;
    unsigned long flags;
    struct net_device *netdev = (struct net_device *)data;
    struct hd_sdio_eth_priv *priv = netdev_priv(netdev);
    struct sk_buff *skb_recv;
    struct hd_eth_msg_packt *eth_msg_packt;

    ret = hd_eth_send_cmd_packet(priv, HD_ETH_FLAG_HOST_RESET);
    if (ret < 0) {
        ETH_NETDEV_ERR(priv, "failed to send HOST_RESET command: %d\n", ret);
        goto exit;
    }

    /* Initialize state machine */
    priv->eth_status = HD_ETH_HSTA_RESET;

    while (1) {
        /* Suspend and wait for wake-up for various reasons */
        wait_event_interruptible(priv->waitq,
                                 !skb_queue_empty(&priv->thread_skb_head) ||
                                     priv->thread_condition ||
                                     kthread_should_stop() ||
                                     priv->eth_status == HD_ETH_HSTA_RESET);

        ETH_NETDEV_DBG(priv, "Daemon thread running, current status: %d\n", priv->eth_status);

        if (kthread_should_stop()) {
            break;
        }

        /* Try to get skb for processing */
        skb_recv = skb_dequeue(&priv->thread_skb_head);
        if (skb_recv) {
            eth_msg_packt = (struct hd_eth_msg_packt *)skb_recv->data;
        } else {
            eth_msg_packt = NULL;
        }

        /* Anytime receiving DEVICE_RESET, need to return to initial state */
        if (eth_msg_packt && eth_msg_packt->flag == HD_ETH_FLAG_DEVICE_RESET) {
            ETH_NETDEV_ERR(priv, "received DEVICE_RESET in state %d, returning to HSTA_RESET\n", priv->eth_status);

            /* If already in HOST_READY state, no need to repeat operations */
            if (priv->eth_status != HD_ETH_HSTA_RESET) {
                netif_carrier_off(netdev);
                netif_stop_queue(netdev);
                priv->eth_status = HD_ETH_HSTA_RESET;
            }

            /* Release skb and restart loop */
            dev_kfree_skb_any(skb_recv);
            skb_recv = NULL;
            continue;
        }

        /* State machine */
        switch (priv->eth_status) {
            case HD_ETH_HSTA_RESET:
                if (eth_msg_packt != NULL) {
                    break;
                }
                /* Reset state, send HOST_READY packet to indicate HOST is ready */
                ret = hd_eth_send_cmd_packet(priv, HD_ETH_FLAG_HOST_READY);
                if (ret < 0) {
                    priv->eth_status = HD_ETH_HSTA_ERROR;
                } else {
                    priv->eth_status = HD_ETH_HSTA_HOST_READY;
                }
                break;

            case HD_ETH_HSTA_HOST_READY:
                /* Host ready state, waiting for DEVICE_READY, get MAC address */
                if (eth_msg_packt == NULL || eth_msg_packt->flag != HD_ETH_FLAG_DEVICE_READY) {
                    break;
                }

                /* Check if packet length is sufficient to contain MAC address */
                if (eth_msg_packt->msg_packt.len < ETH_ALEN + (sizeof(struct hd_eth_msg_packt) - sizeof(struct hd_msg_packt))) {
                    ETH_NETDEV_ERR(priv, "device ready with invalid MAC address length\n");
                    priv->eth_status = HD_ETH_HSTA_ERROR;
                    break;
                }

                /* Get MAC address from DEVICE_READY packet and set it */
                rtnl_lock();
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
                eth_hw_addr_set(netdev, eth_msg_packt->data);
#else
                memcpy(netdev->dev_addr, eth_msg_packt->data, ETH_ALEN);
#endif
                rtnl_unlock();
                ETH_NETDEV_INFO(priv, "Device MAC address set to %pM\n", netdev->dev_addr);

                priv->eth_status = HD_ETH_HSTA_DEVICE_READY;
                ETH_NETDEV_INFO(priv, "device ready\n");
                break;

            case HD_ETH_HSTA_DEVICE_READY:
                /* Device ready state, waiting for DEVICE_START, get initial flow control credit, enable network transmission */
                if (eth_msg_packt == NULL || eth_msg_packt->flag != HD_ETH_FLAG_DEVICE_START) {
                    break;
                }

                if (eth_msg_packt->credit_update_flag == false || eth_msg_packt->credit_limit_cnt == 0) {
                    ETH_NETDEV_ERR(priv, "device start without credit update flag\n");
                    priv->eth_status = HD_ETH_HSTA_ERROR;
                    break;
                }

                spin_lock_irqsave(&priv->credit_lock, flags);
                priv->credit_limit_cnt = eth_msg_packt->credit_limit_cnt;
                priv->credit_consumed_cnt = 0;
                spin_unlock_irqrestore(&priv->credit_lock, flags);

                netif_carrier_on(netdev);
                netif_wake_queue(netdev);

                priv->eth_status = HD_ETH_HSTA_DEVICE_RUN;
                ETH_NETDEV_INFO(priv, "Device is running with credit limit %d\n", priv->credit_limit_cnt);
                break;

            case HD_ETH_HSTA_DEVICE_RUN:
                /* Normal working state */
                if (eth_msg_packt == NULL) {
                    break;
                } else if (eth_msg_packt->flag == HD_ETH_FLAG_UPLD_DATA) {
                    /* Received uplink data packet, parse flow control data, then submit to network device */
                    if (eth_msg_packt->credit_update_flag) {
                        hd_eth_credit_limit_update(netdev, eth_msg_packt->credit_limit_cnt);
                    }
                    hd_eth_upld_recv_net_data(netdev, skb_recv);
                    skb_recv = NULL; /* skb is now owned by netif_rx */
                    break;

                } else if (eth_msg_packt->flag == HD_ETH_FLAG_CREDIT_UPDATE) {
                    /* Received flow control update packet, update flow control information */
                    if (eth_msg_packt->credit_update_flag) {
                        hd_eth_credit_limit_update(netdev, eth_msg_packt->credit_limit_cnt);
                    } else {
                        ETH_NETDEV_ERR(priv, "credit update without flag\n");
                    }
                    break;

                } else if (eth_msg_packt->flag == HD_ETH_FLAG_DEVICE_STOP) {
                    /* Received device stop packet, switch to DEVICE_READY state */
                    priv->eth_status = HD_ETH_HSTA_RESET;

                    netif_carrier_off(netdev);
                    netif_stop_queue(netdev);

                    ETH_NETDEV_INFO(priv, "device stopped, switching to reset state\n");
                    break;
                }
                break;

            default:
                /* Abnormal state, wait for thread shutdown */
                break;
        }

        /* Release skb_recv */
        if (skb_recv) {
            dev_kfree_skb_any(skb_recv);
            skb_recv = NULL;
        }
    }

exit:

    if (priv->eth_status != HD_ETH_HSTA_ERROR) {
        ret = hd_eth_send_cmd_packet(priv, HD_ETH_FLAG_HOST_STOP);
    } else {
        ret = hd_eth_send_cmd_packet(priv, HD_ETH_FLAG_HOST_RESET);
    }
    if (ret < 0) {
        ETH_NETDEV_ERR(priv, "failed to send stop packet: %d\n", ret);
    }

    while (!kthread_should_stop()) {
        wait_event_interruptible(priv->waitq,
                                 kthread_should_stop() || priv->thread_condition);
    }

    ETH_NETDEV_INFO(priv, "daemon thread exiting, status: %d\n", priv->eth_status);
    return 0;
}

/**
 * @brief Initialize Ethernet network device over SDIO
 * @param[in] msg_ctrl Message controller structure
 * @param[in] msg_tag Message tag for routing
 * @retval Network device pointer on success
 * @retval NULL on failure
 * @details This function creates and initializes a virtual Ethernet network
 *          device that operates over SDIO transport. It allocates device
 *          structure, configures network operations, sets MTU and headroom,
 *          and registers with the Linux network subsystem.
 */
struct net_device *hd_sdio_eth_netdev_init(struct hd_msg_ctrl *msg_ctrl, uint8_t msg_tag)
{
    int ret;
    struct net_device *netdev = NULL;
    struct hd_sdio_eth_priv *priv = NULL;

    /* Network device initialization, use alloc_netdev and ether_setup, this is the modern way */
    netdev = alloc_netdev(sizeof(struct hd_sdio_eth_priv), "hd_eth%d", NET_NAME_UNKNOWN, ether_setup);
    if (!netdev) {
        pr_err("failed to allocate net_device\n");
        return NULL;
    }

    priv = netdev_priv(netdev);
    priv->netdev = netdev;
    priv->msg_ctrl = msg_ctrl;
    priv->msg_tag = msg_tag;

    netdev->netdev_ops = &hd_eth_netdev_ops;
    /* skb header needs to reserve extra space */
    netdev->needed_headroom = ALIGN(ETH_HLEN + sizeof(struct hd_eth_msg_packt), L1_CACHE_BYTES);

    /* MAC address will be provided by peer device after receiving DEVICE_READY, not set here */
    /* eth_hw_addr_random(netdev); */

    /* The spin lock must be initialized before register_netdev */
    spin_lock_init(&priv->stats_lock);
    spin_lock_init(&priv->credit_lock);

    ret = register_netdev(netdev);
    if (ret < 0) {
        pr_err("failed to register net_device: %d\n", ret);
        free_netdev(netdev);
        return NULL;
    }

    return netdev;
}

/**
 * @brief Deinitialize and cleanup Ethernet network device
 * @param[in] netdev Network device structure to cleanup
 * @details This function properly shuts down and cleans up the virtual
 *          Ethernet network device. It unregisters from the Linux network
 *          subsystem, releases allocated resources, and frees the device
 *          structure with proper null pointer checking.
 */
void hd_sdio_eth_netdev_deinit(struct net_device *netdev)
{
    struct hd_sdio_eth_priv *priv;

    if (netdev == NULL) {
        return;
    }
    priv = netdev_priv(netdev);

    ETH_NETDEV_INFO(priv, "deinitializing net_device %s\n", netdev->name);

    unregister_netdev(netdev);
    free_netdev(netdev);
}