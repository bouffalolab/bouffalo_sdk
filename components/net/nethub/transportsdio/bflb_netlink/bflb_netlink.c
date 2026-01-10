/**
 * @file bflb_netlink.c
 * @brief Netlink Device Implementation
 * @details Netlink implementation: application module based on msg_ctrl,
 *          - Netlink data output: Call registered callback to output received data
 *          - MSG transfer callbacks: msg_dnld_recv_cb/msg_upld_send_cb (handle message transfer completion)
 */

#include "bflb_core.h"
#include "bflb_mtimer.h"

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bflb_msg_ctrl.h"
#include "bflb_frame_buff_ctrl.h"
#include "bflb_netlink.h"
#include "bflb_netlink_protocol.h"

#define DBG_TAG "NETLINK"
#include "log.h"

/*****************************************************************************
 * Global Variables
 *****************************************************************************/

/** Frame buffer memory attribute (aligned and non-cached) */
#define FRAME_BUFFER_ATTR __ALIGNED(32) ATTR_NOCACHE_NOINIT_RAM_SECTION
/** Download frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t netlink_frame_buff[NETLINK_FRAME_CNT][NETLINK_FRAME_SIZE];

/** Global Netlink private structure pointer */
static struct bflb_netlink_priv *g_netlink_priv = NULL;

/*****************************************************************************
 * Forward Declarations
 *****************************************************************************/

static void netlink_proc_task(void *arg);

static int msg_dnld_recv_done_cb(frame_elem_t *frame_elem, void *arg);
static int msg_upld_send_done_cb(frame_elem_t *frame_elem, void *arg);
static int msg_hw_reset_cb(frame_elem_t *unused, void *arg);

/**
 * @brief Wake up the Netlink processing task
 * @param[in] priv Pointer to Netlink private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int netlink_proc_task_wakeup(struct bflb_netlink_priv *priv)
{
    if (priv->netlink_proc_task == NULL) {
        LOG_E("netlink_proc_task is NULL, please init msg_ctrl first!\r\n");
        return -1;
    }

    /* Wake up message processing task */
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(priv->netlink_proc_task, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        xTaskNotifyGive(priv->netlink_proc_task);
    }

    return 0;
}

/**
 * @brief Callback after frame buffer is freed, used to wake up the processing task
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to Netlink private structure
 */
void netlink_free_after_cb(frame_elem_t *frame_elem, void *arg)
{
    netlink_proc_task_wakeup((struct bflb_netlink_priv *)arg);
}

/*****************************************************************************
 * Netlink data processing
 *****************************************************************************/

/**
 * @brief Send command packet to host
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] flag Command flag
 * @retval 0 Success
 * @retval <0 Error code
 */
