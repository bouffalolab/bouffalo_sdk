/**
 * @file mr_tty.c
 * @brief TTY Device Implementation for SDIO-based Multi-channel Communication
 * @details TTY implementation as an application module based on msg_ctrl:
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
#include "mr_tty.h"

#define DBG_TAG "TTY"
#include "log.h"

#define TTY_EVENT_DNLD_DONE          (1 << 0)
#define TTY_EVENT_UPLD_CREDIT_UPDATE (1 << 1)
#define TTY_EVENT_RESET              (1 << 2)
#define TTY_EVENT_EXIT               (1 << 3)

/**
 * @brief Wake up the TTY processing task
 * @param[in] priv Pointer to TTY private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int tty_proc_task_wakeup(mr_tty_priv_t *priv, uint32_t event)
{
    if (priv->tty_proc_task == NULL) {
        LOG_E("%s tty_proc_task is NULL, please init msg_ctrl first!\r\n", priv->tty_cfg.name);
        return -1;
    }

    /* Wake up message processing task */
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(priv->tty_proc_task, event, eSetBits, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        // xTaskNotifyGive(priv->tty_proc_task);
        xTaskNotify(priv->tty_proc_task, event, eSetBits);
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
 * @param[in] arg Pointer to TTY private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_dnld_recv_done_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    int ret;
    mr_tty_priv_t *priv = (mr_tty_priv_t *)arg;

    if (!priv || !frame_elem) {
        return -1;
    }

    mr_tty_msg_t *tty_msg_pkt = MR_TTY_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);
    ret = mr_frame_queue_send(priv->msg_dnld_queue, &tty_msg_pkt, 0);
    if (ret < 0) {
        LOG_E("%s Failed to send frame to download queue\r\n", priv->tty_cfg.name);
        return -1;
    }

    tty_proc_task_wakeup(priv, TTY_EVENT_DNLD_DONE);
    return 0;
}

/**
 * @brief MSG upload send done callback, releases data back to pool
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to TTY private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_done_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    int ret;
    mr_tty_priv_t *priv = (mr_tty_priv_t *)arg;

    if (!priv || !frame_elem) {
        return -1;
    }

    mr_tty_msg_t *tty_msg_pkt = MR_TTY_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);
    if (tty_msg_pkt->flag == MR_TTY_FLAG_UPLD_DATA && priv->tty_cfg.upld_done_cb) {
        /* User-defined callback */
        ret = priv->tty_cfg.upld_done_cb(priv, tty_msg_pkt);
    } else {
        /* Release back to pool */
        ret = mr_tty_upld_elem_free(priv, tty_msg_pkt);
    }

    return ret;
}

/**
 * @brief Hardware reset callback for message controller
 * @param[in] unused Unused parameter
 * @param[in] priv Pointer to TTY private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_hw_reset_cb(mr_frame_elem_t *unused, void *arg)
{
    mr_tty_priv_t *priv = (mr_tty_priv_t *)arg;

    tty_proc_task_wakeup(priv, TTY_EVENT_RESET);

    LOG_W("Received hw reset request\r\n");

    return 0;
}

/**
 * @brief Send MSG upload control packet
 * @param[in] priv Pointer to TTY private structure
 * @param[in] tty_flag TTY flag to send (DEVICE_START, CREDIT_UPDATE, etc.)
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_cmd_packet(mr_tty_priv_t *priv, uint8_t tty_flag, uint8_t *data, uint16_t data_size)
{
    int ret;
    mr_tty_msg_t *tty_msg_pkt;

    ret = mr_tty_upld_elem_alloc(priv, &tty_msg_pkt, 100);
    if (ret < 0) {
        LOG_E("%s Failed to allocate upld msg packet\r\n", priv->tty_cfg.name);
        return -1;
    }

    tty_msg_pkt->flag = tty_flag;

    if (data && data_size > 0) {
        memcpy(tty_msg_pkt->data, data, data_size);
        MR_TTY_MSG_PACKET_SET_DATA_SIZE(tty_msg_pkt, data_size);
    } else {
        MR_TTY_MSG_PACKET_SET_DATA_SIZE(tty_msg_pkt, 0);
    }

    /* send tty_msg_pkt */
    ret = mr_tty_upld_elem_send(priv, tty_msg_pkt);
    if (ret < 0) {
        LOG_E("%s Failed to send TTY control message\r\n", priv->tty_cfg.name);
        mr_tty_upld_elem_free(priv, tty_msg_pkt);
        return -1;
    }

    return 0;
}

