#include <stdlib.h>
#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#include "FreeRTOS.h"
#include "task.h"
#if defined(CONFIG_SHELL)
#include "shell.h"
#else
#include "cli.h"
#endif /* CONFIG_SHELL */
#include "bl_port.h"
#include "ble_cli_cmds.h"
#if !defined (CONFIG_BT_HOST_HCI_TL)
#if defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#else
#include "btble_lib_api.h"
#endif
#endif
#include "l2cap_internal.h"
#if defined(CONFIG_BLE_MULTI_ADV)
#include "multi_adv.h"
#endif

#if defined(CONFIG_HOGP_SERVER)
#include "hog.h"
#endif

#include "bluetooth.h"
#include "hci_driver.h"

#define BLE_CLI_QUEUE_LEN   10
#define CLI_PRIO (CONFIG_BT_RX_PRIO - 2)
static struct k_thread ble_cli_task_h;
static struct k_fifo ble_cli_queue;
typedef enum{
    CLI_MSG_ENABLE_ADV,
    CLI_MSG_DISCONNECT,
    CLI_MSG_MULTI_ADV_STOP,
    CLI_MSG_GATT_CANCEL_PREPARE_WRITES,
}ble_cli_msg_type_t;

typedef struct{
    ble_cli_msg_type_t msg_type;
    struct bt_conn *conn;
    uint8_t reason;
}ble_disconnect_params;

typedef struct{
    ble_cli_msg_type_t msg_type;
    struct bt_conn *conn;
    struct bt_gatt_write_params *write_params;
}ble_gatt_cancel_prepare_writes_params;

#define 		PASSKEY_MAX  		0xF423F
#define 		NAME_LEN 			30
#define 		CHAR_SIZE_MAX       512

static u8_t 	selected_id = BT_ID_DEFAULT;
bool 			ble_inited 	= false;

#if defined(CONFIG_BT_CONN)
struct bt_conn *default_conn = NULL;
#endif

#if defined(CONFIG_BLE_MULTI_ADV)
static int ble_adv_id;
#endif

#define vOutputString(...)  printf(__VA_ARGS__)
#if defined(CONFIG_SHELL)
#define BLE_CLI(func) static void blecli_##func(int argc, char **argv)
#else
#define BLE_CLI(func) static void blecli_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#endif

BLE_CLI(enable);
BLE_CLI(set_chan_map);
BLE_CLI(init);
#if defined(BL702) || defined (BL616) || defined(BL702L)
BLE_CLI(set_2M_phy);
BLE_CLI(set_coded_phy);
BLE_CLI(set_default_phy);
#endif
BLE_CLI(get_device_name);
BLE_CLI(set_device_name);
#if defined(CONFIG_BT_OBSERVER)
BLE_CLI(start_scan);
BLE_CLI(stop_scan);
#if defined(BL702) || defined(BL602)
BLE_CLI(scan_filter_size);
#endif /* BL702 || BL602 */
#endif
BLE_CLI(read_local_address);
BLE_CLI(set_local_address);
#if defined(CONFIG_BT_PERIPHERAL)
BLE_CLI(set_adv_channel);
BLE_CLI(start_advertise);
BLE_CLI(stop_advertise);
#endif
#if defined(CONFIG_BLE_TP_SERVER)
BLE_CLI(tp_start);
#endif
#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
BLE_CLI(connect);
#if defined(CONFIG_BT_WHITELIST)
BLE_CLI(auto_connect);
BLE_CLI(whitelist_add);
BLE_CLI(whitelist_rem);
BLE_CLI(whitelist_clear);
#endif /* CONFIG_BT_WHITELIST */
#endif
BLE_CLI(disconnect);
BLE_CLI(select_conn);
BLE_CLI(conn_update);
BLE_CLI(send_l2cap_conn_param_update_req);
#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
BLE_CLI(l2cap_send_test_data);
BLE_CLI(l2cap_disconnect);
#endif
#if(BFLB_BLE_ENABLE_TEST_PSM)
BLE_CLI(register_test_psm);
BLE_CLI(connect_test_psm);
#endif
BLE_CLI(read_rssi);
BLE_CLI(unpair);
#if defined(BL702L) || defined(BL616) || defined(BL606P) || defined(BL808)
BLE_CLI(ble_throughput_calc);
#endif
#endif
#if defined(CONFIG_BT_SMP)
BLE_CLI(security);
BLE_CLI(auth);
BLE_CLI(auth_cancel);
BLE_CLI(auth_passkey_confirm);
BLE_CLI(auth_pairing_confirm);
BLE_CLI(auth_passkey);
#endif
BLE_CLI(exchange_mtu);
BLE_CLI(discover);
BLE_CLI(read);
BLE_CLI(write);
BLE_CLI(write_without_rsp);
BLE_CLI(subscribe);
BLE_CLI(unsubscribe);
BLE_CLI(set_data_len);
BLE_CLI(get_all_conn_info);
BLE_CLI(disable);
#if defined(CONFIG_BLE_MULTI_ADV)
BLE_CLI(start_multi_advertise);
BLE_CLI(stop_multi_advertise);
#endif

BLE_CLI(set_tx_pwr);

#if defined(CONFIG_HOGP_SERVER)
BLE_CLI(hog_srv_notify);
#endif
#if defined(BFLB_BLE_DYNAMIC_SERVICE)
#if defined(CONFIG_BT_PERIPHERAL)
#if defined(CONFIG_BT_SPP_SERVER)
BLE_CLI(add_spp_service);
BLE_CLI(del_spp_service);
#endif
BLE_CLI(gatts_get_service_info);
BLE_CLI(gatts_get_char);
BLE_CLI(gatts_get_desp);
#endif
#endif
BLE_CLI(le_tx_test);
BLE_CLI(le_rx_test);
BLE_CLI(le_enh_tx_test);
BLE_CLI(le_enh_rx_test);
BLE_CLI(le_test_end);

#if defined(CONFIG_SHELL)
    SHELL_CMD_EXPORT_ALIAS(blecli_enable, ble_enable, ble enable Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blecli_set_chan_map, ble_set_chan_map, ble set channel map Parameter:[channels]);
    SHELL_CMD_EXPORT_ALIAS(blecli_init, ble_init, ble Initialize Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blecli_get_device_name, ble_get_device_name, ble get device name Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blecli_set_device_name, ble_set_device_name, ble set device name Parameter:[Lenth of name] [name]);
#if defined(CONFIG_BLE_TP_SERVER)
    SHELL_CMD_EXPORT_ALIAS(blecli_tp_start, ble_tp_start, throughput start Parameter:[TP test 1:enable; 0:disable]);
#endif /* CONFIG_BLE_TP_SERVER */
#if defined(BL702) || defined (BL616) || defined(BL702L)
#if defined(CONFIG_BT_CONN)
    SHELL_CMD_EXPORT_ALIAS(blecli_set_default_phy, ble_set_default_phy, ble set default phy Parameter:[defualt phys]);
    SHELL_CMD_EXPORT_ALIAS(blecli_set_2M_phy, ble_set_2M_Phy, ble set 2M Phy Parameter:[defualt phys]);
    SHELL_CMD_EXPORT_ALIAS(blecli_set_coded_phy, ble_set_coded_phy, ble set coded phy Parameter:[all phys] [coded option]);
#endif /* CONFIG_BT_CONN */
#endif /* BL702 */
#if defined(CONFIG_BT_OBSERVER)
#if defined(BL702) || defined(BL602)
    SHELL_CMD_EXPORT_ALIAS(blecli_scan_filter_size, ble_scan_filter_size, ble scan filter sizer Parameter:[filter table size]);
#endif /* BL702 || BL602 */
#endif /* CONFIG_BT_OBSERVER */
#if defined(BFLB_DISABLE_BT)
    SHELL_CMD_EXPORT_ALIAS(blecli_disable, ble_disable, ble disable Parameter:[Null]);
#endif /* BFLB_DISABLE_BT */
#if defined(CONFIG_BT_OBSERVER)
    SHELL_CMD_EXPORT_ALIAS(blecli_start_scan, ble_start_scan, ble start scan \
        Parameter:[Scan type: 0:passive scan; 1:active scan] \
        [filtering: 0:Disable duplicate; 1:Enable duplicate] \
        [Scan interval: 0x0004-4000;e.g.0080] \
        [Scan window: 0x0004-4000;e.g.0050]);
    SHELL_CMD_EXPORT_ALIAS(blecli_stop_scan, ble_stop_scan, ble stop scan Parameter:[Null]);
#endif /* CONFIG_BT_OBSERVER */
#if defined(CONFIG_BT_PERIPHERAL)
    SHELL_CMD_EXPORT_ALIAS(blecli_set_adv_channel, ble_set_adv_channel, ble set adv channel Parameter:[adv channel]);
    SHELL_CMD_EXPORT_ALIAS(blecli_start_advertise, ble_start_adv, ble start adv \
        Parameter:[Adv type: 0:adv_ind; 1:adv_scan_ind; 2:adv_nonconn_ind] \
        [Mode: 0:discov; 1:non-discov] \
        [Adv Interval Min: 0x0020-4000; e.g.0030] \
        [Adv Interval Max: 0x0020-4000; e.g.0060]);
    SHELL_CMD_EXPORT_ALIAS(blecli_stop_advertise, ble_stop_adv, ble stop adv Parameter:[Null]);
#if defined(CONFIG_BLE_MULTI_ADV)
    SHELL_CMD_EXPORT_ALIAS(blecli_start_multi_advertise, ble_start_multi_adv, ble start multi adv Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blecli_stop_multi_advertise, ble_stop_multi_adv, ble stop multi adv Parameter:[instant id]);
#endif /* CONFIG_BLE_MULTI_ADV */
    SHELL_CMD_EXPORT_ALIAS(blecli_read_local_address, ble_read_local_address, ble read local address Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blecli_set_local_address, ble_set_local_address, ble set local address Parameter:[bt address]);
