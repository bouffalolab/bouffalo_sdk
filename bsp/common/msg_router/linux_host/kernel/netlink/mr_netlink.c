/**
 * @file mr_netlink.c
 * @brief Netlink Interface Implementation for Kernel-Userspace Communication
 * @details Netlink driver implementation with state machine, flow control, and
 *          bidirectional communication, modeled after TTY implementation
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

#include "mr_netlink.h"
#include "mr_debugfs.h"

/* Forward declarations */
static int mr_netlink_daemon_thread(void *data);
static int mr_netlink_upld_recv_cb(struct sk_buff *skb, bool success, void *arg);
static int mr_netlink_dnld_send_cb(struct sk_buff *skb, bool success, void *arg);

/**
 * @brief Update the flow control credit limit
 * @param[in] priv Netlink private structure
 * @param[in] credit_limit_new New credit limit value
 * @retval 0 Always returns 0
 * @details This function updates the credit limit for flow control.
 */
static int mr_netlink_credit_limit_update(struct mr_netlink_priv *priv, uint8_t credit_limit_new)
{
    unsigned long flags;

    spin_lock_irqsave(&priv->credit_lock, flags);

    if ((int8_t)(credit_limit_new - priv->credit_limit_cnt) > 0) {
        priv->credit_limit_cnt = credit_limit_new;
        NETLINK_DBG(priv, "Credit limit updated to %u\n", credit_limit_new);
    } else {
        NETLINK_DBG(priv, "Credit limit not updated, old=%u, new=%u\n", priv->credit_limit_cnt, credit_limit_new);
    }

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    /* Wake daemon thread to attempt sending queued packets */
    wake_up_interruptible(&priv->waitq);

    return 0;
}

/**
 * @brief Consume a flow control credit for packet transmission
 * @param[in] priv Netlink private structure
 * @retval 0 Success, credit consumed
 * @retval -1 No credits available
 * @details This function decrements the consumed credit count.
 */
static int mr_netlink_credit_get(struct mr_netlink_priv *priv)
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
 * @param[in] priv Netlink private structure
 * @return Number of available credits (limit - consumed)
 * @details This function returns the number of available credits for packet transmission.
 */
static int mr_netlink_credit_available(struct mr_netlink_priv *priv)
{
    unsigned long flags;
    int available;

    spin_lock_irqsave(&priv->credit_lock, flags);
    available = (int8_t)(priv->credit_limit_cnt - priv->credit_consumed_cnt);
    spin_unlock_irqrestore(&priv->credit_lock, flags);

    return available;
}

/**
 * @brief Return a flow control credit after failed transmission
 * @param[in] priv Netlink private structure
 * @retval 0 Always returns 0
 * @details This function returns a consumed credit.
 */
static int mr_netlink_credit_put(struct mr_netlink_priv *priv)
{
    unsigned long flags;

    spin_lock_irqsave(&priv->credit_lock, flags);

    priv->credit_consumed_cnt--;

    spin_unlock_irqrestore(&priv->credit_lock, flags);

    return 0;
}

/**
 * @brief Process received data packet and forward to userspace
 * @param[in] priv Netlink private structure
 * @param[in] netlink_msg_pkt Message packet containing data
 * @retval 0 Success
 * @details This function processes incoming data packets and sends them
 *          to userspace via netlink socket using multicast.
 */
static int mr_netlink_send_to_userspace(struct mr_netlink_priv *priv, struct mr_netlink_msg_pkt *netlink_msg_pkt)
{
    uint16_t data_len;
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;
    int ret;

    data_len = MR_NETLINK_GET_DATA_LEN(netlink_msg_pkt->msg_pkt.len);
    if (data_len == 0) {
        NETLINK_ERR(priv, "Received empty data packet\n");
        return -EINVAL;
    }

    /* Allocate netlink message */
    nl_skb = nlmsg_new(data_len, GFP_KERNEL);
    if (!nl_skb) {
        NETLINK_ERR(priv, "Failed to allocate netlink message\n");
        return -ENOMEM;
    }

    /* Fill netlink message header */
    nlh = nlmsg_put(nl_skb, 0, 0, NLMSG_DONE, data_len, 0);
    if (!nlh) {
        NETLINK_ERR(priv, "Failed to put netlink header\n");
        kfree_skb(nl_skb);
        return -EMSGSIZE;
    }

    /* Copy data to netlink message */
    memcpy(nlmsg_data(nlh), netlink_msg_pkt->data, data_len);

    /* Multicast to all subscribed userspace clients */
    ret = nlmsg_multicast(priv->nl_sock, nl_skb, 0, MR_NETLINK_MULTICAST_GROUP, GFP_KERNEL);
    if (ret < 0) {
        if (ret == -ESRCH) {
            /* No listeners, this is expected in multicast mode */
            NETLINK_DBG(priv, "No multicast listeners for data packet\n");
        } else {
            NETLINK_ERR(priv, "Failed to multicast netlink message: %d\n", ret);
        }
        /* nlmsg_multicast frees skb on failure */
        return ret;
    }

    NETLINK_DBG(priv, "Multicasted data to userspace: len=%u\n", data_len);
    return 0;
}

