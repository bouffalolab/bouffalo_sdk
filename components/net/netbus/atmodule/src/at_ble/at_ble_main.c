/**
  ******************************************************************************
  * @file    at_ble_main.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#if defined(BFLB_undef) || defined(BFLB_undef)
#include "ble_lib_api.h"
#else
#include "btble_lib_api.h"
#endif
#include "l2cap_internal.h"
#if defined(CONFIG_BLE_MULTI_ADV)
#include "multi_adv.h"
#endif
#include "bluetooth.h"
#include "hci_driver.h"
#include "ble_tp_svc.h"
#include "rfparam_adapter.h"

#include "at_main.h"
#include "at_core.h"
#include "at_port.h"
#include "at_base_config.h"
#include "at_ble_config.h"
#include "at_ble_main.h"

#if defined(CONFIG_DYNAMIC_GATTS)
#include "dynamic_gatts.h"
#include "bt_uuid.h"
#endif

#if defined(CONFIG_BT_SMP)
#include "smp.h"
#include "keys.h"
#include "bt_log.h"
#endif

#include <bt_errno.h>

#define AT_BLE_PRINTF              printf

#define CHECK_BLE_SRV_IDX_VALID(idx) \
    if (!at_ble_srv_idx_is_valid(idx)) { \
        AT_BLE_PRINTF("service idx is invalid\r\n"); \
        return 0; \
    }

#define CHECK_BLE_CHAR_IDX_VALID(idx) \
    if (!at_ble_char_idx_is_valid(idx)) { \
        AT_BLE_PRINTF("characteristic idx is invalid\r\n"); \
        return 0; \
    }


static int g_ble_is_inited = 0;
static int g_ble_dynamic_init = 0;

static ble_work_role g_ble_role = BLE_DISABLE;
static uint8_t g_ble_public_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static ble_scan_callback_t g_ble_scan_callback = NULL;
static uint8_t g_ble_selected_id = BT_ID_DEFAULT;
#if defined(CONFIG_BLE_MULTI_ADV)
static int g_ble_adv_id;
#endif
static int g_ble_tx_power = -255;

#define BLE_CONN_STATE_UNUSED       0
#define BLE_CONN_STATE_CONNCTING    1
#define BLE_CONN_STATE_CONNECTED    2
#define BLE_CONN_STATE_DISCONNECTED 3

struct ble_conn_data
{
    uint8_t role;
    uint8_t valid;
    uint8_t idx;
    uint8_t addr[6];
    uint8_t addr_type;
    struct bt_conn *conn;
    uint8_t state;
    uint16_t min_interval;
    uint16_t max_interval;
    uint16_t cur_interval;
    uint16_t latency;
    uint16_t timeout;
    uint16_t tx_octets;
    uint16_t tx_time;
    uint16_t rx_octets;
    uint16_t rx_time;
    uint8_t indicate_finish;
};

static struct ble_conn_data g_ble_conn_data[BLE_CONN_MAX_NUM];

struct ble_char_data
{
    uint8_t valid;
    struct bt_uuid_128 char_uuid;
    struct bt_uuid_16 char_uuid_16;
    uint32_t char_prop;
    uint32_t char_perm;
    uint8_t uuid_type;
    char read_data[22];
    int read_data_len;
    struct bt_gatt_attr *attr;
};

struct ble_srv_data
{
    uint8_t valid;
    struct bt_uuid_128 srv_uuid;
    struct bt_uuid_16 srv_uuid_16;
    uint8_t srv_type;
    uint8_t uuid_type;
    struct ble_char_data srv_char[BLE_CHAR_MAX_NUM];
};

static struct ble_srv_data *g_ble_srv_data = NULL;

static void ble_gatts_srv_clean(void)
{
    if (g_ble_srv_data != NULL) {
        memset(g_ble_srv_data, 0, sizeof(struct ble_srv_data) * BLE_SRV_MAX_NUM);
        at_free(g_ble_srv_data);
        g_ble_srv_data = NULL;
    }

}

static struct ble_conn_data *ble_conn_data_get_by_idx(int idx)
{
    if (idx < 0 || idx >= BLE_CONN_MAX_NUM)
        return NULL;

    if (!g_ble_conn_data[idx].valid)
        return NULL;

    return &g_ble_conn_data[idx];
}

static struct ble_conn_data *ble_conn_data_get_by_conn(struct bt_conn *conn)
{
    int i;

    if (conn == NULL)
        return NULL;

    for (i = 0; i < BLE_CONN_MAX_NUM; i++) {
        if (g_ble_conn_data[i].valid && g_ble_conn_data[i].conn == conn) {
            return &g_ble_conn_data[i];
        }
    }

    return NULL;
}

static void ble_conn_data_set(int idx, u8_t role, uint8_t *addr, uint8_t addr_type, struct bt_conn *conn, uint16_t min_interval, uint16_t max_interval, uint16_t cur_interval, uint8_t state)
{
    if (idx < 0 || idx >= BLE_CONN_MAX_NUM)
        return;

    g_ble_conn_data[idx].idx = idx;
    memcpy(g_ble_conn_data[idx].addr, addr, 6);
    g_ble_conn_data[idx].role = role;
    g_ble_conn_data[idx].addr_type = addr_type;
    g_ble_conn_data[idx].conn = conn;
    g_ble_conn_data[idx].state = state;
    g_ble_conn_data[idx].min_interval = min_interval;
    g_ble_conn_data[idx].max_interval = max_interval;
    g_ble_conn_data[idx].cur_interval = cur_interval;
    g_ble_conn_data[idx].latency = 0;
    g_ble_conn_data[idx].timeout = 0;
    g_ble_conn_data[idx].valid = 1;
    g_ble_conn_data[idx].indicate_finish = 0;
}

static void ble_addr_trans(uint8_t *src, uint8_t *dst)
{
    int i;

    for (i = 0; i < 6; i++) {
        dst[i] = src[5 - i];
    }
}

static void ble_uuid_trans(uint8_t *src, uint8_t *dst)
{
    int i;

    for (i = 0; i < 16; i++) {
        dst[i] = src[15 - i];
    }
}

static void ble_uuid_16_trans_get(uint16_t src, uint8_t *dst)
{
    dst[0] = (uint8_t)(src >> 8);
    dst[1] = (uint8_t)(src & 0xFF);
}

static void ble_uuid_16_trans_set(uint8_t *src, uint16_t *dst)
{
    *dst = (uint16_t)(src[0] << 8) | src[1]; 
}

static int check_attr_ismatch(struct ble_char_data *srv_char, struct bt_gatt_attr *attr)
{

    if(srv_char->uuid_type == BT_UUID_TYPE_128)
    {
        if(ble_dynamic_gatt_get_attr(&srv_char->char_uuid.uuid)==(attr))
        {
            return 1;
        }

        if(ble_dynamic_gatt_get_attr(&srv_char->char_uuid.uuid)==(attr-1))
        {
            return 1;
        }
    }
    if(srv_char->uuid_type == BT_UUID_TYPE_16)
    {
        if(ble_dynamic_gatt_get_attr(&srv_char->char_uuid_16.uuid)==(attr))
        {
            return 1;
        }
        if(ble_dynamic_gatt_get_attr(&srv_char->char_uuid_16.uuid)==(attr-1))
        {
            return 1;
        }
    }
    return 0;
}

static int find_index_by_attr(const struct bt_gatt_attr *attr, int *srv_idx, int *char_idx)
{
    int i, j;

    for (i = 0; i < BLE_SRV_MAX_NUM; i++) {
        if (g_ble_srv_data[i].valid) {
            for (j = 0; j < BLE_CHAR_MAX_NUM; j++) {
                if (g_ble_srv_data[i].srv_char[j].valid && check_attr_ismatch(&g_ble_srv_data[i].srv_char[j], (struct bt_gatt_attr *)attr)) {
                    AT_BLE_PRINTF("find attr = %d %d\r\n", i,j);
                    *srv_idx = i;
                    *char_idx = j;
                    return 1;
                }
            }
        }
    }

    return 0;
}

static void ble_add_service(void)
{

    for (int i = 0; i < BLE_SRV_MAX_NUM; i++) 
    {
        if (g_ble_srv_data[i].valid) 
        {
            if(g_ble_srv_data[i].srv_type)
            {
                if(g_ble_srv_data[i].uuid_type == BT_UUID_TYPE_128)
                    ble_dynamic_gatt_add_service(&g_ble_srv_data[i].srv_uuid.uuid,BT_UUID_TYPE_128,GATT_SERVICE_PRIMARY);
                if(g_ble_srv_data[i].uuid_type == BT_UUID_TYPE_16)
                    ble_dynamic_gatt_add_service(&g_ble_srv_data[i].srv_uuid_16.uuid,BT_UUID_TYPE_16,GATT_SERVICE_PRIMARY);

            }
            else
            {
                if(g_ble_srv_data[i].uuid_type == BT_UUID_TYPE_128)
                    ble_dynamic_gatt_add_service(&g_ble_srv_data[i].srv_uuid.uuid,BT_UUID_TYPE_128,GATT_SERVICE_SECONDARY);
                if(g_ble_srv_data[i].uuid_type == BT_UUID_TYPE_16)
                    ble_dynamic_gatt_add_service(&g_ble_srv_data[i].srv_uuid_16.uuid,BT_UUID_TYPE_16,GATT_SERVICE_SECONDARY);

            }
            for (int n = 0; n < BLE_CHAR_MAX_NUM; n++) 
            {
                if(g_ble_srv_data[i].srv_char[n].valid)
                {
                    struct gatt_add_characteristic cmd_data;
                    cmd_data.char_id = 0U;
                    cmd_data.properties  = g_ble_srv_data[i].srv_char[n].char_prop;
                    cmd_data.permissions = g_ble_srv_data[i].srv_char[n].char_perm;
                    if(g_ble_srv_data[i].srv_char[n].uuid_type == BT_UUID_TYPE_128)
                        cmd_data.uuid = &g_ble_srv_data[i].srv_char[n].char_uuid.uuid;
                    if(g_ble_srv_data[i].srv_char[n].uuid_type == BT_UUID_TYPE_16)
                        cmd_data.uuid = &g_ble_srv_data[i].srv_char[n].char_uuid_16.uuid;
                    ble_dynamic_gatt_add_characteristic(&cmd_data);

                    if(g_ble_srv_data[i].srv_char[n].char_prop&(BLE_GATT_CHAR_PROP_INDICATE|BLE_GATT_CHAR_PROP_NOTIFY))
                    {
                        struct gatt_add_descriptor desc_cmd;
                        desc_cmd.desc_id=0U;
                        desc_cmd.permissions= BT_GATT_PERM_READ | BT_GATT_PERM_WRITE;
                        desc_cmd.uuid=BT_UUID_GATT_CCC;
                        ble_dynamic_gatt_add_descriptor(&desc_cmd);
                    }
                }
            }
            ble_dynamic_gatt_start_server();
        }
    }

}

static void ble_connected(struct bt_conn *conn, u8_t err)
{
    if(err || conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }
    AT_BLE_PRINTF("%s conn: 0x%x\r\n",__func__, conn);
    if (conn->role == BT_CONN_ROLE_MASTER) {
        struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
        if (!conn_data)
            return;
        conn_data->state = BLE_CONN_STATE_CONNECTED;
        
    } else if (conn->role == BT_CONN_ROLE_SLAVE) {
        bt_addr_le_t* remote_addr = NULL;
        remote_addr = bt_conn_get_dst(conn);
        for(int i = 0; i < BLE_CONN_MAX_NUM; i++)
        {
            if(!at_ble_is_connected(i))
            {
                ble_conn_data_set(i, BT_CONN_ROLE_SLAVE,remote_addr->a.val, remote_addr->type, conn, BT_GAP_INIT_CONN_INT_MIN, BT_GAP_INIT_CONN_INT_MIN,BT_GAP_INIT_CONN_INT_MIN, BLE_CONN_STATE_CONNECTED);
                break;
            }
               
        }
        struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
        if (!conn_data)
            return;
        at_response_string("+BLE:CONNECTED:%d,%d,\"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n",
                        conn_data->idx,
                        BT_CONN_ROLE_SLAVE,
                        remote_addr->a.val[5],
                        remote_addr->a.val[4],
                        remote_addr->a.val[3],
                        remote_addr->a.val[2],
                        remote_addr->a.val[1],
                        remote_addr->a.val[0]);
        
    }
}

static void ble_disconnected(struct bt_conn *conn, u8_t reason)
{ 
    if(conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }
    
    AT_BLE_PRINTF("%s conn: 0x%x, reason: %d\r\n",__func__, conn, reason);

    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (!conn_data)
        return;

    at_response_string("+BLE:DISCONNECTED:%d,%d,\"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n",
            conn_data->idx,
            conn_data->role,
            conn_data->addr[5],
            conn_data->addr[4],
            conn_data->addr[3],
            conn_data->addr[2],
            conn_data->addr[1],
            conn_data->addr[0]);

    conn_data->state = BLE_CONN_STATE_DISCONNECTED;
    conn_data->conn  = NULL;
    conn_data->valid = 0;
    memset(&conn_data->addr,0,sizeof(conn_data->addr));
    conn_data->indicate_finish = 0;
    
}

static void ble_le_param_updated(struct bt_conn *conn, u16_t interval, u16_t latency, u16_t timeout)
{
    AT_BLE_PRINTF("LE conn param updated: int 0x%04x lat %d to %d \r\n", interval, latency, timeout);

    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (!conn_data)
        return;

    conn_data->cur_interval = interval;
    conn_data->latency = latency;
    conn_data->timeout = timeout;
    at_response_string("+BLE:CONNPARAM:%d,%d,%d,%d,%d,%d\r\n",
            conn_data->idx,
            conn_data->min_interval,
            conn_data->max_interval,
            conn_data->cur_interval,
            conn_data->latency,
            conn_data->timeout);
}

static void ble_datalen_updated(struct bt_conn *conn, u16_t tx_octets, u16_t tx_time, u16_t rx_octets,u16_t rx_time)
{
    AT_BLE_PRINTF("LE datalen updated: %d %d %d %d\r\n", tx_octets, tx_time, rx_octets,rx_time);

    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (!conn_data)
        return;

    conn_data->tx_octets = tx_octets;
    conn_data->tx_time = tx_time;
    conn_data->rx_octets = rx_octets;
    conn_data->rx_time = rx_time;
    at_response_string("+BLE:DATALENUPDATED:%d,%d,%d,%d,%d\r\n",
            conn_data->idx,
            conn_data->tx_octets,
            conn_data->tx_time,
            conn_data->rx_octets,
            conn_data->rx_time);
}
#if defined(CONFIG_BT_SMP)
static void ble_identity_resolved(struct bt_conn *conn, const bt_addr_le_t *rpa,
			      const bt_addr_le_t *identity)
{
	char addr_identity[BT_ADDR_LE_STR_LEN];
	char addr_rpa[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(identity, addr_identity, sizeof(addr_identity));
	bt_addr_le_to_str(rpa, addr_rpa, sizeof(addr_rpa));

	AT_BLE_PRINTF("Identity resolved %s -> %s \r\n", addr_rpa, addr_identity);
}

static void ble_security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	AT_BLE_PRINTF("Security changed: %s level %u \r\n", addr, level);
}
#endif

static struct bt_conn_cb ble_conn_callbacks = {
	.connected	=   ble_connected,
	.disconnected	=   ble_disconnected,
	.le_param_updated = ble_le_param_updated,
#if defined(CONFIG_BT_SMP)
	.identity_resolved = ble_identity_resolved,
	.security_changed = ble_security_changed,
#endif
#if defined(CONFIG_USER_DATA_LEN_UPDATE)
    .le_datalen_updated = ble_datalen_updated,
#endif
};

static void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        AT_BLE_PRINTF("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
               bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);
        AT_BLE_PRINTF("SUPPORT %d CONN\r\n",BLE_CONN_MAX_NUM);
        bt_conn_cb_register(&ble_conn_callbacks);
    }
}

static void ble_notification_all_cb(struct bt_conn *conn, u16_t handle,const void *data, u16_t length)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (!conn_data)
        return;

    char *rdata = (char *)at_malloc(32 + length);
    int data_len = 0;
    if (!rdata) {
        AT_BLE_PRINTF("ble_notificaion_callback malloc failed\r\n");
        return;
    }
    memset(rdata,0,(32 + length));
    data_len = sprintf(rdata, "+BLE:NOTIDATA:%d,%d,%d,",conn_data->idx,handle,length);
    memcpy(rdata + data_len, data, length);
    data_len += length;
    memcpy(rdata + data_len, "\r\n", 2);
    data_len += 2;
    AT_CMD_DATA_SEND(rdata,data_len);
    at_free(rdata);
    rdata = NULL;
    
}
#if defined(CONFIG_BT_SMP)

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));    

    at_response_string("+BLE:PASSKEYDISPLAY:%06u\r\n", passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	at_response_string("+BLE:PASSKEYCONFIRM:%sPASSKEY:%06u\r\n", addr, passkey);
}

static void auth_passkey_entry(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	at_response_string("+BLE:PASSKEYENTRY:%s\r\n", addr);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	at_response_string("+BLE:PAIRCANNELED:%s\r\n", addr);
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	at_response_string("+BLE:PAIRINGCONFIRM:%s\r\n", addr);
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
	char addr[BT_ADDR_LE_STR_LEN];
    struct bt_keys *keys= NULL;
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    keys = bt_keys_get_addr(0,bt_conn_get_dst(conn));

	at_response_string("+BLE:PAIRINGCOMPLETED: %s BTADDR: %s LTK: %s\r\n", bonded ? "Bonded" : "Paired", addr, bt_hex(keys->ltk.val,BT_SMP_MAX_ENC_KEY_SIZE));
 
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    at_response_string("+BLE:PAIRINGFAILED: %s\r\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = NULL,
	.passkey_entry = NULL,
	.passkey_confirm = NULL,
	.cancel = NULL,
	.pairing_confirm = NULL,
	.pairing_failed = NULL,
	.pairing_complete = NULL,
};

int at_ble_sec_paramter_setup(int sec)
{
    
    switch(sec)
    {
        case BT_SMP_IO_DISPLAY_ONLY:
            auth_cb_display.passkey_display = auth_passkey_display;
            auth_cb_display.pairing_confirm =  NULL;
            auth_cb_display.passkey_entry   =  NULL;
            auth_cb_display.passkey_confirm =  NULL;
            break;

        case BT_SMP_IO_DISPLAY_YESNO:
            auth_cb_display.passkey_display =   auth_passkey_display;
            auth_cb_display.pairing_confirm =  auth_pairing_confirm;
            auth_cb_display.passkey_confirm =  auth_passkey_confirm;
            auth_cb_display.passkey_entry   =  NULL;
            break;

        case BT_SMP_IO_KEYBOARD_DISPLAY:

            auth_cb_display.passkey_display =  auth_passkey_display;
            auth_cb_display.pairing_confirm =  auth_pairing_confirm;
            auth_cb_display.passkey_entry   =  auth_passkey_entry;
            auth_cb_display.passkey_confirm =  auth_passkey_confirm;
            break;
        
        case BT_SMP_IO_KEYBOARD_ONLY:
            auth_cb_display.passkey_entry   =  auth_passkey_entry;
            auth_cb_display.passkey_display =  NULL;
            auth_cb_display.pairing_confirm =  NULL;
            auth_cb_display.passkey_confirm =  NULL;
            break;
        case BT_SMP_IO_NO_INPUT_OUTPUT:
            auth_cb_display.passkey_display =  NULL;
            auth_cb_display.pairing_confirm =  NULL;
            auth_cb_display.passkey_entry   =  NULL;
            auth_cb_display.passkey_confirm =  NULL;
        default:
            auth_cb_display.passkey_display =  NULL;
            auth_cb_display.pairing_confirm =  NULL;
            auth_cb_display.passkey_entry   =  NULL;
            auth_cb_display.passkey_confirm =  NULL;
            break;
        
    }
    auth_cb_display.pairing_failed   = auth_pairing_failed;
	auth_cb_display.pairing_complete = auth_pairing_complete;
    auth_cb_display.cancel           = auth_cancel;
    
    return bt_conn_auth_cb_register(&auth_cb_display);
}


int at_ble_sec_auth_cancel(int idx)
{
	struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);

    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED)
        return -EINVAL;
    return bt_conn_auth_cancel(conn_data->conn);
}

int at_ble_sec_auth_passkey_confirm(int idx)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);

    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED)
        return -EINVAL;
  
    return bt_conn_auth_passkey_confirm(conn_data->conn);
}

int at_ble_sec_auth_pairing_confirm(int idx)
{
   
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);

    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED)
        return -EINVAL;
    return bt_conn_auth_pairing_confirm(conn_data->conn);
}

#define 		PASSKEY_MAX  		0xF423F

int at_ble_sec_auth_passkey(int idx, int passkey)
{
       
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);

    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED)
        return -EINVAL;
    return bt_conn_auth_passkey_entry(conn_data->conn, passkey);
}

int at_ble_sec_start_security(int idx, int level)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);

    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED)
        return -EINVAL;
    if(bt_conn_get_security(conn_data->conn)>=level)
        return -EINVAL;
    return bt_conn_set_security(conn_data->conn, level);
}

static void at_ble_foreach_bond_info_cb(const struct bt_bond_info *info, void *user_data)
{
    char addr[BT_ADDR_LE_STR_LEN];
    struct bt_keys *keys=NULL;
    if(user_data)
        (*(u8_t *)user_data)++;

    bt_addr_le_to_str(&info->addr, addr, sizeof(addr));
    keys = bt_keys_find(BT_KEYS_ALL, 0, &info->addr);
	at_response_string("+BLESECGETLTKLIST: BONDADDR %s LTK:%s\r\n",addr,bt_hex(keys->ltk.val,BT_SMP_MAX_ENC_KEY_SIZE));
}

void at_ble_get_ltk_list(void)
{
    bt_foreach_bond(0, at_ble_foreach_bond_info_cb, NULL);
}

int at_ble_get_unpair(char *addr, int type)
{
    bt_addr_le_t btaddr;
    btaddr.type =type;
    reverse_bytearray(addr, btaddr.a.val, 6);
    return bt_unpair(0, &btaddr);
}

#endif

static void ble_write_callback(int conn_idx,int srv_idx, int char_idx, void *buf, u16_t len)
{
    char *data = (char *)at_malloc(32 + len);
    int data_len = 0;
    if (!data) {
        AT_BLE_PRINTF("ble_write_callback malloc failed\r\n");
        data = NULL;
        return;
    }

    data_len = sprintf(data, "+BLE:GATTWRITE:%d,%d,%d,%d,", conn_idx,srv_idx, char_idx, len);
    memcpy(data + data_len, buf, len);
    data_len += len;
    memcpy(data + data_len, "\r\n", 2);
    data_len += 2;
    AT_CMD_DATA_SEND(data, data_len); 

    at_free(data);
    data = NULL;
}

void ble_dynamic_wr_cb(struct bt_conn *conn,const struct bt_gatt_attr* attr,u8_t *data, u16_t length)
{
    int srv_idx, char_idx;
    if (find_index_by_attr(attr, &srv_idx, &char_idx) == 0)
        return;
    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (conn_data == NULL)
        return;
    ble_write_callback(conn_data->idx, srv_idx, char_idx, (void *)data, length);
}
void ble_dynamic_rd_cb(struct bt_conn *conn,const struct bt_gatt_attr* attr,u8_t *data, u16_t *length)
{
    int srv_idx, char_idx;
    if (find_index_by_attr(attr, &srv_idx, &char_idx) == 0)
        return;
    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (conn_data == NULL)
        return;
    u16_t read_len =0;

    read_len = g_ble_srv_data[srv_idx].srv_char[char_idx].read_data_len;
    if (read_len > 0) {
        memcpy(data, g_ble_srv_data[srv_idx].srv_char[char_idx].read_data, read_len);
        AT_BLE_PRINTF("%s,%d\r\n",g_ble_srv_data[srv_idx].srv_char[char_idx].read_data,read_len);
    }
    *length=read_len;
}
void ble_dynamic_noti_cb(const struct bt_gatt_attr* attr ,u8_t data)
{
    int srv_idx, char_idx;
    if (find_index_by_attr(attr, &srv_idx, &char_idx) == 0)
        return;
    
    if( g_ble_srv_data[srv_idx].srv_char[char_idx].char_prop & BLE_GATT_CHAR_PROP_INDICATE)
    {
        if (data == BT_GATT_CCC_INDICATE)
        {
            at_response_string("+BLE:INDICATION:1,%d,%d\r\n",srv_idx,char_idx);
        }
        else
        {
            at_response_string("+BLE:INDICATION:0,%d,%d\r\n",srv_idx,char_idx);
        }
    }
    if( g_ble_srv_data[srv_idx].srv_char[char_idx].char_prop &BLE_GATT_CHAR_PROP_NOTIFY)
    {
        if (data == BT_GATT_CCC_NOTIFY)
        {
            at_response_string("+BLE:NOTIFICATION:1,%d,%d\r\n",srv_idx,char_idx);
        }
        else
        {
            at_response_string("+BLE:NOTIFICATION:0,%d,%d\r\n",srv_idx,char_idx);
        }
    }

}

int at_ble_set_public_addr(uint8_t *addr)
{
    reverse_bytearray(addr,g_ble_public_addr, 6);

    return  bt_set_local_public_address(g_ble_public_addr);
}

int at_ble_get_public_addr(uint8_t *addr)
{
    bt_addr_le_t bt_addr;
    bt_get_local_public_address(&bt_addr);
    memcpy(addr,bt_addr.a.val, 6);
    memcpy(g_ble_public_addr,bt_addr.a.val, 6);
    return 0;
}

int at_ble_set_random_addr(uint8_t *addr)
{
    //do nothing, default is random address
    return 0;
}

int at_ble_set_tx_power(int power)
{
    int err = bt_set_tx_pwr((int8_t)power);

    if(err){
        AT_BLE_PRINTF("ble_set_tx_pwr, Fail to set tx power (err %d)\r\n", err);
        return err;
    }
    else{
        AT_BLE_PRINTF("ble_set_tx_pwr, Set tx power successfully\r\n");
        g_ble_tx_power = power;
        return 0;
    }
}

int at_ble_get_tx_power(void)
{
    struct wl_cfg_t * rfparam = NULL;
    
    if (g_ble_tx_power != -255) {
        return g_ble_tx_power;
    } else {
        rfparam = rfparam_cfg_get();
        if (rfparam == NULL)
            return 0;

        return rfparam->param.pwrtarget.pwr_ble;
    }
}

static void ble_device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t evtype, struct net_buf_simple *buf)
{
    uint8_t ble_addr[6];

    if (g_ble_scan_callback)
    {
        ble_addr_trans((uint8_t *)addr->a.val, ble_addr);
        g_ble_scan_callback(addr->type, ble_addr, rssi, evtype, buf->data, buf->len);
    }
}

int  at_ble_scan_start(ble_scan_callback_t cb)
{
    struct bt_le_scan_param scan_param;
    int err;

    scan_param.type = at_ble_config->scan_param.scan_type;
    scan_param.filter_dup = 0;
    scan_param.interval = at_ble_config->scan_param.scan_interval;
    scan_param.window = at_ble_config->scan_param.scan_window;

    g_ble_scan_callback = cb;
    err = bt_le_scan_start(&scan_param, ble_device_found);
    if(err){
        AT_BLE_PRINTF("ble start scan failed, err = %d\r\n", err);
        return -1;
    }else{
        AT_BLE_PRINTF("ble start scan success\r\n");
        return 0;
    }
}

int at_ble_scan_stop(void)
{
    int err;

    g_ble_scan_callback = NULL;
    err = bt_le_scan_stop();
    if (err) {
        AT_BLE_PRINTF("ble stop scan failed, err = %d\r\n", err);
        return -1;
    } else {
        AT_BLE_PRINTF("ble stop scan success\r\n");
        return 0;
    }
}

static int parse_ble_ad_data(const uint8_t *raw_data, size_t raw_data_len,
                            struct bt_data **bt_data_list, uint8_t *bt_data_count)
{
    size_t offset = 0;
    uint8_t count = 0;
    struct bt_data *list = NULL;
    
    *bt_data_list = NULL;
    *bt_data_count = 0;
    if (!raw_data || raw_data_len == 0 || !bt_data_list || !bt_data_count) {
        AT_BLE_PRINTF("Invalid parameter\r\n");
        return 1;
    }

    while (offset < raw_data_len) {
        uint8_t len = raw_data[offset];
        if (len == 0) break;

        if (offset + len + 1 > raw_data_len) {
            AT_BLE_PRINTF("AD data format error: length overflow (offset=%d, len=%d, raw_len=%d)\r\n",
                          offset, len, raw_data_len);
            return 1;
        }
        
        count++;
        offset += len + 1;
    }
    
    if (count == 0) return 0;
    
    list = (struct bt_data *)at_malloc(count * sizeof(struct bt_data));
    if (!list) return 1;
    
    offset = 0;
    for (uint8_t i = 0; i < count; i++) {
        uint8_t len = raw_data[offset];
        list[i].type = raw_data[offset + 1];
        list[i].data_len = len - 1;
        
        if (list[i].data_len > 0) {
            list[i].data = (const uint8_t *)at_malloc(list[i].data_len);
            if (!list[i].data) {
                for (uint8_t j = 0; j < i; j++) {
                    if (list[j].data) at_free((void *)list[j].data);
                }
                at_free(list);
                return 1;
            }
            memcpy((void *)list[i].data, &raw_data[offset + 2], list[i].data_len);
        } else {
            list[i].data = NULL;
        }
        
        offset += len + 1;
    }
    
    *bt_data_list = list;
    *bt_data_count = count;
    return 0;
}

static void free_ble_ad_data(struct bt_data *list, uint8_t count)
{
    if (list) {
        for (uint8_t i = 0; i < count; i++) {
            if (list[i].data) at_free((void *)list[i].data);
        }
        at_free(list);
    }
}

int at_ble_adv_start(void)
{
    struct bt_le_adv_param param;
    const struct bt_data *ad = NULL;
    const struct bt_data *sdad = NULL;
    size_t ad_len = 0;
    uint8_t set_adv_num = 0;
    uint8_t set_sd_num = 0;

    int err = -1;
    /*Get adv type, 0:adv_ind,  1:adv_scan_ind, 2:adv_nonconn_ind 3: adv_direct_ind*/
    uint8_t adv_type;
    /*Get mode, 0:General discoverable,  1:limit discoverable, 2:non discoverable*/
    uint8_t mode;
    char *adv_name = (char*)at_ble_config->ble_name;

    uint8_t non_disc = BT_LE_AD_NO_BREDR;
    uint8_t gen_disc = BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL;
    uint8_t lim_disc = BT_LE_AD_NO_BREDR | BT_LE_AD_LIMITED;
    struct bt_data ad_discov[2] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
    };

    if (set_adv_channel_map(at_ble_config->adv_param.channel_map) != 0) {
        AT_BLE_PRINTF("Failed to Set adv channel\r\n");
        return -1;
    }

    adv_type = at_ble_config->adv_param.adv_type;
    mode = 0;

    param.id = g_ble_selected_id;
    if(adv_type == 0){
        param.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME);
    }else if(adv_type == 1){
        param.options = BT_LE_ADV_OPT_USE_NAME;
    }else if(adv_type == 2){
        param.options = 0;
    }else{
        AT_BLE_PRINTF("Arg1 is invalid\r\n");
        return -1;
    }
    param.interval_min = at_ble_config->adv_param.adv_int_min;
    param.interval_max = at_ble_config->adv_param.adv_int_max;
    
    if (mode == 0) {
        ad_discov[0].data = &gen_disc;
    } else if (mode == 1) {
        ad_discov[0].data = &lim_disc;
    } else if (mode == 2) {
        ad_discov[0].data = &non_disc;
    } else {
        AT_BLE_PRINTF("Invalied AD Mode 0x%x\r\n",mode);
        return -1;
    }
    if(at_ble_config->adv_data.len!=0)
    {
        if (parse_ble_ad_data(at_ble_config->adv_data.data, 
                              at_ble_config->adv_data.len, 
                              &ad, &set_adv_num)) {
            AT_BLE_PRINTF("Failed to parse adv data\n");
            if (ad) free_ble_ad_data(ad, set_adv_num);
            return -1;
        }
        ad_len = set_adv_num;
    }
    else
    {
        ad = ad_discov;
        ad_len = ARRAY_SIZE(ad_discov);
    }
    if(at_ble_config->scan_rsp_data.len!=0)
    {

        if (parse_ble_ad_data(at_ble_config->scan_rsp_data.data, 
                              at_ble_config->scan_rsp_data.len, 
                              &sdad, &set_sd_num)) {
            AT_BLE_PRINTF("Failed to parse scan rsp data\n");
            if(at_ble_config->adv_data.len!=0)
            {
                if (ad) free_ble_ad_data(ad, set_sd_num);
            }
            return -1;
        }
        for (int i = 0; i < set_sd_num; i++) {
            if (sdad[i].type == BT_DATA_NAME_COMPLETE) {
                uint8_t name_len = MIN(sdad[i].data_len, sizeof(at_ble_config->ble_name) - 1);
                memcpy(at_ble_config->ble_name, sdad[i].data, name_len);
                at_ble_config->ble_name[name_len] = '\0';
                bt_set_name(at_ble_config->ble_name);
                if (at->store) {
                    at_ble_config_save(AT_CONFIG_KEY_BLE_NAME);
                }
                break;
            }
        }
        memset(ad_discov,0,ARRAY_SIZE(ad_discov));
        ad_discov[0] = sdad[0];
    }
    if(adv_type == 1 || adv_type == 0){
#if defined(CONFIG_BLE_MULTI_ADV)
        if(ble_adv_id == 0){
            err = bt_le_multi_adv_start(&param, ad, ad_len, &ad_discov[0], 1, &g_ble_adv_id);
        }        
#else
        err = bt_le_adv_start(&param, ad, ad_len, &ad_discov[0], 1);
        #endif/*CONFIG_BLE_MULTI_ADV*/
    }else{
#if defined(CONFIG_BLE_MULTI_ADV)
        if(ble_adv_id == 0){
            err = bt_le_multi_adv_start(&param, ad, ad_len, NULL, 0, &g_ble_adv_id);
        }
#else
        err = bt_le_adv_start(&param, ad, ad_len, NULL, 0);
#endif
    }
    if(at_ble_config->adv_data.len!=0) free_ble_ad_data(ad, set_adv_num);
    if(at_ble_config->scan_rsp_data.len!=0) free_ble_ad_data(sdad, set_sd_num);
    if (err) {
        AT_BLE_PRINTF("ble start advertise failed, err = %d\r\n", err);
        return -1;
    } else {
        AT_BLE_PRINTF("ble start advertise success\r\n");

        return 0;
    }
}

