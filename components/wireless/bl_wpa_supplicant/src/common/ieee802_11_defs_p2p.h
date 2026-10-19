#ifndef IEEE802_11_DEFS_P2P_H
#define IEEE802_11_DEFS_P2P_H

#include "ieee802_11_defs.h"

#ifndef WLAN_ACTION_VENDOR_SPECIFIC_PROTECTED
#define WLAN_ACTION_VENDOR_SPECIFIC_PROTECTED 126
#endif

#ifndef WLAN_ACTION_VENDOR_SPECIFIC
#define WLAN_ACTION_VENDOR_SPECIFIC 127
#endif

#ifndef P2P_IE_VENDOR_TYPE
#define P2P_IE_VENDOR_TYPE 0x506f9a09
#endif

#ifndef WFD_IE_VENDOR_TYPE
#define WFD_IE_VENDOR_TYPE 0x506f9a0a
#endif

#ifndef WPA_IE_VENDOR_TYPE
#define WPA_IE_VENDOR_TYPE 0x0050f201
#endif

#ifndef WMM_IE_VENDOR_TYPE
#define WMM_IE_VENDOR_TYPE 0x0050f202
#endif

#ifndef WPS_IE_VENDOR_TYPE
#define WPS_IE_VENDOR_TYPE 0x0050f204
#endif

#ifndef WLAN_EID_ADV_PROTO
#define WLAN_EID_ADV_PROTO 108
#endif

#ifndef ACCESS_NETWORK_QUERY_PROTOCOL
enum adv_protocol_id {
	ACCESS_NETWORK_QUERY_PROTOCOL = 0
};
#endif

#ifndef ANQP_VENDOR_SPECIFIC
enum anqp_info_id {
	ANQP_VENDOR_SPECIFIC = 56797
};
#endif

#ifndef P2P_OUI_TYPE
#define P2P_OUI_TYPE 9
#endif

#ifndef WFD_OUI_TYPE
#define WFD_OUI_TYPE 10
#endif

#ifndef P2P_ATTR_STATUS
enum p2p_attr_id {
	P2P_ATTR_STATUS = 0,
	P2P_ATTR_MINOR_REASON_CODE = 1,
	P2P_ATTR_CAPABILITY = 2,
	P2P_ATTR_DEVICE_ID = 3,
	P2P_ATTR_GROUP_OWNER_INTENT = 4,
	P2P_ATTR_CONFIGURATION_TIMEOUT = 5,
	P2P_ATTR_LISTEN_CHANNEL = 6,
	P2P_ATTR_GROUP_BSSID = 7,
	P2P_ATTR_EXT_LISTEN_TIMING = 8,
	P2P_ATTR_INTENDED_INTERFACE_ADDR = 9,
	P2P_ATTR_MANAGEABILITY = 10,
	P2P_ATTR_CHANNEL_LIST = 11,
	P2P_ATTR_NOTICE_OF_ABSENCE = 12,
	P2P_ATTR_DEVICE_INFO = 13,
	P2P_ATTR_GROUP_INFO = 14,
	P2P_ATTR_GROUP_ID = 15,
	P2P_ATTR_INTERFACE = 16,
	P2P_ATTR_OPERATING_CHANNEL = 17,
	P2P_ATTR_INVITATION_FLAGS = 18,
	P2P_ATTR_OOB_GO_NEG_CHANNEL = 19,
	P2P_ATTR_SERVICE_HASH = 21,
	P2P_ATTR_SESSION_INFORMATION_DATA = 22,
	P2P_ATTR_CONNECTION_CAPABILITY = 23,
	P2P_ATTR_ADVERTISEMENT_ID = 24,
	P2P_ATTR_ADVERTISED_SERVICE = 25,
	P2P_ATTR_SESSION_ID = 26,
	P2P_ATTR_FEATURE_CAPABILITY = 27,
	P2P_ATTR_PERSISTENT_GROUP = 28,
	P2P_ATTR_VENDOR_SPECIFIC = 221
};
#endif

#ifndef P2P_MAX_GO_INTENT
#define P2P_MAX_GO_INTENT 15
#endif

#ifndef P2P_DEV_CAPAB_SERVICE_DISCOVERY
#define P2P_DEV_CAPAB_SERVICE_DISCOVERY BIT(0)
#define P2P_DEV_CAPAB_CLIENT_DISCOVERABILITY BIT(1)
#define P2P_DEV_CAPAB_CONCURRENT_OPER BIT(2)
#define P2P_DEV_CAPAB_INFRA_MANAGED BIT(3)
#define P2P_DEV_CAPAB_DEVICE_LIMIT BIT(4)
#define P2P_DEV_CAPAB_INVITATION_PROCEDURE BIT(5)
#define P2P_DEV_CAPAB_6GHZ_BAND_CAPABLE BIT(6)
#endif

#ifndef P2P_GROUP_CAPAB_GROUP_OWNER
#define P2P_GROUP_CAPAB_GROUP_OWNER BIT(0)
#define P2P_GROUP_CAPAB_PERSISTENT_GROUP BIT(1)
#define P2P_GROUP_CAPAB_GROUP_LIMIT BIT(2)
#define P2P_GROUP_CAPAB_INTRA_BSS_DIST BIT(3)
#define P2P_GROUP_CAPAB_CROSS_CONN BIT(4)
#define P2P_GROUP_CAPAB_PERSISTENT_RECONN BIT(5)
#define P2P_GROUP_CAPAB_GROUP_FORMATION BIT(6)
#define P2P_GROUP_CAPAB_IP_ADDR_ALLOCATION BIT(7)
#endif

