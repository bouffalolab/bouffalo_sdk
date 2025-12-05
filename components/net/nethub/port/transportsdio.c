#include "bflb_core.h"
#include "bflb_mtimer.h"

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdlib.h>
#include "fhost.h"

#ifdef CONFIG_SDIO3_SOFT_RST_INT_USER
#include <bflb_sdio3.h>
#endif
#include "bflb_msg_ctrl.h"
#include "bflb_frame_buff_ctrl.h"
#include "transportsdio.h"

#include <nethub_types.h>

#define DBG_TAG "WIFI_STA"
#include "log.h"

#define ETHSTA_DBGLOG  (1)

#define DEBUG_SDIO_LOG_ENABLE    (0)// 1-enable log. 0-no log
#define DEBUG_BYPASS_WIFI_TX     (0)// 1-not send to wifi fw. 0-normal
#define DEBUG_BYPASS_WIFI_RX     (0)// 1-not send to wifi fw. 0-normal
#define DEBUG_DUMP_WIFITX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data
#define DEBUG_DUMP_WIFIRX_ENABLE (0)// 1-dump wifi rx data. 0-not dump wifi rx data

/* Debug output for example program */
#define EXAMPLE_PRINTF printf
#if DEBUG_SDIO_LOG_ENABLE
#define EXAMPLE_DEBUG       LOG_D//(fmt, ...) printf("[transport debug] " fmt, ##__VA_ARGS__)
#define EXAMPLE_INFO        LOG_I//(fmt, ...) printf("[TRANSSDIO] " fmt, ##__VA_ARGS__)
#define EXAMPLE_ERR         LOG_E//(fmt, ...) printf("[TRANSSDIO] " fmt, ##__VA_ARGS__)
#else
#define EXAMPLE_DEBUG(fmt, ...)
#define EXAMPLE_INFO(fmt, ...)
#define EXAMPLE_ERR(fmt, ...)
#endif

/*****************************************************************************
 * Global Variables
 *****************************************************************************/

/* External NETHUB statistics variable declaration */
extern nethub_statistics_t g_nethub_stats;

/** WiFi MAC address */
//static uint8_t wifi_mac_addr[6] = { 0x22, 0x22, 0x22, 0x22, 0x34, 0x56 };

/** Frame buffer memory attribute (aligned and non-cached) */
#define FRAME_BUFFER_ATTR __ALIGNED(32) ATTR_NOCACHE_NOINIT_RAM_SECTION
/** Download frame buffer pool */
static FRAME_BUFFER_ATTR uint8_t wifi_frame_buff[WIFI_FRAME_CNT][FRAME_BUFF_TOTALSIZE];

/** Global private structure pointer */
static wifi_wifista_priv_t *g_wifi_priv = NULL;


/* Payload offset mapping functions for SDIO upld direction */
struct payload_offset_entry {
    uint8_t *payload_addr;    /* payload address, NULL means unused */
    uint32_t offset;          /* corresponding offset value */
};

static struct payload_offset_entry offset_whitelist[FHOST_RX_BUF_CNT];

/** Free queue message structure */
typedef struct {
    frame_elem_t *frame_elem;
} free_msg_t;

/*****************************************************************************
 * Forward Declarations
 *****************************************************************************/

static void wifi_proc_task(void *arg);
static void frame_free_task(void *arg);

static int msg_dnld_recv_done_cb(frame_elem_t *frame_elem, void *arg);
static int msg_upld_send_done_cb(frame_elem_t *frame_elem, void *arg);
static int transportsdio_hwreset(frame_elem_t *frame_elem, void *arg);

/* WiFi TX completion callback */
static void wifi_tx_done_cb(struct wifi_wifista_priv *priv, frame_elem_t *frame_elem);

/**
 * @brief Wake up the WiFi processing task
 * @param[in] priv Pointer to WiFi private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int wifi_proc_task_wakeup(struct wifi_wifista_priv *priv)
{
    if (priv->wifi_proc_task == NULL) {
        EXAMPLE_ERR("wifi_proc_task is NULL, please init msg_ctrl first!\r\n");
        return -1;
    }

    /* Wake up message processing task */
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(priv->wifi_proc_task, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        xTaskNotifyGive(priv->wifi_proc_task);
    }

    return 0;
}

/**
 * @brief Callback after frame buffer is freed, used to wake up the processing task
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to WiFi private structure
 */
static void wifi_free_after_cb(frame_elem_t *frame_elem, void *arg)
{
    wifi_proc_task_wakeup((struct wifi_wifista_priv *)arg);
}

/**
 * @brief Update WiFi connection status
 * @param[in] priv Pointer to WiFi private structure
 * @retval true WiFi is connected
 * @retval false WiFi is disconnected
 */
static bool wifi_connection_state_update(struct wifi_wifista_priv *priv)
{
    /* Connection status is now controlled externally through linkup/linkdown interface */
    return priv->wifi_connected;
}

/**
 * @brief WiFi TX complete callback, directly releases frame
 * @param[in] priv Pointer to WiFi private structure
 * @param[in] frame_elem Pointer to frame element
 * @param[in] success TX success flag
 */
static void wifi_tx_done_cb(struct wifi_wifista_priv *priv, frame_elem_t *frame_elem)
{
    if (!frame_elem) {
        return;
    }

    //EXAMPLE_PRINTF("elem:%p, buf:%p\r\n", frame_elem, frame_elem->buff_addr);
    /* Free frame from message */
    frame_queue_free_elem(frame_elem);

    priv->credit_cnt += 1;
    priv->device_dnld_credit_limit += 1;

    /* Flow control threshold reached, wake up thread to update flow control */
    if ((int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) > WIFI_DNLD_CREDIT_UPDATE_THRESHOLD) {
        wifi_proc_task_wakeup(priv);
    }
}

/**
 * @brief Send frame via WiFi interface
 * @param[in] priv Pointer to WiFi private structure
 * @param[in] frame_elem Pointer to frame element
 * @retval 0 Success
 * @retval <0 Error
 */
