#ifndef BLE_LIB_API_H_
#define BLE_LIB_API_H_

#include <stdbool.h>
#include <stdint.h> 
#include "btble_test_cmd.h"

#if defined(CONFIG_BLE_BEACON_ONLY)
#include "btble_adv_api.h"
#endif /* CONFIG_BLE_BEACON_ONLY */


struct btblecontroller_resource_conf{
    //The size of bluetooth EM area
    uint32_t em_size;
    //If allocate resource for ble observer. If CONFIG_BT_OBSERVER is enabled, shall allocate this resource.
    //Otherwise, not allocate.
    //1:allocate, 0:not allocate.
    uint8_t ble_observer;
    //If allocate resource for ble central. If CONFIG_BT_CENTRAL is enabled, shall allocate this resource.
    //Otherwise, not allocate.
    //1:allocate, 0:not allocate.
    uint8_t ble_central;
    //If allocate resource for ble extended adv.  If CONFIG_ADV_EXTENSION is enabled, shall allocate this resource.
    //Otherwise, not allocate.
    //1:allocate, 0:not allocate.
    uint8_t ble_ext_adv;
    //Number of max activities
    uint8_t ble_activity_max;
    //Number of max ble links
    uint8_t ble_conn_max;
    //Maximum number of devices in resolving address list
    uint8_t ble_ral_max;
    //Number of RX descriptors
    uint8_t ble_rx_desc_nb;
    //Number of TX data buffer
    uint8_t ble_acl_buf_nb_tx;
};

//This API is only used in ble only mode without iso/cte to configure ble resource when CONFIG_BLE_RES_DYNAMIC_CONF is enabled
//and shall be called before btble_controller_init.
void btble_controller_resource_config(struct btblecontroller_resource_conf *conf);

//Set stack size of btblecontroller task before btble_controller_init if upper layer wants to modify the stack size.
//The default stack size of btblecontroller task is 2k in ble only mode and 4k in bt/ble mode.
void btble_controller_set_task_stack_size(uint16_t stack_size);
void btble_controller_init(uint8_t task_priority);
#if defined(CONFIG_NUTTX)
void btblecontroller_main( void *pvParameters );
#endif
//API for different RTOS porting to handle btbelcontroller task's messages.
void btblecontroller_proc(void *data);

void btble_controller_deinit(void);
int32_t btble_controller_sleep(int32_t max_sleep_cycles);
void btble_controller_sleep_restore();

#define BTBLE_IN_ACTIVE_STATE 0
#define BTBLE_IN_SLEEP_STATE 1
#define BTBLE_IN_WAKEUP_ONGOING_STATE 2
//The return value is an instantaneous state that may change rapidly.
uint8_t btble_controller_get_state(void);

void btble_controller_reset(void);

#if defined(BL618DG)
/**
 *  Force the shared RF to the Bluetooth-only state on BL618DG.
 */
void btble_controller_set_coex_bt_only(void);

/**
 *  Force the shared RF to the WiFi-only state on BL618DG.
 */
void btble_controller_set_coex_wifi_only(void);

#endif
typedef struct {
    /* BLE EM control structure byte offset. */
    uint16_t cs_off;
    /* First legacy advertising TX descriptor byte offset. */
    uint16_t tx0_off;
    /* Second TX descriptor byte offset, 0 if absent. */
    uint16_t tx1_off;
    /* Advertising PDU payload byte offset in BLE EM. */
    uint16_t adv_data_off;
    /* Full advertising PDU payload length, including AdvA. */
    uint8_t adv_pdu_len;
    /* Advertising activity ID. */
    uint8_t act_id;
    /* Current advertising interval. */
    uint16_t adv_interval;
    /* Next adv prog time in half-slots. */
    uint32_t next_hs;
    /* Next adv prog time in half-microseconds. */
    uint16_t next_hus;
} btble_controller_lp_fw_adv_info_t;

/*
 * Slave connection activity recorded for the LPFW (LTOS connected-state
 * exploration).  Timestamps are BLE half-slots (312.5 us) unless noted;
 * bit offsets are half-microseconds.
 */
