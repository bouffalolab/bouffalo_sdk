/**
 * @file mr_virtualchan.c
 * @brief Virtual channel interface implementation for kernel-userspace communication
 * @details Virtual channel driver implementation with state machine, flow control, and bidirectional communication, similar to TTY implementation
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <net/sock.h>

#include "mr_virtualchan.h"
#include "mr_debugfs.h"

/* Forward declarations */
static int mr_virtualchan_daemon_thread(void *data);
static int mr_virtualchan_upld_recv_cb(struct sk_buff *skb, bool success, void *arg);
static int mr_virtualchan_dnld_send_cb(struct sk_buff *skb, bool success, void *arg);

/**
 * @brief Update flow control credit limit
 * @param[in] priv Virtual channel private structure
 * @param[in] credit_limit_new New credit limit value
 * @retval 0 Always returns 0
 * @details This function updates flow control credit limit
 */
static int mr_virtualchan_credit_limit_update(struct mr_virtualchan_priv *priv, uint8_t credit_limit_new)
{
    unsigned long flags;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if ((int8_t)(credit_limit_new - priv->credit_limit_cnt) > 0) {
        priv->credit_limit_cnt = credit_limit_new;
        VIRTUALCHAN_DBG(priv, "Credit limit updated to %u\n", credit_limit_new);
    } else {
        VIRTUALCHAN_DBG(priv, "Credit limit not updated, old=%u, new=%u\n", priv->credit_limit_cnt, credit_limit_new);
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    /* Wake up daemon thread to try sending queued packets */
    wake_up_interruptible(&priv->waitq);

    return 0;
}

/**
 * @brief Consume one flow control credit for packet transmission
 * @param[in] priv Virtual channel private structure
 * @retval 0 Success, credit consumed
 * @retval -1 No credit available
 * @details This function decrements consumed credit counter
 */
static int mr_virtualchan_credit_get(struct mr_virtualchan_priv *priv)
{
    unsigned long flags;
    int ret = -1;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if ((int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt) > 0) {
        priv->credit_consumed_cnt++;
        ret = 0;
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    return ret;
}

/**
 * @brief Query available flow control credits
 * @param[in] priv Virtual channel private structure
 * @return Number of available credits (limit - consumed)
 * @details This function returns number of credits available for packet transmission
 */
static int mr_virtualchan_credit_available(struct mr_virtualchan_priv *priv)
{
    unsigned long flags;
    int available;

    spin_lock_irqsave(&priv->credit_lock, flags);
    available = (int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt);
    spin_unlock_irqrestore(&priv->credit_lock, flags);

    return available;
}

/**
 * @brief Return one flow control credit after transmission failure
 * @param[in] priv Virtual channel private structure
 * @retval 0 Always returns 0
 * @details This function returns one consumed credit
 */
static int mr_virtualchan_credit_put(struct mr_virtualchan_priv *priv)
{
    unsigned long flags;

    spin_lock_irqsave(&priv->credit_lock, flags);

    priv->credit_consumed_cnt--;

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    return 0;
}

/**
 * @brief Process received packet and forward to userspace
 * @param[in] priv Virtual channel private structure
 * @param[in] virtualchan_msg_pkt Message packet containing data
 * @retval 0 Success
 * @details This function processes incoming packets and sends them to userspace
 *          via netlink socket using multicast
 */
static int mr_virtualchan_send_to_userspace(struct mr_virtualchan_priv *priv, struct mr_virtualchan_msg_pkt *virtualchan_msg_pkt)
{
    uint16_t data_len;
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;
    int ret;

    data_len = MR_VIRTUALCHAN_GET_DATA_LEN(virtualchan_msg_pkt->msg_pkt.len);
    if (data_len == 0) {
        VIRTUALCHAN_ERR(priv, "Received empty data packet\n");
        return -EINVAL;
    }

    VIRTUALCHAN_INFO(priv, "Received data[%d]\n", data_len);

    /* Allocate netlink message */
    nl_skb = nlmsg_new(data_len, GFP_KERNEL);
    if (!nl_skb) {
        VIRTUALCHAN_ERR(priv, "Failed to allocate netlink message\n");
        return -ENOMEM;
    }

    /* Fill netlink message header */
    nlh = nlmsg_put(nl_skb, 0, 0, NLMSG_DONE, data_len, 0);
    if (!nlh) {
        VIRTUALCHAN_ERR(priv, "Failed to put netlink header\n");
        kfree_skb(nl_skb);
        return -EMSGSIZE;
    }

    /* Copy data to netlink message */
    memcpy(nlmsg_data(nlh), virtualchan_msg_pkt->data, data_len);

    /* Multicast to all subscribed userspace clients */
    ret = nlmsg_multicast(priv->nl_sock, nl_skb, 0, MR_VIRTUALCHAN_MULTICAST_GROUP, GFP_KERNEL);
    if (ret < 0) {
        if (ret == -ESRCH) {
            /* No listeners, expected in multicast mode */
            VIRTUALCHAN_DBG(priv, "No multicast listeners for data packet\n");
        } else {
            VIRTUALCHAN_ERR(priv, "Failed to multicast netlink message: %d\n", ret);
        }
        /* nlmsg_multicast frees skb on failure */
        return ret;
    }

    VIRTUALCHAN_DBG(priv, "Multicasted data to userspace: len=%u\n", data_len);
    return 0;
}

/**
 * @brief Send control packet to device
 * @param[in] priv Virtual channel private structure
 * @param[in] flag Command flag
 * @retval 0 Success
 * @retval <0 Error code
 */
static int mr_virtualchan_send_cmd_packet(struct mr_virtualchan_priv *priv, uint8_t flag)
{
    struct sk_buff *skb;
    struct mr_virtualchan_msg_pkt *virtualchan_msg_pkt;
    int ret;

    skb = dev_alloc_skb(sizeof(struct mr_virtualchan_msg_pkt));
    if (!skb) {
        VIRTUALCHAN_ERR(priv, "Failed to allocate skb for command packet\n");
        return -ENOMEM;
    }

    virtualchan_msg_pkt = (struct mr_virtualchan_msg_pkt *)skb_put(skb, sizeof(struct mr_virtualchan_msg_pkt));

    virtualchan_msg_pkt->msg_pkt.tag = priv->msg_tag;
    virtualchan_msg_pkt->msg_pkt.sub_tag = 0;
    virtualchan_msg_pkt->msg_pkt.len = sizeof(struct mr_virtualchan_msg_pkt) - sizeof(struct mr_msg_pkt);

    virtualchan_msg_pkt->reserved[0] = 0;
    virtualchan_msg_pkt->flag = flag;
    virtualchan_msg_pkt->credit_update_flag = 0;
    virtualchan_msg_pkt->credit_limit_cnt = 0;

    ret = mr_msg_ctrl_send(priv->msg_ctrl, skb);
    if (ret < 0) {
        VIRTUALCHAN_ERR(priv, "Failed to send command packet: %d\n", ret);
        dev_kfree_skb_any(skb);
        return ret;
    }

    VIRTUALCHAN_DBG(priv, "Sent command packet: flag=0x%02x\n", flag);
    return 0;
}

/**
 * @brief Message controller socket buffer receive callback
 * @param[in] skb Socket buffer containing received data
 * @param[in] success Receive success flag (always true for received messages)
 * @param[in] arg Virtual channel private structure parameter
 * @retval Always returns 0
 * @details This callback processes virtual channel packets received from device.
 *          For RUN state packets, process directly for speed.
 *          For control packets, queue them for daemon thread processing.
 */
static int mr_virtualchan_upld_recv_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct mr_virtualchan_priv *priv = (struct mr_virtualchan_priv *)arg;
    struct mr_virtualchan_msg_pkt *virtualchan_msg_pkt;

    if (skb->len < sizeof(struct mr_virtualchan_msg_pkt)) {
        VIRTUALCHAN_ERR(priv, "Received packet too small: %u\n", skb->len);
        dev_kfree_skb_any(skb);
        return 0;
    }

    virtualchan_msg_pkt = (struct mr_virtualchan_msg_pkt *)skb->data;

#if 0
    /* Fast path for RUN state packets, avoid waking kthread */
    if (priv->virtualchan_status == MR_VIRTUALCHAN_HSTA_DEVICE_RUN &&
        (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_UPLD_DATA ||
         virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_CREDIT_UPDATE)) {
        /* Update flow control, must use lock protection */
        if (virtualchan_msg_pkt->credit_update_flag) {
            mr_virtualchan_credit_limit_update(priv, virtualchan_msg_pkt->credit_limit_cnt);
        }

        if (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_UPLD_DATA) {
            mr_virtualchan_send_to_userspace(priv, virtualchan_msg_pkt);
        }

        dev_kfree_skb_any(skb);
        return 0;
    }
#endif

    /* Queue control packets for daemon thread processing */
    skb_queue_tail(&priv->receive_skb_head, skb);
    wake_up_interruptible(&priv->waitq);

    return 0;
}

/**
 * @brief Message controller socket buffer send callback
 * @param[in] skb Socket buffer that was sent
 * @param[in] success Transmission success flag
 * @param[in] arg Virtual channel private structure parameter
 * @retval Always returns 0
 * @details This callback handles send completion events
 */
static int mr_virtualchan_dnld_send_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct mr_virtualchan_priv *priv = (struct mr_virtualchan_priv *)arg;
    struct mr_virtualchan_msg_pkt *virtualchan_msg_pkt;
    uint32_t data_len;

    /* Extract data length from message packet */
    virtualchan_msg_pkt = (struct mr_virtualchan_msg_pkt *)skb->data;
    data_len = MR_VIRTUALCHAN_GET_DATA_LEN(virtualchan_msg_pkt->msg_pkt.len);

    if (success) {
        VIRTUALCHAN_DBG(priv, "Send completed successfully: len=%u\n", data_len);
    } else {
        VIRTUALCHAN_ERR(priv, "Send failed: len=%u\n", data_len);

        if (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_DNLD_DATA) {
            mr_virtualchan_credit_put(priv);
        }
    }

    dev_kfree_skb_any(skb);
    return 0;
}

