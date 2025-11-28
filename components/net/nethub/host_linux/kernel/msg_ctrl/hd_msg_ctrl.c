/**
 * @file hd_msg_ctrl.c
 * @brief Host Driver Message Controller for SDIO Communication
 * @author Host Driver Development Team
 * @date 2024
 * @details This module implements a unified message routing system for data flow management
 *          across multiple transport interfaces (SDIO/USB). It uses TLV (Type-Length-Value)
 *          format for message encapsulation and provides work queue-based asynchronous
 *          processing with callback mechanisms for upper layer protocol handlers.
 */

#include <linux/skbuff.h>

#include "hd_sdio_manage.h"
#include "hd_msg_ctrl.h"
#include "hd_debugfs.h"

/**
 * @brief Message receive callback function with race condition protection
 * @param[in] msg_ctrl Pointer to message controller structure
 * @param[in] skb Socket buffer containing received message
 * @retval 0 Success
 * @retval -EINVAL Invalid parameter or malformed message
 * @details This function processes received messages by parsing TLV format packets
 *          and routing them to appropriate handlers based on message tags.
 *          Implements atomic callback access to prevent race conditions during
 *          callback registration/unregistration.
 */
static int hd_msg_upld_recv_handler(struct hd_msg_ctrl *msg_ctrl, struct sk_buff *skb)
{
    int ret = 0;
    hd_msg_cb_t upld_recv_cb = NULL;
    void *upld_recv_arg = NULL;
    struct hd_msg_packt *msg_packt;
    int skb_len;
    uint8_t msg_tag;

    if (skb->len < sizeof(struct hd_msg_packt)) {
        MSG_CTRL_ERR(msg_ctrl, "msg skb len is invalid: %d", skb->len);
        ret = -EINVAL;
        goto err_exit;
    }

    msg_packt = (struct hd_msg_packt *)skb->data;
    msg_tag = msg_packt->tag;

    if (msg_tag >= HD_MSG_TAG_MAX) {
        MSG_CTRL_ERR(msg_ctrl, "msg skb tag is invalid: %d", msg_tag);
        ret = -EINVAL;
        goto err_exit;
    }

    if (msg_packt->len + sizeof(struct hd_msg_packt) > skb->len) {
        MSG_CTRL_ERR(msg_ctrl, "msg skb len is less than hdr len: %d < %lu", skb->len, msg_packt->len + sizeof(struct hd_msg_packt));
        ret = -EINVAL;
        goto err_exit;
    }

    /* Trim SKB to actual message length to remove SDIO block alignment padding */
    skb_trim(skb, msg_packt->len + sizeof(struct hd_msg_packt));
    skb_len = skb->len;

    mutex_lock(&msg_ctrl->upld_cb_mutex);

    upld_recv_cb = msg_ctrl->hd_msg_upld_recv_cb[msg_tag];
    upld_recv_arg = msg_ctrl->hd_msg_upld_recv_arg[msg_tag];

    if (!upld_recv_cb) {
        mutex_unlock(&msg_ctrl->upld_cb_mutex);
        MSG_CTRL_DBG(msg_ctrl, "msg recv cb is NULL for tag: %d", msg_tag);
        ret = -EINVAL;
        goto err_exit;
    }

    /* Call receive callback while holding mutex to prevent unregistration during execution */
    ret = upld_recv_cb(skb, true, upld_recv_arg);
    mutex_unlock(&msg_ctrl->upld_cb_mutex);

    /* Update statistics after callback completes, using saved values */
    spin_lock(&msg_ctrl->stats_lock);
    if (ret >= 0 && msg_tag < HD_MSG_TAG_MAX) {
        /* Only update success statistics if callback succeeded */
        msg_ctrl->stats.rx_packets[msg_tag]++;
        msg_ctrl->stats.rx_bytes[msg_tag] += skb_len;
    } else if (ret < 0) {
        /* Update error statistics if callback failed */
        msg_ctrl->stats.rx_errors++;
    }
    spin_unlock(&msg_ctrl->stats_lock);

    if (ret < 0) {
        MSG_CTRL_ERR(msg_ctrl, "upld_recv_cb failed for tag %d: %d", msg_tag, ret);
        ret = -EINVAL;
        goto err_exit;
    }

    return 0;

err_exit:
    /* Update error statistics only for validation errors (not callback errors) */
    spin_lock(&msg_ctrl->stats_lock);
    msg_ctrl->stats.rx_errors++;
    spin_unlock(&msg_ctrl->stats_lock);

    if (skb) {
        dev_kfree_skb_any(skb);
    }
    return ret;
}

