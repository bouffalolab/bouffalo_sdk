/****************************************************************************
 * btble_test_port.h
 *
 * BLE 性能测试 hook：供 btble_cli_port.c 的 conn 回调调用。
 ****************************************************************************/
#ifndef _BTBLE_TEST_PORT_H_
#define _BTBLE_TEST_PORT_H_

#include <stdint.h>
#include <stdbool.h>

struct bt_conn;

/* 初始化测试上下文（创建信号量等），在 bt_enable 成功后调用一次 */
void ble_test_port_init(void);

/* 连接事件 hook：在 port 文件的 connected/disconnected 回调中调用 */
void ble_test_on_connected(struct bt_conn *conn, uint8_t err);

/* 断开 hook：返回 true 表示测试逻辑已接管广播控制，
 * port 应跳过默认的 set_adv_enable(true)；返回 false 走默认行为。 */
bool ble_test_on_disconnected(struct bt_conn *conn, uint8_t reason);

#endif /* _BTBLE_TEST_PORT_H_ */