int at_ble_adv_stop(void)
{
#if defined(CONFIG_BLE_MULTI_ADV)
    bool err = -1;
    if(g_ble_adv_id && !bt_le_multi_adv_stop(g_ble_adv_id)){
        g_ble_adv_id = 0;
        err = 0;
    }
    if(err){
#else
    if (bt_le_adv_stop()) {
#endif
        AT_BLE_PRINTF("ble stop advertise failed\r\n");
        return -1;	
    } else { 
        AT_BLE_PRINTF("ble stop advertise success\r\n");

        return 0;
    }
}

int at_ble_is_valid_conn_idx(int idx)
{
    if (idx < 0 || idx >= BLE_CONN_MAX_NUM)
        return 0;

    return 1;
}

int at_ble_get_idx_by_conn(struct bt_conn *conn)
{
    for (int i = 0; i < BLE_CONN_MAX_NUM; i++) 
    {
        
        struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(i);

        if(conn_data == NULL)
            continue;

        if (conn_data->conn != NULL)
        {
            if(conn==conn_data->conn)
            {
                return i;
            }
        }    
    
    }
    return -1;
}

int at_ble_is_connected(int idx)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);
    if (!conn_data)
        return 0;

    if (conn_data->state == BLE_CONN_STATE_CONNECTED)
        return 1;
    else
        return 0;
}


