/**
 * @file hd_netlink.c
 * @brief Host Driver Netlink Virtualization Implementation
 * @details Netlink interface implementation with credit-based flow control,
 *          provides userspace-kernel communication over Netlink socket with
 *          SDIO/USB transport backend.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/sched/signal.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

#include "hd_netlink.h"
#include "hd_msg_ctrl.h"
#include "hd_debugfs.h"

#define NETLINK_DBG(priv, fmt, ...) pr_debug("Netlink: " fmt, ##__VA_ARGS__)
#define NETLINK_INFO(priv, fmt, ...) pr_info("Netlink: " fmt, ##__VA_ARGS__)
#define NETLINK_ERR(priv, fmt, ...) pr_err("Netlink: " fmt, ##__VA_ARGS__)

/* Flow control debug log (set to 1 to enable, 0 to disable) */
#define NETLINK_STREAMCTRL_LOG 0

#if NETLINK_STREAMCTRL_LOG
#define NETLINK_STREAMCTRL(priv, fmt, ...) pr_info("Netlink: [FLOW] " fmt, ##__VA_ARGS__)
#else
#define NETLINK_STREAMCTRL(priv, fmt, ...) no_printk(fmt, ##__VA_ARGS__)
#endif

/* Global reference to netlink private structure for callback access */
static struct hd_netlink_priv *g_netlink_priv;

/* Sysfs device and class */
static struct device *netlink_device;
static struct class *netlink_class;

/* Forward declarations */
static int hd_netlink_credit_put(struct hd_netlink_priv *priv);

/**
 * @brief Sysfs show function for tx_packets
 */
static ssize_t tx_packets_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct hd_netlink_priv *priv = dev_get_drvdata(dev);
    if (!priv)
        return -ENODEV;
    return sprintf(buf, "%u\n", atomic_read(&priv->tx_packets));
}

/**
 * @brief Sysfs show function for tx_dropped
 */
static ssize_t tx_dropped_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct hd_netlink_priv *priv = dev_get_drvdata(dev);
    if (!priv)
        return -ENODEV;
    return sprintf(buf, "%u\n", atomic_read(&priv->tx_dropped));
}

/**
 * @brief Sysfs show function for rx_packets
 */
static ssize_t rx_packets_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct hd_netlink_priv *priv = dev_get_drvdata(dev);
    if (!priv)
        return -ENODEV;
    return sprintf(buf, "%u\n", atomic_read(&priv->rx_packets));
}

/**
 * @brief Sysfs show function for rx_dropped
 */
static ssize_t rx_dropped_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct hd_netlink_priv *priv = dev_get_drvdata(dev);
    if (!priv)
        return -ENODEV;
    return sprintf(buf, "%u\n", atomic_read(&priv->rx_dropped));
}

/**
 * @brief Sysfs show function for pending_queue length
 */
static ssize_t pending_queue_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
    struct hd_netlink_priv *priv = dev_get_drvdata(dev);
    if (!priv)
        return -ENODEV;
    return sprintf(buf, "%u\n", skb_queue_len(&priv->pending_queue));
}

/**
 * @brief Sysfs show function for nl_rx_packets (from userspace)
 */
static ssize_t nl_rx_packets_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
    struct hd_netlink_priv *priv = dev_get_drvdata(dev);
    if (!priv)
        return -ENODEV;
    return sprintf(buf, "%u\n", atomic_read(&priv->nl_rx_packets));
}

/**
 * @brief Sysfs show function for nl_tx_packets (to userspace)
 */
static ssize_t nl_tx_packets_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
    struct hd_netlink_priv *priv = dev_get_drvdata(dev);
    if (!priv)
        return -ENODEV;
    return sprintf(buf, "%u\n", atomic_read(&priv->nl_tx_packets));
}

/**
 * @brief Sysfs show function for nl_tx_dropped (to userspace)
 */
static ssize_t nl_tx_dropped_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
    struct hd_netlink_priv *priv = dev_get_drvdata(dev);
    if (!priv)
        return -ENODEV;
    return sprintf(buf, "%u\n", atomic_read(&priv->nl_tx_dropped));
}

/* Define sysfs device attributes (read-only) */
static DEVICE_ATTR_RO(tx_packets);
static DEVICE_ATTR_RO(tx_dropped);
static DEVICE_ATTR_RO(rx_packets);
static DEVICE_ATTR_RO(rx_dropped);
static DEVICE_ATTR_RO(pending_queue);
static DEVICE_ATTR_RO(nl_rx_packets);
static DEVICE_ATTR_RO(nl_tx_packets);
static DEVICE_ATTR_RO(nl_tx_dropped);

