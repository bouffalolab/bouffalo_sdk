/**
 * @file mr_virtualchan.c
 * @brief Virtual channel device implementation - SDIO-based virtual channel communication
 * @details Virtual channel implementation as an application module based on msg_ctrl:
 *          - Receive data from host via download path
 *          - Send data to host via upload path with flow control
 *          - Manage state machine for handshake protocol
 *          - Provide circular credit-based flow control mechanism
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
#include "mr_virtualchan.h"

#define DBG_TAG "VIRTUALCHAN"
#include "log.h"

#define VIRTUALCHAN_EVENT_DNLD_DONE          (1 << 0)
#define VIRTUALCHAN_EVENT_UPLD_CREDIT_UPDATE (1 << 1)
#define VIRTUALCHAN_EVENT_RESET              (1 << 2)
#define VIRTUALCHAN_EVENT_EXIT               (1 << 3)

/**
 * @brief Wake up virtual channel processing task
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] event Event flag
 * @retval 0 Success
 * @retval <0 Error
 */
static int virtualchan_proc_task_wakeup(mr_virtualchan_priv_t *priv, uint32_t event)
{
    if (priv->virtualchan_proc_task == NULL) {
        LOG_E("%s virtualchan_proc_task is NULL, please init msg_ctrl first!\r\n", priv->virtualchan_cfg.name);
        return -1;
    }

    /* Wake up message processing task */
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(priv->virtualchan_proc_task, event, eSetBits, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        xTaskNotify(priv->virtualchan_proc_task, event, eSetBits);
    }

    return 0;
}

/*****************************************************************************
 * Message Transfer Callback Functions (msg_*)
 * @note Called from msg_ctrl layer, runs in task context
 *****************************************************************************/

/**
 * @brief MSG download receive completion callback, queues received message for processing
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to virtual channel private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_dnld_recv_done_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    int ret;
    mr_virtualchan_priv_t *priv = (mr_virtualchan_priv_t *)arg;

    if (!priv || !frame_elem) {
        return -1;
    }

    mr_virtualchan_msg_t *virtualchan_msg_pkt = MR_VIRTUALCHAN_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);
    ret = mr_frame_queue_send(priv->msg_dnld_queue, &virtualchan_msg_pkt, 0);
    if (ret < 0) {
        LOG_E("%s Failed to send frame to download queue\r\n", priv->virtualchan_cfg.name);
        return -1;
    }

    virtualchan_proc_task_wakeup(priv, VIRTUALCHAN_EVENT_DNLD_DONE);
    return 0;
}

/**
 * @brief MSG upload send completion callback, frees data back to pool
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to virtual channel private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_done_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    int ret;
    mr_virtualchan_priv_t *priv = (mr_virtualchan_priv_t *)arg;

    if (!priv || !frame_elem) {
        return -1;
    }

    mr_virtualchan_msg_t *virtualchan_msg_pkt = MR_VIRTUALCHAN_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);
    if (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_UPLD_DATA && priv->virtualchan_cfg.upld_done_cb) {
        /* User defined callback */
        ret = priv->virtualchan_cfg.upld_done_cb(priv, virtualchan_msg_pkt);
    } else {
        /* Free back to pool */
        ret = mr_virtualchan_upld_elem_free(priv, virtualchan_msg_pkt);
    }

    return ret;
}

