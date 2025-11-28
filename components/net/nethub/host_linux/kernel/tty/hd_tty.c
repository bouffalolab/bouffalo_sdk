/**
 * @file hd_tty.c
 * @brief Host Driver TTY Interface Implementation with Dynamic Device Management
 * @author mlwang
 * @date 2025-08-07
 * @details TTY driver implementation with state machine, flow control, and
 *          dynamic device registration, modeled after eth_netdev implementation
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/skbuff.h>

#include "hd_tty.h"
#include "hd_debugfs.h"

#define TTY_DRV_NAME "hd_tty"

/* Forward declarations */
static int hd_tty_daemon_thread(void *data);
static int hd_tty_upld_recv_cb(struct sk_buff *skb, bool success, void *arg);
static int hd_tty_dnld_send_cb(struct sk_buff *skb, bool success, void *arg);

/**
 * @brief Update transmit statistics
 * @param[in] priv Private data structure
 * @param[in] packets Packet count: positive=success, negative=error, zero=dropped
 * @param[in] bytes Byte count for successful packets (ignored for errors/drops)
 * @retval None
 * @details Centralized transmit statistics update function
 */
static inline void hd_tty_update_tx_stats(struct hd_tty_priv *priv, int packets, uint32_t bytes)
{
    spin_lock(&priv->stats_lock);
    if (packets > 0) {
        priv->tx_packets += packets;
        priv->tx_bytes += bytes;
    } else if (packets < 0) {
        priv->tx_errors += (-packets);
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
static inline void hd_tty_update_rx_stats(struct hd_tty_priv *priv, int packets, uint32_t bytes)
{
    spin_lock(&priv->stats_lock);
    if (packets > 0) {
        priv->rx_packets += packets;
        priv->rx_bytes += bytes;
    } else if (packets < 0) {
        priv->rx_errors += (-packets);
    }
    spin_unlock(&priv->stats_lock);
}

/**
 * @brief Update the flow control credit limit
 * @param[in] priv TTY private structure
 * @param[in] credit_limit_new New credit limit value
 * @retval 0 Always returns 0
 * @details This function updates the credit limit for flow control and
 *          notifies TTY layer when credits become available.
 */
static int hd_tty_credit_limit_update(struct hd_tty_priv *priv, uint8_t credit_limit_new)
{
    unsigned long flags;
    bool should_wake_tty = false;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if (priv->credit_nolimit) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return 0;
    }

    if ((int8_t)(credit_limit_new - priv->credit_limit_cnt) > 0) {
        /* Check if we should wake up TTY layer */
        if ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) <= 0) {
            should_wake_tty = true;
        }
        priv->credit_limit_cnt = credit_limit_new;

        TTY_DBG(priv, "Credit limit updated to %u\n", credit_limit_new);
    } else {
        TTY_INFO(priv, "Credit limit not updated: new=%u, current=%u\n",
                 credit_limit_new, priv->credit_limit_cnt);
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    /* Wake up TTY layer if credits become available */
    if (should_wake_tty) {
        tty_port_tty_wakeup(&priv->port);
    }

    return 0;
} /**
 * @brief Consume a flow control credit for packet transmission
 * @param[in] priv TTY private structure
 * @retval 0 Success, credit consumed
 * @retval -1 No credits available
 * @details This function decrements the consumed credit count.
 */
static int hd_tty_credit_get(struct hd_tty_priv *priv)
{
    unsigned long flags;
    int ret = -1;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if (priv->credit_nolimit) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return 0;
    }

    if ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) > 0) {
        priv->credit_consumed_cnt++;
        ret = 0;
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    return ret;
}

/**
 * @brief Return a flow control credit after failed transmission
 * @param[in] priv TTY private structure
 * @retval 0 Always returns 0
 * @details This function returns a consumed credit and notifies TTY layer
 *          when credits become available.
 */
static int hd_tty_credit_put(struct hd_tty_priv *priv)
{
    unsigned long flags;
    bool should_wake_tty = false;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if (priv->credit_nolimit) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return 0;
    }

    if (priv->credit_consumed_cnt > 0) {
        priv->credit_consumed_cnt--;
        /* Wake up TTY if there are now credits available */
        if ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) > 0) {
            should_wake_tty = true;
        }
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    /* Wake up TTY layer if credits become available */
    if (should_wake_tty) {
        tty_port_tty_wakeup(&priv->port);
    }

    return 0;
}