#ifndef P2PS_FEATURE_CAPAB_UDP_TRANSPORT
#define P2PS_FEATURE_CAPAB_UDP_TRANSPORT BIT(0)
#define P2PS_FEATURE_CAPAB_MAC_TRANSPORT BIT(1)
struct p2ps_feature_capab {
	u8 cpt;
	u8 reserved;
} STRUCT_PACKED;
#endif

#ifndef P2P_DEVICE_NOT_IN_GROUP
enum p2p_role_indication {
	P2P_DEVICE_NOT_IN_GROUP = 0x00,
	P2P_CLIENT_IN_A_GROUP = 0x01,
	P2P_GO_IN_A_GROUP = 0x02,
};
#endif

#ifndef P2P_SC_SUCCESS
enum p2p_status_code {
	P2P_SC_SUCCESS = 0,
	P2P_SC_FAIL_INFO_CURRENTLY_UNAVAILABLE = 1,
	P2P_SC_FAIL_INCOMPATIBLE_PARAMS = 2,
	P2P_SC_FAIL_LIMIT_REACHED = 3,
	P2P_SC_FAIL_INVALID_PARAMS = 4,
	P2P_SC_FAIL_UNABLE_TO_ACCOMMODATE = 5,
	P2P_SC_FAIL_PREV_PROTOCOL_ERROR = 6,
	P2P_SC_FAIL_NO_COMMON_CHANNELS = 7,
	P2P_SC_FAIL_UNKNOWN_GROUP = 8,
	P2P_SC_FAIL_BOTH_GO_INTENT_15 = 9,
	P2P_SC_FAIL_INCOMPATIBLE_PROV_METHOD = 10,
	P2P_SC_FAIL_REJECTED_BY_USER = 11,
	P2P_SC_SUCCESS_DEFERRED = 12,
};
#endif

#ifndef P2P_WILDCARD_SSID
#define P2P_WILDCARD_SSID "DIRECT-"
#define P2P_WILDCARD_SSID_LEN 7
#endif

#ifndef P2P_NOA
enum p2p_act_frame_type {
	P2P_NOA = 0,
	P2P_PRESENCE_REQ = 1,
	P2P_PRESENCE_RESP = 2,
	P2P_GO_DISC_REQ = 3
};
#endif

#ifndef P2P_GO_NEG_REQ
enum p2p_action_frame_type {
	P2P_GO_NEG_REQ = 0,
	P2P_GO_NEG_RESP = 1,
	P2P_GO_NEG_CONF = 2,
	P2P_INVITATION_REQ = 3,
	P2P_INVITATION_RESP = 4,
	P2P_DEV_DISC_REQ = 5,
	P2P_DEV_DISC_RESP = 6,
	P2P_PROV_DISC_REQ = 7,
	P2P_PROV_DISC_RESP = 8
};
#endif

#ifndef P2P_MAN_CROSS_CONNECTION_PERMITTED
#define P2P_MAN_DEVICE_MANAGEMENT BIT(0)
#define P2P_MAN_CROSS_CONNECTION_PERMITTED BIT(1)
#define P2P_MAN_COEXISTENCE_OPTIONAL BIT(2)
#endif

#ifndef P2P_INVITATION_FLAGS_TYPE
#define P2P_INVITATION_FLAGS_TYPE BIT(0)
#endif

#ifndef BL_P2P_COMPAT_IEEE80211_MGMT_DEFINED
#define BL_P2P_COMPAT_IEEE80211_MGMT_DEFINED
struct ieee80211_mgmt {
	le16 frame_control;
	le16 duration;
	u8 da[6];
	u8 sa[6];
	u8 bssid[6];
	le16 seq_ctrl;
	union {
		struct {
			le16 auth_alg;
			le16 auth_transaction;
			le16 status_code;
			u8 variable[];
		} STRUCT_PACKED auth;
		struct {
			le16 reason_code;
			u8 variable[];
		} STRUCT_PACKED deauth;
		struct {
			le16 capab_info;
			le16 listen_interval;
			u8 variable[];
		} STRUCT_PACKED assoc_req;
		struct {
			le16 capab_info;
			le16 status_code;
			le16 aid;
			u8 variable[];
		} STRUCT_PACKED assoc_resp, reassoc_resp;
		struct {
			le16 capab_info;
			le16 listen_interval;
			u8 current_ap[6];
			u8 variable[];
		} STRUCT_PACKED reassoc_req;
		struct {
			le16 reason_code;
			u8 variable[];
		} STRUCT_PACKED disassoc;
		struct {
			u8 timestamp[8];
			le16 beacon_int;
			le16 capab_info;
			u8 variable[];
		} STRUCT_PACKED beacon;
		struct {
			u8 timestamp[8];
			le16 beacon_int;
			le16 capab_info;
			u8 variable[];
		} STRUCT_PACKED probe_resp;
		struct {
			u8 category;
			union {
				struct {
					u8 action;
					u8 variable[];
				} STRUCT_PACKED public_action;
				struct {
					u8 action;
					u8 oui[3];
					u8 variable[];
				} STRUCT_PACKED vs_public_action;
				struct {
					u8 action;
					u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
					u8 variable[];
				} STRUCT_PACKED sa_query_req;
				struct {
					u8 action;
					u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
					u8 variable[];
				} STRUCT_PACKED sa_query_resp;
			} u;
		} STRUCT_PACKED action;
	} u;
} STRUCT_PACKED;
#endif

#endif