/**
 * @brief TTY processing task (daemon thread)
 * @details Main tasks:
 *   1. Process received messages from host (dnld path)
 *   2. Handle state machine transitions (handshake protocol)
 *   3. Monitor and report flow control credits
 *   4. Distribute messages based on packet flags
 * @param[in] arg Pointer to TTY private structure
 */
static void tty_proc_task(void *arg)
{
    int ret;
    mr_tty_priv_t *priv = (mr_tty_priv_t *)arg;

    mr_tty_msg_t *tty_msg_pkt = NULL;
    uint32_t notified_value = 0;
    uint32_t notified_mask = 0;

    LOG_I("%s daemon thread started\r\n", priv->tty_cfg.name);

    /* Initialize state machine */
    priv->tty_status = MR_TTY_DSTA_IDLE;
    priv->tty_ready = false;

    while (!priv->stop_requested) {
        /* Wait for notification or timeout */
        uint32_t notified_new = 0;
        if (notified_value) {
            xTaskNotifyWait(0, 0xffffffff, &notified_new, pdMS_TO_TICKS(0));
        } else {
            xTaskNotifyWait(0, 0xffffffff, &notified_new, pdMS_TO_TICKS(priv->tty_cfg.task_period_max_ms));
        }
        notified_value |= notified_new;

        /* task callback */
        if (priv->tty_cfg.tty_task_cb) {
            priv->tty_cfg.tty_task_cb(priv, &notified_value);
        }

        /* reset event */
        notified_mask = TTY_EVENT_RESET;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;
            LOG_W("%s processing reset request\r\n", priv->tty_cfg.name);

            priv->tty_ready = false;
            priv->tty_status = MR_TTY_DSTA_IDLE;
        }

        /* get message from queue (non-blocking) */
        notified_mask = TTY_EVENT_DNLD_DONE;
        if (notified_value & notified_mask) {
            ret = mr_frame_queue_receive(priv->msg_dnld_queue, &tty_msg_pkt, 0);
            if (ret < 0) {
                /* queue empty */
                notified_value &= ~notified_mask;
                tty_msg_pkt = NULL;
            }
        }

        /* Handle reset/stop commands - reset state machine */
        if (tty_msg_pkt &&
            (tty_msg_pkt->flag == MR_TTY_FLAG_HOST_RESET || tty_msg_pkt->flag == MR_TTY_FLAG_HOST_STOP)) {
            LOG_W("%s received host %s command\r\n", priv->tty_cfg.name,
                  tty_msg_pkt->flag == MR_TTY_FLAG_HOST_RESET ? "RESET" : "STOP");

            priv->tty_status = MR_TTY_DSTA_IDLE;
            priv->tty_ready = false;
            priv->device_dnld_credit_limit = priv->tty_cfg.dnld_credit_max;
            priv->credit_limit_update_last = 0;

            mr_tty_dnld_elem_free(priv, tty_msg_pkt);
            tty_msg_pkt = NULL;
            continue;
        }

        /* State machine processing */
        switch (priv->tty_status) {
            case MR_TTY_DSTA_IDLE:
                /* Wait for HOST_READY message */
                if (tty_msg_pkt == NULL || tty_msg_pkt->flag != MR_TTY_FLAG_HOST_READY) {
                    break;
                }
                LOG_I("%s received HOST_READY message\r\n", priv->tty_cfg.name);

                priv->tty_status = MR_TTY_DSTA_HOST_READY;
                /* Fall through to immediately send DEVICE_START */

            case MR_TTY_DSTA_HOST_READY:
                /* Host is ready, send DEVICE_START with initial credit */
                priv->device_dnld_credit_limit = priv->tty_cfg.dnld_credit_max;
                priv->credit_limit_update_last = 0;

                ret = msg_upld_send_cmd_packet(priv, MR_TTY_FLAG_DEVICE_START, NULL, 0);
                if (ret < 0) {
                    LOG_E("%s Failed to send DEVICE_START command\r\n", priv->tty_cfg.name);
                    break;
                }

                priv->tty_status = MR_TTY_DSTA_DEVICE_RUN;
                break;

            case MR_TTY_DSTA_DEVICE_RUN:

                if (tty_msg_pkt == NULL) {
                    break;

                } else if (tty_msg_pkt->flag == MR_TTY_FLAG_DNLD_DATA) {
                    /* Download data packet - pass to user callback */
                    ret = priv->tty_cfg.dnld_output_cb(priv, tty_msg_pkt);
                    if (ret < 0) {
                        LOG_E("%s Failed to output dnld data\r\n", priv->tty_cfg.name);
                        break;
                    }
                    tty_msg_pkt = NULL;
                    break;

                } else if (tty_msg_pkt->flag == MR_TTY_FLAG_OPEN) {
                    LOG_I("%s received tty OPEN message\r\n", priv->tty_cfg.name);
                    if (priv->tty_ready == false) {
                        priv->tty_ready = true;
                        if (priv->tty_cfg.tty_event) {
                            priv->tty_cfg.tty_event(priv, MR_TTY_FLAG_OPEN);
                        }
                    } else {
                        LOG_W("%s already ready, ignoring OPEN message\r\n", priv->tty_cfg.name);
                    }

                } else if (tty_msg_pkt->flag == MR_TTY_FLAG_CLOSE) {
                    LOG_I("%s received tty CLOSE message\r\n", priv->tty_cfg.name);
                    if (priv->tty_ready == true) {
                        priv->tty_ready = false;
                        if (priv->tty_cfg.tty_event) {
                            priv->tty_cfg.tty_event(priv, MR_TTY_FLAG_CLOSE);
                        }
                    } else {
                        LOG_W("%s already closed, ignoring CLOSE message\r\n", priv->tty_cfg.name);
                    }

                } else if (tty_msg_pkt->flag == MR_TTY_FLAG_SET_TERMIOS) {
                    LOG_I("%s received tty SET_TERMIOS message\r\n", priv->tty_cfg.name);
                    if (priv->tty_cfg.tty_event) {
                        priv->tty_cfg.tty_event(priv, MR_TTY_FLAG_SET_TERMIOS);
                    }

                } else if (tty_msg_pkt->flag == MR_TTY_FLAG_BREAK) {
                    LOG_I("%s received tty BREAK message\r\n", priv->tty_cfg.name);
                    if (priv->tty_cfg.tty_event) {
                        priv->tty_cfg.tty_event(priv, MR_TTY_FLAG_BREAK);
                    }

                } else if (tty_msg_pkt->flag == MR_TTY_FLAG_HANGUP) {
                    LOG_I("%s received tty HANGUP message\r\n", priv->tty_cfg.name);
                    if (priv->tty_cfg.tty_event) {
                        priv->tty_cfg.tty_event(priv, MR_TTY_FLAG_HANGUP);
                    }
                }

                break;

            default:
                priv->tty_status = MR_TTY_DSTA_IDLE;
                LOG_E("%s Invalid TTY status: %d\r\n", priv->tty_cfg.name, priv->tty_status);
                break;
        }

        /* Release received packet if not handled by callback */
        if (tty_msg_pkt) {
            mr_tty_dnld_elem_free(priv, tty_msg_pkt);
            tty_msg_pkt = NULL;
        }

        /* Proactively send credit update if threshold exceeded */
        notified_mask = TTY_EVENT_UPLD_CREDIT_UPDATE;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;

            if (priv->tty_cfg.dnld_credit_max && priv->tty_status == MR_TTY_DSTA_DEVICE_RUN && priv->tty_ready &&
                (int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) >
                    priv->tty_cfg.upld_credit_update_threshold) {
                ret = msg_upld_send_cmd_packet(priv, MR_TTY_FLAG_CREDIT_UPDATE, NULL, 0);
                if (ret < 0) {
                    LOG_E("%s Failed to send CREDIT_UPDATE, message\r\n", priv->tty_cfg.name);
                }
            }
        }

        /* unknown event */
        notified_mask = ~(TTY_EVENT_DNLD_DONE);
        if (notified_value & notified_mask) {
            LOG_W("%s unknown event: 0x%08x\r\n", priv->tty_cfg.name, (notified_value & notified_mask));
            notified_value &= ~notified_mask;
        }
    }

    LOG_I("%s TTY daemon thread exiting\r\n", priv->tty_cfg.name);
    vTaskDelete(NULL);
}