/**
 * @brief Send command packet to device
 * @param[in] priv TTY private structure
 * @param[in] flag Command flag
 * @retval 0 Success
 * @retval <0 Error code
 */
static int hd_tty_send_cmd_packet(struct hd_tty_priv *priv, uint8_t flag)
{
    struct sk_buff *skb;
    struct hd_tty_msg_packt *tty_msg_packt;
    int ret;

    skb = dev_alloc_skb(sizeof(struct hd_tty_msg_packt));
    if (!skb) {
        TTY_ERR(priv, "Failed to allocate SKB for command packet\n");
        return -ENOMEM;
    }

    tty_msg_packt = (struct hd_tty_msg_packt *)skb_put(skb, sizeof(struct hd_tty_msg_packt));

    tty_msg_packt->msg_packt.tag = priv->msg_tag;
    tty_msg_packt->msg_packt.sub_tag = 0;
    tty_msg_packt->msg_packt.len = sizeof(struct hd_tty_msg_packt) - sizeof(struct hd_msg_packt);

    tty_msg_packt->reseved[0] = 0;
    tty_msg_packt->flag = flag;
    tty_msg_packt->credit_update_flag = 0;
    tty_msg_packt->credit_limit_cnt = 0;

    ret = hd_msg_ctrl_send(priv->msg_ctrl, skb);
    if (ret < 0) {
        TTY_ERR(priv, "Failed to send command packet: flag=0x%02x, ret=%d\n", flag, ret);
        dev_kfree_skb(skb);
        return ret;
    }

    TTY_DBG(priv, "Sent command packet: flag=0x%02x\n", flag);
    return 0;
}

/**
 * @brief Process received data packet
 * @param[in] priv TTY private structure
 * @param[in] tty_msg_packt Message packet containing data
 * @retval 0 Success
 * @details This function processes incoming data packets and inserts them
 *          into the TTY flip buffer for delivery to userspace.
 */
static int hd_tty_process_data_packet(struct hd_tty_priv *priv,
                                      struct hd_tty_msg_packt *tty_msg_packt)
{
    uint16_t data_len;
    struct tty_struct *tty;

    if (!priv->is_device_registered) {
        TTY_DBG(priv, "TTY device not registered for data\n");
        return -ENODEV;
    }

    /* Get TTY struct safely - this will return NULL if device is not open */
    tty = tty_port_tty_get(&priv->port);
    if (!tty) {
        TTY_DBG(priv, "TTY port not open for data\n");
        return -ENODEV;
    }

    data_len = HD_TTY_GET_DATA_LEN(tty_msg_packt->msg_packt.len);
    if (data_len == 0) {
        TTY_DBG(priv, "Received empty data packet\n");
        tty_kref_put(tty);
        return 0;
    }

    /* Insert received data into TTY flip buffer */
    tty_insert_flip_string(&priv->port, tty_msg_packt->data, data_len);
    tty_flip_buffer_push(&priv->port);

    /* Release TTY reference */
    tty_kref_put(tty);

    /* Update statistics */
    hd_tty_update_rx_stats(priv, 1, data_len);

    TTY_DBG(priv, "Received data: len=%u\n", data_len);
    return 0;
}

/**
 * @brief Socket buffer receive callback for message controller
 * @param[in] skb Socket buffer containing received data
 * @param[in] success Reception success flag (always true for received messages)
 * @param[in] arg TTY private structure argument
 * @retval Always returns 0
 * @details This callback handles received TTY packets from the device.
 *          For RUN state data packets, it processes them directly for speed.
 *          For control packets, it queues them for daemon thread processing.
 */
