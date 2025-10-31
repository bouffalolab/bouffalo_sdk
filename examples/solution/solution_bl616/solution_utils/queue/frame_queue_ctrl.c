#include "stdlib.h"
#include "string.h"

#include <compiler/compiler_gcc.h>
#include <compiler/compiler_ld.h>

#include "bflb_irq.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "frame_queue_ctrl.h"

#include "mem.h"

#if (0)
#define FRAME_DBG(a, ...) printf("[frame dbg]:" a, ##__VA_ARGS__)
#else
#define FRAME_DBG(a, ...)
#endif

#define FRAME_ERR(a, ...) printf("[frame err]:" a, ##__VA_ARGS__)

/***************** queue opt *****************/

/* freeRTOS queue task/ISR opt send */
inline static BaseType_t frame_queue_send(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait)
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

    return ret;
}

/* freeRTOS queue task/ISR opt receive */
inline static BaseType_t frame_queue_receive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait)
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

    return ret;
}

/* freeRTOS queue task/ISR opt get messages_waiting */

inline static BaseType_t frame_queue_messages_waiting(QueueHandle_t xQueue)
{
    BaseType_t ret;

    if (xPortIsInsideInterrupt()) {
        ret = uxQueueMessagesWaitingFromISR(xQueue);
    } else {
        ret = uxQueueMessagesWaiting(xQueue);
    }

    return ret;
}

/* freeRTOS queue task/ISR opt is_full? */
inline static BaseType_t frame_queue_is_full(QueueHandle_t xQueue)
{
    BaseType_t ret;

    if (xPortIsInsideInterrupt()) {
        ret = xQueueIsQueueFullFromISR(xQueue);
    } else {
        ret = !(uxQueueSpacesAvailable(xQueue));
    }

    return ret;
}

/***************** frame queue ctrl *****************/

/* queue ctrl create */
int frame_queue_create(frame_queue_ctrl_t **ctrl, uint16_t queue_depth, uint16_t elem_size, frame_elem_t elem[], char *name)
{
    BaseType_t q_ret;
    int ret = 0;

    frame_queue_ctrl_t *ctrl_ptr = kmalloc(sizeof(frame_queue_ctrl_t));
    memset(ctrl_ptr, 0, sizeof(frame_queue_ctrl_t));

    uint32_t *elem_status_ptr = kmalloc(queue_depth * sizeof(frame_elem_status_t));
    memset(elem_status_ptr, 0, queue_depth * sizeof(frame_elem_status_t));

    /* creat pool queue (memory block pool manage) */
    QueueHandle_t pool_queue = xQueueCreate(queue_depth, elem_size);

    /* fill pool queue */
    for (uint16_t i = 0; i < queue_depth; i++) {
        /* get elem addr */
        void *elem_custom = (void *)((uint32_t)elem + elem_size * i);
        /* send to poolmake */
        q_ret = xQueueSend(pool_queue, elem_custom, 0);
        if (q_ret == pdFALSE) {
            FRAME_ERR("create send to pool failed\r\n");
            ret = -1;
        }
    }

    ctrl_ptr->queue_depth = queue_depth;
    ctrl_ptr->queue_elem_size = elem_size;
    ctrl_ptr->elem_status = (frame_elem_status_t *)elem_status_ptr;
    ctrl_ptr->pool_queue = pool_queue;
    ctrl_ptr->name = name;

    *ctrl = ctrl_ptr;

    FRAME_DBG("frame_queue_create %s\r\n", ctrl_ptr->name);

    return ret;
}

/* Delete the output queue before use */
int frame_queue_remove(frame_queue_ctrl_t *ctrl)
{
    /* out queue */
    for (uint16_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
        if (ctrl->out_queue_desc[i].out_queue != NULL) {
            return -1;
        }
    }

    FRAME_DBG("frame_queue_remov %s\r\n", ctrl->name);

    /* delete queue */
    vQueueDelete(ctrl->pool_queue);

    /* free mem */
    kfree(ctrl->elem_status);
    kfree(ctrl);

    return 0;
}

ATTR_TCM_SECTION int frame_queue_pool_get_available(frame_queue_ctrl_t *ctrl)
{
    return frame_queue_messages_waiting(ctrl->pool_queue);
}

