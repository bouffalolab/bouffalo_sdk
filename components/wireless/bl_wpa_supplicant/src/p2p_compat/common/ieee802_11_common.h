#ifndef BL_P2P_COMPAT_IEEE802_11_COMMON_H
#define BL_P2P_COMPAT_IEEE802_11_COMMON_H

#include "../../utils/common.h"
#include "../../common/defs.h"
#include "../../common/ieee802_11_defs.h"
#include "utils/wpabuf.h"

#ifndef STRUCT_PACKED
#define STRUCT_PACKED __attribute__((packed))
#endif

struct element {
	u8 id;
	u8 datalen;
	u8 data[];
} STRUCT_PACKED;

struct ieee802_11_elems {
	const u8 *ssid;
	const u8 *supp_rates;
	const u8 *ds_params;
	const u8 *ext_supp_rates;
	const u8 *wpa_ie;
	const u8 *wmm;
	const u8 *wps_ie;
	const u8 *p2p;
	const u8 *wfd;
	const u8 *pref_freq_list;
	u8 ssid_len;
	u8 supp_rates_len;
	u8 ext_supp_rates_len;
	u8 wpa_ie_len;
	u8 wmm_len;
	u8 wps_ie_len;
	u8 p2p_len;
	u8 wfd_len;
	u8 pref_freq_list_len;
};

typedef enum { ParseOK = 0, ParseUnknown = 1, ParseFailed = -1 } ParseRes;

ParseRes ieee802_11_parse_elems(const u8 *start, size_t len,
				struct ieee802_11_elems *elems,
				int show_errors);
struct wpabuf *ieee802_11_vendor_ie_concat(const u8 *ies, size_t ies_len,
					    u32 oui_type);
int supp_rates_11b_only(struct ieee802_11_elems *elems);
int ieee80211_chan_to_freq(const char *country, u8 op_class, u8 chan);
enum hostapd_hw_mode ieee80211_freq_to_channel_ext(unsigned int freq,
						   int sec_channel, int vht,
						   u8 *op_class, u8 *channel);
int is_6ghz_freq(int freq);
int is_6ghz_op_class(u8 op_class);

#endif
