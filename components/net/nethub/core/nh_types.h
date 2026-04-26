#ifndef NH_TYPES_H
#define NH_TYPES_H

#include "nethub_defs.h"

typedef struct nethub_frame {
    /*
     * Start address of the valid frame data.
     *
     * For Ethernet traffic this normally points to the first byte of the
     * Ethernet payload area. The owner must guarantee that enough writable
     * headroom is reserved before `data`, because lower TX paths may move the
     * payload pointer backwards in place and prepend hardware/link headers.
     *
     * The current Wi-Fi TX path expects at least
     * `PBUF_LINK_ENCAPSULATION_HLEN` bytes of headroom.
     */
    uint8_t *data;

    /*
     * Length of the valid data starting at `data`.
     *
     * This is the payload length only, not the total buffer capacity. Any
     * reserved headroom before `data` is not included in `len`.
     */
    uint32_t len;

    /*
     * Optional release callback for the frame owner.
     *
     * When nethub and the downstream transport no longer need this frame, they
     * return ownership by calling `free_cb(cb_arg)`. If provided, the callback
     * must tolerate being called exactly once for the frame lifetime.
     */
    void (*free_cb)(void *frame_elem);

    /*
     * Private context passed back to `free_cb`.
     *
     * This usually points to the original descriptor, slot, pbuf, or another
     * owner-defined resource handle. Some TX paths also reuse the object
     * referenced by `cb_arg` as temporary writable descriptor storage, so the
     * pointed object must remain valid for the whole frame lifetime and satisfy
     * the memory layout requirements of that path.
     */
    void *cb_arg;

    /*
     * Reserved for future chaining support.
     *
     * It is typically NULL today, but can later be used for chained frames,
     * scatter-gather buffers, or other multi-segment extensions.
     */
    struct nethub_frame *next;
} nethub_frame_t;

typedef enum {
    /* Continue routing through the normal forwarding path. */
    NETHUB_ROUTE_CONTINUE = 0,

    /* The current node has consumed the frame, so forwarding stops here. */
    NETHUB_ROUTE_STOP,

    /* Processing failed and the caller should handle the error path. */
    NETHUB_ROUTE_ERROR,
} nethub_route_result_t;

/* Common callback prototype used by nethub routing stages. */
typedef nethub_route_result_t (*nethub_route_cb_t)(nethub_frame_t *frame, void *arg);

typedef enum {
    /* Endpoint has not been initialized yet. */
    NETHUB_ENDPOINT_STATE_UNINITIALIZED = 0,

    /* Endpoint has been initialized but is not active yet. */
    NETHUB_ENDPOINT_STATE_INITIALIZED,

    /* Endpoint is active and operating normally. */
    NETHUB_ENDPOINT_STATE_ACTIVE,

    /* Endpoint entered an error state and may require recovery. */
    NETHUB_ENDPOINT_STATE_ERROR,
} nethub_endpoint_state_t;

typedef struct nethub_config {
    /* Reserved for future configuration fields. */
    const void *reserved;
} nethub_config_t;

#endif