/**
 * @brief Virtual channel receive handler for processing messages from userspace
 * @param[in] skb Socket buffer from userspace
 * @details This function processes messages received from userspace via netlink
 *          and forwards them to device through msg_ctrl
 */
static void mr_virtualchan_recv_from_userspace(struct sk_buff *skb)
{
    struct mr_virtualchan_priv *priv;
    struct nlmsghdr *nlh;
    struct sk_buff *dev_skb;
    struct mr_virtualchan_msg_pkt *virtualchan_msg_pkt;
    uint16_t data_len;

    /* Get netlink socket private data */
    priv = (struct mr_virtualchan_priv *)skb->sk->sk_user_data;
    if (!priv) {
        VIRTUALCHAN_ERR(NULL, "No private data in netlink socket\n");
        return;
    }

    nlh = nlmsg_hdr(skb);

    /* Log client connection for debugging (in multicast mode, we don't track individual clients) */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
    VIRTUALCHAN_INFO(priv, "Received message from portid=%u\n", NETLINK_CB(skb).pid);
#else
    VIRTUALCHAN_INFO(priv, "Received message from portid=%u\n", NETLINK_CB(skb).portid);
#endif

    data_len = nlmsg_len(nlh);
    if (data_len == 0) {
        VIRTUALCHAN_ERR(priv, "Received empty message from userspace\n");
        return;
    }

    if (data_len > MR_VIRTUALCHAN_MAX_DATA_LEN) {
        VIRTUALCHAN_ERR(priv, "Data too large: %u > %u\n", data_len, MR_VIRTUALCHAN_MAX_DATA_LEN);
        return;
    }

    /* Check device status */
    if (priv->virtualchan_status != MR_VIRTUALCHAN_HSTA_DEVICE_RUN) {
        VIRTUALCHAN_INFO(priv, "Device not in RUN state, dropping packet\n");
        return;
    }

    /* Allocate device SKB */
    dev_skb = dev_alloc_skb(sizeof(struct mr_virtualchan_msg_pkt) + data_len);
    if (!dev_skb) {
        VIRTUALCHAN_ERR(priv, "Failed to allocate device SKB\n");
        return;
    }

    /* Fill message packet */
    virtualchan_msg_pkt = (struct mr_virtualchan_msg_pkt *)skb_put(dev_skb, sizeof(struct mr_virtualchan_msg_pkt) + data_len);
    virtualchan_msg_pkt->msg_pkt.tag = priv->msg_tag;
    virtualchan_msg_pkt->msg_pkt.sub_tag = 0;
    virtualchan_msg_pkt->msg_pkt.len = sizeof(struct mr_virtualchan_msg_pkt) - sizeof(struct mr_msg_pkt) + data_len;

    virtualchan_msg_pkt->reserved[0] = 0;
    virtualchan_msg_pkt->flag = MR_VIRTUALCHAN_FLAG_DNLD_DATA;
    virtualchan_msg_pkt->credit_update_flag = 0;
    virtualchan_msg_pkt->credit_limit_cnt = 0;

    /* Copy data from userspace message */
    memcpy(virtualchan_msg_pkt->data, nlmsg_data(nlh), data_len);

    /* Queue to transmit queue for daemon thread to send in order */
    skb_queue_tail(&priv->transmit_skb_head, dev_skb);
    wake_up_interruptible(&priv->waitq);

    VIRTUALCHAN_DBG(priv, "Queued data for device (transmit queue): len=%u\n", data_len);
}