/**
 * @brief Message controller hardware reset callback
 * @param[in] unused Unused parameter
 * @param[in] arg Pointer to virtual channel private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_hw_reset_cb(mr_frame_elem_t *unused, void *arg)
{
    mr_virtualchan_priv_t *priv = (mr_virtualchan_priv_t *)arg;

    virtualchan_proc_task_wakeup(priv, VIRTUALCHAN_EVENT_RESET);

    LOG_W("Received hw reset request\r\n");

    return 0;
}

/**
 * @brief Send MSG upload control packet
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] virtualchan_flag Virtual channel flag to send (DEVICE_START, CREDIT_UPDATE, etc.)
 * @param[in] data Data pointer
 * @param[in] data_size Data size
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_cmd_packet(mr_virtualchan_priv_t *priv, uint8_t virtualchan_flag, uint8_t *data, uint16_t data_size)
{
    int ret;
    mr_virtualchan_msg_t *virtualchan_msg_pkt;

    ret = mr_virtualchan_upld_elem_alloc(priv, &virtualchan_msg_pkt, 100);
    if (ret < 0) {
        LOG_E("%s Failed to allocate upld msg packet\r\n", priv->virtualchan_cfg.name);
        return -1;
    }

    virtualchan_msg_pkt->flag = virtualchan_flag;

    if (data && data_size > 0) {
        memcpy(virtualchan_msg_pkt->data, data, data_size);
        MR_VIRTUALCHAN_MSG_PACKET_SET_DATA_SIZE(virtualchan_msg_pkt, data_size);
    } else {
        MR_VIRTUALCHAN_MSG_PACKET_SET_DATA_SIZE(virtualchan_msg_pkt, 0);
    }

    /* Send virtualchan_msg_pkt */
    ret = mr_virtualchan_upld_elem_send(priv, virtualchan_msg_pkt);
    if (ret < 0) {
        LOG_E("%s Failed to send VIRTUALCHAN control message\r\n", priv->virtualchan_cfg.name);
        mr_virtualchan_upld_elem_free(priv, virtualchan_msg_pkt);
        return -1;
    }

    return 0;
}

/**
 * @brief Virtual channel processing task (daemon thread)
 * @details Main tasks:
 *   1. Process messages received from host (download path)
 *   2. Handle state machine transitions (handshake protocol)
 *   3. Monitor and report flow control credits
 *   4. Distribute messages based on packet flags
 * @param[in] arg Pointer to virtual channel private structure
 */
