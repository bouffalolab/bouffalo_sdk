/**
 * @file mr_speed_test.c
 * @brief Speed test instance implementation for SDIO message throughput testing
 */

#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_sdio2.h"

#include "mm.h"
#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>
#include <stdio.h>

#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "mr_speed_test.h"

#define DBG_TAG "SPEED_TEST"
#include "log.h"

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Speed test message receive callback function
 * @param frame_elem Received frame element
 * @param arg Callback argument (speed test instance)
 * @retval 0 Self-handled (keep frame ownership)
 * @retval -1 Return frame control to message controller
 * @note This function is called from message controller when data arrives
 */
static int speed_test_msg_recv_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    speed_test_instance_t *instance = (speed_test_instance_t *)arg;

    /* Validate parameters */
    if (instance == NULL || frame_elem == NULL) {
        LOG_E("Invalid parameters in speed test callback\r\n");
        return -1;
    }

    /* Check if instance is being stopped */
    if (instance->stop_requested) {
        return -1;
    }

    /* Put received data into queue */
    if (mr_frame_queue_send(instance->rx_queue, &frame_elem, 0) != 0) {
        LOG_E("Instance %s rx queue full, drop packet\r\n", instance->name);
        instance->stats.rx_errors++;
        return -1;
    }

    instance->stats.rx_packets++;
    instance->stats.rx_bytes += frame_elem->data_size;

    if (instance->task_handle != NULL) {
        xTaskNotifyGive(instance->task_handle);
    }

    return 0;
}

/**
 * @brief Speed test message send done callback function
 * @param frame_elem Sent frame element
 * @param arg Callback argument (speed test instance)
 * @retval 0 Self-handled (frame freed by this callback)
 * @retval -1 Return frame control to message controller for cleanup
 * @note This function is called from message controller when send is completed.
 *       The callback takes ownership and frees the frame after updating statistics.
 */
static int speed_test_msg_send_done_cb(mr_frame_elem_t *frame_elem, void *arg)
{
    speed_test_instance_t *instance = (speed_test_instance_t *)arg;

    if (instance == NULL || frame_elem == NULL) {
        LOG_E("Invalid parameters in speed test send done callback\r\n");
        return -1;
    }

    /* Update send statistics - send was successful */
    instance->stats.tx_packets++;
    instance->stats.tx_bytes += frame_elem->data_size;

#if !SPEED_TEST_ZEROCOPY_LOOPBACK
    /* Free the frame element back to the pool */
    mr_frame_queue_free_elem(frame_elem);
#else
    /* Free the frame element back to the message controller */
    mr_msg_ctrl_dnld_free(instance->msg_ctrl, frame_elem);
#endif

    return 0;
}

/**
 * @brief Speed test task main function
 * @param arg Task argument (speed test instance)
 * @retval None
 * @note This is the main processing loop for speed test instance
 */