static int wifi_send_frame(struct wifi_wifista_priv *priv, frame_elem_t *frame_elem)
{
    int ret = -1;
    struct bflb_wifi_msg_packt *msg_tx_packt;

    if (!priv || !frame_elem) {
        return -1;
    }

    msg_tx_packt = (struct bflb_wifi_msg_packt *)frame_elem->buff_addr;

    if (msg_tx_packt->msg_packt.len >= 14) {
        static int s_dnld_cnt = 0;
        s_dnld_cnt++;
    #if DEBUG_DUMP_WIFITX_ENABLE
        EXAMPLE_INFO("Dnld[%d]<%d>  real_cb:%p, real_ctx:%p, real_arg:%p ----->  %02X:%02X:%02X:%02X:%02X:%02X  %02X:%02X:%02X:%02X:%02X:%02X   %02X:%02X .... %02X:%02X\r\n",
            msg_tx_packt->msg_packt.len, s_dnld_cnt, wifi_tx_done_cb, priv, frame_elem,
            msg_tx_packt->data[0], msg_tx_packt->data[1], msg_tx_packt->data[2], msg_tx_packt->data[3],
            msg_tx_packt->data[4], msg_tx_packt->data[5], msg_tx_packt->data[6], msg_tx_packt->data[7],
            msg_tx_packt->data[8], msg_tx_packt->data[9], msg_tx_packt->data[10], msg_tx_packt->data[11],
            msg_tx_packt->data[12], msg_tx_packt->data[13],
            msg_tx_packt->data[msg_tx_packt->msg_packt.len - 2], msg_tx_packt->data[msg_tx_packt->msg_packt.len - 1]);
    #endif
    }

#if 1
    if (priv->wifi_send_frame) {
        ret = priv->wifi_send_frame(priv,
                                   (void *)((uintptr_t)frame_elem->buff_addr + sizeof(struct bflb_wifi_msg_packt)),
                                   msg_tx_packt->msg_packt.len + sizeof(bflb_msg_packt_t) - sizeof(struct bflb_wifi_msg_packt),
                                   wifi_tx_done_cb, frame_elem);
    }
#else
    wifi_tx_done_cb(priv, frame_elem);
#endif

    return ret;
}

/**
 * @brief Receive frame via WiFi interface
 * @param[in] priv Pointer to WiFi private structure
 * @param[in] frame_elem Pointer to frame element
 * @retval 0 Success
 * @retval <0 Error
 */
static int wifi_recv_frame(struct wifi_wifista_priv *priv, frame_elem_t *frame_elem)
{
    int ret = 0;

    if (!priv || !frame_elem) {
        return -1;
    }

    if (priv->wifi_recv_frame) {
        ret = priv->wifi_recv_frame(priv,
                                   (void *)((uintptr_t)frame_elem->buff_addr + sizeof(struct bflb_wifi_msg_packt)),
                                   WIFI_RX_BUFF_SIZE);
    }

    return ret;
}

/**
 * @brief MSG download receive done callback, puts received message into queue for processing
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to WiFi private structure
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_dnld_recv_done_cb(frame_elem_t *frame_elem, void *arg)
{
    int ret;
    struct wifi_wifista_priv *priv = (struct wifi_wifista_priv *)arg;

    if (!priv || !frame_elem) {
        return -1;
    }

    ret = frame_queue_send(priv->msg_dnld_queue, &frame_elem, 0);
    #if CONFIG_NETHUB_DUMP_DNLDDATA
    EXAMPLE_PRINTF("send frame_elem:%p, buf:%p, ret:%d, queue_len:%d\n", frame_elem, frame_elem->buff_addr, ret, frame_queue_messages_waiting(priv->msg_dnld_queue));
    #endif
    if (ret < 0) {
        EXAMPLE_ERR("Failed to send frame to download queue\r\n");
        return -1;
    }

    /* Statistics: DNLD producer generates packets */
    //g_nethub_stats.dnld_producer_count++;

    wifi_proc_task_wakeup(priv);
    return 0;
}

/**
 * @brief MSG upload send done callback, releases data back to pool
 * @param[in] frame_elem Pointer to frame element
 * @param[in] arg Pointer to WiFi private structure
 * @retval 0 Success
 * @retval <0 Error
 */
#if 0
static int msg_upld2_send_done_cb(frame_elem_t *frame_elem, void *arg)
{
    int ret;
    skb_free_cb_t skb_free = NULL;
    void *skb;

    struct wifi_wifista_priv *priv = (struct wifi_wifista_priv *)arg;
    if (!frame_elem) {
        return -1;
    }
    if (!priv) {
        EXAMPLE_ERR("\x1b[33m%s:%d\x1b[0m priv is NULL.\r\n", __func__, __LINE__);
        return -1;
    }

    if (frame_elem->type == 0x11) {
        skb_free = frame_elem->skb_free;
        skb = frame_elem->skb;

        if (skb_free) {
            skb_free(skb);
        }
    } else {
        ret = frame_queue_send(priv->msg_dnld_queue, &frame_elem, 0);
        if (ret < 0) {
            EXAMPLE_ERR("Failed to send frame to download queue\r\n");
            return -1;
        }

        wifi_proc_task_wakeup(priv);
    }
    return 0;
}
#endif
static int msg_upld_send_done_customfree(frame_elem_t *frame_elem)
{
    struct wifi_wifista_priv *priv = (struct wifi_wifista_priv *)g_wifi_priv;
    free_msg_t free_msg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (!priv || !frame_elem) {
        return -1;
    }

    /* In interrupt context, send message to frame_free_queue, let frame_free_task free frame */
    free_msg.frame_elem = frame_elem;

    if (xPortIsInsideInterrupt()) {
        /* Use ISR version in interrupt context */
        if (xQueueSendFromISR(priv->frame_free_queue, &free_msg, &xHigherPriorityTaskWoken) != pdTRUE) {
            return -1;
        }
        /* Perform task switch if needed */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        /* Send directly in non-interrupt context */
        if (xQueueSend(priv->frame_free_queue, &free_msg, 0) != pdTRUE) {
            return -1;
        }
    }

    return 0;
}

static int msg_upld_send_done_cb(frame_elem_t *frame_elem, void *arg)
{
    EXAMPLE_DEBUG("%s, frame_elem:%p, frame_elem->skb_free:%p\r\n",
        __FUNCTION__, frame_elem, frame_elem->skb_free);
    if (frame_elem && (frame_elem->skb_free == NULL)) {
        return frame_queue_free_elem(frame_elem);
    }
    return msg_upld_send_done_customfree(frame_elem);
}

static int transportsdio_hwreset(frame_elem_t *frame_elem, void *arg)
{
    return 0;
}

/**
 * @brief Send MSG upload control packet
 * @param[in] priv Pointer to WiFi private structure
 * @param[in] wifi_flag WiFi flag to send
 * @retval 0 Success
 * @retval <0 Error
 */