static void virtualchan_proc_task(void *arg)
{
    int ret;
    mr_virtualchan_priv_t *priv = (mr_virtualchan_priv_t *)arg;

    mr_virtualchan_msg_t *virtualchan_msg_pkt = NULL;
    uint32_t notified_value = 0;
    uint32_t notified_mask = 0;

    LOG_I("%s daemon thread started\r\n", priv->virtualchan_cfg.name);

    /* Initialize state machine */
    priv->virtualchan_status = MR_VIRTUALCHAN_DSTA_IDLE;

    while (!priv->stop_requested) {
        /* Wait for notification or timeout */
        uint32_t notified_new = 0;
        if (notified_value) {
            xTaskNotifyWait(0, 0xffffffff, &notified_new, pdMS_TO_TICKS(0));
        } else {
            xTaskNotifyWait(0, 0xffffffff, &notified_new, pdMS_TO_TICKS(priv->virtualchan_cfg.task_period_max_ms));
        }
        notified_value |= notified_new;

        /* Task callback */
        if (priv->virtualchan_cfg.virtualchan_task_cb) {
            priv->virtualchan_cfg.virtualchan_task_cb(priv, &notified_value);
        }

        /* Reset event */
        notified_mask = VIRTUALCHAN_EVENT_RESET;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;
            LOG_W("%s processing reset request\r\n", priv->virtualchan_cfg.name);

            priv->virtualchan_status = MR_VIRTUALCHAN_DSTA_IDLE;
        }

        /* dnld_done event */
        notified_mask = VIRTUALCHAN_EVENT_DNLD_DONE;
        if (notified_value & notified_mask) {
            ret = mr_frame_queue_receive(priv->msg_dnld_queue, &virtualchan_msg_pkt, 0);
            if (ret < 0) {
                /* Queue is empty */
                notified_value &= ~notified_mask;
                virtualchan_msg_pkt = NULL;
            }
        }

        /* Handle reset/stop commands - reset state machine */
        if (virtualchan_msg_pkt && (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_HOST_RESET ||
                                    virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_HOST_STOP)) {
            LOG_W("%s received host %s command\r\n", priv->virtualchan_cfg.name,
                  virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_HOST_RESET ? "RESET" : "STOP");

            priv->virtualchan_status = MR_VIRTUALCHAN_DSTA_IDLE;
            priv->device_dnld_credit_limit = priv->virtualchan_cfg.dnld_credit_max;
            priv->credit_limit_update_last = 0;

            mr_virtualchan_dnld_elem_free(priv, virtualchan_msg_pkt);
            virtualchan_msg_pkt = NULL;
            continue;
        }

        /* State machine processing */
        switch (priv->virtualchan_status) {
            case MR_VIRTUALCHAN_DSTA_IDLE:
                /* Wait for HOST_READY message */
                if (virtualchan_msg_pkt == NULL || virtualchan_msg_pkt->flag != MR_VIRTUALCHAN_FLAG_HOST_READY) {
                    break;
                }
                LOG_I("%s received HOST_READY message\r\n", priv->virtualchan_cfg.name);

                priv->virtualchan_status = MR_VIRTUALCHAN_DSTA_HOST_READY;
                /* Immediately send DEVICE_START */

            case MR_VIRTUALCHAN_DSTA_HOST_READY:
                /* Host ready, send DEVICE_START and initial credit */
                priv->device_dnld_credit_limit = priv->virtualchan_cfg.dnld_credit_max;
                priv->credit_limit_update_last = 0;

                ret = msg_upld_send_cmd_packet(priv, MR_VIRTUALCHAN_FLAG_DEVICE_START, NULL, 0);
                if (ret < 0) {
                    LOG_E("%s Failed to send DEVICE_START command\r\n", priv->virtualchan_cfg.name);
                    break;
                }

                priv->virtualchan_status = MR_VIRTUALCHAN_DSTA_DEVICE_RUN;
                break;

            case MR_VIRTUALCHAN_DSTA_DEVICE_RUN:

                if (virtualchan_msg_pkt == NULL) {
                    break;
                } else if (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_DNLD_DATA) {
                    if (NULL == priv->virtualchan_cfg.dnld_output_cb) {
                        break;
                    }
                    /* Download data packet - pass to user callback */
                    ret = priv->virtualchan_cfg.dnld_output_cb(priv, virtualchan_msg_pkt);
                    if (ret < 0) {
                        LOG_E("%s Failed to output dnld data\r\n", priv->virtualchan_cfg.name);
                        break;
                    }
                    virtualchan_msg_pkt = NULL;
                    break;

                } else if (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_DEVICE_STOP) {
                    LOG_I("%s received DEVICE_STOP message\r\n", priv->virtualchan_cfg.name);
                    priv->virtualchan_status = MR_VIRTUALCHAN_DSTA_IDLE;
                    break;
                }

                break;

            default:
                priv->virtualchan_status = MR_VIRTUALCHAN_DSTA_IDLE;
                LOG_E("%s Invalid VIRTUALCHAN status: %d\r\n", priv->virtualchan_cfg.name, priv->virtualchan_status);
                break;
        }

        /* If callback didn't process, free received packet */
        if (virtualchan_msg_pkt) {
            mr_virtualchan_dnld_elem_free(priv, virtualchan_msg_pkt);
            virtualchan_msg_pkt = NULL;
        }

        /* Proactively send credit update if threshold exceeded */
        notified_mask = VIRTUALCHAN_EVENT_UPLD_CREDIT_UPDATE;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;

            if (priv->virtualchan_cfg.dnld_credit_max && priv->virtualchan_status == MR_VIRTUALCHAN_DSTA_DEVICE_RUN &&
                (int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) >
                    priv->virtualchan_cfg.upld_credit_update_threshold) {
                ret = msg_upld_send_cmd_packet(priv, MR_VIRTUALCHAN_FLAG_CREDIT_UPDATE, NULL, 0);
                if (ret < 0) {
                    LOG_E("%s Failed to send CREDIT_UPDATE message\r\n", priv->virtualchan_cfg.name);
                }
            }
        }

        /* Unknown events */
        notified_mask = ~(VIRTUALCHAN_EVENT_DNLD_DONE);
        if (notified_value & notified_mask) {
            LOG_W("%s unknown event: 0x%08x\r\n", priv->virtualchan_cfg.name, (notified_value & notified_mask));
            notified_value &= ~notified_mask;
        }
    }

    LOG_I("%s Virtualchan daemon thread exiting\r\n", priv->virtualchan_cfg.name);
    vTaskDelete(NULL);
}

/*****************************************************************************
 * Public API Implementation
 * @note These functions can be called by upper applications
 *****************************************************************************/

/**
 * @brief Allocate upload message packet buffer from pool
 * @param[in] priv Pointer to virtual channel private structure
 * @param[out] virtualchan_msg_pkt Pointer to store allocated message packet
 * @param[in] timeout Timeout in ticks to wait for buffer
 * @retval 0 Success
 * @retval <0 No buffer available
 */
