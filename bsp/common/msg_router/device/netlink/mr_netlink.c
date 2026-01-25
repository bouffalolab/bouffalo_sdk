/**
 * @file mr_netlink.c
 * @brief Netlink Device Implementation for SDIO-based Netlink Communication
 * @details Netlink implementation as an application module based on msg_ctrl:
 *          - Receives data from host via download path
 *          - Sends data to host via upload path with flow control
 *          - Manages state machine for handshake protocol
 *          - Provides circular credit-based flow control
 */

#include "bflb_core.h"
#include "bflb_mtimer.h"

#include "mm.h"
#include "board.h"
#include "shell.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_netlink.h"

#define DBG_TAG "NETLINK"
#include "log.h"

#define NETLINK_EVENT_DNLD_DONE          (1 << 0)
#define NETLINK_EVENT_UPLD_CREDIT_UPDATE (1 << 1)
#define NETLINK_EVENT_RESET              (1 << 2)
#define NETLINK_EVENT_EXIT               (1 << 3)

/**
 * @brief Wake up the Netlink processing task
 * @param[in] priv Pointer to Netlink private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int netlink_proc_task_wakeup(mr_netlink_priv_t *priv, uint32_t event)
{
    if (priv->netlink_proc_task == NULL) {
        LOG_E("%s netlink_proc_task is NULL, please init msg_ctrl first!\r\n", priv->netlink_cfg.name);
        return -1;
    }

    /* Wake up message processing task */
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(priv->netlink_proc_task, event, eSetBits, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        xTaskNotify(priv->netlink_proc_task, event, eSetBits);
    }

    return 0;
}

/*****************************************************************************
 * Message Transfer Callbacks (msg_*)
 * @note Called from msg_ctrl layer, runs in task context
 *****************************************************************************/

/**
 * @brief MSG download receive done callback, puts received message into queue for processing
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to Netlink private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_dnld_recv_done_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    int ret;
    mr_netlink_priv_t *priv = (mr_netlink_priv_t *)arg;

    if (!priv || !frame_elem) {
        return -1;
    }

    mr_netlink_msg_t *netlink_msg_pkt = MR_NETLINK_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);
    ret = mr_frame_queue_send(priv->msg_dnld_queue, &netlink_msg_pkt, 0);
    if (ret < 0) {
        LOG_E("%s Failed to send frame to download queue\r\n", priv->netlink_cfg.name);
        return -1;
    }

    netlink_proc_task_wakeup(priv, NETLINK_EVENT_DNLD_DONE);
    return 0;
}

/**
 * @brief MSG upload send done callback, releases data back to pool
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to Netlink private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_done_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    int ret;
    mr_netlink_priv_t *priv = (mr_netlink_priv_t *)arg;

    if (!priv || !frame_elem) {
        return -1;
    }

    mr_netlink_msg_t *netlink_msg_pkt = MR_NETLINK_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);
    if (priv->netlink_cfg.upld_done_cb) {
        /* User-defined callback */
        ret = priv->netlink_cfg.upld_done_cb(priv, netlink_msg_pkt);
    } else {
        /* Release back to pool */
        ret = mr_netlink_upld_elem_free(priv, netlink_msg_pkt);
    }

    return ret;
}

/**
 * @brief Hardware reset callback for message controller
 * @param[in] unused Unused parameter
 * @param[in] priv Pointer to Netlink private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_hw_reset_cb(mr_frame_elem_t *unused, void *arg)
{
    mr_netlink_priv_t *priv = (mr_netlink_priv_t *)arg;

    netlink_proc_task_wakeup(priv, NETLINK_EVENT_RESET);

    LOG_W("Received hw reset request\r\n");

    return 0;
}

/**
 * @brief Send MSG upload control packet
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] netlink_flag Netlink flag to send (DEVICE_START, CREDIT_UPDATE, etc.)
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_cmd_packet(mr_netlink_priv_t *priv, uint8_t netlink_flag, uint8_t *data, uint16_t data_size)
{
    int ret;
    mr_netlink_msg_t *netlink_msg_pkt;

    ret = mr_netlink_upld_elem_alloc(priv, &netlink_msg_pkt, 100);
    if (ret < 0) {
        LOG_E("%s Failed to allocate upld msg packet\r\n", priv->netlink_cfg.name);
        return -1;
    }

    netlink_msg_pkt->flag = netlink_flag;

    if (data && data_size > 0) {
        memcpy(netlink_msg_pkt->data, data, data_size);
        MR_NETLINK_MSG_PACKET_SET_DATA_SIZE(netlink_msg_pkt, data_size);
    } else {
        MR_NETLINK_MSG_PACKET_SET_DATA_SIZE(netlink_msg_pkt, 0);
    }

    /* send netlink_msg_pkt */
    ret = mr_netlink_upld_elem_send(priv, netlink_msg_pkt);
    if (ret < 0) {
        LOG_E("%s Failed to send NETLINK control message\r\n", priv->netlink_cfg.name);
        mr_netlink_upld_elem_free(priv, netlink_msg_pkt);
        return -1;
    }

    return 0;
}

