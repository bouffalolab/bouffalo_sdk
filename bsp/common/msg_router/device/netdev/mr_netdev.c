/**
 * @file mr_netdev.c
 * @brief Network Device Implementation for SDIO-based Ethernet Communication
 * @details Network device implementation as an application module based on msg_ctrl:
 *          - Receives data from host via download path
 *          - Sends data to host via upload path with flow control
 *          - Manages state machine for handshake protocol
 *          - Provides circular credit-based flow control
 *          - Handles link state transitions (up/down)
 *          - Supports MAC/IP address updates
 */

#include "bflb_core.h"
#include "bflb_mtimer.h"
#include "bflb_irq.h"

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_netdev.h"

#define DBG_TAG "NETDEV"
#include "log.h"

#define NETDEV_EVENT_DNLD_DONE          (1 << 0)
#define NETDEV_EVENT_UPLD_CREDIT_UPDATE (1 << 1)
#define NETDEV_EVENT_RESET              (1 << 2)
#define NETDEV_EVENT_EXIT               (1 << 3)
#define NETDEV_EVENT_LINK_DOWN          (1 << 4)
#define NETDEV_EVENT_LINK_UP            (1 << 5)
#define NETDEV_EVENT_IP_MAC_UPDATE      (1 << 6)

/**
 * @brief Wake up the network device processing task
 * @param[in] priv Pointer to network device private structure
 * @param[in] event Event flags to set
 * @retval 0 Success
 * @retval <0 Error
 */
static int netdev_proc_task_wakeup(mr_netdev_priv_t *priv, uint32_t event)
{
    if (priv->netdev_proc_task == NULL) {
        LOG_E("%s netdev_proc_task is NULL, please init msg_ctrl first!\r\n", priv->netdev_cfg.name);
        return -1;
    }

    /* Wake up message processing task */
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(priv->netdev_proc_task, event, eSetBits, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        xTaskNotify(priv->netdev_proc_task, event, eSetBits);
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
 * @param[in] arg Pointer to EMAC private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_dnld_recv_done_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    int ret;
    mr_netdev_priv_t *priv = (mr_netdev_priv_t *)arg;

    if (!priv || !frame_elem) {
        return -1;
    }

    mr_netdev_msg_t *netdev_msg_packt = MR_NETDEV_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);
    ret = mr_frame_queue_send(priv->msg_dnld_queue, &netdev_msg_packt, 0);
    if (ret < 0) {
        LOG_E("%s Failed to send frame to download queue\r\n", priv->netdev_cfg.name);
        return -1;
    }

    netdev_proc_task_wakeup(priv, NETDEV_EVENT_DNLD_DONE);
    return 0;
}

/**
 * @brief MSG upload send done callback, releases data back to pool
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to EMAC private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_done_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    int ret;
    mr_netdev_priv_t *priv = (mr_netdev_priv_t *)arg;

    if (!priv || !frame_elem) {
        return -1;
    }

    mr_netdev_msg_t *netdev_msg_packt = MR_NETDEV_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);
    if (priv->netdev_cfg.upld_done_cb) {
        /* User-defined callback */
        ret = priv->netdev_cfg.upld_done_cb(priv, netdev_msg_packt);
    } else {
        /* Release back to pool */
        ret = mr_netdev_upld_elem_free(priv, netdev_msg_packt);
    }

    return ret;
}

