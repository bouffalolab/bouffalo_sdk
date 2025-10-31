/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MESH_MAIN_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_MESH_MAIN_H_

/**
 * @brief Bluetooth Mesh Provisioning
 * @defgroup bt_mesh_prov Bluetooth Mesh Provisioning
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "include/access.h" /* Add by bouffalo */
#include "prov.h" /* Add by bouffalo */

/** Available authentication algorithms. */
enum {
	BT_MESH_PROV_AUTH_CMAC_AES128_AES_CCM,
	BT_MESH_PROV_AUTH_HMAC_SHA256_AES_CCM,
};

/** OOB Type field values. */
enum {
	BT_MESH_STATIC_OOB_AVAILABLE = BIT(0), /**< Static OOB information available */
	BT_MESH_OOB_AUTH_REQUIRED    = BIT(1)  /**< OOB authentication required */
};

/** Available Provisioning output authentication actions. */
typedef enum {
	BT_MESH_NO_OUTPUT       = 0,
	BT_MESH_BLINK           = BIT(0),
	BT_MESH_BEEP            = BIT(1),
	BT_MESH_VIBRATE         = BIT(2),
	BT_MESH_DISPLAY_NUMBER  = BIT(3),
	BT_MESH_DISPLAY_STRING  = BIT(4),
} bt_mesh_output_action_t;

/** Available Provisioning input authentication actions. */
typedef enum {
	BT_MESH_NO_INPUT      = 0,
	BT_MESH_PUSH          = BIT(0),
	BT_MESH_TWIST         = BIT(1),
	BT_MESH_ENTER_NUMBER  = BIT(2),
	BT_MESH_ENTER_STRING  = BIT(3),
} bt_mesh_input_action_t;

/** Available Provisioning bearers. */
typedef enum {
	BT_MESH_PROV_ADV   = BIT(0),
	BT_MESH_PROV_GATT  = BIT(1),
	/* Add by bouffalolab */
	BT_MESH_PROV_GATT_ADV  = BIT(0)|BIT(1),
} bt_mesh_prov_bearer_t;

/** Out of Band information location. */
typedef enum {
	BT_MESH_PROV_OOB_OTHER     = BIT(0),
	BT_MESH_PROV_OOB_URI       = BIT(1),
	BT_MESH_PROV_OOB_2D_CODE   = BIT(2),
	BT_MESH_PROV_OOB_BAR_CODE  = BIT(3),
	BT_MESH_PROV_OOB_NFC       = BIT(4),
	BT_MESH_PROV_OOB_NUMBER    = BIT(5),
	BT_MESH_PROV_OOB_STRING    = BIT(6),
	BT_MESH_PROV_CERT_BASED    = BIT(7),
	BT_MESH_PROV_RECORDS       = BIT(8),
	/* 7 - 10 are reserved */
	BT_MESH_PROV_OOB_ON_BOX    = BIT(11),
	BT_MESH_PROV_OOB_IN_BOX    = BIT(12),
	BT_MESH_PROV_OOB_ON_PAPER  = BIT(13),
	BT_MESH_PROV_OOB_IN_MANUAL = BIT(14),
	BT_MESH_PROV_OOB_ON_DEV    = BIT(15),
} bt_mesh_prov_oob_info_t;

#define PROC_RECORDS_CERTIFICATE_BASED_PROVISIONING_BASE_URI    0x0000
#define PROC_RECORDS_DEVICE_CERTIFICATE                         0x0001
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_1                 0x0002
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_2                 0x0003
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_3                 0x0004
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_4                 0x0005
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_5                 0x0006
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_6                 0x0007
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_7                 0x0008
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_8                 0x0009
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_9                 0x000A
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_10                0x000B
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_11                0x000C
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_12                0x000D
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_13                0x000E
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_14                0x000F
#define PROC_RECORDS_INTERMEDIATE_CERTIFICATE_15                0x0010
#define PROC_RECORDS_COMPLETE_LOCAL_NAME                        0x0011
#define PROC_RECORDS_APPEARANCE                                 0x0012
#define PROC_RECORDS_MAX                                        0x0013

/** Provisioning properties & capabilities. */
struct bt_mesh_prov {
	/** The UUID that's used when advertising as unprovisioned */
	const u8_t *uuid;

	/** Optional URI. This will be advertised separately from the
	 *  unprovisioned beacon, however the unprovisioned beacon will
	 *  contain a hash of it so the two can be associated by the
	 *  provisioner.
	 */
	const char *uri;

	/** Out of Band information field. */
	bt_mesh_prov_oob_info_t oob_info;

	/** Flag indicates whether unprovisioned devices support OOB public key */
	bool oob_pub_key;

	/** Static OOB value */
	const u8_t *static_val;
	/** Static OOB value length */
	u8_t        static_val_len;