/*****************************************************************************
 * Public API Implementation
 * @note These functions can be called by upper layer applications
 *****************************************************************************/

/**
 * @brief Allocate upload message packet buffer from pool
 * @param[in] priv Pointer to TTY private structure
 * @param[out] tty_msg_pkt Pointer to store allocated message packet
 * @param[in] timeout Timeout in ticks to wait for buffer
 * @retval 0 Success
 * @retval <0 No buffer available
 */
int mr_tty_upld_elem_alloc(mr_tty_priv_t *priv, mr_tty_msg_t **tty_msg_pkt, uint32_t timeout)
{
    int ret;
    mr_frame_elem_t *frame_elem;

    ret = mr_frame_queue_alloc_elem(priv->upld_frame_ctrl, &frame_elem, timeout);
    if (ret < 0) {
        /* No available buffer, try later */
        *tty_msg_pkt = NULL;
        return -1;
    }

    *tty_msg_pkt = MR_TTY_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);

    MR_TTY_MSG_PACKET_SET_DATA_SIZE((*tty_msg_pkt), 0);
    (*tty_msg_pkt)->msg_pkt.tag = priv->tty_cfg.msg_tag;
    (*tty_msg_pkt)->msg_pkt.sub_tag = 0;
    (*tty_msg_pkt)->flag = MR_TTY_FLAG_UPLD_DATA; /* Upload data flag */
    (*tty_msg_pkt)->credit_update_flag = false;
    (*tty_msg_pkt)->credit_limit_cnt = 0;

    return 0;
}