static void speed_test_task(void *arg)
{
    speed_test_instance_t *instance = (speed_test_instance_t *)arg;
    mr_frame_elem_t *rx_frame = NULL;
    mr_frame_elem_t *tx_frame = NULL;
    uint64_t current_time = 0;
    uint64_t last_stats_time = 0;
    bool queue_empty_flag = false;
    int ret = 0;

    LOG_I("Speed test task %s started\r\n", instance->name);

    /* Register message callbacks using new unified interface */
    ret = mr_msg_cb_register(instance->msg_ctrl, instance->msg_tag, speed_test_msg_recv_cb,
                             instance,                              /* dnld_recv callback */
                             speed_test_msg_send_done_cb, instance, /* upld_send callback */
                             NULL, NULL);                           /* hw_reset callback (not used) */

    if (ret != 0) {
        LOG_E("Failed to register msg callback for tag 0x%02X\r\n", instance->msg_tag);
        goto task_exit;
    }

    last_stats_time = bflb_mtimer_get_time_ms();

    /* Main processing loop, check if stop is requested */
    while (instance->stop_requested == false) {
        /* Try to get data from queue */
        if (mr_frame_queue_receive(instance->rx_queue, &rx_frame, 0) < 0) {
            queue_empty_flag = true;
            goto stats_output;
        }
        queue_empty_flag = false;

#if SPEED_TEST_ZEROCOPY_LOOPBACK
        /* In zero-copy loopback mode, directly use received frame for sending */
        tx_frame = rx_frame;
#else
        /* Try to allocate a free memory block from pool */
        ret = mr_frame_queue_alloc_elem(instance->frame_pool, &tx_frame, pdMS_TO_TICKS(10));
        if (ret < 0) {
            LOG_E("Failed to allocate tx frame\r\n");
            instance->stats.tx_errors++;
            mr_msg_ctrl_dnld_free(instance->msg_ctrl, rx_frame);
            goto stats_output;
        }

        /* Check data length to prevent buffer overflow */
        if (tx_frame->buff_size < rx_frame->data_size) {
            LOG_E("RX msg len (%d) too large for tx buffer (%d)\r\n", rx_frame->data_size,
                  tx_frame->buff_size - sizeof(mr_msg_t));
            instance->stats.tx_errors++;
            mr_msg_ctrl_dnld_free(instance->msg_ctrl, rx_frame);
            mr_frame_queue_free_elem(tx_frame);
            goto stats_output;
        }

        /* Copy data to memory block */
        memcpy(tx_frame->buff_addr, rx_frame->buff_addr, rx_frame->data_size);
        tx_frame->data_size = rx_frame->data_size;

        /* Free received data */
        mr_msg_ctrl_dnld_free(instance->msg_ctrl, rx_frame);
#endif

        /* Send memory block */
        ret = mr_msg_ctrl_upld_send(instance->msg_ctrl, tx_frame);
        if (ret != 0) {
            LOG_E("Failed to send msg\r\n");
            instance->stats.tx_errors++;
#if SPEED_TEST_ZEROCOPY_LOOPBACK
            /* In zero-copy mode, tx_frame is from msg_ctrl dnld buffer */
            mr_msg_ctrl_dnld_free(instance->msg_ctrl, tx_frame);
#else
            /* In copy mode, tx_frame is from frame_pool */
            mr_frame_queue_free_elem(tx_frame);
#endif
            goto stats_output;
        }

    stats_output:

        /* Check statistics output time */
        current_time = bflb_mtimer_get_time_ms();
        if (current_time - last_stats_time >= SPEED_TEST_STATS_INTERVAL_MS) {
            uint32_t time_diff = (uint32_t)(current_time - last_stats_time);
            uint32_t rx_kbps = 0;
            uint32_t tx_kbps = 0;

            /* Only print statistics if there is activity */
            if (instance->stats.rx_packets > 0 || instance->stats.tx_packets > 0 || instance->stats.rx_errors > 0 ||
                instance->stats.tx_errors > 0) {
                /* Prevent division by zero */
                if (time_diff > 0) {
                    rx_kbps = (uint64_t)instance->stats.rx_bytes * 1000 / time_diff / 1024;
                    tx_kbps = (uint64_t)instance->stats.tx_bytes * 1000 / time_diff / 1024;
                }

                uint32_t total_packets = instance->stats.rx_packets + instance->stats.tx_packets;
                uint32_t total_bytes = instance->stats.rx_bytes + instance->stats.tx_bytes;
                uint32_t total_kbps = rx_kbps + tx_kbps;

                LOG_I("========== [\033[33m%s\033[0m] Performance Statistics =========\r\n", instance->name);
                LOG_I("  RX: %6lu packets | %8lu bytes | %8lu KB/s\r\n", instance->stats.rx_packets,
                      instance->stats.rx_bytes, rx_kbps);
                LOG_I("  TX: %6lu packets | %8lu bytes | %8lu KB/s\r\n", instance->stats.tx_packets,
                      instance->stats.tx_bytes, tx_kbps);
                LOG_I("  TOT:%6lu packets | %8lu bytes | %8lu KB/s\r\n", total_packets, total_bytes, total_kbps);
                if (instance->stats.rx_errors > 0 || instance->stats.tx_errors > 0) {
                    LOG_I("  ERR: RX=%lu, TX=%lu\r\n", instance->stats.rx_errors, instance->stats.tx_errors);
                }
                LOG_I("===========================================================\r\n");
            }

            /* Reset statistics counters */
            memset(&instance->stats, 0, sizeof(instance->stats));
            last_stats_time = current_time;
        }

        /* If queue is empty, wait for notification */
        if (queue_empty_flag) {
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(SPEED_TEST_STATS_INTERVAL_MS / 10));
        }
    }

task_exit:

    /* Unregister message callback using new unified interface */
    mr_msg_cb_unregister(instance->msg_ctrl, instance->msg_tag);

    /* Clean up data in receive queue */
    /* Note: Both modes have msg_ctrl dnld buffers in rx_queue */
    while (mr_frame_queue_receive(instance->rx_queue, &rx_frame, 0) == 0) {
        mr_msg_ctrl_dnld_free(instance->msg_ctrl, rx_frame);
    }

    LOG_I("Speed test task %s stopped\r\n", instance->name);

    /* Signal completion */
    xSemaphoreGive(instance->stop_done_sem);
    /* Suspend waiting for deletion */
    vTaskSuspend(NULL);
}

