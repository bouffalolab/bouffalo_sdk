/**
 * @file mr_frame_buff_ctrl.h
 * @brief Frame buffer controller header file for memory pool management
 */

#ifndef __STREAM_QUEUE_CTRL_H__
#define __STREAM_QUEUE_CTRL_H__

#include "stdint.h"

#include <FreeRTOS.h>
#include <queue.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/** Frame buffer header room size in bytes */
#ifdef CONFIG_MR_FRAME_BUFF_CTRL_HEADER_ROOM
#define FRAME_BUFF_HEADER_ROOM CONFIG_MR_FRAME_BUFF_CTRL_HEADER_ROOM
#else
#define FRAME_BUFF_HEADER_ROOM 32
#endif

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

typedef struct frame_elem_s mr_frame_elem_t;
typedef struct mr_frame_queue_ctrl_s mr_frame_queue_ctrl_t;

/**
 * @brief Frame queue free callback function type
 * @param frame_elem Frame element being freed
 * @param arg Callback argument
 * @retval None
 */
typedef void (*mr_frame_queue_free_cb_t)(mr_frame_elem_t *frame_elem, void *arg);

/**
 * @brief Frame queue element structure
 * @note sizeof(mr_frame_elem_t) must be < FRAME_BUFF_HEADER_ROOM
 */
struct frame_elem_s {
    /**** Fields that cannot be modified ****/
    uint16_t frame_type;               /**< Frame type identifier */
    uint16_t frame_id;                 /**< Frame ID */
    mr_frame_queue_ctrl_t *frame_ctrl; /**< Frame controller pointer */

    void *buff_addr;    /**< Buffer address */
    uint16_t buff_size; /**< Buffer size */

    /**** Fields that can be modified ****/
    uint16_t data_size; /**< Actual data size */

    uint8_t ex_data[0]; /**< Extra data for custom use */
};

/**
 * @brief Frame queue controller structure
 */
struct mr_frame_queue_ctrl_s {
    uint16_t frame_type;          /**< Frame type */
    uint16_t frame_elem_cnt;      /**< Frame element count */
    uint16_t frame_elem_size;     /**< Frame element size */
    uint16_t frame_elem_hdr_size; /**< Frame element header size */

    mr_frame_queue_free_cb_t before_free_cb; /**< Before free callback */
    void *before_free_cb_arg;                /**< Before free callback argument */

    mr_frame_queue_free_cb_t after_free_cb; /**< After free callback */
    void *after_free_cb_arg;                /**< After free callback argument */

    QueueHandle_t pool_queue;   /**< Pool queue for memory block management */
    QueueHandle_t output_queue; /**< Output queue for data block FIFO */
    QueueHandle_t input_queue;  /**< Input queue for data block FIFO */

    void *frame_buff; /**< Frame buffer pointer */
    char *name;       /**< Controller name */
};

/**
 * @brief Frame queue controller initialization configuration
 */
typedef struct {
    void *frame_buff;         /**< Frame buffer pointer */
    uint16_t frame_type;      /**< Frame type */
    uint16_t frame_elem_cnt;  /**< Frame element count */
    uint16_t frame_elem_size; /**< Frame element size */

    char *name; /**< Controller name */
} mr_frame_queue_ctrl_init_cfg_t;

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Create frame queue controller
 * @param cfg Configuration structure
 * @retval Pointer to frame queue controller on success
 * @retval NULL on failure
 */
mr_frame_queue_ctrl_t *mr_frame_queue_create(mr_frame_queue_ctrl_init_cfg_t *cfg);

/**
 * @brief Remove frame queue controller
 * @param ctrl Frame queue controller
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_remove(mr_frame_queue_ctrl_t *ctrl);

/**
 * @brief Register frame queue free callbacks
 * @param ctrl Frame queue controller
 * @param before_free_cb Before free callback
 * @param before_free_cb_arg Before free callback argument
 * @param after_free_cb After free callback
 * @param after_free_cb_arg After free callback argument
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_free_cb_register(mr_frame_queue_ctrl_t *ctrl,
                                    mr_frame_queue_free_cb_t before_free_cb, void *before_free_cb_arg,
                                    mr_frame_queue_free_cb_t after_free_cb, void *after_free_cb_arg);

/**
 * @brief Send item to queue (supports ISR)
 * @param xQueue Queue handle
 * @param pvItemToQueue Item to queue
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_send(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait);

/**
 * @brief Receive item from queue (supports ISR)
 * @param xQueue Queue handle
 * @param pvBuffer Buffer to store item
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_receive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);

/**
 * @brief Get number of messages waiting in queue
 * @param xQueue Queue handle
 * @retval Number of messages waiting
 */
int mr_frame_queue_messages_waiting(QueueHandle_t xQueue);

/**
 * @brief Check if queue is full
 * @param xQueue Queue handle
 * @retval 0 Queue is not full
 * @retval 1 Queue is full
 */
int mr_frame_queue_is_full(QueueHandle_t xQueue);

/**
 * @brief Allocate frame element from pool
 * @param ctrl Frame queue controller
 * @param frame_elem Pointer to store allocated frame element
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_alloc_elem(mr_frame_queue_ctrl_t *ctrl, mr_frame_elem_t **frame_elem, TickType_t xTicksToWait);

/**
 * @brief Free frame element back to pool
 * @param frame_elem Frame element to free
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_free_elem(mr_frame_elem_t *frame_elem);

#ifdef __cplusplus
}
#endif

#endif /* __STREAM_QUEUE_CTRL_H__ */
