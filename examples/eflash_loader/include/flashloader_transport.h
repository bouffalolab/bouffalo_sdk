#ifndef FLASHLOADER_TRANSPORT_H
#define FLASHLOADER_TRANSPORT_H

#include <stddef.h>
#include <stdint.h>

#include "flashloader_config.h"

void flashloader_transport_init(void);
int flashloader_transport_receive(uint8_t *frame, size_t capacity,
                                  size_t *frame_length, uint8_t *handshake);
int flashloader_transport_send(const uint8_t *data, size_t length);
void flashloader_transport_send_pending(void);
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_HOST_ACK) != 0U
void flashloader_transport_wait_response_consumed(void);
#endif

#endif