/*****************************************************************************
 * Public Functions
 *****************************************************************************/

/**
 * @brief Create speed test instance
 * @param name Instance name
 * @param msg_ctrl Message controller
 * @param frame_pool Frame buffer pool
 * @param msg_tag Message tag for this instance
 * @retval Pointer to created instance on success
 * @retval NULL on failure
 */
speed_test_instance_t *speed_test_create(const char *name, mr_msg_ctrl_priv_t *msg_ctrl,
                                         mr_frame_queue_ctrl_t *frame_pool, uint8_t msg_tag)
{
    speed_test_instance_t *instance = NULL;

    /* Validate input parameters */
    if (name == NULL || msg_ctrl == NULL) {
        LOG_E("Invalid parameters for speed test create\r\n");
        return NULL;
    }

#if !SPEED_TEST_ZEROCOPY_LOOPBACK
    if (frame_pool == NULL) {
        LOG_E("Frame pool is NULL in non-zero-copy mode\r\n");
        return NULL;
    }
#endif

    if (msg_tag >= MR_MSG_TAG_MAX) {
        LOG_E("Invalid msg tag: 0x%02X\r\n", msg_tag);
        return NULL;
    }

    /* Allocate instance structure */
    instance = (speed_test_instance_t *)kmalloc(sizeof(speed_test_instance_t), 0);
    if (instance == NULL) {
        LOG_E("Failed to allocate instance memory\r\n");
        return NULL;
    }

    memset(instance, 0, sizeof(speed_test_instance_t));

    /* Initialize instance parameters */
    instance->msg_tag = msg_tag;
    instance->msg_ctrl = msg_ctrl;
    instance->frame_pool = frame_pool;
    instance->stop_requested = false;

    strncpy(instance->name, name, sizeof(instance->name) - 1);

    /* Create receive queue */
    instance->rx_queue = xQueueCreate(SPEED_TEST_QUEUE_DEPTH, sizeof(mr_frame_elem_t *));
    if (instance->rx_queue == NULL) {
        LOG_E("Failed to create rx queue\r\n");
        goto create_error;
    }

    /* Create task stop completion semaphore */
    instance->stop_done_sem = xSemaphoreCreateBinary();
    if (instance->stop_done_sem == NULL) {
        LOG_E("Failed to create instance stop done semaphore\r\n");
        goto create_error;
    }

    /* Create task */
    BaseType_t ret = xTaskCreate(speed_test_task, instance->name, SPEED_TEST_TASK_STACK_SIZE, instance,
                                 SPEED_TEST_TASK_PRIORITY, &instance->task_handle);
    if (ret != pdPASS) {
        LOG_E("Failed to create speed test task\r\n");
        goto create_error;
    }

    LOG_I("Speed test instance %s created successfully\r\n", instance->name);
    return instance;

create_error:
    if (instance->rx_queue != NULL) {
        vQueueDelete(instance->rx_queue);
    }
    if (instance->stop_done_sem != NULL) {
        vSemaphoreDelete(instance->stop_done_sem);
    }
    kfree(instance);
    return NULL;
}

/**
 * @brief Destroy speed test instance
 * @param instance Speed test instance to destroy
 * @retval 0 Success
 * @retval -1 Error
 */
int speed_test_destroy(speed_test_instance_t *instance)
{
    /* Validate input parameter */
    if (instance == NULL) {
        LOG_E("Invalid instance pointer\r\n");
        return -1;
    }

    LOG_I("Destroying speed test instance %s\r\n", instance->name);

    /* Set stop request flag */
    instance->stop_requested = true;
    /* Wake up task to execute exit */
    if (instance->task_handle != NULL) {
        xTaskNotifyGive(instance->task_handle);
    }

    /* Wait for task stop completion */
    if (xSemaphoreTake(instance->stop_done_sem, pdMS_TO_TICKS(1000)) != pdTRUE) {
        LOG_E("Timeout waiting for instance stop done\r\n");
        return -1;
    }
    /* Delete task */
    vTaskDelete(instance->task_handle);

    /* Delete queue and stop semaphore */
    vQueueDelete(instance->rx_queue);
    vSemaphoreDelete(instance->stop_done_sem);

    /* Free instance structure */
    kfree(instance);

    LOG_I("Speed test instance destroyed successfully\r\n");
    return 0;
}
