#include <string.h>

#include <ble/ble.h>
#include <xutils/debug.h>
#include <msp/kernel.h>
#include <string.h>

#include <bluetooth.h>
#include <conn.h>
// #include <hci.h>
#include <addr.h>

#include "conn_internal.h"

#include <gatt.h>
#include <settings.h>
#include <hci_core.h>
#include <log.h>

#include "btble_lib_api.h"
#include "hci_driver.h"
#define EVENT_BLE_BASE
#define TAG "BLE"
#define BLE_LOGD(fmt, ...) LOGD(TAG, "[%s]"fmt,__FUNCTION__,  ##__VA_ARGS__)
#define BLE_LOGE(fmt, ...) LOGE(TAG, "[%s]"fmt,__FUNCTION__,  ##__VA_ARGS__)
#define tlog(fmt, ...)      //BT_WARN(fmt, ##__VA_ARGS__)

#define NAME_LEN        30

typedef struct _ble_dev_t {
    uint8_t is_init;
    msp_slist_t cb_list;
    uint8_t io_cap;
    init_param_t init;
} ble_dev_t;

struct _params_pool_t {
    struct bt_gatt_write_params write_params[3];
    union {
        struct bt_uuid_16 uuid16;
        struct bt_uuid_32 uuid32;
        struct bt_uuid_128 uuid128;
    } uuid;
    struct bt_gatt_discover_params disc_params[1];
    struct bt_gatt_read_params read_params[4];

} ble_param_pool = {0};

ble_dev_t ble_dev = {0};

typedef struct _ble_attr_notify_t {
    struct bt_conn *conn;
    const uint8_t *data;
    uint16_t len;
    int err;
} ble_attr_notify_t;

void ble_adv_timeout_cb();

struct k_sem notify_sem;
struct k_sem indicate_sem;
struct k_sem write_sem;
struct k_sem read_sem;
static uint8_t ble_init_cb = 0;

static inline uuid_t *uuid_covert(uuid_t *uuid, const struct bt_uuid *btuuid)
{
    uuid->type = btuuid->type;

    if (btuuid->type == BT_UUID_TYPE_16) {
        UUID16(uuid) = BT_UUID_16(btuuid)->val;
    } else if (btuuid->type == BT_UUID_TYPE_32) {
        UUID32(uuid)  = BT_UUID_32(btuuid)->val;
    } else if (btuuid->type == BT_UUID_TYPE_128) {
        memcpy(UUID128(uuid), BT_UUID_128(btuuid)->val, 16);
    }

    return uuid;
}

static inline struct bt_uuid *btuuid_covert(struct bt_uuid *btuuid, const uuid_t *uuid)
{
    btuuid->type = uuid->type;

    if (uuid->type == UUID_TYPE_16) {
        BT_UUID_16(btuuid)->val =  UUID16(uuid);
    } else if (uuid->type == UUID_TYPE_32) {
        BT_UUID_32(btuuid)->val = UUID32(uuid);
    } else if (uuid->type == UUID_TYPE_128) {
        memcpy(BT_UUID_128(btuuid)->val, UUID128(uuid), 16);
    }

    return btuuid;
}

uint8_t UUID_EQUAL(uuid_t *uuid1, uuid_t *uuid2)
{
    if (UUID_TYPE(uuid1) != UUID_TYPE(uuid2)) {
        return false;
    }

    if (uuid1->type == BT_UUID_TYPE_16) {
        return (0 == memcmp(&((struct bt_uuid_16 *)uuid1)->val, &((struct bt_uuid_16 *)uuid2)->val, UUID_LEN(uuid1)));
    } else if (uuid1->type == BT_UUID_TYPE_32) {
        return (0 == memcmp(&((struct bt_uuid_32 *)uuid1)->val, &((struct bt_uuid_32 *)uuid2)->val, UUID_LEN(uuid1)));
    } else if (uuid1->type == BT_UUID_TYPE_128) {
        return (0 == memcmp(&((struct bt_uuid_128 *)uuid1)->val, &((struct bt_uuid_128 *)uuid2)->val, UUID_LEN(uuid1)));
    }

    return false;
}

uint8_t *get_uuid_val(uuid_t *uuid)
{
    if (uuid->type == BT_UUID_TYPE_16) {
        return (uint8_t *) & ((struct bt_uuid_16 *)uuid)->val;
    } else if (uuid->type == BT_UUID_TYPE_32) {
        return (uint8_t *) & ((struct bt_uuid_32 *)uuid)->val;
    } else if (uuid->type == BT_UUID_TYPE_128) {
        return (uint8_t *) & ((struct bt_uuid_128 *)uuid)->val;
    } else {
        return NULL;
    }
}

static int ble_stack_event_callback(ble_event_en event, void *event_data, uint32_t event_len)
{
    msp_slist_t               *tmp;
    ble_event_cb_t *node;


    msp_slist_for_each_entry_safe(&ble_dev.cb_list, tmp, node, ble_event_cb_t, next) {
        if (node->callback) {
            node->callback(event, event_data);
        }
    }

    return 0;
}

static void connected(struct bt_conn *conn, u8_t err)
{
    evt_data_gap_conn_change_t event_data;
    event_data.conn_handle = bt_conn_index(conn);

    if (err) {
        event_data.err = err;
        event_data.connected = DISCONNECTED;
    } else {
        event_data.err = 0;
        event_data.connected = CONNECTED;
    }

    ble_stack_event_callback(EVENT_GAP_CONN_CHANGE, &event_data, sizeof(event_data));
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
    evt_data_gap_conn_change_t event_data;

    if (reason == BT_HCI_ERR_ADV_TIMEOUT) {
        ble_adv_timeout_cb();
    } else {
        event_data.conn_handle = bt_conn_index(conn);
        event_data.connected = DISCONNECTED;
        event_data.err = reason;
        ble_stack_event_callback(EVENT_GAP_CONN_CHANGE, &event_data, sizeof(event_data));

        memset(&ble_param_pool, 0, sizeof(ble_param_pool));
    }
}

static bool le_param_req_cb(struct bt_conn *conn, struct bt_le_conn_param *param)
{
    evt_data_gap_conn_param_req_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.param = *(conn_param_t *)param;
    event_data.accept = 1;
    ble_stack_event_callback(EVENT_GAP_CONN_PARAM_REQ, &event_data, sizeof(event_data));

    if (event_data.accept) {
        return true;
    } else {
        return false;
    }
}

static void le_param_update_cb(struct bt_conn *conn, u16_t interval, u16_t latency, u16_t timeout)
{
    evt_data_gap_conn_param_update_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.interval = interval;
    event_data.latency = latency;
    event_data.timeout = timeout;
    ble_stack_event_callback(EVENT_GAP_CONN_PARAM_UPDATE, &event_data, sizeof(event_data));
}

