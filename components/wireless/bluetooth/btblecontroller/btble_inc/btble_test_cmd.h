#ifndef BTBLE_TEST_CMD_H_
#define BTBLE_TEST_CMD_H_

#include <stdbool.h>
#include <stdint.h> 

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

struct hci_vs_test_end_cmd_cmp_evt
{
    /// Status of the command reception
    uint8_t             status;
    ///Number of RX/TX packets
    uint16_t            nb_packets;
};

#if defined (CONFIG_BLE_MFG) 
int hci_le_tx_test_v1_cmd_handler(struct hci_le_tx_test_v1_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_rx_test_v1_cmd_handler(struct hci_le_rx_test_v1_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_rx_test_v2_cmd_handler(struct hci_le_rx_test_v2_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_tx_test_v2_cmd_handler(struct hci_le_tx_test_v2_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_rx_test_v3_cmd_handler(struct hci_le_rx_test_v3_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_tx_test_v3_cmd_handler(struct hci_le_tx_test_v3_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_tx_test_v4_cmd_handler(struct hci_le_tx_test_v4_cmd const *param, uint16_t opcode, bool from_hci);
int hci_le_test_end_cmd_handler(void const *param, uint16_t opcode, bool from_hci);
#else
int hci_le_tx_test_v1_cmd_handler(struct hci_le_tx_test_v1_cmd const *param, uint16_t opcode);
int hci_le_rx_test_v1_cmd_handler(struct hci_le_rx_test_v1_cmd const *param, uint16_t opcode);
int hci_le_rx_test_v2_cmd_handler(struct hci_le_rx_test_v2_cmd const *param, uint16_t opcode);
int hci_le_tx_test_v2_cmd_handler(struct hci_le_tx_test_v2_cmd const *param, uint16_t opcode);
int hci_le_rx_test_v3_cmd_handler(struct hci_le_rx_test_v3_cmd const *param, uint16_t opcode);
int hci_le_tx_test_v3_cmd_handler(struct hci_le_tx_test_v3_cmd const *param, uint16_t opcode);
int hci_le_tx_test_v4_cmd_handler(struct hci_le_tx_test_v4_cmd const *param, uint16_t opcode);
int hci_le_test_end_cmd_handler(void const *param, uint16_t opcode);
#endif

#if defined(CONFIG_BLE_MFG)
bool ble_check_test_ongoing(void);
#endif

#if defined(CONFIG_BT_MFG)
int hci_vs_rx_test_cmd_handler(struct hci_vs_rx_test_cmd const *param, uint16_t opcode, bool from_hci);
int hci_vs_tx_test_cmd_handler(struct hci_vs_tx_test_cmd const *param, uint16_t opcode, bool from_hci);
int hci_vs_test_end_cmd_handler(void const *param, uint16_t opcode, bool from_hci);
#else
int hci_vs_rx_test_cmd_handler(struct hci_vs_rx_test_cmd const *param, uint16_t opcode);
int hci_vs_tx_test_cmd_handler(struct hci_vs_tx_test_cmd const *param, uint16_t opcode);
int hci_vs_test_end_cmd_handler(void const *param, uint16_t opcode);
#endif

#if defined (CONFIG_BLE_MFG) || defined (CONFIG_BT_MFG) 
int bt_mfg_cli_register(void);
int reset_cmd_handler(void);
int rd_bd_addr_cmd_handler(void);
#endif

#endif