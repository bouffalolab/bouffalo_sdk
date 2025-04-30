#ifndef BTBLECONTROLLER_PORT_API_H_
#define BTBLECONTROLLER_PORT_API_H_

#include <stdint.h>

#define BL616_A0              0
#define BL616_A1              1

#if defined(BL702L)
#include "misc.h"
#ifndef ATTR_PDS_SECTION
#define ATTR_PDS_SECTION           __attribute__((section(".pds_code." ATTR_UNI_SYMBOL), noinline))
#endif
#else
#ifndef ATTR_PDS_SECTION
#define ATTR_PDS_SECTION
#endif
#endif

void btblecontroller_ble_irq_init(void *handler);
void btblecontroller_bt_irq_init(void *handler);
void btblecontroller_dm_irq_init(void *handler);
void btblecontroller_ble_irq_enable(uint8_t enable);
void btblecontroller_bt_irq_enable(uint8_t enable);
void btblecontroller_dm_irq_enable(uint8_t enable);
void btblecontroller_enable_ble_clk(uint8_t enable);
void btblecontroller_rf_restore();
int btblecontroller_efuse_read_mac(uint8_t mac[6]);
void btblecontroller_software_btdm_reset();
void btblecontroller_software_pds_reset();
void btblecontroller_pds_trim_rc32m();
uint8_t btblecontrolller_get_chip_version();
#if defined(BL702L) || defined(BL616)
void btblecontroller_sys_reset(void);
#endif
#if defined(CONFIG_BT_MFG_HCI_CMD) || defined(CONFIG_BLE_MFG_HCI_CMD)
int btblecontroller_putchar(int c);
#endif
void btblecontroller_puts(const char *str);
#endif