/* Attribute group */
static struct attribute *netlink_dev_attrs[] = {
    &dev_attr_tx_packets.attr,
    &dev_attr_tx_dropped.attr,
    &dev_attr_rx_packets.attr,
    &dev_attr_rx_dropped.attr,
    &dev_attr_pending_queue.attr,
    &dev_attr_nl_rx_packets.attr,
    &dev_attr_nl_tx_packets.attr,
    &dev_attr_nl_tx_dropped.attr,
    NULL,
};

static const struct attribute_group netlink_dev_attr_group = {
    .attrs = netlink_dev_attrs,
};

/**
 * @brief Process pending queue when credits become available
 * @param[in] priv Netlink private structure
 * @details This function tries to send packets from pending queue when credits are available.
 *          Should ONLY be called from daemon thread context to avoid concurrent processing.
 * @note NOT thread-safe - caller must ensure no concurrent execution
 */
static void hd_netlink_process_pending_queue(struct hd_netlink_priv *priv)
{
    struct sk_buff *msg_skb;
    struct hd_netlink_msg_packt *netlink_msg_packt;
    int ret;
    unsigned long flags;

    if (!priv) {
        return;
    }

    /* Quick check without lock - if queue is empty, nothing to do */
    if (skb_queue_len(&priv->pending_queue) == 0) {
        return;
    }

    NETLINK_STREAMCTRL(priv, "Processing pending queue, qlen=%d\n",
                skb_queue_len(&priv->pending_queue));

    while ((msg_skb = skb_dequeue(&priv->pending_queue)) != NULL) {
        /* Check available credits (without consuming yet) */
        spin_lock_irqsave(&priv->credit_lock, flags);
        if (priv->credit_nolimit) {
            spin_unlock_irqrestore(&priv->credit_lock, flags);
        } else {
            int8_t available = (int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt);
            if (available <= 0) {
                /* No credits, put it back and stop */
                skb_queue_head(&priv->pending_queue, msg_skb);
                spin_unlock_irqrestore(&priv->credit_lock, flags);
                NETLINK_STREAMCTRL(priv, " No credits available, stopping (qlen=%d)\n",
                            skb_queue_len(&priv->pending_queue));
                break;
            }
            /* Consume credit */
            priv->credit_consumed_cnt++;
            NETLINK_STREAMCTRL(priv, " Consuming credit: limit=%u, consumed=%u, available=%d->%d\n",
                        priv->credit_limit_cnt, priv->credit_consumed_cnt - 1,
                        available, available - 1);
            spin_unlock_irqrestore(&priv->credit_lock, flags);
        }

        netlink_msg_packt = (struct hd_netlink_msg_packt *)msg_skb->data;

        NETLINK_STREAMCTRL(priv, " Sending queued packet (payload_len=%u)\n",
                    netlink_msg_packt->msg_packt.len);

        /* Send to device via msg_ctrl */
        ret = hd_msg_ctrl_send(priv->msg_ctrl, msg_skb);
        if (ret < 0) {
            NETLINK_STREAMCTRL(priv, " Failed to send queued packet: %d, putting back to queue\n", ret);
            /* Return credit since send failed */
            hd_netlink_credit_put(priv);
            /* Put skb back to head of queue for retry */
            skb_queue_head(&priv->pending_queue, msg_skb);
            NETLINK_STREAMCTRL(priv, " Packet re-queued (qlen=%d), stopping to avoid retry loop\n",
                        skb_queue_len(&priv->pending_queue));
            break; /* Stop processing to avoid immediate retry loop */
        } else {
            NETLINK_STREAMCTRL(priv, " Successfully sent queued packet\n");
            atomic_inc(&priv->tx_packets);
        }
    }

    NETLINK_STREAMCTRL(priv, " Finished processing, qlen=%d\n",
                skb_queue_len(&priv->pending_queue));
}

/**
 * @brief Update the flow control credit limit
 * @param[in] priv Netlink private structure
 * @param[in] credit_limit_new New credit limit value
 * @retval 0 Always returns 0
 * @details This function updates the credit limit for flow control.
 */
static int hd_netlink_credit_limit_update(struct hd_netlink_priv *priv, uint8_t credit_limit_new)
{
    unsigned long flags;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if (priv->credit_nolimit) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return 0;
    }

    uint8_t old_limit = priv->credit_limit_cnt;
    priv->credit_limit_cnt = credit_limit_new;
    NETLINK_STREAMCTRL(priv, " UPDATE: old=%u, new=%u, consumed=%u, available=%d\n",
                old_limit, credit_limit_new, priv->credit_consumed_cnt,
                (int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt));

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    /* Process pending queue when credits are available */
    hd_netlink_process_pending_queue(priv);

    return 0;
}

/**
 * @brief Consume a flow control credit for packet transmission
 * @param[in] priv Netlink private structure
 * @retval 0 Success, credit consumed
 * @retval -1 No credits available
 * @details This function decrements the consumed credit count.
 */