/**
 * @brief Send command packet to device
 * @param[in] priv Netlink private structure
 * @param[in] flag Command flag
 * @retval 0 Success
 * @retval <0 Error code
 */
static int mr_netlink_send_cmd_packet(struct mr_netlink_priv *priv, uint8_t flag)
{
    struct sk_buff *skb;
    struct mr_netlink_msg_pkt *netlink_msg_pkt;
    int ret;

    skb = dev_alloc_skb(sizeof(struct mr_netlink_msg_pkt));
    if (!skb) {
        NETLINK_ERR(priv, "Failed to allocate skb for command packet\n");
        return -ENOMEM;
    }

    netlink_msg_pkt = (struct mr_netlink_msg_pkt *)skb_put(skb, sizeof(struct mr_netlink_msg_pkt));

    netlink_msg_pkt->msg_pkt.tag = priv->msg_tag;
    netlink_msg_pkt->msg_pkt.sub_tag = 0;
    netlink_msg_pkt->msg_pkt.len = sizeof(struct mr_netlink_msg_pkt) - sizeof(struct mr_msg_pkt);

    netlink_msg_pkt->reserved[0] = 0;
    netlink_msg_pkt->flag = flag;
    netlink_msg_pkt->credit_update_flag = 0;
    netlink_msg_pkt->credit_limit_cnt = 0;

    ret = mr_msg_ctrl_send(priv->msg_ctrl, skb);
    if (ret < 0) {
        NETLINK_ERR(priv, "Failed to send command packet: %d\n", ret);
        dev_kfree_skb_any(skb);
        return ret;
    }

    NETLINK_DBG(priv, "Sent command packet: flag=0x%02x\n", flag);
    return 0;
}

/**
 * @brief Socket buffer receive callback for message controller
 * @param[in] skb Socket buffer containing received data
 * @param[in] success Reception success flag (always true for received messages)
 * @param[in] arg Netlink private structure argument
 * @retval Always returns 0
 * @details This callback handles received Netlink packets from the device.
 *          For RUN state data packets, it processes them directly for speed.
 *          For control packets, it queues them for daemon thread processing.
 */
static int mr_netlink_upld_recv_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct mr_netlink_priv *priv = (struct mr_netlink_priv *)arg;
    struct mr_netlink_msg_pkt *netlink_msg_pkt;

    if (skb->len < sizeof(struct mr_netlink_msg_pkt)) {
        NETLINK_ERR(priv, "Received packet too small: %u\n", skb->len);
        dev_kfree_skb_any(skb);
        return 0;
    }

    netlink_msg_pkt = (struct mr_netlink_msg_pkt *)skb->data;