	/** Maximum size of Output OOB supported */
	u8_t        output_size;
	/** Supported Output OOB Actions */
	u16_t       output_actions;

	/** Maximum size of Input OOB supported */
	u8_t        input_size;
	/** Supported Input OOB Actions */
	u16_t       input_actions;

#ifdef CONFIG_BT_MESH_PROVISIONER
	/** Add by bouffalo
	 * prov_caps include provisionee's capabilities;
	 * prov_start will be set by user, for provisioner used capability.
	 */
	u8_t (*capabilities)(prov_caps_t* prv_caps, prov_start_t* prv_start);
#endif
	/** @brief Output of a number is requested.
	 *
	 *  This callback notifies the application that it should
	 *  output the given number using the given action.
	 *
	 *  @param act Action for outputting the number.
	 *  @param num Number to be outputted.
	 *
	 *  @return Zero on success or negative error code otherwise
	 */
	int         (*output_number)(bt_mesh_output_action_t act, u32_t num);

	/** @brief Output of a string is requested.
	 *
	 *  This callback notifies the application that it should
	 *  display the given string to the user.
	 *
	 *  @param str String to be displayed.
	 *
	 *  @return Zero on success or negative error code otherwise
	 */
	int         (*output_string)(const char *str);

	/** @brief Input is requested.
	 *
	 *  This callback notifies the application that it should
	 *  request input from the user using the given action. The
	 *  requested input will either be a string or a number, and
	 *  the application needs to consequently call the
	 *  bt_mesh_input_string() or bt_mesh_input_number() functions
	 *  once the data has been acquired from the user.
	 *
	 *  @param act Action for inputting data.
	 *  @param num Maximum size of the inputted data.
	 *
	 *  @return Zero on success or negative error code otherwise
	 */
	int         (*input)(bt_mesh_input_action_t act, u8_t size);

	/** @brief The other device finished their OOB input.
	 *
	 *  This callback notifies the application that it should stop
	 *  displaying its output OOB value, as the other party finished their
	 *  OOB input.
	 */
	void 	    (*input_complete)(void);

	/** @brief Unprovisioned beacon has been received.
	 *
	 *  This callback notifies the application that an unprovisioned
	 *  beacon has been received.
	 *
	 *  @param uuid     UUID
	 *  @param oob_info OOB Information
	 *  @param uri_hash Pointer to URI Hash value. NULL if no hash was
	 *                  present in the beacon.
	 */
	void        (*unprovisioned_beacon)(u8_t uuid[16],
					    bt_mesh_prov_oob_info_t oob_info,
					    u32_t *uri_hash);

	/** @brief Provisioning link has been opened.
	 *
	 *  This callback notifies the application that a provisioning
	 *  link has been opened on the given provisioning bearer.
	 *
	 *  @param bearer Provisioning bearer.
	 */
	void        (*link_open)(bt_mesh_prov_bearer_t bearer);

	/** @brief Provisioning link has been closed.
	 *
	 *  This callback notifies the application that a provisioning
	 *  link has been closed on the given provisioning bearer.
	 *
	 *  @param bearer Provisioning bearer.
	 */
	void        (*link_close)(bt_mesh_prov_bearer_t bearer);

	/** @brief Provisioning is complete.
	 *
	 *  This callback notifies the application that provisioning has
	 *  been successfully completed, and that the local node has been
	 *  assigned the specified NetKeyIndex and primary element address.
	 *
	 *  @param net_idx NetKeyIndex given during provisioning.
	 *  @param addr    Primary element address.
	 */
	void        (*complete)(u16_t net_idx, u16_t addr);

	/** @brief A new node has been added to the provisioning database.
	 *
	 *  This callback notifies the application that provisioning has
	 *  been successfully completed, and that a node has been assigned
	 *  the specified NetKeyIndex and primary element address.
	 *
	 *  @param net_idx  NetKeyIndex given during provisioning.
	 *  @param uuid     UUID of the added node
	 *  @param addr     Primary element address.
	 *  @param num_elem Number of elements that this node has.
	 */
	void        (*node_added)(u16_t net_idx, u8_t uuid[16], u16_t addr,
				  u8_t num_elem);