static int msg_upld_send_cmd_packet(struct wifi_wifista_priv *priv, uint8_t wifi_flag, uint8_t tofront)
{
    int ret;
    uintptr_t flag;
    frame_elem_t *frame_elem;
    struct bflb_wifi_msg_packt *msg_tx_packt;

    ret = frame_queue_alloc_elem(priv->rx_frame_ctrl, &frame_elem, 1000);
    if (ret < 0) {
        EXAMPLE_ERR("Failed to allocate RX frame element\r\n");
        return -1;
    }

    frame_elem->skb_free = NULL;
    frame_elem->skb = NULL;
    EXAMPLE_DEBUG("===%s, frame_elem:%p\r\n", __FUNCTION__, frame_elem);

    msg_tx_packt = (struct bflb_wifi_msg_packt *)frame_elem->buff_addr;

    if (wifi_flag == BL_WIFI_FLAG_DEVICE_READY) {
        /* Fill MAC address */
        frame_elem->data_size = sizeof(struct bflb_wifi_msg_packt) + sizeof(priv->mac_addr);
        memcpy(msg_tx_packt->data, priv->mac_addr, sizeof(priv->mac_addr));
        EXAMPLE_INFO("Sending DEVICE_READY with MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
              priv->mac_addr[0], priv->mac_addr[1], priv->mac_addr[2],
              priv->mac_addr[3], priv->mac_addr[4], priv->mac_addr[5]);
    } else {
        frame_elem->data_size = sizeof(struct bflb_wifi_msg_packt);
    }

    msg_tx_packt->msg_packt.len = frame_elem->data_size - sizeof(bflb_msg_packt_t);
    msg_tx_packt->msg_packt.tag = priv->msg_tag;
    msg_tx_packt->msg_packt.sub_tag = 0;
    msg_tx_packt->flag = wifi_flag;

    /* Lock to prevent flow control competition and disorder with UPLD_DATA */
    flag = bflb_irq_save();
    if (wifi_flag == BL_WIFI_FLAG_CREDIT_UPDATE || wifi_flag == BL_WIFI_FLAG_DEVICE_START) {
        if (priv->credit_limit_update_last == priv->device_dnld_credit_limit) {
            msg_tx_packt->credit_update_flag = false;
            msg_tx_packt->credit_limit_cnt = priv->device_dnld_credit_limit;
        } else {
            msg_tx_packt->credit_update_flag = true;
            msg_tx_packt->credit_limit_cnt = priv->device_dnld_credit_limit;
            priv->credit_limit_update_last = priv->device_dnld_credit_limit;
            EXAMPLE_DEBUG("Sending CREDIT_UPDATE message, credit limit: %d\r\n", priv->device_dnld_credit_limit);
        }
    }

    ret = bflb_msg_ctrl_send(priv->msg_ctrl, frame_elem, tofront);
    bflb_irq_restore(flag);

    if (ret < 0) {
        EXAMPLE_ERR("Failed to send DEVICE_READY message\r\n");
        frame_queue_free_elem(frame_elem);
        return -1;
    }

    return 0;
}

/**
 * @brief WiFi daemon thread
 * @details Main tasks:
 *   1. Process received RX frames and send to host
 *   2. Process TX frames to be sent via WiFi
 *   3. Monitor WiFi connection state changes
 * @param[in] arg Pointer to WiFi private structure
 */
static void wifi_proc_task(void *arg)
{
    int ret;
    struct wifi_wifista_priv *priv = (struct wifi_wifista_priv *)arg;

    frame_elem_t *frame_elem;
    struct bflb_wifi_msg_packt *msg_packt;

    uint32_t wifi_update_ms = 0;
    bool msg_dnld_recv_suspend = false;

    EXAMPLE_INFO("WiFi daemon thread started\r\n");

    /* Initial state */
    priv->wifi_status = BL_WIFI_DSTA_IDLE;
    priv->wifi_connected = false;

    while (!priv->stop_requested) {
        /* Wait */
        if (msg_dnld_recv_suspend) {
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10));
        }

        /* update WiFi connection status */
        if (bflb_mtimer_get_time_ms() - wifi_update_ms > 100) {
            wifi_connection_state_update(priv);
            wifi_update_ms = bflb_mtimer_get_time_ms();
        }

        /* get msg recv frame*/
        ret = frame_queue_receive(priv->msg_dnld_queue, &frame_elem, 0);
        if (ret < 0) {
            msg_dnld_recv_suspend = true;
            frame_elem = NULL;
            msg_packt = NULL;
        } else {
            msg_dnld_recv_suspend = false;
            msg_packt = (struct bflb_wifi_msg_packt *)frame_elem->buff_addr;
            #if DEBUG_DUMP_WIFITX_ENABLE
            EXAMPLE_PRINTF("recv dnld frame_elem:%p, buf:%p, ret:%d, queue_len:%d\n", frame_elem, frame_elem->buff_addr, ret, frame_queue_messages_waiting(priv->msg_dnld_queue));
            #endif
        }

        /* State machine */
        switch (priv->wifi_status) {
            case BL_WIFI_DSTA_IDLE:
                /* Wait for HOST ready */
                if (msg_packt == NULL || msg_packt->flag != BL_WIFI_FLAG_HOST_READY) {
                    break;
                }
                EXAMPLE_INFO("Received HOST_READY message\r\n");

            case BL_WIFI_DSTA_HOST_READY:
                /* Host ready, send DEVICE_READY */
                EXAMPLE_INFO("Host ready, sending DEVICE_READY\r\n");
                ret = msg_upld_send_cmd_packet(priv, BL_WIFI_FLAG_DEVICE_READY, 0);
                if (ret < 0) {
                    EXAMPLE_ERR("Failed to send DEVICE_READY message\r\n");
                    break;
                }

                priv->wifi_status = BL_WIFI_DSTA_DEVICE_READY;
                break;

            case BL_WIFI_DSTA_DEVICE_READY:
                /* Host ready, send DEVICE_READY after initialization */
                if (priv->wifi_connected == false) {
                    break;
                }

                EXAMPLE_INFO("WiFi connected, sending DEVICE_START\r\n");

                priv->device_dnld_credit_limit = WIFI_DNLD_CREDIT_MAX;
                priv->credit_limit_update_last = 0;
                priv->credit_cnt = WIFI_DNLD_CREDIT_MAX;

                ret = msg_upld_send_cmd_packet(priv, BL_WIFI_FLAG_DEVICE_START, 0);
                if (ret < 0) {
                    EXAMPLE_ERR("Failed to send DEVICE_START message\r\n");
                    break;
                }

                priv->wifi_status = BL_WIFI_DSTA_DEVICE_RUN;
                break;

            case BL_WIFI_DSTA_DEVICE_RUN:
                if (priv->wifi_connected == true) {
                    break;
                }
                EXAMPLE_INFO("WiFi disconnected, sending DEVICE_STOP\r\n");

                ret = msg_upld_send_cmd_packet(priv, BL_WIFI_FLAG_DEVICE_STOP, 0);
                if (ret < 0) {
                    EXAMPLE_ERR("Failed to send DEVICE_STOP message\r\n");
                    break;
                }

                priv->wifi_status = BL_WIFI_DSTA_IDLE;
                break;

            default:
                break;
        }

        /* Send received msg WiFi packets via WiFi */
        if (msg_packt) {
            if (priv->wifi_status == BL_WIFI_DSTA_DEVICE_RUN && msg_packt->flag == BL_WIFI_FLAG_DNLD_DATA) {
                /* Process download data */
                ret = wifi_send_frame(priv, frame_elem);
                if (ret < 0) {
                    EXAMPLE_ERR("Failed to send frame via WiFi: %d\r\n", ret);
                    frame_queue_free_elem(frame_elem);
                }
                /* Statistics: DNLD producer generates packets */
                g_nethub_stats.dnld_producer_count++;
            } else {
                frame_queue_free_elem(frame_elem);
            }
        }

        /* Actively send flow control update, needed when no uplink data */
        if (priv->wifi_status == BL_WIFI_DSTA_DEVICE_RUN &&
            (int8_t)(priv->device_dnld_credit_limit - priv->credit_limit_update_last) > WIFI_DNLD_CREDIT_UPDATE_THRESHOLD) {// todo: double check
            ret = msg_upld_send_cmd_packet(priv, BL_WIFI_FLAG_CREDIT_UPDATE, 1);
            if (ret < 0) {
                EXAMPLE_ERR("Failed to send CREDIT_UPDATE message\r\n");
            }
        }
    }

    EXAMPLE_INFO("WiFi daemon thread exiting\r\n");
    vTaskDelete(NULL);
}

