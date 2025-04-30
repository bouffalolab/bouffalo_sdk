#if defined(CONFIG_BT_STACK_PTS)
#include <stdlib.h>
#include "conn.h"
#include "gatt.h"
#include "hci_core.h"
#include "FreeRTOS.h"
#include "task.h"
#if defined(CONFIG_SHELL)
#include "shell.h"
#else
#include "cli.h"
#endif /* CONFIG_SHELL */
#include "ble_cli_cmds.h"
#include "keys.h"
#include "rpa.h"
#include "bt_log.h"

#if defined(CONFIG_SHELL)
#define PTS_CLI(func) static void pts_##func(int argc, char **argv)
#else
#define PTS_CLI(func) static void pts_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#endif

#define  LIM_ADV_TIME    	30720
#define  LIM_SCAN_TIME   	10240
#define  PASSKEY_MAX  		0xF423F
#define  NAME_LEN 			30
#define  CHAR_SIZE_MAX      512

u8_t peer_irk[16] = {
	0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
	0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef
};
			    
u8_t pts_address[6] = {
	//0xE9,0x20,0xF2,0xDC,0x1b,0x00
	//0xCA,0x97,0xDE,0x05,0xB9,0x18
	0x18,0xB9,0x05,0xDE,0x97,0xCA
};		

const u8_t discover_mode[] = {
	(BT_LE_AD_LIMITED | BT_LE_AD_NO_BREDR)
};

static const u8_t service_uuid[16] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
};

#if defined(PTS_TEST_CASE_INSUFFICIENT_KEY)
struct gatt_value {
	u16_t len;
	u8_t *data;
	u8_t enc_key_size;
	u8_t flags[1];
};

static struct gatt_value value;
#endif
static const u8_t service_data[]= {0x00,0x01};					   		
static const u8_t data_manu[4]= {0x71,0x01,0x04,0x13};
static const u8_t tx_power[1]= {0x50};
static const u8_t data_appearance[2]= {0x80, 0x01};
static const u8_t name[] = "PTS-BLE-DEV";		
extern volatile u8_t event_flag;
extern struct bt_conn *default_conn;
//extern struct bt_data ad_discov[2];

static struct bt_data ad_discov[2] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL)),
    BT_DATA(BT_DATA_NAME_COMPLETE, "PTS-BLE-DEV", 11),
};

static struct bt_data ad_non_discov[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (0)),
	BT_DATA(BT_DATA_NAME_COMPLETE, "PTS-BLE-DEV", 11),
};

static struct bt_data ad_limit_discov[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_LIMITED)),
	BT_DATA(BT_DATA_NAME_COMPLETE, "PTS-BLE-DEV", 11),
};
static u8_t selected_id = BT_ID_DEFAULT;

#define vOutputString(...)  printf(__VA_ARGS__)

PTS_CLI(ble_add_peer_irk);

PTS_CLI(ble_start_scan);
PTS_CLI(ble_start_scan_rpa);
PTS_CLI(ble_start_advertise);
PTS_CLI(ble_connect_le);
PTS_CLI(ble_disconnect);
PTS_CLI(ble_select_conn);
PTS_CLI(ble_conn_update);
PTS_CLI(ble_start_scan_timeout);
PTS_CLI(ble_add_dev_to_resolve_list);
PTS_CLI(ble_address_register);
PTS_CLI(ble_set_flag);
PTS_CLI(ble_set_smp_flag);
PTS_CLI(ble_clear_smp_flag);
PTS_CLI(ble_bondable);
PTS_CLI(ble_read_uuid);
PTS_CLI(ble_mread); 
PTS_CLI(ble_discover_uuid_128);
PTS_CLI(ble_read_uuid_128);

#if defined(PTS_TEST_CASE_INSUFFICIENT_KEY)
PTS_CLI(set_enc_key_size);
#endif
#if defined(CONFIG_BT_SMP)
PTS_CLI(ble_set_mitm);
#endif
#if defined(PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC)
PTS_CLI(ble_notify);
#endif
/*Service change*/
PTS_CLI(ble_sc_indicate);
#if defined(PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC)
PTS_CLI(ble_indicate);
#endif
PTS_CLI(ble_register_pts_svc);
#if defined(CONFIG_BT_WHITELIST)
PTS_CLI(ble_bt_le_whitelist_add);
PTS_CLI(ble_wl_connect);
#endif
PTS_CLI(ble_prepare_write);

#if defined(CONFIG_SHELL)
	SHELL_CMD_EXPORT_ALIAS(pts_ble_address_register, pts_ble_address_register, pts_ble_address_register \
		Parameter:[Address type: 0:non-rpa; 1:rpa; 2:public adderss]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_set_flag, pts_ble_set_flag, pts_ble_set_flag:[Set flag for different applications][Flag; e.g.0;1]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_set_smp_flag, pts_ble_set_smp_flag, pts_ble_set_smp_flag:[Set flag for SM test][Flag; e.g.0;1]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_clear_smp_flag, pts_ble_clear_smp_flag, pts_ble_clear_smp_flag:[Clear smp test flag][Flag; e.g.0;1]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_bondable, pts_ble_bondable, pts_ble_bondable:[Enable or disable bond] \
		Parameter:[State: e.g.0x01:bondable mode; 0x00:non-bondable mode]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_conn_update, pts_ble_conn_update, pts_ble_conn_update: \
		Parameter:[Conn Interval Min:0x0006-0C80;e.g.0030] \
		[Conn Interval Max:0x0006-0C80;e.g.0030] \
		[Conn Latency:0x0000-01f3;e.g.0004] \
		[Supervision Timeout:0x000A-0C80;e.g.0010]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_add_peer_irk, pts_ble_add_peer_irk, pts_ble_add_peer_irk:[Set peer irk] \
		Parameter:[peer irk]);
	#if defined(PTS_TEST_CASE_INSUFFICIENT_KEY)
	SHELL_CMD_EXPORT_ALIAS(pts_set_enc_key_size, pts_set_enc_key_size, pts_set_enc_key_size:[Set key size] \
		Parameter:[size,  e.g.0,1] \
		[index, e.g.0,1]);
	#endif
#if defined(CONFIG_BT_OBSERVER)
	SHELL_CMD_EXPORT_ALIAS(pts_ble_start_scan, pts_ble_start_scan, pts_ble_start_scan: \
		Parameter:[Scan type: 0:passive scan; 1:active scan] \
		[Duplicate filtering: 0:Disable duplicate filtering; 1:Enable duplicate filtering] \
		[Scan interval: 0x0004-4000;e.g.0080] \
		[Scan window: 0x0004-4000;e.g.0050] \
		[Is_RPA:0:non-rpa; 1:rpa]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_start_scan_timeout, pts_ble_start_scan_timeout, pts_ble_start_scan_timeout: \
		Parameter:[Scan type: 0:passive scan; 1:active scan] \
		[Duplicate filtering: 0:Disable duplicate filtering; 1:Enable duplicate filtering] \
		[Scan interval: 0x0004-4000;e.g.0080] \
		[Scan window: 0x0004-4000;e.g.0050] \
		[Is_RPA: 0:non-rpa; 1:rpa]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_add_dev_to_resolve_list, pts_ble_add_dev_to_resolve_list, pts_ble_add_dev_to_resolve_list: \
		Parameter:pts_ble_add_dev_to_resolve_list: \
		[Address type: 0:non-rpa; 1:rpa; 2:public adderss] \
		[Peer_address: e.g.peer_addr]);
#endif
#if defined(CONFIG_BT_PERIPHERAL)
	SHELL_CMD_EXPORT_ALIAS(pts_ble_start_advertise, pts_ble_start_adv, pts_ble_start_adv: \
		Parameter:[Adv type:0:adv_ind;1:adv_scan_ind;2:adv_nonconn_ind;3: adv_direct_ind; 4:adv_direct_ind_low_duty] \
		[Mode: 0:discov; 1:non-discov; 2:limit discoverable] \
		[Addr_type: 0:non-rpa;1:rpa;2:public] \
		[Adv Interval Min:0x0020-4000;e.g.0030] \
		[Adv Interval Max:0x0020-4000;e.g.0060]);