int at_ble_conn(int idx, uint8_t *addr, int addr_type, int timeout)
{
    bt_addr_le_t ble_addr;
    struct bt_conn *conn;
    struct bt_le_conn_param param = {
        .interval_min =  BT_GAP_INIT_CONN_INT_MIN,
        .interval_max =  BT_GAP_INIT_CONN_INT_MAX,
        .latency = 0,
        .timeout = 400,
    };
    
    if (at_ble_is_connected(idx))
    {
        return 0;
    }

    int ret = 0;
    uint64_t start_time = at_current_ms_get();

    ble_addr.type = addr_type; /*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    ble_addr_trans(addr, ble_addr.a.val);

    param.timeout = (timeout * 1000) / 10;

    conn = bt_conn_create_le(&ble_addr, &param);
    if (!conn) {
        AT_BLE_PRINTF("Connection failed\r\n");
    } else {
        if(conn->state == BT_CONN_CONNECTED) {
            AT_BLE_PRINTF("Le link with this peer device has existed\r\n");
            return 1;
        } else {
            AT_BLE_PRINTF("Connection pending\r\n");
            ble_conn_data_set(idx,BT_CONN_ROLE_MASTER,ble_addr.a.val, addr_type, conn, param.interval_min, param.interval_max, 0, BLE_CONN_STATE_CONNCTING);
            while(at_current_ms_get() - start_time < timeout*1000) {
                if (at_ble_is_connected(idx)) {
                     at_response_string("+BLE:CONNECTED:%d %d,\"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n",
                            idx,
                            BT_CONN_ROLE_MASTER,
                            addr[0],
                            addr[1],
                            addr[2],
                            addr[3],
                            addr[4],
                            addr[5]);
                    ret = 1;
                    break;
                }

                vTaskDelay(50);
            }

        }
    }
    if(ret == 0)
    {
        struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);

        conn_data->state = BLE_CONN_STATE_DISCONNECTED;
        conn_data->conn  = NULL;
        conn_data->valid = 0;
        bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        
    }
    
    return ret;
}

int at_ble_conn_get_addr(int idx, uint8_t *addr)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);
    if (!conn_data)
        return 0;

     if (conn_data->state != BLE_CONN_STATE_CONNECTED)
        return 0;

     memcpy(addr, conn_data->addr, 6);
     return 1;
}
int at_ble_conn_get_role(int idx)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);
    if (!conn_data)
        return -1;

     if (conn_data->state != BLE_CONN_STATE_CONNECTED)
        return -1;

    return conn_data->role;
}


int at_ble_conn_get_param(int idx, int *min_interval, int *max_interval, int *cur_interval, int *latency, int *timeout)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);
    if (!conn_data)
        return 0;

     if (conn_data->state != BLE_CONN_STATE_CONNECTED)
        return 0;

     *min_interval = conn_data->min_interval;
     *max_interval = conn_data->max_interval;
     *cur_interval = conn_data->cur_interval;
     *latency = conn_data->latency;
     *timeout = conn_data->timeout;
     return 1;
}

int at_ble_conn_update_param(int idx, int min_interval, int max_interval, int latency, int timeout)
{
    struct bt_le_conn_param param;
    int err = 0;
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);
    if (!conn_data)
        return 0;

    conn_data->min_interval = min_interval;
    conn_data->max_interval = max_interval;

    param.interval_min = min_interval;
    param.interval_max = max_interval;
    param.latency = latency;
    param.timeout = timeout;

    err = bt_conn_le_param_update(conn_data->conn, &param);
    if (err) {
        AT_BLE_PRINTF("conn update failed (err %d)\r\n", err);
        return err;
    } else {
        AT_BLE_PRINTF("conn update initiated\r\n");
        return 0;
    }
}

int at_ble_disconn(int idx)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);
    if (!conn_data)
        return 0;
    
    if(!conn_data->conn){
        AT_BLE_PRINTF("Not connected\r\n");
        return 0;
    }

    if (bt_conn_disconnect(conn_data->conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN)) {
        AT_BLE_PRINTF("Disconnection failed\r\n");
        return 0;
    } else {
        AT_BLE_PRINTF("Disconnect successfully\r\n");
    }

    return 1;
}

int at_ble_conn_update_datalen(int idx, int data_len, int time)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);
    if (!conn_data)
        return 1;
    
    int ret = bt_le_set_data_len(conn_data->conn, data_len, time);
    if(ret == 0)
    {
        AT_BLE_PRINTF("ble set data length success.");
        return 0;
    }
    else
    {
        AT_BLE_PRINTF("ble set data length failure, err: %d\n", ret);
        return ret;
    }
}

static void exchange_func(struct bt_conn *conn, u8_t err,
			  struct bt_gatt_exchange_params *params)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (!conn_data)
        return;

	AT_BLE_PRINTF("Exchange %s MTU Size =%d \r\n", err == 0U ? "successful" : "failed", bt_gatt_get_mtu(conn));
    if(err == 0U)
    {
        at_response_string("+BLE:MTUSIZE:%d,%d\r\n",conn_data->idx,bt_gatt_get_mtu(conn));
    }
}

static struct bt_gatt_exchange_params exchange_params;

int at_ble_conn_get_mtu(int idx, int *mtu_size)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);
    if (!conn_data)
        return 0;

    *mtu_size = bt_gatt_get_mtu(conn_data->conn);
    return 1;
}

int at_ble_conn_update_mtu(int idx)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_idx(idx);
    if (!conn_data)
        return 0;
    int err;

    exchange_params.func = exchange_func;
    err = bt_gatt_exchange_mtu(conn_data->conn, &exchange_params);
    if (err) {
        AT_BLE_PRINTF("Exchange failed (err %d)\r\n", err);
        return 0;
    } else {
        AT_BLE_PRINTF("Exchange pending\r\n");
        return 1;
    }
}

static int at_ble_srv_idx_is_valid(int idx)
{
    if (idx < 0 || idx >= BLE_SRV_MAX_NUM)
        return 0;
    else
        return 1;
}

static int at_ble_char_idx_is_valid(int idx)
{
    if (idx < 0 || idx >= BLE_CHAR_MAX_NUM)
        return 0;
    else
        return 1;
}

int at_ble_gatts_service_get(int srv_idx, uint8_t *srv_uuid, uint8_t *srv_type,uint8_t *uuid_type)
{
    CHECK_BLE_SRV_IDX_VALID(srv_idx);

    if (g_ble_srv_data[srv_idx].valid == 0)
        return 0;
    if (g_ble_srv_data[srv_idx].uuid_type == BT_UUID_TYPE_128)
        ble_uuid_trans(g_ble_srv_data[srv_idx].srv_uuid.val, srv_uuid);
    if (g_ble_srv_data[srv_idx].uuid_type == BT_UUID_TYPE_16)
        ble_uuid_16_trans_get(g_ble_srv_data[srv_idx].srv_uuid_16.val, srv_uuid);
    *srv_type = g_ble_srv_data[srv_idx].srv_type;
    *uuid_type = g_ble_srv_data[srv_idx].uuid_type;
    return 1;
}

int at_ble_gatts_service_set(int srv_idx, uint8_t *srv_uuid, uint8_t srv_type,uint8_t uuid_type)
{
    CHECK_BLE_SRV_IDX_VALID(srv_idx);
    if (g_ble_srv_data[srv_idx].valid == 1)
        return 0;
	// TODO
    /*if (check_uuid_is_unique)
        return 0;*/

    if(uuid_type == BT_UUID_TYPE_16)
    {
        g_ble_srv_data[srv_idx].uuid_type = BT_UUID_TYPE_16;
        g_ble_srv_data[srv_idx].srv_uuid_16.uuid.type = BT_UUID_TYPE_16;
        ble_uuid_16_trans_set(srv_uuid, &g_ble_srv_data[srv_idx].srv_uuid_16.val);
    }
    else
    {
        g_ble_srv_data[srv_idx].uuid_type = BT_UUID_TYPE_128;
        g_ble_srv_data[srv_idx].srv_uuid.uuid.type = BT_UUID_TYPE_128;
        ble_uuid_trans(srv_uuid, g_ble_srv_data[srv_idx].srv_uuid.val);
    }
    g_ble_srv_data[srv_idx].srv_type = srv_type;
    g_ble_srv_data[srv_idx].valid = 1;
    return 1;
}

