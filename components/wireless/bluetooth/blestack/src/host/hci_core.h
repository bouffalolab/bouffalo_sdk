/* hci_core.h - Bluetooth HCI core access */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* LL connection parameters */
#define LE_CONN_LATENCY		0x0000
#define LE_CONN_TIMEOUT		0x002a

#if defined(CONFIG_BT_BREDR)
#define LMP_FEAT_PAGES_COUNT	3
#else
#define LMP_FEAT_PAGES_COUNT	1
#endif

/* SCO  settings */
#define BT_VOICE_CVSD_16BIT     0x0060
#define BT_VOICE_MSBC_16BIT     0x0063

/* k_poll event tags */
enum {
	BT_EVENT_CMD_TX,
	BT_EVENT_CONN_TX_QUEUE,
};

/* bt_dev flags: the flags defined here represent BT controller state */
enum {
	BT_DEV_ENABLE,
	BT_DEV_READY,
	BT_DEV_PRESET_ID,
	BT_DEV_USER_ID_ADDR,
	BT_DEV_HAS_PUB_KEY,
	BT_DEV_PUB_KEY_BUSY,

	BT_DEV_ADVERTISING,
	BT_DEV_ADVERTISING2,
	BT_DEV_ADVERTISING2_SETTING,
	BT_DEV_ADVERTISING_NAME,
	BT_DEV_ADVERTISING_CONNECTABLE,
	BT_DEV_KEEP_ADVERTISING,
	BT_DEV_SCANNING,
	BT_DEV_EXPLICIT_SCAN,
	BT_DEV_ACTIVE_SCAN,
	BT_DEV_SCAN_FILTER_DUP,
	BT_DEV_SCAN_WL,
	BT_DEV_AUTO_CONN,

	BT_DEV_RPA_VALID,

	BT_DEV_ID_PENDING,

#if defined(CONFIG_BT_BREDR)
	BT_DEV_ISCAN,
	BT_DEV_PSCAN,
	BT_DEV_INQUIRY,
#endif /* CONFIG_BT_BREDR */

#if defined(CONFIG_BT_STACK_PTS)
	BT_DEV_ADV_ADDRESS_IS_PUBLIC,
#endif

#if defined(CONFIG_BT_STACK_PTS) || defined(CONFIG_AUTO_PTS)
	BT_DEV_SETTED_NON_RESOLV_ADDR, //The non-reslovable address have been set.
#endif

#if defined(BFLB_HOST_ASSISTANT)
    BT_DEV_ASSIST_RUN,
#endif


	/* Total number of flags - must be at the end of the enum */
	BT_DEV_NUM_FLAGS,
};

/* Flags which should not be cleared upon HCI_Reset */
#define BT_DEV_PERSISTENT_FLAGS (BIT(BT_DEV_ENABLE) | \
				 BIT(BT_DEV_PRESET_ID) | \
				 BIT(BT_DEV_USER_ID_ADDR))

struct bt_dev_le {
	/* LE features */
	u8_t			features[8];
	/* LE states */
	u64_t			states;

#if defined(CONFIG_BT_CONN)
	/* Controller buffer information */
	u16_t			mtu;
	struct k_sem		pkts;
#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_SMP)
	/* Size of the the controller resolving list */
	u8_t                    rl_size;
	/* Number of entries in the resolving list. rl_entries > rl_size
	 * means that host-side resolving is used.
	 */
	u8_t                    rl_entries;
#endif /* CONFIG_BT_SMP */

#if defined(CONFIG_BT_WHITELIST)
	/* Size of the controller whitelist. */
	u8_t			wl_size;
	/* Number of entries in the resolving list. */
	u8_t			wl_entries;
#endif /* CONFIG_BT_WHITELIST */
};

enum {
	BT_A2DP_SOURCE_ROLE,
	BT_A2DP_SINK_ROLE,
};

#if defined(CONFIG_BT_BREDR)
struct bt_dev_br {
	/* Max controller's acceptable ACL packet length */
	u16_t         mtu;
	struct k_sem  pkts;
	u16_t         esco_pkt_type;
	u8_t         a2dp_role;
};
#endif

/* The theoretical max for these is 8 and 64, but there's no point
 * in allocating the full memory if we only support a small subset.
 * These values must be updated whenever the host implementation is
 * extended beyond the current values.
 */