#if defined(PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC)
	SHELL_CMD_EXPORT_ALIAS(pts_ble_notify, pts_ble_notify, pts_ble_notify: [Lenth: e.g. 0000] [Data:  e.g. 00]);
#endif
#if defined(PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC)
	SHELL_CMD_EXPORT_ALIAS(pts_ble_indicate, pts_ble_indicate, pts_ble_indicate: [Lenth: e.g. 0000] [Data:  e.g. 00]);
#endif
	SHELL_CMD_EXPORT_ALIAS(pts_ble_register_pts_svc, pts_ble_register_pts_svc, pts_ble_register_pts_svc);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_sc_indicate, pts_ble_sc_indicate, pts_ble_sc_indicate);
#endif

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
	SHELL_CMD_EXPORT_ALIAS(pts_ble_connect_le, pts_ble_connect_le, pts_ble_connect_le:[Connect remote device] \
		Parameter:[Perr Address type:0:ADDR_RAND; 1:ADDR_RPA_OR_PUBLIC;2:ADDR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
		[Address value: e.g.112233AABBCC] \
		[Own Address type:0:ADDR_RAND; 1:ADDR_RPA_OR_PUBLIC;2:ADDR_PUBLIC; 3:ADDR_RPA_OR_RAND]);
#if defined(CONFIG_BT_WHITELIST)
	SHELL_CMD_EXPORT_ALIAS(pts_ble_wl_connect, pts_ble_wl_connect,pts_ble_wl_connect:[Autoconnect whilt list device] \
		Parameter:[Enable: 0x01:enable; 0x02:disable] \
		[Address type: 0:ADDR_PUBLIC; 1:ADDR_RAND; 2:ADDR_RPA_OR_PUBLIC; 3:ADDR_RPA_OR_RAND]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_bt_le_whitelist_add, pts_ble_bt_le_whitelist_add, pts_ble_bt_le_whitelist_add:[add device to white list] \
		Parameter:[Address type:0:ADDR_RAND; 1:ADDR_RPA_OR_PUBLIC; 2:ADDR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
		[Address value: e.g.112233AABBCC]);
#endif//CONFIG_BT_WHITELIST
#endif //CONFIG_BT_CENTRAL  
	SHELL_CMD_EXPORT_ALIAS(pts_ble_disconnect, pts_ble_disconnect, pts_ble_disconnect:[Disconnect remote device] \
		Parameter:[Address type: 0:ADDR_RAND; 1:ADDR_RPA_OR_PUBLIC;2:ADDR_PUBLIC; 3:ADDR_RPA_OR_RAND] \
		[Address value: e.g.112233AABBCC]);

#endif //CONFIG_BT_CONN
 
#if defined(CONFIG_BT_SMP)
	SHELL_CMD_EXPORT_ALIAS(pts_ble_set_mitm, pts_ble_set_mitm, pts_ble_set_mitm:[set MIMT] [State: 0x01:define;0x00:undefine]);
#endif //CONFIG_BT_SMP
#if defined(CONFIG_BT_GATT_CLIENT)
	SHELL_CMD_EXPORT_ALIAS(pts_ble_discover_uuid_128, pts_ble_discover_uuid_128, pts_ble_discover_uuid_128:[Gatt discovery] \
		Parameter:[Discovery type: 0:Primary; 1:Secondary; 2:Include; 3:Characteristic; 4:Descriptor] \
		[Uuid: 16 Octets; e.g.0000A00C000000000123456789abcdef] \
		[Start handle: 2 Octets; e.g.0001] \
		[End handle: 2 Octets; e.g.ffff]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_read_uuid_128, pts_ble_read_uuid_128, pts_ble_read_uuid_128:[Gatt Read by uuid 128 bit] \
		Parameter:[Uuid: 16 Octets] \
		[Start_handle: 2 Octets] \
		[End_handle: 2 Octets]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_read_uuid, pts_ble_read_uuid, pts_ble_read_uuid:[Gatt Read by uuid]\r\n\
		Parameter:[Uuid: 2 Octets] \
		[Start_handle: 2 Octets] \
		[End_handle: 2 Octets]);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_mread, pts_ble_mread, pts_ble_mread:[Gatt Read multiple]\r\n<handle 1> <handle 2> ...);
	SHELL_CMD_EXPORT_ALIAS(pts_ble_prepare_write, pts_ble_prepare_write, pts_ble_prepare_write:[Gatt prepare write] \
		Parameter:[Attribute handle; 2 Octets] \
		[Value offset; 2 Octets] \
		[Value length; 2 Octets] \
		[Value data]);
#endif /*CONFIG_BT_GATT_CLIENT*/
#else
const struct cli_command PtsCmdSet[] STATIC_CLI_CMD_ATTRIBUTE = {
    /*1.The cmd string to type, 2.Cmd description, 3.The function to run, 4.Number of parameters*/

    {"pts_ble_address_register", "\r\npts_ble_address_register:\r\n\[Address type, 0:non-rpa, 1:rpa, 2:public adderss]\r\n\
    [Pts_address, e.g.peer_addr]\r\n" , pts_ble_address_register}, 
    {"pts_ble_set_flag", "\r\npts_ble_set_flag:[Set flag for different applications]\r\n\[Flag, e.g.0,1]\r\n", pts_ble_set_flag},
    {"pts_ble_set_smp_flag", "\r\npts_ble_set_smp_flag:[Set flag for SM test]\r\n\[Flag, e.g.0,1]\r\n", pts_ble_set_smp_flag},
    {"pts_ble_clear_smp_flag", "\r\npts_ble_clear_smp_flag:[Clear smp test flag]\r\n\[Flag, e.g.0,1]\r\n", pts_ble_clear_smp_flag},
    {"pts_ble_bondable", "\r\npts_ble_bondable:[Enable or disable bond ]\r\n\
    [State, e.g.0x01:bondable mode, 0x00:non-bondable mode]\r\n", pts_ble_bondable},
    {"pts_ble_conn_update", "\r\npts_ble_conn_update:\r\n\
    [Conn Interval Min,0x0006-0C80,e.g.0030]\r\n\
    [Conn Interval Max,0x0006-0C80,e.g.0030]\r\n\
    [Conn Latency,0x0000-01f3,e.g.0004]\r\n\
    [Supervision Timeout,0x000A-0C80,e.g.0010]\r\n", pts_ble_conn_update},
    {"pts_ble_add_peer_irk", "\r\npts_ble_add_peer_irk:[Set peer irk]\r\n\
    [peer irk]\r\n", pts_ble_add_peer_irk},
    #if defined(PTS_TEST_CASE_INSUFFICIENT_KEY)
    {"pts_set_enc_key_size", "\r\npts_set_enc_key_size:[Set key size]\r\n\
    [size:  e.g.0 1]\r\n\
    [index: e.g.0 1]\r\n", pts_set_enc_key_size},
    #endif

#if defined(CONFIG_BT_OBSERVER)
    {"pts_ble_start_scan", "\r\npts_ble_start_scan:\r\n\
    [Scan type, 0:passive scan, 1:active scan]\r\n\
    [Duplicate filtering, 0:Disable duplicate filtering, 1:Enable duplicate filtering]\r\n\
    [Scan interval, 0x0004-4000,e.g.0080]\r\n\
    [Scan window, 0x0004-4000,e.g.0050]\r\n\
    [Is_RPA,0:non-rpa, 1:rpa]\r\n", pts_ble_start_scan},

    {"ble_start_scan_timeout", "\r\nble_start_scan_timeout:\r\n\
    [Scan type, 0:passive scan, 1:active scan]\r\n\
    [Duplicate filtering, 0:Disable duplicate filtering, 1:Enable duplicate filtering]\r\n\
    [Scan interval, 0x0004-4000,e.g.0080]\r\n\
    [Scan window, 0x0004-4000,e.g.0050]\r\n\
    [Is_RPA, 0:non-rpa, 1:rpa]\r\n",pts_ble_start_scan_timeout},

    {"pts_ble_add_dev_to_resolve_list", "\r\nble_add_dev_to_resolve_list:\r\n\
    [Address type, 0:non-rpa, 1:rpa, 2:public adderss]\r\n\
    [Peer_address, e.g.peer_addr]\r\n" , pts_ble_add_dev_to_resolve_list},
#endif

#if defined(CONFIG_BT_PERIPHERAL)
    {"pts_ble_start_adv", "\r\npts_ble_start_adv:\r\n\
    [Adv type,0:adv_ind,1:adv_scan_ind,2:adv_nonconn_ind 3: adv_direct_ind 4:adv_direct_ind_low_duty]\r\n\
    [Mode, 0:discov, 1:non-discov,2:limit discoverable]\r\n\
    [Addr_type, 0:non-rpa,1:rpa,2:public]\r\n\
    [Adv Interval Min,0x0020-4000,e.g.0030]\r\n\
    [Adv Interval Max,0x0020-4000,e.g.0060]\r\n", pts_ble_start_advertise},

#if defined(PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC)
    {"pts_ble_notify", "\r\npts_ble_notify:\r\n [Lenth, e.g. 0000]\r\n [Data,  e.g. 00]\r\n", pts_ble_notify},
#endif

#if defined(PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC)
    {"pts_ble_indicate", "\r\npts_ble_indicate:\r\n [Lenth, e.g. 0000]\r\n [Data,  e.g. 00]\r\n", pts_ble_indicate},
#endif
    {"pts_ble_register_pts_svc", "\r\npts_ble_register_pts_svc:\r\n", pts_ble_register_pts_svc},
    {"pts_ble_sc_indicate", "\r\npts_ble_sc_indicate:\r\n", pts_ble_sc_indicate},
#endif

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)

    {"pts_ble_connect_le", "\r\npts_ble_connect_le:[Connect remote device]\r\n\
    [Perr Address type,0:ADDR_RAND, 1:ADDR_RPA_OR_PUBLIC,2:ADDR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value, e.g.112233AABBCC]\r\n\
    [Own Address type,0:ADDR_RAND, 1:ADDR_RPA_OR_PUBLIC,2:ADDR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n", pts_ble_connect_le},

#if defined(CONFIG_BT_WHITELIST)
    {"pts_ble_wl_connect", "\r\npts_ble_wl_connect:[Autoconnect whilt list device]\r\n[Enable, 0x01:enable, 0x02:disable]\r\n\
    [Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n",pts_ble_wl_connect},
    {"pts_ble_bt_le_whitelist_add", "\r\npts_ble_bt_le_whitelist_add:[add device to white list]\r\n\
    [Address type,0:ADDR_RAND, 1:ADDR_RPA_OR_PUBLIC,2:ADDR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value, e.g.112233AABBCC]\r\n",pts_ble_bt_le_whitelist_add},
#endif//CONFIG_BT_WHITELIST
#endif //CONFIG_BT_CENTRAL  

    {"pts_ble_disconnect", "\r\npts_ble_disconnect:[Disconnect remote device]\r\n\
    [Address type, 0:ADDR_RAND, 1:ADDR_RPA_OR_PUBLIC,2:ADDR_PUBLIC, 3:ADDR_RPA_OR_RAND]\r\n\
    [Address value,e.g.112233AABBCC]\r\n", pts_ble_disconnect},

#endif //CONFIG_BT_CONN
 
#if defined(CONFIG_BT_SMP)
    {"pts_ble_set_mitm", "\r\npts_ble_set_mitm:[set MIMT]\r\n\[State, 0x01:define,0x00:undefine]\r\n", pts_ble_set_mitm},
#endif //CONFIG_BT_SMP

#if defined(CONFIG_BT_GATT_CLIENT)

    {"pts_ble_discover_uuid_128", "\r\npts_ble_discover_uuid_128:[Gatt discovery]\r\n\
    [Discovery type, 0:Primary, 1:Secondary, 2:Include, 3:Characteristic, 4:Descriptor]\r\n\
    [Uuid, 16 Octets, e.g.0000A00C000000000123456789abcdef]\r\n\
    [Start handle, 2 Octets, e.g.0001]\r\n\
    [End handle, 2 Octets, e.g.ffff]\r",pts_ble_discover_uuid_128},

    {"pts_ble_read_uuid_128", "\r\npts_ble_read_uuid_128:[Gatt Read by uuid 128 bit]\r\n\
    [Uuid, 16 Octets]\r\n\
    [Start_handle, 2 Octets]\r\n\
    [End_handle, 2 Octets]\r\n", pts_ble_read_uuid_128},

    {"pts_ble_read_uuid", "\r\npts_ble_read_uuid:[Gatt Read by uuid]\r\n\
    [Uuid, 2 Octets]\r\n\
    [Start_handle, 2 Octets]\r\n\
    [End_handle, 2 Octets]\r\n", pts_ble_read_uuid},

    {"pts_ble_mread", "\r\npts_ble_mread:[Gatt Read multiple]\r\n<handle 1> <handle 2> ...\r\n",pts_ble_mread},

    {"pts_ble_prepare_write", "\r\npts_ble_prepare_write:[Gatt prepare write]\r\n\
    [Attribute handle, 2 Octets]\r\n\
    [Value offset, 2 Octets]\r\n\
    [Value length, 2 Octets]\r\n\
    [Value data]\r\n", pts_ble_prepare_write},
#endif /*CONFIG_BT_GATT_CLIENT*/
};
#endif /* CONFIG_SHELL */