/**
 * @brief Send allocated upload message packet
 * @param[in] priv Pointer to TTY private structure
 * @param[in] tty_msg_pkt Message packet (must be allocated by mr_tty_upld_elem_alloc)
 * @retval 0 Success
 * @retval <0 Error
 * @note Automatically attaches flow control credit if configured
 */
int mr_tty_upld_elem_send(mr_tty_priv_t *priv, mr_tty_msg_t *tty_msg_pkt)
{
    int ret;
    uintptr_t flag;
    mr_frame_elem_t *frame_elem = MR_TTY_MSG_PACKET_TO_FRAME_ELEM_ADDR(tty_msg_pkt);

    if (frame_elem->data_size > frame_elem->buff_size) {
        LOG_E("%s upld data size too large: %d > %d\r\n", priv->tty_cfg.name, frame_elem->data_size,
              frame_elem->buff_size);
    }

    /* Lock protection to prevent race conditions with flow control updates
         * and ensure credit values are sent in order */
    flag = bflb_irq_save();

    if ((priv->tty_cfg.dnld_credit_max != 0) &&
        (priv->credit_limit_update_last != priv->device_dnld_credit_limit) &&
        (tty_msg_pkt->flag == MR_TTY_FLAG_UPLD_DATA ||
         tty_msg_pkt->flag == MR_TTY_FLAG_CREDIT_UPDATE ||
         tty_msg_pkt->flag == MR_TTY_FLAG_DEVICE_START)) {
        /* Send through msg ctrl */
        tty_msg_pkt->credit_update_flag = true;
        tty_msg_pkt->credit_limit_cnt = priv->device_dnld_credit_limit;
        priv->credit_limit_update_last = priv->device_dnld_credit_limit;

        ret = mr_msg_ctrl_upld_send(priv->tty_cfg.msg_ctrl, frame_elem);

        bflb_irq_restore(flag);

    } else {
        bflb_irq_restore(flag);

        if (tty_msg_pkt->flag == MR_TTY_FLAG_CREDIT_UPDATE) {
            /* No credit update needed, skip sending CREDIT_UPDATE message */
            mr_tty_upld_elem_free(priv, tty_msg_pkt);
            return 0;
        }

        tty_msg_pkt->credit_update_flag = false;
        tty_msg_pkt->credit_limit_cnt = 0;
        ret = mr_msg_ctrl_upld_send(priv->tty_cfg.msg_ctrl, frame_elem);
    }

    if (ret < 0) {
        LOG_E("%s Failed to send TTY upld data to msg_ctrl: %d\r\n", priv->tty_cfg.name, ret);
        return -1;
    }

    return 0;
}