#ifdef CONFIG_BT_SMP
static void security_changed_cb(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
    evt_data_gap_security_change_t  event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.level = level;
    event_data.err = err;
    ble_stack_event_callback(EVENT_GAP_CONN_SECURITY_CHANGE, &event_data, sizeof(event_data));
}

static void identity_address_resolved(struct bt_conn *conn,
                                      const bt_addr_le_t *rpa,
                                      const bt_addr_le_t *identity)
{
    evt_data_smp_identity_address_t event_data;

    event_data.conn_handle = bt_conn_index(conn);
    memcpy(&event_data.rpa, rpa, sizeof(dev_addr_t));
    memcpy(&event_data.identity_addr, identity, sizeof(dev_addr_t));
    ble_stack_event_callback(EVENT_SMP_IDENTITY_ADDRESS_GET, &event_data, sizeof(event_data));
}

#endif

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char passkey_str[7] = {0};
    evt_data_smp_passkey_display_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));
    snprintf(passkey_str, 7, "%06u", passkey);

    event_data.conn_handle = bt_conn_index(conn);
    event_data.passkey = passkey_str;
    ble_stack_event_callback(EVENT_SMP_PASSKEY_DISPLAY, &event_data, sizeof(event_data));
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    char passkey_str[7] = {0};
    evt_data_smp_passkey_confirm_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));
    snprintf(passkey_str, 7, "%06u", passkey);

    event_data.conn_handle = bt_conn_index(conn);
    event_data.passkey = passkey_str;
    ble_stack_event_callback(EVENT_SMP_PASSKEY_CONFIRM, &event_data, sizeof(event_data));
}

static void auth_passkey_entry(struct bt_conn *conn)
{
    evt_data_smp_passkey_enter_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    ble_stack_event_callback(EVENT_SMP_PASSKEY_ENTER, &event_data, sizeof(event_data));
}

static void auth_cancel(struct bt_conn *conn)
{
    evt_data_smp_cancel_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    ble_stack_event_callback(EVENT_SMP_CANCEL, &event_data, sizeof(event_data));
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
    evt_data_smp_pairing_confirm_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    ble_stack_event_callback(EVENT_SMP_PAIRING_CONFIRM, &event_data, sizeof(event_data));
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
    evt_data_smp_pairing_complete_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    event_data.bonded = bonded;
    event_data.err = 0;
    ble_stack_event_callback(EVENT_SMP_PAIRING_COMPLETE, &event_data, sizeof(event_data));
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    evt_data_smp_pairing_complete_t event_data;

    memcpy(&event_data.peer_addr, bt_conn_get_dst(conn), sizeof(dev_addr_t));

    event_data.conn_handle = bt_conn_index(conn);
    event_data.err = reason;
    ble_stack_event_callback(EVENT_SMP_PAIRING_COMPLETE, &event_data, sizeof(event_data));
}

void ble_event_init_done(int16_t err)
{
    if (!err)
    {
        ble_dev.is_init = 1;
    }

    evt_data_stack_init_t event_data;
    event_data.err = err;

    ble_stack_event_callback(EVENT_STACK_INIT, &event_data, sizeof(event_data));
}

void ble_event_adv_start(int16_t err)
{
    evt_data_gap_adv_start_t event_data;
    event_data.err = err;

    ble_stack_event_callback(EVENT_GAP_ADV_START, &event_data, sizeof(event_data));
}

void ble_event_adv_stop(int16_t err)
{
    evt_data_gap_adv_stop_t event_data;
    event_data.err = err;

    ble_stack_event_callback(EVENT_GAP_ADV_STOP, &event_data, sizeof(event_data));
}

void ble_event_scan_start(int16_t err)
{
    evt_data_gap_scan_start_t event_data;
    event_data.err = err;

    ble_stack_event_callback(EVENT_GAP_SCAN_START, &event_data, sizeof(event_data));
}

void ble_event_scan_stop(int16_t err)
{
    evt_data_gap_scan_stop_t event_data;
    event_data.err = err;

    ble_stack_event_callback(EVENT_GAP_SCAN_STOP, &event_data, sizeof(event_data));
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
    .le_param_req = le_param_req_cb,
    .le_param_updated = le_param_update_cb,
#ifdef CONFIG_BT_SMP
    .identity_resolved = identity_address_resolved,
    .security_changed = security_changed_cb,
#endif
};

static struct bt_conn_auth_cb auth_callbacks = {
    .cancel = auth_cancel,
    .pairing_failed = auth_pairing_failed,
    .pairing_complete = auth_pairing_complete,
};

static void _bt_enable_cb(int err)
{
    tlog("err (%d)",err);
    if(!err){
        bt_conn_cb_register(&conn_callbacks);
        ble_init_cb = 1;
    }

    ble_event_init_done(err);
}

int ble_stack_init(init_param_t *param)
{
    int ret = BT_STACK_STATUS_SUCCESS;
    tlog("");
    if (param == NULL) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (ble_dev.is_init) {
        return -BT_STACK_STATUS_EALREADY;
    }

    ble_dev.init = *param;

    if (ble_dev.init.dev_addr) {
        bt_addr_le_t addr = *(bt_addr_le_t *)ble_dev.init.dev_addr;

        if (addr.type == BT_ADDR_LE_RANDOM) {
            ret = bt_set_id_addr(&addr);

            if (ret) {
                ret = -BT_STACK_STATUS_EINVAL;
                return ret;
            }
        } else if (addr.type == BT_ADDR_LE_PUBLIC) {
            //extern int bt_set_bdaddr(const bt_addr_le_t *addr);
            //ret = bt_set_bdaddr(&addr);

            if (ret) {
                ret = -BT_STACK_STATUS_EINVAL;
                return ret;
            }
        } else if (addr.type == BT_ADDR_LE_PUBLIC_ID) {
        } else if (addr.type == BT_ADDR_LE_RANDOM_ID) {
        } else {
            ret = -BT_STACK_STATUS_EINVAL;
            return ret;
        }
    }

    k_sem_init(&notify_sem,1,1);
    k_sem_init(&indicate_sem,1,1);
    k_sem_init(&read_sem,1,1);
    k_sem_init(&write_sem,1,1);

    btble_controller_init(configMAX_PRIORITIES);
    hci_driver_init();
    bt_enable(_bt_enable_cb);

    while(!ble_init_cb)
    {
        msp_msleep(1);
    }

    if (ble_dev.init.dev_name) {
        ret = bt_set_name(ble_dev.init.dev_name);

        if (ret) {
            ret = -BT_STACK_STATUS_EINVAL;
            return ret;
        }
    }

    msp_slist_init(&ble_dev.cb_list);
    ble_init_cb = 0;
    return ret;
}

