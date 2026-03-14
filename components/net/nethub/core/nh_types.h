#ifndef NH_TYPES_H
#define NH_TYPES_H

#include "nethub_defs.h"

typedef struct nethub_frame {
    uint8_t *data;
    uint32_t len;
    void (*free_cb)(void *frame_elem);
    void *cb_arg;
    struct nethub_frame *next;
} nethub_frame_t;

typedef enum {
    NETHUB_ROUTE_CONTINUE = 0,
    NETHUB_ROUTE_STOP,
    NETHUB_ROUTE_ERROR,
} nethub_route_result_t;

typedef nethub_route_result_t (*nethub_route_cb_t)(nethub_frame_t *frame, void *arg);

typedef enum {
    NETHUB_ENDPOINT_STATE_UNINITIALIZED = 0,
    NETHUB_ENDPOINT_STATE_INITIALIZED,
    NETHUB_ENDPOINT_STATE_ACTIVE,
    NETHUB_ENDPOINT_STATE_ERROR,
} nethub_endpoint_state_t;

typedef struct nethub_config {
    const void *reserved;
} nethub_config_t;

#endif
