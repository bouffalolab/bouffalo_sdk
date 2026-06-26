/*
 * SPP Header File
 *
 * Copyright (c) 2021-2022 Bouffalolab Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _INCLUDE_BLUETOOTH_SPP_H_
#define _INCLUDE_BLUETOOTH_SPP_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_SPP_MAX_CONN
#define CONFIG_SPP_MAX_CONN 7
#endif

struct spp_callback_t {
	void (*connected)(struct bt_conn *conn);
	void (*disconnected)(struct bt_conn *conn);
	void (*bt_spp_recv)(struct bt_conn *conn, u8_t *data, u16_t length);
};

int bt_spp_init(void);
int bt_spp_connect(struct bt_conn *conn);
int bt_spp_disconnect(struct bt_conn *conn);
int bt_spp_send(struct bt_conn *conn, uint8_t *buf_data, uint16_t length);
void spp_cb_register(struct spp_callback_t *cb);

/* Return the conn at the given SPP slot index, or NULL if not connected.
 * idx range: [0, CONFIG_SPP_MAX_CONN). Used for per-link throughput tests.
 */
struct bt_conn *bt_spp_get_conn(uint8_t idx);

/* Return the number of currently connected SPP links. */
uint8_t bt_spp_get_conn_count(void);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_BLUETOOTH_AVCTP_H_ */
