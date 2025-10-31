/****************************************************************************
FILE NAME
    ble_tp_svc.c

DESCRIPTION
    test profile demo

NOTES
*/
/****************************************************************************/

#include <bt_errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
//#include <blog.h>

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "gatt.h"
#include "hci_core.h"
#include "bt_uuid.h"
#include "ble_tp_svc.h"
#include "bt_log.h"

#define TP_PRIO (CONFIG_BT_RX_PRIO - 1)
#if defined(CONFIG_BLE_TP_SVC_COMPATIBILITY_TEST)
uint32_t m_index_send=1;
uint32_t m_index_receive=1;
#endif
static void ble_tp_connected(struct bt_conn *conn, u8_t err);
static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason);
static int bl_tp_send_indicate(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *data, u16_t len);

#define TP_ATT_TIMEOUT      K_SECONDS(30)
struct bt_conn *ble_tp_conn;
struct bt_gatt_exchange_params exchg_mtu;
TaskHandle_t ble_tp_notify_task_h;
#define BLE_TP_QUEUE_LEN   10
static struct k_thread ble_tp_task_h;
static struct k_fifo ble_tp_queue;

int tx_mtu_size = 20;
u8_t tp_start = 0;
static u8_t created_tp_task = 0;
static u8_t isRegister = 0;
struct k_sem tp_sem;

static struct bt_conn_cb ble_tp_conn_callbacks = {
	.connected	=   ble_tp_connected,
	.disconnected	=   ble_tp_disconnected,
};

typedef enum{
    TP_MSG_EXCHAGNE_MTU,
    TP_MSG_EXCHANGE_DATA_LEN,
    TP_MSG_TX_GATT_INDICATE,
    TP_MSG_TX_GATT_NOTIFY,
}ble_tp_msg_type;

/*************************************************************************
NAME
    ble_tp_tx_mtu_size
*/
static void ble_tp_tx_mtu_size(struct bt_conn *conn, u8_t err,
			  struct bt_gatt_exchange_params *params)
{
   k_sem_give(&tp_sem);
   if(!err)
   {
        tx_mtu_size = bt_gatt_get_mtu(ble_tp_conn);
        uint8_t *msg = k_malloc(sizeof(uint8_t));
        if(msg){
            *msg = TP_MSG_EXCHANGE_DATA_LEN;
            k_fifo_put(&ble_tp_queue, (void *)msg);
        }
        BT_WARN("ble tp echange mtu size success, mtu size: %d", tx_mtu_size);
   }
   else
   {
        BT_WARN("ble tp echange mtu size failure, err: %d", err);
   }
}

/*************************************************************************
NAME
    ble_tp_connected
*/
static void ble_tp_connected(struct bt_conn *conn, u8_t err)
{
    if(err || conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }

    BT_INFO("%s",__func__);
    ble_tp_conn = conn;
    #if defined(CONFIG_BLE_TP_SVC_COMPATIBILITY_TEST)
    m_index_send = 1;
    m_index_receive = 1;
    #if defined(BL602)
    extern void BL602_Delay_MS(uint32_t cnt);
    BL602_Delay_MS(100);
    #else
    extern void arch_delay_ms(uint32_t cnt);
    arch_delay_ms(100);
    #endif
    #endif
    uint8_t *msg = k_malloc(sizeof(uint8_t));
    if(msg){
        *msg = TP_MSG_EXCHAGNE_MTU;
        k_fifo_put(&ble_tp_queue, (void *)msg);
    }
}

/*************************************************************************
NAME    
    ble_tp_disconnected
*/
static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason)
{ 
    if(conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }

    BT_WARN("reason(%d)",reason);

    ble_tp_conn = NULL;
    #if defined(CONFIG_BLE_TP_SVC_COMPATIBILITY_TEST)
    BT_WARN("start ble adv\r\n");
	set_adv_enable(1);
    #endif
}