static int hd_tty_upld_recv_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_tty_priv *priv = (struct hd_tty_priv *)arg;
    struct hd_tty_msg_packt *tty_msg_packt;

    if (skb->len < sizeof(struct hd_tty_msg_packt)) {
        TTY_ERR(priv, "Received packet too small: %u bytes\n", skb->len);
        hd_tty_update_rx_stats(priv, -1, 0);
        dev_kfree_skb(skb);
        return 0;
    }

    tty_msg_packt = (struct hd_tty_msg_packt *)skb->data;

    /* Fast path for data packets in RUN state */
    if (priv->tty_status == HD_TTY_HSTA_DEVICE_RUN &&
        (tty_msg_packt->flag == HD_TTY_FLAG_UPLD_DATA || tty_msg_packt->flag == HD_TTY_FLAG_CREDIT_UPDATE)) {
        /* Update flow control */
        if (tty_msg_packt->credit_update_flag) {
            hd_tty_credit_limit_update(priv, tty_msg_packt->credit_limit_cnt);
        }

        if (tty_msg_packt->flag == HD_TTY_FLAG_UPLD_DATA) {
            hd_tty_process_data_packet(priv, tty_msg_packt);
        }

        dev_kfree_skb_any(skb);
        return 0;
    }

    /* Queue control packets for daemon thread */
    skb_queue_tail(&priv->thread_skb_head, skb);
    wake_up_interruptible(&priv->waitq);

    return 0;
}

/**
 * @brief Socket buffer send callback for message controller
 * @param[in] skb Socket buffer that was sent
 * @param[in] success Transmission success flag
 * @param[in] arg TTY private structure argument
 * @retval Always returns 0
 * @details This callback handles send completion events and updates statistics.
 */
static int hd_tty_dnld_send_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_tty_priv *priv = (struct hd_tty_priv *)arg;
    struct hd_tty_msg_packt *tty_msg_packt;
    uint16_t data_len = 0;

    if (!skb || !priv) {
        return 0;
    }

    if (skb->len >= sizeof(struct hd_tty_msg_packt)) {
        tty_msg_packt = (struct hd_tty_msg_packt *)skb->data;
        if (tty_msg_packt->flag == HD_TTY_FLAG_DNLD_DATA) {
            data_len = HD_TTY_GET_DATA_LEN(tty_msg_packt->msg_packt.len);
        }
    }

    if (success) {
        if (data_len > 0) {
            hd_tty_update_tx_stats(priv, 1, data_len);
        }
        TTY_DBG(priv, "Packet sent successfully\n");
    } else {
        if (data_len > 0) {
            hd_tty_update_tx_stats(priv, -1, 0);
            hd_tty_credit_put(priv);
        }
        TTY_ERR(priv, "Packet send failed\n");
    }

    return 0;
}

/**
 * @brief TTY daemon thread main function
 * @param[in] data TTY private structure
 * @retval 0 Thread exit
 * @details Main daemon thread that manages TTY state machine and processes
 *          control packets. Similar to eth_netdev daemon thread.
 */