static int msg_upld_send_cmd_packt(struct bflb_netlink_priv *priv, uint8_t flag)
{
    frame_elem_t *frame_elem;
    struct bflb_netlink_msg_packt *netlink_msg_packt;
    int ret;
    uint16_t total_len;
    uintptr_t irq_flags;

    /* Allocate frame element */
    ret = frame_queue_alloc_elem(priv->rx_frame_ctrl, &frame_elem, portMAX_DELAY);
    if (ret < 0 || frame_elem == NULL) {
        LOG_E("Failed to allocate frame buffer\r\n");
        return -1;
    }

    /* Build message packet */
    total_len = sizeof(struct bflb_netlink_msg_packt);
    netlink_msg_packt = (struct bflb_netlink_msg_packt *)frame_elem->buff_addr;

    frame_elem->data_size = total_len;
    netlink_msg_packt->msg_packt.len = total_len - sizeof(struct bflb_msg_packt);
    netlink_msg_packt->msg_packt.tag = priv->msg_tag;
    netlink_msg_packt->msg_packt.sub_tag = 0;
    netlink_msg_packt->reseved[0] = 0;
    netlink_msg_packt->flag = flag;

#if NETLINK_DNLD_CREDIT_MAX
    /* Add flow control protection to prevent race conditions */
    irq_flags = bflb_irq_save();
    if (flag == BL_NETLINK_FLAG_CREDIT_UPDATE || flag == BL_NETLINK_FLAG_DEVICE_START) {
        if (priv->credit_limit_update_last == priv->device_dnld_credit_limit) {
            netlink_msg_packt->credit_update_flag = false;
            netlink_msg_packt->credit_limit_cnt = priv->device_dnld_credit_limit;
        } else {
            netlink_msg_packt->credit_update_flag = true;
            netlink_msg_packt->credit_limit_cnt = priv->device_dnld_credit_limit;
            priv->credit_limit_update_last = priv->device_dnld_credit_limit;
        }
    } else {
        netlink_msg_packt->credit_update_flag = false;
        netlink_msg_packt->credit_limit_cnt = priv->device_dnld_credit_limit;
    }

    /* Send message */
    ret = bflb_msg_ctrl_send(priv->msg_ctrl, frame_elem, 0);
    bflb_irq_restore(irq_flags);
#else
    netlink_msg_packt->credit_update_flag = 0;
    netlink_msg_packt->credit_limit_cnt = 0;

    /* Send message */
    ret = bflb_msg_ctrl_send(priv->msg_ctrl, frame_elem, 0);
#endif

    if (ret < 0) {
        LOG_E("Failed to send netlink command: %d\r\n", ret);
        frame_queue_free_elem(frame_elem);
        return -1;
    }

    LOG_D("Sent netlink command: flag=%d, credit=%d\r\n", flag, priv->device_dnld_credit_limit);
    return 0;
}

/**
 * @brief Process Netlink data output using registered callback
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] frame_elem Pointer to frame element
 */
static void netlink_dnld_data_output(struct bflb_netlink_priv *priv, frame_elem_t *frame_elem)
{
    struct bflb_netlink_msg_packt *netlink_msg_packt;
    struct bflb_netlink_data_hdr *data_hdr;
    uint16_t data_len;
    uint16_t user_data_len;
    uint8_t *user_data;

    netlink_msg_packt = (struct bflb_netlink_msg_packt *)frame_elem->buff_addr;

    /* Validate msg_packt.len to prevent integer underflow/overflow */
    if (netlink_msg_packt->msg_packt.len < sizeof(struct bflb_netlink_msg_packt)) {
        LOG_E("Invalid msg_packt.len: %u (min %u)\r\n",
              netlink_msg_packt->msg_packt.len,
              (unsigned int)sizeof(struct bflb_netlink_msg_packt));
        frame_queue_free_elem(frame_elem);
        return;
    }

    data_len = netlink_msg_packt->msg_packt.len - sizeof(struct bflb_netlink_msg_packt) +
               sizeof(struct bflb_msg_packt);

    if (data_len == 0) {
        LOG_W("Empty netlink packet received\r\n");
        return;
    }

    /* Parse data type header */
    if (data_len < BFLB_NETLINK_DATA_HDR_LEN) {
        LOG_W("Data too short for header, data_len=%d\r\n", data_len);
        return;
    }

    data_hdr = (struct bflb_netlink_data_hdr *)netlink_msg_packt->data;
    user_data = data_hdr->data;
    user_data_len = data_len - BFLB_NETLINK_DATA_HDR_LEN;

    /* Print type information */
    LOG_D("Received netlink data, type: %s, len: %d, flag: %d\r\n",
          bflb_netlink_data_type_to_str(data_hdr->data_type), user_data_len, netlink_msg_packt->flag);

    /* Call registered callback if available (pass user data, excluding header) */
    if (data_hdr->data_type < BFLB_NETLINK_DATA_TYPE_MAX) {
        if (priv->dnld_cb[data_hdr->data_type]) {
            priv->dnld_cb[data_hdr->data_type](priv->cbpri_arg[data_hdr->data_type], user_data, user_data_len);
        } else {
            LOG_W("No callback registered for data type=%s, dropping user_data_len:%d\r\n",
                  bflb_netlink_data_type_to_str(data_hdr->data_type), user_data_len);
        }
    } else {
        LOG_W("Invalid data_type=%d (>= max %d), dropping user_data_len:%d\r\n",
              data_hdr->data_type, BFLB_NETLINK_DATA_TYPE_MAX, user_data_len);
    }

    /* Free the frame */
    frame_queue_free_elem(frame_elem);

#if NETLINK_DNLD_CREDIT_MAX
    /* Update flow control */
    uint8_t old_credit = priv->device_dnld_credit_limit;
    priv->credit_cnt += 1;
    priv->device_dnld_credit_limit += 1;
    LOG_D("[CREDIT] DNLD processed: credit_limit %u->%u, cnt=%u\n",
          old_credit, priv->device_dnld_credit_limit, priv->credit_cnt);
#endif
}

