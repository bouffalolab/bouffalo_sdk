#ifndef BLE_LIB_API_H_
#define BLE_LIB_API_H_

#include <stdbool.h>
#include <stdint.h> 
#include "btble_test_cmd.h"


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

typedef enum{
    BTBLE_IN_ACTIVE_STATE,
    BTBLE_IN_SLEEP_STATE,
    BTBLE_IN_WAKEUP_ONGOING_STATE
}btble_controller_state;

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

//The return value is an instantaneous state that may change rapidly.
btble_controller_state btble_controller_get_state(void);

#if defined(CONFIG_BT_RESET)
void btble_controller_reset(void);
#endif

/**
 * @brief BLE event types for priority configuration
 */
typedef enum {
  BLE_EVENT_CONNECT_IND_TX_RX       = 0,  /**< Connection indication transmission/reception on primary advertising channels */
  BLE_EVENT_LLCP_MESSAGE            = 1,  /**< LLCP BLE messages */
  BLE_EVENT_DATA_CHANNEL_TX         = 2,  /**< Data channel transmission BLE messages */
  BLE_EVENT_INITIATING_SCANNING     = 3,  /**< Initiating/Extended initiating (scanning) on primary advertising channels */
  BLE_EVENT_ACTIVE_SCANNING         = 4,  /**< Active scanning/Extended active scanning on primary advertising channels */
  BLE_EVENT_CONNECTABLE_ADV         = 5,  /**< Connectable advertising/Extended advertising on primary advertising channels */
  BLE_EVENT_NON_CONNECTABLE_ADV     = 6,  /**< Non-connectable advertising/Extended advertising on primary advertising channels */
  BLE_EVENT_PASSIVE_SCANNING        = 7   /**< Passive scanning/Extended passive scanning on primary advertising channels */
} ble_event_mask_t;

/**
 * @brief BLE event priority configuration limits
 */
#define BLE_EVENT_PRIORITY_MIN        0    /**< Minimum priority value */
#define BLE_EVENT_PRIORITY_MAX        15   /**< Maximum priority value */
#define BLE_EVENT_PRIORITY_INVALID    0xFFFFFFFF /**< Invalid priority return value */

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
uint32_t btble_controller_get_event_priority(ble_event_mask_t event);

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
int btble_controller_set_event_priority(ble_event_mask_t event, uint8_t priority);

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

#endif