int app_customfree3(frame_elem_t *frame_elem)
{
    if (frame_elem->skb_free) {
#if DEBUG_DUMP_WIFIRX_ENABLE
        EXAMPLE_INFO("upld done ================== %s   cb=%p, p=%p\r\n",
            __FUNCTION__, frame_elem->skb_free,frame_elem->skb);
#endif
        /* Statistics: UPLD SDIO transfer complete */
        g_nethub_stats.upld_transfer_complete++;

        frame_elem->skb_free(frame_elem->skb);
    }
    return 0;
}

/**
 * @brief Frame free task - safely free frame elements outside interrupt context
 * @param[in] arg Pointer to WiFi private structure
 */
static void frame_free_task(void *arg)
{
    struct wifi_wifista_priv *priv = (struct wifi_wifista_priv *)arg;
    free_msg_t free_msg;

    EXAMPLE_INFO("Frame free task started\r\n");

    while (!priv->stop_requested) {
        /* Wait for free message */
        if (xQueueReceive(priv->frame_free_queue, &free_msg, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (free_msg.frame_elem) {
                /* Safely free frame element */
                // frame_queue_free_elem(free_msg.frame_elem);
                app_customfree3(free_msg.frame_elem);
            }
        }
    }

    EXAMPLE_INFO("Frame free task exiting\r\n");
    vTaskDelete(NULL);
}
// extern int platform_get_mac(uint8_t *mac);

/*****************************************************************************
 * Public API Implementation
 *****************************************************************************/

/**
 * @brief Initialize WiFi device
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_init(wifi_wifista_priv_t *ctx, bflb_msg_ctrl_t *msg_ctrl, uint8_t msg_tag)
{
    int ret;
    struct wifi_wifista_priv *priv = (struct wifi_wifista_priv *)ctx;

    /* Basic field initialization */
    priv->msg_ctrl = msg_ctrl;
    priv->msg_tag = msg_tag;
    priv->wifi_status = BL_WIFI_DSTA_IDLE;
    priv->stop_requested = false;
    priv->wifi_connected = false;
    priv->wifi_send_frame = NULL;
    priv->wifi_recv_frame = NULL;
    priv->flow_control_enabled = true;
    priv->flow_control_threshold = WIFI_DNLD_CREDIT_UPDATE_THRESHOLD;
    /* MAC address is set by user or obtained through other mechanisms */
    EXAMPLE_INFO("Using MAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            priv->mac_addr[0], priv->mac_addr[1], priv->mac_addr[2],
            priv->mac_addr[3], priv->mac_addr[4], priv->mac_addr[5]);
    /* Initialize private memory pool */
    frame_queue_ctrl_init_cfg_t rx_cfg = {
        .frame_buff = wifi_frame_buff,
        .frame_type = 0x02,
        .frame_elem_cnt = WIFI_FRAME_CNT,
        .frame_elem_size = FRAME_BUFF_TOTALSIZE,
        .name = "wifi_rx"
    };
    priv->rx_frame_ctrl = frame_queue_create(&rx_cfg);
    if (!priv->rx_frame_ctrl) {
        EXAMPLE_ERR("Failed to create RX frame buffer controller\r\n");
        return -1;
    }
    /* Register the callback for freeing frame_elem after processing */
    frame_queue_free_cb_register(priv->rx_frame_ctrl,
                                 NULL, NULL,
                                 wifi_free_after_cb, priv);

    /* Create msg dnld queue */
    priv->msg_dnld_queue = xQueueCreate(WIFI_MSG_QUEUE_DEPTH, sizeof(frame_elem_t *));

    /* Create frame free queue */
    priv->frame_free_queue = xQueueCreate(WIFI_MSG_QUEUE_DEPTH, sizeof(free_msg_t));

    /* Register MSG message callback */
    ret = bflb_msg_cb_register(msg_ctrl, msg_tag,
        msg_dnld_recv_done_cb, priv,
        msg_upld_send_done_cb, priv,
        transportsdio_hwreset, priv);
    if (ret < 0) {
        EXAMPLE_ERR("Failed to register message callbacks: %d\r\n", ret);
        return -1;
    }

    /* Create daemon thread */
    ret = xTaskCreate(wifi_proc_task, "wifi_proc", 2048, priv, 10, &priv->wifi_proc_task);
    if (ret != pdPASS) {
        EXAMPLE_ERR("Failed to create daemon thread: %d\r\n", ret);
        return -1;
    }

    /* Create frame free task */
    ret = xTaskCreate(frame_free_task, "frame_free", 1024, priv, 9, &priv->frame_free_task);
    if (ret != pdPASS) {
        EXAMPLE_ERR("Failed to create frame free task: %d\r\n", ret);
        return -1;
    }

    /* Save global pointer */
    g_wifi_priv = priv;

    EXAMPLE_INFO("WiFi device initialized successfully, MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
          priv->mac_addr[0], priv->mac_addr[1], priv->mac_addr[2],
          priv->mac_addr[3], priv->mac_addr[4], priv->mac_addr[5]);

    return 0;
}

/**
 * @brief Set MAC address
 * @param[in] mac_addr MAC address (6 bytes)
 * @retval 0 Success
 * @retval -1 Parameter error or device not initialized
 */