int mr_virtualchan_upld_elem_alloc(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t **virtualchan_msg_pkt, uint32_t timeout)
{
    int ret;
    mr_frame_elem_t *frame_elem;

    ret = mr_frame_queue_alloc_elem(priv->upld_frame_ctrl, &frame_elem, timeout);
    if (ret < 0) {
        /* No buffer available, try later */
        *virtualchan_msg_pkt = NULL;
        return -1;
    }

    *virtualchan_msg_pkt = MR_VIRTUALCHAN_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);

    MR_VIRTUALCHAN_MSG_PACKET_SET_DATA_SIZE((*virtualchan_msg_pkt), 0);
    (*virtualchan_msg_pkt)->msg_pkt.tag = priv->virtualchan_cfg.msg_tag;
    (*virtualchan_msg_pkt)->msg_pkt.sub_tag = 0;
    (*virtualchan_msg_pkt)->flag = MR_VIRTUALCHAN_FLAG_UPLD_DATA; /* Upload data flag */
    (*virtualchan_msg_pkt)->credit_update_flag = false;
    (*virtualchan_msg_pkt)->credit_limit_cnt = 0;

    return 0;
}

/**
 * @brief Send allocated upload message packet
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] virtualchan_msg_pkt Message packet (must be allocated by mr_virtualchan_upld_elem_alloc)
 * @retval 0 Success
 * @retval <0 Error
 * @note Automatically attaches flow control credit if configured
 */
int mr_virtualchan_upld_elem_send(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt)
{
    int ret;
    uintptr_t flag;
    mr_frame_elem_t *frame_elem = MR_VIRTUALCHAN_MSG_PACKET_TO_FRAME_ELEM_ADDR(virtualchan_msg_pkt);

    if (frame_elem->data_size > frame_elem->buff_size) {
        LOG_E("%s upld data size too large: %d > %d\r\n", priv->virtualchan_cfg.name, frame_elem->data_size,
              frame_elem->buff_size);
    }

    /* Lock protection to prevent race condition with flow control update
     * and ensure credit value is sent in order */
    flag = bflb_irq_save();

    if ((priv->virtualchan_cfg.dnld_credit_max != 0) &&
        (priv->credit_limit_update_last != priv->device_dnld_credit_limit) &&
        (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_UPLD_DATA ||
         virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_CREDIT_UPDATE ||
         virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_DEVICE_START)) {
        /* Send via msg ctrl */
        virtualchan_msg_pkt->credit_update_flag = true;
        virtualchan_msg_pkt->credit_limit_cnt = priv->device_dnld_credit_limit;
        priv->credit_limit_update_last = priv->device_dnld_credit_limit;

        ret = mr_msg_ctrl_upld_send(priv->virtualchan_cfg.msg_ctrl, frame_elem);

        bflb_irq_restore(flag);

    } else {
        bflb_irq_restore(flag);

        if (virtualchan_msg_pkt->flag == MR_VIRTUALCHAN_FLAG_CREDIT_UPDATE) {
            /* No credit update needed, skip sending CREDIT_UPDATE message */
            mr_virtualchan_upld_elem_free(priv, virtualchan_msg_pkt);
            return 0;
        }

        virtualchan_msg_pkt->credit_update_flag = false;
        virtualchan_msg_pkt->credit_limit_cnt = 0;
        ret = mr_msg_ctrl_upld_send(priv->virtualchan_cfg.msg_ctrl, frame_elem);
    }

    if (ret < 0) {
        LOG_E("%s Failed to send VIRTUALCHAN upld data to msg_ctrl: %d\r\n", priv->virtualchan_cfg.name, ret);
        return -1;
    }

    return 0;
}

/**
 * @brief Free upload message packet buffer back to pool
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] virtualchan_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Equivalent to mr_frame_queue_free_elem, must be called in upld_done_cb
 */
int mr_virtualchan_upld_elem_free(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt)
{
    int ret;

    mr_frame_elem_t *frame_elem = MR_VIRTUALCHAN_MSG_PACKET_TO_FRAME_ELEM_ADDR(virtualchan_msg_pkt);
    ret = mr_frame_queue_free_elem(frame_elem);
    if (ret < 0) {
        LOG_E("%s Failed to free upld frame element\r\n", priv->virtualchan_cfg.name);
        return ret;
    }

    return 0;
}