/* output_queue create */
int frame_queue_output_create(frame_queue_ctrl_t *ctrl, uint16_t *out_queue_id, uint16_t out_queue_depth)
{
    if (*out_queue_id >= OUTPUT_QUEUE_NUM_MAX) {
        return -1;
    }

    /* Specified ID */
    if (*out_queue_id != FRAME_QUEUE_DEFAULT_ID) {
        if (ctrl->out_queue_desc[*out_queue_id].out_queue != NULL) {
            return -1;
        }

        ctrl->out_queue_desc[*out_queue_id].out_queue = xQueueCreate(out_queue_depth, ctrl->queue_elem_size);
        ctrl->out_queue_desc[*out_queue_id].out_queue_lock_flag = 0;

        return 0;
    }

    /* Automatically assign an ID */
    for (uint16_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
        if (ctrl->out_queue_desc[i].out_queue == NULL) {
            ctrl->out_queue_desc[i].out_queue = xQueueCreate(out_queue_depth, ctrl->queue_elem_size);
            ctrl->out_queue_desc[i].out_queue_lock_flag = 0;
            *out_queue_id = i;
            return 0;
        }
    }

    return -2;
}

/* remove output queue */
int frame_queue_output_remove(frame_queue_ctrl_t *ctrl, uint16_t out_queue_id)
{
    BaseType_t q_ret;
    QueueHandle_t out_queue;

    if (out_queue_id >= OUTPUT_QUEUE_NUM_MAX) {
        return -1;
    }

    /* output queue info */
    out_queue = ctrl->out_queue_desc[out_queue_id].out_queue;

    if (out_queue == NULL) {
        return -1;
    }

    /* TODO: free all elem of output_queue */
    /* lock, forbid sending */
    ctrl->out_queue_desc[out_queue_id].out_queue_lock_flag = 1;

    /* Variable-length array (VLA) */
    uint32_t elem_buff[ctrl->queue_elem_size / 4 + 1];
    frame_elem_t *elem = (frame_elem_t *)elem_buff;

    for (;;) {
        /* pop from out_queue */
        q_ret = frame_queue_receive(out_queue, elem, 0);

        if (q_ret == pdTRUE) {
            /* free the elem */
            frame_queue_output_free(ctrl, elem);
        } else {
            /* out_queue empty, end */
            break;
        }
    }

    /* delete queue */
    vQueueDelete(ctrl->out_queue_desc[out_queue_id].out_queue);

    ctrl->out_queue_desc[out_queue_id].out_queue = NULL;
    ctrl->out_queue_desc[out_queue_id].out_queue_lock_flag = 0;

    return 0;
}

/* alloc and lock () */
ATTR_TCM_SECTION int frame_queue_alloc_lock(frame_queue_ctrl_t *ctrl, frame_elem_t *elem, uint32_t timeout)
{
    int ret = 0;
    BaseType_t q_ret;

    QueueHandle_t pool_queue = ctrl->pool_queue;

    q_ret = frame_queue_receive(pool_queue, elem, timeout);

    if (q_ret == pdTRUE) {
        /* set lock bit */
        ctrl->elem_status[elem->frame_id].elem_push_lock_flag = FRAME_QUEUE_ATTR_UNLOCK;
        ctrl->elem_status[elem->frame_id].elem_free_lock_flag = FRAME_QUEUE_ATTR_LOCK;
        ctrl->elem_status[elem->frame_id].elem_valid_cnt = 0;

        ret = 0;
    } else {
        ret = -1;
    }

    FRAME_DBG("queue_alloc %s\r\n", ctrl->name);

    return ret;
}

/* frame queue push */
ATTR_TCM_SECTION int frame_queue_push(frame_queue_ctrl_t *ctrl, frame_elem_t *elem, uint16_t out_queue_id, uint32_t timeout)
{
    int ret = 0;
    BaseType_t q_ret;
    QueueHandle_t out_queue;

    if (out_queue_id >= OUTPUT_QUEUE_NUM_MAX) {
        return -1;
    }

    /* output queue info */
    out_queue = ctrl->out_queue_desc[out_queue_id].out_queue;

    if (out_queue == NULL) {
        /* no the out_queue */
        return -2;
    }

    /* disable irq */
    uintptr_t flag = bflb_irq_save();

    /* check lock status */
    if (ctrl->elem_status[elem->frame_id].elem_push_lock_flag == FRAME_QUEUE_ATTR_UNLOCK &&
        ctrl->out_queue_desc[out_queue_id].out_queue_lock_flag == FRAME_QUEUE_ATTR_UNLOCK) {
        ctrl->elem_status[elem->frame_id].elem_valid_cnt += 1;
        ret = 0;
    } else {
        ret = -3;
    }

    /* restore irq */
    bflb_irq_restore(flag);

    if (ret < 0) {
        return ret;
    }

    /* send queue */
    q_ret = frame_queue_send(out_queue, elem, timeout);

    if (q_ret == pdTRUE) {
        /* send success */
        ret = 0;
    } else {
        /* send failed */
        if (ctrl->elem_status[elem->frame_id].elem_valid_cnt == 0) {
            FRAME_ERR("queue_push name-%s, id-%d elem_valid_cnt = 0\r\n", ctrl->name, elem->frame_id);
            ret = -2;
        } else {
            ctrl->elem_status[elem->frame_id].elem_valid_cnt -= 1;
            ret = -3;
        }
    }

    FRAME_DBG("queue_push %s %d\r\n", ctrl->name, ret);

    return ret;
}