int transportsdio_set_mac_addr(const uint8_t mac_addr[6])
{
    if (!g_wifi_priv || !mac_addr) {
        return -1;
    }

    memcpy(g_wifi_priv->mac_addr, mac_addr, 6);
    return 0;
}

/**
 * @brief Get MAC address
 * @param[out] mac_addr Output MAC address buffer (6 bytes)
 * @retval 0 Success
 * @retval -1 Parameter error or device not initialized
 */
int transportsdio_get_mac_addr(uint8_t mac_addr[6])
{
    if (!g_wifi_priv || !mac_addr) {
        return -1;
    }

    memcpy(mac_addr, g_wifi_priv->mac_addr, 6);
    return 0;
}

/**
 * @brief Deinitialize WiFi device
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_deinit(void)
{
    if (g_wifi_priv) {
        g_wifi_priv->stop_requested = true;

        /* Wait for tasks to end */
        while (eTaskGetState(g_wifi_priv->wifi_proc_task) != eDeleted ||
               eTaskGetState(g_wifi_priv->frame_free_task) != eDeleted) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        /* Clean up resources */
        vQueueDelete(g_wifi_priv->msg_dnld_queue);
        vQueueDelete(g_wifi_priv->frame_free_queue);
        frame_queue_remove(g_wifi_priv->rx_frame_ctrl);

        free(g_wifi_priv);
        g_wifi_priv = NULL;
    }
    return 0;
}

/**
 * @brief Start WiFi device
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_start(void)
{
    if (g_wifi_priv) {
        EXAMPLE_INFO("WiFi device started\r\n");
        return 0;
    }
    return -1;
}

/**
 * @brief Stop WiFi device
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_stop(void)
{
    if (g_wifi_priv) {
        EXAMPLE_INFO("WiFi device stopped\r\n");
        return 0;
    }
    return -1;
}

/**
 * @brief Get WiFi connection status
 * @retval true WiFi is connected
 * @retval false WiFi is disconnected
 */
bool transportsdio_link_status(void)
{
    if (g_wifi_priv) {
        return g_wifi_priv->wifi_connected;
    }
    return false;
}

/**
 * @brief Set WiFi send frame callback function
 * @param[in] send_func WiFi send frame function pointer
 */
void transportsdio_set_wifi_send_cb(int (*send_func)(struct wifi_wifista_priv *, void *, uint32_t, void (*free_cb)(void *arg), void *cb_arg))
{
    if (g_wifi_priv) {
        g_wifi_priv->wifi_send_frame = send_func;
    }
}

/**
 * @brief Set WiFi receive frame callback function
 * @param[in] recv_func WiFi receive frame function pointer
 */
void transportsdio_set_wifi_recv_cb(int (*recv_func)(struct wifi_wifista_priv *, void *, uint32_t))
{
    if (g_wifi_priv) {
        g_wifi_priv->wifi_recv_frame = recv_func;
    }
}

/**
 * @brief WiFi connection establishment callback interface - actively called after WiFi connection success
 * @details When WiFi connection succeeds, call this interface to notify device that WiFi is connected
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_linkup(void)
{
    if (!g_wifi_priv) {
        EXAMPLE_ERR("WiFi device not initialized\r\n");
        return -1;
    }

    if (g_wifi_priv->wifi_connected) {
        LOG_W("WiFi already connected\r\n");
        return 0;
    }

    g_wifi_priv->wifi_connected = true;
    EXAMPLE_INFO("WiFi linkup - connection established\r\n");

    /* Wake up processing task to handle connection status change immediately */
    wifi_proc_task_wakeup(g_wifi_priv);

    return 0;
}

/**
 * @brief WiFi connection disconnection callback interface - actively called when WiFi disconnects
 * @details When WiFi connection is lost, call this interface to notify device that WiFi is disconnected
 * @retval 0 Success
 * @retval <0 Error code
 */
int transportsdio_linkdown(void)
{
    if (!g_wifi_priv) {
        EXAMPLE_ERR("WiFi device not initialized\r\n");
        return -1;
    }

    if (!g_wifi_priv->wifi_connected) {
        LOG_W("WiFi already disconnected\r\n");
        return 0;
    }

    g_wifi_priv->wifi_connected = false;
    EXAMPLE_INFO("WiFi linkdown - connection lost\r\n");

    /* Wake up processing task to handle connection status change immediately */
    wifi_proc_task_wakeup(g_wifi_priv);

    return 0;
}

/**
 * @brief Set flow control parameters
 * @param[in] enabled Whether to enable flow control
 * @param[in] threshold Flow control threshold
 */
void transportsdio_set_flow_control(bool enabled, uint8_t threshold)
{
    if (!g_wifi_priv) {
        EXAMPLE_ERR("WiFi device not initialized\r\n");
        return;
    }

    g_wifi_priv->flow_control_enabled = enabled;
    g_wifi_priv->flow_control_threshold = threshold > WIFI_DNLD_CREDIT_MAX ? WIFI_DNLD_CREDIT_MAX : threshold;

    EXAMPLE_INFO("Flow control %s, threshold: %d\r\n",
          enabled ? "enabled" : "disabled", g_wifi_priv->flow_control_threshold);
}

/**
 * @brief Get current flow control status
 * @param[out] enabled Flow control enable status
 * @param[out] threshold Flow control threshold
 */
void transportsdio_get_flow_control(bool *enabled, uint8_t *threshold)
{
    if (!g_wifi_priv || !enabled || !threshold) {
        EXAMPLE_ERR("Invalid parameters\r\n");
        return;
    }

    *enabled = g_wifi_priv->flow_control_enabled;
    *threshold = g_wifi_priv->flow_control_threshold;
}

/**
 * @brief Initialize payload offset whitelist
 */
void transportsdio_init_payload_offset(void)
{
    memset(offset_whitelist, 0, sizeof(offset_whitelist));
}

/**
 * @brief Register payload address and corresponding offset to whitelist
 * @param[in] payload payload address
 * @param[in] offset corresponding offset value
 * @retval 0 Success
 * @retval -1 Parameter error
 * @retval -2 Whitelist full
 */
int transportsdio_register_payload_offset(uint8_t *payload, uint32_t offset)
{
    int i;

    EXAMPLE_DEBUG("%s, pld:%p, offset:%d\r\n", __func__, payload, offset);
    if (!payload) {
        return -1;
    }

    /* Check if payload already exists, if so update offset */
    for (i = 0; i < FHOST_RX_BUF_CNT; i++) {
        if (offset_whitelist[i].payload_addr == payload) {
            offset_whitelist[i].offset = offset;
            return 0;
        }
    }

    /* Find free slot */
    for (i = 0; i < FHOST_RX_BUF_CNT; i++) {
        if (offset_whitelist[i].payload_addr == NULL) {
            offset_whitelist[i].payload_addr = payload;
            offset_whitelist[i].offset = offset;
            return 0;
        }
    }

    EXAMPLE_ERR("%s:%d Whitelist full %p.\r\n", __func__, __LINE__, payload);
    return -2; /* Whitelist full */
}

