#include "byteorder.h"
#include "oad_service.h"
#include "oad.h"
#include "bt_log.h"
#include "hci_core.h"
#include "conn_internal.h"
#include <FreeRTOS.h>
#include <task.h>

#define OAD_PRIO        (CONFIG_BT_RX_PRIO - 1)
#define OAD_ATT_TIMEOUT K_SECONDS(30)
#define OAD_QUEUE_LEN   4

oad_upper_recv_cb upper_recv_cb;
oad_disc_cb disc_cb;
struct bt_conn *ble_oad_conn = NULL;
bool enable_data_len_exchange = true;
static u8_t isRegister = 0;

static void ble_oad_connected(struct bt_conn *conn, u8_t err);
static void ble_oad_disconnected(struct bt_conn *conn, u8_t reason);

static struct bt_conn_cb ble_oad_conn_callbacks = {
	.connected	=   ble_oad_connected,
	.disconnected	=   ble_oad_disconnected,
};

static struct bt_gatt_exchange_params oad_exchg_mtu;
static struct k_thread oad_task_h;
static struct k_fifo oad_queue;
static struct k_sem oad_sem;

typedef enum {
    OAD_MSG_EXCHANGE_MTU,
    OAD_MSG_EXCHANGE_DATA_LEN,
} oad_msg_type;

static void ble_oad_tx_mtu_size(struct bt_conn *conn, u8_t err,
				struct bt_gatt_exchange_params *params)
{
    k_sem_give(&oad_sem);
    if (!err) {
        uint8_t *msg = k_malloc(sizeof(uint8_t));
        if (msg) {
            *msg = OAD_MSG_EXCHANGE_DATA_LEN;
            k_fifo_put(&oad_queue, (void *)msg);
        }
        BT_WARN("ble oad exchange mtu size success, mtu size: %d\n", bt_gatt_get_mtu(ble_oad_conn));
    } else {
        BT_WARN("ble oad exchange mtu size failure, err: %d\n", err);
    }
}

static void ble_oad_connected(struct bt_conn *conn, u8_t err)
{
    if (err || conn->type != BT_CONN_TYPE_LE) {
        return;
    }

    if (!ble_oad_conn) {
        ble_oad_conn = conn;
    }

    if (enable_data_len_exchange) {
        uint8_t *msg = k_malloc(sizeof(uint8_t));
        if (msg) {
            *msg = OAD_MSG_EXCHANGE_MTU;
            k_fifo_put(&oad_queue, (void *)msg);
        }
    }
}

static void ble_oad_disconnected(struct bt_conn *conn, u8_t reason)
{
    if (conn->type != BT_CONN_TYPE_LE) {
        return;
    }

    BT_WARN("reason(%d)", reason);

    if (conn == ble_oad_conn) {
        ble_oad_conn = NULL;
        (disc_cb)(conn, reason);
    }
}

static void oad_task(void *pvParameters)
{
    int ret = 0;

    while (1) {
        uint8_t *msg = k_fifo_get(&oad_queue, K_FOREVER);
        if (msg == NULL)
            continue;

        switch (*msg) {
            case OAD_MSG_EXCHANGE_MTU:
            {
                if (!ble_oad_conn) {
                    break;
                }
                oad_exchg_mtu.func = ble_oad_tx_mtu_size;
                ret = bt_gatt_exchange_mtu(ble_oad_conn, &oad_exchg_mtu);
                if (!ret) {
                    BT_WARN("ble oad exchange mtu size pending.\n");
                } else {
                    BT_WARN("ble oad exchange mtu size failure, err: %d\n", ret);
                }
                k_sem_take(&oad_sem, OAD_ATT_TIMEOUT);
            }
            break;
            case OAD_MSG_EXCHANGE_DATA_LEN:
            {
                if (!ble_oad_conn) {
                    break;
                }
                int tx_octets = 0x00fb;
                int tx_time = 0x0848;
                ret = bt_le_set_data_len(ble_oad_conn, tx_octets, tx_time);
                if (!ret) {
                    BT_WARN("ble oad set data length success.\n");
                } else {
                    BT_WARN("ble oad set data length failure, err: %d\n", ret);
                }
            }
            break;

            default:
                break;
        }

        k_free(msg);
    }
}

static int oad_recv(struct bt_conn *conn,
		    const struct bt_gatt_attr *attr, const void *buf,
		    u16_t len, u16_t offset, u8_t flags)
{
    (upper_recv_cb)(conn, buf, len);
    return 0;
}

static void oad_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    BT_WARN("oad ccc:value=[%d]\r\n", value);
}

static struct bt_gatt_attr oad_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_OAD),
    BT_GATT_CHARACTERISTIC(BT_UUID_OAD_DATA_IN,
            BT_GATT_CHRC_WRITE_WITHOUT_RESP,
            BT_GATT_PERM_WRITE, NULL, oad_recv, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_OAD_DATA_OUT,
            BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE,
            NULL, NULL, NULL),
    BT_GATT_CCC(oad_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
};

static struct bt_gatt_service oad_svc = BT_GATT_SERVICE(oad_attrs);

void bt_oad_notify(struct bt_conn *conn, const void *data, u16_t len)
{
    bt_gatt_notify(conn, &oad_attrs[3], data, len);
}

void bt_oad_register_recv_cb(oad_upper_recv_cb cb)
{
    upper_recv_cb = cb;
}

void bt_oad_register_disc_cb(oad_disc_cb cb)
{
    disc_cb = cb;
}

void bt_oad_enable_data_len_exchange(bool enable)
{
    enable_data_len_exchange = enable;
}

void bt_oad_service_enable(void)
{
    if (!isRegister) {
        isRegister = 1;
        bt_conn_cb_register(&ble_oad_conn_callbacks);
        bt_gatt_service_register(&oad_svc);
        k_sem_init(&oad_sem, 0, 1);
        k_fifo_init(&oad_queue, OAD_QUEUE_LEN);
        k_thread_create(&oad_task_h, "bleoad", 1024, (k_thread_entry_t)oad_task, OAD_PRIO);
    }
}

void bt_oad_service_disable(void)
{
    if(isRegister)
    {
        bt_gatt_service_unregister(&oad_svc);
        k_queue_free(&oad_queue._queue);
        k_sem_delete(&oad_sem);
        k_thread_delete(&oad_task_h);
        isRegister = 0;
    }
}