int ble_stack_iocapability_set(uint8_t io_cap)
{
    int ret = 0;
    tlog("");
    int data = (io_cap & (IO_CAP_IN_NONE | IO_CAP_IN_YESNO | IO_CAP_IN_KEYBOARD));

    if ((data != IO_CAP_IN_NONE) && (data != IO_CAP_IN_YESNO) && (data != IO_CAP_IN_KEYBOARD)) {
        return -BT_STACK_STATUS_EINVAL;
    }

    int sdata = (io_cap & (IO_CAP_OUT_DISPLAY | IO_CAP_OUT_NONE));

    if ((sdata != IO_CAP_OUT_DISPLAY) && (sdata != IO_CAP_OUT_NONE)) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if ((io_cap & IO_CAP_IN_NONE) && (io_cap & IO_CAP_OUT_DISPLAY)) {
        auth_callbacks.passkey_display = auth_passkey_display;
        auth_callbacks.cancel = auth_cancel;
        auth_callbacks.pairing_confirm = auth_pairing_confirm;
        auth_callbacks.pairing_failed = auth_pairing_failed;
        auth_callbacks.pairing_complete = auth_pairing_complete;
    } else if ((io_cap & IO_CAP_IN_YESNO) && (io_cap & IO_CAP_OUT_DISPLAY)) {
        auth_callbacks.passkey_display = auth_passkey_display;
        auth_callbacks.passkey_confirm = auth_passkey_confirm;
        auth_callbacks.cancel = auth_cancel;
        auth_callbacks.pairing_confirm = auth_pairing_confirm;
        auth_callbacks.pairing_failed = auth_pairing_failed;
        auth_callbacks.pairing_complete = auth_pairing_complete;
    } else if ((io_cap & IO_CAP_IN_KEYBOARD) && (io_cap & IO_CAP_OUT_DISPLAY)) {
        auth_callbacks.passkey_display = auth_passkey_display;
        auth_callbacks.passkey_entry = auth_passkey_entry;
        auth_callbacks.passkey_confirm = auth_passkey_confirm;
        auth_callbacks.cancel = auth_cancel;
        auth_callbacks.pairing_confirm = auth_pairing_confirm;
        auth_callbacks.pairing_failed = auth_pairing_failed;
        auth_callbacks.pairing_complete = auth_pairing_complete;
    } else if ((io_cap & IO_CAP_IN_KEYBOARD) && (io_cap & IO_CAP_OUT_NONE)) {
        auth_callbacks.passkey_entry = auth_passkey_entry;
        auth_callbacks.passkey_confirm = auth_passkey_confirm;
        auth_callbacks.cancel = auth_cancel;
        auth_callbacks.pairing_confirm = auth_pairing_confirm;
        auth_callbacks.pairing_failed = auth_pairing_failed;
        auth_callbacks.pairing_complete = auth_pairing_complete;
    }

    if (auth_callbacks.cancel != NULL) {
        ret = bt_conn_auth_cb_register(&auth_callbacks);

        if (ret) {
            return ret;
        }
    }

    ble_dev.io_cap = io_cap;
    return ret;
}

int ble_stack_event_register(ble_event_cb_t *callback)
{
    tlog("");
    if (!ble_dev.is_init) {
        return -BT_STACK_STATUS_EPERM;
    }

    if (NULL == callback) {
        return -BT_STACK_STATUS_EINVAL;
    }

    msp_slist_t *tmp;
    ble_event_cb_t *node;

    msp_slist_for_each_entry_safe(&ble_dev.cb_list, tmp, node, ble_event_cb_t, next) {
        if (node == callback) {
            return 0;
        }
    }

    msp_slist_add(&callback->next, &ble_dev.cb_list);
    return 0;
}

int ble_stack_adv_start(adv_param_t *param)
{
    tlog("");
    struct bt_le_adv_param p = {0};
    bt_addr_le_t *peer_addr_p = NULL;
    int err;

    if (!ble_dev.is_init) {
        return -BT_STACK_STATUS_EPERM;
    }

    if (param == NULL) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if ((param->ad == NULL || (param->ad != NULL && param->ad_num == 0))
        || ((param->sd == NULL && param->sd_num != 0) || (param->sd != NULL && param->sd_num == 0))
       ) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (param->channel_map > ADV_DEFAULT_CHAN_MAP) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (param->filter_policy > ADV_FILTER_POLICY_ALL_REQ) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (param->type == ADV_IND || param->type == ADV_DIRECT_IND || param->type == ADV_DIRECT_IND_LOW_DUTY) {
        p.options |= BT_LE_ADV_OPT_CONNECTABLE;
    } else if (param->type == ADV_NONCONN_IND || param->type == ADV_SCAN_IND) {
        p.options |= BT_LE_ADV_OPT_NONE;
    } else {
        return -BT_STACK_STATUS_EINVAL;
    }

#if defined(CONFIG_BT_EXT_ADV)
    if (param->phy_select > (ADV_PHY_1M | ADV_PHY_2M | ADV_PHY_CODED))
    {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (param->phy_select)
    {
        p.options |= BT_LE_ADV_OPT_EXT_ADV;
    }

    if ((param->phy_select & ADV_PHY_2M) == 0)
    {
        p.options |= BT_LE_ADV_OPT_NO_2M;
    }

    if ((param->phy_select & ADV_PHY_CODED) == ADV_PHY_CODED)
    {
        p.options |= BT_LE_ADV_OPT_CODED;
    }
#endif
    if (param->type == ADV_DIRECT_IND_LOW_DUTY) {
        p.options |= BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY;
    }

    p.options |= BT_LE_ADV_OPT_ONE_TIME;

    if (ble_dev.init.dev_name) {
        p.options |= BT_LE_ADV_OPT_USE_NAME;
    }

    if (param->filter_policy == ADV_FILTER_POLICY_ALL_REQ) {
        p.options |= (BT_LE_ADV_OPT_FILTER_SCAN_REQ | BT_LE_ADV_OPT_FILTER_CONN);
    } else if (param->filter_policy == ADV_FILTER_POLICY_CONN_REQ) {
        p.options |= BT_LE_ADV_OPT_FILTER_CONN;
    } else if (param->filter_policy == ADV_FILTER_POLICY_SCAN_REQ) {
        p.options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
    }

    p.options |= BT_LE_ADV_OPT_USE_IDENTITY;

    p.id = 0;
    p.interval_min = param->interval_min;
    p.interval_max = param->interval_max;
    //p.channel_map = param->channel_map;
    extern u8_t adv_ch_map;
    if(param->channel_map != 0){
        adv_ch_map = param->channel_map;
    }

    if (param->type == ADV_DIRECT_IND_LOW_DUTY || param->type == ADV_DIRECT_IND) {
        if (!bt_addr_le_cmp((const bt_addr_le_t *)&param->direct_peer_addr, BT_ADDR_LE_ANY)) {
            return -BT_STACK_STATUS_EINVAL;
        }

        param->ad = NULL;
        param->ad_num = 0;
        param->sd = NULL;
        param->sd_num = 0;
    }

    bt_addr_le_t peer_addr = {0};

    if (param->type == ADV_DIRECT_IND_LOW_DUTY || param->type == ADV_DIRECT_IND) {
        //p.peer = &peer_addr;
        peer_addr.type = param->direct_peer_addr.type;
        memcpy(peer_addr.a.val, param->direct_peer_addr.val, 6);
        peer_addr_p = &peer_addr;
    }

    err = bt_le_adv_start_internal(&p, (struct bt_data *)param->ad, param->ad_num,
                           (struct bt_data *)param->sd, param->sd_num,peer_addr_p);
    ble_event_adv_start(err);

    return err;
}

int ble_stack_adv_stop()
{
    tlog("");
    int ret;

    if (!ble_dev.is_init) {
        return -BT_STACK_STATUS_EPERM;
    }

    ret = bt_le_adv_stop();

    ble_event_adv_stop(ret);

    return ret;
}

static bool _data_cb(struct bt_data *data, void *user_data)
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

static void _device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t evtype,
            struct net_buf_simple *buf)
{
    char le_addr[BT_ADDR_LE_STR_LEN];
    char name[30];
    evt_data_gap_dev_find_t event_data;