/**
 * @brief Unregister payload address from whitelist
 * @param[in] payload payload address to remove
 * @retval 0 Success
 * @retval -1 Parameter error
 * @retval -2 Payload not found
 */
int transportsdio_unregister_payload_offset(uint8_t *payload)
{
    int i;

    if (!payload) {
        return -1;
    }

    /* Find payload and set to NULL */
    for (i = 0; i < FHOST_RX_BUF_CNT; i++) {
        if (offset_whitelist[i].payload_addr == payload) {
            offset_whitelist[i].payload_addr = NULL;
            offset_whitelist[i].offset = 0;
            return 0;
        }
    }

    return -2; /* Payload not found */
}

/**
 * @brief Get corresponding offset by payload address
 * @param[in] payload payload address
 * @return offset value, return FRAME_BUFF_PAYLOAD_OFFSET if not found
 */
uint32_t transportsdio_get_payload_offset(uint8_t *payload)
{
    int i;

    if (!payload) {
        EXAMPLE_ERR("%s:%d payload err %p.\r\n", __func__, __LINE__, payload);
        return FRAME_BUFF_PAYLOAD_OFFSET;
    }

    /* Linear search for matching payload address */
    for (i = 0; i < FHOST_RX_BUF_CNT; i++) {
        if (offset_whitelist[i].payload_addr == payload) {
            EXAMPLE_DEBUG("%s, pld:%p, offset:%d\r\n", __func__, payload, offset_whitelist[i].offset);
            return offset_whitelist[i].offset;
        }
    }

    EXAMPLE_DEBUG("%s, pld:%p, offset:%d\r\n", __func__, payload, FRAME_BUFF_PAYLOAD_OFFSET);
    /* Return default value if not found */
    return FRAME_BUFF_PAYLOAD_OFFSET;
}

/**
 * @brief User call interface: send WiFi data to host (upld)
 * @param[in] data Data buffer to send
 * @param[in] len Data length
 * @retval 0 Success
 * @retval -1 Parameter error
 * @retval -2 Device not initialized
 * @retval -3 Memory allocation failed
 * @retval -4 WiFi not connected
 */
int transportsdio_send_upld_skb(nh_skb_t *skb)
{
    frame_elem_t *frame_elem;
    struct bflb_wifi_msg_packt *msg_tx_packt;
    int ret;
    uint8_t *payload = (uint8_t *)skb->data;

    EXAMPLE_DEBUG("%s payload:%p\r\n", __func__, payload);
    if (!g_wifi_priv) {
        EXAMPLE_ERR("WiFi device not initialized.\r\n", __func__, __LINE__);
        return -2;
    }

    if (!g_wifi_priv->wifi_connected) {
        EXAMPLE_ERR("WiFi not connected.\r\n", __func__, __LINE__);
        return -4;
    }

    /* Assign based on head */
    transportsdio_register_payload_offset(payload, FRAME_BUFF_ELEM_SIZE + sizeof(struct bflb_wifi_msg_packt));
    frame_elem   = (frame_elem_t *)(payload - FRAME_BUFF_ELEM_SIZE - sizeof(struct bflb_wifi_msg_packt));
    msg_tx_packt = (struct bflb_wifi_msg_packt *)(payload - sizeof(struct bflb_wifi_msg_packt));
    frame_elem->buff_addr = (void *)msg_tx_packt;

    /* Set callback free */
    frame_elem->skb_free = (skb_free_cb_t)skb->free_cb;
    frame_elem->skb      = skb->cb_arg;

    /* Statistics: UPLD starts to be transferred to SDIO module */
    g_nethub_stats.upld_start_consume++;
#if DEBUG_DUMP_WIFIRX_ENABLE
    EXAMPLE_INFO("upld start ================== p:%p, elem:%p, elem->buff_addr:%p, pld:%p, cb=%p\r\n",
            frame_elem->skb,
            frame_elem,
            frame_elem->buff_addr,
            payload,
            frame_elem->skb_free
            );
#endif
    /* Copy user data */
    // memcpy(msg_tx_packt->data, skb->data, skb->len);
    frame_elem->data_size = sizeof(struct bflb_wifi_msg_packt) + skb->len;
    frame_elem->buff_size = WIFI_RX_BUFF_SIZE;

    /* Set message header */
    msg_tx_packt->msg_packt.len = skb->len + sizeof(struct bflb_wifi_msg_packt) - sizeof(bflb_msg_packt_t);
    msg_tx_packt->msg_packt.tag = g_wifi_priv->msg_tag;
    msg_tx_packt->msg_packt.sub_tag = 0;
    msg_tx_packt->flag = BL_WIFI_FLAG_UPLD_DATA;

    /* Set flow control information */
    msg_tx_packt->credit_update_flag = false;
    msg_tx_packt->credit_limit_cnt = g_wifi_priv->device_dnld_credit_limit;

#if 1//ETHSTA_DBGLOG
    if (msg_tx_packt->msg_packt.len >= 14) {
        static int s_upld_cnt = 0;
        s_upld_cnt++;
        #if DEBUG_DUMP_WIFIRX_ENABLE
        EXAMPLE_INFO("Upld[%d] tag[%d] upld_cnt:<%d> <-----  %02X:%02X:%02X:%02X:%02X:%02X  %02X:%02X:%02X:%02X:%02X:%02X   %02X:%02X .... %02X:%02X\r\n",
            msg_tx_packt->msg_packt.len, msg_tx_packt->msg_packt.tag, s_upld_cnt,
            msg_tx_packt->data[0], msg_tx_packt->data[1], msg_tx_packt->data[2], msg_tx_packt->data[3],
            msg_tx_packt->data[4], msg_tx_packt->data[5], msg_tx_packt->data[6], msg_tx_packt->data[7],
            msg_tx_packt->data[8], msg_tx_packt->data[9], msg_tx_packt->data[10], msg_tx_packt->data[11],
            msg_tx_packt->data[12], msg_tx_packt->data[13],
            msg_tx_packt->data[msg_tx_packt->msg_packt.len - 2], msg_tx_packt->data[msg_tx_packt->msg_packt.len - 1]);
        #endif
    }
#endif
    /* Send data to host */
    //EXAMPLE_DEBUG("g_wifi_priv->msg_ctrl = %p\r\n", g_wifi_priv->msg_ctrl);

    ret = bflb_msg_ctrl_send(g_wifi_priv->msg_ctrl, frame_elem, 0);
    if (ret < 0) {
        EXAMPLE_ERR("\x1b[33m%s:%d\x1b[0m Failed to send upld data, ret:%d.\r\n", __func__, __LINE__, ret);
        msg_upld_send_done_customfree(frame_elem);
        return ret;
    }

    //EXAMPLE_INFO("Upld data sent: len=%lu\r\n", len);
    return 0;
}