/*************************************************************************
NAME
    ble_tp_recv_rd
*/
static int ble_tp_recv_rd(struct bt_conn *conn,	const struct bt_gatt_attr *attr,
                                        void *buf, u16_t len, u16_t offset)
{
    #if defined(CONFIG_BLE_TP_SVC_COMPATIBILITY_TEST)
    int size = 128;
    char data[128] = {0x01, 0x00, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    data[1] = data[1]+ (uint8_t)m_index_send - 1;
    memcpy(buf, data, size);
    BT_WARN("send_packet_n=%d\r\n",m_index_send);
    m_index_send++;
    #else
    int size = 9;
    char data[9] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    memcpy(buf, data, size);
    #endif
    
    return size;
}

/*************************************************************************
NAME
    ble_tp_recv_wr
*/
static int ble_tp_recv_wr(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                        const void *buf, u16_t len, u16_t offset, u8_t flags)
{
    BT_WARN("recv data len=%d, offset=%d, flag=%d", len, offset, flags);
    #if defined(CONFIG_BLE_TP_SVC_COMPATIBILITY_TEST)
	u16_t iLen = (len < 16)?len:16;
	u8_t err = 0;
	u8_t *local_buf = buf;
    if(len >16)
    {  
       for(u16_t i = 16;i < len; i++)
       	{
       	    if(local_buf[i] != 0xff)
			{
			   err = 1;
			   break;
       	    }
       	}
    }	
    
	BT_WARN("recv_packet_n=%d,len=%d,err=%d\r\n",m_index_receive,len,err);
	BT_WARN("recv data:%s\r\n", bt_hex(buf, iLen));
	/*
	printf("recv_data:");
	for(uint8_t i = 0; i < iLen; i++)
	{
		printf("0x",buf[i]);
	}
	printf("\r\n");*/
	m_index_receive++;
    #endif
    if (flags & BT_GATT_WRITE_FLAG_PREPARE)
    {
        //Don't use prepare write data, execute write will upload data again.
        BT_WARN("rcv prepare write request");
        return 0;
    }

    if(flags & BT_GATT_WRITE_FLAG_CMD)
    {
        //Use write command data.
        BT_WARN("rcv write command");
    }
    else
    {
        //Use write request / execute write data.
        BT_WARN("rcv write request / exce write");
    }

    return len;
}

/*************************************************************************
NAME    
    indicate_rsp /bl_tp_send_indicate
*/ 
void indicate_rsp(struct bt_conn *conn, const struct bt_gatt_attr *attr,	u8_t err)
{
    k_sem_give(&tp_sem);
    BT_WARN("receive confirm, err:%d", err);
}

/*************************************************************************
NAME
    ble_tp_ind_ccc_changed
*/
static void ble_tp_ind_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    BT_WARN("ccc:value=[%d]",value);
    if(value == BT_GATT_CCC_INDICATE)
    {
        uint8_t *msg = k_malloc(sizeof(uint8_t));
        if(msg){
            *msg = TP_MSG_TX_GATT_INDICATE;
            k_fifo_put(&ble_tp_queue, (void *)msg);
        }
    }
}

/*************************************************************************
NAME
    ble_tp_notify
*/
static void ble_tp_notify_task(void *pvParameters)
{
    #if defined(CONFIG_BLE_TP_SVC_COMPATIBILITY_TEST)
    int err = -1, counter = 0;
    char data[128] = {};

    while(1)
    {
        counter++;
        data[0] = (counter >> 24) & 0xff;
        data[1] = (counter >> 16) & 0xff;
        data[2] = (counter >>  8) & 0xff;
        data[3] = (counter >>  0) & 0xff;
        err = bt_gatt_notify(ble_tp_conn, get_attr(BT_CHAR_BLE_TP_NOT_ATTR_VAL_INDEX), data, sizeof(data));
        printf("bt_gatt_notify counter[%d] status[%d]\r\n", counter, err);
        // Every 20s notify 128 bytes.
        vTaskDelay(pdMS_TO_TICKS(20000));
        
    }
    #else
    int err = -1;
    char data[244] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    while(1)
    {
        err = bt_gatt_notify(ble_tp_conn, get_attr(BT_CHAR_BLE_TP_NOT_ATTR_VAL_INDEX), data, (tx_mtu_size - 3));
        BT_WARN("ble tp send notify : %d", err);
    }
    #endif
}

static void ble_tp_task(void *pvParameters)
{
    int ret = 0;
   
    while(1){
         uint8_t *msg = k_fifo_get(&ble_tp_queue, K_FOREVER);
         if(msg == NULL)
             continue;
 
         switch(*msg){
             case TP_MSG_EXCHAGNE_MTU:
             {
                 exchg_mtu.func = ble_tp_tx_mtu_size;
                 ret = bt_gatt_exchange_mtu(ble_tp_conn, &exchg_mtu);
                 if (!ret) {
                     BT_WARN("ble tp exchange mtu size pending.");
                 } else {
                     BT_WARN("ble tp exchange mtu size failure, err: %d", ret);
                 }
                 k_sem_take(&tp_sem, TP_ATT_TIMEOUT);
             }
             break;
             case TP_MSG_EXCHANGE_DATA_LEN:
             {
                 int tx_octets = 0x00fb;
                 int tx_time = 0x0848;
                 ret = bt_le_set_data_len(ble_tp_conn, tx_octets, tx_time);
                 if(!ret)
                 {
                     BT_WARN("ble tp set data length success.");
                 }
                 else
                 {
                     BT_WARN("ble tp set data length failure, err: %d\n", ret);
                 }
             }
             break;
             case TP_MSG_TX_GATT_INDICATE:
             {
                 char data[9] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
                 ret = bl_tp_send_indicate(ble_tp_conn, get_attr(BT_CHAR_BLE_TP_IND_ATTR_VAL_INDEX), data, 9);
                 BT_WARN("ble tp send indicate: %d", ret);
                 k_sem_take(&tp_sem, TP_ATT_TIMEOUT);
             }
             break;
 
             case TP_MSG_TX_GATT_NOTIFY:
             {
                 ret = bt_gatt_notify(ble_tp_conn, get_attr(BT_CHAR_BLE_TP_NOT_ATTR_VAL_INDEX), "notify", strlen("notify"));
                 BT_WARN("ble tp send notify: %d", ret);
             }
             break;

             default:
                break;
         }

         k_free(msg);
    }     
}