static int hd_tty_daemon_thread(void *data)
{
    int ret;
    unsigned long flags;
    struct hd_tty_priv *priv = (struct hd_tty_priv *)data;
    struct sk_buff *skb;
    struct hd_tty_msg_packt *tty_msg_packt;

    TTY_INFO(priv, "TTY daemon thread started\n");

    ret = hd_tty_send_cmd_packet(priv, HD_TTY_FLAG_HOST_RESET);
    if (ret < 0) {
        TTY_ERR(priv, "Failed to send HOST_READY command: %d\n", ret);
        goto exit;
    }

    /* Initialize state machine */
    priv->tty_status = HD_TTY_HSTA_RESET;

    while (1) {
        wait_event_interruptible(priv->waitq,
                                 !skb_queue_empty(&priv->thread_skb_head) ||
                                     priv->thread_condition ||
                                     kthread_should_stop() ||
                                     priv->tty_status == HD_TTY_HSTA_RESET);

        TTY_DBG(priv, "Daemon thread running, current status: %d\n", priv->tty_status);

        if (priv->thread_condition) {
            msleep(2);
        }

        if (kthread_should_stop()) {
            break;
        }

        /* Try to get skb for processing */
        skb = skb_dequeue(&priv->thread_skb_head);
        if (skb) {
            tty_msg_packt = (struct hd_tty_msg_packt *)skb->data;
        } else {
            tty_msg_packt = NULL;
        }

        /* Anytime receiving DEVICE_RESET, need to return to initial state */
        if (tty_msg_packt && (tty_msg_packt->flag == HD_TTY_FLAG_DEVICE_RESET || tty_msg_packt->flag == HD_TTY_FLAG_DEVICE_STOP)) {
            TTY_ERR(priv, "received DEVICE_RESET in state %d, returning to HSTA_RESET\n", priv->tty_status);

            /* Hangup any open TTY connections to release user space references immediately */
            if (priv->is_device_registered) {
                tty_port_hangup(&priv->port);
            }

            /* Change state to reset */
            priv->tty_status = HD_TTY_HSTA_RESET;

            dev_kfree_skb(skb);
            skb = NULL;
            continue;
        }

        switch (priv->tty_status) {
            case HD_TTY_HSTA_RESET:
                if (tty_msg_packt != NULL) {
                    break;
                }
                /* Reset state, send HOST_READY packet to indicate HOST is ready */
                ret = hd_tty_send_cmd_packet(priv, HD_TTY_FLAG_HOST_READY);
                if (ret < 0) {
                    priv->tty_status = HD_TTY_HSTA_ERROR;
                } else {
                    TTY_INFO(priv, "Host ready, waiting for device start\n");
                    priv->tty_status = HD_TTY_HSTA_HOST_READY;
                }
                break;

            case HD_TTY_HSTA_HOST_READY:
                if (tty_msg_packt == NULL || tty_msg_packt->flag != HD_TTY_FLAG_DEVICE_START) {
                    break;
                }

                if (tty_msg_packt->credit_update_flag == false || tty_msg_packt->credit_limit_cnt == 0) {
                    spin_lock_irqsave(&priv->credit_lock, flags);
                    priv->credit_nolimit = true;
                    spin_unlock_irqrestore(&priv->credit_lock, flags);
                    TTY_INFO(priv, "Device Ready! No Credit Limit\n");
                } else {
                    spin_lock_irqsave(&priv->credit_lock, flags);
                    priv->credit_limit_cnt = tty_msg_packt->credit_limit_cnt;
                    priv->credit_consumed_cnt = 0;
                    priv->credit_nolimit = false;
                    spin_unlock_irqrestore(&priv->credit_lock, flags);
                    TTY_INFO(priv, "Device Ready! Credit Limit=%u\n", tty_msg_packt->credit_limit_cnt);
                }

                /* Device is now ready for communication */
                priv->tty_status = HD_TTY_HSTA_DEVICE_RUN;
                break;

            case HD_TTY_HSTA_DEVICE_RUN:
                if (tty_msg_packt == NULL) {
                    break;
                } else if (tty_msg_packt->flag == HD_TTY_FLAG_UPLD_DATA) {
                    if (tty_msg_packt->credit_update_flag) {
                        hd_tty_credit_limit_update(priv, tty_msg_packt->credit_limit_cnt);
                    }
                    hd_tty_process_data_packet(priv, tty_msg_packt);

                } else if (tty_msg_packt->flag == HD_TTY_FLAG_CREDIT_UPDATE) {
                    hd_tty_credit_limit_update(priv, tty_msg_packt->credit_limit_cnt);
                }
                break;

            default:
                break;
        }

        /* Release skb_recv */
        if (skb) {
            dev_kfree_skb_any(skb);
            skb = NULL;
        }
    }

exit:
    if (priv->tty_status != HD_TTY_HSTA_ERROR) {
        ret = hd_tty_send_cmd_packet(priv, HD_TTY_FLAG_HOST_STOP);
    } else {
        ret = hd_tty_send_cmd_packet(priv, HD_TTY_FLAG_HOST_RESET);
    }

    if (ret < 0) {
        TTY_ERR(priv, "failed to send stop packet: %d\n", ret);
    }

    /* Wait for kthread_stop() to be called to avoid premature thread exit */
    while (!kthread_should_stop()) {
        wait_event_interruptible(priv->waitq,
                                 kthread_should_stop() || priv->thread_condition);
    }

    TTY_INFO(priv, "TTY daemon thread exiting, status: %d\n", priv->tty_status);
    return 0;
}

/* TTY Driver Operations */

/**
 * @brief TTY open operation
 * @param[in] tty TTY structure
 * @param[in] filp File pointer
 * @retval 0 Success
 * @retval <0 Error code
 */
static int hd_tty_open(struct tty_struct *tty, struct file *filp)
{
    struct hd_tty_priv *priv = tty->driver->driver_state;

    if (!priv) {
        return -ENODEV;
    }

    tty->driver_data = priv;

    /* Use tty_port_open which will call our port activate function */
    return tty_port_open(&priv->port, tty, filp);
}

/**
 * @brief TTY close operation
 * @param[in] tty TTY structure
 * @param[in] filp File pointer
 */