static int hd_netlink_credit_get(struct hd_netlink_priv *priv)
{
    unsigned long flags;
    int ret = -1;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if (priv->credit_nolimit) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return 0;
    }

    int8_t available = (int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt);
    if (available > 0) {
        priv->credit_consumed_cnt++;
        NETLINK_STREAMCTRL(priv, " GET: limit=%u, consumed=%u->%u, available=%d->%d\n",
                    priv->credit_limit_cnt, priv->credit_consumed_cnt - 1,
                    priv->credit_consumed_cnt, available, available - 1);
        ret = 0;
    } else {
        NETLINK_STREAMCTRL(priv, " GET FAILED: limit=%u, consumed=%u, available=%d\n",
                    priv->credit_limit_cnt, priv->credit_consumed_cnt, available);
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    return ret;
}

/**
 * @brief Return a flow control credit after failed transmission
 * @param[in] priv Netlink private structure
 * @retval 0 Always returns 0
 * @details This function returns a consumed credit.
 */
static int hd_netlink_credit_put(struct hd_netlink_priv *priv)
{
    unsigned long flags;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if (priv->credit_nolimit) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return 0;
    }

    if (priv->credit_consumed_cnt > 0) {
        uint8_t old_consumed = priv->credit_consumed_cnt;
        priv->credit_consumed_cnt--;
        NETLINK_STREAMCTRL(priv, " PUT: consumed=%u->%u, available=%d\n",
                    old_consumed, priv->credit_consumed_cnt,
                    (int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt));
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    return 0;
}

/**
 * @brief Send message to userspace via Netlink
 * @param[in] priv Netlink private structure
 * @param[in] msg Message data
 * @param[in] msg_len Message length
 * @param[in] pid Target PID (0 for multicast)
 * @retval 0 Success
 * @retval <0 Error code
 */
static int hd_netlink_send_to_userspace(struct hd_netlink_priv *priv,
                                        const void *msg, uint32_t msg_len, uint32_t pid)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int ret;

    if (!priv->nl_sock) {
        NETLINK_ERR(priv, "Netlink socket not initialized\n");
        return -ENODEV;
    }

    skb = nlmsg_new(msg_len, GFP_KERNEL);
    if (!skb) {
        NETLINK_ERR(priv, "Failed to allocate Netlink SKB\n");
        return -ENOMEM;
    }

    nlh = nlmsg_put(skb, 0, 0, NLMSG_DONE, msg_len, 0);
    if (!nlh) {
        NETLINK_ERR(priv, "Failed to put Netlink header\n");
        kfree_skb(skb);
        return -EMSGSIZE;
    }

    memcpy(nlmsg_data(nlh), msg, msg_len);

    if (pid == 0) {
        /* Multicast to all sessions */
        NETLINK_DBG(priv, "Multicasting %u bytes to all sessions\n", msg_len);
        ret = nlmsg_multicast(priv->nl_sock, skb, 0, HD_NETLINK_MCGRP, GFP_KERNEL);
    } else {
        /* Unicast to specific PID */
        NETLINK_INFO(priv, "Unicasting %u bytes to PID %u\n", msg_len, pid);
        ret = nlmsg_unicast(priv->nl_sock, skb, pid);
    }

    if (ret < 0) {
        atomic_inc(&priv->nl_tx_dropped);
        if (ret == -ECONNREFUSED) {
            NETLINK_ERR(priv, "Failed to send Netlink message: %d (ECONNREFUSED - no userspace listener)\n", ret);
        } else {
            NETLINK_ERR(priv, "Failed to send Netlink message: %d\n", ret);
        }

        /* Print first bytes of data for debugging */
        if (msg_len > 0) {
            int print_len = min_t(int, msg_len, 2048);
            NETLINK_ERR(priv, "Data preview[%u]: %*ph\n", msg_len, print_len, msg);
        }

        return ret;
    }

    atomic_inc(&priv->nl_tx_packets);
    NETLINK_DBG(priv, "Successfully sent %u bytes to userspace\n", msg_len);

    /* Print data preview for confirmation */
    if (msg_len > 0) {
        NETLINK_DBG(priv, "Data preview[%u]: %*ph\n", msg_len, min_t(int, msg_len, 2048), msg);
    }

    return 0;
}

/**
 * @brief Send command packet to device
 * @param[in] priv Netlink private structure
 * @param[in] flag Command flag
 * @param[in] credit_update Credit update flag (0 or 1)
 * @retval 0 Success
 * @retval <0 Error code
 */