/**
 * @brief Upload data to host
 * @param[in] priv Pointer to Netlink private structure
 * @param[in] data_buff Data buffer to send
 * @param[in] data_size Data size
 * @retval >0 Number of bytes sent
 * @retval <0 Error code
 */
static int netlink_upld_data_send(struct bflb_netlink_priv *priv, uint8_t data_type, uint8_t *data_buff, uint32_t data_size)
{
    frame_elem_t *frame_elem;
    struct bflb_netlink_msg_packt *netlink_msg_packt;
    struct bflb_netlink_data_hdr *data_hdr;
    int ret;
    uint16_t total_len;
    uintptr_t irq_flags;

    if (data_size == 0 || data_buff == NULL) {
        LOG_E("Invalid parameters\r\n");
        return -1;
    }

    /* Check status */
    if (priv->netlink_status != BL_NETLINK_DSTA_DEVICE_RUN) {
        LOG_W("Cannot send data in current state: %d\r\n", priv->netlink_status);
        return data_size;
    }

    /* Check flow control */
    if (priv->host_upld_credit_limit > 0) {
        if (priv->device_upld_credit_consumed >= priv->host_upld_credit_limit) {
            LOG_D("Upload flow control: waiting for credit\r\n");
            return 0;
        }
    }

    /* Allocate frame element */
    ret = frame_queue_alloc_elem(priv->rx_frame_ctrl, &frame_elem, portMAX_DELAY);
    if (ret < 0 || frame_elem == NULL) {
        LOG_E("Failed to allocate frame buffer\r\n");
        return -1;
    }

    /* Build message packet with data type header */
    total_len = sizeof(struct bflb_netlink_msg_packt) + BFLB_NETLINK_DATA_HDR_LEN + data_size;
    netlink_msg_packt = (struct bflb_netlink_msg_packt *)frame_elem->buff_addr;

    /* Build data type header */
    data_hdr = (struct bflb_netlink_data_hdr *)netlink_msg_packt->data;
    data_hdr->data_type = data_type;
    memset(data_hdr->reserved, 0, sizeof(data_hdr->reserved));

    /* Copy user data after the header */
    memcpy(data_hdr->data, data_buff, data_size);
    frame_elem->data_size = total_len;
    netlink_msg_packt->msg_packt.len = total_len - sizeof(struct bflb_msg_packt);
    netlink_msg_packt->msg_packt.tag = priv->msg_tag;
    netlink_msg_packt->msg_packt.sub_tag = 0;
    netlink_msg_packt->reseved[0] = 0;
    netlink_msg_packt->flag = BL_NETLINK_FLAG_UPLD_DATA;

#if NETLINK_DNLD_CREDIT_MAX
    /* Add flow control protection to prevent race conditions with command packets */
    irq_flags = bflb_irq_save();
    if (priv->credit_limit_update_last == priv->device_dnld_credit_limit) {
        netlink_msg_packt->credit_update_flag = false;
        netlink_msg_packt->credit_limit_cnt = priv->device_dnld_credit_limit;
        LOG_D("[CREDIT] UPLD: credit_update_flag=0 (unchanged)\n");
    } else {
        netlink_msg_packt->credit_update_flag = true;
        netlink_msg_packt->credit_limit_cnt = priv->device_dnld_credit_limit;
        LOG_D("[CREDIT] UPLD: credit_update_flag=1, credit_limit=%u (last was %u)\n",
              priv->device_dnld_credit_limit, priv->credit_limit_update_last);
        priv->credit_limit_update_last = priv->device_dnld_credit_limit;
    }

    /* Update flow control */
    if (priv->host_upld_credit_limit > 0) {
        priv->device_upld_credit_consumed++;
    }

    /* Send message */
    ret = bflb_msg_ctrl_send(priv->msg_ctrl, frame_elem, 0);
    bflb_irq_restore(irq_flags);
#else
    netlink_msg_packt->credit_update_flag = 0;
    netlink_msg_packt->credit_limit_cnt = 0;

    /* Update flow control */
    if (priv->host_upld_credit_limit > 0) {
        priv->device_upld_credit_consumed++;
    }

    /* Send message */
    ret = bflb_msg_ctrl_send(priv->msg_ctrl, frame_elem, 0);
#endif

    if (ret < 0) {
        LOG_E("Failed to send netlink message: %d\r\n", ret);
        frame_queue_free_elem(frame_elem);
        return -1;
    }

    LOG_D("Sent netlink data, len: %d\r\n", data_size);
    return data_size;
}