#endif /* CONFIG_BT_PERIPHERAL */
#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
    SHELL_CMD_EXPORT_ALIAS(blecli_connect, ble_connect, ble Connect remote device \
        Parameter:[Address type: 0:ADDR_PUBLIC; 1:ADDR_RAND; 2:ADDR_RPA_OR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
        [Address value; e.g.112233AABBCC]);
    #if defined(CONFIG_BT_WHITELIST)
    SHELL_CMD_EXPORT_ALIAS(blecli_auto_connect, ble_auto_connect, ble Connect remote device \
        Parameter:[Address type: 0:ADDR_PUBLIC; 1:ADDR_RAND; 2:ADDR_RPA_OR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
        [Address value: e.g.112233AABBCC]);
    SHELL_CMD_EXPORT_ALIAS(blecli_whitelist_add, ble_whitelist_add, Add white list \
        Parameter [Address type: 0:ADDR_PUBLIC; 1:ADDR_RAND; 2:ADDR_RPA_OR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
        [Address value: e.g.112233AABBCC]);
    SHELL_CMD_EXPORT_ALIAS(blecli_whitelist_rem, ble_whitelist_rem, Remove white list \
        Parameter [Address type: 0:ADDR_PUBLIC; 1:ADDR_RAND; 2:ADDR_RPA_OR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
        [Address value: e.g.112233AABBCC]);
    SHELL_CMD_EXPORT_ALIAS(blecli_whitelist_clear, ble_whitelist_clear, Clear white list Parameter []);
#endif /* CONFIG_BT_WHITELIST */
#endif /* CONFIG_BT_CENTRAL */
    SHELL_CMD_EXPORT_ALIAS(blecli_disconnect, ble_disconnect, Disconnect remote device \
        Parameter:[Address type: 0:ADDR_PUBLIC; 1:ADDR_RAND; 2:ADDR_RPA_OR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
        [Address value: e.g.112233AABBCC]);
    SHELL_CMD_EXPORT_ALIAS(blecli_select_conn, ble_select_conn, Select a specific connection \
        Parameter:[Address type: 0:ADDR_PUBLIC; 1:ADDR_RAND; 2:ADDR_RPA_OR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
        [Address value: e.g.112233AABBCC]);
    SHELL_CMD_EXPORT_ALIAS(blecli_unpair, ble_unpair, bleUnpair connection] \
        Parameter:[Address type: 0:ADDR_PUBLIC; 1:ADDR_RAND; 2:ADDR_RPA_OR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
        [Address value: all 0: unpair all connection; otherwise:unpair specific connection]);
    SHELL_CMD_EXPORT_ALIAS(blecli_conn_update, ble_conn_update, ble conn update \
        Parameter:[Conn Interval Min: 0x0006-0C80; e.g.0030] \
        [Conn Interval Max: 0x0006-0C80; e.g.0030] \
        [Conn Latency: 0x0000-01f3; e.g.0004] \
        [Supervision Timeout: 0x000A-0C80; e.g.0010]);
    SHELL_CMD_EXPORT_ALIAS(blecli_send_l2cap_conn_param_update_req, ble_send_l2cap_conn_param_update_req, ble l2cap conn parameters update \
        Parameter:[Conn Interval Min: 0x0006-0C80; e.g.0030] \
        [Conn Interval Max: 0x0006-0C80; e.g.0030] \
        [Conn Latency: 0x0000-01f3; e.g.0004] \
        [Supervision Timeout: 0x000A-0C80; e.g.0010]);
#if defined(BL702L) || defined(BL616) || defined(BL606P) || defined(BL808)
    SHELL_CMD_EXPORT_ALIAS(blecli_ble_throughput_calc, ble_set_throughput_calc, ble set throughputcalc \
        Parameter:[enable];[duration]);
#endif
    #if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
    SHELL_CMD_EXPORT_ALIAS(blecli_l2cap_send_test_data, ble_l2cap_send_test_data, ble send l2cap data Parameter:[channel id]);
    SHELL_CMD_EXPORT_ALIAS(blecli_l2cap_disconnect, ble_l2cap_disconnect, ble send l2cap data Parameter:[channel id]);
    #endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */
    #if(BFLB_BLE_ENABLE_TEST_PSM)
    SHELL_CMD_EXPORT_ALIAS(blecli_register_test_psm, ble_register_test_psm, register ble test psm Parameter:[psm: security level]);
    SHELL_CMD_EXPORT_ALIAS(blecli_connect_test_psm, ble_connect_test_psm, connect ble test psm Parameter:[psm]);
    #endif /* BFLB_BLE_ENABLE_TEST_PSM */
    SHELL_CMD_EXPORT_ALIAS(blecli_read_rssi, ble_read_rssi, ble read rssi Parameter:[Null]);
 #if defined(CONFIG_BT_SMP)
    SHELL_CMD_EXPORT_ALIAS(blecli_security, ble_security, Start security \
        Parameter:[Security level: Default value 4; 2:BT_SECURITY_MEDIUM; 3:BT_SECURITY_HIGH; 4:BT_SECURITY_FIPS]);
    SHELL_CMD_EXPORT_ALIAS(blecli_auth, ble_auth, Register auth callback Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blecli_auth_cancel, ble_auth_cancel, Cancel register auth callback Parameter:[Null]]);
    SHELL_CMD_EXPORT_ALIAS(blecli_auth_passkey_confirm, ble_auth_passkey_confirm, Confirm passkey Parameter:[Null]]);
    SHELL_CMD_EXPORT_ALIAS(blecli_auth_pairing_confirm, ble_auth_pairing_confirm, Confirm pairing in secure connection Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blecli_auth_passkey, ble_auth_passkey, Input passkey Parameter:[Passkey: 00000000-000F423F]);
#endif /* CONFIG_BT_SMP */
#if defined(CONFIG_BT_GATT_CLIENT)
    SHELL_CMD_EXPORT_ALIAS(blecli_exchange_mtu, ble_exchange_mtu, Exchange mtu Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(blecli_discover, ble_discover, Gatt discovery \
        Parameter:[Discovery type: 0:Primary; 1:Secondary; 2:Include; 3:Characteristic; 4:Descriptor] \
        [Uuid value: 2 Octets; e.g.1800] \
        [Start handle: 2 Octets; e.g.0001] \
        [End handle: 2 Octets; e.g.ffff]);
    SHELL_CMD_EXPORT_ALIAS(blecli_read, ble_read, Gatt Read \
        Parameter:[Attribute handle: 2 Octets]\
        [Value offset: 2 Octets]);
    SHELL_CMD_EXPORT_ALIAS(blecli_write, ble_write, Gatt write\
        Parameter:[Attribute handle: 2 Octets]\
        [Value offset: 2 Octets]\
        [Value length: 2 Octets]\
        [Value data]);
    SHELL_CMD_EXPORT_ALIAS(blecli_write_without_rsp, ble_write_without_rsp, Gatt write without response \
        Parameter:[Sign: 0: No need; signed: 1; Signed write cmd if no smp] \
        [Attribute handle: 2 Octets] \
        [Value length: 2 Octets] \
        [Value data]);
    SHELL_CMD_EXPORT_ALIAS(blecli_subscribe, ble_subscribe, Gatt subscribe \
        Parameter:[CCC handle: 2 Octets] \
        [Value handle: 2 Octets] \
        [Value: 1:notify: 2:indicate]);
    SHELL_CMD_EXPORT_ALIAS(blecli_unsubscribe, ble_unsubscribe, Gatt unsubscribe Parameter:[Null]);
#endif /* CONFIG_BT_GATT_CLIENT */
    SHELL_CMD_EXPORT_ALIAS(blecli_set_data_len, ble_set_data_len, LE Set Data Length \
        Parameter:[tx octets: 2 octets] [tx time: 2 octets]);
    SHELL_CMD_EXPORT_ALIAS(blecli_get_all_conn_info, ble_conn_info, LE get all connection devices info Parameter:[Null]);
#endif /* CONFIG_BT_CONN */
    SHELL_CMD_EXPORT_ALIAS(blecli_set_tx_pwr, ble_set_tx_pwr, Set tx power mode Parameter:[mode: 1 octet; value:5; 6; 7]);
        
#if defined(CONFIG_HOGP_SERVER)
    SHELL_CMD_EXPORT_ALIAS(blecli_hog_srv_notify, ble_hog_srv_notify, HOG srv notify Parameter [hid usage] [press]);
#endif /* CONFIG_HOGP_SERVER */
#if defined(BFLB_BLE_DYNAMIC_SERVICE)
#if defined(CONFIG_BT_PERIPHERAL)
    #if defined(CONFIG_BT_SPP_SERVER)
    SHELL_CMD_EXPORT_ALIAS(blecli_add_spp_service, ble_add_spp_svc, );
    SHELL_CMD_EXPORT_ALIAS(blecli_del_spp_service, ble_del_spp_svc, );
    #endif /* CONFIG_BT_SPP_SERVER */
    SHELL_CMD_EXPORT_ALIAS(blecli_gatts_get_service_info, ble_get_svc_info, );
    SHELL_CMD_EXPORT_ALIAS(blecli_gatts_get_char, ble_get_svc_char, );
    SHELL_CMD_EXPORT_ALIAS(blecli_gatts_get_desp, ble_get_svc_desp, );
#endif /* CONFIG_BT_PERIPHERAL */
#endif /* BFLB_BLE_DYNAMIC_SERVICE */
    SHELL_CMD_EXPORT_ALIAS(blecli_le_tx_test, ble_tx_test, LE tx test \
        parameter:[tx channel:1 datalen:1 octet;packet payload:1);
    SHELL_CMD_EXPORT_ALIAS(blecli_le_rx_test, ble_rx_test, LE tx test \
        parameter:[rx channel:1);
    SHELL_CMD_EXPORT_ALIAS(blecli_le_enh_tx_test, ble_enh_tx_test, LE enh tx test \
        parameter:[tx channel:1 octet;test data length:1 octet;packet payload:1 octet; phy:1 octet);
    SHELL_CMD_EXPORT_ALIAS(blecli_le_enh_rx_test, ble_enh_rx_test, LE enh tx test \
        parameter:[rx channel:1 octet;phy:1 octet;modulation index:1 octet);
    SHELL_CMD_EXPORT_ALIAS(blecli_le_test_end, ble_test_end, );
#else /* CONFIG_SHELL */
const struct cli_command btStackCmdSet[] STATIC_CLI_CMD_ATTRIBUTE = {
#if 1
    /*1.The cmd string to type, 2.Cmd description, 3.The function to run, 4.Number of parameters*/

    {"ble_enable", "ble enable\r\nParameter [Null]\r\n", blecli_enable},
    {"ble_set_chan_map", "ble set channel map\r\nParameter [channels]\r\n", blecli_set_chan_map},
    {"ble_init", "ble Initialize\r\nParameter [Null]\r\n", blecli_init},
    {"ble_get_device_name", "ble get device name\r\nParameter [Null]\r\n", blecli_get_device_name},
    {"ble_set_device_name", "ble set device name\r\nParameter [Lenth of name] [name]\r\n", blecli_set_device_name},
#if defined(CONFIG_BLE_TP_SERVER)
    {"ble_tp_start", "throughput start\r\nParameter [TP test,1:enable, 0:disable]\r\n", blecli_tp_start},
#endif
#if defined(BL702)|| defined (BL616) || defined(BL702L)
#if defined(CONFIG_BT_CONN)
    {"ble_set_default_phy", "ble set default phy\r\nParameter [defualt phys]\r\n", blecli_set_default_phy},
    {"ble_set_2M_Phy", "ble set 2M Phy\r\nParameter [defualt phys]\r\n", blecli_set_2M_phy},
    {"ble_set_coded_phy", "ble set coded phy\r\nParameter [all phys] [coded option]\r\n", blecli_set_coded_phy},
#endif
#endif
#if defined(CONFIG_BT_OBSERVER)
#if defined(BL702) || defined(BL602)
    {"ble_scan_filter_size", "ble scan filter sizer\nParameter [filter table size]\r\n", blecli_scan_filter_size},
#endif
#endif
#if defined(BFLB_DISABLE_BT)
    {"ble_disable", "ble disable\r\nParameter [Null]\r\n", blecli_disable},
#endif
#if defined(CONFIG_BT_OBSERVER)
    {"ble_start_scan", " ble start scan\r\n\
    Parameter [Scan type, 0:passive scan, 1:active scan]\r\n\
    [filtering, 0:Disable duplicate, 1:Enable duplicate]\r\n\
    [Scan interval, 0x0004-4000,e.g.0080]\r\n\
    [Scan window, 0x0004-4000,e.g.0050]\r\n", blecli_start_scan},
    {"ble_stop_scan", "ble stop scan\r\nParameter [Null]\r\n", blecli_stop_scan},
#endif
#if defined(CONFIG_BT_PERIPHERAL)
    {"ble_set_adv_channel", "ble set adv channel\r\nParameter [adv channel]\r\n", blecli_set_adv_channel},
    {"ble_start_adv", "ble start adv\r\n\
    Parameter [Adv type, 0:adv_ind, 1:adv_scan_ind, 2:adv_nonconn_ind]\r\n\
    [Mode, 0:discov, 1:non-discov]\r\n\
    [Adv Interval Min,0x0020-4000,e.g.0030]\r\n\
    [Adv Interval Max,0x0020-4000,e.g.0060]\r\n", blecli_start_advertise},
    {"ble_stop_adv", "ble stop adv\r\nParameter [Null]\r\n", blecli_stop_advertise},
#if defined(CONFIG_BLE_MULTI_ADV)
    {"ble_start_multi_adv", "ble start multi adv\r\nParameter [Null]\r\n", blecli_start_multi_advertise},
    {"ble_stop_multi_adv", "ble stop multi adv\r\nParameter [instant id]\r\n", blecli_stop_multi_advertise},
#endif
    {"ble_read_local_address", "ble read local address\r\nParameter [Null]\r\n", blecli_read_local_address},
    {"ble_set_local_address", "ble set local address\r\nParameter [bt address]\r\n", blecli_set_local_address},
#endif
#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
    {"ble_connect", "ble Connect remote device\r\n\
     Parameter [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
     [Address value, e.g.112233AABBCC]\r\n", blecli_connect},
#if defined(CONFIG_BT_WHITELIST)
    {"ble_auto_connect", "ble Connect remote device\r\n\
    Parameter [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value, e.g.112233AABBCC]\r\n", blecli_auto_connect},
    {"ble_whitelist_add", "Add white list\r\n\
    Parameter [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value, e.g.112233AABBCC]\r\n", blecli_whitelist_add},
    {"ble_whitelist_rem", "Remove white list\r\n\
    Parameter [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value, e.g.112233AABBCC]\r\n", blecli_whitelist_rem},
    {"ble_whitelist_clear", "Clear white list\r\n\
    Parameter []\r\n", blecli_whitelist_clear},
#endif /* CONFIG_BT_WHITELIST */
#endif
    {"ble_disconnect", "Disconnect remote device\r\n\
    Parameter [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value,e.g.112233AABBCC]\r\n", blecli_disconnect},
    {"ble_select_conn", "Select a specific connection\r\n\
    Parameter [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value, e.g.112233AABBCC]\r\n", blecli_select_conn},
    {"ble_unpair", "bleUnpair connection]\r\n\
    Parameter [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value, all 0: unpair all connection, otherwise:unpair specific connection]\r\n", blecli_unpair},
    {"ble_conn_update", "ble conn update\r\n\
    Parameter [Conn Interval Min,0x0006-0C80,e.g.0030]\r\n\
    [Conn Interval Max,0x0006-0C80,e.g.0030]\r\n\
    [Conn Latency,0x0000-01f3,e.g.0004]\r\n\
    [Supervision Timeout,0x000A-0C80,e.g.0010]\r\n", blecli_conn_update},
    {"ble_send_l2cap_conn_param_update_req", "ble l2cap conn parameters update\r\n\
    Parameter [Conn Interval Min,0x0006-0C80,e.g.0030]\r\n\
    [Conn Interval Max,0x0006-0C80,e.g.0030]\r\n\
    [Conn Latency,0x0000-01f3,e.g.0004]\r\n\
    [Supervision Timeout,0x000A-0C80,e.g.0010]\r\n", blecli_send_l2cap_conn_param_update_req},
    #if defined(BL702L) || defined(BL616) || defined(BL606P) || defined(BL808)
    {"ble_set_throughput_calc", "ble set throughputcalc\r\nParameter [enable]\r\n [duration]\r\n", blecli_ble_throughput_calc},
    #endif
    #if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
    {"ble_l2cap_send_test_data", "ble send l2cap data\r\nParameter [channel id]\r\n", blecli_l2cap_send_test_data}, 
    {"ble_l2cap_disconnect", "ble send l2cap data\r\nParameter [channel id]\r\n", blecli_l2cap_disconnect},
    #endif
    #if(BFLB_BLE_ENABLE_TEST_PSM)
    {"ble_register_test_psm", "register ble test psm\r\nParameter [psm,security level]\r\n", blecli_register_test_psm},
    {"ble_connect_test_psm", "connect ble test psm\r\nParameter [psm]\r\n", blecli_connect_test_psm},
    #endif
    {"ble_read_rssi", "ble read rssi\r\nParameter [Null]\r\n", blecli_read_rssi},
 #if defined(CONFIG_BT_SMP)
    {"ble_security", "Start security\r\n\
    Parameter [Security level, Default value 4, 2:BT_SECURITY_MEDIUM, 3:BT_SECURITY_HIGH, 4:BT_SECURITY_FIPS]\r\n", blecli_security},
    {"ble_auth", "Register auth callback\r\nParameter [Null]\r\n", blecli_auth},
    {"ble_auth_cancel", "Cancel register auth callback\r\nParameter [Null]]\r\n", blecli_auth_cancel},
    {"ble_auth_passkey_confirm", "Confirm passkey]\r\nParameter [Null]]\r\n", blecli_auth_passkey_confirm},
    {"ble_auth_pairing_confirm", "Confirm pairing in secure connection\r\nParameter [Null]\r\n", blecli_auth_pairing_confirm},
    {"ble_auth_passkey", "Input passkey\r\nParameter [Passkey, 00000000-000F423F]\r\n", blecli_auth_passkey},
#endif
#if defined(CONFIG_BT_GATT_CLIENT)
    {"ble_exchange_mtu", "Exchange mtu\r\nParameter [Null]\r\n", blecli_exchange_mtu},
    {"ble_discover", "Gatt discovery\r\n\
    Parameter [Discovery type, 0:Primary, 1:Secondary, 2:Include, 3:Characteristic, 4:Descriptor]\r\n\
    [Uuid value, 2 Octets, e.g.1800]\r\n\
    [Start handle, 2 Octets, e.g.0001]\r\n\
    [End handle, 2 Octets, e.g.ffff]\r\n", blecli_discover},
    {"ble_read", "Gatt Read\r\n\
    Parameter [Attribute handle, 2 Octets]\r\n\
    [Value offset, 2 Octets]\r\n", blecli_read},
    {"ble_write", "Gatt write\r\n\
    Parameter [Attribute handle, 2 Octets]\r\n\
    [Value offset, 2 Octets]\r\n\
    [Value length, 2 Octets]\r\n\
    [Value data]\r\n", blecli_write},
    {"ble_write_without_rsp", "Gatt write without response\r\n\
    Parameter [Sign, 0: No need signed, 1:Signed write cmd if no smp]\r\n\
    [Attribute handle, 2 Octets]\r\n\
    [Value length, 2 Octets]\r\n\
    [Value data]\r\n", blecli_write_without_rsp},
    {"ble_subscribe", "Gatt subscribe\r\n\
    Parameter [CCC handle, 2 Octets]\r\n\
    [Value handle, 2 Octets]\r\n\
    [Value, 1:notify, 2:indicate]\r\n", blecli_subscribe},
    {"ble_unsubscribe", "Gatt unsubscribe\r\nParameter [Null]\r\n", blecli_unsubscribe},
#endif
    {"ble_set_data_len","LE Set Data Length\r\n\
    Parameter [tx octets, 2 octets] [tx time, 2 octets]\r\n", blecli_set_data_len},
    {"ble_conn_info", "LE get all connection devices info\r\nParameter [Null]\r\n", blecli_get_all_conn_info},
#endif//CONFIG_BT_CONN

    {"ble_set_tx_pwr","Set tx power mode\r\nParameter [mode, 1 octet, value:5,6,7]\r\n", blecli_set_tx_pwr},
        
#if defined(CONFIG_HOGP_SERVER)
    {"ble_hog_srv_notify", "HOG srv notify\r\nParameter [hid usage] [press]\r\n", blecli_hog_srv_notify},
#endif
#if defined(BFLB_BLE_DYNAMIC_SERVICE)
#if defined(CONFIG_BT_PERIPHERAL)
    #if defined(CONFIG_BT_SPP_SERVER)
    {"ble_add_spp_svc", "", blecli_add_spp_service},
    {"ble_del_spp_svc", "", blecli_del_spp_service},
    #endif
    {"ble_get_svc_info","",blecli_gatts_get_service_info},
    {"ble_get_svc_char","",blecli_gatts_get_char},
    {"ble_get_svc_desp","",blecli_gatts_get_desp},
#endif
#endif
    {"ble_tx_test","LE tx test\r\nparameter [tx channel, 1 octet, test data length, 1 octet, packet payload, 1 octet, phy, 1 octet]",blecli_le_enh_tx_test},
    {"ble_rx_test","LE tx test\r\nparameter [rx channel, 1 octet, phy, 1 octet, modulation index, 1 octet]",blecli_le_enh_rx_test},
    {"ble_test_end","",blecli_le_test_end},
#else
    {"ble_init", "", blecli_init},
#if defined(CONFIG_BLE_TP_SERVER)
    {"ble_tp_start", "", blecli_tp_start},
#endif
#if defined(BL702)
    {"ble_set_2M_Phy", "", blecli_set_2M_phy},
    {"ble_set_coded_phy", "", blecli_set_coded_phy},
    {"ble_set_default_phy", "", blecli_set_default_phy},
#endif
#if defined(BFLB_DISABLE_BT)
    {"ble_disable", "", blecli_disable},
#endif
    {"ble_get_device_name", "", blecli_get_device_name},
    {"ble_set_device_name", "", blecli_set_device_name},
#if defined(CONFIG_BT_OBSERVER)
    {"ble_start_scan", "", blecli_start_scan},
    {"ble_stop_scan", "", blecli_stop_scan},
#if defined(BL702) || defined(BL602)
    {"ble_scan_filter_size", "", blecli_scan_filter_size},
#endif
#endif
#if defined(CONFIG_BT_PERIPHERAL)
    {"ble_set_adv_channel", "", blecli_set_adv_channel},
    {"ble_start_adv", "", blecli_start_advertise},
    {"ble_stop_adv", "", blecli_stop_advertise},
#if defined(CONFIG_BLE_MULTI_ADV)
    {"ble_start_multi_adv", "", blecli_start_multi_advertise},
    {"ble_stop_multi_adv", "", blecli_stop_multi_advertise},
#endif
    {"ble_read_local_address", "", blecli_read_local_address},
#endif
#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
    {"ble_connect", "", blecli_connect},
#endif
    {"ble_disconnect", "", blecli_disconnect},
    {"ble_select_conn", "", blecli_select_conn},
    {"ble_unpair", "", blecli_unpair},
    {"ble_conn_update", "", blecli_conn_update},
    {"ble_read_rssi", "", blecli_read_rssi},
#endif
#if defined(CONFIG_BT_SMP)
    {"ble_security", "", blecli_security},
    {"ble_auth", "", blecli_auth},
    {"ble_auth_cancel", "", blecli_auth_cancel},
    {"ble_auth_passkey_confirm", "", blecli_auth_passkey_confirm},
    {"ble_auth_pairing_confirm", "", blecli_auth_pairing_confirm},
    {"ble_auth_passkey", "", blecli_auth_passkey},
#endif
#if defined(CONFIG_BT_GATT_CLIENT)
    {"ble_exchange_mtu", "", blecli_exchange_mtu},
    {"ble_discover", "", blecli_discover},
    {"ble_read", "", blecli_read},
    {"ble_write", "", blecli_write},
    {"ble_write_without_rsp", "", blecli_write_without_rsp},
    {"ble_subscribe", "", blecli_subscribe},
    {"ble_unsubscribe", "", blecli_unsubscribe},
#endif
    {"ble_set_data_len", "", blecli_set_data_len},
    {"ble_conn_info", "", blecli_get_all_conn_info},
    {"ble_set_tx_pwr", "", blecli_set_tx_pwr},
#if defined(CONFIG_HOGP_SERVER)
    {"ble_hog_srv_notify", "", blecli_hog_srv_notify},
#endif
#endif
};
#endif /* CONFIG_SHELL */

#if defined(CONFIG_BT_CONN)
static void connected(struct bt_conn *conn, u8_t err)
{
    if(err || conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }

    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

#if defined(CONFIG_BLE_MULTI_ADV)
    if(ble_adv_id){
        uint8_t *msg = k_malloc(sizeof(uint8_t));
        if(!msg){
            vOutputString("Fail to allocate buffer.");
            return;
        }
        *msg = CLI_MSG_MULTI_ADV_STOP;
        k_fifo_put(&ble_cli_queue, (void *)msg);
    }
#endif /* CONFIG_BLE_MULTI_ADV */

    if (err) {
        vOutputString("Failed to connect to %s (%u) \r\n", addr, err);
        return;
    }

    vOutputString("Connected: %s \r\n", addr);

    if (!default_conn) {
        default_conn = conn;
    }

#if defined(CONFIG_BLE_RECONNECT_TEST)
    if (conn->role == BT_CONN_ROLE_MASTER) {
        ble_disconnect_params *params = k_malloc(sizeof(ble_disconnect_params));
        if(!params){
            vOutputString("Fail to allocate buffer.");
            return;
        }
        params->msg_type = CLI_MSG_DISCONNECT;
        params->conn = conn;
        params->reason = BT_HCI_ERR_REMOTE_USER_TERM_CONN;
        k_fifo_put(&ble_cli_queue, (void *)params);
    }
#endif
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
    if(conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }

    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    vOutputString("Disconnected: %s (reason %u) \r\n", addr, reason);

#if defined(CONFIG_BLE_RECONNECT_TEST)
    if(conn->role == BT_CONN_ROLE_SLAVE) {
        uint8_t *msg = k_malloc(sizeof(uint8_t));
        if(!msg){
            vOutputString("Fail to allocate buffer.");
            return;
        }
        *msg = CLI_MSG_ENABLE_ADV;
        k_fifo_put(&ble_cli_queue, (void *)msg);
    }
#endif

    if (default_conn == conn) {
        default_conn = NULL;
    }
}

static void le_param_updated(struct bt_conn *conn, u16_t interval,
			     u16_t latency, u16_t timeout)
{
    if(conn == default_conn)
    {
        vOutputString("LE conn param updated: int 0x%04x lat %d to %d \r\n", interval, latency, timeout);
    }
}

static void le_phy_updated(struct bt_conn *conn, u8_t tx_phy, u8_t rx_phy)
{
    if(conn == default_conn)
    {
        vOutputString("LE phy updated: rx_phy %d, rx_phy %d\r\n", tx_phy, rx_phy);
    }
}

#if defined(CONFIG_BT_SMP)
static void identity_resolved(struct bt_conn *conn, const bt_addr_le_t *rpa,
			      const bt_addr_le_t *identity)
{
    if(conn == default_conn)
    {
        char addr_identity[BT_ADDR_LE_STR_LEN];
        char addr_rpa[BT_ADDR_LE_STR_LEN];

        bt_addr_le_to_str(identity, addr_identity, sizeof(addr_identity));
        bt_addr_le_to_str(rpa, addr_rpa, sizeof(addr_rpa));

        vOutputString("Identity resolved %s -> %s \r\n", addr_rpa, addr_identity);
    }
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
    if(conn == default_conn)
    {
        char addr[BT_ADDR_LE_STR_LEN];

        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
        vOutputString("Security changed: %s level %u \r\n", addr, level);
    }
}
#endif

#if (CONFIG_BT_REMOTE_VERSION)
static void remote_version(struct bt_conn *conn, u8_t version,
		u16_t manufacturer, u16_t subversion)
{
    printf("Remote Version: %d, Manufacturer: %d, Subversion: %d \r\n", version, manufacturer, subversion);
}
#endif /* CONFIG_BT_REMOTE_VERSION */

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
	.le_param_updated = le_param_updated,
	.le_phy_updated = le_phy_updated,
#if defined(CONFIG_BT_SMP)
	.identity_resolved = identity_resolved,
	.security_changed = security_changed,
#endif
#if (CONFIG_BT_REMOTE_VERSION)
	.remote_version = remote_version,
#endif /* CONFIG_BT_REMOTE_VERSION */
};
#endif //CONFIG_BT_CONN

void btcli_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_conn_cb_register(&conn_callbacks);
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
            bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);
    }else{
         printf("bt_enable_failed\n");
    }
}

BLE_CLI(enable)
{
    if (atomic_test_bit(bt_dev.flags, BT_DEV_ENABLE)) {
        return;
    }
    #if !defined (CONFIG_BT_HOST_HCI_TL)
    // Initialize BLE controller
    #if defined(BL702) || defined(BL602)
    ble_controller_init(configMAX_PRIORITIES - 1);
    #else
    btble_controller_init(configMAX_PRIORITIES - 1);
    #endif
    #endif
    // Initialize BLE Host stack
    hci_driver_init();

    bt_enable(btcli_enable_cb);
}

BLE_CLI(set_chan_map)
{
    u8_t chan_map[5] = {0};
    uint8_t nb_good_channels = 0;
    int err,i,j;

    if(argc != 2){
       vOutputString("Number of Parameters is not correct\r\n");
       return;
    }
    get_bytearray_from_string(&argv[1], chan_map, sizeof(chan_map));

    // Make sure that the three most significant bits are 0
    if (chan_map[5 - 1] >> 5){
        vOutputString("Parameters is not correct\r\n");
        return;
    }

    // Count number of good channels
    for(i = 0; i < 5; ++i){
        for(j = 0; j < 8; ++j){
            if(chan_map[i]&(1<<j)){
                nb_good_channels++;
            }
        }
    }
    if(nb_good_channels == 0){
        vOutputString("Parameters is not correct\r\n");
        return;
    }

    printf("channel map %02x %02x %02x %02x %02x\r\n",
            chan_map[0], chan_map[1], chan_map[2], chan_map[3], chan_map[4]);
    err = bt_le_set_chan_map(chan_map);
    if(!err){
        vOutputString("Set ble channel map pending\r\n");
    }else{
        vOutputString("Failed to set ble channel map\r\n");
    }
}

BLE_CLI(init)
{
    #if defined(CONFIG_BT_CONN)
    default_conn = NULL;
    bt_conn_cb_register(&conn_callbacks);
    #endif
    vOutputString("Init successfully\r\n");
}

#if defined(BL702) || defined (BL616) ||defined(BL702L)
#if defined(CONFIG_BT_CONN)
BLE_CLI(set_2M_phy)
{
    int err = 0;

    if(!default_conn){
        vOutputString("Not connected \r\n");
        return;
    }
    err = hci_le_set_phy(default_conn, 0U, BT_HCI_LE_PHY_PREFER_2M,
				    BT_HCI_LE_PHY_PREFER_2M,
				    BT_HCI_LE_PHY_CODED_ANY);
    if(!err){
        vOutputString("Set ble 2M Phy pending \r\n");
    }else{
        vOutputString("Failed to set ble 2M Phy\r\n");
    }
}

BLE_CLI(set_coded_phy)
{
    int err = 0;
    uint8_t all_phys = 0;
    uint8_t opts;
    uint8_t coded_opts = BT_HCI_LE_PHY_CODED_ANY;

    if(!default_conn){
        vOutputString("Not connected \r\n");
        return;
    }

    get_uint8_from_string(&argv[1], &all_phys);

    get_uint8_from_string(&argv[2], &opts);
    if(opts == 2)
    {
        coded_opts = BT_HCI_LE_PHY_CODED_S2;
    }
    if(opts == 8)
    {
        coded_opts = BT_HCI_LE_PHY_CODED_S8;
    }

    err = hci_le_set_phy(default_conn, all_phys, BT_HCI_LE_PHY_PREFER_CODED,
				    BT_HCI_LE_PHY_PREFER_CODED,
				    coded_opts);
    if(!err){
        vOutputString("Set ble coded Phy pending \r\n");
    }else{
        vOutputString("Failed to set ble coded Phy\r\n");
    }
}

BLE_CLI(set_default_phy)
{
    u8_t default_phy = 0;
    
    get_uint8_from_string(&argv[1], &default_phy);
    
    if(!hci_le_set_default_phy(default_phy)){
        vOutputString("Set ble default Phy successfully \r\n");
    }else{
        vOutputString("Failed to set ble default Phy\r\n");
    }
}
#endif
#endif

BLE_CLI(le_rx_test)
{
    int err;
    u8_t rx_ch;

    if(argc != 2){
       vOutputString("Number of Parameters is not correct\r\n");
       return;
    }
    get_uint8_from_string(&argv[1], &rx_ch); 
    
    err = bt_ble_rx_test_cmd(rx_ch);
    if(err)
    {
        vOutputString("le rx test failed (err %d)\r\n", err); 
    }
    else
    {
        vOutputString("le rx test success\r\n");
    }
}

BLE_CLI(le_tx_test)
{
    int err;
    u8_t tx_ch;
    u8_t data_len;
    u8_t pkt_payload;

    if(argc != 4){
       vOutputString("Number of Parameters is not correct\r\n");
       return;
    }
    get_uint8_from_string(&argv[1], &tx_ch); 
    get_uint8_from_string(&argv[2], &data_len); 
    get_uint8_from_string(&argv[3], &pkt_payload); 
    
    err = bt_ble_tx_test_cmd(tx_ch, data_len, pkt_payload);
    if(err)
    {
        vOutputString("le tx test failed (err %d)\r\n", err); 
    }
    else
    {
        vOutputString("le tx test success\r\n");
    }
}

BLE_CLI(le_enh_tx_test)
{
    int err;
    u8_t tx_ch;
    u8_t test_data_len;
    u8_t pkt_payload;
    u8_t phy;

    if(argc != 5){
       vOutputString("Number of Parameters is not correct\r\n");
       return;
    }
    get_uint8_from_string(&argv[1], &tx_ch); 
    get_uint8_from_string(&argv[2], &test_data_len); 
    get_uint8_from_string(&argv[3], &pkt_payload); 
    get_uint8_from_string(&argv[4], &phy); 
    
    err = bt_le_enh_tx_test(tx_ch, test_data_len, pkt_payload, phy);
    if(err)
    {
        vOutputString("le tx test failed (err %d)\r\n", err); 
    }
    else
    {
        vOutputString("le tx test success\r\n");
    }

}

BLE_CLI(le_enh_rx_test)
{
    int err;
    u8_t rx_ch;
    u8_t phy;
    u8_t mod_index;

    if(argc != 4){
       vOutputString("Number of Parameters is not correct\r\n");
       return;
    }
    get_uint8_from_string(&argv[1], &rx_ch); 
    get_uint8_from_string(&argv[2], &phy); 
    get_uint8_from_string(&argv[3], &mod_index); 
    
    err = bt_le_enh_rx_test(rx_ch, phy, mod_index);
    if(err)
    {
        vOutputString("le rx test failed (err %d)\r\n", err); 
    }
    else
    {
        vOutputString("le rx test success\r\n");
    }
}

BLE_CLI(le_test_end)
{
    int err;
    err = bt_le_test_end();
    if(err)
    {
        vOutputString("set le test end failed (err %d)\r\n", err); 
    }
    else
    {
        vOutputString("set le test end success\r\n");
    }  
}



BLE_CLI(get_device_name)
{
	const char *device_name = bt_get_name();

	if(device_name){
		vOutputString("device_name: %s\r\n",device_name);
	}else
		vOutputString("Failed to read device name\r\n");
}

BLE_CLI(set_device_name)
{
	int	err = 0;

	if(strlen(argv[1]) > 0 && strlen(argv[1])<=CONFIG_BT_DEVICE_NAME_MAX){
		err = bt_set_name((char*)argv[1]);
		if(err){
			vOutputString("Failed to set device name\r\n");
		}else
			vOutputString("Set the device name successfully\r\n");
	}else{
		vOutputString("Invaild lenth(%d)\r\n",strlen(argv[1]));
	}
}

#if defined(CONFIG_BLE_TP_SERVER)
BLE_CLI(tp_start)
{
    extern u8_t tp_start;
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    get_uint8_from_string(&argv[1], &tp_start);
    if( tp_start == 1 ){
        vOutputString("Ble Throughput enable\r\n");
    }else if(tp_start == 0){
        vOutputString("Ble Throughput disable\r\n");
    }else{
        vOutputString("Invalid parameter\r\n");
    }
}
#endif

#if defined(CONFIG_BT_OBSERVER)
static bool data_cb(struct bt_data *data, void *user_data)
{
	char *name = user_data;
    u8_t len;

	switch (data->type) {
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
        len = (data->data_len > NAME_LEN - 1)?(NAME_LEN - 1):(data->data_len);
		memcpy(name, data->data, len);
		return false;		
	default:
		return true;
	}
}


static void device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t evtype,
			 struct net_buf_simple *buf)
{
	char 		le_addr[BT_ADDR_LE_STR_LEN];
	char 		name[NAME_LEN];

	(void)memset(name, 0, sizeof(name));
	bt_data_parse(buf, data_cb, name);
	bt_addr_le_to_str(addr, le_addr, sizeof(le_addr));
	
	vOutputString("[DEVICE]: %s, AD evt type %u, RSSI %i %s \r\n",le_addr, evtype, rssi, name);
}

BLE_CLI(start_scan)
{
    struct bt_le_scan_param scan_param;
    int err;

    (void)err;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    get_uint8_from_string(&argv[1], &scan_param.type);
    
    get_uint8_from_string(&argv[2], &scan_param.filter_dup);
    
    get_uint16_from_string(&argv[3], &scan_param.interval);
    
    get_uint16_from_string(&argv[4], &scan_param.window);

    err = bt_le_scan_start(&scan_param, device_found);
    
    if(err){
        vOutputString("Failed to start scan (err %d) \r\n", err);
    }else{
        vOutputString("Start scan successfully \r\n");
    }
}

BLE_CLI(stop_scan)
{
	int err;
    
	err = bt_le_scan_stop();
	if (err) {
		vOutputString("Stopping scanning failed (err %d)\r\n", err);
	} else {
		vOutputString("Scan successfully stopped \r\n");
	}
}

#if defined(BL702) || defined(BL602)
BLE_CLI(scan_filter_size)
{
	uint8_t size;
    int8_t err;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &size);
    
	err = ble_controller_set_scan_filter_table_size(size);
	if (err) {
		vOutputString("Set failed (err %d)\r\n", err);
	} else {
		vOutputString("Set success\r\n");
	}
}
#endif /* BL702 || BL602 */

#endif //#if defined(CONFIG_BT_OBSERVER)


#if defined(CONFIG_BT_PERIPHERAL)
BLE_CLI(read_local_address)
{
	bt_addr_le_t local_pub_addr;
	bt_addr_le_t local_ram_addr;
	char le_addr[BT_ADDR_LE_STR_LEN];

       bt_get_local_public_address(&local_pub_addr);
	bt_addr_le_to_str(&local_pub_addr, le_addr, sizeof(le_addr));
	vOutputString("Local public addr : %s\r\n", le_addr);

	bt_get_local_ramdon_address(&local_ram_addr);
	bt_addr_le_to_str(&local_ram_addr, le_addr, sizeof(le_addr));
	vOutputString("Local random addr : %s\r\n", le_addr);
}

BLE_CLI(set_local_address)
{
    bt_addr_le_t addr;
    u8_t  addr_val[6];
    int   err = -1;
	
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    get_bytearray_from_string(&argv[1], addr_val,6);
    reverse_bytearray(addr_val, addr.a.val, 6);

    err = bt_set_local_public_address(addr.a.val);
    if(err)
    {
        vOutputString("Failed to set bt address (err %d) \r\n", err);
    }
    else
    {
        vOutputString("Set bt address success\r\n");
    }

}

BLE_CLI(set_adv_channel)
{
    u8_t channel = 7;

    if(argc != 2)
    {
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    get_uint8_from_string(&argv[1], &channel);

    if (set_adv_channel_map(channel) == 0)
    {
        vOutputString("Set adv channel success\r\n");
    }
    else
    {
        vOutputString("Failed to Set adv channel\r\n");
    }
}

BLE_CLI(start_advertise)
{
    struct bt_le_adv_param param;
    const struct bt_data *ad;
    size_t ad_len;
    int err = -1;
    uint8_t adv_type, mode;
	
    if(argc != 3 && argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    uint8_t non_disc = BT_LE_AD_NO_BREDR;
    uint8_t gen_disc = BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL;
    uint8_t lim_disc = BT_LE_AD_NO_BREDR | BT_LE_AD_LIMITED;
    char *adv_name = (char*)bt_get_name();
    struct bt_data ad_discov[2] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
    };
 
    param.id = selected_id;
    param.interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
    param.interval_max = BT_GAP_ADV_FAST_INT_MAX_2;

    /*Get adv type, 0:adv_ind,  1:adv_scan_ind, 2:adv_nonconn_ind 3: adv_direct_ind*/
    get_uint8_from_string(&argv[1], &adv_type);
    vOutputString("adv_type 0x%x\r\n",adv_type);
    if(adv_type == 0){
        param.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME);
    }else if(adv_type == 1){
        param.options = BT_LE_ADV_OPT_USE_NAME;
    }else if(adv_type == 2){
        param.options = 0;
    }else{
        vOutputString("Arg1 is invalid\r\n");
        return;
    }
    
    /*Get mode, 0:General discoverable,  1:limit discoverable, 2:non discoverable*/
    get_uint8_from_string(&argv[2], &mode);
    vOutputString("mode 0x%x\r\n",mode);
    
    if (mode == 0) {
        ad_discov[0].data = &gen_disc;
    } else if (mode == 1) {
        ad_discov[0].data = &lim_disc;
    } else if (mode == 2) {
        ad_discov[0].data = &non_disc;
    } else {
        vOutputString("Invalied AD Mode 0x%x\r\n",mode);
        return;
    }

    ad = ad_discov;
    ad_len = ARRAY_SIZE(ad_discov);


    if(argc == 5){
        get_uint16_from_string(&argv[3], &param.interval_min);
        get_uint16_from_string(&argv[4], &param.interval_max);

      	vOutputString("interval min 0x%x\r\n",param.interval_min);  
      	vOutputString("interval max 0x%x\r\n",param.interval_max);  
    }
	
    if(adv_type == 1 || adv_type == 0){
        #if defined(CONFIG_BLE_MULTI_ADV)
        if(ble_adv_id == 0){
            err = bt_le_multi_adv_start(&param, ad, ad_len, &ad_discov[0], 1, &ble_adv_id);
        }        
        #else
        err = bt_le_adv_start(&param, ad, ad_len, &ad_discov[0], 1);
        #endif/*CONFIG_BLE_MULTI_ADV*/
    }else{
        #if defined(CONFIG_BLE_MULTI_ADV)
        if(ble_adv_id == 0){
            err = bt_le_multi_adv_start(&param, ad, ad_len, NULL, 0, &ble_adv_id);
        }
        #else
        err = bt_le_adv_start(&param, ad, ad_len, NULL, 0);
        #endif
    }
 
    if(err){
        vOutputString("Failed to start advertising (err %d) \r\n", err);
    }else{
        vOutputString("Advertising started\r\n");
    }

}

BLE_CLI(stop_advertise)
{
#if defined(CONFIG_BLE_MULTI_ADV)
    bool err = -1;
    if(ble_adv_id && !bt_le_multi_adv_stop(ble_adv_id)){
        ble_adv_id = 0;
        err = 0;
    }
    if(err){
#else
    if(bt_le_adv_stop()){
#endif
        vOutputString("Failed to stop advertising\r\n");	
    }else{ 
        vOutputString("Advertising stopped\r\n");
    }
}

#if defined(CONFIG_BLE_MULTI_ADV)
struct bt_data data_1 = (struct bt_data)BT_DATA_BYTES(BT_DATA_NAME_COMPLETE, "multi_adv_connect_01");
struct bt_data data_2 = (struct bt_data)BT_DATA_BYTES(BT_DATA_NAME_COMPLETE, "multi_adv_nonconn_02");
BLE_CLI(start_multi_advertise)
{
    struct bt_le_adv_param param_1, param_2;
    struct bt_data *ad_1, *ad_2;
    size_t ad_len_1, ad_len_2;
    int err_1, err_2;
    int instant_id_1, instant_id_2;
    char *adv_name = (char*)bt_get_name();
    struct bt_data ad_discov[2] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
    };

    param_1.id = 0;
    param_1.interval_min = 0x00A0;
    param_1.interval_max = 0x00A0;
    param_1.options = BT_LE_ADV_OPT_CONNECTABLE;

    ad_discov[1] = data_1;
    ad_1 = ad_discov;
    ad_len_1 = ARRAY_SIZE(ad_discov);

    err_1 = bt_le_multi_adv_start(&param_1, ad_1, ad_len_1, NULL, 0, &instant_id_1);
    if(err_1){
        vOutputString("Failed to start multi adv_1 (err_1%d)\r\n", err_1);
    }else{
        vOutputString("Multi Adv started --> instant_id: %d\r\n", instant_id_1);
    }

    param_2.id = 0;
    param_2.interval_min = 0x0140;
    param_2.interval_max = 0x0140;
    param_2.options = 0;

    ad_discov[1] = data_2;
    ad_2 = ad_discov;
    ad_len_2 = ARRAY_SIZE(ad_discov);

    err_2 = bt_le_multi_adv_start(&param_2, ad_2, ad_len_2, NULL, 0, &instant_id_2);
    if(err_2){
        vOutputString("Failed to start multi adv_2 (err_2: %d)\r\n", err_2);
    }else{
        vOutputString("Multi Adv started --> instant_id: %d\r\n", instant_id_2);
    }
}

BLE_CLI(stop_multi_advertise)
{
    uint8_t instant_id;

    get_uint8_from_string(&argv[1], &instant_id);
    printf("Try to stop multi adv instant of: %d\n", instant_id);

    if(bt_le_multi_adv_stop(instant_id)) {
        vOutputString("Multi adv instant %d stop failed\r\n", instant_id);
    }
    else
    {
        vOutputString("Multi adv instant %d stop successed\r\n", instant_id);
    }
}
#endif //CONFIG_BLE_MULTI_ADV

#endif //#if defined(CONFIG_BT_PERIPHERAL)

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
BLE_CLI(connect)
{
    bt_addr_le_t addr;
    struct bt_conn *conn;
    u8_t  addr_val[6];
	s8_t  type = -1;
	int i=0;

	struct bt_le_conn_param param = {
		.interval_min =  BT_GAP_INIT_CONN_INT_MIN,
		.interval_max =  BT_GAP_INIT_CONN_INT_MAX,
		.latency = 0,
		.timeout = 400,
	};

	(void)memset(addr_val,0,6);

    if(argc != 3 && argc != 6){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
       
    get_uint8_from_string(&argv[1], (u8_t *)&type);

	/*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    addr.type = type;

    get_bytearray_from_string(&argv[2], addr_val,6);
     for(i=0;i<6;i++){
		vOutputString("addr[%d]:[0x%x]\r\n",i,addr_val[i]);
    }
	
    reverse_bytearray(addr_val, addr.a.val, 6);
    if(argc == 6)
    {
        get_uint16_from_string(&argv[3], &param.interval_min);
        get_uint16_from_string(&argv[4], &param.interval_max);
        get_uint16_from_string(&argv[5], &param.timeout);
    }

    conn = bt_conn_create_le(&addr, /*BT_LE_CONN_PARAM_DEFAULT*/&param);

    if(!conn){
        vOutputString("Connection failed\r\n");
    }else{
        if(conn->state == BT_CONN_CONNECTED)
            vOutputString("Le link with this peer device has existed\r\n");
        else
            vOutputString("Connection pending\r\n");
    }
}

#if defined(CONFIG_BT_WHITELIST)
BLE_CLI(auto_connect)
{
	int err;
	unsigned char enable = 0U;
	struct bt_le_conn_param param = {
		.interval_min =  BT_GAP_INIT_CONN_INT_MIN,
		.interval_max =  BT_GAP_INIT_CONN_INT_MAX,
		.latency = 0,
		.timeout = 400,
	};
	/*Auto connect whitelist device, enable : 0x01, disable : 0x02*/
	get_uint8_from_string(&argv[1], &enable);

	if(enable == 0x01){
		err = bt_conn_create_auto_le(&param);
		if(err){
			vOutputString("Auto connect failed (err = [%d])\r\n",err);
			return;
		}else{
			vOutputString("Auto connection is succeed\r\n");
		}
	}else if(enable == 0x02){
		err = bt_conn_create_auto_stop();
		if(err){
			vOutputString("Auto connection stop (err = [%d])\r\n",err);
			return ;
		}
	}
}

BLE_CLI(whitelist_add)
{
	bt_addr_le_t waddr;
	int 		err;
	u8_t 		val[6];

	if(argc != 3){
		printf("Number of Parameters is not correct (argc = [%d])\r\n",argc);
		return;
	}
	// 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*
	get_uint8_from_string(&argv[1], &waddr.type);
	get_bytearray_from_string(&argv[2], val,6);

	reverse_bytearray(val, waddr.a.val, 6);

	err = bt_le_whitelist_add(&waddr);
	if(err){
		printf("Failed to add device to whitelist (err = [%d])\r\n",err);
	}
}

BLE_CLI(whitelist_rem)
{
	bt_addr_le_t waddr;
	int 		err;
	u8_t 		val[6];

	if(argc != 3){
		printf("Number of Parameters is not correct (argc = [%d])\r\n",argc);
		return;
	}

	get_uint8_from_string(&argv[1], &waddr.type);
	get_bytearray_from_string(&argv[2], val,6);

	reverse_bytearray(val, waddr.a.val, 6);

	err = bt_le_whitelist_rem(&waddr);
	if(err){
		printf("Failed to add device to whitelist (err = [%d])\r\n",err);
	}
}

BLE_CLI(whitelist_clear)
{
	int 		err;

	if(argc != 1){
		printf("Number of Parameters is not correct (argc = [%d])\r\n",argc);
		return;
	}

	err = bt_le_whitelist_clear();
	if(err){
		printf("Clear white list device failed (err = [%d])\r\n",err);
	}
}
#endif /* CONFIG_BT_WHITELIST */

#endif //#if defined(CONFIG_BT_CENTRAL)

BLE_CLI(disconnect)
{
    bt_addr_le_t addr;
    u8_t  addr_val[6];
    struct bt_conn *conn;
	s8_t  type = -1;
	
    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    get_uint8_from_string(&argv[1], (u8_t *)&type);
    get_bytearray_from_string(&argv[2], addr_val,6);
    reverse_bytearray(addr_val, addr.a.val, 6);

	/*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    addr.type = type;

    conn = bt_conn_lookup_addr_le(selected_id, &addr);

    if(!conn){
        vOutputString("Not connected\r\n");
        return;
    }

    if(bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN)){
        vOutputString("Disconnection failed\r\n");
    }else{
        vOutputString("Disconnect successfully\r\n");
    }

    /*Notice:Because conn is got via bt_conn_lookup_addr_le in which bt_conn_ref(increase conn_ref by 1)
      this conn, we need to bt_conn_unref(decrease conn_ref by 1) this conn.*/
    bt_conn_unref(conn);
}

BLE_CLI(select_conn)
{
    bt_addr_le_t addr;
    struct bt_conn *conn;
    u8_t  addr_val[6];

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    /*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    get_uint8_from_string(&argv[1], &addr.type);

    get_bytearray_from_string(&argv[2], addr_val,6);

    reverse_bytearray(addr_val, addr.a.val, 6);
    
    conn = bt_conn_lookup_addr_le(selected_id, &addr);
           
    if(!conn){
        vOutputString("No matching connection found\r\n");
        return;
    }

    if(conn){
        bt_conn_unref(conn);
    }

    default_conn = conn;
}

BLE_CLI(unpair)
{
    bt_addr_le_t addr;
    u8_t  addr_val[6];
    int err;

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    /*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    get_uint8_from_string(&argv[1], &addr.type);
    
    get_bytearray_from_string(&argv[2], addr_val,6);

    reverse_bytearray(addr_val, addr.a.val, 6);
        
    err = bt_unpair(selected_id, &addr);

    if(err){
        vOutputString("Failed to unpair\r\n");
    }else{
        vOutputString("Unpair successfully\r\n");
    }
}

BLE_CLI(conn_update)
{
	struct bt_le_conn_param param;
	int err;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    get_uint16_from_string(&argv[1], &param.interval_min);
    get_uint16_from_string(&argv[2], &param.interval_max);
    get_uint16_from_string(&argv[3], &param.latency);
    get_uint16_from_string(&argv[4], &param.timeout);	
    err = bt_conn_le_param_update(default_conn, &param);

	if (err) {
		vOutputString("conn update failed (err %d)\r\n", err);
	} else {
		vOutputString("conn update initiated\r\n");
	}
}

BLE_CLI(send_l2cap_conn_param_update_req)
{
	struct bt_le_conn_param param;
	int err;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    get_uint16_from_string(&argv[1], &param.interval_min);
    get_uint16_from_string(&argv[2], &param.interval_max);
    get_uint16_from_string(&argv[3], &param.latency);
    get_uint16_from_string(&argv[4], &param.timeout);	
    err = bt_l2cap_update_conn_param(default_conn, &param);

	if (err) {
		vOutputString("Fail to send l2cap conn param update request (err %d)\r\n", err);
	} else {
		vOutputString("conn update initiated\r\n");
	}
}

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
BLE_CLI(l2cap_send_test_data)
{
    int err = 0;
    uint8_t test_data[10] = {0x01, 0x02, 0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a};
    uint8_t large_test_data[30];
    uint16_t cid;
    uint8_t isLarge;

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
     
    get_uint16_from_string(&argv[1], &cid);
    get_uint8_from_string(&argv[2], &isLarge);

    extern int bt_l2cap_send_data(struct bt_conn *conn, uint16_t cid, uint8_t *data, uint8_t len);
    if(isLarge)
    {
        for(int i = 0; i < sizeof(large_test_data); i++)
            large_test_data[i] = i; 
        err = bt_l2cap_send_data(default_conn, cid, large_test_data, sizeof(large_test_data));
    }
    else
        err = bt_l2cap_send_data(default_conn, cid, test_data, sizeof(test_data));

    if(err)
        printf("Fail to send l2cap test data with error (%d)\r\n", err);
    else
        printf("Send l2cap test data successfully\r\n");
}


BLE_CLI(l2cap_disconnect)
{
    int err = 0;
    uint16_t tx_cid;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
    get_uint16_from_string(&argv[1], &tx_cid);

    err = bt_l2cap_disconnect(default_conn, tx_cid);

    if(err)
        printf("Fail to send l2cap disconnect request with error (%d)\r\n", err);
    else
        printf("Send l2cap disconnect request successfully\r\n");
}
#endif

#if(BFLB_BLE_ENABLE_TEST_PSM)
BLE_CLI(register_test_psm)
{
    int err = 0;
    uint16_t psm;
    uint8_t sec_level;
    uint8_t l2cap_policy;
    bool add_allow;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    get_uint16_from_string(&argv[1], &psm);
    get_uint8_from_string(&argv[2], &sec_level);
    get_uint8_from_string(&argv[3], &l2cap_policy);
    get_uint8_from_string(&argv[4], (uint8_t *)&add_allow);

    extern int bt_register_test_psm(uint16_t psm, uint8_t sec_level, uint8_t policy,bool add_allow);
    err = bt_register_test_psm(psm, sec_level,l2cap_policy,add_allow);

    if (err) {
        vOutputString("Fail to register test psm(err %d)\r\n", err);
    } else {
        vOutputString("Register test psm successfully\r\n");
    }
}

BLE_CLI(connect_test_psm)
{
    int err = 0;
    uint16_t psm;
    
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    get_uint16_from_string(&argv[1], &psm);
    
    extern int bt_connect_test_psm(struct bt_conn *conn, uint16_t psm);
    bt_connect_test_psm(default_conn, psm);

    if (err) {
        vOutputString("Fail to connect using test psm(err %d)\r\n", err);
    } else {
        vOutputString("connection using test psm initiated\r\n");
    }
}
#endif

BLE_CLI(read_rssi)
{
    int8_t rssi;
    int err;

    if(!default_conn){
        vOutputString("not connect\r\n");
        return;
    }

    err = bt_le_read_rssi(default_conn->handle, &rssi);
    if (err) {
        vOutputString("read rssi failed (err %d)\r\n", err);
    } else {
        vOutputString("read rssi %d\r\n", rssi);
    }
}

#if defined(BL702L) || defined(BL616) || defined(BL606P) || defined(BL808)
BLE_CLI(ble_throughput_calc)
{  
    int err;
    u8_t enable;
    u8_t interval;
    if(argc != 3){
       vOutputString("Number of Parameters is not correct\r\n");
       return;
    }
    get_uint8_from_string(&argv[1], (uint8_t *)&enable); 
    get_uint8_from_string(&argv[2], &interval); 
    extern int bt_le_throughput_calc(bool enable, u8_t interval);
    err = bt_le_throughput_calc(enable,interval);
    if(err)
    {
        vOutputString("set le throughput calculation failed (err %d)\r\n", err); 
    }
    else
    {
        vOutputString("set le throughput calculation success\r\n");
    }
    
}
#endif

#endif //#if defined(CONFIG_BT_CONN)

#if defined(CONFIG_BT_SMP)
BLE_CLI(security)
{
    int err;
    u8_t sec_level = /*BT_SECURITY_FIPS*/BT_SECURITY_L4;

    if(!default_conn){
        vOutputString("Please firstly choose the connection using ble_select_conn\r\n");
        return;
    }

    if(argc == 2)
        get_uint8_from_string(&argv[1], &sec_level);
    
    err = bt_conn_set_security(default_conn, sec_level);

    if(err){
        vOutputString("Failed to start security, (err %d) \r\n", err);
    }else{
        vOutputString("Start security successfully\r\n");
    }
}

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));    

    vOutputString("passkey_str is: %06u\r\n", passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	vOutputString("Confirm passkey for %s: %06u\r\n", addr, passkey);
}

static void auth_passkey_entry(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	vOutputString("Enter passkey for %s\r\n", addr);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	
	vOutputString("Pairing cancelled: %s\r\n", addr);
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	vOutputString("Confirm pairing for %s\r\n", addr);
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	vOutputString("%s with %s\r\n", bonded ? "Bonded" : "Paired", addr);
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	vOutputString("Pairing failed with %s\r\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = auth_passkey_entry,
	.passkey_confirm = auth_passkey_confirm,
	.cancel = auth_cancel,
	.pairing_confirm = auth_pairing_confirm,
	.pairing_failed = auth_pairing_failed,
	.pairing_complete = auth_pairing_complete,
};

BLE_CLI(auth)
{
    int err;

    err = bt_conn_auth_cb_register(&auth_cb_display);

    if(err){
        vOutputString("Auth callback has already been registered\r\n");
    }else{
        vOutputString("Register auth callback successfully\r\n");
    }
}

BLE_CLI(auth_cancel)
{
	struct bt_conn *conn;
    
	if (default_conn) {
		conn = default_conn;
	}else {
		conn = NULL;
	}

	if (!conn) {
        vOutputString("Not connected\r\n");
		return;
	}

	bt_conn_auth_cancel(conn);
}

BLE_CLI(auth_passkey_confirm)
{
    
	if (!default_conn) {
        vOutputString("Not connected\r\n");
		return;
	}

	bt_conn_auth_passkey_confirm(default_conn);
}

BLE_CLI(auth_pairing_confirm)
{
   
	if (!default_conn) {
        vOutputString("Not connected\r\n");
		return;
	}

	bt_conn_auth_pairing_confirm(default_conn);
}

BLE_CLI(auth_passkey)
{
	uint32_t passkey;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
        vOutputString("Not connected\r\n");
		return;
	}

    passkey = atoi(argv[1]);
	if (passkey > PASSKEY_MAX) {
        vOutputString("Passkey should be between 0-999999\r\n");
		return;
	}

	bt_conn_auth_passkey_entry(default_conn, passkey);
}

#endif //#if defined(CONFIG_BT_SMP)

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_GATT_CLIENT)
static void exchange_func(struct bt_conn *conn, u8_t err,
			  struct bt_gatt_exchange_params *params)
{
	vOutputString("Exchange %s MTU Size =%d \r\n", err == 0U ? "successful" : "failed",bt_gatt_get_mtu(conn));
}

static struct bt_gatt_exchange_params exchange_params;

BLE_CLI(exchange_mtu)
{
	int err;
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	exchange_params.func = exchange_func;

	err = bt_gatt_exchange_mtu(default_conn, &exchange_params);
	if (err) {
		vOutputString("Exchange failed (err %d)\r\n", err);
	} else {
		vOutputString("Exchange pending\r\n");
	}
}

static struct bt_gatt_discover_params discover_params;
static struct bt_uuid_16 uuid = BT_UUID_INIT_16(0);

static void print_chrc_props(u8_t properties)
{
	vOutputString("Properties: ");

	if (properties & BT_GATT_CHRC_BROADCAST) {
		vOutputString("[bcast]\r\n");
	}

	if (properties & BT_GATT_CHRC_READ) {
		vOutputString("[read]\r\n");
	}

	if (properties & BT_GATT_CHRC_WRITE) {
		vOutputString("[write]\r\n");
	}

	if (properties & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
		vOutputString("[write w/w rsp]\r\n");
	}

	if (properties & BT_GATT_CHRC_NOTIFY) {
		vOutputString("[notify]\r\n");
	}

	if (properties & BT_GATT_CHRC_INDICATE) {
		vOutputString("[indicate]");
	}

	if (properties & BT_GATT_CHRC_AUTH) {
		vOutputString("[auth]\r\n");
	}

	if (properties & BT_GATT_CHRC_EXT_PROP) {
		vOutputString("[ext prop]\r\n");
	}
}

u8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params)
{
	struct bt_gatt_service_val *gatt_service;
	struct bt_gatt_chrc *gatt_chrc;
	struct bt_gatt_include *gatt_include;
	char str[37];

	if (!attr) {
		vOutputString( "Discover complete\r\n");
		#if defined(BFLB_BLE_PATCH_ADD_ERRNO_IN_DISCOVER_CALLBACK)
		if(params->err != 0){
			printf("Discover erro %x\n", params->err);
		}
		#endif /* BFLB_BLE_PATCH_ADD_ERRNO_IN_DISCOVER_CALLBACK */
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	switch (params->type) {
	case BT_GATT_DISCOVER_SECONDARY:
	case BT_GATT_DISCOVER_PRIMARY:
		gatt_service = attr->user_data;
		bt_uuid_to_str(gatt_service->uuid, str, sizeof(str));
		vOutputString("Service %s found: start handle %x, end_handle %x\r\n", str, attr->handle, gatt_service->end_handle);
		break;
	case BT_GATT_DISCOVER_CHARACTERISTIC:
		gatt_chrc = attr->user_data;
		bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));
		vOutputString("Characteristic %s found: attr->handle %x  chrc->handle %x \r\n", str, attr->handle,gatt_chrc->value_handle);
		print_chrc_props(gatt_chrc->properties);
		break;
	case BT_GATT_DISCOVER_INCLUDE:
		gatt_include = attr->user_data;
		bt_uuid_to_str(gatt_include->uuid, str, sizeof(str));
		vOutputString("Include %s found: handle %x, start %x, end %x\r\n", str, attr->handle,
			    gatt_include->start_handle, gatt_include->end_handle);
		break;
	default:
		bt_uuid_to_str(attr->uuid, str, sizeof(str));
		vOutputString("Descriptor %s found: handle %x\r\n", str, attr->handle);
		break;
	}

	return BT_GATT_ITER_CONTINUE;
}