/**
 * @brief Upload receive callback for received messages
 * @param[in] skb Socket buffer containing received data
 * @param[in] success Transfer success flag
 * @param[in] arg Message controller argument
 * @retval 0 Success (skb ownership transferred to callback)
 * @retval <0 Error code (skb ownership remains with caller, caller should free)
 * @details This callback is invoked when upload operations complete.
 *          It queues received messages for processing by the work queue.
 *          If callback returns negative, caller (transport manage layer) will free the skb.
 *          If callback returns 0, callback has taken ownership of skb.
 */
static int hd_msg_upld_recv_cplt_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_msg_ctrl *msg_ctrl = arg;

    MSG_CTRL_DBG(msg_ctrl, "hd_msg_upld_recv_cplt_cb, success: %s", success ? "true" : "false");

    if (!msg_ctrl) {
        /* Return negative to let caller free the skb */
        return -EINVAL;
    }

    if (!success) {
        MSG_CTRL_ERR(msg_ctrl, "upload transfer failed");
        /* Return negative to let caller free the skb */
        return -EIO;
    }

    /* Check queue depth to prevent memory exhaustion */
    if (skb_queue_len(&msg_ctrl->upld_skb_head) >= HD_MSG_CTRL_UPLD_QUEUE_DEPTH_MAX) {
        MSG_CTRL_ERR(msg_ctrl, "upld queue full, dropping SKB");

        spin_lock(&msg_ctrl->stats_lock);
        msg_ctrl->stats.rx_dropped++;
        spin_unlock(&msg_ctrl->stats_lock);

        return -ENOMEM;
    }

    /* Queue skb to upld_skb_head for workqueue processing */
    skb_queue_tail(&msg_ctrl->upld_skb_head, skb);
    queue_work(msg_ctrl->recv_workqueue, &msg_ctrl->recv_work);
    return 0;
}

/**
 * @brief Download send completion handler for sent messages
 * @param[in] msg_ctrl Pointer to message controller structure
 * @param[in] skb Socket buffer containing sent message
 * @retval 0 Success (callback took ownership of skb)
 * @retval -ENOENT No callback registered for this message tag
 * @retval <0 Error code from callback function
 * @details This function processes completed download send operations by calling
 *          the appropriate upper layer callback based on the message tag.
 *          Uses mutex protection to safely access callback functions and prevent
 *          race conditions during callback registration/unregistration.
 */
static int hd_msg_dnld_send_handler(struct hd_msg_ctrl *msg_ctrl, struct sk_buff *skb)
{
    int ret = 0;
    hd_msg_cb_t dnld_send_cb = NULL;
    void *dnld_send_arg = NULL;
    struct hd_msg_packt *msg_packt;
    uint8_t msg_tag;

    msg_packt = (struct hd_msg_packt *)skb->data;
    msg_tag = msg_packt->tag;

    mutex_lock(&msg_ctrl->dnld_cb_mutex);
    dnld_send_cb = msg_ctrl->hd_msg_dnld_send_cb[msg_tag];
    dnld_send_arg = msg_ctrl->hd_msg_dnld_send_arg[msg_tag];

    if (dnld_send_cb) {
        ret = dnld_send_cb(skb, true, dnld_send_arg);
        mutex_unlock(&msg_ctrl->dnld_cb_mutex);
        if (ret < 0) {
            MSG_CTRL_ERR(msg_ctrl, "dnld send completion callback failed for tag %d: %d", msg_tag, ret);
            return ret;
        }
        return 0;
    } else {
        mutex_unlock(&msg_ctrl->dnld_cb_mutex);
        MSG_CTRL_DBG(msg_ctrl, "no dnld send completion callback for tag: %d", msg_tag);
        return -ENOENT;
    }
}