/**
 * @brief Free download message packet and update flow control
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] virtualchan_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Must be called after processing download data, automatically updates credit counter
 */
int mr_virtualchan_dnld_elem_free(mr_virtualchan_priv_t *priv, mr_virtualchan_msg_t *virtualchan_msg_pkt)
{
    int ret;

    if (!priv || !virtualchan_msg_pkt) {
        return -1;
    }

    uint8_t pkt_flag = virtualchan_msg_pkt->flag;

    /* Free frame element */
    mr_frame_elem_t *frame_elem = MR_VIRTUALCHAN_MSG_PACKET_TO_FRAME_ELEM_ADDR(virtualchan_msg_pkt);
    ret = mr_msg_ctrl_dnld_free(priv->virtualchan_cfg.msg_ctrl, frame_elem);
    if (ret < 0) {
        LOG_E("%s Failed to free dnld frame element\r\n", priv->virtualchan_cfg.name);
        return ret;
    }

    /* Update flow control credit counter */
    if (priv->virtualchan_cfg.dnld_credit_max &&
        priv->virtualchan_status == MR_VIRTUALCHAN_DSTA_DEVICE_RUN &&
        pkt_flag == MR_VIRTUALCHAN_FLAG_DNLD_DATA) {
        /* Lock protection for atomic credit update */
        uintptr_t flag = bflb_irq_save();
        bool wakeup_needed = false;
        priv->device_dnld_credit_limit += 1; /* Circular increment (wraps at 256) */
        if ((int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) >=
            priv->virtualchan_cfg.upld_credit_update_threshold) {
            wakeup_needed = true;
        }
        /* Unlock */
        bflb_irq_restore(flag);

        /* Wake up processing task to send credit update when threshold is reached */
        if (wakeup_needed) {
            virtualchan_proc_task_wakeup(priv, VIRTUALCHAN_EVENT_UPLD_CREDIT_UPDATE);
        }
    }

    return 0;
}

/**
 * @brief Send virtual channel upload data
 * @param[in] priv Pointer to virtual channel private structure
 * @param[in] data_buff Pointer to data buffer to send
 * @param[in] data_size Size of data to send
 * @retval 0 Success
 * @retval <0 Error
 */
int mr_virtualchan_upld_data_send(mr_virtualchan_priv_t *priv, const uint8_t *data_buff, uint16_t data_size)
{
    int ret;
    mr_virtualchan_msg_t *virtualchan_msg_pkt;

    if (!priv || !data_buff || data_size == 0) {
        return -1;
    }

    /* Allocate message packet */
    ret = mr_virtualchan_upld_elem_alloc(priv, &virtualchan_msg_pkt, 100);
    if (ret < 0) {
        return -1; /* No buffer available, try later */
    }

    /* Check buffer size */
    if (data_size > MR_VIRTUALCHAN_MSG_PACKET_GET_BUFF_SIZE(virtualchan_msg_pkt)) {
        LOG_E("%s upld data size too large: %d > %d\r\n", priv->virtualchan_cfg.name, data_size,
                  MR_VIRTUALCHAN_MSG_PACKET_GET_BUFF_SIZE(virtualchan_msg_pkt));
        mr_virtualchan_upld_elem_free(priv, virtualchan_msg_pkt);
        return -1;
    }

    /* Copy user data */
    memcpy(virtualchan_msg_pkt->data, data_buff, data_size);
    MR_VIRTUALCHAN_MSG_PACKET_SET_DATA_SIZE(virtualchan_msg_pkt, data_size);

    /* Send message packet */
    ret = mr_virtualchan_upld_elem_send(priv, virtualchan_msg_pkt);
    if (ret < 0) {
        LOG_E("%s Failed to send VIRTUALCHAN upld data to msg_ctrl: %d\r\n", priv->virtualchan_cfg.name, ret);
        mr_virtualchan_upld_elem_free(priv, virtualchan_msg_pkt);
        return -1;
    }

    return 0;
}