typedef struct {
    /* Connection control structure byte offset in BLE EM. */
    uint16_t cs_off;
    /* Connection interval, half-slots. */
    uint16_t interval;
    /* Slave latency. */
    uint16_t latency;
    /* Event counter of the last event run by the controller. */
    uint16_t evt_cnt;
    /* Amount by which the connection event counter should be incremented. */
    uint16_t evt_inc;
    /* Master sleep clock accuracy, ppm. */
    uint16_t master_sca;
    /* Local sleep clock drift used for RX window widening, ppm. */
    uint16_t local_drift;
    /* CSA#1: channel index programmed for the last event. */
    uint16_t last_cs_ch_idx;
    /* Next anchor target (middle of the RX window), half-slots. */
    uint32_t next_ts;
    /* Exchange-table program time of the pending event (window start). */
    uint32_t prog_hs;
    uint16_t prog_hus;
    /* Half-us bit offset stored with the pending event (window start). */
    int16_t  next_bit_off;
    /* Scheduled RX window of the pending event, half-us. */
    uint32_t sync_win_size;
    /* Last anchor with a sync, half-slots / half-us. */
    uint32_t last_sync_ts;
    int16_t  last_sync_bit_off;
    /* Last anchor with a CRC-correct packet (supervision reference). */
    uint32_t last_crc_ok_ts;
    /* Supervision timeout, half-slots. */
    uint32_t timeout;
    /* Minimum event duration, half-us. */
    uint32_t duration_min;
    /* Rates (enum lld_rate: 0 = 1M, 1 = 2M). */
    uint8_t  rx_rate;
    uint8_t  tx_rate;
    /* 1: CSA#1 (software computes the channel), 0: CSA#2 (hardware). */
    uint8_t  hop_sel_1;
    uint8_t  hop_inc;
    uint8_t  link_id;
    /* RX encryption enabled (informational, the hardware does the CCM). */
    uint8_t  encrypted;
    /* Arbiter priority of the pending event. */
    uint8_t  current_prio;
    /* Slave latency was applied to the pending event. */
    uint8_t  latency_applied;
} btble_controller_lp_fw_con_info_t;

/* What the LPFW did with a BTBLE_ST_CONN hand-off (activity con_wake_cause). */
#define BTBLE_LPFW_CON_WAKE_NONE   0   /*Not caused by BLE */
#define BTBLE_LPFW_CON_WAKE_MAC_ERR  1  /* mac error   */
#define BTBLE_LPFW_CON_WAKE_RX     2  /* a PDU is left in the RX descriptor  */
#define BTBLE_LPFW_CON_WAKE_MIC_ERR     3 /* last event: MIC error*/
#define BTBLE_LPFW_CON_WAKE_LINK_TIMEOUT 4
#define BTBLE_LPFW_CON_WAKE_RX_DESC_NOT_VALID 5 /*rx descriptor is not valid*/
#define BTBLE_LPFW_CON_WAKE_END_ISR_MISS   6 /*No end isr comes*/
#define BTBLE_LPFW_CON_WAKE_LATENCY_SYNC_ERR 7
#define BTBLE_LPFW_CCON_WAKE_OTHER_RX_ERR 8

#define BTBLE_LPFW_CON_STATE_WAIT_NEXT_PROG 1  /* waiting for the next event to be programmed. */
#define BTBLE_LPFW_CON_STATE_WAIT_END_ISR   2  /*event program time has been set and wait for end isr */
#define BTBLE_LPFW_CON_STATE_WAIT_CHECK_RX  3  /*end isr comes, and wait for rx*/
#define BTBLE_LPFW_CON_STATE_WAIT_CAL_NEXT_PROG_TIME 4  /*wait to calculated next prog time*/


#define BTBLE_ST_NONE        0xFF
#define BTBLE_ST_ADV         0
#define BTBLE_ST_RX_CONN_IND 1
#define BTBLE_ST_CONN        2

typedef struct {
    /*
     * State recorded before handing control to LPFW, one of BTBLE_ST_*.
     * BTBLE_ST_RX_CONN_IND is excluded; LPFW returns that state in
     * btble_controller_lp_fw_activity_t.state.
     */
    uint8_t state;
    /*if wakeup_app=1,not work in lpfw*/
    uint8_t wakeup_app;
    /* True when the next BLE wake-up is for a scheduled radio event; false for other internal controller timer. */
    bool is_arbTarget;
    /* Current RX descriptor byte offset in BLE EM. */
    uint16_t rx_desc_off;
    /* RX data buffer byte offset in BLE EM. */
    uint16_t rx_data_off;
    /* RX data buffer length in bytes. */
    uint16_t rx_data_len;
    /* Saved BLE core register snapshot address. */
    const uint32_t *saved_blecore;
    /* Saved IP core register snapshot address. */
    const uint32_t *saved_ipcore;
    /* Number of valid uint32_t entries in saved_blecore. */
    uint16_t saved_blecore_count;
    /* Number of valid uint32_t entries in saved_ipcore. */
    uint16_t saved_ipcore_count;
    uint16_t rc_calibration;
    /* Legacy advertising information. */
    btble_controller_lp_fw_adv_info_t adv;
    /* Connection information, valid when state == BTBLE_ST_CONN. */
    btble_controller_lp_fw_con_info_t con;
} btble_controller_lp_fw_info_t;