/**
 * @brief Hardware reset callback for message controller
 * @param[in] unused Unused parameter
 * @param[in] priv Pointer to EMAC private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_hw_reset_cb(mr_frame_elem_t *unused, void *arg)
{
    mr_netdev_priv_t *priv = (mr_netdev_priv_t *)arg;

    netdev_proc_task_wakeup(priv, NETDEV_EVENT_RESET);

    LOG_W("Received hw reset request\r\n");

    return 0;
}

/**
 * @brief Send MSG upload control packet
 * @param[in] priv Pointer to EMAC private structure
 * @param[in] eth_flag Ethernet flag to send
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_cmd_packet(mr_netdev_priv_t *priv, uint8_t eth_flag, uint8_t *data, uint16_t data_size)
{
    int ret;
    mr_netdev_msg_t *netdev_msg_packt;

    ret = mr_netdev_upld_elem_alloc(priv, &netdev_msg_packt, 100);
    if (ret < 0) {
        LOG_E("%s Failed to allocate upld msg packet\r\n", priv->netdev_cfg.name);
        return -1;
    }

    netdev_msg_packt->flag = eth_flag;

    if (data && data_size > 0) {
        memcpy(netdev_msg_packt->data, data, data_size);
        MR_NETDEV_MSG_PACKET_SET_DATA_SIZE(netdev_msg_packt, data_size);
    } else {
        MR_NETDEV_MSG_PACKET_SET_DATA_SIZE(netdev_msg_packt, 0);
    }

    /* send netdev_msg_packt */
    ret = mr_netdev_upld_elem_send(priv, netdev_msg_packt);
    if (ret < 0) {
        LOG_E("%s Failed to send NETDEV control message\r\n", priv->netdev_cfg.name);
        mr_netdev_upld_elem_free(priv, netdev_msg_packt);
        return -1;
    }

    return 0;
}

/*****************************************************************************
 * Network Device Daemon Thread
 *****************************************************************************/

/**
 * @brief Network device daemon thread
 * @details Main tasks:
 *   1. Implement state machine (IDLE → HOST_READY → DEVICE_RUN)
 *   2. Process download packets from host
 *   3. Send control messages (DEVICE_START, DEVICE_STOP, credit updates)
 *   4. Monitor link state changes and trigger transitions
 *   5. Handle MAC/IP update requests
 *   6. Manage proactive credit updates based on threshold
 * @param[in] arg Pointer to network device private structure
 */