#if defined(CONFIG_BT_PERIPHERAL)
#if defined(PTS_GAP_SLAVER_CONFIG_READ_CHARC)
static u8_t report[]= {
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09
};

static ssize_t read_pts_long_value(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{
	const char *lvalue = "PTS-LONG-VALUE0123456789abcdef1122";

	return bt_gatt_attr_read(conn, attr, buf, len, offset, lvalue,
				 strlen(lvalue));
}

static ssize_t read_pts_name(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{
	const char *name = "PTS_NAME";

	return bt_gatt_attr_read(conn, attr, buf, len, offset, name,strlen(name));
}

static ssize_t pts_read_report(
			struct bt_conn *conn,
			const struct bt_gatt_attr *attr, void *buf,
			u16_t len, u16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, 
				attr->user_data,
				sizeof(report)/sizeof(u8_t));
}

#endif /* PTS_GAP_SLAVER_CONFIG_READ_CHARC */

#if defined(PTS_GAP_SLAVER_CONFIG_WRITE_CHARC)
#define 	TEST_LVAL_MAX    	30
#define 	TEST_SVAL_MAX   	4

static u16_t test_len = 0;
//static u16_t test_slen = 0;

static u8_t short_buf[TEST_SVAL_MAX]= 
{
	0x01,0x02,0x03,0x04				
};
static u8_t long_buf[TEST_LVAL_MAX]	=
{
	0x01,0x02,0x03,0x04,
	0x05,0x06,0x07,0x08,
	0x09,0x0a,0x11,0x12,
	0x13,0x14,0x15,0x16,
	0x17,0x18,0x19,0x1a,
	0x21,0x22,0x23,0x24,
	0x25,0x26,0x27,0x28,
	0x29,0x2a
};	

static ssize_t pts_write_short_value(struct bt_conn *conn,const struct bt_gatt_attr *bt_attr, 
					void *buf,u16_t len, u16_t offset,u8_t flags)
{
	u8_t i 			= 0;
	u16_t tlen 		= len;
	u8_t *data 		= (u8_t*)buf;

	for(i=0;i<tlen;i++)
		vOutputString("data[%d]->[0x%x]\r\n",i,data[i]);

	/*The rest of space is enough.*/
	if(TEST_SVAL_MAX >= tlen){
		(void)memcpy(short_buf,data,tlen);
	}
	else{
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	
	/*If prepare write, it will return 0 */
	if(flags == BT_GATT_WRITE_FLAG_PREPARE)
		tlen = 0;
		
	return tlen;
}

static ssize_t pts_read_value(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{	
	u8_t *data 		= short_buf;
	u16_t data_len 	= sizeof(short_buf)/sizeof(u8_t);

	if(/*test_len8*/0)
	{
		data_len = test_len;
		data = long_buf;
		test_len = 0;
	}
	
	return bt_gatt_attr_read(conn, attr, buf, len, offset, data,data_len);
}


static ssize_t pts_read_long_value(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{
	u8_t *data 		= long_buf;
	u16_t data_len 	= sizeof(long_buf)/sizeof(u8_t);

	/*Get new value */
	if(test_len > 0 && test_len <= TEST_LVAL_MAX)
	{
		data = long_buf;
		data_len = test_len;
	}
	
	return bt_gatt_attr_read(conn, attr, buf, len, offset, data,data_len);
}

static ssize_t pts_write_long_value(
				struct bt_conn *conn,
				const struct bt_gatt_attr *bt_attr, 
				void *buf,u16_t len, u16_t offset,u8_t flags)
{
	u16_t tlen 		= len;
	u8_t *data 		= (u8_t*)buf;
	u8_t i		 	= 0;

	/*Reset test value */
	if(!offset)
	{
		test_len = 0;
		(void)memset(long_buf,0,TEST_LVAL_MAX);
	}
	else if(offset > TEST_LVAL_MAX)
	{
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	/*The rest of space is enough.*/
	if(TEST_LVAL_MAX - test_len >= tlen)
	{
		(void)memcpy(&long_buf[test_len],data,tlen);
		test_len += tlen;
	}
	else
	{
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	
	/*If prepare write, it will return 0 */
	if(flags == BT_GATT_WRITE_FLAG_PREPARE)
		tlen = 0;
	
	return tlen;
}

#endif /*PTS_GAP_SLAVER_CONFIG_WRITE_CHARC*/

#if defined(PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC) || defined(PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC)
static bool notify_status 	= 0;
static u8_t battery_level 	= 100;

static void notify(const struct bt_gatt_attr *attr, u16_t value)
{
	ARG_UNUSED(attr);	
	
	notify_status = (value == BT_GATT_CCC_NOTIFY);
}

static ssize_t pts_read_value(
			struct bt_conn *conn,	
			const struct bt_gatt_attr *bt_attr, 
			void *buf,u16_t len, u16_t offset,u8_t flags)
{
	const char *data 	= "PTS_NAME";
	u16_t length 		= strlen(data);

	return bt_gatt_attr_read(conn, bt_attr, buf, len, offset, data,length);
}

static ssize_t pts_write_value(
			struct bt_conn *conn,
			const struct bt_gatt_attr *bt_attr, 
			void *buf,u16_t len, u16_t offset,u8_t flags)
{
	return len;
}

#endif /*PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC*/


static ssize_t pts_read_chara_value(
			struct bt_conn *conn,	
			const struct bt_gatt_attr *bt_attr, 
			void *buf,u16_t len, u16_t offset,u8_t flags)
{
	const char *data 	= "PTS_NAME0123456789abcde";
	u16_t length 		= strlen(data);

	return bt_gatt_attr_read(conn, bt_attr, buf, len, offset, data,length);
}
#if defined(PTS_CHARC_LEN_EQUAL_MTU_SIZE)
static ssize_t pts_read_mtu_size(
			struct bt_conn *conn,	
			const struct bt_gatt_attr *bt_attr, 
			void *buf,u16_t len, u16_t offset,u8_t flags)
{
	const char *data 	= "876543210123456789abcde";
	u16_t length 		= strlen(data);

	return bt_gatt_attr_read(conn, bt_attr, buf, len, offset, data,length);
}
#endif
#if defined(PTS_TEST_CASE_INSUFFICIENT_KEY)
static ssize_t pts_read_value_with_key(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset,u8_t flags)
{	
	const struct gatt_value *value = attr->user_data;
	
	if ((attr->perm & (BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_READ_AUTHEN)) &&
	    (value->enc_key_size > bt_conn_enc_key_size(conn))) {
		return BT_GATT_ERR(BT_ATT_ERR_ENCRYPTION_KEY_SIZE);
	}

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value->data,value->len);
}

static ssize_t pts_write_value_with_key(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset,u8_t flags)
{	
	const struct gatt_value *value = attr->user_data;
	
	if ((attr->perm & (BT_GATT_PERM_WRITE_ENCRYPT | BT_GATT_PERM_WRITE_AUTHEN )) &&
	    (value->enc_key_size > bt_conn_enc_key_size(conn))) {
		return BT_GATT_ERR(BT_ATT_ERR_ENCRYPTION_KEY_SIZE);
	}

	return len;
}

#endif

static struct bt_gatt_attr pts_attr[] = {

	BT_GATT_CHARACTERISTIC
	(		
			BT_UUID_PTS_AUTH_CHAR, 
			BT_GATT_CHRC_READ,
			BT_GATT_PERM_READ_AUTHEN, 
			pts_read_chara_value, 
			NULL, 
			NULL
	),

	#if defined(PTS_TEST_CASE_INSUFFICIENT_KEY)
	BT_GATT_CHARACTERISTIC
	(		
			BT_UUID_PTS_ENC_KEY, 
			BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			BT_GATT_PERM_READ | BT_GATT_PERM_READ_ENCRYPT | 
			BT_GATT_PERM_WRITE | BT_GATT_PERM_WRITE_ENCRYPT | BT_GATT_PERM_PREPARE_WRITE,
			pts_read_value_with_key, 
			pts_write_value_with_key, 
			&value
	),
	#endif


	#if defined(PTS_GAP_SLAVER_CONFIG_WRITE_CHARC)

	/* Case : GATT/SR/GAR/BV-03-C
	 * Verify that a Generic Attribute Profile server can support writing a Characteristic
	 * Value selected by handle
	 */	
	BT_GATT_CHARACTERISTIC
	(
			BT_UUID_PTS_CHAR_WRITE_VALUE,
	        BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
	        pts_read_value,
	        pts_write_short_value, 
	        NULL
	),


	/* Case : GATT/SR/GAR/BV-01-C
	 * Verify that a Generic Attribute Profile server can support a write to a 
	 * characteristic without response
	 */	
    BT_GATT_CHARACTERISTIC
    (
    		BT_UUID_PTS_CHAR_WRITE_NORSP,
			BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			pts_read_value, 
			pts_write_short_value,
			NULL
	),
			        
	/* Case : GATT/SR/GAR/BI-03-C, GATT/SR/GAR/BI-12-C 
	 * Verify that a Generic Attribute Profile server can detect and reject a 
	 * Write Characteristic Value Request to a non-writeable Characteristic Value 
	 * and issue a Write Not Permitted Response.
	 */			        
	BT_GATT_CHARACTERISTIC
	(
			BT_UUID_PTS_CHAR_WRITE_AUTHEN,
			BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, 
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE_AUTHEN,
			pts_read_long_value, 
			pts_write_long_value,
			NULL
	),
	
	BT_GATT_CHARACTERISTIC
	(		
			BT_UUID_PTS_CHAR_WRITE_AUTHEN,
			BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, 
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE_AUTHEN,
			pts_read_value, 
			pts_write_long_value, 
			NULL
	),

	/* Case : GATT/SR/GAW/BV-05-C */
	BT_GATT_CHARACTERISTIC
	(		
			BT_UUID_PTS_CHAR_WRITE_LONGVAL,
			BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, 
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			pts_read_long_value, 
			pts_write_long_value, 
			NULL
	),
			        
	/* Case : GATT/SR/GAW/BV-10-C */
    BT_GATT_CHARACTERISTIC
    (		
    		BT_UUID_PTS_CHAR_WRITE_2LONGVAL,
			BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, 
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			pts_read_long_value, 
			pts_write_long_value, 
			NULL
	),

	BT_GATT_DESCRIPTOR
	(		
			BT_UUID_PTS_CHAR_WRITE_L_DES, 
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			pts_read_long_value, 
			pts_write_long_value, 
			NULL
	),

#endif

	#if defined(PTS_GAP_SLAVER_CONFIG_READ_CHARC)
	
	/* Case : GATT/SR/GAR/BI-04-C
	 * Using authenticated link-key for read access. handle is 15
	 */
	BT_GATT_CHARACTERISTIC
	(
			BT_UUID_PTS_CHAR_READ_AUTHEN,
	       	BT_GATT_CHRC_READ, 
	       	BT_GATT_PERM_READ_AUTHEN,
	       	read_pts_name, 
	       	NULL, 
	       	NULL
	),

	/* Case : GATT/SR/GAR/BI-06-C
	 * Verify that a Generic Attribute Profile server can detect and reject a Read 
	 * Characteristic by UUID Request to a non-readable Characteristic Value and issue 
	 * a Read Not Permitted Response
	 */
	BT_GATT_CHARACTERISTIC
	(		
			BT_UUID_PTS_CHAR_READ_NOPERM,
			BT_GATT_CHRC_READ,
			BT_GATT_PERM_NONE,
			read_pts_name, 
			NULL, 
			NULL
	),

	/* Case : GATT/SR/GAR/BV-04-C;GATT/SR/GAR/BI-13-C
	 * Verify that a Generic Attribute Profile server can support reading a 
	 * long Characteristic Value selected by handle.
	 */
	BT_GATT_CHARACTERISTIC
	(
			BT_UUID_PTS_CHAR_READ_LONGVAL,
			BT_GATT_CHRC_READ, 
			BT_GATT_PERM_READ,
			read_pts_long_value, 
			NULL, 
			NULL
	),

	/* Case : GATT/SR/GAR/BV-06-C;GATT/SR/GAR/BV-07-C;GATT/SR/GAR/BV-08-C
	 * Verify that a Generic Attribute Profile server can support reading a Long 
	 * Characteristic Descriptor selected by handle.
	 */
	BT_GATT_DESCRIPTOR
	(
			BT_UUID_PTS_CHAR_READ_LVAL_REF, 
			BT_GATT_PERM_READ,
			pts_read_report, 
			NULL,  
			report
	),
	       
	/* Case : GATT/SR/GAR/BI-12-C
	 * Verify that a Generic Attribute Profile server can detect and reject a Read Long 
	 * Characteristic Value Request to a non-readable Characteristic Value and issue a 
	 * Read Not Permitted Response.
	 */	       
	BT_GATT_CHARACTERISTIC
	(
			BT_UUID_PTS_CHAR_READ_L_NOPERM,
			BT_GATT_CHRC_READ, 
			BT_GATT_PERM_NONE,
			read_pts_long_value, 
			NULL, 
			NULL
	),

	#endif /* PTS_GAP_SLAVER_CONFIG_READ_CHARC */

	#if defined(PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC)
	BT_GATT_CHARACTERISTIC
	(		
			BT_UUID_PTS_CHAR_NOTIFY_CHAR,
			BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			BT_GATT_PERM_READ ,
			pts_read_value, 
			NULL, 
			&battery_level
	),
						   
	BT_GATT_CCC
	(		
			notify,	
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
	),
	#endif
	
	#if defined(PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC)
	BT_GATT_CHARACTERISTIC
	(
			BT_UUID_PTS_CHAR_INDICATE_CHAR,
			BT_GATT_CHRC_READ | BT_GATT_CHRC_INDICATE,
			BT_GATT_PERM_READ ,
			pts_read_value, 
			NULL, 
			&battery_level
	),
						   
	BT_GATT_CCC
	(
			notify,	
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE
	),
	#endif
	#if defined(PTS_CHARC_LEN_EQUAL_MTU_SIZE)
	BT_GATT_CHARACTERISTIC
	(		
			BT_UUID_PTS_READ_MTU_SIZE_CHAR, 
			BT_GATT_CHRC_READ,
			BT_GATT_PERM_READ,
			pts_read_mtu_size, 
			NULL, 
			NULL
	),
	#endif

};

struct bt_gatt_service pts_svc = BT_GATT_SERVICE(pts_attr);

#if defined(PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC)
static void notify_cb(struct bt_conn *conn, void *user_data)
{
	vOutputString("%s: Nofication sent to conn %p\r\n",__func__,conn);
}

PTS_CLI(ble_notify)
{
	struct bt_gatt_notify_params 	params;
	u8_t 		data = 0;
	u16_t		len = 0;
	int			err;

	if(!default_conn){
		vOutputString("Not connected\r\n");
		return;
	}
	
	get_uint16_from_string(&argv[1], &len);
	get_bytearray_from_string(&argv[2], (u8_t *)&data,len);

	memset(&params, 0, sizeof(params));

	params.uuid = pts_attr[1].uuid;
	params.attr = &pts_attr[1];
	params.data = &data;
	params.len 	= len;
	params.func = notify_cb;

	vOutputString("len = [0x%x]\r\n",params.len);
	
	err = bt_gatt_notify_cb(default_conn, &params);
	if(err){
		vOutputString("Failed to notifition [%d]\r\n",err);
	}
}
#endif /* PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC */

#if defined(PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC)
static void indicate_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			u8_t err)
{
	if (err != 0U) {
		vOutputString("Indication fail");	
	} 
	else {
		vOutputString("Indication success");
	}
}

PTS_CLI(ble_indicate)
{
	static struct bt_gatt_indicate_params 	params;
	u8_t 		data[16];
	u16_t		len = 0;
	int			err;

	if(!default_conn){
		vOutputString("Not connected\r\n");
		return;
	}
	
	get_uint16_from_string(&argv[1], &len);
	
	if(len > 16){ 
		len = 16;
	}
	
	get_bytearray_from_string(&argv[2], data,len);

	memset(&params, 0, sizeof(params));

	params.attr = &pts_attr[1];
	params.data = data;
	params.len 	= len;
	params.func = indicate_cb;

	vOutputString("len = [0x%x] handle(0x%x)\r\n",params.len,params.attr->handle);
	
	err = bt_gatt_indicate(default_conn, &params);
	if(err){
		vOutputString("Failed to indicate [%d]\r\n",err);
	}
}
#endif /* PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC */
#endif /* CONFIG_BT_PERIPHERAL */

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

static bool ad_flag_data_cb(struct bt_data *data, void *user_data)
{
    char *ad_flag = user_data;
	
    switch(data->type){
        case BT_DATA_FLAGS:
            memcpy(ad_flag, data->data, data->data_len);
            return false;
		
        default:
            return true;
    }
}

static char *pts_cmplt_name = "PTS-GAP-224B";
static char *pts_short_name = "PTS-GAP";
extern bt_addr_le_t pts_addr;

static void pts_device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t evtype,
			 struct net_buf_simple *buf)
{
	char 		le_addr[BT_ADDR_LE_STR_LEN];
	char 		name[NAME_LEN];
	u8_t 		dst_address[6];
	char 		ad_flag[1];
	struct net_buf_simple 	abuf;
	
	(void)memset(ad_flag, 0, sizeof(ad_flag));
	(void)memset(dst_address, 0, sizeof(dst_address));
	memcpy(&abuf,buf,sizeof(struct net_buf_simple));

	(void)memset(name, 0, sizeof(name));
	bt_data_parse(buf, data_cb, name);
	bt_addr_le_to_str(addr, le_addr, sizeof(le_addr));
	
    if(!memcmp(&pts_addr, addr, sizeof(bt_addr_le_t)) ||
       !memcmp(name,pts_cmplt_name, sizeof(*pts_cmplt_name)) ||
       !memcmp(name,pts_short_name, sizeof(*pts_short_name))){
       
   		if(memcmp(&pts_addr, addr, sizeof(bt_addr_le_t)))
        	memcpy(&pts_addr, addr, sizeof(pts_addr));  
	
	vOutputString("[DEVICE]: %s, AD evt type %u, RSSI %i %s \r\n",le_addr, evtype, rssi, name);
		
	bt_data_parse(&abuf,ad_flag_data_cb,ad_flag);

	if(*ad_flag & 0x01){	
	    vOutputString("Advertising data : 'Limited Discovered Mode' flag is set one\r\n");
	}else
	    vOutputString("Advertising data : 'Limited Discovered Mode' flag is not set\r\n");

	if(*ad_flag & 0x02){
		vOutputString("Advertising data : 'General Discovered Mode' flag is set one\r\n");
    }else
     	vOutputString("Advertising data : 'General Discovered Mode' flag is not set\r\n");
	}
}

PTS_CLI(ble_start_scan)
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

    err = bt_le_scan_start(&scan_param, pts_device_found);
    
    if(err){
        vOutputString("Failed to start scan (err %d) \r\n", err);
    }else{
        vOutputString("Start scan successfully \r\n");
    }
}

PTS_CLI(ble_sc_indicate)
{
	struct bt_gatt_indicate_params 	params;
	u8_t data[16];
	u16_t len = 0;
	int err;

	if(!default_conn){
		vOutputString("Not connected\r\n");
		return;
	}

	memset(&params, 0, sizeof(params));
	params.func = NULL;	
	err = service_change_test(&params,default_conn);
	if(err){
		vOutputString("Failed to sc indicate\r\n");
		return;
	}

}

PTS_CLI(ble_start_scan_rpa)
{
    struct bt_le_scan_param scan_param;
    int err;
    (void)err;
    u8_t is_rpa;

    if(argc != 6){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }

    get_uint8_from_string(&argv[1], &scan_param.type);
    get_uint8_from_string(&argv[2], &scan_param.filter_dup);
    get_uint16_from_string(&argv[3], &scan_param.interval);
    get_uint16_from_string(&argv[4], &scan_param.window);
    get_uint8_from_string(&argv[5], (uint8_t *)&is_rpa);

    err = bt_le_pts_scan_start(&scan_param, pts_device_found, is_rpa);
    if(err){
        vOutputString("Failed to start scan (err %d) \r\n", err);
    }else{
        vOutputString("Start scan successfully \r\n");
    }
}

PTS_CLI(ble_add_peer_irk)
{
	u8_t peer_key[16];
	
	memset(peer_key,0,16);
	
	get_bytearray_from_string(&argv[1], peer_key,16);
	reverse_bytearray(peer_key, peer_irk, 16);
	vOutputString("peer_irk=[%s]\r\n",bt_hex(peer_irk, 16));
}

PTS_CLI(ble_add_dev_to_resolve_list)
{
	struct bt_keys key;
	bt_addr_le_t addr;
	u8_t type = 0;
	u8_t val[6];

	memset(&key,0,sizeof(struct bt_keys));
	
	get_uint8_from_string(&argv[1], &type);	
	get_bytearray_from_string(&argv[2], val/*(uint8_t *)addr.a.val*/,6);
	reverse_bytearray(val, addr.a.val, 6);
	if(type == 0)
		addr.type = BT_ADDR_LE_PUBLIC;
   	else if(type == 1)
		addr.type = BT_ADDR_LE_RANDOM;

	memcpy(&key.addr,&addr,sizeof(bt_addr_le_t));
	memcpy(key.irk.val,peer_irk,16);

    bt_id_add(&key);

}


#if defined(CONFIG_BT_PERIPHERAL)
PTS_CLI(ble_register_pts_svc)
{
	int	err;
	
	err = bt_gatt_service_register(&pts_svc);
	if(err){
		vOutputString("Failed to register PTS service\r\n");
	}else{
		vOutputString("Succeed to register PTS service\r\n");
	}
}

#if defined(PTS_TEST_CASE_INSUFFICIENT_KEY)
static int set_attr_enc_key_size(const struct bt_gatt_attr *attr,
				 u8_t key_size)
{
	struct gatt_value *value;

	/* Fail if requested attribute is a service */
	/*if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_PRIMARY) ||
	    !bt_uuid_cmp(attr->uuid, BT_UUID_GATT_SECONDARY) ||
	    !bt_uuid_cmp(attr->uuid, BT_UUID_GATT_INCLUDE)) {
		return -1;
	}*/

	/* Fail if permissions are not set */
	/*if (!(attr->perm & ((BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_READ_AUTHEN) |
						 (BT_GATT_PERM_WRITE_ENCRYPT | BT_GATT_PERM_WRITE_AUTHEN)))) {
		
		return -1;
	}*/

	vOutputString("perm:0x%x\r\n",attr->perm);
	vOutputString("handle:0x%04x\r\n",attr->handle);
	value = attr->user_data;
	value->enc_key_size = key_size;

	return 0;
}

PTS_CLI(set_enc_key_size)
{
	u8_t key_size;
	u8_t index;
	int err;
	
	get_uint8_from_string(&argv[1], &key_size);
	get_uint8_from_string(&argv[2], &index);
	
	if(key_size < 0x07 || key_size > 0x0f)
		vOutputString("Invalid key size(%d)\r\n",key_size);
	else{
		err = set_attr_enc_key_size(&pts_attr[index],key_size);
		if(err){
			vOutputString("Failed to set attr enc key size(%d)\r\n",err);
		}
	}
}
#endif //PTS_TEST_CASE_INSUFFICIENT_KEY
#endif
#if defined(CONFIG_BT_WHITELIST)
PTS_CLI(ble_wl_connect)
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
	/*Address type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
	get_uint8_from_string(&argv[2], &param.own_address_type);

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

PTS_CLI(ble_bt_le_whitelist_add)
{
	bt_addr_le_t waddr;
	int 		err;
	u8_t 		val[6];
	
	if(argc != 3){
		vOutputString("Number of Parameters is not correct (argc = [%d])\r\n",argc);
		return;
	}

	err = bt_le_whitelist_clear();
	if(err){
		vOutputString("Clear white list device failed (err = [%d])\r\n",err);
	}

	get_uint8_from_string(&argv[1], &waddr.type);
	get_bytearray_from_string(&argv[2], val,6);
	
	reverse_bytearray(val, waddr.a.val, 6);

	err = bt_le_whitelist_add(&waddr);
	if(err){
		vOutputString("Failed to add device to whitelist (err = [%d])\r\n",err);
	}
}

#endif 
PTS_CLI(ble_start_scan_timeout)
{
	struct bt_le_scan_param scan_param;
	uint16_t time = 0;
    int err;
	u8_t addre_type;
	
    get_uint8_from_string(&argv[1], &scan_param.type);
    get_uint8_from_string(&argv[2], &scan_param.filter_dup);
    get_uint16_from_string(&argv[3], &scan_param.interval);
    get_uint16_from_string(&argv[4], &scan_param.window);
    get_uint8_from_string(&argv[5], (uint8_t *)&addre_type);
	get_uint16_from_string(&argv[6], (uint16_t *)&time);
	
    err = bt_le_pts_scan_start(&scan_param, pts_device_found, addre_type);
    if(err){
        vOutputString("Failed to start scan (err %d) \r\n", err);
    }else{
        vOutputString("Start scan successfully \r\n");
    }

	k_sleep(time);
	bt_le_scan_stop();
	vOutputString("Scan stop \r\n");
}

PTS_CLI(ble_address_register)
{
	bt_addr_le_t addr;
	u8_t type;
	char le_addr[BT_ADDR_LE_STR_LEN];
	u8_t  addr_val[6];
	get_uint8_from_string(&argv[1], &type);
	get_bytearray_from_string(&argv[2], addr_val,6);

	if(type == 0){
		addr.type = BT_ADDR_LE_PUBLIC;
	}else if(type == 1)
		addr.type = BT_ADDR_LE_RANDOM;

	reverse_bytearray(addr_val, addr.a.val, 6);
	memcpy(&pts_addr,&addr,sizeof(bt_addr_le_t));

	bt_addr_le_to_str(&pts_addr, le_addr, sizeof(le_addr));
	
	memcpy(pts_address,pts_addr.a.val,6);

	vOutputString("Pts address %s \r\n",le_addr);	
}

PTS_CLI(ble_set_flag)
{
	u8_t flag;
	get_uint8_from_string(&argv[1],&flag);
	event_flag	= flag;
	
	vOutputString("Event flag = [0x%x] \r\n",event_flag);
}

PTS_CLI(ble_set_smp_flag)
{
	u8_t flag;
	
	get_uint8_from_string(&argv[1],&flag);
	bt_set_smpflag((smp_test_id)flag);

	vOutputString("Smp flag = [0x%x] \r\n",flag);
}

PTS_CLI(ble_clear_smp_flag)
{
	u8_t flag;
	
	get_uint8_from_string(&argv[1],&flag);
	bt_clear_smpflag((smp_test_id)flag);

	vOutputString("Clear smp flag \r\n");
}

PTS_CLI(ble_bondable)
{
	u8_t bondable;

	get_uint8_from_string(&argv[1], &bondable);

	if(bondable == 0x01)
		bt_set_bondable(true);
	else if(bondable == 0x00)
		bt_set_bondable(false);
	else
		vOutputString("Bondable status is unknow \r\n");

}
#endif //#if defined(CONFIG_BT_OBSERVER)


#if defined(CONFIG_BT_PERIPHERAL)
PTS_CLI(ble_start_advertise)
{
    struct bt_le_adv_param param;
	const struct bt_data *ad;
	size_t ad_len=0;
	int err = 0;
    uint8_t adv_type, tmp;
    struct bt_data pts_ad ;
	bt_addr_le_t pts; 
	struct bt_conn *conn;
	u8_t adder_type = 0;
	u8_t is_ad = 0;
	
	memset(pts.a.val,0,6);
	memset(&pts_ad,0,sizeof(struct bt_data));

    if(argc != 4 && argc != 6){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
	switch(event_flag){
		case ad_type_service_uuid:
			pts_ad.type = BT_DATA_UUID128_ALL;
			pts_ad.data = service_uuid;
			pts_ad.data_len = sizeof(service_uuid);
			is_ad = 1;
		break;
		
		case ad_type_local_name:
			pts_ad.type = BT_DATA_NAME_COMPLETE;
			pts_ad.data = name;
			pts_ad.data_len = 13;
			is_ad = 1;
		break;

		case ad_type_flags:
			pts_ad.type = BT_DATA_FLAGS;
			pts_ad.data = discover_mode;
			pts_ad.data_len = sizeof(discover_mode);
			is_ad = 1;
		break;

        case ad_type_manu_data:
			pts_ad.type = BT_DATA_MANUFACTURER_DATA;
			pts_ad.data = data_manu;
			pts_ad.data_len = sizeof(data_manu);
			is_ad = 1;
		break;

		case ad_type_tx_power_level:
			pts_ad.type = BT_DATA_TX_POWER;
			pts_ad.data = tx_power;
			pts_ad.data_len = sizeof(tx_power);
			is_ad = 1;
		break;

        case ad_type_service_data:
			pts_ad.type = BT_DATA_SVC_DATA16;
			pts_ad.data = service_data;
			pts_ad.data_len = sizeof(service_data);
			is_ad = 1;
		break;

        case ad_type_appearance:
			pts_ad.type = BT_DATA_GAP_APPEARANCE;
			pts_ad.data = data_appearance;
			pts_ad.data_len = sizeof(data_appearance);
			is_ad = 1;
		break;
	
		default:
			break;
	}
        
    param.id = selected_id;
    param.interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
    param.interval_max = BT_GAP_ADV_FAST_INT_MAX_2;

    /*Get adv type, 0:adv_ind,  1:adv_scan_ind, 2:adv_nonconn_ind 3: adv_direct_ind*/
    get_uint8_from_string(&argv[1], &adv_type);
    
    if(adv_type == 0){
        param.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME);
    }else if(adv_type == 1){
        param.options = BT_LE_ADV_OPT_USE_NAME;
    }else if(adv_type == 2){
        param.options = 0;
	}else if(adv_type == 3){
		param.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME);
    }else if(adv_type == 4){
		param.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY | BT_LE_ADV_OPT_ONE_TIME);
    }
    else{
        vOutputString("Arg1 is invalid\r\n");
        return;
    }

    ad = ad_discov;
    ad_len = ARRAY_SIZE(ad_discov);

    /*Get mode, 0:General discoverable,  1:non discoverable, 2:limit discoverable*/
    get_uint8_from_string(&argv[2], &tmp);

    if(tmp == 0 || tmp == 1 || tmp == 2){
        if(tmp ==0)
        {
	        ad = ad_discov;
	        ad_len = ARRAY_SIZE(ad_discov);
        }
        else if(tmp == 1){
	        ad = ad_non_discov;
	        ad_len = ARRAY_SIZE(ad_discov);
        }
        else if(tmp == 2){
            ad = ad_limit_discov;
            ad_len = ARRAY_SIZE(ad_limit_discov);
        }
        if(is_ad == 1){
	        ad = &pts_ad;
	        ad_len = 1;	
        }/*else{
	        ad = ad_discov;
	        ad_len = ARRAY_SIZE(ad_discov);
        }*/
		
    }else{
        vOutputString("Arg2 is invalid\r\n");
        return;
    }

    /*upper address type, 0:non-resolvable private address,1:resolvable private address,2:public address*/
    //get_bytearray_from_string(&argv[3], (u8_t *)&param.addr_type, 1);
    get_uint8_from_string(&argv[3], (u8_t *)&adder_type);

	if(adder_type == 0)
		param.addr_type = BT_ADDR_TYPE_NON_RPA;
	else if(adder_type == 1)
		param.addr_type = BT_ADDR_TYPE_RPA;
	else if(adder_type == 2)
		param.addr_type = BT_ADDR_LE_PUBLIC;
	else
		vOutputString("Invaild address type\r\n");
	
    if(argc == 6){
        get_uint16_from_string(&argv[4], &param.interval_min);
        get_uint16_from_string(&argv[5], &param.interval_max);
    }  

	if(adv_type == 3){	
		param.interval_min = 0;
		param.interval_max = 0;
	}
	
    if(adv_type == 1){
        err = bt_le_adv_start(&param, ad, ad_len, &ad_discov[0], 1);	
    }else if(adv_type == 3 || adv_type == 4){
    	pts.type = BT_ADDR_LE_PUBLIC;
		memcpy(pts.a.val,pts_address,6);   
		conn = bt_conn_create_slave_le(&pts,&param);
		if(!conn){
        	err = 1; 
		}
	    else{
	        bt_conn_unref(conn);
	    }

    }else{
        err = bt_le_adv_start(&param, ad, ad_len, NULL, 0);
    }
 
    if(err){
        vOutputString("Failed to start advertising\r\n");
    }else{
        vOutputString("Advertising started\r\n");
    }

	if(tmp == 2){	
		k_sleep(LIM_ADV_TIME);
		bt_le_adv_stop();
		vOutputString("Adv timeout \r\n");
	}

}

