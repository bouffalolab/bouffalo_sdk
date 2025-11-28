
/**
 * @file hd_speed_test.h
 * @brief Host Driver speed test module header definitions
 * @details This file contains speed test structures and interfaces for measuring
 *          SDIO data transfer performance between host and device
 * @author mlwang
 * @date 2025-07-17
 * @version 1.0
 */

#ifndef __HD_SPEED_TEST_H__
#define __HD_SPEED_TEST_H__
#include <linux/kernel.h>

#include "hd_msg_ctrl.h"

/** @defgroup SPEED_TEST_CONFIG Speed Test Configuration
 * @{
 */
/** Number of test buffers and transfers per test round */
#define HD_SPEED_TEST_BUFF_CNT (20)
/** Number of test rounds to perform */
#define HD_SPEED_TEST_CNT      (1000)
/** @} */

/**
 * @struct hd_speed_test
 * @brief Speed test control structure
 * @details Structure for managing speed test operations and statistics
 */
struct hd_speed_test {
    /** @name Core Components
     * @{
     */
    struct hd_msg_ctrl *msg_ctrl; /**< Message control interface */
    struct task_struct *thread;   /**< Kernel thread for speed test */
    bool thread_mode;             /**< Threading mode flag */
    uint8_t msg_tag;              /**< Message tag for routing */
    /** @} */

    /** @name Test Buffers
     * @{
     */
    uint8_t *dnld_test_buff[HD_SPEED_TEST_BUFF_CNT]; /**< Download test buffers */
    uint8_t *upld_test_buff[HD_SPEED_TEST_BUFF_CNT]; /**< Upload test buffers */
    /** @} */

    /** @name Counters
     * @{
     */
    int dnld_cnt; /**< Download transfer counter */
    int upld_cnt; /**< Upload transfer counter */
    /** @} */

    /** @name Buffer Sizes
     * @{
     */
    int dnld_max_size; /**< Maximum download buffer size */
    int upld_max_size; /**< Maximum upload buffer size */
    /** @} */

    /** @name Statistics
     * @{
     */
    int dnld_total_size;   /**< Total download data size */
    int upld_total_size;   /**< Total upload data size */
    spinlock_t stats_lock; /**< Statistics protection lock */
    /** @} */
};

/** @defgroup SPEED_TEST_API Speed Test API Functions
 * @{
 */

/**
 * @brief Run speed test
 * @param[in] msg_ctrl Message control interface
 * @param[in] msg_tag Message tag for routing
 * @param[in] thread_mode Enable threading mode
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_speed_test_run(struct hd_msg_ctrl *msg_ctrl, uint8_t msg_tag, bool thread_mode);

/**
 * @brief Destroy speed test thread for specific message tag
 * @param[in] msg_tag Message tag to destroy
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_speed_test_thread_destroy(uint8_t msg_tag);

/**
 * @brief Destroy all speed test threads
 * @retval 0 Success
 * @retval <0 Error code
 */
int hd_speed_test_thread_destroy_all(void);

/** @} */

#endif