/*****************************************************************************
 * Message controller callbacks
 *****************************************************************************/

/**
 * @brief Download receive done callback (from host to device)
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to Netlink private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_dnld_recv_done_cb(frame_elem_t *frame_elem, void *arg)
{
    struct bflb_netlink_priv *priv = (struct bflb_netlink_priv *)arg;

    if (priv == NULL) {
        LOG_E("Invalid private structure\r\n");
        return -1;
    }

    /* Note: Flow control credit counting is done in netlink_proc_task(), not here */

    /* Send frame to processing queue */
    if (frame_queue_send(priv->msg_dnld_queue, &frame_elem, 0) < 0) {
        LOG_E("Failed to send to dnld queue\r\n");
        frame_queue_free_elem(frame_elem);
        return -1;
    }

    /* Wake up processing task */
    netlink_proc_task_wakeup(priv);

    return 0;
}

/**
 * @brief Upload send done callback (from device to host completed)
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to Netlink private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_done_cb(frame_elem_t *frame_elem, void *arg)
{
    struct bflb_netlink_priv *priv = (struct bflb_netlink_priv *)arg;

    if (priv == NULL) {
        LOG_E("Invalid private structure\r\n");
        return -1;
    }

    LOG_D("Upload send done\r\n");

    /* Free frame buffer */
    frame_queue_free_elem(frame_elem);

    /* Wake up processing task */
    netlink_proc_task_wakeup(priv);

    return 0;
}

/**
 * @brief Hardware reset callback for message controller
 * @param[in] unused Unused parameter
 * @param[in] arg Pointer to Netlink private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_hw_reset_cb(frame_elem_t *unused, void *arg)
{
    struct bflb_netlink_priv *priv = (struct bflb_netlink_priv *)arg;

    priv->reset_flag = true;
    netlink_proc_task_wakeup(priv);

    LOG_W("Received hw reset request\r\n");

    return 0;
}

/*****************************************************************************
 * Public API implementation
 *****************************************************************************/

/**
 * @brief Send data to host (upload) with type
 * @param[in] data_type Data type (see bflb_netlink_data_type enum)
 * @param[in] data_buff Data buffer to send
 * @param[in] data_size Data size
 * @retval >0 Number of bytes sent
 * @retval <0 Error code
 */
int bflb_netlink_upld_send(uint8_t data_type, uint8_t *data_buff, uint32_t data_size)
{
    struct bflb_netlink_priv *priv = g_netlink_priv;

    if (priv->netlink_status != BL_NETLINK_DSTA_DEVICE_RUN) {
        printf("%s, drop data_size:%d, because of netlink_status:%d\r\n", __FUNCTION__, data_size, priv->netlink_status);
        return data_size;
    }

    return netlink_upld_data_send(priv, data_type, data_buff, data_size);
}