/**
 * @brief Download completion callback for sent messages
 * @param[in] skb Socket buffer that was sent
 * @param[in] success Transfer success flag
 * @param[in] arg Message controller argument
 * @retval 0 Success (skb ownership transferred to callback)
 * @retval <0 Error code (skb ownership remains with caller, caller should free)
 * @details This callback is invoked when download operations complete.
 *          It notifies upper layer applications about send completion.
 *          If callback returns negative, caller (transport manage layer) will free the skb.
 *          If callback returns 0, callback has taken ownership of skb.
 */
static int hd_msg_dnld_send_cplt_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_msg_ctrl *msg_ctrl = arg;
    struct hd_msg_packt *msg_packt;
    uint8_t msg_tag;
    hd_msg_cb_t dnld_send_cb = NULL;
    void *dnld_send_arg = NULL;
    int ret = 0;

    MSG_CTRL_DBG(msg_ctrl, "hd_msg_dnld_send_cplt_cb, success: %s", success ? "true" : "false");

    msg_packt = (struct hd_msg_packt *)skb->data;
    msg_tag = msg_packt->tag;

    if (msg_tag >= HD_MSG_TAG_MAX) {
        MSG_CTRL_ERR(msg_ctrl, "dnld completion invalid tag: %d", msg_tag);
        /* Return negative to let caller free the skb */
        return -EINVAL;
    }

    spin_lock(&msg_ctrl->stats_lock);
    if (success) {
        msg_ctrl->stats.tx_packets[msg_tag]++;
        msg_ctrl->stats.tx_bytes[msg_tag] += skb->len;
    } else {
        msg_ctrl->stats.tx_errors++;
    }
    spin_unlock(&msg_ctrl->stats_lock);

    if (success) {
        skb_queue_tail(&msg_ctrl->dnld_skb_head, skb);
        queue_work(msg_ctrl->recv_workqueue, &msg_ctrl->recv_work);
        return 0;
    }

    mutex_lock(&msg_ctrl->dnld_cb_mutex);
    dnld_send_cb = msg_ctrl->hd_msg_dnld_send_cb[msg_tag];
    dnld_send_arg = msg_ctrl->hd_msg_dnld_send_arg[msg_tag];

    /* Call upper layer send completion callback if registered */
    if (dnld_send_cb) {
        ret = dnld_send_cb(skb, success, dnld_send_arg);
        mutex_unlock(&msg_ctrl->dnld_cb_mutex);
        if (ret < 0) {
            MSG_CTRL_ERR(msg_ctrl, "dnld send completion callback failed for tag %d: %d", msg_tag, ret);
            return ret;
        }
        return 0;
    } else {
        mutex_unlock(&msg_ctrl->dnld_cb_mutex);
        MSG_CTRL_DBG(msg_ctrl, "no dnld send completion callback for tag: %d", msg_tag);
        return -ENOENT;
    }
}

/**
 * @brief Work queue function for asynchronous message processing
 * @param[in] work Work structure containing message controller context
 * @details This function processes received messages from the work queue
 *          in a deferred context, allowing for non-blocking message
 *          reception and proper callback execution with scheduling protection.
 */
static void hd_msg_workqueue_func(struct work_struct *work)
{
    struct hd_msg_ctrl *msg_ctrl = container_of(work, struct hd_msg_ctrl, recv_work);
    struct sk_buff *skb = NULL;
    bool upld_proc_flag = false;
    bool dnld_proc_flag = false;
    int processed = 0;

    MSG_CTRL_DBG(msg_ctrl, "hd_msg_workqueue_func");

    while (1) {
        skb = skb_dequeue(&msg_ctrl->upld_skb_head);
        if (skb) {
            upld_proc_flag = false;
            hd_msg_upld_recv_handler(msg_ctrl, skb);
        } else {
            upld_proc_flag = true;
        }

        skb = skb_dequeue(&msg_ctrl->dnld_skb_head);
        if (skb) {
            dnld_proc_flag = false;
            hd_msg_dnld_send_handler(msg_ctrl, skb);
        } else {
            dnld_proc_flag = true;
        }

        if (upld_proc_flag && dnld_proc_flag) {
            break;
        }

        processed++;
        if (processed >= 256) {
            MSG_CTRL_DBG(msg_ctrl, "Processed %d messages in this workqueue run", processed);
            queue_work(msg_ctrl->recv_workqueue, &msg_ctrl->recv_work);
            break;
        }
    }
}