#endif //#if defined(CONFIG_BT_PERIPHERAL)

#if defined(CONFIG_BT_CONN)
#if defined(CONFIG_BT_CENTRAL)
PTS_CLI(ble_connect_le)
{
    bt_addr_le_t addr;
    struct bt_conn *conn;
    u8_t  addr_val[6];
	s8_t  type = -1;

	struct bt_le_conn_param param = {
		.interval_min =  BT_GAP_INIT_CONN_INT_MIN,
		.interval_max =  BT_GAP_INIT_CONN_INT_MAX,
		.latency = 0,
		.timeout = 400,
		.own_address_type = BT_ADDR_LE_PUBLIC_ID,

	};

    if(argc != 4){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
	if(event_flag == own_addr_type_random){
		param.own_address_type = BT_ADDR_LE_RANDOM_ID;
	}
       
    get_uint8_from_string(&argv[1], (u8_t *)&type);

	/*Get addr type,0:ADDR_RAND, 1:ADDR_RPA_OR_PUBLIC,2:ADDR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
	if(type == 0)
		addr.type = 1; /*ADDR_RAND*/
	else if(type == 1)
		addr.type = 2; /*ADDR_RPA_OR_PUBLIC*/
	else if(type == 2)
		addr.type = 0; /*ADDR_PUBLIC*/
	else if(type == 3)
		addr.type = 3; /*ADDR_RPA_OR_RAND*/
	else 
		vOutputString("adderss type is unknow [0x%x]\r\n",type);

    get_bytearray_from_string(&argv[2], addr_val,6);

    reverse_bytearray(addr_val, addr.a.val, 6);

    get_uint8_from_string(&argv[3], (u8_t *)&type);

    if(type == 0)
		param.own_address_type = 1; /*ADDR_RAND*/
	else if(type == 1)
		param.own_address_type = 2; /*ADDR_RPA_OR_PUBLIC*/
	else if(type == 2)
		param.own_address_type = 0; /*ADDR_PUBLIC*/
	else if(type == 3)
		param.own_address_type = 3; /*ADDR_RPA_OR_RAND*/
	else 
		vOutputString("adderss type is unknow [0x%x]\r\n",type);
    
    conn = bt_conn_create_le(&addr, /*BT_LE_CONN_PARAM_DEFAULT*/&param);

    if(!conn){
        vOutputString("Connection failed\r\n");
    }else{
        vOutputString("Connection pending\r\n");
    }
}

#endif //#if defined(CONFIG_BT_CENTRAL)

PTS_CLI(ble_disconnect)
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

	/*Get addr type,0:ADDR_RAND, 1:ADDR_RPA_OR_PUBLIC,2:ADDR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
	if(type == 0)
		addr.type = 1; /*ADDR_RAND*/
	else if(type == 1)
		addr.type = 2; /*ADDR_RPA_OR_PUBLIC*/
	else if(type == 2)
		addr.type = 0; /*ADDR_PUBLIC*/
	else if(type == 3)
		addr.type = 3; /*ADDR_RPA_OR_RAND*/
	else 
		vOutputString("adderss type is unknow\r\n");

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
    bt_conn_unref(conn);
}