static void hd_tty_close(struct tty_struct *tty, struct file *filp)
{
    struct hd_tty_priv *priv = tty->driver_data;

    if (!priv) {
        return;
    }

    TTY_INFO(priv, "TTY close called\n");

    /* Use tty_port_close which will call our port shutdown function */
    tty_port_close(&priv->port, tty, filp);
}

/**
 * @brief TTY write operation
 * @param[in] tty TTY structure
 * @param[in] buf Data buffer to write
 * @param[in] count Number of bytes to write
 * @retval >0 Number of bytes written
 * @retval 0 No space available (TTY will wait and retry)
 * @retval <0 Error code
 */
static ssize_t hd_tty_write(struct tty_struct *tty, const unsigned char *buf, size_t count)
{
    struct hd_tty_priv *priv = tty->driver_data;
    struct sk_buff *skb;
    struct hd_tty_msg_packt *tty_msg_packt;
    size_t data_len;
    int ret;

    TTY_DBG(priv, "hd_tty_write called: count=%zu\n", count);

    if (!priv || !buf || count == 0) {
        TTY_DBG(priv, "hd_tty_write: Invalid parameters\n");
        return -EINVAL;
    }

    if (priv->tty_status != HD_TTY_HSTA_DEVICE_RUN) {
        TTY_DBG(priv, "hd_tty_write: Device not ready, status=%d\n", priv->tty_status);
        return -EAGAIN;
    }

    /* Check if we have credits available */
    if (hd_tty_credit_get(priv) < 0) {
        return 0; /* No credits, TTY layer will wait and retry */
    }

    /* Limit data length */
    data_len = min(count, (size_t)HD_TTY_MAX_DATA_LEN);

    /* Allocate SKB */
    skb = dev_alloc_skb(sizeof(struct hd_tty_msg_packt) + data_len);
    if (!skb) {
        hd_tty_credit_put(priv);
        return -ENOMEM;
    }

    /* Build packet */
    tty_msg_packt = (struct hd_tty_msg_packt *)skb_put(skb, sizeof(struct hd_tty_msg_packt) + data_len);

    tty_msg_packt->msg_packt.tag = priv->msg_tag;
    tty_msg_packt->msg_packt.sub_tag = 0;
    tty_msg_packt->msg_packt.len = sizeof(struct hd_tty_msg_packt) - sizeof(struct hd_msg_packt) + data_len;

    tty_msg_packt->reseved[0] = 0;
    tty_msg_packt->flag = HD_TTY_FLAG_DNLD_DATA;
    tty_msg_packt->credit_update_flag = 0;
    tty_msg_packt->credit_limit_cnt = 0;

    /* Copy data */
    memcpy(tty_msg_packt->data, buf, data_len);

    TTY_DBG(priv, "Sent data: %zu bytes\n", data_len);

    /* Send packet */
    ret = hd_msg_ctrl_send(priv->msg_ctrl, skb);
    if (ret < 0) {
        TTY_ERR(priv, "Failed to send data packet: ret=%d\n", ret);
        hd_tty_credit_put(priv);
        dev_kfree_skb(skb);
        return ret;
    }

    return data_len;
}

/**
 * @brief TTY write room operation
 * @param[in] tty TTY structure
 * @retval Number of bytes that can be written
 */
static unsigned int hd_tty_write_room(struct tty_struct *tty)
{
    struct hd_tty_priv *priv = tty->driver_data;
    unsigned long flags;
    int credits_available;

    if (!priv || priv->tty_status != HD_TTY_HSTA_DEVICE_RUN) {
        return 0;
    }

    spin_lock_irqsave(&priv->credit_lock, flags);

    if (priv->credit_nolimit) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return HD_TTY_MAX_DATA_LEN;
    }

    credits_available = (int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt);
    spin_unlock_irqrestore(&priv->credit_lock, flags);

    if (credits_available <= 0) {
        return 0;
    } else {
        return HD_TTY_MAX_DATA_LEN;
    }
}

/**
 * @brief TTY chars in buffer operation
 * @param[in] tty TTY structure
 * @retval Number of characters in buffer (always 0 for this implementation)
 */
static unsigned int hd_tty_chars_in_buffer(struct tty_struct *tty)
{
    return 0; // No local buffering
}