/**
 * @brief Send message through SDIO interface
 * @param[in] sdio_manage SDIO management structure
 * @param[in] skb Socket buffer containing message data
 * @retval 0 Success
 * @retval <0 Error code from SDIO operations
 * @details This function transmits messages through the SDIO interface
 *          using the SDIO management layer for reliable delivery.
 */
static int hd_msg_sdio_send(struct hd_sdio_manage *sdio_manage, struct sk_buff *skb)
{
    int ret;

    ret = hd_sdio_manage_dnld_send(sdio_manage, skb);
    return ret;
}

/**
 * @brief Send message through USB interface (placeholder)
 * @param[in] usb_manage USB management structure
 * @param[in] skb Socket buffer containing message data
 * @retval -1 Not implemented
 * @details This function is a placeholder for USB message transmission.
 *          Currently not implemented and returns error.
 */
static int hd_msg_usb_send(void *usb_manage, struct sk_buff *skb)
{
    return -1;
}

/**
 * @brief Delete and cleanup message controller resources
 * @param[in] msg_ctrl Message controller structure
 * @retval 0 Success
 * @details This function performs cleanup of message controller resources
 *          including work queues, memory, and associated structures.
 */
static int hd_msg_ctrl_del(struct hd_msg_ctrl *msg_ctrl)
{
    int i;

    if (!msg_ctrl) {
        return -EINVAL;
    }

    /* rm workqueue */
    if (msg_ctrl->recv_workqueue) {
        flush_workqueue(msg_ctrl->recv_workqueue);
        destroy_workqueue(msg_ctrl->recv_workqueue);
        msg_ctrl->recv_workqueue = NULL;
    }

    /* Clean up pending SKBs in upld queue */
    skb_queue_purge(&msg_ctrl->upld_skb_head);
    skb_queue_purge(&msg_ctrl->dnld_skb_head);

    /* Destroy synchronization mutexes */
    mutex_destroy(&msg_ctrl->upld_cb_mutex);
    mutex_destroy(&msg_ctrl->dnld_cb_mutex);

    for (i = 0; i < HD_MSG_TAG_MAX; i++) {
        msg_ctrl->hd_msg_upld_recv_cb[i] = NULL;
        msg_ctrl->hd_msg_upld_recv_arg[i] = NULL;
        msg_ctrl->hd_msg_dnld_send_cb[i] = NULL;
        msg_ctrl->hd_msg_dnld_send_arg[i] = NULL;
    }

    kfree(msg_ctrl);
    return 0;
}

/**
 * @brief Create and initialize message controller
 * @retval Message controller pointer on success
 * @retval NULL on failure
 * @details This function allocates and initializes a new message controller
 *          including work queues, callback arrays, and synchronization
 *          primitives for message routing and processing.
 */
static struct hd_msg_ctrl *hd_msg_ctrl_create(void)
{
    struct hd_msg_ctrl *msg_ctrl;

    msg_ctrl = kzalloc(sizeof(struct hd_msg_ctrl), GFP_KERNEL);
    if (!msg_ctrl) {
        return NULL;
    }

    spin_lock_init(&msg_ctrl->stats_lock);
    mutex_init(&msg_ctrl->upld_cb_mutex);
    mutex_init(&msg_ctrl->dnld_cb_mutex);

    /* msg work queue */
    msg_ctrl->recv_workqueue = alloc_workqueue("HD_MSG_RECV_WQ", WQ_HIGHPRI | WQ_UNBOUND | WQ_MEM_RECLAIM, 1);
    if (!msg_ctrl->recv_workqueue) {
        goto err_exit;
    }
    INIT_WORK(&msg_ctrl->recv_work, hd_msg_workqueue_func);