/**
 * @brief Process received packets and manage state machine
 * @param[in] priv Virtual channel private structure
 * @retval 0 Success
 * @retval <0 Error code (caller should exit thread on fatal error)
 * @details This function processes one packet from receive queue
 *          and handles state machine transitions
 */
static int mr_virtualchan_received_process(struct mr_virtualchan_priv *priv)
{
    unsigned long flags;
    struct sk_buff *skb;
    struct mr_virtualchan_msg_pkt *virtualchan_msg_pkt;

    if (priv->virtualchan_status == MR_VIRTUALCHAN_HSTA_RESET || priv->virtualchan_status == MR_VIRTUALCHAN_HSTA_ERROR) {
        /* In RESET or ERROR state, skip processing received packets */
        return 1;
    }

    /* Try to get skb for processing (received from device) */
    skb = skb_dequeue(&priv->receive_skb_head);
    if (skb == NULL) {
        return 2;
    }
    virtualchan_msg_pkt = (struct mr_virtualchan_msg_pkt *)skb->data;

    /* DEVICE_RESET or DEVICE_STOP received at any time, return to initial state */
    if (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_DEVICE_RESET ||
        virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_DEVICE_STOP) {
        /* Transition to reset state */
        priv->virtualchan_status = MR_VIRTUALCHAN_HSTA_RESET;
        dev_kfree_skb_any(skb);

        VIRTUALCHAN_INFO(priv, "Received DEVICE_RESET/STOP in state %d, returning to HSTA_RESET\n", priv->virtualchan_status);
        return 3;
    }

    /* State machine */
    switch (priv->virtualchan_status) {
        case MR_VIRTUALCHAN_HSTA_HOST_READY:
            if (virtualchan_msg_pkt->flag != MR_VIRTUALCHAN_FLAG_DEVICE_START) {
                break;
            }

            /* Process initial credit limit */
            if (virtualchan_msg_pkt->credit_update_flag == false || virtualchan_msg_pkt->credit_limit_cnt == 0) {
                VIRTUALCHAN_INFO(priv, "Device ready with no credit limit\n");
            } else {
                spin_lock_irqsave(&priv->credit_lock, flags);
                priv->credit_limit_cnt = virtualchan_msg_pkt->credit_limit_cnt;
                priv->credit_consumed_cnt = 0;
                spin_unlock_irqrestore(&priv->credit_lock, flags);
                priv->virtualchan_status = MR_VIRTUALCHAN_HSTA_DEVICE_RUN;

                VIRTUALCHAN_INFO(priv, "Device ready! Credit Limit=%u\n", virtualchan_msg_pkt->credit_limit_cnt);
            }
            break;

        case MR_VIRTUALCHAN_HSTA_DEVICE_RUN:
            if (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_UPLD_DATA) {
                /* Process credit update */
                if (virtualchan_msg_pkt->credit_update_flag) {
                    mr_virtualchan_credit_limit_update(priv, virtualchan_msg_pkt->credit_limit_cnt);
                }

                /* Process data */
                mr_virtualchan_send_to_userspace(priv, virtualchan_msg_pkt);

            } else if (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_CREDIT_UPDATE) {
                /* Credit update */
                if (virtualchan_msg_pkt->credit_update_flag) {
                    mr_virtualchan_credit_limit_update(priv, virtualchan_msg_pkt->credit_limit_cnt);
                }
            }
            break;

        case MR_VIRTUALCHAN_HSTA_RESET:
            break;

        case MR_VIRTUALCHAN_HSTA_ERROR:
            break;

        default:
            VIRTUALCHAN_ERR(priv, "Unknown status: %d\n", priv->virtualchan_status);
            break;
    }

    /* Free skb_recv */
    dev_kfree_skb_any(skb);

    return 0;
}