/**
 * @brief TTY flush buffer operation
 * @param[in] tty TTY structure
 */
static void hd_tty_flush_buffer(struct tty_struct *tty)
{
    /* No local buffering to flush */
}

/**
 * @brief TTY hangup operation
 * @param[in] tty TTY structure
 */
static void hd_tty_hangup(struct tty_struct *tty)
{
    struct hd_tty_priv *priv = tty->driver_data;

    if (!priv) {
        return;
    }

    priv->is_open = false;

    TTY_INFO(priv, "TTY hangup called by user space\n");

    /* Send hangup command to device */
    hd_tty_send_cmd_packet(priv, HD_TTY_FLAG_HANGUP);
}

/* TTY driver operations structure */
static const struct tty_operations hd_tty_ops = {
    .open = hd_tty_open,
    .close = hd_tty_close,
    .write = hd_tty_write,
    .write_room = hd_tty_write_room,
    .chars_in_buffer = hd_tty_chars_in_buffer,
    .flush_buffer = hd_tty_flush_buffer,
    .hangup = hd_tty_hangup,
};

/* TTY port operations */

/**
 * @brief TTY port activate operation
 * @param[in] port TTY port structure
 * @param[in] tty TTY structure
 * @retval 0 Success
 * @retval <0 Error code
 */
static int hd_tty_port_activate(struct tty_port *port, struct tty_struct *tty)
{
    struct hd_tty_priv *priv = container_of(port, struct hd_tty_priv, port);

    /* Check if already opened - only single open supported */
    if (priv->is_open) {
        TTY_INFO(priv, "TTY already opened, only single open supported\n");
        return -EBUSY;
    }

    /* Device must be in RUN state to accept opens */
    if (priv->tty_status != HD_TTY_HSTA_DEVICE_RUN) {
        TTY_DBG(priv, "TTY not ready for open (state: %d)\n", priv->tty_status);
        return -EIO; /* Use EIO instead of EAGAIN to avoid infinite retry */
    }

    priv->is_open = true;
    TTY_INFO(priv, "TTY port activated\n");

    /* Send open command to device */
    hd_tty_send_cmd_packet(priv, HD_TTY_FLAG_OPEN);

    return 0;
}

/**
 * @brief TTY port shutdown operation
 * @param[in] port TTY port structure
 */
static void hd_tty_port_shutdown(struct tty_port *port)
{
    struct hd_tty_priv *priv = container_of(port, struct hd_tty_priv, port);

    priv->is_open = false;
    TTY_INFO(priv, "TTY port shutdown called\n");

    /* Send close command to device */
    hd_tty_send_cmd_packet(priv, HD_TTY_FLAG_CLOSE);
}

static const struct tty_port_operations hd_tty_port_ops = {
    .activate = hd_tty_port_activate,
    .shutdown = hd_tty_port_shutdown,
};

/**
 * @brief Initialize TTY driver with dynamic device management
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval Pointer to TTY private structure on success
 * @retval NULL on failure
 */
struct hd_tty_priv *hd_tty_init(struct hd_msg_ctrl *msg_ctrl, uint8_t msg_tag)
{
    struct hd_tty_priv *priv;
    int ret;

    if (!msg_ctrl) {
        pr_err("TTY: Invalid msg_ctrl\n");
        return NULL;
    }

    /* Allocate private structure */
    priv = kzalloc(sizeof(struct hd_tty_priv), GFP_KERNEL);
    if (!priv) {
        pr_err("TTY: Failed to allocate private structure\n");
        return NULL;
    }

    priv->msg_ctrl = msg_ctrl;
    priv->msg_tag = msg_tag;

    /* Initialize synchronization objects */
    spin_lock_init(&priv->credit_lock);
    spin_lock_init(&priv->stats_lock);
    init_waitqueue_head(&priv->waitq);

    /* Initialize SKB queue for daemon thread */
    skb_queue_head_init(&priv->thread_skb_head);

    /* Initialize TTY port */
    tty_port_init(&priv->port);
    priv->port.ops = &hd_tty_port_ops;

    /* Allocate TTY driver */
    priv->tty_driver = tty_alloc_driver(1, HD_TTY_FLAGS);
    if (IS_ERR(priv->tty_driver)) {
        pr_err("TTY: Failed to allocate TTY driver\n");
        ret = PTR_ERR(priv->tty_driver);
        goto cleanup_port;
    }