#if 0
    /* Fast path for data packets in RUN state to avoid waking up the kthread */
    if (priv->netlink_status == MR_NETLINK_HSTA_DEVICE_RUN &&
        (netlink_msg_pkt->flag == MR_NETLINK_FLAG_UPLD_DATA ||
         netlink_msg_pkt->flag == MR_NETLINK_FLAG_CREDIT_UPDATE)) {
        /* Update flow control, must be protected with lock */
        if (netlink_msg_pkt->credit_update_flag) {
            mr_netlink_credit_limit_update(priv, netlink_msg_pkt->credit_limit_cnt);
        }

        if (netlink_msg_pkt->flag == MR_NETLINK_FLAG_UPLD_DATA) {
            mr_netlink_send_to_userspace(priv, netlink_msg_pkt);
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
 * @param[in] arg Netlink private structure argument
 * @retval Always returns 0
 * @details This callback handles send completion events.
 */
static int mr_netlink_dnld_send_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct mr_netlink_priv *priv = (struct mr_netlink_priv *)arg;
    struct mr_netlink_msg_pkt *netlink_msg_pkt;
    uint32_t data_len;

    /* Extract data length from message packet */
    netlink_msg_pkt = (struct mr_netlink_msg_pkt *)skb->data;
    data_len = MR_NETLINK_GET_DATA_LEN(netlink_msg_pkt->msg_pkt.len);

    if (success) {
        NETLINK_DBG(priv, "Send completed successfully: len=%u\n", data_len);
    } else {
        NETLINK_ERR(priv, "Send failed: len=%u\n", data_len);

        if (netlink_msg_pkt->flag == MR_NETLINK_FLAG_DNLD_DATA) {
            mr_netlink_credit_put(priv);
        }
    }

    dev_kfree_skb_any(skb);
    return 0;
}

/**
 * @brief Netlink receive handler for messages from userspace
 * @param[in] skb Socket buffer from userspace
 * @details This function handles messages received from userspace via netlink
 *          and forwards them to the device through msg_ctrl.
 */
static void mr_netlink_recv_from_userspace(struct sk_buff *skb)
{
    struct mr_netlink_priv *priv;
    struct nlmsghdr *nlh;
    struct sk_buff *dev_skb;
    struct mr_netlink_msg_pkt *netlink_msg_pkt;
    uint16_t data_len;

    /* Get netlink socket private data */
    priv = (struct mr_netlink_priv *)skb->sk->sk_user_data;
    if (!priv) {
        NETLINK_ERR(NULL, "No private data in netlink socket\n");
        return;
    }

    nlh = nlmsg_hdr(skb);

    /* Log client connection for debugging (in multicast mode, we don't track individual clients) */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
    NETLINK_INFO(priv, "Received message from portid=%u\n", NETLINK_CB(skb).pid);
#else
    NETLINK_INFO(priv, "Received message from portid=%u\n", NETLINK_CB(skb).portid);
#endif

    data_len = nlmsg_len(nlh);
    if (data_len == 0) {
        NETLINK_ERR(priv, "Received empty message from userspace\n");
        return;
    }

    if (data_len > MR_NETLINK_MAX_DATA_LEN) {
        NETLINK_ERR(priv, "Data too large: %u > %u\n", data_len, MR_NETLINK_MAX_DATA_LEN);
        return;
    }

    /* Check device state */
    if (priv->netlink_status != MR_NETLINK_HSTA_DEVICE_RUN) {
        NETLINK_INFO(priv, "Device not in RUN state, dropping packet\n");
        return;
    }

    /* Allocate device SKB */
    dev_skb = dev_alloc_skb(sizeof(struct mr_netlink_msg_pkt) + data_len);
    if (!dev_skb) {
        NETLINK_ERR(priv, "Failed to allocate device SKB\n");
        return;
    }

    /* Fill message packet */
    netlink_msg_pkt = (struct mr_netlink_msg_pkt *)skb_put(dev_skb, sizeof(struct mr_netlink_msg_pkt) + data_len);
    netlink_msg_pkt->msg_pkt.tag = priv->msg_tag;
    netlink_msg_pkt->msg_pkt.sub_tag = 0;
    netlink_msg_pkt->msg_pkt.len = sizeof(struct mr_netlink_msg_pkt) - sizeof(struct mr_msg_pkt) + data_len;

    netlink_msg_pkt->reserved[0] = 0;
    netlink_msg_pkt->flag = MR_NETLINK_FLAG_DNLD_DATA;
    netlink_msg_pkt->credit_update_flag = 0;
    netlink_msg_pkt->credit_limit_cnt = 0;

    /* Copy data from userspace message */
    memcpy(netlink_msg_pkt->data, nlmsg_data(nlh), data_len);

    /* Enqueue to transmit queue for ordered sending by daemon thread */
    skb_queue_tail(&priv->transmit_skb_head, dev_skb);
    wake_up_interruptible(&priv->waitq);

    NETLINK_DBG(priv, "Queued data for device (transmit queue): len=%u\n", data_len);
}

/**
 * @brief Process received packets and manage state machine
 * @param[in] priv Netlink private structure
 * @retval 0 Success
 * @retval <0 Error code (caller should exit thread on fatal error)
 * @details This function processes one packet from the receive queue
 *          and handles state machine transitions.
 */
static int mr_netlink_received_process(struct mr_netlink_priv *priv)
{
    unsigned long flags;
    struct sk_buff *skb;
    struct mr_netlink_msg_pkt *netlink_msg_pkt;

    if (priv->netlink_status == MR_NETLINK_HSTA_RESET || priv->netlink_status == MR_NETLINK_HSTA_ERROR) {
        /* In RESET or ERROR state, skip processing received packets */
        return 1;
    }

    /* Try to get skb for processing (received from device) */
    skb = skb_dequeue(&priv->receive_skb_head);
    if (skb == NULL) {
        return 2;
    }
    netlink_msg_pkt = (struct mr_netlink_msg_pkt *)skb->data;

    /* Anytime receiving DEVICE_RESET or DEVICE_STOP, need to return to initial state */
    if (netlink_msg_pkt->flag == MR_NETLINK_FLAG_DEVICE_RESET ||
        netlink_msg_pkt->flag == MR_NETLINK_FLAG_DEVICE_STOP) {
        /* Change state to reset */
        priv->netlink_status = MR_NETLINK_HSTA_RESET;
        dev_kfree_skb_any(skb);

        NETLINK_INFO(priv, "Received DEVICE_RESET/STOP in state %d, returning to HSTA_RESET\n", priv->netlink_status);
        return 3;
    }

    /* State machine */
    switch (priv->netlink_status) {
        case MR_NETLINK_HSTA_HOST_READY:
            if (netlink_msg_pkt->flag != MR_NETLINK_FLAG_DEVICE_START) {
                break;
            }

            /* Process initial credit limit */
            if (netlink_msg_pkt->credit_update_flag == false || netlink_msg_pkt->credit_limit_cnt == 0) {
                NETLINK_INFO(priv, "Device ready with no credit limit\n");
            } else {
                spin_lock_irqsave(&priv->credit_lock, flags);
                priv->credit_limit_cnt = netlink_msg_pkt->credit_limit_cnt;
                priv->credit_consumed_cnt = 0;
                spin_unlock_irqrestore(&priv->credit_lock, flags);
                priv->netlink_status = MR_NETLINK_HSTA_DEVICE_RUN;

                NETLINK_INFO(priv, "Device ready! Credit Limit=%u\n", netlink_msg_pkt->credit_limit_cnt);
            }
            break;

        case MR_NETLINK_HSTA_DEVICE_RUN:
            if (netlink_msg_pkt->flag == MR_NETLINK_FLAG_UPLD_DATA) {
                /* Process credit update */
                if (netlink_msg_pkt->credit_update_flag) {
                    mr_netlink_credit_limit_update(priv, netlink_msg_pkt->credit_limit_cnt);
                }

                /* Process data */
                mr_netlink_send_to_userspace(priv, netlink_msg_pkt);

            } else if (netlink_msg_pkt->flag == MR_NETLINK_FLAG_CREDIT_UPDATE) {
                /* Credit update */
                if (netlink_msg_pkt->credit_update_flag) {
                    mr_netlink_credit_limit_update(priv, netlink_msg_pkt->credit_limit_cnt);
                }
            }
            break;

        case MR_NETLINK_HSTA_RESET:
            break;

        case MR_NETLINK_HSTA_ERROR:
            break;

        default:
            NETLINK_ERR(priv, "Unknown status: %d\n", priv->netlink_status);
            break;
    }

    /* Release skb_recv */
    dev_kfree_skb_any(skb);

    return 0;
}

/**
 * @brief Process transmit queue for sending packets to device
 * @param[in] priv Netlink private structure
 * @retval 0 Success
 * @retval <0 Error code (caller should exit thread on fatal error)
 * @details This function processes pending packets in the transmit queue.
 *          Only sends packets when in DEVICE_RUN state and credits are available.
 */
static int mr_netlink_transmit_process(struct mr_netlink_priv *priv)
{
    int ret;
    struct sk_buff *tx_skb;

    /* Only send when in DEVICE_RUN state */
    if (priv->netlink_status != MR_NETLINK_HSTA_DEVICE_RUN) {
        if (!skb_queue_empty(&priv->transmit_skb_head)) {
            NETLINK_ERR(priv, "Not in DEVICE_RUN state, clearing transmit queue\n");
            skb_queue_purge(&priv->transmit_skb_head);
        }
        return 1;
    }

    if (mr_netlink_credit_available(priv) <= 0) {
        /* No credits available, wait */
        return 2;
    }

    /* Dequeue and send */
    tx_skb = skb_dequeue(&priv->transmit_skb_head);
    if (!tx_skb) {
        return 3;
    }

    /* Consume credit (daemon ensures we have credit before calling) */
    if (mr_netlink_credit_get(priv) < 0) {
        /* Should not happen, but requeue and wait */
        skb_queue_head(&priv->transmit_skb_head, tx_skb);
        NETLINK_ERR(priv, "Unexpected: no credits available, will retry\n");
        return 4;
    }

    ret = mr_msg_ctrl_send(priv->msg_ctrl, tx_skb);
    if (ret < 0) {
        /* Return consumed credit and requeue at head */
        mr_netlink_credit_put(priv);
        skb_queue_head(&priv->transmit_skb_head, tx_skb);

        priv->netlink_status = MR_NETLINK_HSTA_ERROR;

        NETLINK_ERR(priv, "Failed to send queued packet: %d\n", ret);
        return ret;
    }

    return 0;
}

/**
 * @brief Daemon thread for handling control packets
 * @param[in] data Netlink private structure
 * @retval 0 Thread exit
 * @details This thread processes control packets and manages state transitions.
 */
static int mr_netlink_daemon_thread(void *data)
{
    int ret;
    struct mr_netlink_priv *priv = (struct mr_netlink_priv *)data;

    NETLINK_INFO(priv, "Daemon thread started\n");

thread_restart:
    /* Send HOST_RESET command to initialize state machine */
    ret = mr_netlink_send_cmd_packet(priv, MR_NETLINK_FLAG_HOST_RESET);
    if (ret < 0) {
        priv->netlink_status = MR_NETLINK_HSTA_ERROR;
        NETLINK_ERR(priv, "Failed to send HOST_RESET command: %d\n", ret);
        goto exit;
    }
    priv->netlink_status = MR_NETLINK_HSTA_RESET;

thread_reset:
    /* Clear queues */
    skb_queue_purge(&priv->receive_skb_head);
    skb_queue_purge(&priv->transmit_skb_head);
    /* Send HOST_READY packet to indicate HOST is ready */
    ret = mr_netlink_send_cmd_packet(priv, MR_NETLINK_FLAG_HOST_READY);
    if (ret < 0) {
        priv->netlink_status = MR_NETLINK_HSTA_ERROR;
        NETLINK_ERR(priv, "Failed to send HOST_READY command: %d\n", ret);
        goto exit;
    }
    NETLINK_INFO(priv, "Host ready, waiting for device start\n");
    priv->netlink_status = MR_NETLINK_HSTA_HOST_READY;

    while (1) {
        /* Suspend and wait for wake-up for various reasons */
        wait_event_interruptible(priv->waitq, !skb_queue_empty(&priv->receive_skb_head) ||
                                                  (!skb_queue_empty(&priv->transmit_skb_head) &&
                                                   mr_netlink_credit_available(priv) > 0) ||
                                                  priv->thread_condition ||
                                                  kthread_should_stop());

        NETLINK_DBG(priv, "Daemon thread running, current status: %d\n", priv->netlink_status);

        if (kthread_should_stop()) {
            goto exit;
        } else if (priv->thread_condition) {
            usleep_range(500, 1000);
        }

        /* process received packet and handle state machine */
        while (mr_netlink_received_process(priv) == 0) {}

        /* process transmit queue (send pending packets) */
        while (mr_netlink_transmit_process(priv) == 0) {}

        if (priv->netlink_status == MR_NETLINK_HSTA_RESET) {
            usleep_range(500, 1000);
            goto thread_reset;
        }

        /* If in ERROR state, restart the thread */
        if (priv->netlink_status == MR_NETLINK_HSTA_ERROR) {
            usleep_range(10000, 20000);
            goto thread_restart;
        }
    }

exit:
    if (priv->netlink_status != MR_NETLINK_HSTA_ERROR) {
        ret = mr_netlink_send_cmd_packet(priv, MR_NETLINK_FLAG_HOST_STOP);
        if (ret < 0) {
            NETLINK_ERR(priv, "Failed to send stop packet: %d\n", ret);
        }
    }

    /* Wait for kthread_stop() to be called to avoid premature thread exit */
    while (!kthread_should_stop()) {
        wait_event_interruptible(priv->waitq, kthread_should_stop() || priv->thread_condition);
        usleep_range(500, 1000);
    }

    NETLINK_INFO(priv, "Daemon thread exiting, status: %d\n", priv->netlink_status);
    return 0;
}

/**
 * @brief Initialize Netlink interface
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval Pointer to Netlink private structure on success
 * @retval NULL on failure
 */
struct mr_netlink_priv *mr_netlink_init(struct mr_msg_ctrl *msg_ctrl, uint8_t msg_tag)
{
    int ret;
    struct mr_netlink_priv *priv;

    NETLINK_INFO(NULL, "Initializing Netlink interface with msg_tag=%u\n", msg_tag);

    /* Allocate private structure */
    priv = kzalloc(sizeof(struct mr_netlink_priv), GFP_KERNEL);
    if (!priv) {
        NETLINK_ERR(NULL, "Failed to allocate private structure\n");
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
    priv->netlink_status = MR_NETLINK_HSTA_RESET;

    /* Create netlink socket first to establish userspace communication */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
    priv->nl_sock = netlink_kernel_create(&init_net, MR_NETLINK_PROTOCOL, MR_NETLINK_MULTICAST_GROUP, mr_netlink_recv_from_userspace, NULL, THIS_MODULE);
#else
    struct netlink_kernel_cfg cfg = {
        .input = mr_netlink_recv_from_userspace,
        .groups = MR_NETLINK_MULTICAST_GROUP,
    };
    priv->nl_sock = netlink_kernel_create(&init_net, MR_NETLINK_PROTOCOL, &cfg);
#endif
    if (!priv->nl_sock) {
        NETLINK_ERR(priv, "Failed to create netlink socket\n");
        goto free_priv;
    }

    /* Store private data in socket for callback access */
    priv->nl_sock->sk_user_data = priv;

    /* Create daemon thread (thread will be in TASK_UNINTERRUPTIBLE state, not running yet) */
    priv->daemon_thread = kthread_create(mr_netlink_daemon_thread, priv, "mr_netlink_%u", msg_tag);
    if (IS_ERR(priv->daemon_thread)) {
        NETLINK_ERR(priv, "Failed to create daemon thread\n");
        goto release_socket;
    }

    /* Register message callbacks after socket is created */
    ret = mr_msg_cb_register(msg_ctrl, msg_tag, mr_netlink_upld_recv_cb, priv, mr_netlink_dnld_send_cb, priv);
    if (ret < 0) {
        NETLINK_ERR(priv, "Failed to register message callbacks: %d\n", ret);
        goto stop_thread;
    }

    /* Wake up daemon thread after socket and callbacks are ready */
    wake_up_process(priv->daemon_thread);

    NETLINK_INFO(priv, "Netlink interface initialized successfully\n");
    return priv;

/* Error cleanup paths */
stop_thread:
    kthread_stop(priv->daemon_thread);
release_socket:
    netlink_kernel_release(priv->nl_sock);
free_priv:
    kfree(priv);
    return NULL;
}

/**
 * @brief Deinitialize Netlink interface
 * @param[in] netlink_priv Netlink private structure to cleanup
 */
void mr_netlink_deinit(struct mr_netlink_priv *priv)
{
    if (!priv) {
        return;
    }

    NETLINK_INFO(priv, "Deinitializing Netlink interface\n");

    /* Step 1: First unregister message controller callbacks to stop new messages */
    mr_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);

    /* Step 2: Stop daemon thread after callbacks are unregistered */
    if (priv->daemon_thread) {
        priv->thread_condition = true;
        wake_up_interruptible(&priv->waitq);
        kthread_stop(priv->daemon_thread);
        priv->daemon_thread = NULL;
    }

    /* Step 3: Clean up skb queues after thread is stopped */
    skb_queue_purge(&priv->receive_skb_head);
    skb_queue_purge(&priv->transmit_skb_head);

    /* Step 4: Release netlink socket */
    if (priv->nl_sock) {
        priv->nl_sock->sk_user_data = NULL;
        netlink_kernel_release(priv->nl_sock);
        priv->nl_sock = NULL;
    }

    kfree(priv);

    NETLINK_INFO(NULL, "Netlink interface deinitialized\n");
}
