/**
 * @file mr_frame_buff_ctrl.c
 * @brief Frame buffer controller implementation for memory pool management
 */

#include "stdlib.h"
#include "string.h"

#include "bflb_core.h"

#include "mm.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "mr_frame_buff_ctrl.h"

#define DBG_TAG "BF_CTRL"
#include "log.h"

/**
 * @brief Simple flexible memory block management
 *
 * Each memory block header saves descriptor information.
 * Uses FreeRTOS Queue for memory block pool management.
 */

/***************** queue opt *****************/

/**
 * @brief FreeRTOS queue task/ISR operation send
 * @param xQueue Queue handle
 * @param pvItemToQueue Item to queue
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_send(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait)
{
    BaseType_t ret;

    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

        ret = xQueueSendFromISR(xQueue, pvItemToQueue, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }

    } else {
        ret = xQueueSend(xQueue, pvItemToQueue, xTicksToWait);
    }

    return ret == pdTRUE ? 0 : -1;
}

/**
 * @brief FreeRTOS queue task/ISR operation receive
 * @param xQueue Queue handle
 * @param pvBuffer Buffer to store item
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_receive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait)
{
    BaseType_t ret;

    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

        ret = xQueueReceiveFromISR(xQueue, pvBuffer, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }

    } else {
        ret = xQueueReceive(xQueue, pvBuffer, xTicksToWait);
    }

    return ret == pdTRUE ? 0 : -1;
}

/**
 * @brief Get number of messages waiting in FreeRTOS queue (supports ISR)
 * @param xQueue Queue handle
 * @retval Number of messages waiting
 */
int mr_frame_queue_messages_waiting(QueueHandle_t xQueue)
{
    BaseType_t ret;

    if (xPortIsInsideInterrupt()) {
        ret = uxQueueMessagesWaitingFromISR(xQueue);
    } else {
        ret = uxQueueMessagesWaiting(xQueue);
    }

    return ret;
}

/**
 * @brief Check if FreeRTOS queue is full (supports ISR)
 * @param xQueue Queue handle
 * @retval 1 Queue is full
 * @retval 0 Queue is not full
 */
int mr_frame_queue_is_full(QueueHandle_t xQueue)
{
    BaseType_t ret;

    if (xPortIsInsideInterrupt()) {
        ret = xQueueIsQueueFullFromISR(xQueue);
    } else {
        ret = !(uxQueueSpacesAvailable(xQueue));
    }

    return ret == pdTRUE ? 1 : 0;
}

/***************** frame queue ctrl *****************/

/**
 * @brief Create frame queue controller
 * @param cfg Configuration structure
 * @retval Pointer to frame queue controller on success
 * @retval NULL on failure
 */
mr_frame_queue_ctrl_t *mr_frame_queue_create(mr_frame_queue_ctrl_init_cfg_t *cfg)
{
    int ret = 0;
    mr_frame_elem_t *elem_head;

    mr_frame_queue_ctrl_t *ctrl = kmalloc(sizeof(mr_frame_queue_ctrl_t), 0);
    memset(ctrl, 0, sizeof(mr_frame_queue_ctrl_t));

    /* Create pool queue (memory block pool management) */
    QueueHandle_t pool_queue = xQueueCreate(cfg->frame_elem_cnt, sizeof(uintptr_t));
    /* Create output queue (data-block FIFO) */
    QueueHandle_t output_queue = xQueueCreate(cfg->frame_elem_cnt, sizeof(uintptr_t));
    /* Create input queue (data-block FIFO) */
    QueueHandle_t input_queue = xQueueCreate(cfg->frame_elem_cnt, sizeof(uintptr_t));

    for (int i = 0; i < cfg->frame_elem_cnt; i++) {
        /* Get element address */
        elem_head = (mr_frame_elem_t *)((uintptr_t)cfg->frame_buff + cfg->frame_elem_size * i);

        elem_head->frame_type = cfg->frame_type;
        elem_head->frame_id = i;
        elem_head->frame_ctrl = ctrl;

        elem_head->buff_addr = (void *)((uintptr_t)elem_head + FRAME_BUFF_HEADER_ROOM);
        elem_head->buff_size = cfg->frame_elem_size - FRAME_BUFF_HEADER_ROOM; /* Default header room size */
        elem_head->data_size = 0;

        /* Send to pool queue */
        ret = mr_frame_queue_send(pool_queue, &elem_head, 0);
        if (ret != 0) {
            LOG_E("create send to pool failed\r\n");
            break;
        }
    }

    ctrl->frame_type = cfg->frame_type;
    ctrl->frame_elem_cnt = cfg->frame_elem_cnt;
    ctrl->frame_elem_size = cfg->frame_elem_size;
    ctrl->frame_elem_hdr_size = FRAME_BUFF_HEADER_ROOM; /* Default header room size */
    ctrl->name = cfg->name;

    ctrl->pool_queue = pool_queue;
    ctrl->output_queue = output_queue;
    ctrl->input_queue = input_queue;

    LOG_I("mr_frame_queue_create %s\r\n", ctrl->name);
    return ctrl;
}