/**
 * @brief Netlink processing task (daemon thread)
 * @details Main tasks:
 *   1. Process received messages from host (dnld path)
 *   2. Handle state machine transitions (handshake protocol)
 *   3. Monitor and report flow control credits
 *   4. Distribute messages based on packet flags
 * @param[in] arg Pointer to Netlink private structure
 */
static void netlink_proc_task(void *arg)
{
    int ret;
    mr_netlink_priv_t *priv = (mr_netlink_priv_t *)arg;

    mr_netlink_msg_t *netlink_msg_pkt = NULL;
    uint32_t notified_value = 0;
    uint32_t notified_mask = 0;

    LOG_I("%s daemon thread started\r\n", priv->netlink_cfg.name);

    /* Initialize state machine */
    priv->netlink_status = MR_NETLINK_DSTA_IDLE;

    while (!priv->stop_requested) {
        /* Wait for notification or timeout */
        uint32_t notified_new = 0;
        if (notified_value) {
            xTaskNotifyWait(0, 0xffffffff, &notified_new, pdMS_TO_TICKS(0));
        } else {
            xTaskNotifyWait(0, 0xffffffff, &notified_new, pdMS_TO_TICKS(priv->netlink_cfg.task_period_max_ms));
        }
        notified_value |= notified_new;

        /* task callback */
        if (priv->netlink_cfg.netlink_task_cb) {
            priv->netlink_cfg.netlink_task_cb(priv, &notified_value);
        }

        /* reset event */
        notified_mask = NETLINK_EVENT_RESET;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;
            LOG_W("%s processing reset request\r\n", priv->netlink_cfg.name);

            priv->netlink_status = MR_NETLINK_DSTA_IDLE;
        }

        /* dnld_done event */
        notified_mask = NETLINK_EVENT_DNLD_DONE;
        if (notified_value & notified_mask) {
            ret = mr_frame_queue_receive(priv->msg_dnld_queue, &netlink_msg_pkt, 0);
            if (ret < 0) {
                /* queue empty */
                notified_value &= ~notified_mask;
                netlink_msg_pkt = NULL;
            }
        }

        /* Handle reset/stop commands - reset state machine */
        if (netlink_msg_pkt && (netlink_msg_pkt->flag == MR_NETLINK_FLAG_HOST_RESET ||
                                netlink_msg_pkt->flag == MR_NETLINK_FLAG_HOST_STOP)) {
            LOG_W("%s received host %s command\r\n", priv->netlink_cfg.name,
                  netlink_msg_pkt->flag == MR_NETLINK_FLAG_HOST_RESET ? "RESET" : "STOP");

            priv->netlink_status = MR_NETLINK_DSTA_IDLE;
            priv->device_dnld_credit_limit = priv->netlink_cfg.dnld_credit_max;
            priv->credit_limit_update_last = 0;

            mr_netlink_dnld_elem_free(priv, netlink_msg_pkt);
            netlink_msg_pkt = NULL;
            continue;
        }

        /* State machine processing */
        switch (priv->netlink_status) {
            case MR_NETLINK_DSTA_IDLE:
                /* Wait for HOST_READY message */
                if (netlink_msg_pkt == NULL || netlink_msg_pkt->flag != MR_NETLINK_FLAG_HOST_READY) {
                    break;
                }
                LOG_I("%s received HOST_READY message\r\n", priv->netlink_cfg.name);

                priv->netlink_status = MR_NETLINK_DSTA_HOST_READY;
                /* Fall through to immediately send DEVICE_START */

            case MR_NETLINK_DSTA_HOST_READY:
                /* Host is ready, send DEVICE_START with initial credit */
                priv->device_dnld_credit_limit = priv->netlink_cfg.dnld_credit_max;
                priv->credit_limit_update_last = 0;

                ret = msg_upld_send_cmd_packet(priv, MR_NETLINK_FLAG_DEVICE_START, NULL, 0);
                if (ret < 0) {
                    LOG_E("%s Failed to send DEVICE_START command\r\n", priv->netlink_cfg.name);
                    break;
                }

                priv->netlink_status = MR_NETLINK_DSTA_DEVICE_RUN;
                break;

            case MR_NETLINK_DSTA_DEVICE_RUN:

                if (netlink_msg_pkt == NULL) {
                    break;

                } else if (netlink_msg_pkt->flag == MR_NETLINK_FLAG_DNLD_DATA) {
                    /* Download data packet - pass to user callback */
                    ret = priv->netlink_cfg.dnld_output_cb(priv, netlink_msg_pkt);
                    if (ret < 0) {
                        LOG_E("%s Failed to output dnld data\r\n", priv->netlink_cfg.name);
                        break;
                    }
                    netlink_msg_pkt = NULL;
                    break;

                } else if (netlink_msg_pkt->flag == MR_NETLINK_FLAG_DEVICE_STOP) {
                    LOG_I("%s received DEVICE_STOP message\r\n", priv->netlink_cfg.name);
                    priv->netlink_status = MR_NETLINK_DSTA_IDLE;
                    break;
                }

                break;

            default:
                priv->netlink_status = MR_NETLINK_DSTA_IDLE;
                LOG_E("%s Invalid NETLINK status: %d\r\n", priv->netlink_cfg.name, priv->netlink_status);
                break;
        }

        /* Release received packet if not handled by callback */
        if (netlink_msg_pkt) {
            mr_netlink_dnld_elem_free(priv, netlink_msg_pkt);
            netlink_msg_pkt = NULL;
        }

        /* Proactively send credit update if threshold exceeded */
        notified_mask = NETLINK_EVENT_UPLD_CREDIT_UPDATE;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;

            if (priv->netlink_cfg.dnld_credit_max && priv->netlink_status == MR_NETLINK_DSTA_DEVICE_RUN &&
                (int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) >
                    priv->netlink_cfg.upld_credit_update_threshold) {
                ret = msg_upld_send_cmd_packet(priv, MR_NETLINK_FLAG_CREDIT_UPDATE, NULL, 0);
                if (ret < 0) {
                    LOG_E("%s Failed to send CREDIT_UPDATE message\r\n", priv->netlink_cfg.name);
                }
            }
        }

        /* unknown event */
        notified_mask = ~(NETLINK_EVENT_DNLD_DONE);
        if (notified_value & notified_mask) {
            LOG_W("%s unknown event: 0x%08x\r\n", priv->netlink_cfg.name, (notified_value & notified_mask));
            notified_value &= ~notified_mask;
        }
    }

    LOG_I("%s Netlink daemon thread exiting\r\n", priv->netlink_cfg.name);
    vTaskDelete(NULL);
}

