#ifndef NETHUB_H
#define NETHUB_H

#include "nethub_defs.h"

/*
 * Public facade of nethub.
 *
 * nethub_bootstrap() selects the active host-link profile through the
 * compiled CONFIG_NETHUB_PROFILE_* choice and brings up the corresponding
 * endpoints as one bridge instance.
 */
int nethub_bootstrap(void);

/* Stop the active bridge instance and release runtime resources. */
void nethub_shutdown(void);

/* Return a snapshot of the current runtime state and statistics. */
int nethub_get_status(nethub_runtime_status_t *status);

/*
 * Select the Wi-Fi endpoint used by host-link -> Wi-Fi forwarding.
 * Valid values are NETHUB_CHANNEL_WIFI_STA and NETHUB_CHANNEL_WIFI_AP.
 */
int nethub_set_active_wifi_channel(nethub_channel_t channel);

/*
 * Send one control payload on the logical uplink control channel carried by
 * the active host-link backend.
 */
int nethub_ctrl_upld_send(uint8_t *data_buff, uint32_t data_size);

/*
 * Register the receiver for payloads delivered on the logical downlink
 * control channel carried by the active host-link backend.
 */
int nethub_ctrl_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg);

#endif