    /* Setup TTY driver */
    priv->tty_driver->driver_name = HD_TTY_DRIVER_NAME;
    priv->tty_driver->name = HD_TTY_DEVICE_NAME;
    priv->tty_driver->major = HD_TTY_MAJOR;
    priv->tty_driver->minor_start = 0;
    priv->tty_driver->type = HD_TTY_TYPE;
    priv->tty_driver->subtype = HD_TTY_SUBTYPE;
    priv->tty_driver->init_termios = tty_std_termios;
    priv->tty_driver->driver_state = priv;

    tty_set_operations(priv->tty_driver, &hd_tty_ops);
    tty_port_link_device(&priv->port, priv->tty_driver, 0);

    /* Register TTY driver (but not the device yet) */
    ret = tty_register_driver(priv->tty_driver);
    if (ret < 0) {
        pr_err("TTY: Failed to register TTY driver: %d\n", ret);
        goto free_tty_driver;
    }

    /* Register message callbacks */
    ret = hd_msg_cb_register(priv->msg_ctrl, priv->msg_tag,
                             hd_tty_upld_recv_cb, priv,
                             hd_tty_dnld_send_cb, priv);
    if (ret < 0) {
        pr_err("TTY: Failed to register message callbacks: %d\n", ret);
        goto unregister_tty;
    }

    /* Start daemon thread */
    priv->daemon_thread = kthread_run(hd_tty_daemon_thread, priv, "hd_tty_daemon");
    if (IS_ERR(priv->daemon_thread)) {
        pr_err("TTY: Failed to create daemon thread\n");
        ret = PTR_ERR(priv->daemon_thread);
        goto unregister_callbacks;
    }

    /* Register TTY device immediately - it will be available when device is ready */
    priv->tty_dev = tty_port_register_device(&priv->port,
                                             priv->tty_driver, 0, NULL);
    if (IS_ERR(priv->tty_dev)) {
        pr_err("TTY: Failed to register TTY device\n");
        ret = PTR_ERR(priv->tty_dev);
        priv->tty_dev = NULL;
        goto stop_daemon;
    }
    priv->is_device_registered = true;

    TTY_INFO(priv, "TTY driver initialized successfully\n");
    return priv;

stop_daemon:
    priv->thread_condition = true;
    wake_up_interruptible(&priv->waitq);
    kthread_stop(priv->daemon_thread);
    priv->daemon_thread = NULL;

unregister_callbacks:
    hd_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);
unregister_tty:
    tty_unregister_driver(priv->tty_driver);
free_tty_driver:
    tty_driver_kref_put(priv->tty_driver);
cleanup_port:
    tty_port_destroy(&priv->port);
    kfree(priv);
    return NULL;
}

/**
 * @brief Deinitialize TTY driver
 * @param[in] priv TTY private structure to cleanup
 */
void hd_tty_deinit(struct hd_tty_priv *priv)
{
    struct sk_buff *skb;

    if (!priv) {
        return;
    }

    TTY_INFO(priv, "TTY driver deinitializing\n");

    /* Unregister message callbacks */
    hd_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);

    /* Unregister TTY device first if registered */
    if (priv->is_device_registered) {
        tty_port_hangup(&priv->port);
        tty_port_unregister_device(&priv->port, priv->tty_driver, 0);

        priv->is_device_registered = false;
        priv->tty_dev = NULL;
    }

    /* Stop daemon thread */
    if (priv->daemon_thread) {
        priv->thread_condition = true;
        wake_up_interruptible(&priv->waitq);
        kthread_stop(priv->daemon_thread);
        priv->daemon_thread = NULL;
    }

    /* Unregister TTY driver */
    TTY_INFO(priv, "Unregistering TTY driver...\n");
    tty_unregister_driver(priv->tty_driver);
    TTY_INFO(priv, "Putting TTY driver reference...\n");
    tty_driver_kref_put(priv->tty_driver);

    /* Cleanup TTY port */
    tty_port_destroy(&priv->port);

    /* Free pending SKBs */
    while ((skb = skb_dequeue(&priv->thread_skb_head)) != NULL) {
        dev_kfree_skb(skb);
    }

    /* Free private structure */
    kfree(priv);

    pr_info("TTY: Driver deinitialized successfully\n");
}
