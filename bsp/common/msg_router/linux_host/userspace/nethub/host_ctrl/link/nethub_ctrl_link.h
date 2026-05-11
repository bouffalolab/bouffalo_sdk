#ifndef NETHUB_CTRL_LINK_H
#define NETHUB_CTRL_LINK_H

#include <stddef.h>
#include <stdint.h>

#include "nethub_wifi_types.h"

typedef void (*nethub_ctrl_link_rx_cb_t)(const uint8_t *data, size_t len, void *user_data);
typedef void (*nethub_ctrl_link_event_cb_t)(nethub_wifi_link_state_t state, void *user_data);

int nethub_ctrl_link_init(nethub_ctrl_link_rx_cb_t rx_cb, void *user_data);
void nethub_ctrl_link_deinit(void);
int nethub_ctrl_link_send(const void *data, size_t len);
int nethub_ctrl_link_get_state(nethub_wifi_link_state_t *state);
int nethub_ctrl_link_register_event_callback(nethub_ctrl_link_event_cb_t cb, void *user_data);

#endif /* NETHUB_CTRL_LINK_H */