int at_ble_gatts_service_del(int srv_idx)
{
    int i;

    CHECK_BLE_SRV_IDX_VALID(srv_idx);

    if (g_ble_srv_data[srv_idx].valid == 0)
        return 0;

    g_ble_srv_data[srv_idx].valid = 0;
    memset(&g_ble_srv_data[srv_idx].srv_uuid, 0, sizeof(g_ble_srv_data[srv_idx].srv_uuid));
    memset(&g_ble_srv_data[srv_idx].srv_uuid_16, 0, sizeof(g_ble_srv_data[srv_idx].srv_uuid_16));
    g_ble_srv_data[srv_idx].srv_type = 0;
    g_ble_srv_data[srv_idx].uuid_type = 0;
    for (i = 0; i < BLE_CHAR_MAX_NUM; i++) {
        g_ble_srv_data[srv_idx].srv_char[i].valid = 0;
        memset(&g_ble_srv_data[srv_idx].srv_char[i].char_uuid, 0, sizeof(g_ble_srv_data[srv_idx].srv_char[i].char_uuid));
        memset(&g_ble_srv_data[srv_idx].srv_char[i].char_uuid_16, 0, sizeof(g_ble_srv_data[srv_idx].srv_char[i].char_uuid_16));
        g_ble_srv_data[srv_idx].srv_char[i].char_prop = 0;
        g_ble_srv_data[srv_idx].srv_char[i].char_perm = 0;
        g_ble_srv_data[srv_idx].srv_char[i].uuid_type = 0;
        g_ble_srv_data[srv_idx].srv_char[i].attr = NULL;
        g_ble_srv_data[srv_idx].srv_char[i].read_data_len = 0;
    }

    return 1;
}