    skb_queue_head_init(&msg_ctrl->upld_skb_head);
    skb_queue_head_init(&msg_ctrl->dnld_skb_head);

    return msg_ctrl;

err_exit:
    if (msg_ctrl->recv_workqueue) {
        flush_workqueue(msg_ctrl->recv_workqueue);
        destroy_workqueue(msg_ctrl->recv_workqueue);
        msg_ctrl->recv_workqueue = NULL;
    }

    if (msg_ctrl) {
        kfree(msg_ctrl);
    }

    return NULL;
}

/******************************************************************************** */

/**
 * @brief Deinitialize SDIO message controller
 * @param msg_ctrl Pointer to message controller structure
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_sdio_msg_ctrl_deinit(struct hd_msg_ctrl *msg_ctrl)
{
    if (!msg_ctrl) {
        return -EINVAL;
    }

    if (msg_ctrl->sdio_manage) {
        /* unregister both upload and download callbacks */
        hd_sdio_manage_cplt_cb_register(msg_ctrl->sdio_manage, NULL, NULL, NULL, NULL);

        msg_ctrl->sdio_manage->msg_ctrl = NULL;
    }

    MSG_CTRL_INFO(msg_ctrl, "hd_sdio_msg_ctrl_deinit");

    /* remove workqueue */
    hd_msg_ctrl_del(msg_ctrl);

    return 0;
}

/**
 * @brief Initialize SDIO message controller
 * @param hd_msg_ctrl Pointer to message controller pointer (output)
 * @param sdio_manage Pointer to SDIO management structure
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_sdio_msg_ctrl_init(struct hd_msg_ctrl **hd_msg_ctrl, struct hd_sdio_manage *sdio_manage)
{
    struct hd_msg_ctrl *msg_ctrl;

    if (!hd_msg_ctrl || !sdio_manage) {
        return -EINVAL;
    }

    msg_ctrl = hd_msg_ctrl_create();
    if (!msg_ctrl) {
        return -ENOMEM;
    }

    /* register both upload and download callbacks */
    hd_sdio_manage_cplt_cb_register(sdio_manage, hd_msg_upld_recv_cplt_cb, msg_ctrl, hd_msg_dnld_send_cplt_cb, msg_ctrl);

    /* sdio mode */
    msg_ctrl->msg_hw_mode = 0;
    msg_ctrl->sdio_manage = sdio_manage;
    msg_ctrl->msg_dnld_max_size = sdio_manage->sdio_card->dnld_max_size;
    msg_ctrl->msg_upld_max_size = sdio_manage->sdio_card->upld_max_size;
    sdio_manage->msg_ctrl = msg_ctrl;

    *hd_msg_ctrl = msg_ctrl;

    MSG_CTRL_INFO(msg_ctrl, "hd_msg_ctrl_init success, msg_hw_mode: %d", msg_ctrl->msg_hw_mode);
    return 0;
}

/**
 * @brief Deinitialize USB message controller (placeholder)
 * @param hd_msg_ctrl Pointer to message controller structure
 * @retval 0 Success
 */
int hd_usb_msg_ctrl_deinit(struct hd_msg_ctrl *hd_msg_ctrl)
{
    return 0;
}

/**
 * @brief Initialize USB message controller (placeholder)
 * @param hd_msg_ctrl Pointer to message controller pointer (output)
 * @param usb_manage Pointer to USB management structure
 * @retval 0 Success
 */
int hd_usb_msg_ctrl_init(struct hd_msg_ctrl **hd_msg_ctrl, void *usb_manage)
{
    *hd_msg_ctrl = NULL;
    return -ENOMEM;
}

/**
 * @brief Unified callback registration for both upload receive and download send directions
 * @param[in] msg_ctrl Message control structure
 * @param[in] tag Message tag to register callbacks for
 * @param[in] upld_recv_cb Upload receive callback function (can be NULL to keep existing)
 * @param[in] upld_recv_arg User argument to pass to upload receive callback
 * @param[in] dnld_send_cb Download send completion callback function (can be NULL to keep existing)
 * @param[in] dnld_send_arg User argument to pass to download send callback
 * @retval 0 Success
 * @retval <0 Error code
 * @details This function provides a unified interface for registering both upload and download
 *          callbacks simultaneously for a specific message tag. Pass NULL for callback functions
 *          to leave the corresponding direction unchanged.
 */