/**
 * @brief Free upload message packet buffer back to pool
 * @param[in] priv Pointer to TTY private structure
 * @param[in] tty_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Equivalent to mr_frame_queue_free_elem, must be called in upld_done_cb
 */
int mr_tty_upld_elem_free(mr_tty_priv_t *priv, mr_tty_msg_t *tty_msg_pkt)
{
    int ret;

    mr_frame_elem_t *frame_elem = MR_TTY_MSG_PACKET_TO_FRAME_ELEM_ADDR(tty_msg_pkt);
    ret = mr_frame_queue_free_elem(frame_elem);
    if (ret < 0) {
        LOG_E("%s Failed to free upld frame element\r\n", priv->tty_cfg.name);
        return ret;
    }

    return 0;
}

/**
 * @brief Send TTY data for upload
 * @param[in] priv Pointer to TTY private structure
 * @param[in] data_buff Pointer to data buffer to send
 * @param[in] data_size Size of data to send
 * @retval 0 Success
 * @retval <0 Error
 */
int mr_tty_upld_data_send(mr_tty_priv_t *priv, const uint8_t *data_buff, uint16_t data_size)
{
    int ret;
    mr_tty_msg_t *tty_msg_pkt;

    if (!priv || !data_buff || data_size == 0) {
        return -1;
    }

    /* Allocate message packet */
    ret = mr_tty_upld_elem_alloc(priv, &tty_msg_pkt, 100);
    if (ret < 0) {
        return -1; /* No available buffer, try later */
    }

    /* Check buffer size */
    if (data_size > MR_TTY_MSG_PACKET_GET_BUFF_SIZE(tty_msg_pkt)) {
        LOG_E("%s upld data size too large: %d > %d\r\n", priv->tty_cfg.name, data_size,
              MR_TTY_MSG_PACKET_GET_BUFF_SIZE(tty_msg_pkt));
        mr_tty_upld_elem_free(priv, tty_msg_pkt);
        return -1;
    }

    /* Copy user data */
    memcpy(tty_msg_pkt->data, data_buff, data_size);
    MR_TTY_MSG_PACKET_SET_DATA_SIZE(tty_msg_pkt, data_size);

    /* Send message packet */
    ret = mr_tty_upld_elem_send(priv, tty_msg_pkt);
    if (ret < 0) {
        LOG_E("%s Failed to send TTY upld data to msg_ctrl: %d\r\n", priv->tty_cfg.name, ret);
        mr_tty_upld_elem_free(priv, tty_msg_pkt);
        return -1;
    }

    return 0;
}

/**
 * @brief Free download message packet and update flow control
 * @param[in] priv Pointer to TTY private structure
 * @param[in] tty_msg_pkt Message packet to free
 * @retval 0 Success
 * @retval <0 Error
 * @note Must be called after processing download data, automatically updates credit counter
 */
int mr_tty_dnld_elem_free(mr_tty_priv_t *priv, mr_tty_msg_t *tty_msg_pkt)
{
    int ret;

    if (!priv || !tty_msg_pkt) {
        return -1;
    }

    uint8_t pkt_flag = tty_msg_pkt->flag;

    /* Free frame element */
    mr_frame_elem_t *frame_elem = MR_TTY_MSG_PACKET_TO_FRAME_ELEM_ADDR(tty_msg_pkt);
    ret = mr_msg_ctrl_dnld_free(priv->tty_cfg.msg_ctrl, frame_elem);
    if (ret < 0) {
        LOG_E("%s Failed to free dnld frame element\r\n", priv->tty_cfg.name);
        return ret;
    }

    /* Update flow control credit counter */
    if (priv->tty_cfg.dnld_credit_max &&
        priv->tty_status == MR_TTY_DSTA_DEVICE_RUN &&
        pkt_flag == MR_TTY_FLAG_DNLD_DATA) {
        /* Lock protection for atomic credit update */
        uintptr_t flag = bflb_irq_save();
        bool wakeup_needed = false;
        priv->device_dnld_credit_limit += 1; /* Circular increment (wraps at 256) */
        if (priv->device_dnld_credit_limit - priv->credit_limit_update_last >=
            priv->tty_cfg.upld_credit_update_threshold) {
            wakeup_needed = true;
        }
        /* Unlock */
        bflb_irq_restore(flag);

        /* Wake up processing task to send credit update if threshold reached */
        if (wakeup_needed) {
            tty_proc_task_wakeup(priv, TTY_EVENT_UPLD_CREDIT_UPDATE);
        }
    }

    return 0;
}

