/**
 * @file mr_tty.c
 * @brief TTY Interface Implementation with Dynamic Device Management
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

#include "mr_tty.h"
#include "mr_msg_ctrl.h"
#include "mr_debugfs.h"

#define TTY_DRV_NAME "mr_tty"

/* Forward declarations */
static int mr_tty_daemon_thread(void *data);
static int mr_tty_upld_recv_cb(struct sk_buff *skb, bool success, void *arg);
static int mr_tty_dnld_send_cb(struct sk_buff *skb, bool success, void *arg);
static int mr_tty_received_process(struct mr_tty_priv *priv);

/**
 * @brief Update the flow control credit limit
 * @param[in] priv TTY private structure
 * @param[in] credit_limit_new New credit limit value
 * @retval 0 Always returns 0
 * @details This function updates the credit limit for flow control and
 *          notifies TTY layer when credits become available.
 */
static int mr_tty_credit_limit_update(struct mr_tty_priv *priv, uint8_t credit_limit_new)
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
        TTY_INFO(priv, "Credit limit not updated: new=%u, current=%u\n", credit_limit_new, priv->credit_limit_cnt);
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    /* Wake up TTY layer if credits become available */
    if (should_wake_tty) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
        struct tty_struct *tty = tty_port_tty_get(&priv->port);
        if (!tty) {
            TTY_DBG(priv, "TTY port not open\n");
            return 0;
        }
        tty_wakeup(tty);
        tty_kref_put(tty);
#else
        tty_port_tty_wakeup(&priv->port);
#endif
    }

    return 0;
} /**
 * @brief Consume a flow control credit for packet transmission
 * @param[in] priv TTY private structure
 * @retval 0 Success, credit consumed
 * @retval -1 No credits available
 * @details This function decrements the consumed credit count.
 */
static int mr_tty_credit_get(struct mr_tty_priv *priv)
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

#if 0
/**
 * @brief Query available flow control credits
 * @param[in] priv Netlink private structure
 * @return Number of available credits (limit - consumed)
 * @details This function returns the number of available credits for packet transmission.
 */
static int mr_tty_credit_available(struct mr_tty_priv *priv)
{
    unsigned long flags;
    int available;

    spin_lock_irqsave(&priv->credit_lock, flags);
    available = (int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt);
    spin_unlock_irqrestore(&priv->credit_lock, flags);

    return available;
}
#endif

/**
 * @brief Return a flow control credit after failed transmission
 * @param[in] priv TTY private structure
 * @retval 0 Always returns 0
 * @details This function returns a consumed credit and notifies TTY layer
 *          when credits become available.
 */
static int mr_tty_credit_put(struct mr_tty_priv *priv)
{
    unsigned long flags;
    uint8_t credit_consumed_old;
    bool should_wake_tty = false;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if (priv->credit_nolimit) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return 0;
    }

    credit_consumed_old = priv->credit_consumed_cnt;
    priv->credit_consumed_cnt--;
    /* Wake up TTY if there are now credits available */
    if (((int8_t)(credit_consumed_old - priv->credit_consumed_cnt) <= 0) &&
        ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) > 0)) {
        should_wake_tty = true;
    }
    spin_unlock_irqrestore(&priv->credit_lock, flags);

    /* Wake up TTY layer if credits become available */
    if (should_wake_tty) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
        struct tty_struct *tty = tty_port_tty_get(&priv->port);
        if (!tty) {
            TTY_DBG(priv, "TTY port not open\n");
            return 0;
        }
        tty_wakeup(tty);
        tty_kref_put(tty);
#else
        tty_port_tty_wakeup(&priv->port);
