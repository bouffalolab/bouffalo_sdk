#ifndef BLE_LIB_API_H_
#define BLE_LIB_API_H_

#include <stdbool.h>
#include <stdint.h> 


#if defined(CONFIG_BLE_MFG)
#define MAX_SWITCHING_PATTERN_LEN  (0x4B)
struct hci_le_rx_test_v2_cmd
{
    uint8_t     rx_channel;
    uint8_t     phy;
    uint8_t     mod_idx;
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

void btble_controller_init(uint8_t task_priority);
void btble_controller_deinit(void);
#if defined(CFG_NUTTX)
void bbtlecontroller_main(void);
#endif

extern int32_t btble_controller_sleep(int32_t max_sleep_cycles);
extern void btble_controller_sleep_restore();
#if defined(CFG_BT_RESET)
void ble_controller_reset(void);
#endif

// return sleep duration, in unit of 1/32768s
// if 0, means not allow sleep
// if -1, means allow sleep, but there is no end of sleep interrupt (ble core deep sleep is not enabled)
bool ble_controller_sleep_is_ongoing(void);

char *btble_controller_get_lib_ver(void);

void btble_controller_remaining_mem(uint8_t** addr, int* size);
#if defined(BL702L)
void ble_controller_set_tx_pwr(int ble_tx_power);
#endif

#if defined (CONFIG_BLE_MFG) || defined (CONFIG_BT_MFG) 
int bt_mfg_cli_register(void);
int reset_cmd_handler(void);
#if defined (CONFIG_BLE_MFG)
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
#endif