typedef struct {
    /* Final LPFW state, one of BTBLE_ST_*. */
    uint8_t state;
    /*
     * LPFW activity program time in half-slots. Current meanings:
     * - state == BTBLE_ST_ADV: next ADV program time.
     * - state == BTBLE_ST_RX_CONN_IND: program time of the ADV event that
     *   received the CONNECT_IND, not the following advertising event time.
    */
    uint32_t next_hs;
    /* Fine time corresponding to next_hs, with the same activity-specific meaning. */
    uint16_t next_hus;
    /*
     * BLE EM byte address of the RX descriptor that received the
     * Connection Indication (CONNECT_IND).
     * Valid when state == BTBLE_ST_RX_CONN_IND.
     */
    uint32_t conn_ind_rx_desc_addr;
    /*
     * BLE deep-sleep duration programmed by LPFW, in low-power clock cycles.
     * Valid when lpfw_ble_awake is false.
     */
    uint32_t sleep_duration;
    /*
     * True when BLE was awake as LPFW returned control to APP.
     * When false, BLE remains in deep sleep and sleep_duration is valid.
     */
    bool lpfw_ble_awake;
    /*
     * BTBLE_ST_CONN hand-back.  con_wake_cause is BTBLE_LPFW_CON_WAKE_*;
     * the remaining fields are valid when it is not _NONE.
     */
    uint8_t con_wake_cause;
    uint8_t con_state;
    uint8_t con_proged_in_lpfw;
    /* Event counter of the last event the LPFW programmed. */
    uint16_t con_evt_cnt;
     /* Amount by which the connection event counter should be incremented. */
    uint16_t con_evt_inc;
    /* Anchor of that event: the sync time if it synced, else its target. */
    uint32_t con_next_ts;
    uint16_t con_next_bit_off;
    /* next event: exchange-table program time (window start) */
    uint32_t con_prog_hs;
    /* next event: exchange-table fine time */
    uint16_t con_prog_hus;
    /* Last anchor with a sync, half-slots / half-us. */
    uint32_t con_last_sync_ts;
    int16_t  con_last_sync_bit_off;
    /* Last anchor with a CRC-correct packet. */
    uint32_t con_last_crc_ok_ts;
    /* CSA#1: channel index of the last event. */
    uint16_t con_last_cs_ch_idx;
    /* BLE EM byte address of the LPFW's current RX descriptor. */
    uint32_t con_rx_desc_addr;
} btble_controller_lp_fw_activity_t;

/**
 * @brief Get the BLE controller information for LPFW handoff.
 *
 * Called by the APP low-power preparation path after the controller has
 * recorded an active BLE activity. The returned information is copied to
 * LPFW before control is handed over.
 *
 * @return Pointer to the recorded LPFW handoff information.
 */
btble_controller_lp_fw_info_t *btble_controller_get_lp_fw_info(void);

/**
 * @brief Restore a BLE activity after LPFW returns control to the APP.
 *
 * @param[in] restore LPFW activity state and timing to restore.
 *
 * @return 0 on success, otherwise an error code.
 */
int btble_controller_lp_fw_activity_restore(const btble_controller_lp_fw_activity_t *restore);

/**
 * @brief BLE event priority configuration limits
 */
#define BLE_EVENT_PRIORITY_MIN        0    /**< Minimum priority value */
#define BLE_EVENT_PRIORITY_MAX        15   /**< Maximum priority value */
#define BLE_EVENT_PRIORITY_INVALID    0xFFFFFFFF /**< Invalid priority return value */
/**
 * @brief BLE event types for priority configuration
 */
