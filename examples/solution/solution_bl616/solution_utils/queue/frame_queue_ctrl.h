#ifndef __STREAM_QUEUE_CTRL_H__
#define __STREAM_QUEUE_CTRL_H__

#include "stdint.h"

#include <FreeRTOS.h>
#include <queue.h>

/*
    frame queue control, Support single input multiple output
*/

/* output queue max num */
#define OUTPUT_QUEUE_NUM_MAX    (3)

/* output queue attribute */
// #define FRAME_QUEUE_ATTR_BLOCK       (1)
// #define FRAME_QUEUE_ATTR_OUTPUT_LOSS (2)
// #define FRAME_QUEUE_ATTR_INTPUT_LOSS (3)

/* all/default/not-specified queue id */
#define FRAME_QUEUE_DEFAULT_ID  (0xffff)

#define FRAME_QUEUE_ATTR_UNLOCK (0)
#define FRAME_QUEUE_ATTR_LOCK   (1)

/* frame queue element description */
typedef struct {
    void *frame_addr;
    uint32_t frame_size;
    uint16_t frame_id;

    uint8_t custom_info[];
} frame_elem_t;

/* frame queue element status */
typedef struct {
    uint16_t elem_valid_cnt;
    uint8_t elem_push_lock_flag;
    uint8_t elem_free_lock_flag;
} frame_elem_status_t;

/* frame output queue description */
typedef struct
{
    QueueHandle_t out_queue;
    uint16_t out_queue_lock_flag;
} frame_queue_desc_t;

/* frame queue description */
typedef struct {
    uint16_t queue_depth;
    uint16_t queue_elem_size;

    char *name;

    /* valid_bitmap for the element in the queue, num same as queue depth */
    frame_elem_status_t *elem_status;

    /* pool queue (memory block pool management) */
    QueueHandle_t pool_queue;

    /* output queue (data-block fifo) */
    frame_queue_desc_t out_queue_desc[OUTPUT_QUEUE_NUM_MAX];

} frame_queue_ctrl_t;

/* frame queue opt */
int frame_queue_create(frame_queue_ctrl_t **ctrl, uint16_t queue_depth, uint16_t elem_size, frame_elem_t elem[], char *name);
int frame_queue_remove(frame_queue_ctrl_t *ctrl);
int frame_queue_pool_get_available(frame_queue_ctrl_t *ctrl);

/* frame output queue opt */
int frame_queue_output_create(frame_queue_ctrl_t *ctrl, uint16_t *out_queue_id, uint16_t out_queue_depth);
int frame_queue_output_remove(frame_queue_ctrl_t *ctrl, uint16_t out_queue_id);

/* data frame opt (support ISR) */
int frame_queue_alloc_lock(frame_queue_ctrl_t *ctrl, frame_elem_t *elem, uint32_t timeout);
int frame_queue_push(frame_queue_ctrl_t *ctrl, frame_elem_t *elem, uint16_t out_queue_id, uint32_t timeout);
int frame_queue_free_unlock(frame_queue_ctrl_t *ctrl, frame_elem_t *elem);
int frame_queue_output_pop(frame_queue_ctrl_t *ctrl, frame_elem_t *elem, uint16_t out_queue_id, uint32_t timeout);
int frame_queue_output_free(frame_queue_ctrl_t *ctrl, frame_elem_t *elem);
int frame_queue_output_get_waiting(frame_queue_ctrl_t *ctrl, uint16_t out_queue_id);

#endif