static int hd_netlink_send_cmd_packet(struct hd_netlink_priv *priv, uint8_t flag, bool credit_update)
{
    struct sk_buff *msg_skb;
    struct hd_netlink_msg_packt *netlink_msg_packt;
    int ret;

    if (!priv->msg_ctrl) {
        NETLINK_ERR(priv, "Invalid msg_ctrl\n");
        return -ENODEV;
    }

    /* Lock to prevent concurrent sends with userspace */
    mutex_lock(&priv->dnld_send_mutex);

    msg_skb = alloc_skb(sizeof(struct hd_netlink_msg_packt) + 32, GFP_KERNEL);
    if (!msg_skb) {
        NETLINK_ERR(priv, "Failed to allocate message SKB\n");
        mutex_unlock(&priv->dnld_send_mutex);
        return -ENOMEM;
    }

    netlink_msg_packt = (struct hd_netlink_msg_packt *)skb_put(msg_skb, sizeof(struct hd_netlink_msg_packt));
    netlink_msg_packt->msg_packt.len = sizeof(struct hd_netlink_msg_packt) - sizeof(struct hd_msg_packt);
    netlink_msg_packt->msg_packt.tag = priv->msg_tag;
    netlink_msg_packt->msg_packt.sub_tag = 0;
    netlink_msg_packt->reseved[0] = 0;
    netlink_msg_packt->flag = flag;
    netlink_msg_packt->credit_update_flag = credit_update ? 1 : 0;
    netlink_msg_packt->credit_limit_cnt = credit_update ? priv->credit_limit_cnt : 0;

    ret = hd_msg_ctrl_send(priv->msg_ctrl, msg_skb);
    if (ret < 0) {
        NETLINK_ERR(priv, "Failed to send command: %d\n", ret);
        kfree_skb(msg_skb);
        mutex_unlock(&priv->dnld_send_mutex);
        return ret;
    }

    NETLINK_DBG(priv, "Sent command: flag=%d, credit=%d\n", flag,
                credit_update ? priv->credit_limit_cnt : 0);

    mutex_unlock(&priv->dnld_send_mutex);
    return 0;
}

/**
 * @brief Socket buffer receive callback for message controller (device → host)
 * @param[in] skb Socket buffer containing received data
 * @param[in] success Reception success flag
 * @param[in] arg Netlink private structure argument
 * @retval Always returns 0
 * @details This callback handles received packets from the device through SDIO.
 */
static int hd_netlink_upld_recv_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_netlink_priv *priv = (struct hd_netlink_priv *)arg;

    if (!success || !skb) {
        NETLINK_ERR(priv, "Invalid upload callback\n");
        atomic_inc(&priv->rx_dropped);
        if (skb) {
            dev_kfree_skb(skb);
        }
        return 0;
    }

    if (skb->len < sizeof(struct hd_msg_packt)) {
        NETLINK_ERR(priv, "Received packet too small: %u bytes\n", skb->len);
        atomic_inc(&priv->rx_dropped);
        dev_kfree_skb(skb);
        return 0;
    }

    /* Add SKB to daemon thread queue */
    skb_queue_tail(&priv->thread_skb_head, skb);
    wake_up_interruptible(&priv->waitq);
    priv->thread_condition = true;
    atomic_inc(&priv->rx_packets);

    return 0;
}

/**
 * @brief Socket buffer send callback for message controller (host → device completion)
 * @param[in] skb Socket buffer that was sent
 * @param[in] success Transmission success flag
 * @param[in] arg Netlink private structure argument
 * @retval Always returns 0
 * @details This callback handles send completion events.
 */
static int hd_netlink_dnld_send_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_netlink_priv *priv = (struct hd_netlink_priv *)arg;

    if (!skb || !priv) {
        return 0;
    }

    if (!success) {
        NETLINK_ERR(priv, "Packet send failed\n");
    }

    return 0;
}

/**
 * @brief Netlink message receive callback (userspace → kernel)
 * @param[in] skb Netlink socket buffer
 * @details This function handles messages received from userspace via Netlink.
 */