/*************************************************************************
NAME    
    ble_tp_not_ccc_changed
*/ 
static void ble_tp_not_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    int err;

    UNUSED(err);
    BT_WARN("ccc:value=[%d]",value);
    
    if(tp_start)
    {
        if(value == BT_GATT_CCC_NOTIFY)
        {
            if(xTaskCreate(ble_tp_notify_task, (char*)"bletpnotify", 256, NULL, TP_PRIO, &ble_tp_notify_task_h) == pdPASS)
            {
                created_tp_task = 1;
                BT_WARN("Create throughput tx task success.");
            }       
            else        
            {      
                created_tp_task = 0;
                BT_WARN("Create throughput tx task fail.");
            }
        }
        else
        {
            if(created_tp_task)
            {
                BT_WARN("Delete throughput tx task.");
                vTaskDelete(ble_tp_notify_task_h);
                created_tp_task = 0;
            }
        }
    }
    else
    {
        if(created_tp_task)
        {
            BT_WARN("Delete throughput tx task.");
            vTaskDelete(ble_tp_notify_task_h);
            created_tp_task = 0;
        }
        if(value == BT_GATT_CCC_NOTIFY) {
            uint8_t *msg = k_malloc(sizeof(uint8_t));
            if(msg){
                *msg = TP_MSG_TX_GATT_NOTIFY;
                k_fifo_put(&ble_tp_queue, (void *)msg);
            }
        }
    }
}

/*************************************************************************
*  DEFINE : attrs 
*/
static struct bt_gatt_attr attrs[]= {
	BT_GATT_PRIMARY_SERVICE(BT_UUID_SVC_BLE_TP),

        BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_TP_RD,
							BT_GATT_CHRC_READ,
							BT_GATT_PERM_READ,
							ble_tp_recv_rd,
							NULL,
							NULL),

	BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_TP_WR,
							BT_GATT_CHRC_WRITE |BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							BT_GATT_PERM_WRITE|BT_GATT_PERM_PREPARE_WRITE,
							NULL,
							ble_tp_recv_wr,
							NULL),

	BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_TP_IND,
							BT_GATT_CHRC_INDICATE,
							0,
							NULL,
							NULL,
							NULL),

	BT_GATT_CCC(ble_tp_ind_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

	BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_TP_NOT,
							BT_GATT_CHRC_NOTIFY,
							0,
							NULL,
							NULL,
							NULL),
							
	BT_GATT_CCC(ble_tp_not_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)

};

static int bl_tp_send_indicate(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				                    const void *data, u16_t len)
{
    //indicate paramete must be allocated statically
    static struct bt_gatt_indicate_params ind_params;
    ind_params.attr = attr;
    ind_params.data = data;
    ind_params.len = len;
    ind_params.func = indicate_rsp;
    ind_params.uuid = NULL;
    /*it is possible to indicate by UUID by setting it on the
      parameters, when using this method the attribute given is used as the
      start range when looking up for possible matches.In this case,set uuid as follows.*/
    //ind_params->uuid = attrs[6].uuid;

    return bt_gatt_indicate(conn, &ind_params);
}

/*************************************************************************
NAME    
    get_attr
*/
struct bt_gatt_attr *get_attr(u8_t index)
{
	return &attrs[index];
}


struct bt_gatt_service ble_tp_server = BT_GATT_SERVICE(attrs);


/*************************************************************************
NAME    
    ble_tp_init
*/
void ble_tp_init()
{
    bt_conn_cb_register(&ble_tp_conn_callbacks);
    if( !isRegister )
    {
        isRegister = 1;
        bt_gatt_service_register(&ble_tp_server);
        k_sem_init(&tp_sem, 0, 1);
        k_fifo_init(&ble_tp_queue, BLE_TP_QUEUE_LEN);
        if(ble_tp_queue._queue.hdl)
            k_thread_create(&ble_tp_task_h, "bletp", 1024,(k_thread_entry_t)ble_tp_task, TP_PRIO);
    }
}