int at_ble_gatts_service_char_get(int srv_idx, int char_idx, uint8_t *char_uuid, uint32_t *char_prop,uint32_t *char_perm,uint8_t *uuidtype)
{
    CHECK_BLE_SRV_IDX_VALID(srv_idx);
    CHECK_BLE_CHAR_IDX_VALID(char_idx);

    if (g_ble_srv_data[srv_idx].valid == 0)
        return 0;
    if (g_ble_srv_data[srv_idx].srv_char[char_idx].valid == 0)
        return 0;
    if (g_ble_srv_data[srv_idx].srv_char[char_idx].uuid_type == BT_UUID_TYPE_16)
        ble_uuid_16_trans_get(g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid_16.val, char_uuid);
    if (g_ble_srv_data[srv_idx].srv_char[char_idx].uuid_type == BT_UUID_TYPE_128)
        ble_uuid_trans(g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid.val, char_uuid);
    *char_prop = g_ble_srv_data[srv_idx].srv_char[char_idx].char_prop;
    *char_perm = g_ble_srv_data[srv_idx].srv_char[char_idx].char_perm;
    *uuidtype=g_ble_srv_data[srv_idx].srv_char[char_idx].uuid_type;
    return 1;
}

int at_ble_gatts_service_register(int enable)
{
    if(enable == 1)
    {
        if(g_ble_dynamic_init == 0)
        {
            ble_dynamic_gatt_server_init();
            ble_dynamic_gatt_cb_register(ble_dynamic_rd_cb,ble_dynamic_wr_cb,ble_dynamic_noti_cb);
            g_ble_dynamic_init = 1;
        }
        ble_add_service();
    }
    if(enable == 0)
    {
        if(ble_dynamic_unregister_service())
            return 1;
        if(g_ble_dynamic_init == 1)
        {
            ble_dynamic_gatt_server_deinit();
            g_ble_dynamic_init = 0;
        }
    }

    return 0;
}

int at_ble_gatts_service_char_set(int srv_idx, int char_idx, uint8_t *char_uuid, uint32_t char_prop,uint32_t char_perm, uint8_t uuid_type)
{
    CHECK_BLE_SRV_IDX_VALID(srv_idx);
    CHECK_BLE_CHAR_IDX_VALID(char_idx);

    if (g_ble_srv_data[srv_idx].valid == 0)
        return 0;
    if (g_ble_srv_data[srv_idx].srv_char[char_idx].valid == 1)
        return 0;

    
    if(uuid_type == BT_UUID_TYPE_16)
    {
        g_ble_srv_data[srv_idx].srv_char[char_idx].uuid_type = BT_UUID_TYPE_16;
        g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid_16.uuid.type = BT_UUID_TYPE_16;
        ble_uuid_16_trans_set(char_uuid, &g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid_16.val);
    }
    else
    {
        g_ble_srv_data[srv_idx].srv_char[char_idx].uuid_type = BT_UUID_TYPE_128;
        g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid.uuid.type = BT_UUID_TYPE_128;
        ble_uuid_trans(char_uuid, g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid.val);
    }
    g_ble_srv_data[srv_idx].srv_char[char_idx].char_prop = char_prop;
    g_ble_srv_data[srv_idx].srv_char[char_idx].char_perm = char_perm;
    g_ble_srv_data[srv_idx].srv_char[char_idx].attr = NULL;
    g_ble_srv_data[srv_idx].srv_char[char_idx].read_data_len = 0;
    g_ble_srv_data[srv_idx].srv_char[char_idx].valid = 1;
    return 1;
}

int at_ble_gatts_service_notify(int idx,int srv_idx, int char_idx, void * buffer, int length)
{
    CHECK_BLE_SRV_IDX_VALID(srv_idx);
    CHECK_BLE_CHAR_IDX_VALID(char_idx);
    
    struct ble_conn_data *conn_data = NULL;
    conn_data = ble_conn_data_get_by_idx(idx);
    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED||conn_data->role != BT_CONN_ROLE_SLAVE)
        return 0;

    if (g_ble_srv_data[srv_idx].valid == 0)
        return 0;
    if (g_ble_srv_data[srv_idx].srv_char[char_idx].valid == 0)
        return 0;
    if (g_ble_srv_data[srv_idx].srv_char[char_idx].char_prop & BLE_GATT_CHAR_PROP_NOTIFY && length <= 244) {

        struct bt_gatt_attr* noti_attr =NULL;
        if(g_ble_srv_data[srv_idx].srv_char[char_idx].uuid_type == BT_UUID_TYPE_16)
        {
            noti_attr=ble_dynamic_gatt_get_attr(&g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid_16.uuid);
        }
        else
        {
            noti_attr=ble_dynamic_gatt_get_attr(&g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid.uuid);
        }
        if(noti_attr)
        {
            int err = bt_gatt_notify(conn_data->conn,noti_attr, buffer, length);
            if (err != 0) {
                AT_BLE_PRINTF("ble send notify data failed: %d\r\n", err);
                noti_attr = NULL;
                return 0;
            }
            noti_attr = NULL;
            return length;
        }
        else
        {
            noti_attr = NULL;
            return 0;
        }
    } else {
        return 0;
    }
}

static void ble_gatt_send_indicate_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,	u8_t err)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (!conn_data)
        return;

    conn_data->indicate_finish = 0;
    at_response_string("+BLE:INDICATION:2,%d\r\n",err);
    
}

static int ble_gatt_send_indicate(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				                    const void *data, u16_t len)
{
    //indicate paramete must be allocated statically
    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (!conn_data)
        return 1;
    if(conn_data->indicate_finish)
        return 1;
    static struct bt_gatt_indicate_params ind_params;
    ind_params.attr = attr;
    ind_params.data = data;
    ind_params.len = len;
    ind_params.func = ble_gatt_send_indicate_cb;
    ind_params.uuid = NULL;
    /*it is possible to indicate by UUID by setting it on the
      parameters, when using this method the attribute given is used as the
      start range when looking up for possible matches.In this case,set uuid as follows.*/
    //ind_params->uuid = attrs[6].uuid;

    if(bt_gatt_indicate(conn, &ind_params)==0)
    {
        conn_data->indicate_finish = 1;
        return 0;
    }
    return 1;
}

