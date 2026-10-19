#include "common/ieee802_11_common.h"

#define QCA_VENDOR_IE_TYPE_P2P_PREF_CHAN_LIST 0x00137400

static int p2p_compat_freq_to_channel(unsigned int freq)
{
	if (freq == 2484)
		return 14;
	if (freq >= 2412 && freq <= 2472 && (freq - 2407) % 5 == 0)
		return (freq - 2407) / 5;
	if (freq >= 5000 && freq <= 5900 && (freq - 5000) % 5 == 0)
		return (freq - 5000) / 5;
	return -1;
}

static int p2p_compat_is_11b_rate(u8 rate)
{
	return rate == 0x02 || rate == 0x04 || rate == 0x0b || rate == 0x16;
}

ParseRes ieee802_11_parse_elems(const u8 *start, size_t len,
				struct ieee802_11_elems *elems,
				int show_errors)
{
	const u8 *pos = start;
	const u8 *end = start + len;

	(void) show_errors;

	os_memset(elems, 0, sizeof(*elems));

	while (end - pos >= 2) {
		u8 id = *pos++;
		u8 elen = *pos++;
		const u8 *data = pos;

		if (elen > end - pos)
			return ParseFailed;

		switch (id) {
		case WLAN_EID_SSID:
			elems->ssid = data;
			elems->ssid_len = elen;
			break;
		case WLAN_EID_SUPP_RATES:
			elems->supp_rates = data;
			elems->supp_rates_len = elen;
			break;
		case WLAN_EID_DS_PARAMS:
			elems->ds_params = data;
			break;
		case WLAN_EID_EXT_SUPP_RATES:
			elems->ext_supp_rates = data;
			elems->ext_supp_rates_len = elen;
			break;
		case WLAN_EID_VENDOR_SPECIFIC:
			if (elen >= 4) {
				u32 vendor_type = WPA_GET_BE32(data);

				if (vendor_type == WPA_IE_VENDOR_TYPE) {
					elems->wpa_ie = data;
					elems->wpa_ie_len = elen;
				} else if (vendor_type == WMM_IE_VENDOR_TYPE) {
					elems->wmm = data;
					elems->wmm_len = elen;
				} else if (vendor_type == WPS_IE_VENDOR_TYPE) {
					elems->wps_ie = data;
					elems->wps_ie_len = elen;
				} else if (vendor_type == P2P_IE_VENDOR_TYPE) {
					elems->p2p = data;
					elems->p2p_len = elen;
				} else if (vendor_type == WFD_IE_VENDOR_TYPE) {
					elems->wfd = data;
					elems->wfd_len = elen;
				} else if (vendor_type ==
					   QCA_VENDOR_IE_TYPE_P2P_PREF_CHAN_LIST) {
					elems->pref_freq_list = data;
					elems->pref_freq_list_len = elen;
				}
			}
			break;
		default:
			break;
		}

		pos += elen;
	}

	return pos == end ? ParseOK : ParseFailed;
}

int supp_rates_11b_only(struct ieee802_11_elems *elems)
{
	int i;
	int num_11b = 0;
	int num_others = 0;

	if (elems->supp_rates == NULL && elems->ext_supp_rates == NULL)
		return 0;

	for (i = 0; elems->supp_rates && i < elems->supp_rates_len; i++) {
		if (p2p_compat_is_11b_rate(elems->supp_rates[i]))
			num_11b++;
		else
			num_others++;
	}

	for (i = 0; elems->ext_supp_rates && i < elems->ext_supp_rates_len;
	     i++) {
		if (p2p_compat_is_11b_rate(elems->ext_supp_rates[i]))
			num_11b++;
		else
			num_others++;
	}

	return num_11b > 0 && num_others == 0;
}

size_t utf8_escape(const char *inp, size_t in_size,
		   char *outp, size_t out_size)
{
	size_t res_size = 0;

	if (!inp || !outp)
		return 0;

	if (!in_size)
		in_size = os_strlen(inp);

	while (in_size) {
		in_size--;
		if (res_size++ >= out_size)
			return 0;

		switch (*inp) {
		case '\\':
		case '\'':
			if (res_size++ >= out_size)
				return 0;
			*outp++ = '\\';
			/* fall through */
		default:
			*outp++ = *inp++;
			break;
		}
	}

	if (res_size < out_size)
		*outp = '\0';

	return res_size;
}

struct wpabuf *ieee802_11_vendor_ie_concat(const u8 *ies, size_t ies_len,
					   u32 oui_type)
{
	const u8 *pos = ies;
	const u8 *end = ies + ies_len;
	struct wpabuf *buf;
	size_t total = 0;

	while (end - pos >= 2) {
		u8 id = *pos++;
		u8 elen = *pos++;

		if (elen > end - pos)
			return NULL;
		if (id == WLAN_EID_VENDOR_SPECIFIC && elen >= 4 &&
		    WPA_GET_BE32(pos) == oui_type)
			total += elen - 4;
		pos += elen;
	}

	if (total == 0)
		return NULL;

	buf = wpabuf_alloc(total);
	if (buf == NULL)
		return NULL;

	pos = ies;
	while (end - pos >= 2) {
		u8 id = *pos++;
		u8 elen = *pos++;

		if (elen > end - pos) {
			wpabuf_free(buf);
			return NULL;
		}
		if (id == WLAN_EID_VENDOR_SPECIFIC && elen >= 4 &&
		    WPA_GET_BE32(pos) == oui_type)
			wpabuf_put_data(buf, pos + 4, elen - 4);
		pos += elen;
	}

	return buf;
}

int ieee80211_chan_to_freq(const char *country, u8 op_class, u8 chan)
{
	(void) country;

	switch (op_class) {
	case 81:
	case 83:
	case 84:
		if (chan >= 1 && chan <= 13)
			return 2407 + chan * 5;
		break;
	case 82:
		if (chan == 14)
			return 2484;
		break;
	default:
		if (chan >= 1 && chan <= 196)
			return 5000 + chan * 5;
		break;
	}

	return -1;
}

enum hostapd_hw_mode ieee80211_freq_to_channel_ext(unsigned int freq,
						   int sec_channel, int vht,
						   u8 *op_class, u8 *channel)
{
	int chan = p2p_compat_freq_to_channel(freq);

	(void) sec_channel;
	(void) vht;

	if (chan < 0)
		return NUM_HOSTAPD_MODES;

	if (channel)
		*channel = (u8) chan;

	if (freq == 2484) {
		if (op_class)
			*op_class = 82;
		return HOSTAPD_MODE_IEEE80211G;
	}

	if (freq >= 2412 && freq <= 2472) {
		if (op_class)
			*op_class = 81;
		return HOSTAPD_MODE_IEEE80211G;
	}

	if (op_class) {
		if (freq >= 5745)
			*op_class = 124;
		else
			*op_class = 115;
	}

	return HOSTAPD_MODE_IEEE80211A;
}


int is_6ghz_freq(int freq)
{
	return freq >= 5935 && freq <= 7115;
}


int is_6ghz_op_class(u8 op_class)
{
	return op_class >= 131 && op_class <= 136;
}