/**
 * @brief Process transmit queue for packets to be sent to device
 * @param[in] priv Virtual channel private structure
 * @retval 0 Success
 * @retval <0 Error code (caller should exit thread on fatal error)
 * @details This function processes pending packets in transmit queue.
 *          Only sends packets when in DEVICE_RUN state and with available credits.
 */
static int mr_virtualchan_transmit_process(struct mr_virtualchan_priv *priv)
{
    int ret;
    struct sk_buff *tx_skb;

    /* Only send in DEVICE_RUN state */
    if (priv->virtualchan_status != MR_VIRTUALCHAN_HSTA_DEVICE_RUN) {
        if (!skb_queue_empty(&priv->transmit_skb_head)) {
            VIRTUALCHAN_ERR(priv, "Not in DEVICE_RUN state, clearing transmit queue\n");
            skb_queue_purge(&priv->transmit_skb_head);
        }
        return 1;
    }

    if (mr_virtualchan_credit_available(priv) <= 0) {
        /* No credit available, wait */
        return 2;
    }

    /* Dequeue and send */
    tx_skb = skb_dequeue(&priv->transmit_skb_head);
    if (!tx_skb) {
        return 3;
    }

    /* Consume credit (daemon ensures there is credit before calling) */
    if (mr_virtualchan_credit_get(priv) < 0) {
        /* Should not happen, but requeue and wait */
        skb_queue_head(&priv->transmit_skb_head, tx_skb);
        VIRTUALCHAN_ERR(priv, "Unexpected: no credits available, will retry\n");
        return 4;
    }

    ret = mr_msg_ctrl_send(priv->msg_ctrl, tx_skb);
    if (ret < 0) {
        /* Return consumed credit and requeue at head */
        mr_virtualchan_credit_put(priv);
        skb_queue_head(&priv->transmit_skb_head, tx_skb);

        priv->virtualchan_status = MR_VIRTUALCHAN_HSTA_ERROR;

        VIRTUALCHAN_ERR(priv, "Failed to send queued packet: %d\n", ret);
        return ret;
    }

    return 0;
}