int at_ble_gatts_service_indicate(int idx,int srv_idx, int char_idx, void * buffer, int length)
{
    CHECK_BLE_SRV_IDX_VALID(srv_idx);
    CHECK_BLE_CHAR_IDX_VALID(char_idx);

    if (g_ble_srv_data[srv_idx].valid == 0)
        return 0;
    if (g_ble_srv_data[srv_idx].srv_char[char_idx].valid == 0)
        return 0;
    struct ble_conn_data *conn_data = NULL;
    conn_data = ble_conn_data_get_by_idx(idx);
    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED||conn_data->role != BT_CONN_ROLE_SLAVE)
        return 0;
    if (g_ble_srv_data[srv_idx].srv_char[char_idx].char_prop & BLE_GATT_CHAR_PROP_INDICATE && length <= 244) {
       
        struct bt_gatt_attr* indicate_attr =NULL;
        if(g_ble_srv_data[srv_idx].srv_char[char_idx].uuid_type == BT_UUID_TYPE_16)
        {
            indicate_attr=ble_dynamic_gatt_get_attr(&g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid_16.uuid);
        }
        else
        {
            indicate_attr=ble_dynamic_gatt_get_attr(&g_ble_srv_data[srv_idx].srv_char[char_idx].char_uuid.uuid);
        }
        if(indicate_attr)
        {
            int err = ble_gatt_send_indicate(conn_data->conn, indicate_attr, buffer, length);
            if (err != 0) {
                AT_BLE_PRINTF("ble send indicate data failed: %d\r\n", err);
                indicate_attr = NULL; 
                return 0;
            }
            indicate_attr = NULL; 
            return length;
        }
        else
        {
            indicate_attr = NULL;
            return 0;
        }
 
    } else {
        return 0;
    }
}

int at_ble_gatts_service_read(int srv_idx, int char_idx, void * buffer, int length)
{
    CHECK_BLE_SRV_IDX_VALID(srv_idx);
    CHECK_BLE_CHAR_IDX_VALID(char_idx);
    
    if (g_ble_srv_data[srv_idx].valid == 0)
        return 0;
    if (g_ble_srv_data[srv_idx].srv_char[char_idx].valid == 0)
        return 0;

    if (g_ble_srv_data[srv_idx].srv_char[char_idx].char_prop & BLE_GATT_CHAR_PROP_READ && length <= 244) {
        memcpy(g_ble_srv_data[srv_idx].srv_char[char_idx].read_data, buffer, length);
        g_ble_srv_data[srv_idx].srv_char[char_idx].read_data_len = length;
        return length;
        
    } else {
        return 0;
    }
}

static uint32_t print_chrc_props(u8_t properties)
{
    uint32_t props = 0;
    AT_BLE_PRINTF("Properties: ");

    if (properties & BT_GATT_CHRC_BROADCAST) {
        AT_BLE_PRINTF("[bcast]");
    }

    if (properties & BT_GATT_CHRC_READ) {
        AT_BLE_PRINTF("[read]");
        props |= BLE_GATT_CHAR_PROP_READ;
    }

    if (properties & BT_GATT_CHRC_WRITE) {
        AT_BLE_PRINTF("[write]");
        props |= BLE_GATT_CHAR_PROP_WRITE;
    }

    if (properties & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
        AT_BLE_PRINTF("[write w/w rsp]");
        props |= BLE_GATT_CHAR_PROP_WRITE_WITHOUT_RESP;
    }

    if (properties & BT_GATT_CHRC_NOTIFY) {
        AT_BLE_PRINTF("[notify]");
        props |= BLE_GATT_CHAR_PROP_NOTIFY;
    }

    if (properties & BT_GATT_CHRC_INDICATE) {
        AT_BLE_PRINTF("[indicate]");
        props |= BLE_GATT_CHAR_PROP_INDICATE;
    }

    if (properties & BT_GATT_CHRC_AUTH) {
        AT_BLE_PRINTF("[auth]");
    }

    if (properties & BT_GATT_CHRC_EXT_PROP) {
        AT_BLE_PRINTF("[ext prop]");
    }

    AT_BLE_PRINTF("\r\n");
    return props;
}

static struct bt_gatt_discover_params g_ble_discover_params;
static int g_ble_discover_type = 0;
static struct bt_gatt_write_params g_ble_write_params;
static struct bt_gatt_read_params g_ble_read_params;
static int g_ble_discover_finish = 0;
static int g_ble_discover_service_index = 0;
static int g_ble_read_finish = 0;
static int g_ble_read_service_index = 0;
static int g_ble_read_char_index = 0;


struct ble_disc_char
{
    uint8_t valid;
    uint8_t uuid_type;
    uint16_t char_handle;
    uint16_t char_value_handle;
    uint32_t char_props;
};

struct ble_discover_data
{
    uint8_t conn_idx;
    uint8_t valid;
    uint8_t uuid_type;
    uint16_t start_handle;
    uint16_t end_handle;
    uint8_t type;

    struct ble_disc_char disc_char[BLE_GATTC_CHAR_MAX_NUM];
};

struct ble_client_disc
{
   struct ble_discover_data disc_srv[BLE_GATTC_SRV_MAX_NUM];
};
static struct ble_client_disc* g_ble_disc_srv =NULL;

static void ble_disc_srv_clean(void)
{
    if (g_ble_disc_srv != NULL) {
        memset(g_ble_disc_srv, 0, sizeof(struct ble_client_disc) * BLE_CONN_MAX_NUM);
        at_free(g_ble_disc_srv);
        g_ble_disc_srv = NULL;
    }
}

static int ble_disc_srv_set(int idx, uint16_t start_handle, uint16_t end_handle, uint8_t type)
{
    int i;

    for (i = 0; i < BLE_GATTC_SRV_MAX_NUM; i++) {
        if (g_ble_disc_srv[idx].disc_srv[i].valid == 0) {
            g_ble_disc_srv[idx].disc_srv[i].valid = 1;
            g_ble_disc_srv[idx].disc_srv[i].conn_idx = idx;
            g_ble_disc_srv[idx].disc_srv[i].start_handle = start_handle;
            g_ble_disc_srv[idx].disc_srv[i].end_handle = end_handle;
            g_ble_disc_srv[idx].disc_srv[i].type = type;
            return 1;
        }
    }

    return 0;
}
static int ble_disc_srv_handle_check(uint16_t handle,int idx)
{
    int i;
    for (i = 0; i < BLE_GATTC_SRV_MAX_NUM; i++) {
        if (g_ble_disc_srv[idx].disc_srv[i].valid == 1 && g_ble_disc_srv[idx].disc_srv[i].start_handle <= handle && g_ble_disc_srv[idx].disc_srv[i].end_handle >= handle) {
            return 1;
        }
    }
    return 0;
}

static int ble_get_disc_srv_idx(int idx,uint16_t handle)
{
    int i;
    for (i = 0; i < BLE_GATTC_SRV_MAX_NUM; i++) {
        if (g_ble_disc_srv[idx].disc_srv[i].start_handle == handle) {
            return i;
        }
    }
    return -1;
}

static void ble_disc_srv_clean_char(int srv_idx,int idx)
{
    int index = srv_idx - 1;
 
    if (index < 0 || index >= BLE_GATTC_SRV_MAX_NUM)
        return;
    for(int i = 0; i < BLE_GATTC_CHAR_MAX_NUM; i++)
    {
        memset(&g_ble_disc_srv[idx].disc_srv[index].disc_char[i], 0, sizeof(g_ble_disc_srv[idx].disc_srv[index].disc_char[i]));
    }
   
}

static struct ble_discover_data *ble_disc_srv_get_char(int srv_idx,int idx)
{
    int index = srv_idx - 1;

    if (index < 0 || index >= BLE_GATTC_SRV_MAX_NUM)
        return NULL;

    if (g_ble_disc_srv[idx].disc_srv[index].valid == 0)
        return NULL;
    return &g_ble_disc_srv[idx].disc_srv[index];
}

static int ble_disc_srv_get_char_idx(int idx,int srv_idx,uint16_t handle)
{
    int i;
    for (i = 0; i < BLE_GATTC_CHAR_MAX_NUM; i++) {
        if (g_ble_disc_srv[idx].disc_srv[srv_idx-1].disc_char[i].char_handle == handle) {
            return i;
        }
    }
    return -1;
}

static int ble_disc_srv_set_char(int idx, int srv_idx, uint16_t handle, uint16_t value_handle, uint32_t props)
{
    int index = srv_idx - 1;
    int i;
    if (index < 0 || index >= BLE_GATTC_SRV_MAX_NUM)
        return;
    if (g_ble_disc_srv[idx].disc_srv[index].valid) {
        for (i = 0; i < BLE_GATTC_CHAR_MAX_NUM; i++) {
            if (g_ble_disc_srv[idx].disc_srv[index].disc_char[i].valid == 0) {
                g_ble_disc_srv[idx].disc_srv[index].disc_char[i].valid = 1;
                g_ble_disc_srv[idx].disc_srv[index].disc_char[i].char_handle = handle;
                g_ble_disc_srv[idx].disc_srv[index].disc_char[i].char_value_handle = value_handle;
                g_ble_disc_srv[idx].disc_srv[index].disc_char[i].char_props = props;
                return 1;
            }
        }
    }

    return 0;
}
static u8_t ble_discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params)
{
    struct bt_gatt_service_val *gatt_service;
    struct bt_gatt_chrc *gatt_chrc;
    struct bt_gatt_include *gatt_include;
    char str[37]={0};
    uint32_t props = 0;

    if (!attr) {
        g_ble_discover_finish = 1;
        AT_BLE_PRINTF( "Discover complete\r\n");
#if defined(BFLB_BLE_PATCH_ADD_ERRNO_IN_DISCOVER_CALLBACK)
        if(params->err != 0){
            AT_BLE_PRINTF("Discover erro %x\n", params->err);
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
                AT_BLE_PRINTF("Service %s found: start handle %x, end_handle %x\r\n", str, attr->handle, gatt_service->end_handle);
                if(at_ble_get_idx_by_conn(conn)>=0)
                {
                    if (!ble_disc_srv_set(at_ble_get_idx_by_conn(conn), attr->handle, gatt_service->end_handle, g_ble_discover_type))
                        AT_BLE_PRINTF("Service num exceeds %d\r\n", BLE_GATTC_SRV_MAX_NUM);
                    int srv_idx = ble_get_disc_srv_idx(at_ble_get_idx_by_conn(conn),attr->handle);
                    if(srv_idx>=0)
                    {
                        at_response_string("+BLE:SRV:%d,%d,%s,%d,%d,%d\r\n", at_ble_get_idx_by_conn(conn), srv_idx + 1,str, g_ble_disc_srv[at_ble_get_idx_by_conn(conn)].disc_srv[srv_idx].type,g_ble_disc_srv[at_ble_get_idx_by_conn(conn)].disc_srv[srv_idx].start_handle,g_ble_disc_srv[at_ble_get_idx_by_conn(conn)].disc_srv[srv_idx].end_handle);
                    }
                   
                }
                break;
            case BT_GATT_DISCOVER_CHARACTERISTIC:
                gatt_chrc = attr->user_data;
                bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));
                AT_BLE_PRINTF("Characteristic %s found: attr->handle %x  chrc->handle %x \r\n", str, attr->handle,gatt_chrc->value_handle);
                props = print_chrc_props(gatt_chrc->properties);
                if(at_ble_get_idx_by_conn(conn)>=0)
                {
                    if (!ble_disc_srv_set_char(at_ble_get_idx_by_conn(conn), g_ble_discover_service_index, attr->handle, gatt_chrc->value_handle, props))
                        AT_BLE_PRINTF("Service num exceeds %d\r\n", BLE_GATTC_CHAR_MAX_NUM);

                    int char_idx = ble_disc_srv_get_char_idx(at_ble_get_idx_by_conn(conn),g_ble_discover_service_index,attr->handle);
                    if(char_idx>=0)
                    {
                        at_response_string("+BLE:SRVCHAR:%d,%d,%d,%s,0x%02x,%d,%d\r\n", at_ble_get_idx_by_conn(conn), g_ble_discover_service_index, char_idx + 1, str, props, g_ble_disc_srv[at_ble_get_idx_by_conn(conn)].disc_srv[g_ble_discover_service_index-1].disc_char[char_idx].char_handle, g_ble_disc_srv[at_ble_get_idx_by_conn(conn)].disc_srv[g_ble_discover_service_index-1].disc_char[char_idx].char_value_handle);
                    }
                }
                break;
            case BT_GATT_DISCOVER_INCLUDE:
                gatt_include = attr->user_data;
                bt_uuid_to_str(gatt_include->uuid, str, sizeof(str));
                AT_BLE_PRINTF("Include %s found: handle %x, start %x, end %x\r\n", str, attr->handle,
                gatt_include->start_handle, gatt_include->end_handle);
                break;
            default:
                bt_uuid_to_str(attr->uuid, str, sizeof(str));
                AT_BLE_PRINTF("Descriptor %s found: handle %x\r\n", str, attr->handle);
                break;
        }

        return BT_GATT_ITER_CONTINUE;
}

