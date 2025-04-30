/****************************************************************************
FILE NAME
    ias.c

DESCRIPTION
    Immediate Alert Service

****************************************************************************/

#include <bt_errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "bluetooth.h"
#include "gatt.h"
#include "bt_uuid.h"
#include "ias.h"
#include "bt_log.h"

/*************************************************************************
*  NAME: ias recv callback
*/
static ias_recv_callbck_func_t ias_recv_callback=NULL;

void ias_register_recv_callback(ias_recv_callbck_func_t cb)
{
    ias_recv_callback=cb;
}
/*************************************************************************
*  NAME: ias_recv_wr
*/
static int ias_recv_wr(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                        const void *buf, u16_t len, u16_t offset, u8_t flags)
{
    BT_INFO("recv data len=%d, offset=%d, flag=%d", len, offset, flags);

    if(flags & BT_GATT_WRITE_FLAG_CMD)
    {
        BT_INFO("rcv write command");
        //handle alert level.
        if(ias_recv_callback)
            ias_recv_callback(conn,buf,len);
    }

    return len;
}


/*************************************************************************
*  DEFINE : attrs 
*/
static struct bt_gatt_attr attrs[]= {
	BT_GATT_PRIMARY_SERVICE(BT_UUID_IAS),

        BT_GATT_CHARACTERISTIC(BT_UUID_IAS_ALERT_LEVEL,
							BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							BT_GATT_PERM_WRITE,
							NULL,
							ias_recv_wr,
							NULL)

};


struct bt_gatt_service ias_server = BT_GATT_SERVICE(attrs);


/*************************************************************************
*  NAME: ias_init
*/
void ias_init()
{
    bt_gatt_service_register(&ias_server);
}

/*************************************************************************
*  NAME: ias_deinit
*/
void ias_deinit()
{
    bt_gatt_service_unregister(&ias_server);
}