int transportsdio_send_upld(const void *data, uint32_t len)
{
    int ret;
    frame_elem_t *frame_elem;
    struct bflb_wifi_msg_packt *msg_tx_packt;

    if (!data || len == 0 || len > WIFI_RX_BUFF_SIZE) {
        EXAMPLE_ERR("Invalid parameters: data=%p, len=%lu\r\n", data, len);
        return -1;
    }

    if (!g_wifi_priv) {
        EXAMPLE_ERR("WiFi device not initialized\r\n");
        return -2;
    }

    if (!g_wifi_priv->wifi_connected) {
        EXAMPLE_ERR("WiFi not connected\r\n");
        return -4;
    }

    ret = frame_queue_alloc_elem(g_wifi_priv->rx_frame_ctrl, &frame_elem, 1000);
    if (ret < 0) {
        EXAMPLE_ERR("Failed to allocate frame element\r\n");
        return -3;
    }

    msg_tx_packt = (struct bflb_wifi_msg_packt *)frame_elem->buff_addr;

    frame_elem->skb_free = NULL;
    frame_elem->skb = NULL;

    /* Copy user data */
    memcpy(msg_tx_packt->data, data, len);
    frame_elem->data_size = sizeof(struct bflb_wifi_msg_packt) + len;

    /* Set message header */
    msg_tx_packt->msg_packt.len = len + sizeof(struct bflb_wifi_msg_packt) - sizeof(bflb_msg_packt_t);
    msg_tx_packt->msg_packt.tag = g_wifi_priv->msg_tag;
    msg_tx_packt->msg_packt.sub_tag = 0;
    msg_tx_packt->flag = BL_WIFI_FLAG_UPLD_DATA;

    /* Set flow control information */
    msg_tx_packt->credit_update_flag = false;
    msg_tx_packt->credit_limit_cnt = g_wifi_priv->device_dnld_credit_limit;

#if 1//ETHSTA_DBGLOG
    if (msg_tx_packt->msg_packt.len >= 14) {
        EXAMPLE_PRINTF("Upld[%d] <-----  %02X:%02X:%02X:%02X:%02X:%02X  %02X:%02X:%02X:%02X:%02X:%02X   %02X:%02X .... %02X:%02X\r\n",
            msg_tx_packt->msg_packt.len,
            msg_tx_packt->data[0], msg_tx_packt->data[1], msg_tx_packt->data[2], msg_tx_packt->data[3],
            msg_tx_packt->data[4], msg_tx_packt->data[5], msg_tx_packt->data[6], msg_tx_packt->data[7],
            msg_tx_packt->data[8], msg_tx_packt->data[9], msg_tx_packt->data[10], msg_tx_packt->data[11],
            msg_tx_packt->data[12], msg_tx_packt->data[13],
            msg_tx_packt->data[msg_tx_packt->msg_packt.len - 2], msg_tx_packt->data[msg_tx_packt->msg_packt.len - 1]);
    }
#endif
    /* Send data to host */
    ret = bflb_msg_ctrl_send(g_wifi_priv->msg_ctrl, frame_elem, 0);
    if (ret < 0) {
        EXAMPLE_ERR("Failed to send upld data: %d\r\n", ret);
        frame_queue_free_elem(frame_elem);
        return ret;
    }

    //EXAMPLE_INFO("Upld data sent: len=%lu\r\n", len);
    return 0;
}

#ifdef CONFIG_NETHUB_DEBUG
/**
 * @brief Dump g_wifi_priv structure parameters for debugging
 * @details Print important parameters including queue status, connection state,
 *          flow control status and other key information
 */