static void netdev_proc_task(void *arg)
{
    int ret;
    mr_netdev_priv_t *priv = (mr_netdev_priv_t *)arg;

    mr_netdev_msg_t *netdev_msg_packt = NULL;
    uint32_t notified_value = 0;
    uint32_t notified_mask = 0;

    LOG_I("%s daemon thread started\r\n", priv->netdev_cfg.name);

    /* 初始状态 */
    priv->netdev_status = MR_NETDEV_DSTA_IDLE;
    priv->netdev_cfg.link_up_flag = false;

    while (!priv->stop_requested) {
        /* Wait for notification or timeout */
        uint32_t notified_new = 0;
        if (notified_value) {
            xTaskNotifyWait(0, 0xffffffff, &notified_new, pdMS_TO_TICKS(0));
        } else {
            xTaskNotifyWait(0, 0xffffffff, &notified_new, pdMS_TO_TICKS(priv->netdev_cfg.task_period_max_ms));
        }
        notified_value |= notified_new;

        /* task callback */
        if (priv->netdev_cfg.netdev_task_cb) {
            priv->netdev_cfg.netdev_task_cb(priv, &notified_value);
        }

        /* link up event */
        notified_mask = NETDEV_EVENT_LINK_UP;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;
            LOG_I("%s processing link up event\r\n", priv->netdev_cfg.name);

            priv->netdev_cfg.link_up_flag = true;
        }
        /* link down event */
        notified_mask = NETDEV_EVENT_LINK_DOWN;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;
            LOG_I("%s processing link down event\r\n", priv->netdev_cfg.name);

            priv->netdev_cfg.link_up_flag = false;
        }
        /* reset event */
        notified_mask = NETDEV_EVENT_RESET;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;
            LOG_W("%s processing reset request \r\n", priv->netdev_cfg.name);

            priv->netdev_status = MR_NETDEV_DSTA_IDLE;
        }

        /* dnld_done event */
        notified_mask = NETDEV_EVENT_DNLD_DONE;
        if (notified_value & notified_mask) {
            /* get message from queue (non-blocking) */
            ret = mr_frame_queue_receive(priv->msg_dnld_queue, &netdev_msg_packt, 0);
            if (ret < 0) {
                /* queue empty */
                notified_value &= ~notified_mask;
                netdev_msg_packt = NULL;
            }
        }

        /* Handle reset/stop commands - reset state machine */
        if (netdev_msg_packt && (netdev_msg_packt->flag == MR_NETDEV_FLAG_HOST_RESET ||
                                 netdev_msg_packt->flag == MR_NETDEV_FLAG_HOST_STOP)) {
            LOG_W("%s received host %s command\r\n", priv->netdev_cfg.name,
                  netdev_msg_packt->flag == MR_NETDEV_FLAG_HOST_RESET ? "RESET" : "STOP");

            priv->netdev_status = MR_NETDEV_DSTA_IDLE;
            priv->device_dnld_credit_limit = priv->netdev_cfg.dnld_credit_max;
            priv->credit_limit_update_last = 0;

            mr_netdev_dnld_elem_free(priv, netdev_msg_packt);
            netdev_msg_packt = NULL;
            continue;
        }

        /* State machine processing */
        switch (priv->netdev_status) {
            case MR_NETDEV_DSTA_IDLE:
                /* Wait for HOST_READY message */
                if (netdev_msg_packt == NULL || netdev_msg_packt->flag != MR_NETDEV_FLAG_HOST_READY) {
                    break;
                }
                LOG_I("%s received HOST_READY message\r\n", priv->netdev_cfg.name);

                priv->netdev_status = MR_NETDEV_DSTA_HOST_READY;
                /* Fall through to immediately send HOST_READY */

            case MR_NETDEV_DSTA_HOST_READY:
                /* Host ready, send DEVICE_START with initial credit */
                if (priv->netdev_cfg.link_up_flag == false) {
                    break;
                }
                LOG_I("%s link up, sending DEVICE_START\r\n", priv->netdev_cfg.name);

                uint8_t cmd_data[6 + 4];
                memcpy(&cmd_data[0], priv->netdev_cfg.mac, 6);
                memcpy(&cmd_data[6], priv->netdev_cfg.ip, 4);

                priv->device_dnld_credit_limit = priv->netdev_cfg.dnld_credit_max;
                priv->credit_limit_update_last = 0;
                ret = msg_upld_send_cmd_packet(priv, MR_NETDEV_FLAG_DEVICE_START, cmd_data, sizeof(cmd_data));
                if (ret < 0) {
                    LOG_E("%s Failed to send DEVICE_START message\r\n", priv->netdev_cfg.name);
                    break;
                }

                priv->netdev_status = MR_NETDEV_DSTA_DEVICE_RUN;
                break;

            case MR_NETDEV_DSTA_DEVICE_RUN:
                /* Check for link down */
                if (priv->netdev_cfg.link_up_flag == false) {
                    LOG_I("%s link down, sending DEVICE_STOP\r\n", priv->netdev_cfg.name);
                    ret = msg_upld_send_cmd_packet(priv, MR_NETDEV_FLAG_DEVICE_STOP, NULL, 0);
                    if (ret < 0) {
                        LOG_E("%s Failed to send DEVICE_STOP message\r\n", priv->netdev_cfg.name);
                    }
                    priv->netdev_status = MR_NETDEV_DSTA_IDLE;
                }

                if (netdev_msg_packt == NULL) {
                    break;
                } else if (netdev_msg_packt->flag == MR_NETDEV_FLAG_DNLD_DATA) {
                    /* Download data packet - pass to user callback */
                    ret = priv->netdev_cfg.dnld_output_cb(priv, netdev_msg_packt);
                    if (ret < 0) {
                        LOG_E("%s Failed to output dnld data\r\n", priv->netdev_cfg.name);
                        break;
                    }
                    netdev_msg_packt = NULL;
                    break;
                }

                break;

            default:
                priv->netdev_status = MR_NETDEV_DSTA_IDLE;
                LOG_E("%s Invalid NETDEV status: %d\r\n", priv->netdev_cfg.name, priv->netdev_status);
                break;
        }

        /* Release received packet if not handled by callback */
        if (netdev_msg_packt) {
            mr_netdev_dnld_elem_free(priv, netdev_msg_packt);
            netdev_msg_packt = NULL;
        }

        /* MAC/IP update event */
        notified_mask = NETDEV_EVENT_IP_MAC_UPDATE;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;
            LOG_I("%s Processing MAC/IP update event for NETDEV %s\r\n", priv->netdev_cfg.name);

            uint8_t cmd_data[6 + 4];
            memcpy(&cmd_data[0], priv->netdev_cfg.mac, 6);
            memcpy(&cmd_data[6], priv->netdev_cfg.ip, 4);

            ret = msg_upld_send_cmd_packet(priv, MR_NETDEV_FLAG_MAC_IP_UPDATE, cmd_data, sizeof(cmd_data));
            if (ret < 0) {
                LOG_E("%s Failed to send MAC/IP update message\r\n", priv->netdev_cfg.name);
            }
        }

        /* Proactively send credit update if threshold exceeded */
        notified_mask = NETDEV_EVENT_UPLD_CREDIT_UPDATE;
        if (notified_value & notified_mask) {
            notified_value &= ~notified_mask;

            if (priv->netdev_cfg.dnld_credit_max && priv->netdev_status == MR_NETDEV_DSTA_DEVICE_RUN &&
                (int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) >
                    priv->netdev_cfg.upld_credit_update_threshold) {
                ret = msg_upld_send_cmd_packet(priv, MR_NETDEV_FLAG_CREDIT_UPDATE, NULL, 0);
                if (ret < 0) {
                    LOG_E("%s Failed to send CREDIT_UPDATE message\r\n", priv->netdev_cfg.name);
                }
            }
        }

        /* unknown event */
        notified_mask = ~(NETDEV_EVENT_DNLD_DONE);
        if (notified_value & notified_mask) {
            LOG_W("%s unknown event: 0x%08x\r\n", priv->netdev_cfg.name, (notified_value & notified_mask));
            notified_value &= ~notified_mask;
        }
    }

    LOG_I("%s Ethernet EMAC daemon thread exiting\r\n", priv->netdev_cfg.name);
    vTaskDelete(NULL);
}