/**
 * @brief Initialize virtual channel device instance
 * @param[in] cfg Pointer to virtual channel configuration structure
 * @retval Pointer to virtual channel private structure on success
 * @retval NULL on failure
 */
mr_virtualchan_priv_t *mr_virtualchan_init(mr_virtualchan_cfg_t *cfg)
{
    int ret;
    mr_virtualchan_priv_t *priv;

    /* Parameter validation */
    if (cfg == NULL ||
        cfg->msg_ctrl == NULL ||
        cfg->upld_frame_size <= sizeof(mr_virtualchan_msg_t) ||
        cfg->upld_frame_count < 2 ||
        cfg->upld_frame_buff == NULL ||
        (cfg->dnld_credit_max && cfg->dnld_credit_max <= cfg->upld_credit_update_threshold) ||
        cfg->dnld_credit_max > 127 ||
        cfg->dnld_output_cb == NULL) {
        LOG_E("%s Invalid parameters for VIRTUALCHAN initialization\r\n", cfg->name);
        return NULL;
    }

    /* Allocate private structure */
    priv = malloc(sizeof(mr_virtualchan_priv_t));
    if (!priv) {
        LOG_E("%s Failed to allocate VIRTUALCHAN private structure\r\n", cfg->name);
        return NULL;
    }
    memset(priv, 0, sizeof(mr_virtualchan_priv_t));

    /* Copy configuration */
    priv->virtualchan_cfg = *cfg;

    /* Initialize upload frame buffer pool */
    mr_frame_queue_ctrl_init_cfg_t upld_frame_cfg = {
        .frame_buff = priv->virtualchan_cfg.upld_frame_buff,
        .frame_type = priv->virtualchan_cfg.upld_frame_type,
        .frame_elem_cnt = priv->virtualchan_cfg.upld_frame_count,
        .frame_elem_size = priv->virtualchan_cfg.upld_frame_size,
        .name = priv->virtualchan_cfg.name
    };
    priv->upld_frame_ctrl = mr_frame_queue_create(&upld_frame_cfg);
    if (!priv->upld_frame_ctrl) {
        LOG_E("%s Failed to create VIRTUALCHAN frame queue\r\n", priv->virtualchan_cfg.name);
        goto error_exit;
    }

    /* Create download message queue */
    priv->msg_dnld_queue = xQueueCreate(priv->virtualchan_cfg.msg_ctrl->cfg.dnld_frame_count + 1, sizeof(mr_virtualchan_msg_t *));
    if (!priv->msg_dnld_queue) {
        LOG_E("%s Failed to create msg download queue\r\n", priv->virtualchan_cfg.name);
        goto error_exit;
    }

    /* Register callbacks with msg_ctrl */
    ret = mr_msg_cb_register(priv->virtualchan_cfg.msg_ctrl, priv->virtualchan_cfg.msg_tag,
                             msg_dnld_recv_done_cb, priv,
                             msg_upld_send_done_cb, priv,
                             msg_hw_reset_cb, priv);
    if (ret < 0) {
        LOG_E("%s Failed to register msg callbacks\r\n", priv->virtualchan_cfg.name);
        goto error_exit;
    }

    /* Create daemon task */
    ret = xTaskCreate(virtualchan_proc_task, priv->virtualchan_cfg.name, priv->virtualchan_cfg.task_stack_size, priv,
                      priv->virtualchan_cfg.task_priority, &priv->virtualchan_proc_task);
    if (ret != pdPASS) {
        LOG_E("%s Failed to create VIRTUALCHAN processing task\r\n", priv->virtualchan_cfg.name);
        goto error_exit;
    }

    LOG_I("%s VIRTUALCHAN device initialized successfully\r\n", priv->virtualchan_cfg.name);
    return priv;

error_exit:
    if (priv->msg_dnld_queue) {
        vQueueDelete(priv->msg_dnld_queue);
    }
    if (priv->upld_frame_ctrl) {
        mr_frame_queue_remove(priv->upld_frame_ctrl);
    }
    if (priv->virtualchan_cfg.msg_ctrl) {
        mr_msg_cb_unregister(priv->virtualchan_cfg.msg_ctrl, priv->virtualchan_cfg.msg_tag);
    }
    free(priv);
    LOG_E("%s device initialization failed\r\n", cfg->name);
    return NULL;
}
