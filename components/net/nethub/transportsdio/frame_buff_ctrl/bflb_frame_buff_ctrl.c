/**
 * @file bflb_frame_buff_ctrl.c
 * @brief Frame buffer controller implementation for memory pool management
 * @author mlwang
 * @date 2025-07-17
 */

#include "stdlib.h"
#include "string.h"

#include "bflb_core.h"

#include "mm.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "bflb_frame_buff_ctrl.h"

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
int frame_queue_send(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait)
{
    BaseType_t ret;

    // printf("%s, %d\r\n", __func__, __LINE__);
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
 * @brief FreeRTOS queue task/ISR operation send
 * @param xQueue Queue handle
 * @param pvItemToQueue Item to queue
 * @param xTicksToWait Ticks to wait
 * @retval 0 Success
 * @retval -1 Error
 */
int frame_queue_send_tofront(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait)
{
    BaseType_t ret;

    // printf("%s, %d\r\n", __func__, __LINE__);
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

        ret = xQueueSendToFrontFromISR(xQueue, pvItemToQueue, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }

    } else {
        ret = xQueueSendToFront(xQueue, pvItemToQueue, xTicksToWait);
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
int frame_queue_receive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait)
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
int frame_queue_messages_waiting(QueueHandle_t xQueue)
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
int frame_queue_is_full(QueueHandle_t xQueue)
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
frame_queue_ctrl_t *frame_queue_create(frame_queue_ctrl_init_cfg_t *cfg)
{
    int ret = 0;
    frame_elem_t *elem_head;

    frame_queue_ctrl_t *ctrl = kmalloc(sizeof(frame_queue_ctrl_t), 0);
    memset(ctrl, 0, sizeof(frame_queue_ctrl_t));

    /* Create pool queue (memory block pool management) */
    QueueHandle_t pool_queue = xQueueCreate(cfg->frame_elem_cnt, sizeof(uintptr_t));
    /* Create output queue (data-block FIFO) */
    QueueHandle_t output_queue = xQueueCreate(cfg->frame_elem_cnt, sizeof(uintptr_t));
    /* Create input queue (data-block FIFO) */
    QueueHandle_t input_queue = xQueueCreate(cfg->frame_elem_cnt, sizeof(uintptr_t));

    for (int i = 0; i < cfg->frame_elem_cnt; i++) {
        /* Get element address */
        elem_head = (frame_elem_t *)((uintptr_t)cfg->frame_buff + cfg->frame_elem_size * i);

        elem_head->frame_type = cfg->frame_type;
        elem_head->frame_id = i;
        elem_head->frame_ctrl = ctrl;

        elem_head->buff_addr = (void *)((uintptr_t)elem_head + FRAME_BUFF_MSGSTRUCT_OFFSET);
        elem_head->buff_size = cfg->frame_elem_size - FRAME_BUFF_MSGSTRUCT_OFFSET; /* Default header room size */
        elem_head->data_size = 0;

        /* Send to pool queue */
        ret = frame_queue_send(pool_queue, &elem_head, 0);
        if (ret != 0) {
            LOG_E("create send to pool failed\r\n");
            break;
        }
    }

    ctrl->frame_type = cfg->frame_type;
    ctrl->frame_elem_cnt = cfg->frame_elem_cnt;
    ctrl->frame_elem_size = cfg->frame_elem_size;
    ctrl->name = cfg->name;

    ctrl->pool_queue = pool_queue;
    ctrl->output_queue = output_queue;
    ctrl->input_queue = input_queue;

    LOG_I("frame_queue_create %s\r\n", ctrl->name);
    return ctrl;
}

/**
 * @brief Remove frame queue controller
 * @param ctrl Frame queue controller
 * @retval 0 Success
 * @note Delete the output queue before use
 */
int frame_queue_remove(frame_queue_ctrl_t *ctrl)
{
    /* Delete queue */
    vQueueDelete(ctrl->input_queue);
    ctrl->input_queue = NULL;

    vQueueDelete(ctrl->output_queue);
    ctrl->output_queue = NULL;

    vQueueDelete(ctrl->pool_queue);
    ctrl->pool_queue = NULL;

    LOG_I("frame_queue_remove %s\r\n", ctrl->name);

    /* Free controller */
    kfree(ctrl);

    return 0;
}

/**
 * @brief Get available elements in the pool
 * @param ctrl Frame queue controller
 * @retval Number of available elements
 */
int frame_queue_pool_get_available(frame_queue_ctrl_t *ctrl)
{
    return frame_queue_messages_waiting(ctrl->pool_queue);
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
int frame_queue_free_cb_register(frame_queue_ctrl_t *ctrl,
                                 frame_queue_free_cb_t before_free_cb,
                                 void *before_free_cb_arg,
                                 frame_queue_free_cb_t after_free_cb,
                                 void *after_free_cb_arg)
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
int frame_queue_alloc_elem(frame_queue_ctrl_t *ctrl, frame_elem_t **frame_elem, TickType_t xTicksToWait)
{
    // printf("%s, line:%d\r\n", __func__, __LINE__);
    int ret = 0;

    ret = frame_queue_receive(ctrl->pool_queue, frame_elem, xTicksToWait);
    #if CONFIG_NETHUB_DUMP_DNLDDATA
    if (ret == 0) {
        printf("------------- malloc elem:%p, buff_addr:%p, ctrl:%p\r\n", (*frame_elem), (*frame_elem)->buff_addr, (*frame_elem)->frame_ctrl);

    }
    #endif

    return ret;
}

/**
 * @brief Free frame element back to pool
 * @param frame_elem Frame element to free
 * @retval 0 Success
 * @retval -1 Error
 */
int frame_queue_free_elem(frame_elem_t *frame_elem)
{
    int ret;
    frame_queue_ctrl_t *ctrl = frame_elem->frame_ctrl;

    #if CONFIG_NETHUB_DUMP_DNLDDATA
    printf("------------- free elem:%p, buff_addr:%p, ctrl:%p\r\n", frame_elem, frame_elem->buff_addr, frame_elem->frame_ctrl);
    #endif
    if (((uint32_t)frame_elem + FRAME_BUFF_MSGSTRUCT_OFFSET) != (uint32_t)(frame_elem->buff_addr)) {
        printf("never here, elem:0x%08X, msg:0x%08X, pld:0x%08X\r\n",
                (uint32_t)frame_elem,
                (uint32_t)(frame_elem->buff_addr),
                ((uint32_t)(frame_elem->buff_addr)) + FRAME_BUFF_MSGSTRUCT);
        while(1);
    }
    if ((frame_elem->buff_addr == NULL) || (frame_elem->frame_ctrl == NULL)) {
        printf("err\r\n");
        while(1);
    }

    // printf("%s, line:%d\r\n", __func__, __LINE__);
    if (ctrl->before_free_cb) {
        ctrl->before_free_cb(frame_elem, ctrl->before_free_cb_arg);
    }

    /* Reset element */
    frame_elem->data_size = 0;
    /* Send to pool queue */
    ret = frame_queue_send(frame_elem->frame_ctrl->pool_queue, &frame_elem, 0);
    if (ret != 0) {
        printf("err send ret:%d\r\n", ret);
        while(1);
    }

    if (ctrl->after_free_cb) {
        ctrl->after_free_cb(frame_elem, ctrl->after_free_cb_arg);
    }

    return ret;
}