#define BT_DEV_VS_FEAT_MAX  1
#define BT_DEV_VS_CMDS_MAX  2

/* State tracking for the local Bluetooth controller */
struct bt_dev {
	/* Local Identity Address(es) */
	bt_addr_le_t		id_addr[CONFIG_BT_ID_MAX];
	u8_t                    id_count;

	/* ID Address used for advertising */
	u8_t                    adv_id;

	/* Current local Random Address */
	bt_addr_le_t		random_addr;

	#if defined(BFLB_BLE)
	/* Current local Public Address */
	bt_addr_le_t		public_addr;
	#endif
	/* Controller version & manufacturer information */
	u8_t			hci_version;
	u8_t			lmp_version;
	u16_t			hci_revision;
	u16_t			lmp_subversion;
	u16_t			manufacturer;

	/* LMP features (pages 0, 1, 2) */
	u8_t			features[LMP_FEAT_PAGES_COUNT][8];

	/* Supported commands */
	u8_t			supported_commands[64];

#if defined(CONFIG_BT_HCI_VS_EXT)
	/* Vendor HCI support */
	u8_t                    vs_features[BT_DEV_VS_FEAT_MAX];
	u8_t                    vs_commands[BT_DEV_VS_CMDS_MAX];
#endif

	struct k_work           init;

	ATOMIC_DEFINE(flags, BT_DEV_NUM_FLAGS);

	/* LE controller specific features */
	struct bt_dev_le	le;

#if defined(CONFIG_BT_BREDR)
	/* BR/EDR controller specific features */
	struct bt_dev_br	br;
#endif

	/* Number of commands controller can accept */
	struct k_sem		ncmd_sem;

	/* Last sent HCI command */
	struct net_buf		*sent_cmd;

#if !defined(CONFIG_BT_RECV_IS_RX_THREAD)
	/* Queue for incoming HCI events & ACL data */
	struct k_fifo		rx_queue;
#endif

	/* Queue for outgoing HCI commands */
	struct k_fifo		cmd_tx_queue;

	/* Registered HCI driver */
	const struct bt_hci_driver *drv;

#if defined(CONFIG_BT_PRIVACY)
	/* Local Identity Resolving Key */
	u8_t			irk[CONFIG_BT_ID_MAX][16];

	/* Work used for RPA rotation */
	struct k_delayed_work rpa_update;
#endif

	/* Local Name */
#if defined(CONFIG_BT_DEVICE_NAME_DYNAMIC)
	char			name[CONFIG_BT_DEVICE_NAME_MAX + 1];
#endif
#if defined(BFLB_BLE_SMP_SUPPORT_DISABLE_PAIR)
    bool disable_pair;
#endif
};

#if defined (CONFIG_BT_STACK_PTS)
typedef enum __packed{
	dir_connect_req 		= 0x01, /*Send a direct connection require while the Lower test enters direct mode .*/

	ad_type_service_uuid 	= 0x02,    
	ad_type_local_name   	= 0x03,
	ad_type_flags     	 	= 0x04,
	ad_type_manu_data   	= 0x05,
	ad_type_tx_power_level 	= 0x06,
	ad_type_service_data 	= 0x07,
	ad_type_appearance 		= 0x08,

	gatt_discover_chara		= 0x09,
	gatt_exec_write_req		= 0x0a,
	gatt_cancel_write_req	= 0x0b,
	att_read_by_group_type_ind = 0x0c, /* CASE : GATT/SR/GAD/BV-01-C. Indicate PTS sends a GATT discover all primary services request to iut */ 
	att_find_by_type_value_ind = 0x0d, /* CASE : GATT/SR/GAD/BV-02-C. Indicate PTS sends a request to iut for discover it contains Primary Services by Service UUID */ 
	att_read_by_type_ind = 0x0e, /* CASE : GATT/SR/GAD/BV-04-C. Indicate PTS sends a request to iut for discover all characteristics of a specified service.*/ 

	own_addr_type_random = 0x0f
}event_id;

#endif 

extern struct bt_dev bt_dev;

#if !defined(BL602) && !defined(BL702)
struct bt_controller_sdk_ver{
    uint8_t status;
    uint8_t ver_major;
    uint8_t ver_minor;
    uint8_t ver_patch;
    uint32_t sdk_commit_id;
};
/*If this API return a value other than 0, it means it fails to get controller sdk version.*/
int bt_get_controller_sdk_version(struct bt_controller_sdk_ver *version);
#endif