/*****************************************************************************
 * Public API Implementation
 * @note These functions can be called by upper layer applications
 *****************************************************************************/

int mr_netdev_upld_elem_alloc(mr_netdev_priv_t *priv, mr_netdev_msg_t **netdev_msg_packt, uint32_t timeout)
{
    int ret;
    mr_frame_elem_t *frame_elem;

    ret = mr_frame_queue_alloc_elem(priv->upld_frame_ctrl, &frame_elem, timeout);
    if (ret < 0) {
        /* No available buffer, try later */
        *netdev_msg_packt = NULL;
        return -1;
    }

    *netdev_msg_packt = MR_NETDEV_FRAME_ELEM_TO_MSG_PACKET_ADDR(frame_elem);

    MR_NETDEV_MSG_PACKET_SET_DATA_SIZE((*netdev_msg_packt), 0);
    (*netdev_msg_packt)->msg_packt.tag = priv->netdev_cfg.msg_tag;
    (*netdev_msg_packt)->msg_packt.sub_tag = 0;
    (*netdev_msg_packt)->flag = MR_NETDEV_FLAG_UPLD_DATA; /* Upload data flag */
    (*netdev_msg_packt)->credit_update_flag = false;
    (*netdev_msg_packt)->credit_limit_cnt = 0;

    return 0;
}

