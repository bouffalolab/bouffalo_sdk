#ifndef __OAD_SERVICE_H__
#define __OAD_SERVICE_H__

#include "types.h"
#include "gatt.h"

typedef void (*oad_upper_recv_cb)(struct bt_conn *conn, const u8_t *data, u16_t len);
typedef void (*oad_disc_cb)(struct bt_conn *conn,u8_t reason);
void bt_oad_register_recv_cb(oad_upper_recv_cb cb);
void bt_oad_register_disc_cb(oad_disc_cb cb);
void bt_oad_service_enable(void);
void bt_oad_servkce_disable(void);
void bt_oad_notify(struct bt_conn *conn, const void *data, u16_t len);
//enable or disable data length exchange and att mtu exchange.Enabled by default.
void bt_oad_enable_data_len_exchange(bool enable);
#endif //__OAD_SERVICE_H__