PTS_CLI(ble_conn_update)
{
	struct bt_le_conn_param param;
	int err;

    if(argc != 5){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
	if(default_conn == NULL){
		vOutputString("Connection is NULL\r\n");
        return;
	}
	memset(&param,0,sizeof(struct bt_le_conn_param));
    get_uint16_from_string(&argv[1], &param.interval_min);
    get_uint16_from_string(&argv[2], &param.interval_max);
    get_uint16_from_string(&argv[3], &param.latency);
    get_uint16_from_string(&argv[4], &param.timeout);	

	if(event_flag == dir_connect_req)
		err = bt_conn_le_param_update(default_conn, &param);
	else
		err = pts_bt_conn_le_param_update(default_conn, &param);

	if (err) {
		vOutputString("conn update failed (err %d)\r\n", err);
	} else {
		vOutputString("conn update initiated\r\n");
	}
}
#endif //#if defined(CONFIG_BT_CONN)

#if defined(CONFIG_BT_SMP)
PTS_CLI(ble_set_mitm)
{
	u8_t enable = 0;

	get_uint8_from_string(&argv[1],&enable);

	if(enable == 0x01)
		bt_set_mitm(true);
	else if(enable == 0x00)
		bt_set_mitm(false);
	else
		vOutputString("Inviad parameter\r\n");
}

#endif //#if defined(CONFIG_BT_SMP)

#if defined(CONFIG_BT_GATT_CLIENT)
static struct bt_gatt_discover_params discover_params;
static struct bt_uuid_16 uuid = BT_UUID_INIT_16(0);
static struct bt_uuid_128 uuid_128 = BT_UUID_INIT_128(0);

extern u8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params);
PTS_CLI(ble_discover_uuid_128)
{
	int err;
    u8_t disc_type;
	u8_t val[16];

	(void)memset(val,0x0,16);
	
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
	get_bytearray_from_string(&argv[2], val,16);

	reverse_bytearray(val, uuid_128.val,16);
		
	/*Set array value to 0 */
	(void)memset(val, 0x0, 16);
	
	if(!memcmp(uuid_128.val, val, 16))	
		discover_params.uuid = NULL;
	else
    	discover_params.uuid = &uuid_128.uuid;
       
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

	memset(str,0,15);
	
	if(length > 0 && length <= sizeof(str)){
		memcpy(str,buf,length);
		vOutputString("device name : %s \r\n",str);
		
		u8_t i = 0;
		for(i=0;i<length;i++)
			vOutputString("data[%d] = [0x%x]\r\n", i, buf[i]);	
	}

	if (!data) {
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_CONTINUE;
}

PTS_CLI(ble_read_uuid_128)
{
	int err;
	u8_t val[16];
	struct bt_uuid_128 uuid = BT_UUID_INIT_128(0);

	(void)memset(val, 0, 16);
	
    if(argc != 4){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}
	
    read_params.func = read_func;
	read_params.handle_count = 0;
	read_params.by_uuid.start_handle = 0x0001;
	read_params.by_uuid.end_handle = 0xffff;
	
	get_bytearray_from_string(&argv[1], val,16);
	reverse_bytearray(val, uuid.val, 16);
	//(void)memcpy(uuid.val, val, 16)

	(void)memset(val, 0, 16);
	
	if(!memcmp(uuid.val, val, 16))
		read_params.by_uuid.uuid = NULL;
	else
		read_params.by_uuid.uuid = &uuid.uuid;
	
	get_uint16_from_string(&argv[2], &read_params.by_uuid.start_handle);
	get_uint16_from_string(&argv[3], &read_params.by_uuid.end_handle);
	
	err = bt_gatt_read(default_conn, &read_params);
	if (err) {
		vOutputString("Read failed (err %d)\r\n", err);
	} else {
		vOutputString("Read pending\r\n");
	}

}

PTS_CLI(ble_read_uuid)
{
	int err;
	struct bt_uuid_16 uuid = BT_UUID_INIT_16(0);
	
    if(argc != 4){
        vOutputString("Number of Parameters is not correct\r\n");
        return;
    }
    
	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}
	
    read_params.func = read_func;
	read_params.handle_count = 0;
	read_params.by_uuid.start_handle = 0x0001;
	read_params.by_uuid.end_handle = 0xffff;
	
	get_uint16_from_string(&argv[1], &uuid.val);
	if(uuid.val)
		read_params.by_uuid.uuid = &uuid.uuid;
	
	get_uint16_from_string(&argv[2], &read_params.by_uuid.start_handle);
	get_uint16_from_string(&argv[3], &read_params.by_uuid.end_handle);
	
	err = bt_gatt_read(default_conn, &read_params);
	if (err) {
		vOutputString("Read failed (err %d)\r\n", err);
	} else {
		vOutputString("Read pending\r\n");
	}

}