BLE_CLI(discover)
{
	int err;
    u8_t disc_type;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	discover_params.func = discover_func;
	discover_params.start_handle = 0x0001;
	discover_params.end_handle = 0xffff;

    get_uint8_from_string(&argv[1], &disc_type);
    if(disc_type == 0){
        discover_params.type = BT_GATT_DISCOVER_PRIMARY;
    }else if(disc_type == 1){
        discover_params.type = BT_GATT_DISCOVER_SECONDARY;
    }else if(disc_type == 2){
        discover_params.type = BT_GATT_DISCOVER_INCLUDE;
    }else if(disc_type == 3){
        discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    }else if(disc_type == 4){
        discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    }else{
        vOutputString("Invalid discovery type\r\n");
        return;
    }
    get_uint16_from_string(&argv[2], &uuid.val);
	
    if(uuid.val)
        discover_params.uuid = &uuid.uuid;
    else
        discover_params.uuid = NULL;

    get_uint16_from_string(&argv[3], &discover_params.start_handle);
    get_uint16_from_string(&argv[4], &discover_params.end_handle);

	err = bt_gatt_discover(default_conn, &discover_params);
	if (err) {
		vOutputString("Discover failed (err %d)\r\n", err);
	} else {
		vOutputString("Discover pending\r\n");
	}
}