static void hd_netlink_recv_msg(struct sk_buff *skb)
{
    struct hd_netlink_priv *priv;
    struct nlmsghdr *nlh;
    struct sk_buff *msg_skb;
    struct hd_netlink_msg_packt *netlink_msg_packt;
    void *payload;
    int payload_len;
    int ret;

    if (!skb) {
        pr_err("Netlink: Received NULL skb\n");
        return;
    }

    /* Use global reference to get private structure */
    priv = g_netlink_priv;
    if (!priv) {
        pr_err("Netlink: Module not initialized\n");
        return;
    }

    if (skb->len < NLMSG_HDRLEN) {
        NETLINK_ERR(priv, "SKB too small for Netlink header\n");
        return;
    }

    nlh = nlmsg_hdr(skb);
    NETLINK_DBG(priv, "Received message from userspace\n");

    payload = nlmsg_data(nlh);
    payload_len = nlmsg_len(nlh);

    if (payload_len <= 0 || payload_len > HD_NETLINK_MAX_MSGLEN) {
        NETLINK_ERR(priv, "Invalid payload length: %d\n", payload_len);
        return;
    }

    /* Update userspace RX statistics */
    atomic_inc(&priv->nl_rx_packets);

    /* Lock to prevent concurrent userspace sends */
    mutex_lock(&priv->dnld_send_mutex);

    /* Allocate message packet FIRST before consuming credit */
    msg_skb = dev_alloc_skb(sizeof(struct hd_netlink_msg_packt) + payload_len);
    if (!msg_skb) {
        NETLINK_ERR(priv, "Failed to allocate message SKB\n");
        mutex_unlock(&priv->dnld_send_mutex);
        return;
    }

    /* Build message packet with DNLD_DATA flag BEFORE credit check */
    netlink_msg_packt = (struct hd_netlink_msg_packt *)skb_put(msg_skb, sizeof(struct hd_netlink_msg_packt) + payload_len);
    netlink_msg_packt->msg_packt.tag = priv->msg_tag;
    netlink_msg_packt->msg_packt.sub_tag = 0;
    netlink_msg_packt->msg_packt.len = sizeof(struct hd_netlink_msg_packt) - sizeof(struct hd_msg_packt) + payload_len;
    netlink_msg_packt->reseved[0] = 0;
    netlink_msg_packt->flag = HD_NETLINK_FLAG_DNLD_DATA;
    netlink_msg_packt->credit_update_flag = 0;
    netlink_msg_packt->credit_limit_cnt = 0;
    memcpy(netlink_msg_packt->data, payload, payload_len);

    NETLINK_DBG(priv, "Sending DNLD_DATA: payload_len=%u, msg_packt.len=%u, skb->len=%u\n",
                payload_len, netlink_msg_packt->msg_packt.len, msg_skb->len);

    /* Check flow control credits AFTER building packet */
    if (hd_netlink_credit_get(priv) < 0) {
        NETLINK_DBG(priv, "No credits available - queuing packet (payload_len=%u, qlen=%d)\n",
                    payload_len, skb_queue_len(&priv->pending_queue));

        /* Check pending queue limit */
        if (HD_NETLINK_PENDING_MAX > 0 &&
            skb_queue_len(&priv->pending_queue) >= HD_NETLINK_PENDING_MAX) {
            NETLINK_ERR(priv, "[PENDING] Pending queue full (max=%d), dropping packet\n",
                        HD_NETLINK_PENDING_MAX);
            /* No credit was consumed, so don't call credit_put */
            atomic_inc(&priv->tx_dropped);
            dev_kfree_skb(msg_skb);
            mutex_unlock(&priv->dnld_send_mutex);
            return;
        }

        /* Queue the built msg_skb (not the original netlink skb) */
        skb_queue_tail(&priv->pending_queue, msg_skb);
        NETLINK_STREAMCTRL(priv, " Queued packet (total qlen=%d)\n",
                    skb_queue_len(&priv->pending_queue));

        mutex_unlock(&priv->dnld_send_mutex);
        return;
    }

    /* Send to device via msg_ctrl */
    ret = hd_msg_ctrl_send(priv->msg_ctrl, msg_skb);
    if (ret < 0) {
        NETLINK_ERR(priv, "Failed to send to msg_ctrl: %d\n", ret);
        hd_netlink_credit_put(priv);
        dev_kfree_skb(msg_skb);
    } else {
        atomic_inc(&priv->tx_packets);
    }

    /* Unlock after send completes */
    mutex_unlock(&priv->dnld_send_mutex);
}

/**
 * @brief Process data packet from device
 * @param[in] priv Netlink private structure
 * @param[in] netlink_msg_packt Netlink message packet
 */
static void hd_netlink_process_data_packet(struct hd_netlink_priv *priv,
                                          struct hd_netlink_msg_packt *netlink_msg_packt)
{
    uint16_t data_len;
    uint8_t *data_start;

    data_len = netlink_msg_packt->msg_packt.len -
               (sizeof(struct hd_netlink_msg_packt) - sizeof(struct hd_msg_packt));
    data_start = netlink_msg_packt->data;

#if 0
    NETLINK_INFO(priv, "=== Netlink packet dump ===\n");
    NETLINK_INFO(priv, "msg_packt.tag: %u\n", netlink_msg_packt->msg_packt.tag);
    NETLINK_INFO(priv, "msg_packt.sub_tag: %u\n", netlink_msg_packt->msg_packt.sub_tag);
    NETLINK_INFO(priv, "msg_packt.len: %u\n", netlink_msg_packt->msg_packt.len);
    NETLINK_INFO(priv, "flag: %u\n", netlink_msg_packt->flag);
    NETLINK_INFO(priv, "credit_update_flag: %u\n", netlink_msg_packt->credit_update_flag);
    NETLINK_INFO(priv, "credit_limit_cnt: %u\n", netlink_msg_packt->credit_limit_cnt);