int at_ble_gattc_service_discover(int idx, int timeout)
{
    int err;
    struct ble_conn_data *conn_data = NULL;
    uint64_t start_time = at_current_ms_get();

    conn_data = ble_conn_data_get_by_idx(idx);
    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED || conn_data->role != BT_CONN_ROLE_MASTER)
        return 0;

    memset(&g_ble_disc_srv[idx], 0, sizeof(struct ble_client_disc));
    g_ble_discover_finish = 0;
    g_ble_discover_type = 1; //discover primary services

    g_ble_discover_params.func = ble_discover_func;
    g_ble_discover_params.start_handle = 0x0001;
    g_ble_discover_params.end_handle = 0xffff;
    g_ble_discover_params.type = BT_GATT_DISCOVER_PRIMARY;
    g_ble_discover_params.uuid = NULL;

    err = bt_gatt_discover(conn_data->conn, &g_ble_discover_params);
    if (err) {
        AT_BLE_PRINTF("Discover failed (err %d)\r\n", err);
        return 0;
    } else {
        AT_BLE_PRINTF("Discover pending\r\n");
        while(at_current_ms_get() - start_time < timeout) {
            if (g_ble_discover_finish) {
               break;
            }
            vTaskDelay(50);
        }
    }

    g_ble_discover_finish = 0;
    g_ble_discover_type = 0; //discover secondary services

    g_ble_discover_params.func = ble_discover_func;
    g_ble_discover_params.start_handle = 0x0001;
    g_ble_discover_params.end_handle = 0xffff;
    g_ble_discover_params.type = BT_GATT_DISCOVER_SECONDARY;
    g_ble_discover_params.uuid = NULL;

    err = bt_gatt_discover(conn_data->conn, &g_ble_discover_params);
    if (err) {
        AT_BLE_PRINTF("Discover failed (err %d)\r\n", err);
        return 0;
    } else {
        AT_BLE_PRINTF("Discover pending\r\n");
        while(at_current_ms_get() - start_time < timeout) {
            if (g_ble_discover_finish) {
               break;
            }
            vTaskDelay(50);
        }
    }

    return 1;
}

int at_ble_gattc_service_char_discover(int idx, int srv_idx, int timeout)
{
    int err;
    struct ble_conn_data *conn_data = NULL;
    struct ble_discover_data *discover_data = NULL;
    uint64_t start_time = at_current_ms_get();

    conn_data = ble_conn_data_get_by_idx(idx);
    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED || conn_data->role != BT_CONN_ROLE_MASTER)
        return 0;

    discover_data = ble_disc_srv_get_char(srv_idx,idx);
    if (discover_data == NULL)
        return 0;
   
    g_ble_discover_finish = 0;
    g_ble_discover_service_index = srv_idx;
    ble_disc_srv_clean_char(srv_idx,idx);

    g_ble_discover_params.func = ble_discover_func;
    g_ble_discover_params.start_handle = discover_data->start_handle;
    g_ble_discover_params.end_handle = discover_data->end_handle;
    g_ble_discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    g_ble_discover_params.uuid  = NULL;

    err = bt_gatt_discover(conn_data->conn, &g_ble_discover_params);
    if (err) {
        AT_BLE_PRINTF("Discover failed (err %d)\r\n", err);
        return 0;
    } else {
        AT_BLE_PRINTF("Discover pending\r\n");
        while(at_current_ms_get() - start_time < timeout) {
            if (g_ble_discover_finish) {
                return 1;
            }
            vTaskDelay(50);
        }
        return 0;
    }
}
static struct bt_gatt_subscribe_params subscribe_params[BLE_GATTC_CHAR_MAX_NUM] = {0};


static uint8_t notify_func(struct bt_conn *conn,
			struct bt_gatt_subscribe_params *params,
			const void *data, u16_t length)
{
    if (!params->value) {
        AT_BLE_PRINTF("Unsubscribed\r\n");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }
    
    return BT_GATT_ITER_CONTINUE;
}
int at_ble_subscribe(int idx, int ccc_handle, int value_handle,int value)
{
    struct ble_conn_data *conn_data = NULL;
    struct bt_gatt_subscribe_params *subscribe_index = NULL;

    conn_data = ble_conn_data_get_by_idx(idx);
    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED||conn_data->role != BT_CONN_ROLE_MASTER)
        return -EINVAL;
    if(!ble_disc_srv_handle_check(value_handle,idx)||!ble_disc_srv_handle_check(ccc_handle,idx))
    {
        AT_BLE_PRINTF("Unsubscribe failed (err: Invalid handle)\r\n");
        return -EINVAL;
    }
    // Iterate through the subscriptions array to find an available entry
    for (int i = 0; i < sizeof(subscribe_params) / sizeof(subscribe_params[0]); i++) {
        if(subscribe_params[i].value_handle == 0U){
            subscribe_index = &subscribe_params[i];
            break;
        }
    }    

    if(subscribe_index == NULL){
        AT_BLE_PRINTF("Unsubscribe failed (err: Invalid handle)\r\n");
        return -EINVAL;
    }

    subscribe_index->ccc_handle=ccc_handle;
    subscribe_index->value_handle=value_handle;
    subscribe_index->value=value;
    subscribe_index->notify = notify_func;

    int err = bt_gatt_subscribe(conn_data->conn, subscribe_index);
    if (err) {
        AT_BLE_PRINTF("Subscribe failed (err %d)\r\n", err);
        return err;
    } else {
        AT_BLE_PRINTF("Subscribed\r\n");
        return 0;
    }

}

int at_ble_unsubscribe(int idx, int value_handle)
{
    struct ble_conn_data *conn_data = NULL;
    struct bt_gatt_subscribe_params *subscribe_index = NULL;

    conn_data = ble_conn_data_get_by_idx(idx);
    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED||conn_data->role != BT_CONN_ROLE_MASTER)
        return -EINVAL;
    if(!ble_disc_srv_handle_check(value_handle,idx))
    {
        AT_BLE_PRINTF("Unsubscribe failed (err: Invalid handle)\r\n");
        return -EINVAL;
    }
    // Iterate through the subscriptions array to find an available entry
    for (int i = 0; i < sizeof(subscribe_params) / sizeof(subscribe_params[0]); i++) {
        if(subscribe_params[i].value_handle == value_handle){
            subscribe_index = &subscribe_params[i];
            break;
        }
    } 

    if(subscribe_index == NULL){
        AT_BLE_PRINTF("Unsubscribe failed (err: Invalid handle)\r\n");
        return -EINVAL;
    }

    int err = bt_gatt_unsubscribe(conn_data->conn, subscribe_index);
    if (err) {
        AT_BLE_PRINTF("Unsubscribe failed (err %d)\r\n", err);
        return err;
    } else {
        AT_BLE_PRINTF("Unsubscribe success\r\n");
        return 0;
    }
}
static void ble_write_func(struct bt_conn *conn, u8_t err, struct bt_gatt_write_params *params)
{
	AT_BLE_PRINTF("Write complete: err %u \r\n", err);

	(void)memset(&g_ble_write_params, 0, sizeof(g_ble_write_params));
}