	/** @brief Node has been reset.
	 *
	 *  This callback notifies the application that the local node
	 *  has been reset and needs to be reprovisioned. The node will
	 *  not automatically advertise as unprovisioned, rather the
	 *  bt_mesh_prov_enable() API needs to be called to enable
	 *  unprovisioned advertising on one or more provisioning bearers.
	 */
	void        (*reset)(void);
#ifdef CONFIG_BT_MESH_PROVISIONER
	/** @brief Added by bouffalo lab , add role type.
	 * 0 For node.
	 * 1 For provisioner.
	 */
	u8_t role;
#endif
    /* Add by bouffalolab */
#ifdef CONFIG_BT_MESH_MOD_BIND_CB
	int (*mod_bind_cb)(struct bt_mesh_model *model, 
            u16_t net_idx, u16_t mod_app_idx);
#endif /* CONFIG_BT_MESH_MOD_BIND_CB */
    /* Add by bouffalolab */
#ifdef CONFIG_BT_MESH_APPKEY_ADD_CB
	int (*app_key_add_cb)(u16_t net_idx, u16_t mod_app_idx);
#endif /* CONFIG_BT_MESH_APPKEY_ADD_CB */
    /* Add by bouffalolab */
#ifdef CONFIG_BT_MESH_MOD_SUB_ADD_CB
    int (*mod_sub_add_cb)(struct bt_mesh_model *model, 
                u16_t elem_addr, u16_t group_addr);
#endif /* CONFIG_BT_MESH_MOD_SUB_ADD_CB */
#if defined(CONFIG_BLE_MESH_CERT_BASED_PROV)
    int (*node_cert_based_prov_init_cb)(void);
    int (*node_cert_based_prov_deinit_cb)(void);
    int (*prov_record_req_cb)(struct net_buf_simple *buf, uint16_t record_id, uint16_t offset, uint16_t max_size);
    int (*prov_records_get_cb)(struct net_buf_simple *buf);
#endif /* CONFIG_BLE_MESH_CERT_BASED_PROV */
};

/** @brief Provide provisioning input OOB string.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BT_MESH_ENTER_STRING as the action.
 *
 *  @param str String.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_input_string(const char *str);

/** @brief Provide provisioning input OOB number.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BT_MESH_ENTER_NUMBER as the action.
 *
 *  @param num Number.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_input_number(u32_t num);

#if defined(CONFIG_BT_MESH_PTS) || defined(CONFIG_AUTO_PTS)
/** @brief Provide Device public key.
 *
 *  @param public_key Device public key.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_remote_pub_key_set(const uint8_t public_key[64]);
#endif /* CONFIG_AUTO_PTS */

/** @brief Enable specific provisioning bearers
 *
 *  Enable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise or of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers);

/** @brief Disable specific provisioning bearers
 *
 *  Disable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise or of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers);

/** @brief Provision the local Mesh Node.
 *
 *  This API should normally not be used directly by the application. The
 *  only exception is for testing purposes where manual provisioning is
 *  desired without an actual external provisioner.
 *
 *  @param net_key  Network Key
 *  @param net_idx  Network Key Index
 *  @param flags    Provisioning Flags
 *  @param iv_index IV Index
 *  @param addr     Primary element address
 *  @param dev_key  Device Key
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx,
		      u8_t flags, u32_t iv_index, u16_t addr,
		      const u8_t dev_key[16]);

/** @brief Provision a Mesh Node using PB-ADV
 *
 *  @param uuid               UUID
 *  @param net_idx            Network Key Index
 *  @param addr               Address to assign to remote device. If addr is 0,
 *                            the lowest available address will be chosen.
 *  @param attention_duration The attention duration to be send to remote device
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_provision_adv(const u8_t uuid[16], u16_t net_idx, u16_t addr,
			  u8_t attention_duration);

/** @brief Check if the local node has been provisioned.
 *
 *  This API can be used to check if the local node has been provisioned
 *  or not. It can e.g. be helpful to determine if there was a stored
 *  network in flash, i.e. if the network was restored after calling
 *  settings_load().
 *
 *  @return True if the node is provisioned. False otherwise.
 */
bool bt_mesh_is_provisioned(void);