    NETLINK_INFO(priv, "Calculated data_len: %u\n", data_len);
    NETLINK_INFO(priv, "sizeof(hd_netlink_msg_packt)=%zu, sizeof(hd_msg_packt)=%zu\n",
                 sizeof(struct hd_netlink_msg_packt), sizeof(struct hd_msg_packt));

    NETLINK_INFO(priv, "Data start address offset: %lu\n",
                 (unsigned long)(data_start - (uint8_t *)netlink_msg_packt));

    NETLINK_INFO(priv, "Processing data packet from device, len: %u\n", data_len);

    /* Print data preview for debugging */
    if (data_len > 0) {
        NETLINK_INFO(priv, "Raw data (%u bytes): %*ph\n", data_len, (int)data_len, data_start);
    }
#endif

    /* Send data to userspace */
    hd_netlink_send_to_userspace(priv, data_start, data_len, 0);
}

/**
 * @brief Netlink daemon thread
 * @details Main tasks:
 *   1. Manage state machine (RESET -> HOST_READY -> DEVICE_RUN)
 *   2. Process received packets from device
 *   3. Handle flow control
 * @param[in] data Netlink private structure
 */
static int hd_netlink_daemon_thread(void *data)
{
    struct hd_netlink_priv *priv = (struct hd_netlink_priv *)data;
    struct sk_buff *skb;
    struct hd_netlink_msg_packt *netlink_msg_packt;
    int ret;
    unsigned long flags;

    NETLINK_INFO(priv, "Netlink daemon thread started\n");

    /* Send HOST_RESET to device to reset state machine */
    ret = hd_netlink_send_cmd_packet(priv, HD_NETLINK_FLAG_HOST_RESET, 0);
    if (ret < 0) {
        NETLINK_ERR(priv, "Failed to send HOST_RESET command: %d\n", ret);
        goto exit;
    }

    /* Initialize state machine */
    priv->host_state = HD_NETLINK_HSTA_RESET;

    while (1) {
        wait_event_interruptible(priv->waitq,
                                 !skb_queue_empty(&priv->thread_skb_head) ||
                                 priv->thread_condition ||
                                 kthread_should_stop() ||
                                 priv->host_state == HD_NETLINK_HSTA_RESET);

        NETLINK_DBG(priv, "Daemon thread running, current status: %d\n", priv->host_state);

        if (priv->thread_condition) {
            msleep(2);
        }

        if (kthread_should_stop()) {
            break;
        }

        /* Try to get skb for processing */
        skb = skb_dequeue(&priv->thread_skb_head);
        if (skb) {
            netlink_msg_packt = (struct hd_netlink_msg_packt *)skb->data;
        } else {
            netlink_msg_packt = NULL;
        }

        /* Anytime receiving DEVICE_RESET/STOP, need to return to initial state */
        if (netlink_msg_packt && (netlink_msg_packt->flag == HD_NETLINK_FLAG_DEVICE_RESET ||
                                  netlink_msg_packt->flag == HD_NETLINK_FLAG_DEVICE_STOP)) {
            NETLINK_ERR(priv, "Received DEVICE_RESET/STOP in state %d, returning to HSTA_RESET\n",
                       priv->host_state);

            /* Change state to reset */
            priv->host_state = HD_NETLINK_HSTA_RESET;

            dev_kfree_skb(skb);
            skb = NULL;
            continue;
        }

        /* State machine */
        switch (priv->host_state) {
            case HD_NETLINK_HSTA_RESET:
                if (netlink_msg_packt != NULL) {
                    break;
                }
                /* Reset state, send HOST_READY packet to indicate HOST is ready */
                ret = hd_netlink_send_cmd_packet(priv, HD_NETLINK_FLAG_HOST_READY, 0);
                if (ret < 0) {
                    priv->host_state = HD_NETLINK_HSTA_ERROR;
                } else {
                    NETLINK_INFO(priv, "Host ready, waiting for device start\n");
                    priv->host_state = HD_NETLINK_HSTA_HOST_READY;
                }
                break;

            case HD_NETLINK_HSTA_HOST_READY:
                if (netlink_msg_packt == NULL || netlink_msg_packt->flag != HD_NETLINK_FLAG_DEVICE_START) {
                    break;
                }

                if (netlink_msg_packt->credit_update_flag == false ||
                    netlink_msg_packt->credit_limit_cnt == 0) {
                    spin_lock_irqsave(&priv->credit_lock, flags);
                    priv->credit_nolimit = true;
                    spin_unlock_irqrestore(&priv->credit_lock, flags);
                    NETLINK_STREAMCTRL(priv, " DEVICE_START: No Credit Limit (unlimited mode)\n");
                } else {
                    spin_lock_irqsave(&priv->credit_lock, flags);
                    priv->credit_limit_cnt = netlink_msg_packt->credit_limit_cnt;
                    priv->credit_consumed_cnt = 0;
                    priv->credit_nolimit = false;
                    spin_unlock_irqrestore(&priv->credit_lock, flags);
                    NETLINK_STREAMCTRL(priv, " DEVICE_START: Initial Credit Limit=%u\n",
                                netlink_msg_packt->credit_limit_cnt);
                }

                /* Device is now ready for communication */
                priv->host_state = HD_NETLINK_HSTA_DEVICE_RUN;
                break;

            case HD_NETLINK_HSTA_DEVICE_RUN:
                if (netlink_msg_packt == NULL) {
                    break;
                } else if (netlink_msg_packt->flag == HD_NETLINK_FLAG_UPLD_DATA) {
                    NETLINK_STREAMCTRL(priv, " UPLD_DATA: credit_update_flag=%u, credit_limit_cnt=%u\n",
                                netlink_msg_packt->credit_update_flag, netlink_msg_packt->credit_limit_cnt);
                    if (netlink_msg_packt->credit_update_flag) {
                        hd_netlink_credit_limit_update(priv, netlink_msg_packt->credit_limit_cnt);
                    }
                    hd_netlink_process_data_packet(priv, netlink_msg_packt);

                } else if (netlink_msg_packt->flag == HD_NETLINK_FLAG_CREDIT_UPDATE) {
                    NETLINK_STREAMCTRL(priv, " CREDIT_UPDATE packet: credit_limit_cnt=%u\n",
                                netlink_msg_packt->credit_limit_cnt);
                    hd_netlink_credit_limit_update(priv, netlink_msg_packt->credit_limit_cnt);
                }
                break;

            default:
                break;
        }

        /* Release skb */
        if (skb) {
            dev_kfree_skb_any(skb);
            skb = NULL;
        }
    }

exit:
    if (priv->host_state != HD_NETLINK_HSTA_ERROR) {
        ret = hd_netlink_send_cmd_packet(priv, HD_NETLINK_FLAG_HOST_STOP, 0);
    } else {
        ret = hd_netlink_send_cmd_packet(priv, HD_NETLINK_FLAG_HOST_RESET, 0);
    }

    if (ret < 0) {
        NETLINK_ERR(priv, "Failed to send stop/reset packet: %d\n", ret);
    }

    NETLINK_INFO(priv, "Netlink daemon thread exiting\n");
    return 0;
}

