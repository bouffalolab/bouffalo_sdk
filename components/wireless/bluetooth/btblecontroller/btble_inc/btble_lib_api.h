#ifndef BLE_LIB_API_H_
#define BLE_LIB_API_H_

#include <stdbool.h>
#include <stdint.h> 


#if defined(CONFIG_BLE_MFG)
#define MAX_SWITCHING_PATTERN_LEN  (0x4B)

/// HCI command structure for the LE receiver test v3 command
/*@TRACE*/
struct hci_le_rx_test_v3_cmd
{
    /// RX channel, range: 0x00 to 0x27
    uint8_t     rx_channel;
    /// PHY (@enum le_phy_value)
    uint8_t     phy;
    /// Modulation index (0: standard | 1: stable)
    uint8_t     mod_idx;
    /// Expected CTE length in 8us units, range: 0x02 to 0x14
    uint8_t     exp_cte_len;
    /// Expected CTE type (0: AOA | 1: AOD-1us | 2: AOD-2us)
    uint8_t     exp_cte_type;
    /// Slot durations (1: 1 us | 2: 2 us)
    uint8_t     slot_dur;
    /// Length of switching pattern (number of antenna IDs in the pattern), range: 0x02 to 0x4B
    uint8_t     switching_pattern_len;
    /// Antenna IDs
    uint8_t     antenna_id[MAX_SWITCHING_PATTERN_LEN];
};

struct hci_le_rx_test_v2_cmd
{
    uint8_t     rx_channel;
    uint8_t     phy;
    uint8_t     mod_idx;
};

struct hci_le_rx_test_v1_cmd
{
    /// RX channel, range: 0x00 to 0x27
    uint8_t     rx_channel;
};

struct hci_le_tx_test_v4_cmd
{
    uint8_t     tx_channel;
    uint8_t     test_data_len;
    uint8_t     pkt_payl;
    uint8_t     phy;
    uint8_t     cte_len;
    uint8_t     cte_type;
    uint8_t     switching_pattern_len;
    uint8_t     antenna_id[MAX_SWITCHING_PATTERN_LEN];
    int8_t     tx_pwr_lvl;
};

/// HCI command structure for the LE transmitter test v3 command
/*@TRACE*/
struct hci_le_tx_test_v3_cmd
{
    /// TX channel, range: 0x00 to 0x27
    uint8_t     tx_channel;
    /// Length of test data in bytes, range: 0x00 to 0xFF
    uint8_t     test_data_len;
    /**
     * Packet payload
     * 0x00 PRBS9 sequence "11111111100000111101" (in transmission order) as described in [Vol 6] Part F, Section 4.1.5
     * 0x01 Repeated "11110000" (in transmission order) sequence as described in [Vol 6] Part F, Section 4.1.5
     * 0x02 Repeated "10101010" (in transmission order) sequence as described in [Vol 6] Part F, Section 4.1.5
     * 0x03 PRBS15 sequence as described in [Vol 6] Part F, Section 4.1.5
     * 0x04 Repeated "11111111" (in transmission order) sequence
     * 0x05 Repeated "00000000" (in transmission order) sequence
     * 0x06 Repeated "00001111" (in transmission order) sequence
     * 0x07 Repeated "01010101" (in transmission order) sequence
     * 0x08-0xFF Reserved for future use
     */
    uint8_t     pkt_payl;
    /// PHY (@enum le_phy_value)
    uint8_t     phy;
    /// CTE length (in 8us unit)
    uint8_t     cte_len;
    /// CTE type (0: AOA | 1: AOD-1us | 2: AOD-2us)
    uint8_t     cte_type;
    /// Length of switching pattern (number of antenna IDs in the pattern)
    uint8_t     switching_pattern_len;
    /// Antenna IDs
    uint8_t     antenna_id[MAX_SWITCHING_PATTERN_LEN];
};

struct hci_le_tx_test_v2_cmd
{
    /// TX channel, range: 0x00 to 0x27
    uint8_t     tx_channel;
    /// Length of test data in bytes, range: 0x00 to 0xFF
    uint8_t     test_data_len;
    /**
     * Packet payload
     * 0x00 PRBS9 sequence "11111111100000111101" (in transmission order) as described in [Vol 6] Part F, Section 4.1.5
     * 0x01 Repeated "11110000" (in transmission order) sequence as described in [Vol 6] Part F, Section 4.1.5
     * 0x02 Repeated "10101010" (in transmission order) sequence as described in [Vol 6] Part F, Section 4.1.5
     * 0x03 PRBS15 sequence as described in [Vol 6] Part F, Section 4.1.5
     * 0x04 Repeated "11111111" (in transmission order) sequence
     * 0x05 Repeated "00000000" (in transmission order) sequence
     * 0x06 Repeated "00001111" (in transmission order) sequence
     * 0x07 Repeated "01010101" (in transmission order) sequence
     * 0x08-0xFF Reserved for future use
     */
    uint8_t     pkt_payl;
    /// PHY (@enum le_phy_value)
    uint8_t     phy;

};

