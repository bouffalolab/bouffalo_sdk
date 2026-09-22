/*
 * Private MACSW driver structures shared with the MACSW control interface.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef DRIVER_MACSW_PRIV_H
#define DRIVER_MACSW_PRIV_H

struct wpa_macsw_driver_data;

/**
 * Per interface driver data
 * !! keep same with ctrl_iface_macsw.c
 */
struct wpa_macsw_driver_itf_data {
	// WPA_supplicant global context
	void *ctx;
	// Global driver data
	struct wpa_macsw_driver_data *gdrv;
	// Index, at FHOST level, of the interface
	int fhost_vif_idx;
	// Initial interface type (ref @ enum mac_vif_type)
	uint8_t vif_init_type;
	// List of scan results
	struct dl_list scan_res;
	// Driver status
	int status;
	// MAC address of the AP we are connected to
	u8 bssid[ETH_ALEN];
	// SSID of the AP we are connected to
	u8 *ssid;
	// SSID length
	u8 ssid_len;
	// Next authentication alg to try (used when connect with several algos)
	int next_auth_alg;
	// DTIM period cached from set_ap, reused when building CSA beacon
	u8 dtim_period;
};

/**
 * Global data driver info
 * !! keep same with driver_macsw.c
 */
struct wpa_macsw_driver_data {
	// WPA_supplicant context
	void *ctx;
	// List of interface driver data
	struct wpa_macsw_driver_itf_data itfs[MACSW_VIRT_DEV_MAX];
	rtos_queue resp_queue;
	// Extended capabilities
	u8 extended_capab[10];
	// Extended capabilities mask
	u8 extended_capab_mask[10];
};

#endif /* DRIVER_MACSW_PRIV_H */