/**
 * @brief Initialize Netlink module
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing (e.g., HD_MSG_TAG_NETLINK)
 * @retval Pointer to private structure on success
 * @retval NULL on failure
 */
struct hd_netlink_priv *hd_netlink_init(struct hd_msg_ctrl *msg_ctrl, uint8_t msg_tag)
{
    struct hd_netlink_priv *priv;
    struct netlink_kernel_cfg cfg = {
        .input = hd_netlink_recv_msg,
        .groups = HD_NETLINK_MCGRP,
    };
    int ret;

    if (!msg_ctrl) {
        pr_err("Netlink: Invalid msg_ctrl\n");
        return NULL;
    }

    /* Allocate private structure */
    priv = kzalloc(sizeof(struct hd_netlink_priv), GFP_KERNEL);
    if (!priv) {
        pr_err("Netlink: Failed to allocate private structure\n");
        return NULL;
    }

    priv->msg_ctrl = msg_ctrl;
    priv->msg_tag = msg_tag;

    /* Initialize synchronization objects */
    mutex_init(&priv->dnld_send_mutex);
    spin_lock_init(&priv->credit_lock);

    /* Initialize pending queue for packets waiting for credits */
    skb_queue_head_init(&priv->pending_queue);

    /* Initialize credits (no limit by default) */
    priv->credit_nolimit = true;
    priv->credit_limit_cnt = 0;
    priv->credit_consumed_cnt = 0;

    /* Initialize statistics */
    atomic_set(&priv->tx_packets, 0);
    atomic_set(&priv->tx_dropped, 0);
    atomic_set(&priv->rx_packets, 0);
    atomic_set(&priv->rx_dropped, 0);
    atomic_set(&priv->nl_rx_packets, 0);
    atomic_set(&priv->nl_tx_packets, 0);
    atomic_set(&priv->nl_tx_dropped, 0);