/* unlock (allow free) */
ATTR_TCM_SECTION int frame_queue_free_unlock(frame_queue_ctrl_t *ctrl, frame_elem_t *elem)
{
    int ret = 0;
    int free_flag = 0;
    BaseType_t q_ret;

    /* disable irq */
    uintptr_t flag = bflb_irq_save();

    /* push lock */
    ctrl->elem_status[elem->frame_id].elem_push_lock_flag = FRAME_QUEUE_ATTR_LOCK;

    /* check valid cnt, free the elem if need */
    if (ctrl->elem_status[elem->frame_id].elem_valid_cnt == 0) {
        free_flag = 1;
    }

    /* free unlock */
    ctrl->elem_status[elem->frame_id].elem_free_lock_flag = FRAME_QUEUE_ATTR_UNLOCK;

    /* restore irq */
    bflb_irq_restore(flag);

    /* free the elem if need */
    if (free_flag) {
        /* send to pool queue */
        q_ret = frame_queue_send(ctrl->pool_queue, elem, 0);

        if (q_ret == pdTRUE) {
            ret = 1;
        } else {
            FRAME_ERR("free_unlock %s send to pool failed\r\n", ctrl->name);
            ret = -1;
        }
    }

    if (ret == 0) {
        FRAME_DBG("unlock %s\r\n", ctrl->name);
    } else {
        FRAME_DBG("unlock_free %s\r\n", ctrl->name);
    }

    return ret;
}

/* frame pop */
ATTR_TCM_SECTION int frame_queue_output_pop(frame_queue_ctrl_t *ctrl, frame_elem_t *elem, uint16_t out_queue_id, uint32_t timeout)
{
    int ret = 0;
    BaseType_t q_ret;
    QueueHandle_t out_queue;

    if (out_queue_id >= OUTPUT_QUEUE_NUM_MAX) {
        return -1;
    }

    /* output queue info */
    out_queue = ctrl->out_queue_desc[out_queue_id].out_queue;

    if (out_queue == NULL) {
        /* no the out_queue */
        return -2;
    }

    /*  */
    q_ret = frame_queue_receive(out_queue, elem, timeout);

    if (q_ret == pdTRUE) {
        ret = 0;
    } else {
        ret = -3;
    }

    FRAME_DBG("pop %s %d\r\n", ctrl->name, ret);

    return ret;
}

ATTR_TCM_SECTION int frame_queue_output_free(frame_queue_ctrl_t *ctrl, frame_elem_t *elem)
{
    int ret = 0;
    int free_flag = 0;
    BaseType_t q_ret;

    if (ctrl->elem_status[elem->frame_id].elem_valid_cnt == 0) {
        FRAME_ERR("output_free name-%s, id-%d elem_valid_cnt = 0\r\n", ctrl->name, elem->frame_id);
        return -2;
    }

    /* disable irq */
    uintptr_t flag = bflb_irq_save();

    ctrl->elem_status[elem->frame_id].elem_valid_cnt -= 1;

    if (ctrl->elem_status[elem->frame_id].elem_valid_cnt == 0 &&
        ctrl->elem_status[elem->frame_id].elem_free_lock_flag == FRAME_QUEUE_ATTR_UNLOCK) {
        /* free the elem */
        free_flag = 1;
    }

    /* restore irq */
    bflb_irq_restore(flag);

    if (free_flag) {
        /* send to pool queue */
        q_ret = frame_queue_send(ctrl->pool_queue, elem, 0);

        if (q_ret == pdTRUE) {
            ret = 1;
        } else {
            FRAME_ERR("output_free send to pool failed\r\n");
            ret = -2;
        }
    }

    FRAME_DBG("free %s %d\r\n", ctrl->name, ret);

    return ret;
}

ATTR_TCM_SECTION int frame_queue_output_get_waiting(frame_queue_ctrl_t *ctrl, uint16_t out_queue_id)
{
    QueueHandle_t out_queue;

    if (out_queue_id >= OUTPUT_QUEUE_NUM_MAX) {
        return -1;
    }

    /* output queue info */
    out_queue = ctrl->out_queue_desc[out_queue_id].out_queue;

    if (out_queue == NULL) {
        /* no the out_queue */
        return -2;
    }

    return frame_queue_messages_waiting(out_queue);
}
