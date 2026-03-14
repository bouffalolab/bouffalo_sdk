#ifndef NETHUB_VCHAN_H
#define NETHUB_VCHAN_H

#include <stdint.h>

/*
 * Public logical virtual-channel API carried by the active host-link backend.
 * The API surface is transport-neutral and should remain stable across
 * SDIO / USB / SPI backends.
 */

#define NETHUB_VCHAN_MAX_DATA_LEN (1500)

typedef uint8_t nethub_vchan_type_t;

#define NETHUB_VCHAN_TYPE_INVALID ((nethub_vchan_type_t)0U)
#define NETHUB_VCHAN_TYPE_USER    ((nethub_vchan_type_t)1U)
#define NETHUB_VCHAN_TYPE_AT      ((nethub_vchan_type_t)2U)
#define NETHUB_VCHAN_TYPE_SYSTEM  ((nethub_vchan_type_t)3U)
#define NETHUB_VCHAN_TYPE_MAX     (4U)

typedef int (*nethub_vchan_recv_cb_t)(void *cb_arg, uint8_t *data_buff, uint16_t data_size);

/* Send one payload on the specified logical virtual channel. */
int nethub_vchan_send(nethub_vchan_type_t type, const void *data, uint16_t len);

/* Register the receiver for one logical virtual channel type. */
int nethub_vchan_recv_register(nethub_vchan_type_t type,
                               nethub_vchan_recv_cb_t recv_cb,
                               void *cb_arg);

/* Convenience wrapper for the user virtual channel. */
int nethub_vchan_user_send(const void *data, uint16_t len);

/* Convenience wrapper for receiving user virtual channel payloads. */
int nethub_vchan_user_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg);

/* Convenience wrapper for the AT virtual channel. */
int nethub_vchan_at_send(const void *data, uint16_t len);

/* Convenience wrapper for receiving AT virtual channel payloads. */
int nethub_vchan_at_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg);

#endif