/**
 * @brief Daemon thread for processing control packets
 * @param[in] data Virtual channel private structure
 * @retval 0 Thread exit
 * @details This thread processes control packets and manages state transitions
 */
static int mr_virtualchan_daemon_thread(void *data)
{
    int ret;
    struct mr_virtualchan_priv *priv = (struct mr_virtualchan_priv *)data;

    VIRTUALCHAN_INFO(priv, "Daemon thread started\n");

thread_restart:
    /* Send HOST_RESET command to initialize state machine */
    ret = mr_virtualchan_send_cmd_packet(priv, MR_VIRTUALCHAN_FLAG_HOST_RESET);
    if (ret < 0) {
        priv->virtualchan_status = MR_VIRTUALCHAN_HSTA_ERROR;
        VIRTUALCHAN_ERR(priv, "Failed to send HOST_RESET command: %d\n", ret);
        goto exit;
    }
    priv->virtualchan_status = MR_VIRTUALCHAN_HSTA_RESET;

thread_reset:
    /* Clear queues */
    skb_queue_purge(&priv->receive_skb_head);
    skb_queue_purge(&priv->transmit_skb_head);
    /* Send HOST_READY packet to indicate HOST is ready */
    ret = mr_virtualchan_send_cmd_packet(priv, MR_VIRTUALCHAN_FLAG_HOST_READY);
    if (ret < 0) {
        priv->virtualchan_status = MR_VIRTUALCHAN_HSTA_ERROR;
        VIRTUALCHAN_ERR(priv, "Failed to send HOST_READY command: %d\n", ret);
        goto exit;
    }
    VIRTUALCHAN_INFO(priv, "Host ready, waiting for device start\n");
    priv->virtualchan_status = MR_VIRTUALCHAN_HSTA_HOST_READY;

    while (1) {
        /* Sleep and wait for wake up from various reasons */
        wait_event_interruptible(priv->waitq, !skb_queue_empty(&priv->receive_skb_head) ||
                                                  (!skb_queue_empty(&priv->transmit_skb_head) &&
                                                   mr_virtualchan_credit_available(priv) > 0) ||
                                                  priv->thread_condition ||
                                                  kthread_should_stop());

        VIRTUALCHAN_DBG(priv, "Daemon thread running, current status: %d\n", priv->virtualchan_status);

        if (kthread_should_stop()) {
            goto exit;
        } else if (priv->thread_condition) {
            usleep_range(500, 1000);
        }

        /* Process received packets and handle state machine */
        while (mr_virtualchan_received_process(priv) == 0) {}

        /* Process transmit queue (send pending packets) */
        while (mr_virtualchan_transmit_process(priv) == 0) {}

        if (priv->virtualchan_status == MR_VIRTUALCHAN_HSTA_RESET) {
            usleep_range(500, 1000);
            goto thread_reset;
        }

        /* If in ERROR state, restart thread */
        if (priv->virtualchan_status == MR_VIRTUALCHAN_HSTA_ERROR) {
            usleep_range(10000, 20000);
            goto thread_restart;
        }
    }

exit:
    if (priv->virtualchan_status != MR_VIRTUALCHAN_HSTA_ERROR) {
        ret = mr_virtualchan_send_cmd_packet(priv, MR_VIRTUALCHAN_FLAG_HOST_STOP);
        if (ret < 0) {
            VIRTUALCHAN_ERR(priv, "Failed to send stop packet: %d\n", ret);
        }
    }

    /* Wait for kthread_stop() to be called to avoid premature thread exit */
    while (!kthread_should_stop()) {
        wait_event_interruptible(priv->waitq, kthread_should_stop() || priv->thread_condition);
        usleep_range(500, 1000);
    }

    VIRTUALCHAN_INFO(priv, "Daemon thread exiting, status: %d\n", priv->virtualchan_status);
    return 0;
}