static struct bt_gatt_read_params read_params;

static u8_t read_func(struct bt_conn *conn, u8_t err, struct bt_gatt_read_params *params, const void *data, u16_t length)
{
	vOutputString("Read complete: err %u length %u \r\n", err, length);

	char str[22]; 
	u8_t *buf = (u8_t *)data;
    int i=0;

	memset(str,0,15);
	
	if(length > 0 && length <= sizeof(str)){
		memcpy(str,buf,length);
		vOutputString("device name : %s \r\n",str);
        for(i=0;i<length;i++){
            vOutputString("buf=[0x%x]\r\n",buf[i]);
        }
	}

	if (!data) {
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_CONTINUE;
}

BLE_CLI(read)
{
	int err;

    if(argc != 3){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

    get_uint16_from_string(&argv[1], &read_params.single.handle);
    get_uint16_from_string(&argv[2], &read_params.single.offset);

    read_params.func = read_func;
	read_params.handle_count = 1;

	err = bt_gatt_read(default_conn, &read_params);
	if (err) {
		vOutputString("Read failed (err %d)\r\n", err);
	} else {
		vOutputString("Read pending\r\n");
	}
}

static struct bt_gatt_write_params write_params;
static u8_t *gatt_write_buf = NULL;
static void write_func(struct bt_conn *conn, u8_t err,
		       struct bt_gatt_write_params *params)
{
	vOutputString("Write complete: err %u \r\n", err);
    if(gatt_write_buf != NULL){
        k_free(gatt_write_buf);
        gatt_write_buf = NULL;
    }
    if(err == BT_ATT_ERR_PREPARE_QUEUE_FULL){
        ble_gatt_cancel_prepare_writes_params *cancel_params = k_malloc(sizeof(ble_gatt_cancel_prepare_writes_params));
        if(!cancel_params){
            vOutputString("Fail to allocate buffer");
            return;
        }
        cancel_params->msg_type = CLI_MSG_GATT_CANCEL_PREPARE_WRITES;
        cancel_params->conn = conn;
        cancel_params->write_params = params;
        k_fifo_put(&ble_cli_queue, (void *)cancel_params);
    }else
        memset(params, 0, sizeof(struct bt_gatt_write_params));
}

BLE_CLI(write)
{
	int err;
    uint16_t data_len;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	if (write_params.func) {
		vOutputString("Write ongoing\r\n");
		return;
	}

    get_uint16_from_string(&argv[1], &write_params.handle);  
    get_uint16_from_string(&argv[2], &write_params.offset);
    get_uint16_from_string(&argv[3], &write_params.length);
    data_len = write_params.length;
    gatt_write_buf = k_malloc(data_len);
    if(!gatt_write_buf)
    {
        vOutputString("Failed to alloc buffer for the data\r\n");
        return;
    }
    get_bytearray_from_string(&argv[4], gatt_write_buf,data_len);
    
	write_params.data = gatt_write_buf;
	write_params.length = data_len;
	write_params.func = write_func;

	err = bt_gatt_write(default_conn, &write_params);
    
	if (err) {
		vOutputString("Write failed (err %d)\r\n", err);
	} else {
		vOutputString("Write pending\r\n");
	}
}

BLE_CLI(write_without_rsp)
{
	u16_t handle;
	int err;
	u16_t len;
	bool sign;
    u8_t *gatt_write_buf;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
   
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

    get_uint8_from_string(&argv[1], (uint8_t *)&sign);
    get_uint16_from_string(&argv[2], &handle);
	get_uint16_from_string(&argv[3], &len);
    gatt_write_buf = k_malloc(len);
    if(!gatt_write_buf)
    {
        vOutputString("Failed to alloc buffer for the data\r\n");
        return;
    }
	get_bytearray_from_string(&argv[4], gatt_write_buf,len);
	
	err = bt_gatt_write_without_response(default_conn, handle, gatt_write_buf, len, sign);

    k_free(gatt_write_buf);

	vOutputString("Write Complete (err %d)\r\n", err);
}

static struct bt_gatt_subscribe_params subscribe_params;

static u8_t notify_func(struct bt_conn *conn,
			struct bt_gatt_subscribe_params *params,
			const void *data, u16_t length)
{
#if defined(CONFIG_BLE_TP_TEST)
    static u32_t time = 0;
    static int len = 0;
#endif  

    if (!params->value) {
        vOutputString("Unsubscribed\r\n");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }

#if defined(CONFIG_BLE_TP_TEST)    
    if(!time){
        time = k_now_ms();
    }
    len += length;
    if(k_now_ms()- time >= 1000){
        vOutputString("data rate = [%d byte]\r\n", len);
        time = k_now_ms();
        len = 0;
    }
#endif   

    //vOutputString("Notification: data length %u\r\n", length);
    return BT_GATT_ITER_CONTINUE;
}

BLE_CLI(subscribe)
{
    if(argc != 4){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    if (!default_conn) {
        vOutputString("Not connected\r\n");
        return;
    }

    get_uint16_from_string(&argv[1], &subscribe_params.ccc_handle);
    get_uint16_from_string(&argv[2], &subscribe_params.value_handle);
    get_uint16_from_string(&argv[3], &subscribe_params.value);
    subscribe_params.notify = notify_func;

    int err = bt_gatt_subscribe(default_conn, &subscribe_params);
    if (err) {
        vOutputString("Subscribe failed (err %d)\r\n", err);
    } else {
        vOutputString("Subscribed\r\n");
    }

}

BLE_CLI(unsubscribe)
{
    if (!default_conn) {
        vOutputString("Not connected\r\n");
        return;
    }

    if (!subscribe_params.value_handle) {
        vOutputString("No subscription found\r\n");
        return;
    }

    int err = bt_gatt_unsubscribe(default_conn, &subscribe_params);
    if (err) {
        vOutputString("Unsubscribe failed (err %d)\r\n", err);
    } else {
        vOutputString("Unsubscribe success\r\n");
    }
}
#endif /* CONFIG_BT_GATT_CLIENT */

BLE_CLI(set_data_len)
{
	u16_t tx_octets;
	u16_t tx_time;
	int err;

	if(argc != 3){
	    vOutputString("Number of Parameters is not correct\r\n");
	    return;
	}

	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	get_uint16_from_string(&argv[1], &tx_octets);
	get_uint16_from_string(&argv[2], &tx_time);

	err = bt_le_set_data_len(default_conn, tx_octets, tx_time);
	if (err) {
		vOutputString("ble_set_data_len, LE Set Data Length (err %d)\r\n", err);
	}
	else
	{
		vOutputString("ble_set_data_len, LE Set Data Length success\r\n");
	}
}

BLE_CLI(get_all_conn_info)
{
        struct bt_conn_info info[CONFIG_BT_MAX_CONN];
        char le_addr[BT_ADDR_LE_STR_LEN];
        int link_num;

        link_num = bt_conn_get_remote_dev_info(info);

        if(link_num > 0)
        {
                bt_addr_le_to_str(info[0].le.local, le_addr, sizeof(le_addr));
                vOutputString("ble local device address: %s\r\n", le_addr);
        }

        vOutputString("ble connected devices count: %d\r\n", link_num);
        for(int i = 0; i < link_num; i++)
        {
	        bt_addr_le_to_str(info[i].le.remote, le_addr, sizeof(le_addr));
	        vOutputString("[%d]: address %s\r\n", i, le_addr);
        }
}
#endif /* CONFIG_BT_CONN */

BLE_CLI(set_tx_pwr)
{
    u8_t power;
    int err;

    if(argc != 2){
	    vOutputString("Number of Parameters is not correct\r\n");
	    return;
	}

    get_uint8_from_string(&argv[1],&power);
    #if defined(BL602)
    if(power > 21){
        vOutputString("ble_set_tx_pwr, invalid value, value shall be in [%d - %d]\r\n", 0, 21);
        return;
    }
    #elif defined(BL702) || defined(BL616)
    if(power > 14){
        vOutputString("ble_set_tx_pwr, invalid value, value shall be in [%d - %d]\r\n", 0, 14);
        return;
    }
	#endif
    err = bt_set_tx_pwr((int8_t)power);

    if(err){
		vOutputString("ble_set_tx_pwr, Fail to set tx power (err %d)\r\n", err);
	}
	else{
		vOutputString("ble_set_tx_pwr, Set tx power successfully\r\n");
	}
    
}

#if defined(BFLB_DISABLE_BT)
BLE_CLI(disable)
{
	int err;

    err = bt_disable();
    if(err){
        vOutputString("err =%d,fail to disable bt\r\n",err);
    }else{
        vOutputString("Disable bt successfully\r\n");
    }
}
#endif /* BFLB_DISABLE_BT */

#if defined(CONFIG_HOGP_SERVER)
BLE_CLI(hog_srv_notify)
{
    uint16_t hid_usage;
    uint8_t press;
    int err = 0;

    if(!default_conn){
        vOutputString("Not connected\r\n");
        return;
    }

    get_uint16_from_string(&argv[1],&hid_usage);
    get_uint8_from_string(&argv[2],&press);

    err = hog_notify(default_conn,hid_usage,press);
    if(err){
        vOutputString("Failed to send notification\r\n");
    }else{
        vOutputString("Notification sent successfully\r\n");
    }
}
#endif /* CONFIG_HOGP_SERVER */
#if defined(BFLB_BLE_DYNAMIC_SERVICE)
#if defined(CONFIG_BT_PERIPHERAL)

#if defined(CONFIG_BT_SPP_SERVER)
BLE_CLI(add_spp_service)
{
    extern void bt_dyn_register_spp_srv(void);
    bt_dyn_register_spp_srv();
}

BLE_CLI(del_spp_service)
{
    void bt_dyn_unregister_spp_srv(void);
    bt_dyn_unregister_spp_srv();
}
#endif /* CONFIG_BT_SPP_SERVER */

BLE_CLI(gatts_get_service_info)
{
    struct simple_svc_info svc_info[2];
    uint16_t svc_id;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    memset(svc_info,0,sizeof(svc_info));
    get_uint16_from_string(&argv[1],&svc_id);

    bt_gatts_get_service_simple_info(svc_id,&svc_info[0],sizeof(svc_info)/sizeof(struct simple_svc_info));
    for(int i=0;i<sizeof(svc_info)/sizeof(struct simple_svc_info);i++){
        vOutputString("svc_info : i(%d),idx(%d),state(%d),uuid(%s),type(%d)\r\n",i,svc_info[i].idx,svc_info[i].state,
                svc_info[i].uuid,svc_info[i].type);
    }
}

BLE_CLI(gatts_get_char)
{
    struct char_info cinfo[3];
    uint16_t svc_id;

    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    memset(cinfo,0,sizeof(cinfo));
    get_uint16_from_string(&argv[1],&svc_id);

    bt_gatts_get_service_char(svc_id,cinfo,sizeof(cinfo)/sizeof(struct char_info));
    for(int i=0;i<sizeof(cinfo)/sizeof(struct char_info);i++){
        vOutputString("svc_info : i(%d),idx(%d),char_idx(0x%x),uuid(%s),prop(%d)\r\n",i,cinfo[i].svc_idx,cinfo[i].char_idx,
                cinfo[i].uuid,cinfo[i].prop);
    }

}

BLE_CLI(gatts_get_desp)
{
    struct descrip_info dinfo[4];
    uint16_t svc_id;

    memset(dinfo,0,sizeof(dinfo));
    if(argc != 2){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    get_uint16_from_string(&argv[1],&svc_id);
    bt_gatts_get_service_desc(svc_id,dinfo,sizeof(dinfo)/sizeof(struct descrip_info));

    for(int i=0;i<sizeof(dinfo)/sizeof(struct descrip_info);i++){
        vOutputString("svc_info : i(%d),idx(%d),char_idx(0x%x),uuid(%s),desp_idx(0x%x)\r\n",i,dinfo[i].svc_idx,dinfo[i].char_idx,
            dinfo[i].uuid,dinfo[i].desp_idx);
    }
}
#endif /* CONFIG_BT_PERIPHERAL */
#endif /* BFLB_BLE_DYNAMIC_SERVICE */

static void ble_cli_task(void *pvParameters)
{
    int ret = 0;

    while(1){
         uint8_t *msg = k_fifo_get(&ble_cli_queue, K_FOREVER);
         if(msg == NULL)
             continue;

         switch(*msg){
             case CLI_MSG_ENABLE_ADV:
             {
                 ret = set_adv_enable(true);
                if(ret) {
                    vOutputString("Fail to enable adv.\r\n");
                }else{
                    vOutputString("Enable adv success.\r\n");
                }
             }
             break;
             case CLI_MSG_DISCONNECT:
             {
                 ble_disconnect_params *params = (ble_disconnect_params *)msg;
                 if(params->conn != NULL){
                     ret = bt_conn_disconnect(params->conn, params->reason);
                     if(ret) {
                         vOutputString("Disconnection fail. \r\n");
                     }else{
                         vOutputString("Disconnect success. \r\n");
                     }
                 }else{
                     vOutputString("Invalid parameters in CLI_MSG_DISCONNECT,conn=%p", params->conn);
                 }
             }
             break;
             #if defined(CONFIG_BLE_MULTI_ADV)
             case CLI_MSG_MULTI_ADV_STOP:
             {
                  if(ble_adv_id && !bt_le_multi_adv_stop(ble_adv_id))
                       ble_adv_id = 0;
             }
             break;
             #endif
             case CLI_MSG_GATT_CANCEL_PREPARE_WRITES:
             {
                ble_gatt_cancel_prepare_writes_params *gatt_cancel_params = (ble_gatt_cancel_prepare_writes_params *)msg;
                if(gatt_cancel_params->conn != NULL && gatt_cancel_params->write_params != NULL)
                {
                    ret = bt_gatt_cancle_prepare_writes(gatt_cancel_params->conn, gatt_cancel_params->write_params);
                    if(ret){
                        vOutputString("Fail to cancel prepare writes(err %d)\r\n", ret);
                    }else{
                        vOutputString("Cancel prepare writes pending\r\n");
                    }
                }else{
                    vOutputString("Invalid parameters in CLI_MSG_GATT_CANCEL_PREPARE_WRITES,conn=%p,write_params=%p", gatt_cancel_params->conn, gatt_cancel_params->write_params);
                }
             }
             break;

             default:
             break;
         }

         k_free(msg);
    }
}

int ble_cli_register(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(btStackCmdSet, sizeof(btStackCmdSet)/sizeof(btStackCmdSet[0]));
     k_fifo_init(&ble_cli_queue, BLE_CLI_QUEUE_LEN);
     if(ble_cli_queue._queue.hdl)
         k_thread_create(&ble_cli_task_h, "blecli", 1024,(k_thread_entry_t)ble_cli_task, CLI_PRIO);
    return 0;
}
