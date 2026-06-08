#ifndef NETHUB_H
#define NETHUB_H

#include "nethub_defs.h"

/*
 * Public facade of nethub.
 *
 * Bootstrap selects the configured host-link profile and brings up the
 * corresponding endpoints as one bridge instance.
 *
 * Return NETHUB_OK on success, or a negative nethub_status_t value.
 */
int nethub_bootstrap(void);

/*
 * Stop the active bridge instance and release runtime resources.
 *
 * It is valid to call this during product shutdown or before rebuilding the
 * NetHub runtime state.
 */
void nethub_shutdown(void);

/*
 * Read the current runtime state and statistics.
 *
 * status must point to writable storage. The snapshot contains the selected
 * profile name, active host link, active Wi-Fi channel, and packet counters.
 *
 * Return NETHUB_OK on success, or NETHUB_ERR_INVALID_PARAM if status is NULL.
 */
int nethub_get_status(nethub_runtime_status_t *status);

/*
 * Select the Wi-Fi endpoint used by host-link -> Wi-Fi forwarding.
 *
 * Valid values are NETHUB_CHANNEL_WIFI_STA and NETHUB_CHANNEL_WIFI_AP. The
 * default active Wi-Fi channel is NETHUB_CHANNEL_WIFI_STA.
 *
 * Return NETHUB_OK on success, or a negative nethub_status_t value.
 */
int nethub_set_active_wifi_channel(nethub_channel_t channel);

/*
 * Send one control payload on the logical uplink control channel carried by
 * the active host-link backend.
 *
 * The control facade is intended for the optional AT-based control solution.
 * When CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE is disabled, this API returns
 * NETHUB_ERR_NOT_SUPPORTED.
 *
 * data_size must not exceed NETHUB_VCHAN_MAX_DATA_LEN.
 *
 * Return NETHUB_OK on success, or a negative nethub_status_t value.
 */
int nethub_ctrl_upld_send(uint8_t *data_buff, uint32_t data_size);

/*
 * Register the receiver for payloads delivered on the logical downlink
 * control channel carried by the active host-link backend.
 *
 * Passing NULL unregisters the receiver. When
 * CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE is disabled, this API returns
 * NETHUB_ERR_NOT_SUPPORTED.
 *
 * Return NETHUB_OK on success, or a negative nethub_status_t value.
 */
int nethub_ctrl_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg);

/*
 * Check whether the active host link is idle enough for a low-power entry.
 *
 * If the active host backend does not provide an is_idle hook, the host link
 * is treated as idle.
 */
bool nethub_host_link_is_idle(void);

/*
 * Let the active host backend prepare for a low-power window.
 *
 * If the active host backend does not provide a lowpower_prepare hook, this
 * API is a no-op and returns NETHUB_OK.
 */
int nethub_host_link_lowpower_prepare(void);

/*
 * Let the active host backend resume after a low-power window.
 *
 * If the active host backend does not provide a lowpower_resume hook, this API
 * is a no-op and returns NETHUB_OK.
 */
int nethub_host_link_lowpower_resume(void);

/*
 * Select the active host link at runtime.
 *
 * host_link must be a host channel such as NETHUB_CHANNEL_SDIO or
 * NETHUB_CHANNEL_USB. The corresponding profile and endpoint must already be
 * compiled, initialized, and registered. This API only switches NetHub's
 * active host-link routing state; it does not initialize or enumerate a new
 * physical interface.
 *
 * Return NETHUB_OK on success, NETHUB_ERR_NOT_INITIALIZED before bootstrap, or
 * NETHUB_ERR_NOT_FOUND if the requested host link is not available.
 */
int nethub_set_active_host_link(nethub_channel_t host_link);

#endif