/**
 * @brief Initialize virtual channel interface
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message routing tag
 * @retval Pointer to virtual channel private structure on success
 * @retval NULL on failure
 */
struct mr_virtualchan_priv *mr_virtualchan_init(struct mr_msg_ctrl *msg_ctrl, uint8_t msg_tag)
{
    int ret;
    struct mr_virtualchan_priv *priv;

    VIRTUALCHAN_INFO(NULL, "Initializing Virtualchan interface with msg_tag=%u\n", msg_tag);

    /* Allocate private structure */
    priv = kzalloc(sizeof(struct mr_virtualchan_priv), GFP_KERNEL);
    if (!priv) {
        VIRTUALCHAN_ERR(NULL, "Failed to allocate private structure\n");
        return NULL;
    }

    /* Initialize basic fields */
    priv->msg_ctrl = msg_ctrl;
    priv->msg_tag = msg_tag;

    /* Initialize locks */
    spin_lock_init(&priv->credit_lock);

    /* Initialize thread structures */
    skb_queue_head_init(&priv->receive_skb_head);
    skb_queue_head_init(&priv->transmit_skb_head);
    init_waitqueue_head(&priv->waitq);

    /* Initialize daemon thread to RESET state */
    priv->virtualchan_status = MR_VIRTUALCHAN_HSTA_RESET;