/**
 * @brief Register download data callback for specific data type
 * @param[in] data_type Data type (see bflb_netlink_data_type enum)
 * @param[in] dnld_cb Download callback function
 * @param[in] cbpri_arg Callback private argument
 * @retval 0 Success
 * @retval <0 Error code
 */
int bflb_netlink_dnld_register(uint8_t data_type, bflb_netlink_callback_t dnld_cb, void *cbpri_arg)
{
    struct bflb_netlink_priv *priv = g_netlink_priv;

    if (priv == NULL) {
        LOG_E("Netlink not initialized\r\n");
        return -1;
    }

    if (data_type >= BFLB_NETLINK_DATA_TYPE_MAX) {
        LOG_E("Invalid data_type: %d (max: %d)\r\n", data_type, BFLB_NETLINK_DATA_TYPE_MAX);
        return -2;
    }

    priv->dnld_cb[data_type] = dnld_cb;
    priv->cbpri_arg[data_type] = cbpri_arg;

    LOG_I("Download callback registered for type=%s\r\n", bflb_netlink_data_type_to_str(data_type));
    return 0;
}

/*****************************************************************************
 * Netlink daemon thread
 *****************************************************************************/

/**
 * @brief Netlink daemon thread
 * @details Main tasks:
 *   1. Process state machine transitions
 *   2. Process received messages from host
 *   3. Handle flow control
 *   4. Monitor reset requests
 * @param[in] arg Pointer to Netlink private structure
 */
static void netlink_proc_task(void *arg)
{
    int ret;
    struct bflb_netlink_priv *priv = (struct bflb_netlink_priv *)arg;

    frame_elem_t *frame_elem;
    struct bflb_netlink_msg_packt *netlink_msg_packt;

    bool msg_dnld_recv_suspend = false;

    LOG_I("Netlink daemon thread started\r\n");

    /* Initial state */
    priv->netlink_status = BL_NETLINK_DSTA_IDLE;
    priv->netlink_ready = false;

    while (!priv->stop_requested) {
        /* Wait for notification or timeout */
        if (msg_dnld_recv_suspend) {
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10));
        }

        if (priv->reset_flag) {
            LOG_W("Processing reset request\r\n");
            priv->reset_flag = false;
            priv->netlink_ready = false;
            priv->netlink_status = BL_NETLINK_DSTA_IDLE;
        }

        /* Get message from queue */
        ret = frame_queue_receive(priv->msg_dnld_queue, &frame_elem, 0);
        if (ret < 0) {
            msg_dnld_recv_suspend = true;
            frame_elem = NULL;
            netlink_msg_packt = NULL;
        } else {
            msg_dnld_recv_suspend = false;
            netlink_msg_packt = (struct bflb_netlink_msg_packt *)frame_elem->buff_addr;
        }

        /* Process HOST_RESET/STOP in any state */
        if (netlink_msg_packt != NULL &&
            (netlink_msg_packt->flag == BL_NETLINK_FLAG_HOST_RESET ||
             netlink_msg_packt->flag == BL_NETLINK_FLAG_HOST_STOP)) {
            LOG_W("Received host %s command\r\n",
                  netlink_msg_packt->flag == BL_NETLINK_FLAG_HOST_RESET ? "RESET" : "STOP");

            priv->netlink_status = BL_NETLINK_DSTA_IDLE;
            priv->netlink_ready = false;
            priv->device_dnld_credit_limit = NETLINK_DNLD_CREDIT_MAX;
            priv->credit_limit_update_last = NETLINK_DNLD_CREDIT_MAX;
            priv->credit_cnt = NETLINK_DNLD_CREDIT_MAX;

            frame_queue_free_elem(frame_elem);
            continue;
        }

        /* State machine */
        switch (priv->netlink_status) {
            case BL_NETLINK_DSTA_IDLE:
                /* Wait for HOST ready */
                if (netlink_msg_packt == NULL || netlink_msg_packt->flag != BL_NETLINK_FLAG_HOST_READY) {
                    break;
                }
                LOG_I("Received HOST_READY message\r\n");

                priv->netlink_status = BL_NETLINK_DSTA_HOST_READY;
                // break;

            case BL_NETLINK_DSTA_HOST_READY:
                /* Host ready, send DEVICE_START */

                priv->device_dnld_credit_limit = NETLINK_DNLD_CREDIT_MAX;
                priv->credit_limit_update_last = 0;
                priv->credit_cnt = NETLINK_DNLD_CREDIT_MAX;

                ret = msg_upld_send_cmd_packt(priv, BL_NETLINK_FLAG_DEVICE_START);
                if (ret < 0) {
                    LOG_E("Failed to send DEVICE_START command\r\n");
                    break;
                }

                priv->netlink_status = BL_NETLINK_DSTA_DEVICE_RUN;
                // break;

            case BL_NETLINK_DSTA_DEVICE_RUN:
                break;

            default:
                LOG_E("Invalid Netlink status: %d\r\n", priv->netlink_status);
                priv->netlink_status = BL_NETLINK_DSTA_IDLE;
                if (netlink_msg_packt) {
                    frame_queue_free_elem(frame_elem);
                    frame_elem = NULL;
                }
                break;
        }

        /* Output and free received packets */
        if (netlink_msg_packt) {
            if (priv->netlink_status == BL_NETLINK_DSTA_DEVICE_RUN && netlink_msg_packt->flag == BL_NETLINK_FLAG_DNLD_DATA) {
                if (priv->netlink_ready == false) {
                    priv->netlink_ready = true;
                    LOG_I("Netlink ready for data transfer\r\n");
                }
                netlink_dnld_data_output(priv, frame_elem);
            } else {
                frame_queue_free_elem(frame_elem);
            }
        }