#endif
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
static int mr_tty_send_cmd_packet(struct mr_tty_priv *priv, uint8_t flag)
{
    struct sk_buff *skb;
    struct mr_tty_msg_pkt *tty_msg_pkt;
    int ret;

    skb = dev_alloc_skb(sizeof(struct mr_tty_msg_pkt));
    if (!skb) {
        TTY_ERR(priv, "Failed to allocate SKB for command packet\n");
        return -ENOMEM;
    }

    tty_msg_pkt = (struct mr_tty_msg_pkt *)skb_put(skb, sizeof(struct mr_tty_msg_pkt));

    tty_msg_pkt->msg_pkt.tag = priv->msg_tag;
    tty_msg_pkt->msg_pkt.sub_tag = 0;
    tty_msg_pkt->msg_pkt.len = sizeof(struct mr_tty_msg_pkt) - sizeof(struct mr_msg_pkt);

    tty_msg_pkt->reserved[0] = 0;
    tty_msg_pkt->flag = flag;
    tty_msg_pkt->credit_update_flag = 0;
    tty_msg_pkt->credit_limit_cnt = 0;

    ret = mr_msg_ctrl_send(priv->msg_ctrl, skb);
    if (ret < 0) {
        TTY_ERR(priv, "Failed to send command packet: flag=0x%02x, ret=%d\n", flag, ret);
        dev_kfree_skb_any(skb);
        return ret;
    }

    TTY_DBG(priv, "Sent command packet: flag=0x%02x\n", flag);
    return 0;
}

/**
 * @brief Process received data packet
 * @param[in] priv TTY private structure
 * @param[in] tty_msg_pkt Message packet containing data
 * @retval 0 Success
 * @details This function processes incoming data packets and inserts them
 *          into the TTY flip buffer for delivery to userspace.
 */
static int mr_tty_process_data_packet(struct mr_tty_priv *priv, struct mr_tty_msg_pkt *tty_msg_pkt)
{
    uint16_t data_len;
    int inserted;
    struct tty_struct *tty;

    /* Get TTY struct safely - this will return NULL if device is not open */
    tty = tty_port_tty_get(&priv->port);
    if (!tty) {
        TTY_DBG(priv, "TTY port not open for data\n");
        return -ENODEV;
    }

    data_len = MR_TTY_GET_DATA_LEN(tty_msg_pkt->msg_pkt.len);
    if (data_len == 0) {
        TTY_DBG(priv, "Received empty data packet\n");
        tty_kref_put(tty);
        return 0;
    }

    /* Insert received data into TTY flip buffer */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0)
    inserted = tty_insert_flip_string(tty, tty_msg_pkt->data, data_len);
#else
    inserted = tty_insert_flip_string(&priv->port, tty_msg_pkt->data, data_len);
#endif
    /* Check if all data was inserted */
    if (inserted < data_len) {
        TTY_ERR(priv, "Flip buffer full: requested=%u, inserted=%d, dropped=%d\n", data_len, inserted,
                data_len - inserted);
    }

    /* Push data to TTY layer */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0)
    tty_flip_buffer_push(tty);
#else
    tty_flip_buffer_push(&priv->port);
#endif

    /* Release TTY reference */
    tty_kref_put(tty);

    TTY_DBG(priv, "Received data: len=%u, inserted=%d\n", data_len, inserted);
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
static int mr_tty_upld_recv_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct mr_tty_priv *priv = (struct mr_tty_priv *)arg;
    struct mr_tty_msg_pkt *tty_msg_pkt;

    if (skb->len < sizeof(struct mr_tty_msg_pkt)) {
        TTY_ERR(priv, "Received packet too small: %u bytes\n", skb->len);
        dev_kfree_skb_any(skb);
        return 0;
    }

    tty_msg_pkt = (struct mr_tty_msg_pkt *)skb->data;

#if 0
    /* Fast path for data packets in RUN state */
    if (priv->tty_status == MR_TTY_HSTA_DEVICE_RUN &&
        (tty_msg_pkt->flag == MR_TTY_FLAG_UPLD_DATA || tty_msg_pkt->flag == MR_TTY_FLAG_CREDIT_UPDATE)) {
        /* Update flow control */
        if (tty_msg_pkt->credit_update_flag) {
            mr_tty_credit_limit_update(priv, tty_msg_pkt->credit_limit_cnt);
        }

        if (tty_msg_pkt->flag == MR_TTY_FLAG_UPLD_DATA) {
            mr_tty_process_data_packet(priv, tty_msg_pkt);
        }

        dev_kfree_skb_any(skb);
        return 0;
    }