/**
 * @brief Initialize TTY device instance
 * @param[in] cfg Pointer to TTY configuration structure
 * @retval Pointer to TTY private structure on success
 * @retval NULL on failure
 */
mr_tty_priv_t *mr_tty_init(mr_tty_cfg_t *cfg)
{
    int ret;
    mr_tty_priv_t *priv;

    /* Parameter validation */
    if (cfg == NULL || cfg->msg_ctrl == NULL || cfg->upld_frame_size <= sizeof(mr_tty_msg_t) ||
        cfg->upld_frame_count < 2 || cfg->upld_frame_buff == NULL ||
        (cfg->dnld_credit_max && cfg->dnld_credit_max <= cfg->upld_credit_update_threshold) ||
        cfg->dnld_credit_max > 127 || cfg->dnld_output_cb == NULL) {
        LOG_E("%s Invalid parameters for TTY initialization\r\n", cfg->name);
        return NULL;
    }

    /* Allocate private structure */
    priv = malloc(sizeof(mr_tty_priv_t));
    if (!priv) {
        LOG_E("%s Failed to allocate TTY private structure\r\n", cfg->name);
        return NULL;
    }
    memset(priv, 0, sizeof(mr_tty_priv_t));

    /* Copy configuration */
    priv->tty_cfg = *cfg;

    /* Initialize upload frame buffer pool */
    mr_frame_queue_ctrl_init_cfg_t upld_frame_cfg = {
        .frame_buff = priv->tty_cfg.upld_frame_buff,
        .frame_type = priv->tty_cfg.upld_frame_type,
        .frame_elem_cnt = priv->tty_cfg.upld_frame_count,
        .frame_elem_size = priv->tty_cfg.upld_frame_size,
        .name = priv->tty_cfg.name
    };
    priv->upld_frame_ctrl = mr_frame_queue_create(&upld_frame_cfg);
    if (!priv->upld_frame_ctrl) {
        LOG_E("%s Failed to create TTY frame queue\r\n", priv->tty_cfg.name);
        goto error_exit;
    }

    /* Create download message queue */
    priv->msg_dnld_queue = xQueueCreate(priv->tty_cfg.msg_ctrl->cfg.dnld_frame_count + 1, sizeof(mr_tty_msg_t *));
    if (!priv->msg_dnld_queue) {
        LOG_E("%s Failed to create message download queue\r\n", priv->tty_cfg.name);
        goto error_exit;
    }

    /* Register message controller callbacks */
    ret = mr_msg_cb_register(priv->tty_cfg.msg_ctrl, priv->tty_cfg.msg_tag, msg_dnld_recv_done_cb, priv,
                             msg_upld_send_done_cb, priv, msg_hw_reset_cb, priv);
    if (ret < 0) {
        LOG_E("%s Failed to register TTY message callbacks\r\n", priv->tty_cfg.name);
        goto error_exit;
    }

    /* Create daemon thread */
    ret = xTaskCreate(tty_proc_task, priv->tty_cfg.name, priv->tty_cfg.task_stack_size, priv,
                      priv->tty_cfg.task_priority, &priv->tty_proc_task);
    if (ret != pdPASS) {
        LOG_E("%s Failed to create TTY processing task\r\n", priv->tty_cfg.name);
        goto error_exit;
    }

    LOG_I("%s device initialized successfully, tag=%d\r\n", priv->tty_cfg.name, priv->tty_cfg.msg_tag);
    return priv;

error_exit:
    if (priv->msg_dnld_queue) {
        vQueueDelete(priv->msg_dnld_queue);
    }
    if (priv->upld_frame_ctrl) {
        mr_frame_queue_remove(priv->upld_frame_ctrl);
    }
    if (priv->tty_cfg.msg_ctrl) {
        mr_msg_cb_unregister(priv->tty_cfg.msg_ctrl, priv->tty_cfg.msg_tag);
    }
    free(priv);
    LOG_E("%s device initialized failed\r\n", cfg->name);
    return NULL;
}
