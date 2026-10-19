/**
 * @file mr_speed_test.h
 * @brief Speed test instance header file for SDIO message throughput testing
 */

#ifndef __MR_SPEED_TEST_H__
#define __MR_SPEED_TEST_H__

#include "stdint.h"
#include "mr_msg_ctrl.h"
#include "mr_frame_buff_ctrl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/**< Enable zero-copy loopback test mode */
#define SPEED_TEST_ZEROCOPY_LOOPBACK (0)

/** Statistics output interval in milliseconds */
#define SPEED_TEST_STATS_INTERVAL_MS (1000)
/** Receive queue depth */
#define SPEED_TEST_QUEUE_DEPTH       (64)
/** Task stack size in bytes */
#define SPEED_TEST_TASK_STACK_SIZE   (512)
/** Task priority */
#define SPEED_TEST_TASK_PRIORITY     (10)

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

/**
 * @brief Speed test statistics structure
 */
typedef struct {
    uint32_t rx_packets;      /**< Number of received packets */
    uint32_t tx_packets;      /**< Number of transmitted packets */
    uint32_t rx_bytes;        /**< Number of received bytes */
    uint32_t tx_bytes;        /**< Number of transmitted bytes */
    uint32_t rx_errors;       /**< Number of receive errors */
    uint32_t tx_errors;       /**< Number of transmit errors */
    uint64_t last_stats_time; /**< Last statistics time */
} speed_test_stats_t;

/**
 * @brief Speed test instance structure
 */
typedef struct {
    uint8_t msg_tag;          /**< Message tag */
    TaskHandle_t task_handle; /**< Task handle */
    QueueHandle_t rx_queue;   /**< Receive queue */

    mr_msg_ctrl_priv_t *msg_ctrl;      /**< Message controller */
    mr_frame_queue_ctrl_t *frame_pool; /**< Shared memory pool */

    bool stop_requested;             /**< Stop request flag */
    SemaphoreHandle_t stop_done_sem; /**< Instance stop completion semaphore */

    speed_test_stats_t stats; /**< Statistics information */

    char name[32]; /**< Instance name */
} speed_test_instance_t;

/*****************************************************************************
 * Function Declarations
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
                                         mr_frame_queue_ctrl_t *frame_pool, uint8_t msg_tag);

/**
 * @brief Destroy speed test instance
 * @param instance Speed test instance to destroy
 * @retval 0 Success
 * @retval -1 Error
 */
int speed_test_destroy(speed_test_instance_t *instance);

#ifdef __cplusplus
}
#endif

#endif /* __MR_SPEED_TEST_H__ */