struct hci_le_tx_test_v1_cmd
{
    /// TX channel, range: 0x00 to 0x27
    uint8_t     tx_channel;
    /// Length of test data in bytes, range: 0x00 to 0xFF
    uint8_t     test_data_len;
    /**
     * Packet payload
     * 0x00 PRBS9 sequence "11111111100000111101" (in transmission order) as described in [Vol 6] Part F, Section 4.1.5
     * 0x01 Repeated "11110000" (in transmission order) sequence as described in [Vol 6] Part F, Section 4.1.5
     * 0x02 Repeated "10101010" (in transmission order) sequence as described in [Vol 6] Part F, Section 4.1.5
     * 0x03 PRBS15 sequence as described in [Vol 6] Part F, Section 4.1.5
     * 0x04 Repeated "11111111" (in transmission order) sequence
     * 0x05 Repeated "00000000" (in transmission order) sequence
     * 0x06 Repeated "00001111" (in transmission order) sequence
     * 0x07 Repeated "01010101" (in transmission order) sequence
     * 0x08-0xFF Reserved for future use
     */
    uint8_t     pkt_payl;
};

#endif

#if defined(CONFIG_BT_MFG)
struct hci_vs_rx_test_cmd
{
    uint8_t     rx_channel;
    uint8_t     pkt_type;
};
struct hci_vs_tx_test_cmd
{
    uint8_t     tx_channel;
    uint16_t     test_data_len;
    uint8_t     pkt_payl;
    uint8_t     pkt_type;
    int8_t     tx_pwr_lvl;
};
#endif

//Set stack size of btblecontroller task before btble_controller_init if upper layer wants to modify the stack size.
//The default stack size of btblecontroller task is 2k in ble only mode and 4k in bt/ble mode.
void btble_controller_set_task_stack_size(uint16_t stack_size);
void btble_controller_init(uint8_t task_priority);
#if defined(CFG_NUTTX)
void btblecontroller_main( void *pvParameters );
#endif
//API for different RTOS porting to handle btbelcontroller task's messages.
void btblecontroller_proc(void *data);

void btble_controller_deinit(void);
int32_t btble_controller_sleep(int32_t max_sleep_cycles);
void btble_controller_sleep_restore();
#if defined(CFG_BT_RESET)
void btble_controller_reset(void);
#endif

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
#if !defined(CONFIG_BLE_MFG)
/*
  If ble sleep preparation is aborted before sleep, this callback will be trigerred. Please be noticed, 
  this callback is triggerd after before_sleep_callback.
  e.g. Application disables something before sleep, application needs to enable these when sleep is aborted.
*/
void btble_set_sleep_aborted_callback(btble_sleep_aborted_cb_t cb);
#endif
#endif

#if defined (CONFIG_BLE_MFG) || defined (CONFIG_BT_MFG) 
int bt_mfg_cli_register(void);
int reset_cmd_handler(void);
int rd_bd_addr_cmd_handler(void);

#if defined (CONFIG_BLE_MFG)
int hci_le_tx_test_v1_cmd_handler(struct hci_le_tx_test_v1_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_rx_test_v1_cmd_handler(struct hci_le_rx_test_v1_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_tx_test_v2_cmd_handler(struct hci_le_tx_test_v2_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_tx_test_v4_cmd_handler(struct hci_le_tx_test_v4_cmd const *param, uint16_t opcode,bool from_hci);
int hci_le_rx_test_v2_cmd_handler(struct hci_le_rx_test_v2_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_test_end_cmd_handler(void const *param, uint16_t opcode, bool from_hci);
bool ble_check_test_ongoing(void);
#endif
#if defined (CONFIG_BT_MFG)
int hci_vs_rx_test_cmd_handler(struct hci_vs_rx_test_cmd const *param, uint16_t opcode, bool from_hci);
int hci_vs_tx_test_cmd_handler(struct hci_vs_tx_test_cmd const *param, uint16_t opcode, bool from_hci);
int hci_vs_test_end_cmd_handler(void const *param, uint16_t opcode, bool from_hci);
#endif
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