int hd_msg_cb_register(struct hd_msg_ctrl *msg_ctrl, uint8_t tag,
                       hd_msg_cb_t upld_recv_cb, void *upld_recv_arg,
                       hd_msg_cb_t dnld_send_cb, void *dnld_send_arg)
{
    if (!msg_ctrl) {
        return -EINVAL;
    }

    if (tag >= HD_MSG_TAG_MAX) {
        MSG_CTRL_ERR(msg_ctrl, "tag is invalid: %d", tag);
        return -EINVAL;
    }

    /* Register upload receive callback if provided */
    if (upld_recv_cb) {
        /* Acquire upld mutex only for upld operations */
        mutex_lock(&msg_ctrl->upld_cb_mutex);

        if (msg_ctrl->hd_msg_upld_recv_cb[tag] != NULL) {
            mutex_unlock(&msg_ctrl->upld_cb_mutex);
            MSG_CTRL_ERR(msg_ctrl, "msg upld recv cb already registered for tag: %d", tag);
            return -EEXIST;
        }

        msg_ctrl->hd_msg_upld_recv_cb[tag] = upld_recv_cb;
        msg_ctrl->hd_msg_upld_recv_arg[tag] = upld_recv_arg;

        mutex_unlock(&msg_ctrl->upld_cb_mutex);
        MSG_CTRL_DBG(msg_ctrl, "msg upld recv cb registered for tag: %d", tag);
    }

    /* Register download send callback if provided */
    if (dnld_send_cb) {
        /* Acquire dnld mutex only for dnld operations */
        mutex_lock(&msg_ctrl->dnld_cb_mutex);

        if (msg_ctrl->hd_msg_dnld_send_cb[tag] != NULL) {
            mutex_unlock(&msg_ctrl->dnld_cb_mutex);
            MSG_CTRL_ERR(msg_ctrl, "msg dnld send cb already registered for tag: %d", tag);
            return -EEXIST;
        }

        msg_ctrl->hd_msg_dnld_send_cb[tag] = dnld_send_cb;
        msg_ctrl->hd_msg_dnld_send_arg[tag] = dnld_send_arg;

        mutex_unlock(&msg_ctrl->dnld_cb_mutex);
        MSG_CTRL_DBG(msg_ctrl, "msg dnld send cb registered for tag: %d", tag);
    }
    return 0;
}

/**
 * @brief Unified callback unregistration for both upload receive and download send directions
 * @param[in] msg_ctrl Message control structure
 * @param[in] tag Message tag to unregister callbacks for
 * @retval 0 Success
 * @retval <0 Error code
 * @details This function unregisters both upload receive and download send callbacks
 *          for the specified message tag simultaneously with mutex protection.
 */
int hd_msg_cb_unregister(struct hd_msg_ctrl *msg_ctrl, uint8_t tag)
{
    if (!msg_ctrl) {
        return -EINVAL;
    }

    if (tag >= HD_MSG_TAG_MAX) {
        MSG_CTRL_ERR(msg_ctrl, "tag is invalid: %d", tag);
        return -EINVAL;
    }

    /* Unregister upload receive callback */
    mutex_lock(&msg_ctrl->upld_cb_mutex);
    msg_ctrl->hd_msg_upld_recv_cb[tag] = NULL;
    msg_ctrl->hd_msg_upld_recv_arg[tag] = NULL;
    mutex_unlock(&msg_ctrl->upld_cb_mutex);

    /* Unregister download send callback */
    mutex_lock(&msg_ctrl->dnld_cb_mutex);
    msg_ctrl->hd_msg_dnld_send_cb[tag] = NULL;
    msg_ctrl->hd_msg_dnld_send_arg[tag] = NULL;
    mutex_unlock(&msg_ctrl->dnld_cb_mutex);

    MSG_CTRL_DBG(msg_ctrl, "msg callbacks unregistered for tag: %d", tag);
    return 0;
}