#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
extern const struct bt_conn_auth_cb *bt_auth;
#endif /* CONFIG_BT_SMP || CONFIG_BT_BREDR */

bool bt_le_conn_params_valid(const struct bt_le_conn_param *param);

int bt_le_scan_update(bool fast_scan);

int bt_le_auto_conn(const struct bt_le_conn_param *conn_param);
int bt_le_auto_conn_cancel(void);

bool bt_addr_le_is_bonded(u8_t id, const bt_addr_le_t *addr);
const bt_addr_le_t *bt_lookup_id_addr(u8_t id, const bt_addr_le_t *addr);

int bt_send(struct net_buf *buf);

/* Don't require everyone to include keys.h */
struct bt_keys;
void bt_id_add(struct bt_keys *keys);
void bt_id_del(struct bt_keys *keys);

int bt_setup_id_addr(void);
void bt_finalize_init(void);

int bt_le_adv_start_internal(const struct bt_le_adv_param *param,
			     const struct bt_data *ad, size_t ad_len,
			     const struct bt_data *sd, size_t sd_len,
			     const bt_addr_le_t *peer);
#if defined(CONFIG_BLE_MULTI_ADV)
int bt_le_adv_start_instant(const struct bt_le_adv_param *param,
        const uint8_t *ad_data, size_t ad_len,
        const uint8_t *sd_data, size_t sd_len);
#endif

#if defined (BFLB_BLE)
int bt_le_read_rssi(u16_t handle,int8_t *rssi);
int set_ad_and_rsp_d(u16_t hci_op, u8_t *data, u32_t ad_len);
int set_adv_enable(bool enable);
int set_adv_param(const struct bt_le_adv_param *param);
int set_adv_channel_map(bt_gap_adv_chnl_map_t channel);
int bt_get_local_public_address(bt_addr_le_t *adv_addr);
int bt_get_local_ramdon_address(bt_addr_le_t *adv_addr);
int bt_set_local_public_address(u8_t *adv_addr);
int bt_le_set_data_len(struct bt_conn *conn, u16_t tx_octets, u16_t tx_time);
int hci_le_set_phy(struct bt_conn *conn, uint8_t all_phys,
		  uint8_t pref_tx_phy, uint8_t pref_rx_phy, uint8_t phy_opts);
int hci_le_set_default_phy(u8_t default_phy);
/* Notice: Use of this API will lead to mismatch between chip mac address
   and address set by this API.
   In some condition, BD address may be reset to chip mac address.*/
int bt_set_bd_addr(const bt_addr_t *addr);

int bt_set_tx_pwr(int8_t power);
#if defined(BL702L) || defined(BL602) || defined(BL702)
int8_t bt_get_tx_pwr(void);
#endif
int bt_le_read_chan_map(struct bt_conn *conn, struct bt_hci_rp_le_read_chan_map *rsp_buf);

#if defined(BL702L) || defined(BL616) || defined(BL606P) || defined(BL808)
int bt_le_throughput_calc(bool enable, u8_t interval);
int bt_le_set_conn_window(u8_t percentage);
#endif

int bt_le_enh_tx_test(u8_t tx_ch, u8_t test_data_len, u8_t pkt_payload, u8_t phy);
int bt_le_enh_rx_test(u8_t rx_ch, u8_t phy, u8_t mod_index);
int bt_ble_tx_test_cmd(u8_t tx_ch,u8_t  data_len,u8_t  pkt_payload);
int bt_ble_rx_test_cmd(u8_t rx_ch);
int bt_le_test_end(void);

#if defined(BFLB_HOST_ASSISTANT)
struct hast_cb{
    void (*le_scan_cb)(const struct bt_le_scan_param *param, bt_le_scan_cb_t cb);
    void (*le_adv_cb)(const struct bt_le_adv_param *param, const struct bt_data *ad, 
		size_t ad_len, const struct bt_data *sd, size_t sd_len);
};
int bt_set_flow_control(void);
int bt_set_event_mask(void);
int bt_le_set_event_mask(void);
void bt_hci_reset_complete(struct net_buf *buf);
void bt_register_host_assist_cb(struct hast_cb *cb);
#endif

typedef void (*bredr_name_callback)(const char *name);
int remote_name_req(const bt_addr_t *addr, bredr_name_callback cb);
bool bt_is_ready(void);
#endif