/**
 * @brief Remove frame queue controller
 * @param ctrl Frame queue controller
 * @retval 0 Success
 * @note Delete the output queue before use
 */
int mr_frame_queue_remove(mr_frame_queue_ctrl_t *ctrl)
{
    /* Delete queue */
    vQueueDelete(ctrl->input_queue);
    ctrl->input_queue = NULL;

    vQueueDelete(ctrl->output_queue);
    ctrl->output_queue = NULL;

    vQueueDelete(ctrl->pool_queue);
    ctrl->pool_queue = NULL;

    LOG_I("mr_frame_queue_remove %s\r\n", ctrl->name);

    /* Free controller */
    kfree(ctrl);

    return 0;
}

/**
 * @brief Get available elements in the pool
 * @param ctrl Frame queue controller
 * @retval Number of available elements
 */
int mr_frame_queue_pool_get_available(mr_frame_queue_ctrl_t *ctrl)
{
    return mr_frame_queue_messages_waiting(ctrl->pool_queue);
}

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
int mr_frame_queue_free_cb_register(mr_frame_queue_ctrl_t *ctrl, mr_frame_queue_free_cb_t before_free_cb,
                                    void *before_free_cb_arg, mr_frame_queue_free_cb_t after_free_cb, void *after_free_cb_arg)
{
    if (!ctrl) {
        return -1;
    }

    ctrl->before_free_cb = before_free_cb;
    ctrl->before_free_cb_arg = before_free_cb_arg;
    ctrl->after_free_cb = after_free_cb;
    ctrl->after_free_cb_arg = after_free_cb_arg;

    return 0;
}

/************************************************************************************/

/**
 * @brief Allocate frame element from pool
 * @param ctrl Frame queue controller
 * @param frame_elem Pointer to store allocated frame element
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_alloc_elem(mr_frame_queue_ctrl_t *ctrl, mr_frame_elem_t **frame_elem, TickType_t xTicksToWait)
{
    int ret = 0;

    ret = mr_frame_queue_receive(ctrl->pool_queue, frame_elem, xTicksToWait);

    return ret;
}

/**
 * @brief Free frame element back to pool
 * @param frame_elem Frame element to free
 * @retval 0 Success
 * @retval -1 Error
 */
int mr_frame_queue_free_elem(mr_frame_elem_t *frame_elem)
{
    int ret;
    mr_frame_queue_ctrl_t *ctrl = frame_elem->frame_ctrl;

    if (!ctrl) {
        LOG_E("frame elem has no ctrl: %p\r\n", frame_elem);
        return -1;
    }

    if (ctrl->before_free_cb) {
        ctrl->before_free_cb(frame_elem, ctrl->before_free_cb_arg);
    }

    /* Reset element */
    frame_elem->data_size = 0;
    /* Send to pool queue */
    ret = mr_frame_queue_send(frame_elem->frame_ctrl->pool_queue, &frame_elem, 0);

    if (ctrl->after_free_cb) {
        ctrl->after_free_cb(frame_elem, ctrl->after_free_cb_arg);
    }

    return ret;
}