    (void)memset(name, 0, sizeof(name));
    bt_data_parse(buf, _data_cb, name);
    bt_addr_le_to_str(addr, le_addr, sizeof(le_addr));

    tlog("[DEVICE]: %s, AD evt type %u, RSSI %i %s",le_addr, evtype, rssi, name);

    memcpy(&event_data.dev_addr, addr, sizeof(dev_addr_t));
    event_data.adv_type = evtype;
    memcpy(event_data.adv_data, buf->data, buf->len);
    event_data.adv_len = buf->len;
    event_data.rssi = rssi;

    ble_stack_event_callback(EVENT_GAP_DEV_FIND, &event_data, sizeof(event_data));

}

int ble_stack_scan_start(const scan_param_t *param)
{
    int ret;
    struct bt_le_scan_param p = {0};
    tlog("");

    if (param == NULL) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (!ble_dev.is_init) {
        return -BT_STACK_STATUS_EPERM;
    }

    if (param->scan_filter > SCAN_FILTER_POLICY_WHITE_LIST) {
        return  -BT_STACK_STATUS_EINVAL;
    }

    p.type = param->type;
    p.interval = param->interval;
    p.window = param->window;

#if defined(CONFIG_BT_EXT_ADV)
    if (param->phy_select > (SCAN_PHY_1M | SCAN_PHY_2M | SCAN_PHY_CODED))
    {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (param->phy_select && (param->phy_select & SCAN_PHY_CODED) != 0)
    {
        p.options |= BT_LE_SCAN_OPT_CODED;
    }

    if (param->phy_select && (param->phy_select & SCAN_PHY_1M) == 0)
    {
        p.options |= BT_LE_SCAN_OPT_NO_1M;
    }
#endif

    p.filter_dup = param->filter_dup;

    if (param->scan_filter == SCAN_FILTER_POLICY_WHITE_LIST) {

        atomic_set_bit(bt_dev.flags, BT_DEV_SCAN_WL);
    }

    //bt_le_scan_cb_register(&ble_scan_cb);

    ret = bt_le_scan_start(&p, _device_found);
    ble_event_scan_start(ret);

    return ret;
}

int ble_stack_scan_stop()
{
    int ret;
    tlog("");

    if (!ble_dev.is_init) {
        return -BT_STACK_STATUS_EPERM;
    }

    //bt_le_scan_cb_unregister(&ble_scan_cb);

    ret = bt_le_scan_stop();
    ble_event_scan_stop(ret);

    return ret;
}

int gatt_read_handle(void *conn, const void *attr, void *buf, uint16_t len, uint16_t offset)
{
    evt_data_gatt_char_read_t event_data = {0};

    tlog("");
    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = ((const struct bt_gatt_attr *)attr)->handle;
    event_data.offset = offset;

    ble_stack_event_callback(EVENT_GATT_CHAR_READ, &event_data, sizeof(event_data));

    if (event_data.len < 0) {
        return event_data.len;
    }

    if (event_data.data && event_data.len > 0) {
        return bt_gatt_attr_read(conn, attr, buf, len, offset, event_data.data, event_data.len);
    }

    return 0;
}

int gatt_write_handle(void *conn, const void *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    evt_data_gatt_char_write_t event_data;

    tlog("");
    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = ((const struct bt_gatt_attr *)attr)->handle;
    event_data.data = buf;
    event_data.len = len;
    event_data.offset = offset;
    event_data.flag = flags;

    ble_stack_event_callback(EVENT_GATT_CHAR_WRITE, &event_data, sizeof(event_data));

    return event_data.len;
}

int gatt_cfg_write(void *conn, const void *attr, u16_t value)
{
    evt_data_gatt_char_ccc_write_t event_data;

    tlog("");
    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = ((const struct bt_gatt_attr *)attr)->handle;
    event_data.ccc_value = value;
    event_data.allow_write = true;

    ble_stack_event_callback(EVENT_GATT_CHAR_CCC_WRITE, &event_data, sizeof(event_data));

    return event_data.allow_write ? true : false;
}

int gatt_cfg_match(void *conn, const void *attr)
{
    evt_data_gatt_char_ccc_match_t event_data;

    tlog("");
    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = ((const struct bt_gatt_attr *)attr)->handle;
    event_data.is_matched = true;

    ble_stack_event_callback(EVENT_GATT_CHAR_CCC_MATCH, &event_data, sizeof(event_data));

    return event_data.is_matched ? true : false;
}

void gatt_cfg_changed(const void *attr, u16_t value)
{
    evt_data_gatt_char_ccc_change_t event_data;

    tlog("");
    event_data.char_handle = ((const struct bt_gatt_attr *)attr)->handle;
    event_data.ccc_value = value;

    ble_stack_event_callback(EVENT_GATT_CHAR_CCC_CHANGE, &event_data, sizeof(event_data));
}

int uuid_compare(const uuid_t *uuid1, const uuid_t *uuid2)
{
    return bt_uuid_cmp((struct bt_uuid *)uuid1, (struct bt_uuid *)uuid2);
}

int gatt_attr_read_service(void *conn, const void *attr, void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read_service(conn, attr, buf, len, offset);
}

int gatt_attr_read_chrc(void *conn, const void *attr, void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read_chrc(conn, attr, buf, len, offset);
}

int gatt_attr_read_ccc(void *conn, const void *attr, void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read_ccc(conn, attr, buf, len, offset);
}

int gatt_attr_write_ccc(void *conn, const void *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    return bt_gatt_attr_write_ccc(conn, attr, buf, len ,offset, flags);
}

int gatt_attr_read_cep(void *conn, const void *attr, void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read_cep(conn, attr, buf, len,offset);
}

int ble_stack_gatt_registe_service(gatt_service *s, gatt_attr_t attrs[], uint16_t attr_num)
{
    int ret = 0;

    struct bt_gatt_service  *ss;

    ss = (struct bt_gatt_service *)s;

    tlog("");
    if (!ble_dev.is_init) {
        return -BT_STACK_STATUS_EPERM;
    }

    if (attrs == NULL || attr_num == 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    ss->attr_count = attr_num;
    ss->attrs = (struct bt_gatt_attr *)attrs;

    ret = bt_gatt_service_register(ss);

    if (ret < 0) {
        return ret;
    }

    return ss->attrs[0].handle;
}

int ble_stack_gatt_service_handle(const gatt_service_static *service)
{
    tlog("");

    if (!ble_dev.is_init) {
        return -BT_STACK_STATUS_EPERM;
    }

    if (NULL == service) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (service->attrs[0].handle == 0){
        extern u16_t find_static_attr(const struct bt_gatt_attr *attr);
        return find_static_attr((const struct bt_gatt_attr *)service->attrs);
    }

    return service->attrs[0].handle + service->attr_count;
}

static void ble_gatt_indicate_cb(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 u8_t err)
{
    evt_data_gatt_indicate_cb_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.char_handle = attr->handle;
    event_data.err = err;

    ble_stack_event_callback(EVENT_GATT_INDICATE_CB, &event_data, sizeof(event_data));
    k_sem_give(&indicate_sem);
}


static uint8_t ble_gatt_attr_indicate(const struct bt_gatt_attr *attr, void *user_data)
{
    ble_attr_notify_t *notify = (ble_attr_notify_t *)user_data;
    static struct bt_gatt_indicate_params param = {0};
    param.attr = attr;
    param.func = ble_gatt_indicate_cb;
    param.data = notify->data;
    param.len = notify->len;

    notify->err = bt_gatt_indicate(notify->conn, &param);

    return BT_GATT_ITER_STOP;
}

static void _notify_cb(struct bt_conn *conn, void *user_data)
{
    evt_data_gatt_notify_t *event_data = (evt_data_gatt_notify_t *)user_data;

    tlog("conn_hdl(0x%x),char_hdl(0x%x),data(%s)",event_data->conn_handle,
            event_data->char_handle,bt_hex(event_data->data,event_data->len));
    ble_stack_event_callback(EVENT_GATT_NOTIFY, event_data, sizeof(*event_data));
    if(event_data->data){
        k_free(event_data->data);
        event_data->data = NULL;
    }

    k_sem_give(&notify_sem);
}
extern const struct bt_gatt_attr *find_attr(uint16_t handle);
int ble_stack_gatt_notificate(int16_t conn_handle, uint16_t char_handle, const uint8_t *data, uint16_t len)
{
    tlog("");
    struct bt_conn *conn = NULL;
    struct bt_gatt_notify_params params = {0};
    static evt_data_gatt_notify_t cb;
    int err;

    if(k_sem_take(&notify_sem,K_SECONDS(3)) < 0){
        tlog("waiting notify sem timeout");
        return BT_STACK_STATUS_EIO;
    }

    conn = bt_conn_lookup_handle(conn_handle);
    if(!conn){
        tlog("Invalid connection handle");
        return BT_STACK_STATUS_EINVAL;
    }

    cb.conn_handle = conn_handle;
    cb.char_handle = char_handle;
    cb.len = len;
    cb.data = (uint8_t*)k_malloc(len*2);
    if(!cb.data){
        tlog("failed to alloc");
        return BT_STACK_STATUS_EINVAL;
    }
    memcpy(cb.data,data,len);

    params.data = data;
    params.len = len;
    params.func = _notify_cb;
    params.attr = find_attr(char_handle);
    params.user_data = (void*)&cb;

    err = bt_gatt_notify_cb(conn, &params);
    if(err){
        tlog("err (%d)",err);
        k_sem_give(&notify_sem);
    }

    return err == 0 ? BT_STACK_STATUS_SUCCESS : BT_STACK_STATUS_EIO;

}

int ble_stack_gatt_indicate(int16_t conn_handle, int16_t char_handle, const uint8_t *data, uint16_t len)
{
    ble_attr_notify_t notify;
    struct bt_conn *conn;

    tlog("");
    if (conn_handle < 0 || data == NULL || len == 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if(k_sem_take(&indicate_sem,K_SECONDS(3)) < 0){
        tlog("waiting inicate sem timeout");
        return BT_STACK_STATUS_EIO;
    }

    conn = bt_conn_lookup_handle(conn_handle);

    if (conn == NULL) {
        return -BT_STACK_STATUS_ENOTCONN;
    }

    notify.conn = conn;
    notify.data = data;
    notify.len = len;
    notify.err = 0;

    bt_gatt_foreach_attr(char_handle, char_handle,
                         ble_gatt_attr_indicate, &notify);

    if (notify.conn) {
        bt_conn_unref(notify.conn);
    }

    return notify.err;
}

void ble_adv_timeout_cb()
{
    ble_stack_event_callback(EVENT_GAP_ADV_TIMEOUT, NULL, 0);
}

void mtu_exchange_cb(struct bt_conn *conn, u8_t err,
                     struct bt_gatt_exchange_params *params)
{
    evt_data_gatt_mtu_exchange_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.err = err;

    ble_stack_event_callback(EVENT_GATT_MTU_EXCHANGE, &event_data, sizeof(event_data));
}

int ble_stack_gatt_mtu_get(int16_t conn_handle)
{
    int ret;
    struct bt_conn *conn = NULL;

    tlog("");
    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    conn = bt_conn_lookup_handle(conn_handle);

    if (conn == NULL) {
        return -BT_STACK_STATUS_ENOTCONN;
    }

    ret = bt_gatt_get_mtu(conn);

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_gatt_mtu_exchange(int16_t conn_handle)
{
    static struct bt_gatt_exchange_params params;
    int ret;
    struct bt_conn *conn;

    tlog("");
    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    conn = bt_conn_lookup_handle(conn_handle);

    if (conn == NULL) {
        return -BT_STACK_STATUS_ENOTCONN;
    }

    params.func = mtu_exchange_cb;
    ret = bt_gatt_exchange_mtu(conn, &params);

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

static u8_t discover_func(struct bt_conn *conn,
                          const struct bt_gatt_attr *attr,
                          struct bt_gatt_discover_params *params)
{
    int event = EVENT_STACK_UNKNOWN;
    union {
        evt_data_gatt_discovery_svc_t svc;
        evt_data_gatt_discovery_inc_svc_t svc_inc;
        evt_data_gatt_discovery_char_t char_c;
        evt_data_gatt_discovery_char_des_t char_des;
        evt_data_gatt_discovery_complete_t comp;
    } event_data;

    if (attr == NULL) {
        if (params) {
            params->func = NULL;
        }

        event_data.comp.conn_handle = bt_conn_index(conn);
        event_data.comp.err = 0;
        event = EVENT_GATT_DISCOVERY_COMPLETE;
        ble_stack_event_callback(event, &event_data, sizeof(event_data));
        return BT_GATT_ITER_STOP;
    }

    if (params->type == BT_GATT_DISCOVER_PRIMARY) {
        struct bt_gatt_service_val *value = attr->user_data;
        event_data.svc.conn_handle = bt_conn_index(conn);
        event_data.svc.start_handle = attr->handle;
        event_data.svc.end_handle = value->end_handle;
        uuid_covert(&event_data.svc.uuid, value->uuid);
        event = EVENT_GATT_DISCOVERY_SVC;
    } else if (params->type == BT_GATT_DISCOVER_INCLUDE) {
        struct bt_gatt_include *inc_val = (struct bt_gatt_include *)attr->user_data;
        event_data.svc_inc.conn_handle = bt_conn_index(conn);
        event_data.svc_inc.attr_handle = attr->handle;
        event_data.svc_inc.start_handle = inc_val->start_handle;
        event_data.svc_inc.end_handle = inc_val->end_handle;
        uuid_covert(&event_data.svc_inc.uuid, inc_val->uuid);
        event = EVENT_GATT_DISCOVERY_INC_SVC;
    } else if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
        struct bt_gatt_chrc *chrc = (struct bt_gatt_chrc *)attr->user_data;
        event_data.char_c.conn_handle = bt_conn_index(conn);
        event_data.char_c.attr_handle = attr->handle;
        event_data.char_c.val_handle = attr->handle + 1;
        event_data.char_c.props = chrc->properties;
        uuid_covert(&event_data.char_c.uuid, chrc->uuid);
        event = EVENT_GATT_DISCOVERY_CHAR;
    } else if (params->type == BT_GATT_DISCOVER_DESCRIPTOR) {
        event_data.char_des.conn_handle = bt_conn_index(conn);
        event_data.char_des.attr_handle = attr->handle;
        uuid_covert(&event_data.char_des.uuid, params->uuid);
        event = EVENT_GATT_DISCOVERY_CHAR_DES;
    }

    ble_stack_event_callback(event, &event_data, sizeof(event_data));

    return BT_GATT_ITER_CONTINUE;
}

int ble_stack_gatt_discovery(int16_t conn_handle,
                             gatt_discovery_type_en type,
                             uuid_t *uuid,
                             uint16_t start_handle,
                             uint16_t end_handle)
{
    int ret;
    struct bt_conn *conn;

    tlog("");
    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (ble_param_pool.disc_params[0].func) {
        return -BT_STACK_STATUS_EALREADY;
    }

    conn = bt_conn_lookup_handle(conn_handle);

    if (conn == NULL) {
        return -BT_STACK_STATUS_ENOTCONN;
    }

    struct bt_gatt_discover_params *params = &ble_param_pool.disc_params[0];

    struct bt_uuid *u;

    if (uuid) {
        u = (struct bt_uuid *)&ble_param_pool.uuid;
        params->uuid = btuuid_covert(u, uuid);
    } else {
        params->uuid = NULL;
    }

    if (type == GATT_FIND_PRIMARY_SERVICE) {
        params->type = BT_GATT_DISCOVER_PRIMARY;
    } else if (type == GATT_FIND_INC_SERVICE) {
        params->type = BT_GATT_DISCOVER_INCLUDE;
    } else if (type == GATT_FIND_CHAR) {
        params->type = BT_GATT_DISCOVER_CHARACTERISTIC;
    } else if (type == GATT_FIND_CHAR_DESCRIPTOR) {
        params->type = BT_GATT_DISCOVER_DESCRIPTOR;
    } else {
        return -BT_STACK_STATUS_EINVAL;
    }

    params->start_handle = start_handle;
    params->end_handle = end_handle;
    params->func = discover_func;

    ret = bt_gatt_discover(conn, params);

    if (ret) {
        params->func = NULL;
    }

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

static void gatt_write_cb(struct bt_conn *conn, u8_t err, struct bt_gatt_write_params *params)
{
    if (NULL == params) {
        return;
    }

    evt_data_gatt_write_cb_t event_data;
    event_data.conn_handle = bt_conn_index(conn);
    event_data.err = err;
    event_data.char_handle = params->handle;

    if (params) {
        params->func = NULL;
    }

    ble_stack_event_callback(EVENT_GATT_CHAR_WRITE_CB, &event_data, sizeof(event_data));
    k_sem_give(&write_sem);
}


int ble_stack_gatt_write(int16_t conn_handle, uint16_t attr_handle, uint8_t *data, uint16_t len, uint16_t offset, gatt_write_en type)
{
    int ret = -BT_STACK_STATUS_EIO;
    int i;
    struct bt_gatt_write_params *params = NULL;
    tlog("");

    if ((data == NULL) || (len == 0)) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (conn_handle < 0) {
        return -BT_STACK_STATUS_ENOTCONN;
    }

    if (attr_handle <= 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if(k_sem_take(&write_sem,K_SECONDS(3)) < 0){
        return BT_STACK_STATUS_EIO;
    }

    struct bt_conn *conn = bt_conn_lookup_handle(conn_handle);

    if (conn == NULL) {
        return -BT_STACK_STATUS_ENOTCONN;
    }

    if (type == GATT_WRITE) {

        for (i = 0; i < sizeof(ble_param_pool.write_params) / (sizeof(struct bt_gatt_write_params)); i++) {
            if (ble_param_pool.write_params[i].func == NULL) {
                break;
            }
        }

        if (i == sizeof(ble_param_pool.write_params) / (sizeof(struct bt_gatt_write_params))) {
            return -BT_STACK_STATUS_EINVAL;
        }

        params = &ble_param_pool.write_params[i];

        params->handle = attr_handle;
        params->data = data;
        params->length = len;
        params->offset = offset;
        params->func = gatt_write_cb;
        ret = bt_gatt_write(conn, params);
    } else if (type == GATT_WRITE_WITHOUT_RESPONSE || type == GATT_WRITE_SINGED) {
        ret = bt_gatt_write_without_response(conn, attr_handle, data, len, type == GATT_WRITE_SINGED);
    }

    if (ret && params) {
        params->func = NULL;
        k_sem_give(&write_sem);
    }

    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

u8_t gatt_read_cb(struct bt_conn *conn, u8_t err,
                  struct bt_gatt_read_params *params,
                  const void *data, u16_t length)
{
    evt_data_gatt_read_cb_t event_data = {0};

    event_data.conn_handle = bt_conn_index(conn);

    if (err || data == NULL || length == 0) {
        if (params) {
            params->func = NULL;
        }

        event_data.err = err;
        event_data.data = NULL;
        event_data.len = 0;
        ble_stack_event_callback(EVENT_GATT_CHAR_READ_CB, &event_data, sizeof(event_data));
        k_sem_give(&read_sem);
        return BT_GATT_ITER_STOP;
    }

    event_data.data = data;
    event_data.len = length;
    event_data.err = err;

    ble_stack_event_callback(EVENT_GATT_CHAR_READ_CB, &event_data, sizeof(event_data));
    k_sem_give(&read_sem);

    return BT_GATT_ITER_CONTINUE;
}


int ble_stack_gatt_read(int16_t conn_handle, uint16_t attr_handle, uint16_t offset)
{
    int ret = -BT_STACK_STATUS_EIO;
    struct bt_conn *conn;
    int i;
    tlog("");

    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if(k_sem_take(&read_sem,K_SECONDS(3)) < 0){
        return BT_STACK_STATUS_EIO;
    }

    conn = bt_conn_lookup_handle(conn_handle);

    if (conn == NULL) {
        return -BT_STACK_STATUS_ENOTCONN;
    }

    for (i = 0; i < sizeof(ble_param_pool.read_params) / (sizeof(struct bt_gatt_read_params)); i++) {
        if (ble_param_pool.read_params[i].func == NULL) {
            break;
        }
    }

    if (i == sizeof(ble_param_pool.read_params) / (sizeof(struct bt_gatt_read_params))) {
        return -BT_STACK_STATUS_EINVAL;
    }

    struct bt_gatt_read_params *params = &ble_param_pool.read_params[i];

    params->func = gatt_read_cb;

    params->handle_count = 1;

    params->single.handle = attr_handle;

    params->single.offset = offset;

    ret = bt_gatt_read(conn, params);

    if(ret){
        tlog("ret (%d)",ret);
        k_sem_give(&read_sem);
    }
    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_gatt_read_multiple(int16_t conn_handle, uint16_t attr_count, uint16_t attr_handle[])
{
    int ret = -BT_STACK_STATUS_EIO;
    //struct bt_gatt_read_params params;
    struct bt_conn *conn;
    //uint16_t *attr_list;
    int i;
    tlog("");

    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    if(k_sem_take(&read_sem,K_SECONDS(3)) < 0){
        return BT_STACK_STATUS_EIO;
    }

    if (attr_handle == NULL || attr_count <= 1) {
        return -BT_STACK_STATUS_EINVAL;
    }

    conn = bt_conn_lookup_handle(conn_handle);

    if (conn == NULL) {
        return -BT_STACK_STATUS_ENOTCONN;
    }

    for (i = 0; i < sizeof(ble_param_pool.read_params) / (sizeof(struct bt_gatt_read_params)); i++) {
        if (ble_param_pool.read_params[i].func == NULL) {
            break;
        }
    }

    if (i == sizeof(ble_param_pool.read_params) / (sizeof(struct bt_gatt_read_params))) {
        return -BT_STACK_STATUS_EINVAL;
    }

    struct bt_gatt_read_params *params = &ble_param_pool.read_params[i];

    params->func = gatt_read_cb;

    params->handle_count = attr_count;

    params->handles = attr_handle;

    ret = bt_gatt_read(conn, params);

    if(ret){
        tlog("ret (%d)",ret);
        k_sem_give(&read_sem);
    }
    if (conn) {
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_get_local_addr(dev_addr_t *addr)
{
    struct bt_le_oob oob;
    tlog("");

    if (addr == NULL) {
        return -BT_STACK_STATUS_EINVAL;
    }

    bt_le_oob_get_local(0, &oob);
    memcpy(addr, &oob.addr, sizeof(dev_addr_t));

    return 0;
}

int ble_stack_connect(dev_addr_t *peer_addr, conn_param_t *param, uint8_t auto_connect)
{
    int ret;
    struct bt_conn *conn = NULL;
    int16_t conn_handle;
    bt_addr_le_t peer;
    struct bt_le_conn_param conn_param;
    tlog("");

    if (peer_addr == NULL) {
        return -BT_STACK_STATUS_EINVAL;
    }

#if !defined(CONFIG_BT_WHITELIST)

    if (auto_connect) {
        return -BT_STACK_STATUS_EINVAL;
    }

#endif

    if (auto_connect > 1) {
        return -BT_STACK_STATUS_EINVAL;
    }

    peer = *(bt_addr_le_t *)peer_addr;

    if (param) {
        conn_param = *(struct bt_le_conn_param *)param;

        if (auto_connect) {
#if !defined(CONFIG_BT_WHITELIST)
            ret = bt_le_set_auto_conn(&peer, &conn_param);

            if (ret) {
                return ret;
            }

            conn = bt_conn_lookup_addr_le(0, &peer);
#endif
        } else {
            conn = bt_conn_create_le(&peer, &conn_param);
        }
    } else if (!auto_connect) {
#if !defined(CONFIG_BT_WHITELIST)
        ret = bt_le_set_auto_conn(&peer, NULL);

        if (ret) {
            return ret;
        }

#endif
    } else {
        return -BT_STACK_STATUS_EINVAL;
    }

    if (conn) {
        conn_handle = bt_conn_index(conn);
        bt_conn_unref(conn);
        return conn_handle;
    }

    (void)ret;
    return -BT_STACK_STATUS_ENOTCONN;
}

int ble_stack_disconnect(int16_t conn_handle)
{
    int ret = -BT_STACK_STATUS_EINVAL;

    tlog("");
    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    struct bt_conn *conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {
        ret = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_connect_info_get(int16_t conn_handle, connect_info_t *info)
{
    tlog("hdl (0x%x),info(%p)",conn_handle,info);
    if (conn_handle < 0 || NULL == info) {
        return -BT_STACK_STATUS_EINVAL;
    }

    struct bt_conn_info bt_conn_info = {0};
    struct bt_conn *conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {

        bt_conn_get_info(conn, &bt_conn_info);

        info->conn_handle = conn_handle;
        info->role = bt_conn_info.role;
        info->interval = bt_conn_info.le.interval;
        info->latency = bt_conn_info.le.latency;
        info->timeout = bt_conn_info.le.timeout;

        info->local_addr.type = bt_conn_info.le.src->type;
        memcpy(info->local_addr.val, bt_conn_info.le.src->a.val, 6);
        info->peer_addr.type = bt_conn_info.le.dst->type;
        memcpy(info->peer_addr.val, bt_conn_info.le.dst->a.val, 6);

        bt_conn_unref(conn);
    } else {
        return -BT_STACK_STATUS_EINVAL;
    }

    return 0;
}

int ble_stack_check_conn_params(const conn_param_t *param)
{
    tlog("");
    if (param->interval_min > param->interval_max ||
        param->interval_min < 6 || param->interval_max > 3200) {
        return false;
    }

    if (param->latency > 499) {
        return false;
    }

    if (param->timeout < 10 || param->timeout > 3200 ||
        ((4 * param->timeout) <=
         ((1 + param->latency) * param->interval_max))) {
        return false;
    }

    return true;
}

int ble_stack_security(int16_t conn_handle, security_en level)
{
    int ret = -BT_STACK_STATUS_EINVAL;
    struct bt_conn *conn;

    tlog("");
    if (conn_handle < 0) {
        return ret;
    }

    conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {
        ret = bt_conn_set_security(conn, level);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_pref_phy_set(int16_t conn_handle, pref_phy_en pref_tx_phy, pref_phy_en pref_rx_phy)
{
    int ret = -BT_STACK_STATUS_EIO;
    tlog("");

    if (conn_handle < 0) {
        return ret;
    }

#if defined(CONFIG_BT_USER_PHY_UPDATE)
    struct bt_conn * conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {
        ret = hci_le_set_phy(conn, 0U, pref_tx_phy,
                        pref_rx_phy,
                        BT_HCI_LE_PHY_CODED_ANY);
        if(ret){
            tlog("ret (%d)",ret);
        }
        bt_conn_unref(conn);
    }
    return ret;
#else
    return -BT_STACK_STATUS_EIO;
#endif
}

int ble_stack_connect_param_update(int16_t conn_handle, conn_param_t *param)
{
    int ret = -BT_STACK_STATUS_EINVAL;
    struct bt_conn *conn;
    tlog("");

    if (param == NULL || conn_handle < 0) {
        return ret;
    }

    conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {
        ret = bt_conn_le_param_update(conn, (struct bt_le_conn_param *)param);
        bt_conn_unref(conn);
    }
    else
    {
        return -BT_STACK_STATUS_ENOTCONN;
    }

    return ret;
}


int ble_stack_smp_passkey_entry(int16_t conn_handle, uint32_t passkey)
{
    tlog("passkey 0x%x",passkey);
    int ret = -BT_STACK_STATUS_EINVAL;

    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    struct bt_conn *conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {
        ret = bt_conn_auth_passkey_entry(conn, passkey);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_smp_cancel(int16_t conn_handle)
{
    int ret = -BT_STACK_STATUS_EINVAL;
    tlog("");

    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    struct bt_conn *conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {
        ret = bt_conn_auth_cancel(conn);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_smp_passkey_confirm(int16_t conn_handle)
{
    int ret = -BT_STACK_STATUS_EINVAL;
    tlog("");

    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    struct bt_conn *conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {
        ret = bt_conn_auth_passkey_confirm(conn);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_smp_pairing_confirm(int16_t conn_handle)
{
    int ret = -BT_STACK_STATUS_EINVAL;
    tlog("");

    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    struct bt_conn *conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {
        ret = bt_conn_auth_pairing_confirm(conn);
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_enc_key_size_get(int16_t conn_handle)
{
    int ret = -BT_STACK_STATUS_EINVAL;
    tlog("");

    if (conn_handle < 0) {
        return -BT_STACK_STATUS_EINVAL;
    }

    struct bt_conn *conn = bt_conn_lookup_handle(conn_handle);

    if (conn) {
#if defined(CONFIG_BT_SMP)
        ret = bt_conn_enc_key_size(conn);
#else
        ret = 0;
#endif
        bt_conn_unref(conn);
    }

    return ret;
}

int ble_stack_setting_load()
{
#ifdef CONFIG_BT_SETTINGS
    return  settings_load();
#else
    return 0;
#endif
}

int ble_stack_dev_unpair(dev_addr_t *peer_addr)
{
    bt_addr_le_t peer;
    tlog("");

    if (peer_addr == NULL) {
        return bt_unpair(0, NULL);
    }

    peer = *(bt_addr_le_t *)peer_addr;

    return bt_unpair(0, &peer);
}

int ble_stack_white_list_clear()
{
    tlog("");
    return bt_le_whitelist_clear();
}

int ble_stack_white_list_add(dev_addr_t *peer_addr)
{
    tlog("");
    return bt_le_whitelist_add((bt_addr_le_t *)peer_addr);
}

int ble_stack_white_list_remove(dev_addr_t *peer_addr)
{
    tlog("");
    return bt_le_whitelist_rem((bt_addr_le_t *)peer_addr);
}

int ble_stack_white_list_size()
{
    tlog("");
    extern struct bt_dev bt_dev;
    tlog("size (%p) (%u)",&bt_dev,bt_dev.le.wl_size);
    return bt_dev.le.wl_size;
}

int ble_stack_set_name(const char *name)
{
    tlog("");
    return bt_set_name(name);
}

int ble_stack_set_addr(const uint8_t addr[6])
{
    tlog("");
    bt_addr_le_t le_addr;
    le_addr.type = 0;
    memcpy(le_addr.a.val, addr, 6);
    extern int bt_set_bdaddr(const bt_addr_le_t *addr);
    return bt_set_bdaddr(&le_addr);
}

char *uuid_str(uuid_t *uuid)
{
    static char uuid_str[37];
    uint32_t tmp1, tmp5;
    uint16_t tmp0, tmp2, tmp3, tmp4;

    switch (uuid->type) {
        case UUID_TYPE_16:
            snprintf(uuid_str, 37, "%04x", UUID16(uuid));
            break;

        case UUID_TYPE_32:
            snprintf(uuid_str, 37, "%04x", UUID32(uuid));
            break;

        case UUID_TYPE_128:
            memcpy(&tmp0, &UUID128(uuid)[0], sizeof(tmp0));
            memcpy(&tmp1, &UUID128(uuid)[2], sizeof(tmp1));
            memcpy(&tmp2, &UUID128(uuid)[6], sizeof(tmp2));
            memcpy(&tmp3, &UUID128(uuid)[8], sizeof(tmp3));
            memcpy(&tmp4, &UUID128(uuid)[10], sizeof(tmp4));
            memcpy(&tmp5, &UUID128(uuid)[12], sizeof(tmp5));

            snprintf(uuid_str, 37, "%08x-%04x-%04x-%04x-%08x%04x",
                     tmp5, tmp4, tmp3, tmp2, tmp1, tmp0);
            break;

        default:
            memset(uuid_str, 0, 37);
    }

    return uuid_str;
}

typedef void (*event_callback_dev_addr_func)(dev_addr_t *addr, void *data);

struct dev_addr_callback_func{
    event_callback_dev_addr_func func;
    void *data;
};

static void _paired_key_find(const struct bt_bond_info *info, void *data)
{
    dev_addr_t peer_addr;
    struct dev_addr_callback_func *call = (struct dev_addr_callback_func *)data;

    if (info) {
        peer_addr.type = info->addr.type;
        memcpy(peer_addr.val, info->addr.a.val, sizeof(peer_addr.val));
        call->func(&peer_addr, call->data);
    }
}

int ble_stack_paired_dev_foreach(void (*func)(dev_addr_t *addr, void *data), void *data)
{
    struct dev_addr_callback_func inpara = {
        .func = func,
        .data = data,
    };

    if (!func) {
        return -BT_STACK_STATUS_EINVAL;
    }

    bt_foreach_bond(0, _paired_key_find, &inpara);

    return BT_STACK_STATUS_SUCCESS;
}