#if NETLINK_DNLD_CREDIT_MAX
        /* Actively send flow control updates */
        if (priv->netlink_status == BL_NETLINK_DSTA_DEVICE_RUN && priv->netlink_ready &&
            (int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) > NETLINK_DNLD_CREDIT_UPDATE_THRESHOLD) {
            LOG_D("[CREDIT] Sending CREDIT_UPDATE: limit=%u, last=%u, threshold=%d\n",
                  priv->device_dnld_credit_limit, priv->credit_limit_update_last,
                  (int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last));
            ret = msg_upld_send_cmd_packt(priv, BL_NETLINK_FLAG_CREDIT_UPDATE);
            if (ret < 0) {
                LOG_E("Failed to send credit update message\r\n");
            }
        }
#endif
    }

    LOG_I("Netlink daemon thread exiting\r\n");
    priv->netlink_proc_task = NULL;
    vTaskDelete(NULL);
}

/*****************************************************************************
 * Initialization and deinitialization
 *****************************************************************************/

/**
 * @brief Initialize Netlink device
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval 0 Success
 * @retval <0 Error code
 */
int bflb_netlink_init(bflb_msg_ctrl_t *msg_ctrl, uint8_t msg_tag)
{
    struct bflb_netlink_priv *priv;
    int ret;
    frame_queue_ctrl_init_cfg_t frame_cfg;

    if (msg_ctrl == NULL) {
        LOG_E("Invalid msg_ctrl\r\n");
        return -1;
    }

    if (g_netlink_priv != NULL) {
        LOG_W("Netlink already initialized\r\n");
        return 0;
    }

    /* Allocate private structure */
    priv = pvPortMalloc(sizeof(struct bflb_netlink_priv));
    if (priv == NULL) {
        LOG_E("Failed to allocate private structure\r\n");
        return -1;
    }
    memset(priv, 0, sizeof(struct bflb_netlink_priv));

    priv->msg_ctrl = msg_ctrl;
    priv->msg_tag = msg_tag;
    priv->reset_flag = false;
    priv->stop_requested = false;
    priv->netlink_ready = false;
    priv->netlink_status = BL_NETLINK_DSTA_IDLE;

    /* Initialize flow control */
    priv->device_dnld_credit_limit = NETLINK_DNLD_CREDIT_MAX;
    priv->credit_limit_update_last = 0;
    priv->credit_cnt = 0;
    priv->host_upld_credit_limit = 0;
    priv->device_upld_credit_consumed = 0;

    /* Initialize RX frame controller */
    frame_cfg.frame_buff = netlink_frame_buff;
    frame_cfg.frame_type = 0;
    frame_cfg.frame_elem_cnt = NETLINK_FRAME_CNT;
    frame_cfg.frame_elem_size = NETLINK_FRAME_SIZE;
    frame_cfg.name = "netlink";

    priv->rx_frame_ctrl = frame_queue_create(&frame_cfg);
    if (priv->rx_frame_ctrl == NULL) {
        LOG_E("Failed to initialize RX frame controller\r\n");
        goto cleanup_priv;
    }

    /* Create message download queue */
    priv->msg_dnld_queue = xQueueCreate(NETLINK_MSG_QUEUE_DEPTH, sizeof(frame_elem_t *));
    if (priv->msg_dnld_queue == NULL) {
        LOG_E("Failed to create message queue\r\n");
        goto cleanup_frame_ctrl;
    }

    /* Register message callbacks */
    ret = bflb_msg_cb_register(msg_ctrl, msg_tag,
                                msg_dnld_recv_done_cb, priv,
                                msg_upld_send_done_cb, priv,
                                msg_hw_reset_cb, priv);
    if (ret < 0) {
        LOG_E("Failed to register callbacks: %d\r\n", ret);
        goto cleanup_queue;
    }

    /* Register free callback */
    frame_queue_free_cb_register(priv->rx_frame_ctrl, NULL, NULL,
                                  netlink_free_after_cb, priv);

    /* Create processing task */
    ret = xTaskCreate(netlink_proc_task, (char *)"netlink_proc",
                     2048, priv, 12, &priv->netlink_proc_task);
    if (ret != pdPASS) {
        LOG_E("Failed to create processing task\r\n");
        goto cleanup_callback;
    }

    g_netlink_priv = priv;

    LOG_I("Netlink device initialized successfully\r\n");
    return 0;

cleanup_callback:
    bflb_msg_cb_unregister(msg_ctrl, msg_tag);
cleanup_queue:
    vQueueDelete(priv->msg_dnld_queue);
cleanup_frame_ctrl:
    frame_queue_remove(priv->rx_frame_ctrl);
cleanup_priv:
    vPortFree(priv);
    return -1;
}

/**
 * @brief Deinitialize Netlink device
 * @retval 0 Success
 * @retval <0 Error code
 */
int bflb_netlink_deinit(void)
{
    struct bflb_netlink_priv *priv = g_netlink_priv;

    if (priv == NULL) {
        LOG_W("Netlink not initialized\r\n");
        return 0;
    }

    LOG_I("Deinitializing Netlink device\r\n");

    /* Stop processing task */
    priv->stop_requested = true;
    priv->netlink_ready = false;

    if (priv->netlink_proc_task != NULL) {
        /* Wait for task to finish */
        while (priv->netlink_proc_task != NULL) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    /* Unregister callbacks */
    bflb_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);

    /* Clean up queue */
    if (priv->msg_dnld_queue != NULL) {
        vQueueDelete(priv->msg_dnld_queue);
        priv->msg_dnld_queue = NULL;
    }

    /* Clean up frame controller */
    if (priv->rx_frame_ctrl != NULL) {
        frame_queue_remove(priv->rx_frame_ctrl);
        priv->rx_frame_ctrl = NULL;
    }

    /* Free private structure */
    vPortFree(priv);
    g_netlink_priv = NULL;

    LOG_I("Netlink device deinitialized\r\n");
    return 0;
}