int at_ble_gattc_service_write(int idx, int srv_idx, int char_idx, void * buffer, int length)
{
    struct ble_conn_data *conn_data = NULL;
    struct ble_discover_data *discover_data = NULL;
    int index = char_idx - 1;
    u8_t *gatt_write_buf = NULL;
    int err = 0;

    conn_data = ble_conn_data_get_by_idx(idx);
    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED || conn_data->role != BT_CONN_ROLE_MASTER)
        return 0;

    discover_data = ble_disc_srv_get_char(srv_idx,idx);
    if (discover_data == NULL)
        return 0;

    if (!discover_data->disc_char[index].valid)
         return 0;

    if (discover_data->disc_char[index].char_props & BLE_GATT_CHAR_PROP_WRITE_WITHOUT_RESP) {
        gatt_write_buf = k_malloc(length);
        if(!gatt_write_buf) {
            AT_BLE_PRINTF("Failed to alloc buffer for the data\r\n");
            return -1;
        }
        memcpy(gatt_write_buf, buffer, length);

        err = bt_gatt_write_without_response(conn_data->conn, discover_data->disc_char[index].char_value_handle, gatt_write_buf, length, 0);
        k_free(gatt_write_buf);

        if (err) {
            AT_BLE_PRINTF("Write failed (err %d)\r\n", err);
            return -1;
        } else {
            AT_BLE_PRINTF("Write pending\r\n");
            return length;
        }
    } else if (discover_data->disc_char[index].char_props & BLE_GATT_CHAR_PROP_WRITE) {
        gatt_write_buf = k_malloc(length);
        if(!gatt_write_buf) {
            AT_BLE_PRINTF("Failed to alloc buffer for the data\r\n");
            return -1;
        }
        memcpy(gatt_write_buf, buffer, length);

        g_ble_write_params.handle = discover_data->disc_char[index].char_value_handle;
        g_ble_write_params.offset = 0;
        g_ble_write_params.length = length;
        g_ble_write_params.data = gatt_write_buf;
        g_ble_write_params.length = length;
        g_ble_write_params.func = ble_write_func;

        err = bt_gatt_write(conn_data->conn, &g_ble_write_params);
        k_free(gatt_write_buf);

        if (err) {
            AT_BLE_PRINTF("Write failed (err %d)\r\n", err);
            return -1;
        } else {
            AT_BLE_PRINTF("Write pending\r\n");
            return length;
        }
    } else {
        return 0;
    }
}
static void ble_read_callback(int idx, int srv_idx, int char_idx, void *buf, u16_t len)
{
    char *data = (char *)at_malloc(32 + len);
    int data_len = 0;
    if (!data) {
        AT_BLE_PRINTF("ble_read_callback malloc failed\r\n");
        return;
    }

    data_len = sprintf(data, "+BLE:GATTREAD:%d,%d,%d,%d,", idx,srv_idx, char_idx, len);
    memcpy(data + data_len, buf, len);
    data_len += len;
    memcpy(data + data_len, "\r\n", 2);
    data_len += 2;
    AT_CMD_DATA_SEND(data, data_len);

    at_free(data);
}

static u8_t ble_read_func(struct bt_conn *conn, u8_t err, struct bt_gatt_read_params *params, const void *data, u16_t length)
{
    AT_BLE_PRINTF("AT_BLE_PRINTF complete: err %u length %u \r\n", err, length);

    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);

    if (!conn_data)
        return;
    ble_read_callback( conn_data->idx, g_ble_read_service_index, g_ble_read_char_index, (void *)data, length);
    g_ble_read_finish = 1;

    if (!data) {
        (void)memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }

    return BT_GATT_ITER_STOP;
}

int at_ble_gattc_service_read(int idx, int srv_idx, int char_idx, int timeout)
{
    struct ble_conn_data *conn_data = NULL;
    struct ble_discover_data *discover_data = NULL;
    int index = char_idx - 1;
    int err = 0;
    uint64_t start_time = at_current_ms_get();

    conn_data = ble_conn_data_get_by_idx(idx);
    if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED || conn_data->role != BT_CONN_ROLE_MASTER)
        return 0;

    discover_data = ble_disc_srv_get_char(srv_idx,idx);
    if (discover_data == NULL)
        return 0;

    if (!discover_data->disc_char[index].valid)
        return 0;

    if (discover_data->disc_char[index].char_props & BLE_GATT_CHAR_PROP_READ) {
        g_ble_read_finish = 0;
        g_ble_read_service_index = srv_idx;
        g_ble_read_char_index = char_idx;

        g_ble_read_params.single.handle = discover_data->disc_char[index].char_value_handle;
        g_ble_read_params.single.offset = 0;
        g_ble_read_params.func = ble_read_func;
        g_ble_read_params.handle_count = 1;

        err = bt_gatt_read(conn_data->conn, &g_ble_read_params);
        if (err) {
            AT_BLE_PRINTF("Read failed (err %d)\r\n", err);
            return 0;
        } else {
            AT_BLE_PRINTF("Read pending\r\n");
            while(at_current_ms_get() - start_time < timeout) {
                if (g_ble_read_finish)
                   return 1;
                vTaskDelay(50);
            }
            return 0;
        }
    } else {
        return 0;
    }
}

static void at_bt_gatt_mtu_changed_cb(struct bt_conn *conn, int mtu)
{
    struct ble_conn_data *conn_data = ble_conn_data_get_by_conn(conn);
    if (!conn_data)
        return;
    at_response_string("+BLE:MTUSIZE:%d,%d\r\n",conn_data->idx,bt_gatt_get_mtu(conn));
}

#if defined(CONFIG_BT_BAS_SERVER)
#include "bas.h"

int at_ble_register_bas(void)
{
    if(g_ble_role==BLE_SERVER||g_ble_role==BLE_DUALMODE)
    {
        bas_init();
        return 0;
    }
    else
    {
        return -1;
    }

}

int at_ble_unregister_bas(void)
{
    if(g_ble_role==BLE_SERVER||g_ble_role==BLE_DUALMODE)
    {
        bas_deinit();
        return 0;
    }
    else
    {
        return -1;
    }

}

int at_ble_get_battery_level(void)
{
    if(g_ble_role == BLE_SERVER||g_ble_role == BLE_DUALMODE)
    {
        return bt_gatt_bas_get_battery_level();
    }
    else
    {
        return -1;
    }

}

int at_ble_set_battery_level(int idx, int lvl)
{
    if(g_ble_role==BLE_SERVER||g_ble_role==BLE_DUALMODE)
    {
        struct ble_conn_data *conn_data = NULL;

        conn_data = ble_conn_data_get_by_idx(idx);
        if (conn_data == NULL || conn_data->state != BLE_CONN_STATE_CONNECTED)
            return -EINVAL;

        return bt_gatt_bas_set_battery_level(conn_data->conn,lvl);
    }
    else
    {
        return -EINVAL;
    }

}
#endif

#if defined (CONFIG_BT_IAS_SERVER)
#include "ias.h"
static void ias_recv(struct bt_conn *conn, void *buf, u8_t len)
{
    at_response_string("+BLE:IAS:%s\r\n",bt_hex(buf,len));
}

int at_ble_register_ias(void)
{
    if(g_ble_role==BLE_SERVER||g_ble_role==BLE_DUALMODE)
    {
        ias_init();
        ias_register_recv_callback(ias_recv);
        return 0;
    }
    else
    {
        return -1;
    }

}

int at_ble_unregister_ias(void)
{
    if(g_ble_role==BLE_SERVER||g_ble_role==BLE_DUALMODE)
    {
        ias_deinit();
        return 0;
    }
    else
    {
        return -1;
    }

}
#endif

#if defined (CONFIG_BT_DIS_SERVER)

#include "dis.h"

int at_ble_register_dis(int at_vid_src, int at_vid, int at_pid, int at_pnp_ver)
{
    if(g_ble_role==BLE_SERVER||g_ble_role==BLE_DUALMODE)
    {
        dis_init(at_vid_src,at_vid,at_pid,at_pnp_ver);
        return 0;
    }
    else
    {
        return -1;
    }

}

int at_ble_unregister_dis(void)
{
    if(g_ble_role==BLE_SERVER||g_ble_role==BLE_DUALMODE)
    {
        dis_deinit();
        return 0;
    }
    else
    {
        return -1;
    }

}

int at_ble_dis_set(char* dis_name, char* dis_value, int dis_value_len)
{
    if(g_ble_role==BLE_SERVER||g_ble_role==BLE_DUALMODE)
    {
        return dis_settings_init(dis_name,dis_value,dis_value_len);
    }
    else
    {
        return -EINVAL;
    }

}
#endif
int at_ble_init(int role)
{
    if (g_ble_is_inited == 0&&role != BLE_DISABLE) {

       if (!atomic_test_bit(bt_dev.flags, BT_DEV_ENABLE))
        {
            // Initialize BLE controller
            #if defined(BFLB_undef) || defined(BFLB_undef)
            ble_controller_init(configMAX_PRIORITIES - 1);
            #else
            btble_controller_init(configMAX_PRIORITIES - 1);
            #endif
            // Initialize BLE Host stack
            hci_driver_init();
            bt_enable(bt_enable_cb);  
        }
        g_ble_is_inited = 1;
    }
    else {
        if(role ==BLE_DISABLE)
        {
            if (atomic_test_bit(bt_dev.flags, BT_DEV_ENABLE))
            {

                if(g_ble_role==BLE_SERVER)
                {
                    ble_gatts_srv_clean();
                    at_ble_gatts_service_register(0);

                }

                if(g_ble_role == BLE_CLIENT)
                {
                    ble_disc_srv_clean();
                }

                if(g_ble_role == BLE_DUALMODE)
                {
                    ble_gatts_srv_clean();
                    ble_disc_srv_clean();
                    at_ble_gatts_service_register(0);
                }

                if(bt_force_disable())
                {
                    return -1;
                }
                else
                {
                    at_ble_config_default();
                    g_ble_is_inited = 0;
                }
            }
        }
        else
        {
            return -1;
        }
        
    }

    g_ble_role = role;
    
    if(g_ble_role == BLE_CLIENT)
    {
        if(g_ble_disc_srv == NULL)
            g_ble_disc_srv = at_malloc(sizeof(struct ble_client_disc)*BLE_CONN_MAX_NUM);
        
        memset(g_ble_disc_srv, 0, sizeof(struct ble_client_disc)*BLE_CONN_MAX_NUM);
        bt_gatt_register_notification_callback(ble_notification_all_cb);
    }

    if(g_ble_role == BLE_SERVER)
    {
        if(g_ble_srv_data == NULL)
            g_ble_srv_data = at_malloc(sizeof(struct ble_srv_data)*BLE_SRV_MAX_NUM);
        memset(g_ble_srv_data,0,sizeof(struct ble_srv_data)*BLE_SRV_MAX_NUM);

    }

    if(g_ble_role == BLE_DUALMODE)
    {
        if(g_ble_disc_srv == NULL)
            g_ble_disc_srv = at_malloc(sizeof(struct ble_client_disc)*BLE_CONN_MAX_NUM);
        
        memset(g_ble_disc_srv, 0, sizeof(struct ble_client_disc)*BLE_CONN_MAX_NUM);
        bt_gatt_register_notification_callback(ble_notification_all_cb);

        if(g_ble_srv_data == NULL)
            g_ble_srv_data = at_malloc(sizeof(struct ble_srv_data)*BLE_SRV_MAX_NUM);
        memset(g_ble_srv_data,0,sizeof(struct ble_srv_data)*BLE_SRV_MAX_NUM);
    }

    #if defined(BFLB_BLE_MTU_CHANGE_CB)
    bt_gatt_register_mtu_callback(at_bt_gatt_mtu_changed_cb);
    #endif

    return 0;
}

int at_ble_start(void)
{

    return 0;
}

int at_ble_stop(void)
{
    return 0;
}