/**
 * @brief Send message through message controller
 * @param msg_ctrl Pointer to message controller structure
 * @param skb Socket buffer containing message to send
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_msg_ctrl_send(struct hd_msg_ctrl *msg_ctrl, struct sk_buff *skb)
{
    int ret;

    struct hd_msg_packt *msg_packt;

    if (!msg_ctrl || !skb) {
        MSG_CTRL_ERR(msg_ctrl, "send msg_ctrl or skb is NULL");
        return -EINVAL;
    }

    if (skb->len < sizeof(struct hd_msg_packt)) {
        MSG_CTRL_ERR(msg_ctrl, "send msg skb len is invalid: %d", skb->len);
        ret = -EINVAL;
        goto err_exit;
    }

    msg_packt = (struct hd_msg_packt *)skb->data;

    if (msg_packt->tag >= HD_MSG_TAG_MAX) {
        MSG_CTRL_ERR(msg_ctrl, "send msg skb tag is invalid: %d", msg_packt->tag);
        ret = -EINVAL;
        goto err_exit;
    }

    if (msg_packt->len + sizeof(struct hd_msg_packt) > skb->len) {
        MSG_CTRL_ERR(msg_ctrl, "send msg skb len is less than hdr len: %d < %lu", skb->len, msg_packt->len + sizeof(struct hd_msg_packt));
        ret = -EINVAL;
        goto err_exit;
    }

    if (msg_ctrl->msg_hw_mode == 0) {
        ret = hd_msg_sdio_send(msg_ctrl->sdio_manage, skb);
    } else {
        ret = hd_msg_usb_send(msg_ctrl->usb_manage, skb);
    }
    if (ret < 0) {
        MSG_CTRL_ERR(msg_ctrl, "send msg failed: %d", ret);
        /* Note: Error statistics will be updated by dnld completion callback */
        goto err_exit;
    }

    MSG_CTRL_DBG(msg_ctrl, "hd_msg_ctrl_send queued, tag: %d, len: %d", msg_packt->tag, msg_packt->len);
    /* Note: Success statistics will be updated by dnld completion callback */
    return 0;

err_exit:

    MSG_CTRL_ERR(msg_ctrl, "hd_msg_sdio_send failed: %d", ret);
    return ret;
}

/**
 * @brief Dump message controller statistics
 * @param[in] msg_ctrl Pointer to message controller structure
 * @retval None
 * @note Prints detailed statistics for all message tags
 */
void hd_msg_ctrl_dump_stats(struct hd_msg_ctrl *msg_ctrl)
{
    int i;

    if (!msg_ctrl) {
        return;
    }

    spin_lock(&msg_ctrl->stats_lock);

    MSG_CTRL_INFO(msg_ctrl, "MSG CTRL Statistics:\n");
    MSG_CTRL_INFO(msg_ctrl, "RX Errors: %lu, TX Errors: %lu, RX Dropped: %lu\n",
                  msg_ctrl->stats.rx_errors, msg_ctrl->stats.tx_errors, msg_ctrl->stats.rx_dropped);

    for (i = 0; i < HD_MSG_TAG_MAX; i++) {
        if (msg_ctrl->stats.rx_packets[i] > 0 || msg_ctrl->stats.tx_packets[i] > 0) {
            MSG_CTRL_INFO(msg_ctrl, "Tag %d: RX %lu packets/%lu bytes, TX %lu packets/%lu bytes\n",
                          i, msg_ctrl->stats.rx_packets[i], msg_ctrl->stats.rx_bytes[i],
                          msg_ctrl->stats.tx_packets[i], msg_ctrl->stats.tx_bytes[i]);
        }
    }

    /* Show queue status */
    MSG_CTRL_INFO(msg_ctrl, "Current upld queue length: %u/%d\n",
                  skb_queue_len(&msg_ctrl->upld_skb_head), HD_MSG_CTRL_UPLD_QUEUE_DEPTH_MAX);

    spin_unlock(&msg_ctrl->stats_lock);
}
