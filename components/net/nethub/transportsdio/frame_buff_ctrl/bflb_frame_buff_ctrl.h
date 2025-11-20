/**
 * @file bflb_frame_buff_ctrl.h
 * @brief Frame buffer controller header file for memory pool management
 * @author mlwang
 * @date 2025-07-17
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
#define FRAME_BUFF_ELEM_OFFSET      (0)
#define FRAME_BUFF_ELEM_SIZE        (32)
#define FRAME_BUFF_PBUFCUSTOM       (32)
#define FRAME_BUFF_HWDESCTAG        (388)
#define FRAME_BUFF_MSGSTRUCT_OFFSET (FRAME_BUFF_ELEM_SIZE+FRAME_BUFF_PBUFCUSTOM+FRAME_BUFF_HWDESCTAG)// 452
#define FRAME_BUFF_MSGSTRUCT        (8)
#define FRAME_BUFF_PAYLOAD_OFFSET   (FRAME_BUFF_MSGSTRUCT_OFFSET+FRAME_BUFF_MSGSTRUCT)               // 460
#define FRAME_BUFF_PAYLOAD          (1536)
#define FRAME_BUFF_TOTALSIZE        (FRAME_BUFF_PAYLOAD_OFFSET+FRAME_BUFF_PAYLOAD)                      // 1996

/*****************************************************************************
 * Type Definitions
 *****************************************************************************/

typedef struct frame_elem_t frame_elem_t;
typedef struct frame_queue_ctrl_t frame_queue_ctrl_t;

/**
 * @brief Frame queue free callback function type
 * @param frame_elem Frame element being freed
 * @param arg Callback argument
 * @retval None
 */
typedef void (*frame_queue_free_cb_t)(frame_elem_t *frame_elem, void *arg);

typedef int (*skb_free_cb_t)(void* data);
/**
 * @brief Frame queue element structure
 * @note sizeof(frame_elem_t) must be < FRAME_BUFF_PAYLOAD_OFFSET
 */
struct frame_elem_t {
    /**** Fields that cannot be modified ****/
    uint16_t frame_type;                    /**< Frame type identifier */
    uint16_t frame_id;                      /**< Frame ID */
    frame_queue_ctrl_t *frame_ctrl;         /**< Frame controller pointer */

    void *buff_addr;                        /**< Buffer address */
    uint16_t buff_size;                     /**< Buffer size */

    /**** Fields that can be modified ****/
    uint16_t data_size;                     /**< Actual data size */
    skb_free_cb_t skb_free;
    void *skb;

    uint8_t ex_data[0];                     /**< Extra data for custom use */
};

/**
 * @brief Frame queue controller structure
 */
struct frame_queue_ctrl_t {
    uint16_t frame_type;                    /**< Frame type */
    uint16_t frame_elem_cnt;                /**< Frame element count */
    uint16_t frame_elem_size;               /**< Frame element size */

    frame_queue_free_cb_t before_free_cb;   /**< Before free callback */
    void *before_free_cb_arg;               /**< Before free callback argument */

    frame_queue_free_cb_t after_free_cb;    /**< After free callback */
    void *after_free_cb_arg;                /**< After free callback argument */

    QueueHandle_t pool_queue;               /**< Pool queue for memory block management */
    QueueHandle_t output_queue;             /**< Output queue for data block FIFO */
    QueueHandle_t input_queue;              /**< Input queue for data block FIFO */

    void *frame_buff;                       /**< Frame buffer pointer */
    char *name;                             /**< Controller name */
};

/**
 * @brief Frame queue controller initialization configuration
 */
typedef struct {
    void *frame_buff;                       /**< Frame buffer pointer */
    uint16_t frame_type;                    /**< Frame type */
    uint16_t frame_elem_cnt;                /**< Frame element count */
    uint16_t frame_elem_size;               /**< Frame element size */

    char *name;                             /**< Controller name */
} frame_queue_ctrl_init_cfg_t;

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Create frame queue controller
 * @param cfg Configuration structure
 * @retval Pointer to frame queue controller on success
 * @retval NULL on failure
 */
frame_queue_ctrl_t *frame_queue_create(frame_queue_ctrl_init_cfg_t *cfg);

/**
 * @brief Remove frame queue controller
 * @param ctrl Frame queue controller
 * @retval 0 Success
 * @retval -1 Error
 */
int frame_queue_remove(frame_queue_ctrl_t *ctrl);

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
int frame_queue_free_cb_register(frame_queue_ctrl_t *ctrl,
                                 frame_queue_free_cb_t before_free_cb,
                                 void *before_free_cb_arg,
                                 frame_queue_free_cb_t after_free_cb,
                                 void *after_free_cb_arg);

/**
 * @brief Send item to queue (supports ISR)
 * @param xQueue Queue handle
 * @param pvItemToQueue Item to queue
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int frame_queue_send(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait);

/**
 * @brief Send item to queue (supports ISR)
 * @param xQueue Queue handle
 * @param pvItemToQueue Item to queue
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int frame_queue_send_tofront(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait);

/**
 * @brief Receive item from queue (supports ISR)
 * @param xQueue Queue handle
 * @param pvBuffer Buffer to store item
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int frame_queue_receive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);

/**
 * @brief Get number of messages waiting in queue
 * @param xQueue Queue handle
 * @retval Number of messages waiting
 */
int frame_queue_messages_waiting(QueueHandle_t xQueue);

/**
 * @brief Check if queue is full
 * @param xQueue Queue handle
 * @retval 0 Queue is not full
 * @retval 1 Queue is full
 */
int frame_queue_is_full(QueueHandle_t xQueue);

/**
 * @brief Allocate frame element from pool
 * @param ctrl Frame queue controller
 * @param frame_elem Pointer to store allocated frame element
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int frame_queue_alloc_elem(frame_queue_ctrl_t *ctrl, frame_elem_t **frame_elem, TickType_t xTicksToWait);

/**
 * @brief Free frame element back to pool
 * @param frame_elem Frame element to free
 * @retval 0 Success
 * @retval -1 Error
 */
int frame_queue_free_elem(frame_elem_t *frame_elem);

#ifdef __cplusplus
}
#endif

#endif /* __STREAM_QUEUE_CTRL_H__ */