#endif

    /* Queue control packets for daemon thread */
    skb_queue_tail(&priv->receive_skb_head, skb);
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
static int mr_tty_dnld_send_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct mr_tty_priv *priv = (struct mr_tty_priv *)arg;
    struct mr_tty_msg_pkt *tty_msg_pkt;
    uint16_t data_len = 0;

    tty_msg_pkt = (struct mr_tty_msg_pkt *)skb->data;
    data_len = MR_TTY_GET_DATA_LEN(tty_msg_pkt->msg_pkt.len);

    if (success) {
        TTY_DBG(priv, "Send completed successfully: len=%u\n", data_len);
    } else {
        TTY_DBG(priv, "Send failed: len=%u\n", data_len);
        if (tty_msg_pkt->flag == MR_TTY_FLAG_DNLD_DATA) {
            mr_tty_credit_put(priv);
        }
    }

    dev_kfree_skb_any(skb);
    return 0;
}

/**
 * @brief Process received packets and manage state machine
 * @param[in] priv TTY private structure
 * @retval 0 Success
 * @retval 1 No packet available or skipped (RESET/ERROR state)
 * @retval 2 Queue empty
 * @retval 3 State transition to RESET occurred
 * @details This function processes one packet from the receive queue
 *          and handles state machine transitions.
 */
static int mr_tty_received_process(struct mr_tty_priv *priv)
{
    unsigned long flags;
    struct sk_buff *skb;
    struct mr_tty_msg_pkt *tty_msg_pkt;

    if (priv->tty_status == MR_TTY_HSTA_RESET || priv->tty_status == MR_TTY_HSTA_ERROR) {
        /* In RESET or ERROR state, skip processing received packets */
        return 1;
    }

    /* Try to get skb for processing (received from device) */
    skb = skb_dequeue(&priv->receive_skb_head);
    if (skb == NULL) {
        return 2;
    }
    tty_msg_pkt = (struct mr_tty_msg_pkt *)skb->data;

    /* Anytime receiving DEVICE_RESET or DEVICE_STOP, need to return to initial state */
    if (tty_msg_pkt->flag == MR_TTY_FLAG_DEVICE_RESET ||
        tty_msg_pkt->flag == MR_TTY_FLAG_DEVICE_STOP) {
        /* Hangup any open TTY connections to release user space references immediately */
        tty_port_hangup(&priv->port);

        /* Change state to reset */
        priv->tty_status = MR_TTY_HSTA_RESET;
        dev_kfree_skb_any(skb);

        TTY_INFO(priv, "Received DEVICE_RESET/STOP in state %d, returning to HSTA_RESET\n", priv->tty_status);
        return 3;
    }

    /* State machine */
    switch (priv->tty_status) {
        case MR_TTY_HSTA_HOST_READY:
            if (tty_msg_pkt->flag != MR_TTY_FLAG_DEVICE_START) {
                break;
            }

            /* Process initial credit limit */
            if (tty_msg_pkt->credit_update_flag == false || tty_msg_pkt->credit_limit_cnt == 0) {
                spin_lock_irqsave(&priv->credit_lock, flags);
                priv->credit_nolimit = true;
                spin_unlock_irqrestore(&priv->credit_lock, flags);
                TTY_INFO(priv, "Device Ready! No Credit Limit\n");
            } else {
                spin_lock_irqsave(&priv->credit_lock, flags);
                priv->credit_limit_cnt = tty_msg_pkt->credit_limit_cnt;
                priv->credit_consumed_cnt = 0;
                priv->credit_nolimit = false;
                spin_unlock_irqrestore(&priv->credit_lock, flags);
                TTY_INFO(priv, "Device Ready! Credit Limit=%u\n", tty_msg_pkt->credit_limit_cnt);
            }

            /* Device is now ready for communication */
            priv->tty_status = MR_TTY_HSTA_DEVICE_RUN;
            break;

        case MR_TTY_HSTA_DEVICE_RUN:
            if (tty_msg_pkt->flag == MR_TTY_FLAG_UPLD_DATA) {
                /* Process credit update */
                if (tty_msg_pkt->credit_update_flag) {
                    mr_tty_credit_limit_update(priv, tty_msg_pkt->credit_limit_cnt);
                }
                /* Process data */
                mr_tty_process_data_packet(priv, tty_msg_pkt);

            } else if (tty_msg_pkt->flag == MR_TTY_FLAG_CREDIT_UPDATE) {
                /* Credit update */
                if (tty_msg_pkt->credit_update_flag) {
                    mr_tty_credit_limit_update(priv, tty_msg_pkt->credit_limit_cnt);
                }
            }
            break;

        case MR_TTY_HSTA_RESET:
            break;

        case MR_TTY_HSTA_ERROR:
            break;

        default:
            TTY_ERR(priv, "Unknown status: %d\n", priv->tty_status);
            break;
    }

    /* Release skb_recv */
    dev_kfree_skb_any(skb);

    return 0;
}