/*****************************************************************************
 * Public API Implementation
 * @note These functions can be called by upper layer applications
 *****************************************************************************/

/**
 * @brief Allocate upload message packet buffer from pool
 * @param[in] priv Pointer to Netlink private structure
 * @param[out] netlink_msg_pkt Pointer to store allocated message packet
 * @param[in] timeout Timeout in ticks to wait for buffer
 * @retval 0 Success
 * @retval <0 No buffer available
 */
int mr_netlink_upld_elem_alloc(mr_netlink_priv_t *priv, mr_netlink_msg_t **netlink_msg_pkt, uint32_t timeout)
{
    int ret;
    mr_frame_elem_t *frame_elem;

    ret = mr_frame_queue_alloc_elem(priv->upld_frame_ctrl, &frame_elem, timeout);
    if (ret < 0) {
        /* No available buffer, try later */
        *netlink_msg_pkt = NULL;
        return -1;
    }

    *netlink_msg_pkt = MR_NETLINK_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);

    MR_NETLINK_MSG_PACKET_SET_DATA_SIZE((*netlink_msg_pkt), 0);
    (*netlink_msg_pkt)->msg_pkt.tag = priv->netlink_cfg.msg_tag;
    (*netlink_msg_pkt)->msg_pkt.sub_tag = 0;
    (*netlink_msg_pkt)->flag = MR_NETLINK_FLAG_UPLD_DATA; /* Upload data flag */
    (*netlink_msg_pkt)->credit_update_flag = false;
    (*netlink_msg_pkt)->credit_limit_cnt = 0;

    return 0;
}

/**
 * @brief Send allocated upload message packet
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] netlink_msg_pkt Message packet (must be allocated by mr_netlink_upld_elem_alloc)
 * @retval 0 Success
 * @retval <0 Error
 * @note Automatically attaches flow control credit if configured
 */