int mr_netdev_upld_elem_send(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt)
{
    int ret;
    uintptr_t flag;
    mr_frame_elem_t *frame_elem = MR_NETDEV_MSG_PACKET_TO_FRAME_ELEM_ADDR(netdev_msg_packt);

    if (frame_elem->data_size > frame_elem->buff_size) {
        LOG_E("%s upld data size too large: %d > %d\r\n", priv->netdev_cfg.name, frame_elem->data_size,
              frame_elem->buff_size);
    }

    /* Lock protection to prevent race conditions with flow control updates
         * and ensure credit values are sent in order */
    flag = bflb_irq_save();

    if ((priv->netdev_cfg.dnld_credit_max != 0) && (priv->credit_limit_update_last != priv->device_dnld_credit_limit) &&
        (netdev_msg_packt->flag == MR_NETDEV_FLAG_UPLD_DATA || netdev_msg_packt->flag == MR_NETDEV_FLAG_CREDIT_UPDATE ||
         netdev_msg_packt->flag == MR_NETDEV_FLAG_DEVICE_START)) {
        /* Send through msg ctrl */
        netdev_msg_packt->credit_update_flag = true;
        netdev_msg_packt->credit_limit_cnt = priv->device_dnld_credit_limit;
        priv->credit_limit_update_last = priv->device_dnld_credit_limit;

        ret = mr_msg_ctrl_upld_send(priv->netdev_cfg.msg_ctrl, frame_elem);

        bflb_irq_restore(flag);

    } else {
        bflb_irq_restore(flag);

        netdev_msg_packt->credit_update_flag = false;
        netdev_msg_packt->credit_limit_cnt = 0;
        ret = mr_msg_ctrl_upld_send(priv->netdev_cfg.msg_ctrl, frame_elem);
    }

    if (ret < 0) {
        LOG_E("%s Failed to send NETDEV upld data to msg_ctrl: %d\r\n", priv->netdev_cfg.name, ret);
        return -1;
    }

    return 0;
}

int mr_netdev_upld_elem_free(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt)
{
    int ret;

    mr_frame_elem_t *frame_elem = MR_NETDEV_MSG_PACKET_TO_FRAME_ELEM_ADDR(netdev_msg_packt);
    ret = mr_frame_queue_free_elem(frame_elem);
    if (ret < 0) {
        LOG_E("%s Failed to free upld frame element\r\n", priv->netdev_cfg.name);
        return ret;
    }

    return 0;
}

int mr_netdev_dnld_elem_free(mr_netdev_priv_t *priv, mr_netdev_msg_t *netdev_msg_packt)
{
    int ret;
    uint8_t packt_flag;

    if (!priv || !netdev_msg_packt) {
        return -1;
    }

    packt_flag = netdev_msg_packt->flag;

    /* Free frame element */
    mr_frame_elem_t *frame_elem = MR_NETDEV_MSG_PACKET_TO_FRAME_ELEM_ADDR(netdev_msg_packt);
    // ret = mr_frame_queue_free_elem(frame_elem);
    ret = mr_msg_ctrl_dnld_free(priv->netdev_cfg.msg_ctrl, frame_elem);
    if (ret < 0) {
        LOG_E("%s Failed to free upld frame element\r\n", priv->netdev_cfg.name);
        return ret;
    }

    /* Update flow control credit counter */
    if (priv->netdev_cfg.dnld_credit_max && packt_flag == MR_NETDEV_FLAG_DNLD_DATA) {
        /* Lock protection for atomic credit update */
        uintptr_t flag = bflb_irq_save();
        bool wakeup_needed = false;
        priv->device_dnld_credit_limit += 1; /* Circular increment (wraps at 256) */
        if ((int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) >=
            priv->netdev_cfg.upld_credit_update_threshold) {
            wakeup_needed = true;
        }
        /* Unlock */
        bflb_irq_restore(flag);

        /* Wake up processing task to send credit update if threshold reached */
        if (wakeup_needed) {
            netdev_proc_task_wakeup(priv, NETDEV_EVENT_UPLD_CREDIT_UPDATE);
        }
    }

    return 0;
}

int mr_netdev_mac_ip_update(mr_netdev_priv_t *priv, uint8_t *mac, uint8_t *ip)
{
    uintptr_t flag;

    if (!priv || (!mac && !ip)) {
        return -1;
    }

    flag = bflb_irq_save();
    if (mac) {
        memcpy(priv->netdev_cfg.mac, mac, 6);
    }
    if (ip) {
        memcpy(priv->netdev_cfg.ip, ip, 4);
    }
    bflb_irq_restore(flag);

    netdev_proc_task_wakeup(priv, NETDEV_EVENT_IP_MAC_UPDATE);

    return 0;
}

int mr_netdev_set_link_up(mr_netdev_priv_t *priv, bool link_up)
{
    if (!priv) {
        return -1;
    }

    if (link_up) {
        /* Link went up */
        netdev_proc_task_wakeup(priv, NETDEV_EVENT_LINK_UP);
    } else {
        /* Link went down */
        netdev_proc_task_wakeup(priv, NETDEV_EVENT_LINK_DOWN);
    }

    return 0;
}

