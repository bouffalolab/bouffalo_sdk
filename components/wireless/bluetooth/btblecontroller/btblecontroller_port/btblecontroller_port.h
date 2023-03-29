#ifndef BTBLECONTROLLER_PORT_API_H_
#define BTBLECONTROLLER_PORT_API_H_

#include <stdint.h>

void btblecontroller_ble_irq_init(void *handler);
void btblecontroller_bt_irq_init(void *handler);
void btblecontroller_dm_irq_init(void *handler);
void btblecontroller_ble_irq_register(void *handler);
void btblecontroller_bt_irq_register(void *handler);
void btblecontroller_dm_irq_register(void *handler);
void btblecontroller_ble_irq_enable(uint8_t enable);
void btblecontroller_bt_irq_enable(uint8_t enable);
void btblecontroller_dm_irq_enable(uint8_t enable);
void btblecontroller_irq_pending_clear(int irq_num);
void btblecontroller_irq_register(int irq_num, void *handler);
void btblecontroller_irq_enable(int irq_num);
void btblecontroller_enable_ble_clk(uint8_t enable);
void btblecontroller_rf_restore();
int btblecontroller_efuse_read_mac(uint8_t mac[6]);

#endif