int mr_netlink_upld_elem_send(mr_netlink_priv_t *priv, mr_netlink_msg_t *netlink_msg_pkt)
{
    int ret;
    uintptr_t flag;
    mr_frame_elem_t *frame_elem = MR_NETLINK_MSG_PACKET_TO_FRAME_ELEM_ADDR(netlink_msg_pkt);

    if (frame_elem->data_size > frame_elem->buff_size) {
        LOG_E("%s upld data size too large: %d > %d\r\n", priv->netlink_cfg.name, frame_elem->data_size,
              frame_elem->buff_size);
    }

    /* Lock protection to prevent race conditions with flow control updates
         * and ensure credit values are sent in order */
    flag = bflb_irq_save();

    if ((priv->netlink_cfg.dnld_credit_max != 0) &&
        (priv->credit_limit_update_last != priv->device_dnld_credit_limit) &&
        (netlink_msg_pkt->flag == MR_NETLINK_FLAG_UPLD_DATA ||
         netlink_msg_pkt->flag == MR_NETLINK_FLAG_CREDIT_UPDATE ||
         netlink_msg_pkt->flag == MR_NETLINK_FLAG_DEVICE_START)) {
        /* Send through msg ctrl */
        netlink_msg_pkt->credit_update_flag = true;
        netlink_msg_pkt->credit_limit_cnt = priv->device_dnld_credit_limit;
        priv->credit_limit_update_last = priv->device_dnld_credit_limit;

        ret = mr_msg_ctrl_upld_send(priv->netlink_cfg.msg_ctrl, frame_elem);

        bflb_irq_restore(flag);

    } else {
        bflb_irq_restore(flag);

        if (netlink_msg_pkt->flag == MR_NETLINK_FLAG_CREDIT_UPDATE) {
            /* No credit update needed, skip sending CREDIT_UPDATE message */
            mr_netlink_upld_elem_free(priv, netlink_msg_pkt);
            return 0;
        }

        netlink_msg_pkt->credit_update_flag = false;
        netlink_msg_pkt->credit_limit_cnt = 0;
        ret = mr_msg_ctrl_upld_send(priv->netlink_cfg.msg_ctrl, frame_elem);
    }

    if (ret < 0) {
        LOG_E("%s Failed to send NETLINK upld data to msg_ctrl: %d\r\n", priv->netlink_cfg.name, ret);
        return -1;
    }

    return 0;
}

/**
 * @brief Free upload message packet buffer back to pool
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] netlink_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Equivalent to mr_frame_queue_free_elem, must be called in upld_done_cb
 */
int mr_netlink_upld_elem_free(mr_netlink_priv_t *priv, mr_netlink_msg_t *netlink_msg_pkt)
{
    int ret;

    mr_frame_elem_t *frame_elem = MR_NETLINK_MSG_PACKET_TO_FRAME_ELEM_ADDR(netlink_msg_pkt);
    ret = mr_frame_queue_free_elem(frame_elem);
    if (ret < 0) {
        LOG_E("%s Failed to free upld frame element\r\n", priv->netlink_cfg.name);
        return ret;
    }

    return 0;
}

/**
 * @brief Free download message packet and update flow control
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] netlink_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Must be called after processing download data, automatically updates credit counter
 */
int mr_netlink_dnld_elem_free(mr_netlink_priv_t *priv, mr_netlink_msg_t *netlink_msg_pkt)
{
    int ret;

    if (!priv || !netlink_msg_pkt) {
        return -1;
    }

    uint8_t pkt_flag = netlink_msg_pkt->flag;

    /* Free frame element */
    mr_frame_elem_t *frame_elem = MR_NETLINK_MSG_PACKET_TO_FRAME_ELEM_ADDR(netlink_msg_pkt);
    ret = mr_msg_ctrl_dnld_free(priv->netlink_cfg.msg_ctrl, frame_elem);
    if (ret < 0) {
        LOG_E("%s Failed to free dnld frame element\r\n", priv->netlink_cfg.name);
        return ret;
    }

    /* Update flow control credit counter */
    if (priv->netlink_cfg.dnld_credit_max &&
        priv->netlink_status == MR_NETLINK_DSTA_DEVICE_RUN &&
        pkt_flag == MR_NETLINK_FLAG_DNLD_DATA) {
        /* Lock protection for atomic credit update */
        uintptr_t flag = bflb_irq_save();
        bool wakeup_needed = false;
        priv->device_dnld_credit_limit += 1; /* Circular increment (wraps at 256) */
        if ((int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) >=
            priv->netlink_cfg.upld_credit_update_threshold) {
            wakeup_needed = true;
        }
        /* Unlock */
        bflb_irq_restore(flag);

        /* Wake up processing task to send credit update if threshold reached */
        if (wakeup_needed) {
            netlink_proc_task_wakeup(priv, NETLINK_EVENT_UPLD_CREDIT_UPDATE);
        }
    }

    return 0;
}