mr_netdev_priv_t *mr_netdev_init(mr_netdev_cfg_t *cfg)
{
    int ret;
    mr_netdev_priv_t *priv;

    /* Parameter validation */
    if (cfg == NULL || cfg->msg_ctrl == NULL || cfg->upld_frame_size <= sizeof(mr_netdev_msg_t) ||
        cfg->upld_frame_count < 2 || cfg->upld_frame_buff == NULL ||
        (cfg->dnld_credit_max && cfg->dnld_credit_max <= cfg->upld_credit_update_threshold) ||
        cfg->dnld_credit_max > 127 || cfg->dnld_output_cb == NULL) {
        LOG_E("%s Invalid parameters for NETDEV initialization\r\n", cfg->name);
        return NULL;
    }

    /* Allocate private structure */
    priv = malloc(sizeof(mr_netdev_priv_t));
    if (!priv) {
        LOG_E("%s Failed to allocate NETDEV private structure\r\n", cfg->name);
        return NULL;
    }
    memset(priv, 0, sizeof(mr_netdev_priv_t));

    /* Copy configuration */
    priv->netdev_cfg = *cfg;

    /* Initialize upload frame buffer pool */
    mr_frame_queue_ctrl_init_cfg_t upld_frame_cfg = { .frame_buff = priv->netdev_cfg.upld_frame_buff,
                                                   .frame_type = priv->netdev_cfg.upld_frame_type,
                                                   .frame_elem_cnt = priv->netdev_cfg.upld_frame_count,
                                                   .frame_elem_size = priv->netdev_cfg.upld_frame_size,
                                                   .name = priv->netdev_cfg.name };
    priv->upld_frame_ctrl = mr_frame_queue_create(&upld_frame_cfg);
    if (!priv->upld_frame_ctrl) {
        LOG_E("%s Failed to create NETDEV frame queue\r\n", priv->netdev_cfg.name);
        goto error_exit;
    }

    /* Create download message queue */
    priv->msg_dnld_queue = xQueueCreate(priv->netdev_cfg.msg_ctrl->cfg.dnld_frame_count + 1, sizeof(mr_netdev_msg_t *));
    if (!priv->msg_dnld_queue) {
        LOG_E("%s Failed to create message download queue\r\n", priv->netdev_cfg.name);
        goto error_exit;
    }

    /* Register message controller callbacks */
    ret = mr_msg_cb_register(priv->netdev_cfg.msg_ctrl, priv->netdev_cfg.msg_tag, msg_dnld_recv_done_cb, priv,
                             msg_upld_send_done_cb, priv, msg_hw_reset_cb, priv);
    if (ret < 0) {
        LOG_E("%s Failed to register NETDEV message callbacks\r\n", priv->netdev_cfg.name);
        goto error_exit;
    }

    /* Create daemon thread */
    ret = xTaskCreate(netdev_proc_task, priv->netdev_cfg.name, priv->netdev_cfg.task_stack_size, priv,
                      priv->netdev_cfg.task_priority, &priv->netdev_proc_task);
    if (ret != pdPASS) {
        LOG_E("%s Failed to create NETDEV processing task\r\n", priv->netdev_cfg.name);
        goto error_exit;
    }

    LOG_I("%s device initialized successfully, tag=%d\r\n", priv->netdev_cfg.name, priv->netdev_cfg.msg_tag);
    return priv;

error_exit:

    if (priv->msg_dnld_queue) {
        vQueueDelete(priv->msg_dnld_queue);
    }
    if (priv->upld_frame_ctrl) {
        mr_frame_queue_remove(priv->upld_frame_ctrl);
    }
    if (priv->netdev_cfg.msg_ctrl) {
        mr_msg_cb_unregister(priv->netdev_cfg.msg_ctrl, priv->netdev_cfg.msg_tag);
    }
    free(priv);
    LOG_I("%s device initialized failed\r\n", cfg->name);
    return NULL;
}