#define BLE_EVENT_CONNECT_IND_TX_RX   0   /**< Connection indication transmission/reception on primary advertising channels */
#define BLE_EVENT_LLCP_MESSAGE        1   /**< LLCP BLE messages */
#define BLE_EVENT_DATA_CHANNEL_TX     2   /**< Data channel transmission BLE messages */
#define BLE_EVENT_INITIATING_SCANNING 3   /**< Initiating/Extended initiating (scanning) on primary advertising channels */
#define BLE_EVENT_ACTIVE_SCANNING     4   /**< Active scanning/Extended active scanning on primary advertising channels */
#define BLE_EVENT_CONNECTABLE_ADV     5   /**< Connectable advertising/Extended advertising on primary advertising channels */
#define BLE_EVENT_NON_CONNECTABLE_ADV 6   /**< Non-connectable advertising/Extended advertising on primary advertising channels */
#define BLE_EVENT_PASSIVE_SCANNING    7   /**< Passive scanning/Extended passive scanning on primary advertising channels */
/**
 * @brief Get the priority of a specific BLE event
 * 
 * @param event The BLE event type to get priority for
 * @return uint32_t Event priority value (0-15) or 0xFFFFFFFF if event is invalid
 * 
 * @note This function returns the current real-time priority value for the specified event type.
 *       The returned value may differ from the previously set priority due to the controller's 
 *       internal scheduling mechanism. Higher values indicate higher priority.
*/
uint32_t btble_controller_get_event_priority(uint8_t event);
/**
 * @brief Set the priority for a specific BLE event
 * 
 * @param event The BLE event type to set priority for
 * @param priority Priority value to set (0-15, where 15 is highest priority)
 * @return int 0 on success, -1 if event or priority is invalid
 * 
 * @note This function configures the priority for the specified event type in the controller.
 *       The set priority value may be modified by the controller's internal scheduling mechanism
 *       to optimize overall system performance.
 */
int btble_controller_set_event_priority(uint8_t event, uint8_t priority);

/**
 * @brief BT coex event types for priority configuration
 */
#define BT_COEX_EVENT_SCO   0   /**< SCO/eSCO traffic (incl. retransmission) */
#define BT_COEX_EVENT_ACL   1   /**< ACL data traffic (incl. retransmission) */
/**
 * @brief Get the BT coex priority of SCO or ACL traffic
 *
 * @param event BT_COEX_EVENT_SCO or BT_COEX_EVENT_ACL
 * @return uint32_t Priority value (0-15) or 0xFFFFFFFF if event is invalid
 */
uint32_t btble_controller_get_bt_coex_priority(uint8_t event);
/**
 * @brief Set the BT coex priority of SCO or ACL traffic
 *
 * @param event BT_COEX_EVENT_SCO or BT_COEX_EVENT_ACL
 * @param priority Priority value to set (0-15, where 15 is highest priority)
 * @return int 0 on success, -1 if event or priority is invalid
 */
int btble_controller_set_bt_coex_priority(uint8_t event, uint8_t priority);


/* key: 32 bytes ecdh private key. This key shall be malloced and passed to btblecontroller_set_private_key api,
 * and when encrypt is done shall call btblecontroller_del_private_key to delete key and then free malloced key.*/
void btblecontroller_set_private_key(uint8_t* key);
uint8_t* btblecontroller_del_private_key(void);

char *btble_controller_get_lib_ver(void);

void btble_controller_remaining_mem(uint8_t** addr, int* size);

void btble_controller_set_cs2(uint8_t enable);    // cs2 is enabled by default

void btble_controller_set_local_sdk_ver(uint32_t sdk_ver);

#if defined(BL702L)
void btble_controller_sleep_init(void);
typedef int (*btble_before_sleep_cb_t)(void);
typedef void (*btble_after_sleep_cb_t)(void);
typedef void (*btble_sleep_aborted_cb_t)(void);
int8_t btble_controller_get_tx_pwr(void);
void btble_set_before_sleep_callback(btble_before_sleep_cb_t cb);
void btble_set_after_sleep_callback(btble_after_sleep_cb_t cb);
/*
  If ble sleep preparation is aborted before sleep, this callback will be trigerred. Please be noticed, 
  this callback is triggerd after before_sleep_callback.
  e.g. Application disables something before sleep, application needs to enable these when sleep is aborted.
*/
void btble_set_sleep_aborted_callback(btble_sleep_aborted_cb_t cb);
#endif

//sco/esco callback to codec
typedef void (*bt_sco_codec_cb_t) (uint16_t   interval_halfslot,
                                uint32_t   tx_buffer_0,
                                uint32_t   tx_buffer_1,
                                uint32_t   rx_buffer_0,
                                uint32_t   rx_buffer_1,
                                uint32_t   tx_buffer_size,
                                uint32_t   rx_buffer_size,
                                uint32_t   start_time_halfslot,
                                uint8_t    buffer_index);
void btble_controller_sco_codec_callback_register(bt_sco_codec_cb_t cb);

/**
 * @brief Set the BLE public device address. Called after btble_controller_init.
 *
 * @param[in] pub_addr Public device address, exactly 6 bytes.
 *
 * @return 0 on success, -1 if pub_addr is NULL or all zeros.
 */
int btble_controller_set_mac_addr(const uint8_t pub_addr[6]);

#endif
