/* MACSW structured P2P notifications. */
#ifndef P2P_NOTIFY_MACSW_H
#define P2P_NOTIFY_MACSW_H

#include "p2p/p2p.h"

struct wpa_ssid;
struct wpa_supplicant;

struct wpas_p2p_prov_disc_info;

void wpas_macsw_p2p_find_stopped(struct wpa_supplicant *wpa_s);
void wpas_macsw_p2p_listen_stopped(struct wpa_supplicant *wpa_s);
void wpas_macsw_p2p_device_found(struct wpa_supplicant *wpa_s,
				 const u8 *dev_addr,
				 const u8 *iface_addr,
				 const struct p2p_peer_info *info);
void wpas_macsw_p2p_device_lost(struct wpa_supplicant *wpa_s,
				const u8 *dev_addr);
void wpas_macsw_p2p_group_removed(struct wpa_supplicant *wpa_s,
				  const char *role, const char *reason);
void wpas_macsw_p2p_go_neg_req(struct wpa_supplicant *wpa_s,
			       const u8 *src, u16 dev_passwd_id,
			       u8 go_intent);
void wpas_macsw_p2p_go_neg_completed(struct wpa_supplicant *wpa_s,
				     const struct p2p_go_neg_results *res);
void wpas_macsw_p2p_provision_discovery(
	struct wpa_supplicant *wpa_s, const u8 *dev_addr, int request,
	enum p2p_prov_disc_status status, u16 config_methods,
	unsigned int generated_pin,
	const struct wpas_p2p_prov_disc_info *info);
void wpas_macsw_p2p_provision_discovery_failure(
	struct wpa_supplicant *wpa_s, const u8 *dev_addr);
void wpas_macsw_p2p_group_started(struct wpa_supplicant *wpa_s,
				  const struct wpa_ssid *ssid, int persistent,
				  int client, int freq, const u8 *go_dev_addr);
/* Dump Key RAM after a P2P GO client has completed key installation. */
void wpas_macsw_p2p_go_sta_authorized(struct wpa_supplicant *wpa_s,
				      const u8 *sta_addr);
void wpas_macsw_p2p_group_formation_success(struct wpa_supplicant *wpa_s);
void wpas_macsw_p2p_group_formation_failure(struct wpa_supplicant *wpa_s,
					    const char *reason);
void wpas_macsw_p2p_invitation_received(struct wpa_supplicant *wpa_s,
					const u8 *sa,
					const u8 *go_dev_addr,
					const u8 *bssid, int id,
					int id_valid, int op_freq);
void wpas_macsw_p2p_invitation_accepted(struct wpa_supplicant *wpa_s,
					const u8 *sa,
					const u8 *go_dev_addr,
					const u8 *bssid, int id,
					int id_valid, int op_freq);
void wpas_macsw_p2p_invitation_result(struct wpa_supplicant *wpa_s,
				      int status, const u8 *bssid);

#endif /* P2P_NOTIFY_MACSW_H */
