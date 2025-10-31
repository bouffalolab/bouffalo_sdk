/****************************************************************************
FILE NAME
    ias.h

DESCRIPTION
    Immediate Alert Service

****************************************************************************/
#ifndef _BLE_IA_SVC_H_
#define _BLE_IA_SVC_H_


typedef void (*ias_recv_callbck_func_t)(struct bt_conn *conn, void *buf, u8_t len);

void ias_register_recv_callback(ias_recv_callbck_func_t cb);
/** 
    @brief register immediate alert service.
 */
void ias_init();

void ias_deinit();

#endif