/**
 * @brief TTY daemon thread main function
 * @param[in] data TTY private structure
 * @retval 0 Thread exit
 * @details Main daemon thread that manages TTY state machine and processes
 *          control packets. Similar to eth_netdev daemon thread.
 */
static int mr_tty_daemon_thread(void *data)
{
    int ret;
    struct mr_tty_priv *priv = (struct mr_tty_priv *)data;

    TTY_INFO(priv, "TTY daemon thread started\n");

thread_restart:
    ret = mr_tty_send_cmd_packet(priv, MR_TTY_FLAG_HOST_RESET);
    if (ret < 0) {
        priv->tty_status = MR_TTY_HSTA_ERROR;
        TTY_ERR(priv, "Failed to send HOST_READY command: %d\n", ret);
        goto exit;
    }
    priv->tty_status = MR_TTY_HSTA_RESET;

thread_reset:
    /* Clear queues */
    skb_queue_purge(&priv->receive_skb_head);
    /* Send HOST_READY packet to indicate HOST is ready */
    ret = mr_tty_send_cmd_packet(priv, MR_TTY_FLAG_HOST_READY);
    if (ret < 0) {
        priv->tty_status = MR_TTY_HSTA_ERROR;
        TTY_ERR(priv, "Failed to send HOST_READY command: %d\n", ret);
        goto exit;
    }
    TTY_INFO(priv, "Host ready, waiting for device start\n");
    priv->tty_status = MR_TTY_HSTA_HOST_READY;

    while (1) {
        /* Suspend and wait for wake-up for various reasons */
        wait_event_interruptible(priv->waitq, !skb_queue_empty(&priv->receive_skb_head) ||
                                                  priv->thread_condition ||
                                                  kthread_should_stop());

        TTY_DBG(priv, "Daemon thread running, current status: %d\n", priv->tty_status);

        if (kthread_should_stop()) {
            break;
        } else if (priv->thread_condition) {
            usleep_range(500, 1000);
        }

        /* process received packet and handle state machine */
        while (mr_tty_received_process(priv) == 0) {}

        if (priv->tty_status == MR_TTY_HSTA_RESET) {
            usleep_range(500, 1000);
            goto thread_reset;
        }

        /* If in ERROR state, restart the thread */
        if (priv->tty_status == MR_TTY_HSTA_ERROR) {
            usleep_range(10000, 20000);
            goto thread_restart;
        }
    }

exit:
    if (priv->tty_status != MR_TTY_HSTA_ERROR) {
        ret = mr_tty_send_cmd_packet(priv, MR_TTY_FLAG_HOST_STOP);
        if (ret < 0) {
            TTY_ERR(priv, "failed to send stop packet: %d\n", ret);
        }
    }

    /* Wait for kthread_stop() to be called to avoid premature thread exit */
    while (!kthread_should_stop()) {
        wait_event_interruptible(priv->waitq, kthread_should_stop() || priv->thread_condition);
        usleep_range(500, 1000);
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
static int mr_tty_open(struct tty_struct *tty, struct file *filp)
{
    struct mr_tty_priv *priv = tty->driver->driver_state;

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
static void mr_tty_close(struct tty_struct *tty, struct file *filp)
{
    struct mr_tty_priv *priv = tty->driver_data;

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
 * @details This function signature differs between kernel versions:
 * - Linux 6.5: int count parameter
 * - Linux 6.6+: size_t count parameter
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 6, 0)
static int mr_tty_write(struct tty_struct *tty, const unsigned char *buf, int count)
#else
static ssize_t mr_tty_write(struct tty_struct *tty, const unsigned char *buf, size_t count)
#endif
{
    struct mr_tty_priv *priv = tty->driver_data;
    struct sk_buff *skb;
    struct mr_tty_msg_pkt *tty_msg_pkt;
    size_t data_len;
    int ret;

    TTY_DBG(priv, "mr_tty_write called: count=%zu\n", count);

    if (!priv || !buf || count == 0) {
        TTY_DBG(priv, "mr_tty_write: Invalid parameters\n");
        return -EINVAL;
    }

    if (priv->tty_status != MR_TTY_HSTA_DEVICE_RUN || !priv->is_open) {
        TTY_DBG(priv, "mr_tty_write: Device not ready, status=%d, is_open=%d\n", priv->tty_status, priv->is_open);
        return -EAGAIN;
    }

    /* Check if we have credits available */
    if (mr_tty_credit_get(priv) < 0) {
        return 0; /* No credits, TTY layer will wait and retry */
    }

    /* Limit data length */
    data_len = min(count, MR_TTY_MAX_DATA_LEN);

    /* Allocate SKB */
    skb = dev_alloc_skb(sizeof(struct mr_tty_msg_pkt) + data_len);
    if (!skb) {
        mr_tty_credit_put(priv);
        return -ENOMEM;
    }

    /* Build packet */
    tty_msg_pkt = (struct mr_tty_msg_pkt *)skb_put(skb, sizeof(struct mr_tty_msg_pkt) + data_len);

    tty_msg_pkt->msg_pkt.tag = priv->msg_tag;
    tty_msg_pkt->msg_pkt.sub_tag = 0;
    tty_msg_pkt->msg_pkt.len = sizeof(struct mr_tty_msg_pkt) - sizeof(struct mr_msg_pkt) + data_len;

    tty_msg_pkt->reserved[0] = 0;
    tty_msg_pkt->flag = MR_TTY_FLAG_DNLD_DATA;
    tty_msg_pkt->credit_update_flag = 0;
    tty_msg_pkt->credit_limit_cnt = 0;

    /* Copy data */
    memcpy(tty_msg_pkt->data, buf, data_len);

    TTY_DBG(priv, "Sent data: %zu bytes\n", data_len);

    /* Send packet */
    ret = mr_msg_ctrl_send(priv->msg_ctrl, skb);
    if (ret < 0) {
        TTY_ERR(priv, "Failed to send data packet: ret=%d\n", ret);
        mr_tty_credit_put(priv);
        dev_kfree_skb_any(skb);
        return ret;
    }

    return data_len;
}

/**
 * @brief TTY write room operation
 * @param[in] tty TTY structure
 * @retval Number of bytes that can be written
 * @details This function signature differs between kernel versions:
 * - Linux 5.13: returns int
 * - Linux 5.14+: returns unsigned int
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 14, 0)
static int mr_tty_write_room(struct tty_struct *tty)
#else
static unsigned int mr_tty_write_room(struct tty_struct *tty)
#endif
{
    struct mr_tty_priv *priv = tty->driver_data;
    unsigned long flags;
    int credits_available;

    if (!priv || priv->tty_status != MR_TTY_HSTA_DEVICE_RUN) {
        return 0;
    }

    spin_lock_irqsave(&priv->credit_lock, flags);

    if (priv->credit_nolimit) {
        spin_unlock_irqrestore(&priv->credit_lock, flags);
        return MR_TTY_MAX_DATA_LEN;
    }

    credits_available = (int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt);
    spin_unlock_irqrestore(&priv->credit_lock, flags);

    if (credits_available <= 0) {
        return 0;
    } else {
        return MR_TTY_MAX_DATA_LEN;
    }
}

/**
 * @brief TTY chars in buffer operation
 * @param[in] tty TTY structure
 * @retval Number of characters in buffer (always 0 for this implementation)
 * @details This function signature differs between kernel versions:
 * - Linux 5.13: returns int
 * - Linux 5.14+: returns unsigned int
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 14, 0)
static int mr_tty_chars_in_buffer(struct tty_struct *tty)
#else
static unsigned int mr_tty_chars_in_buffer(struct tty_struct *tty)
#endif
{
    return 0; // No local buffering
}

/**
 * @brief TTY flush buffer operation
 * @param[in] tty TTY structure
 */
static void mr_tty_flush_buffer(struct tty_struct *tty)
{
    /* No local buffering to flush */
}

/**
 * @brief TTY hangup operation
 * @param[in] tty TTY structure
 */
static void mr_tty_hangup(struct tty_struct *tty)
{
    struct mr_tty_priv *priv = tty->driver_data;

    if (!priv) {
        return;
    }

    priv->is_open = false;

    TTY_INFO(priv, "TTY hangup called by user space\n");

    /* Send hangup command to device */
    mr_tty_send_cmd_packet(priv, MR_TTY_FLAG_HANGUP);
}

/* TTY driver operations structure */
static const struct tty_operations mr_tty_ops = {
    .open = mr_tty_open,
    .close = mr_tty_close,
    .write = mr_tty_write,
    .write_room = mr_tty_write_room,
    .chars_in_buffer = mr_tty_chars_in_buffer,
    .flush_buffer = mr_tty_flush_buffer,
    .hangup = mr_tty_hangup,
};

/* TTY port operations */

/**
 * @brief TTY port activate operation
 * @param[in] port TTY port structure
 * @param[in] tty TTY structure
 * @retval 0 Success
 * @retval <0 Error code
 */
static int mr_tty_port_activate(struct tty_port *port, struct tty_struct *tty)
{
    struct mr_tty_priv *priv = container_of(port, struct mr_tty_priv, port);

    /* Check if already opened - only single open supported */
    if (priv->is_open) {
        TTY_INFO(priv, "TTY already opened, only single open supported\n");
        return -EBUSY;
    }

    /* Device must be in RUN state to accept opens */
    if (priv->tty_status != MR_TTY_HSTA_DEVICE_RUN) {
        TTY_DBG(priv, "TTY not ready for open (state: %d)\n", priv->tty_status);
        return -EIO; /* Use EIO instead of EAGAIN to avoid infinite retry */
    }

    priv->is_open = true;
    TTY_INFO(priv, "TTY port activated\n");

    /* Send open command to device */
    mr_tty_send_cmd_packet(priv, MR_TTY_FLAG_OPEN);

    return 0;
}

/**
 * @brief TTY port shutdown operation
 * @param[in] port TTY port structure
 */
static void mr_tty_port_shutdown(struct tty_port *port)
{
    struct mr_tty_priv *priv = container_of(port, struct mr_tty_priv, port);

    priv->is_open = false;
    TTY_INFO(priv, "TTY port shutdown called\n");

    /* Send close command to device */
    mr_tty_send_cmd_packet(priv, MR_TTY_FLAG_CLOSE);
}

static const struct tty_port_operations mr_tty_port_ops = {
    .activate = mr_tty_port_activate,
    .shutdown = mr_tty_port_shutdown,
};

/**
 * @brief Initialize TTY driver with dynamic device management
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @param[in] device_name Name of the TTY device to create
 * @param[in] idx Index of the TTY device
 * @retval Pointer to TTY private structure on success
 * @retval NULL on failure
 */
struct mr_tty_priv *mr_tty_init(struct mr_msg_ctrl *msg_ctrl, uint8_t msg_tag, char *device_name, uint8_t idx)
{
    struct mr_tty_priv *priv;
    int ret;

    if (!msg_ctrl) {
        pr_err("TTY: Invalid msg_ctrl\n");
        return NULL;
    }

    /* Allocate private structure */
    priv = kzalloc(sizeof(struct mr_tty_priv), GFP_KERNEL);
    if (!priv) {
        pr_err("TTY: Failed to allocate private structure\n");
        return NULL;
    }

    priv->msg_ctrl = msg_ctrl;
    priv->msg_tag = msg_tag;

    /* Initialize synchronization objects */
    spin_lock_init(&priv->credit_lock);
    init_waitqueue_head(&priv->waitq);

    /* Initialize SKB queue for daemon thread */
    skb_queue_head_init(&priv->receive_skb_head);

    /* Initialize TTY port */
    tty_port_init(&priv->port);
    priv->port.ops = &mr_tty_port_ops;

    /* Allocate TTY driver */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
    priv->tty_driver = alloc_tty_driver(1);
#else
    priv->tty_driver = tty_alloc_driver(1, MR_TTY_FLAGS);
#endif
    if (IS_ERR(priv->tty_driver)) {
        pr_err("TTY: Failed to allocate TTY driver\n");
        ret = PTR_ERR(priv->tty_driver);
        goto cleanup_port;
    }

    /* Setup TTY driver */
    priv->tty_driver->driver_name = MR_TTY_DRIVER_NAME;
    priv->tty_driver->name = device_name;
    priv->tty_driver->major = MR_TTY_MAJOR;
    priv->tty_driver->minor_start = 0;
    priv->tty_driver->name_base = idx;
    priv->tty_driver->type = MR_TTY_TYPE;
    priv->tty_driver->subtype = MR_TTY_SUBTYPE;
    priv->tty_driver->init_termios = tty_std_termios;
    priv->tty_driver->driver_state = priv;
    tty_set_operations(priv->tty_driver, &mr_tty_ops);
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
    priv->tty_driver->flags = MR_TTY_FLAGS;
#else
    tty_port_link_device(&priv->port, priv->tty_driver, 0);
#endif

    /* Register TTY driver (but not the device yet) */
    ret = tty_register_driver(priv->tty_driver);
    if (ret < 0) {
        pr_err("TTY: Failed to register TTY driver: %d\n", ret);
        goto free_tty_driver;
    }

    /* Create network daemon thread */
    priv->daemon_thread = kthread_create(mr_tty_daemon_thread, priv, "%s_daemon", device_name);
    if (IS_ERR(priv->daemon_thread)) {
        pr_err("TTY: Failed to create daemon thread\n");
        ret = PTR_ERR(priv->daemon_thread);
        goto unregister_tty;
    }

    /* Register message callbacks */
    ret = mr_msg_cb_register(priv->msg_ctrl, priv->msg_tag, mr_tty_upld_recv_cb, priv, mr_tty_dnld_send_cb, priv);
    if (ret < 0) {
        pr_err("TTY: Failed to register message callbacks: %d\n", ret);
        goto stop_daemon;
    }

    /* Register TTY device immediately - it will be available when device is ready */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
    priv->tty_dev = tty_register_device(priv->tty_driver, 0, NULL);
#else
    priv->tty_dev = tty_port_register_device(&priv->port, priv->tty_driver, 0, NULL);
#endif
    if (IS_ERR(priv->tty_dev)) {
        pr_err("TTY: Failed to register TTY device\n");
        ret = PTR_ERR(priv->tty_dev);
        priv->tty_dev = NULL;
        goto unregister_callbacks;
    }

    wake_up_process(priv->daemon_thread);

    TTY_INFO(priv, "TTY driver initialized successfully\n");
    return priv;

/* Error cleanup paths */
unregister_callbacks:
    mr_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);
stop_daemon:
    kthread_stop(priv->daemon_thread);
unregister_tty:
    tty_unregister_driver(priv->tty_driver);
free_tty_driver:
    tty_driver_kref_put(priv->tty_driver);
cleanup_port:
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
#else
    tty_port_destroy(&priv->port);
#endif
    kfree(priv);
    return NULL;
}

/**
 * @brief Deinitialize TTY driver
 * @param[in] priv TTY private structure to cleanup
 */
void mr_tty_deinit(struct mr_tty_priv *priv)
{
    if (!priv) {
        return;
    }

    TTY_INFO(priv, "TTY driver deinitializing\n");

    /* Unregister message callbacks */
    mr_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);

    /* Stop daemon thread */
    if (priv->daemon_thread) {
        priv->thread_condition = true;
        wake_up_interruptible(&priv->waitq);
        kthread_stop(priv->daemon_thread);
        priv->daemon_thread = NULL;
    }

    /* Unregister TTY device first if registered */
    if (priv->tty_dev) {
        tty_port_hangup(&priv->port);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0)
        tty_unregister_device(priv->tty_driver, 0);
#else
        tty_port_unregister_device(&priv->port, priv->tty_driver, 0);
#endif
        tty_driver_kref_put(priv->tty_driver);
        tty_unregister_driver(priv->tty_driver);
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
#else
        tty_port_destroy(&priv->port);
#endif
        priv->tty_dev = NULL;
    }

    skb_queue_purge(&priv->receive_skb_head);

    /* Free private structure */
    kfree(priv);

    pr_info("TTY: Driver deinitialized successfully\n");
}