/**
 * @brief Initialize Netlink device instance
 * @param[in] cfg Pointer to Netlink configuration structure
 * @retval Pointer to Netlink private structure on success
 * @retval NULL on failure
 */
mr_netlink_priv_t *mr_netlink_init(mr_netlink_cfg_t *cfg)
{
    int ret;
    mr_netlink_priv_t *priv;

    /* Parameter validation */
    if (cfg == NULL ||
        cfg->msg_ctrl == NULL ||
        cfg->upld_frame_size <= sizeof(mr_netlink_msg_t) ||
        cfg->upld_frame_count < 2 ||
        cfg->upld_frame_buff == NULL ||
        (cfg->dnld_credit_max && cfg->dnld_credit_max <= cfg->upld_credit_update_threshold) ||
        cfg->dnld_credit_max > 127 ||
        cfg->dnld_output_cb == NULL) {
        LOG_E("%s Invalid parameters for NETLINK initialization\r\n", cfg->name);
        return NULL;
    }

    /* Allocate private structure */
    priv = malloc(sizeof(mr_netlink_priv_t));
    if (!priv) {
        LOG_E("%s Failed to allocate NETLINK private structure\r\n", cfg->name);
        return NULL;
    }
    memset(priv, 0, sizeof(mr_netlink_priv_t));

    /* Copy configuration */
    priv->netlink_cfg = *cfg;

    /* Initialize upload frame buffer pool */
    mr_frame_queue_ctrl_init_cfg_t upld_frame_cfg = {
        .frame_buff = priv->netlink_cfg.upld_frame_buff,
        .frame_type = priv->netlink_cfg.upld_frame_type,
        .frame_elem_cnt = priv->netlink_cfg.upld_frame_count,
        .frame_elem_size = priv->netlink_cfg.upld_frame_size,
        .name = priv->netlink_cfg.name
    };
    priv->upld_frame_ctrl = mr_frame_queue_create(&upld_frame_cfg);
    if (!priv->upld_frame_ctrl) {
        LOG_E("%s Failed to create NETLINK frame queue\r\n", priv->netlink_cfg.name);
        goto error_exit;
    }

    /* Create download message queue */
    priv->msg_dnld_queue = xQueueCreate(priv->netlink_cfg.msg_ctrl->cfg.dnld_frame_count + 1, sizeof(mr_netlink_msg_t *));
    if (!priv->msg_dnld_queue) {
        LOG_E("%s Failed to create msg download queue\r\n", priv->netlink_cfg.name);
        goto error_exit;
    }

    /* Register callbacks with msg_ctrl */
    ret = mr_msg_cb_register(priv->netlink_cfg.msg_ctrl, priv->netlink_cfg.msg_tag,
                             msg_dnld_recv_done_cb, priv,
                             msg_upld_send_done_cb, priv,
                             msg_hw_reset_cb, priv);
    if (ret < 0) {
        LOG_E("%s Failed to register msg callbacks\r\n", priv->netlink_cfg.name);
        goto error_exit;
    }

    /* Create daemon task */
    ret = xTaskCreate(netlink_proc_task, priv->netlink_cfg.name, priv->netlink_cfg.task_stack_size, priv,
                      priv->netlink_cfg.task_priority, &priv->netlink_proc_task);
    if (ret != pdPASS) {
        LOG_E("%s Failed to create NETLINK processing task\r\n", priv->netlink_cfg.name);
        goto error_exit;
    }

    LOG_I("%s NETLINK device initialized successfully\r\n", priv->netlink_cfg.name);
    return priv;

error_exit:
    if (priv->msg_dnld_queue) {
        vQueueDelete(priv->msg_dnld_queue);
    }
    if (priv->upld_frame_ctrl) {
        mr_frame_queue_remove(priv->upld_frame_ctrl);
    }
    if (priv->netlink_cfg.msg_ctrl) {
        mr_msg_cb_unregister(priv->netlink_cfg.msg_ctrl, priv->netlink_cfg.msg_tag);
    }
    free(priv);
    LOG_E("%s device initialization failed\r\n", cfg->name);
    return NULL;
}
