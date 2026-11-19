#ifndef BTBLECONTROLLER_PORT_API_H_
#define BTBLECONTROLLER_PORT_API_H_

#include <stdint.h>
#include <stdbool.h>

#define BL616_A0              0
#define BL616_A1              1

#if defined(BL702L)
#define ATTR_STRINGIFY(x)          #x
#define ATTR_TOSTRING(x)           ATTR_STRINGIFY(x)
#define ATTR_UNI_SYMBOL            __FILE__ ATTR_TOSTRING(__LINE__)
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
#if defined(BL702L) || defined(BL616) || defined(BL618DG)
void btblecontroller_sys_reset(void);
#endif
#if defined(CONFIG_BT_MFG_HCI_CMD) || defined(CONFIG_BLE_MFG_HCI_CMD)
int btblecontroller_putchar(int c);
#endif
void btblecontroller_puts(const char *str);
uint64_t btblecontroller_mtimer_get_time_us(void);
#if defined(BL618DG)
/* RX ppm sampling runs in controller interrupt context; overrides must not block.
 * RTC getter returns nominal us; sample/age use half-us, interval half-slots. */
typedef struct {
    uint64_t rtc_hus;
    uint16_t evt_cnt;
    uint16_t interval;
    uint8_t rate;
    uint8_t valid;
    int32_t ppm;
} btblecontroller_lp_rx_ppm_state_t;

/* Disabled until LPFW initialization succeeds. Changing this setting clears
 * the sampling history; repeated calls with the same setting preserve it. */
void btblecontroller_lp_rx_ppm_set_enabled(bool enabled);
uint64_t btblecontroller_lp_rtc_get_us(void);
/* Read-only check for a new baseline or at least 500 ms of connection events. */
bool btblecontroller_lp_rx_ppm_sample_due(uint8_t link_id, uint16_t evt_cnt,
                                        uint16_t interval, uint8_t rate);
void btblecontroller_lp_rx_ppm_update(uint8_t link_id, uint16_t evt_cnt,
                                    uint16_t interval, uint8_t rate, uint64_t rx_rtc_hus,
                                    int64_t age_hus);
void btblecontroller_lp_rx_ppm_reset(uint8_t link_id);
/* False until an estimate exists for this link, interval and RX rate. */
bool btblecontroller_lp_rx_ppm_ready(uint8_t link_id, uint16_t interval, uint8_t rate);
/* Export/import the complete APP-side sampling state at an LPFW handoff. */
bool btblecontroller_lp_rx_ppm_state_export(uint8_t link_id, uint16_t interval, uint8_t rate,
                                            btblecontroller_lp_rx_ppm_state_t *state);
void btblecontroller_lp_rx_ppm_state_import(uint8_t link_id,
                                            const btblecontroller_lp_rx_ppm_state_t *state);
void btblecontroller_gpio_config(uint8_t pin, uint8_t is_high);
void btblecontroller_rc32k_xtal_count_trigger(void);
uint16_t btblecontroller_rc32k_xtal_count_wait_result(void);
#endif
#endif