    /* First create netlink socket to establish userspace communication */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
    priv->nl_sock = netlink_kernel_create(&init_net, MR_VIRTUALCHAN_PROTOCOL, MR_VIRTUALCHAN_MULTICAST_GROUP, mr_virtualchan_recv_from_userspace, NULL, THIS_MODULE);
#else
    struct netlink_kernel_cfg cfg = {
        .input = mr_virtualchan_recv_from_userspace,
        .groups = MR_VIRTUALCHAN_MULTICAST_GROUP,
    };
    priv->nl_sock = netlink_kernel_create(&init_net, MR_VIRTUALCHAN_PROTOCOL, &cfg);
#endif
    if (!priv->nl_sock) {
        VIRTUALCHAN_ERR(priv, "Failed to create netlink socket\n");
        goto free_priv;
    }

    /* Store private data in socket for callback access */
    priv->nl_sock->sk_user_data = priv;

    /* Create daemon thread (thread will be in TASK_UNINTERRUPTIBLE state, not yet running) */
    priv->daemon_thread = kthread_create(mr_virtualchan_daemon_thread, priv, "mr_virtualchan_%u", msg_tag);
    if (IS_ERR(priv->daemon_thread)) {
        VIRTUALCHAN_ERR(priv, "Failed to create daemon thread\n");
        goto release_socket;
    }

    /* Register message callbacks after socket creation */
    ret = mr_msg_cb_register(msg_ctrl, msg_tag, mr_virtualchan_upld_recv_cb, priv, mr_virtualchan_dnld_send_cb, priv);
    if (ret < 0) {
        VIRTUALCHAN_ERR(priv, "Failed to register message callbacks: %d\n", ret);
        goto stop_thread;
    }

    /* Wake up daemon thread after socket and callbacks are ready */
    wake_up_process(priv->daemon_thread);

    VIRTUALCHAN_INFO(priv, "Virtualchan interface initialized successfully\n");
    return priv;

/* Error cleanup path */
stop_thread:
    kthread_stop(priv->daemon_thread);
release_socket:
    netlink_kernel_release(priv->nl_sock);
free_priv:
    kfree(priv);
    return NULL;
}

/**
 * @brief Deinitialize virtual channel interface
 * @param[in] virtualchan_priv Virtual channel private structure to cleanup
 */
void mr_virtualchan_deinit(struct mr_virtualchan_priv *priv)
{
    if (!priv) {
        return;
    }

    VIRTUALCHAN_INFO(priv, "Deinitializing Virtualchan interface\n");

    /* Step 1: Unregister message controller callbacks first to stop new messages */
    mr_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);

    /* Step 2: Stop daemon thread after callback unregistration */
    if (priv->daemon_thread) {
        priv->thread_condition = true;
        wake_up_interruptible(&priv->waitq);
        kthread_stop(priv->daemon_thread);
        priv->daemon_thread = NULL;
    }

    /* Step 3: Clean up skb queues after thread stopped */
    skb_queue_purge(&priv->receive_skb_head);
    skb_queue_purge(&priv->transmit_skb_head);

    /* Step 4: Release netlink socket */
    if (priv->nl_sock) {
        priv->nl_sock->sk_user_data = NULL;
        netlink_kernel_release(priv->nl_sock);
        priv->nl_sock = NULL;
    }

    kfree(priv);

    VIRTUALCHAN_INFO(NULL, "Virtualchan interface deinitialized\n");
}