PTS_CLI(ble_mread)
{
	u16_t h[8];
	int i, err;

	if (!default_conn) {
		vOutputString("Not connected\r\n");
		return;
	}

	if((argc-1) > ARRAY_SIZE(h)){	
		vOutputString("Enter max %lu handle items to read\r\n",ARRAY_SIZE(h));
		return;
	}

	for (i = 0; i < argc - 1; i++) {
		get_uint16_from_string(&argv[i + 1],&h[i]);
	}	

	read_params.func = read_func;
	read_params.handle_count = i;
	read_params.handles = h; /* not used in read func */

	vOutputString("i = [%d]\r\n",i);
	
	err = bt_gatt_read(default_conn, &read_params);
	if (err) {
		vOutputString("Read failed (err %d)\r\n", err);
	} else {
		vOutputString("Read pending\r\n");
	}
}


static struct bt_gatt_write_params write_params;
static u8_t gatt_write_buf[CHAR_SIZE_MAX];

static void write_func(struct bt_conn *conn, u8_t err,
		       struct bt_gatt_write_params *params)
{
	vOutputString("Write complete: err %u \r\n", err);

	(void)memset(&write_params, 0, sizeof(write_params));
}

PTS_CLI(ble_prepare_write)
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
    data_len = write_params.length > sizeof(gatt_write_buf)? (sizeof(gatt_write_buf)):(write_params.length);
    get_bytearray_from_string(&argv[4], gatt_write_buf,data_len);
    
	write_params.data = gatt_write_buf;
	write_params.length = data_len;
	write_params.func = write_func;
	
 	err = bt_gatt_prepare_write(default_conn, &write_params);	

	if (err) {
		vOutputString("Prepare write failed (err %d)\r\n", err);
	} else {
		vOutputString("Prepare write pending\r\n");
	}

}

//static struct bt_gatt_subscribe_params subscribe_params;

#endif /* CONFIG_BT_GATT_CLIENT */


int pts_cli_register(void)
{
    //memcpy(&pts_addr.a, BT_ADDR_NONE, sizeof(pts_addr.a));

    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(btStackCmdSet, sizeof(btStackCmdSet)/sizeof(btStackCmdSet[0]));
    return 0;
}

#endif