    /* Create Netlink socket */
    priv->nl_sock = netlink_kernel_create(&init_net, HD_NETLINK_PROTO, &cfg);
    if (!priv->nl_sock) {
        pr_err("Netlink: Failed to create Netlink socket\n");
        goto cleanup_priv;
    }

    NETLINK_INFO(priv, "Netlink socket created (protocol=%d)\n", HD_NETLINK_PROTO);

    /* Initialize SKB queue for daemon thread */
    skb_queue_head_init(&priv->thread_skb_head);
    init_waitqueue_head(&priv->waitq);
    priv->thread_condition = false;
    priv->host_state = HD_NETLINK_HSTA_RESET;

    /* Start daemon thread */
    priv->daemon_thread = kthread_run(hd_netlink_daemon_thread, priv, "hd_netlink_daemon");
    if (IS_ERR(priv->daemon_thread)) {
        pr_err("Netlink: Failed to create daemon thread\n");
        ret = PTR_ERR(priv->daemon_thread);
        goto cleanup_sock;
    }

    /* Set global reference for callback access */
    g_netlink_priv = priv;

    /* Register message callbacks */
    ret = hd_msg_cb_register(priv->msg_ctrl, priv->msg_tag,
                             hd_netlink_upld_recv_cb, priv,
                             hd_netlink_dnld_send_cb, priv);
    if (ret < 0) {
        pr_err("Netlink: Failed to register message callbacks: %d\n", ret);
        goto cleanup_thread;
    }

    /* Create sysfs class (if not already created) */
    if (!netlink_class) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
        /* Linux 6.4+ changed class_create() to only take name parameter */
        netlink_class = class_create("hd_netlink");
#else
        /* Linux 4.0 - 6.3 require THIS_MODULE as first parameter */
        netlink_class = class_create(THIS_MODULE, "hd_netlink");
#endif
        if (IS_ERR(netlink_class)) {
            ret = PTR_ERR(netlink_class);
            pr_err("Netlink: Failed to create sysfs class: %d\n", ret);
            netlink_class = NULL;
            goto cleanup_cb;
        }
    }

    /* Create sysfs device */
    netlink_device = device_create(netlink_class, NULL, MKDEV(0, 0), priv, "netlink%d", 0);
    if (IS_ERR(netlink_device)) {
        ret = PTR_ERR(netlink_device);
        pr_err("Netlink: Failed to create sysfs device: %d\n", ret);
        netlink_device = NULL;
        goto cleanup_class;
    }

    /* Create sysfs attributes */
    ret = sysfs_create_group(&netlink_device->kobj, &netlink_dev_attr_group);
    if (ret < 0) {
        pr_err("Netlink: Failed to create sysfs attribute group: %d\n", ret);
        goto cleanup_device;
    }

    NETLINK_INFO(priv, "Netlink module initialized successfully\n");
    return priv;

cleanup_device:
    device_destroy(netlink_class, MKDEV(0, 0));
    netlink_device = NULL;
cleanup_class:
    class_destroy(netlink_class);
    netlink_class = NULL;
cleanup_cb:
    hd_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);
cleanup_thread:
    kthread_stop(priv->daemon_thread);
    skb_queue_purge(&priv->thread_skb_head);
cleanup_sock:
    netlink_kernel_release(priv->nl_sock);
cleanup_priv:
    kfree(priv);
    return NULL;
}

/**
 * @brief Deinitialize Netlink module
 * @param[in] priv Netlink private structure to cleanup
 */
void hd_netlink_deinit(struct hd_netlink_priv *priv)
{
    if (!priv) {
        return;
    }

    NETLINK_INFO(priv, "Netlink module deinitializing\n");

    /* Stop daemon thread */
    if (priv->daemon_thread) {
        kthread_stop(priv->daemon_thread);
        priv->daemon_thread = NULL;
    }

    /* Clear SKB queue */
    skb_queue_purge(&priv->thread_skb_head);

    /* Clear pending queue */
    skb_queue_purge(&priv->pending_queue);

    /* Remove sysfs interface */
    if (netlink_device) {
        sysfs_remove_group(&netlink_device->kobj, &netlink_dev_attr_group);
        device_destroy(netlink_class, MKDEV(0, 0));
        netlink_device = NULL;
    }

    if (netlink_class) {
        class_destroy(netlink_class);
        netlink_class = NULL;
    }

    /* Clear global reference */
    g_netlink_priv = NULL;

    /* Unregister message callbacks */
    hd_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);

    /* Release Netlink socket */
    if (priv->nl_sock) {
        netlink_kernel_release(priv->nl_sock);
        priv->nl_sock = NULL;
    }

    /* Destroy synchronization objects */
    mutex_destroy(&priv->dnld_send_mutex);

    /* Free private structure */
    kfree(priv);

    pr_info("Netlink: Module deinitialized successfully\n");
}