/**
 * @}
 */

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh Bluetooth Mesh
 * @ingroup bluetooth
 * @{
 */

/** Bluetooth Mesh feature states */
enum bt_mesh_feat_state {
	/** Feature is supported, but disabled. */
	BT_MESH_FEATURE_DISABLED,
	/** Feature is supported and enabled. */
	BT_MESH_FEATURE_ENABLED,
	/** Feature is not supported, and cannot be enabled. */
	BT_MESH_FEATURE_NOT_SUPPORTED,
};

/* Primary Network Key index */
#define BT_MESH_NET_PRIMARY                 0x000

/* Added by bouffalo lab
 * Primary Application Key index 
 */
#define BT_MESH_APP_PRIMARY                 0x000

#define BT_MESH_RELAY_DISABLED              0x00
#define BT_MESH_RELAY_ENABLED               0x01
#define BT_MESH_RELAY_NOT_SUPPORTED         0x02

#define BT_MESH_BEACON_DISABLED             0x00
#define BT_MESH_BEACON_ENABLED              0x01

#define BT_MESH_PRIV_BEACON_DISABLED        BT_MESH_FEATURE_DISABLED
#define BT_MESH_PRIV_BEACON_ENABLED         BT_MESH_FEATURE_ENABLED

#define BT_MESH_GATT_PROXY_DISABLED         0x00
#define BT_MESH_GATT_PROXY_ENABLED          0x01
#define BT_MESH_GATT_PROXY_NOT_SUPPORTED    0x02

#define BT_MESH_PRIV_GATT_PROXY_DISABLED      BT_MESH_FEATURE_DISABLED
#define BT_MESH_PRIV_GATT_PROXY_ENABLED       BT_MESH_FEATURE_ENABLED
#define BT_MESH_PRIV_GATT_PROXY_NOT_SUPPORTED BT_MESH_FEATURE_NOT_SUPPORTED

#define BT_MESH_FRIEND_DISABLED             0x00
#define BT_MESH_FRIEND_ENABLED              0x01
#define BT_MESH_FRIEND_NOT_SUPPORTED        0x02

#define BT_MESH_NODE_IDENTITY_STOPPED       0x00
#define BT_MESH_NODE_IDENTITY_RUNNING       0x01
#define BT_MESH_NODE_IDENTITY_NOT_SUPPORTED 0x02

/* Features */
#define BT_MESH_FEAT_RELAY                  BIT(0)
#define BT_MESH_FEAT_PROXY                  BIT(1)
#define BT_MESH_FEAT_FRIEND                 BIT(2)
#define BT_MESH_FEAT_LOW_POWER              BIT(3)
#define BT_MESH_FEAT_SUPPORTED              (BT_MESH_FEAT_RELAY |   \
					     BT_MESH_FEAT_PROXY |   \
					     BT_MESH_FEAT_FRIEND |  \
					     BT_MESH_FEAT_LOW_POWER)

/** @brief Initialize Mesh support
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 *  @param prov Node provisioning information.
 *  @param comp Node Composition.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_init(const struct bt_mesh_prov *prov,
		 const struct bt_mesh_comp *comp);

/** @brief Reset the state of the local Mesh node.
 *
 *  Resets the state of the node, which means that it needs to be
 *  reprovisioned to become an active node in a Mesh network again.
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 */
void bt_mesh_reset(void);

/** @brief Suspend the Mesh network temporarily.
 *
 *  This API can be used for power saving purposes, but the user should be
 *  aware that leaving the local node suspended for a long period of time
 *  may cause it to become permanently disconnected from the Mesh network.
 *  If at all possible, the Friendship feature should be used instead, to
 *  make the node into a Low Power Node.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_suspend(void);

/** @brief Resume a suspended Mesh network.
 *
 *  This API resumes the local node, after it has been suspended using the
 *  bt_mesh_suspend() API.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_resume(void);

/** @brief Toggle the IV Update test mode
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @param enable true to enable IV Update test mode, false to disable it.
 */
void bt_mesh_iv_update_test(bool enable);

/** @brief Toggle the IV Update state
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @return true if IV Update In Progress state was entered, false otherwise.
 */
bool bt_mesh_iv_update(void);

/** @brief Toggle the Low Power feature of the local device
 *
 *  Enables or disables the Low Power feature of the local device. This is
 *  exposed as a run-time feature, since the device might want to change
 *  this e.g. based on being plugged into a stable power source or running
 *  from a battery power source.
 *
 *  @param enable  true to enable LPN functionality, false to disable it.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_set(bool enable);

/** @brief Send out a Friend Poll message.
 *
 *  Send a Friend Poll message to the Friend of this node. If there is no
 *  established Friendship the function will return an error.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_poll(void);

/** @brief Register a callback for Friendship changes.
 *
 *  Registers a callback that will be called whenever Friendship gets
 *  established or is lost.
 *
 *  @param cb Function to call when the Friendship status changes.
 */
void bt_mesh_lpn_set_cb(void (*cb)(u16_t friend_addr, bool established));

#ifdef CONFIG_BT_MESH_MOD_BIND_CB
void bt_mesh_mod_bind_cb(struct bt_mesh_model *model, u16_t net_idx, u16_t mod_app_idx);
#endif /* CONFIG_BT_MESH_MOD_BIND_CB */

#ifdef CONFIG_BT_MESH_APPKEY_ADD_CB
void bt_mesh_app_key_add_cb(u16_t net_idx, u16_t mod_app_idx);
#endif /* CONFIG_BT_MESH_APPKEY_ADD_CB */

#ifdef CONFIG_BT_MESH_MOD_SUB_ADD_CB
void bt_mesh_mod_sub_add_cb(struct bt_mesh_model *model, u16_t elem_addr, 
            u16_t group_addr);
#endif /* CONFIG_BT_MESH_MOD_SUB_ADD_CB */

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MESH_MAIN_H_ */