void transportsdio_dump_priv(void)
{
    if (!g_wifi_priv) {
        EXAMPLE_PRINTF("g_wifi_priv is NULL - device not initialized\r\n");
        return;
    }

    EXAMPLE_PRINTF("=== WiFi Private Structure Dump ===\r\n");

    /* Basic information */
    EXAMPLE_PRINTF("Basic Info:\r\n");
    EXAMPLE_PRINTF("  msg_ctrl: %p\r\n", g_wifi_priv->msg_ctrl);
    EXAMPLE_PRINTF("  msg_tag: %d\r\n", g_wifi_priv->msg_tag);
    EXAMPLE_PRINTF("  wifi_status: %d\r\n", g_wifi_priv->wifi_status);
    EXAMPLE_PRINTF("  stop_requested: %s\r\n", g_wifi_priv->stop_requested ? "true" : "false");

    /* MAC address and connection status */
    EXAMPLE_PRINTF("MAC & Connection:\r\n");
    EXAMPLE_PRINTF("  mac_addr: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
          g_wifi_priv->mac_addr[0], g_wifi_priv->mac_addr[1], g_wifi_priv->mac_addr[2],
          g_wifi_priv->mac_addr[3], g_wifi_priv->mac_addr[4], g_wifi_priv->mac_addr[5]);
    EXAMPLE_PRINTF("  wifi_connected: %s\r\n", g_wifi_priv->wifi_connected ? "connected" : "disconnected");

    /* Flow control information */
    EXAMPLE_PRINTF("Flow Control:\r\n");
    EXAMPLE_PRINTF("  flow_control_enabled: %s\r\n", g_wifi_priv->flow_control_enabled ? "enabled" : "disabled");
    EXAMPLE_PRINTF("  flow_control_threshold: %d\r\n", g_wifi_priv->flow_control_threshold);
    EXAMPLE_PRINTF("  device_dnld_credit_limit: %d\r\n", g_wifi_priv->device_dnld_credit_limit);
    EXAMPLE_PRINTF("  credit_limit_update_last: %d\r\n", g_wifi_priv->credit_limit_update_last);
    EXAMPLE_PRINTF("  credit_cnt: %d\r\n", g_wifi_priv->credit_cnt);
    EXAMPLE_PRINTF("  host_upld_credit_limit: %d\r\n", g_wifi_priv->host_upld_credit_limit);
    EXAMPLE_PRINTF("  device_upld_credit_consumed: %d\r\n", g_wifi_priv->device_upld_credit_consumed);

    /* Task information */
    EXAMPLE_PRINTF("Tasks:\r\n");
    EXAMPLE_PRINTF("  wifi_proc_task: %p", g_wifi_priv->wifi_proc_task);
    if (g_wifi_priv->wifi_proc_task) {
        EXAMPLE_PRINTF(" (state: %d)",
              eTaskGetState(g_wifi_priv->wifi_proc_task));
#if (INCLUDE_uxTaskGetStackHighWaterMark == 1)
        EXAMPLE_PRINTF(", stack_high_water_mark: %u",
              uxTaskGetStackHighWaterMark(g_wifi_priv->wifi_proc_task));
#endif
    }
    EXAMPLE_PRINTF("\r\n");

    EXAMPLE_PRINTF("  frame_free_task: %p", g_wifi_priv->frame_free_task);
    if (g_wifi_priv->frame_free_task) {
        EXAMPLE_PRINTF(" (state: %d)",
              eTaskGetState(g_wifi_priv->frame_free_task));
#if (INCLUDE_uxTaskGetStackHighWaterMark == 1)
        EXAMPLE_PRINTF(", stack_high_water_mark: %u",
              uxTaskGetStackHighWaterMark(g_wifi_priv->frame_free_task));
#endif
    }
    EXAMPLE_PRINTF("\r\n");

    /* Queue information */
    EXAMPLE_PRINTF("Queues:\r\n");
    if (g_wifi_priv->msg_dnld_queue) {
        UBaseType_t msg_dnld_count = uxQueueMessagesWaiting(g_wifi_priv->msg_dnld_queue);
        UBaseType_t msg_dnld_spaces = uxQueueSpacesAvailable(g_wifi_priv->msg_dnld_queue);
        EXAMPLE_PRINTF("  msg_dnld_queue: %p (used: %lu/%u, free: %lu)\r\n",
              g_wifi_priv->msg_dnld_queue, msg_dnld_count, WIFI_MSG_QUEUE_DEPTH, msg_dnld_spaces);
    } else {
        EXAMPLE_PRINTF("  msg_dnld_queue: NULL\r\n");
    }

    if (g_wifi_priv->frame_free_queue) {
        UBaseType_t frame_free_count = uxQueueMessagesWaiting(g_wifi_priv->frame_free_queue);
        UBaseType_t frame_free_spaces = uxQueueSpacesAvailable(g_wifi_priv->frame_free_queue);
        EXAMPLE_PRINTF("  frame_free_queue: %p (used: %lu/%u, free: %lu)\r\n",
              g_wifi_priv->frame_free_queue, frame_free_count, WIFI_MSG_QUEUE_DEPTH, frame_free_spaces);
    } else {
        EXAMPLE_PRINTF("  frame_free_queue: NULL\r\n");
    }

    /* Frame buffer information */
    EXAMPLE_PRINTF("Frame Buffer:\r\n");
    EXAMPLE_PRINTF("  rx_frame_ctrl: %p\r\n", g_wifi_priv->rx_frame_ctrl);
    if (g_wifi_priv->rx_frame_ctrl) {
        /* More information can be added here according to frame_queue_ctrl_t structure */
        EXAMPLE_PRINTF("  rx_frame_ctrl is valid\r\n");
    }

    /* Callback function information */
    EXAMPLE_PRINTF("Callbacks:\r\n");
    EXAMPLE_PRINTF("  wifi_send_frame: %p\r\n", g_wifi_priv->wifi_send_frame);
    EXAMPLE_PRINTF("  wifi_recv_frame: %p\r\n", g_wifi_priv->wifi_recv_frame);

    EXAMPLE_PRINTF("=== End of WiFi Private Structure Dump ===\r\n");
}
#endif

#ifdef CONFIG_SDIO3_SOFT_RST_INT_USER
static void _sdio3_reinit(void)
{
    extern struct bflb_device_s *sdio3_hd;
    struct bflb_sdio3_config_s cfg = {
        .func_num = 1,                            /*!< function num: 1~2. */
        .ocr = 0xff0000,                                           /*!< OCR, [14:0]:res, [23:15]:2.7v~3.6v */
        .cap_flag = SDIO3_CAP_FLAG_SDR50 | SDIO3_CAP_FLAG_RD_WAIT, /*!< capability flag */
        .func1_dnld_size_max = (512*3),                /* dnld max size */
        .func2_dnld_size_max = (512*3),                /* dnld max size */
    };

    /* Initialize SDIO3 */
    EXAMPLE_PRINTF("bflb_sdio3_init\r\n");
    bflb_sdio3_init(sdio3_hd, &cfg);

    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_INIT_READY, 0);
    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_SET_FUNC_CARD_READY, 1);
}
void transportsdio3_reset_handler_isr(void *arg)
{
    extern uint32_t transportsdio_get_payload_offset(uint8_t *payload);
    extern volatile bool sdio3_ready_flag;
    extern struct bflb_device_s *sdio3_hd;
    bflb_msg_ctrl_t *msg_ctrl;
    frame_elem_t *frame_elem;
    int offset;

    printf("SDIO hotplug detected, this feature is not yet supported\r\n");
    //while(1);

    sdio3_ready_flag = false;
    bflb_msg_host_reset_cb();

    {
        bflb_sdio3_trans_desc_t trans_desc;

        while (bflb_sdio3_dnld_pop(sdio3_hd, &trans_desc, 1) == 0) {
            msg_ctrl = (bflb_msg_ctrl_t *)trans_desc.user_arg;
            frame_elem = (frame_elem_t *)((uintptr_t)trans_desc.buff - FRAME_BUFF_MSGSTRUCT_OFFSET);

            EXAMPLE_PRINTF("dnld pop:%d\r\n", FRAME_BUFF_MSGSTRUCT_OFFSET);
            frame_elem->data_size = 0;
            bflb_msg_dnld_recv_done_cb(msg_ctrl, frame_elem, false);
        }
        while (bflb_sdio3_upld_pop(sdio3_hd, &trans_desc, 1) == 0) {
            msg_ctrl = (bflb_msg_ctrl_t *)trans_desc.user_arg;
            offset = transportsdio_get_payload_offset((uint8_t *)trans_desc.buff+FRAME_BUFF_MSGSTRUCT);
            EXAMPLE_PRINTF("upld pop offset:%d\r\n", offset);
            frame_elem = (frame_elem_t *)((uintptr_t)trans_desc.buff - offset + FRAME_BUFF_MSGSTRUCT);

            bflb_msg_upld_send_done_cb(msg_ctrl, frame_elem, true);
        }
    }

    /* Reset SDIO3 queue control */
    bflb_sdio3_deinit(sdio3_hd);

    /* Re-initialize SDIO3 */
    _sdio3_reinit();
}
#endif

