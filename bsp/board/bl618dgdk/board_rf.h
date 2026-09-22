#ifndef __BOARD_RF_H__
#define __BOARD_RF_H__

#if __has_include("board_rf_overlay.h")
#include "board_rf_overlay.h"
#else

#include <stdbool.h>
#include <stdint.h>

/* Board overlays opt in only when they implement the same hardware contract. */
#define BOARD_RF_COEX_OPS_SUPPORTED 1

#define BOARD_RF_MODE_COMBO_BIT       (1u << 0)
#define BOARD_RF_MODE_STANDALONE_BIT  (1u << 1)
/* Validate declared wiring and report confirmed modes without touching RF/GPIO. */
int board_rf_modes_get(bool standalone, int spdt_gpio, uint32_t *modes);

enum board_rf_spdt_mode {
    BOARD_RF_SPDT_FIXED_BT,
    BOARD_RF_SPDT_DYNAMIC_PTA,
    BOARD_RF_SPDT_FIXED_2G,
};

/* Hardware-only operations; caller owns serialization and radio safety checks. */
int board_rf_spdt_mode_apply(enum board_rf_spdt_mode mode);
int board_rf_spdt_mode_verify(enum board_rf_spdt_mode mode);

enum board_ctl_ops {
  /* @ rf configuration start {  */
  BRD_CTL_RF_RESET_DEFAULT,

  BRD_CTL_RF_INIT_WLAN,
  BRD_CTL_RF_DEINIT_WLAN,

  BRD_CTL_RF_INIT_BZ,
  BRD_CTL_RF_DEINIT_BZ,

  BRD_CTL_RF_INIT_ALL,
  BRD_CTL_RF_DEINIT_ALL,

  BRD_CTL_RF_SET_XTAL,
  BRD_CTL_RF_SET_CAPCODE,
  /* } rf configuration end @ */

};

int board_rf_ctl(enum board_ctl_ops ops, ...);

/* Initializers below only prepare hardware. The integration layer must check
 * stack lifetime and register board configuration/callbacks separately.
 * They no longer configure WiFi6 Coex as a side effect. */
/** single antenna 
 *  ┌────────────┐
 *  │   BT PATH ─┼────── NC
 *  │            │       ┌────────────┐
 *  │   2G PATH ─┼──────►│  2G / 5G   │
 *  │   5G PATH ─┼──────►│  Diplexer  │──► Antenna
 *  └────────────┘       └────────────┘
 * */
int board_rf_single_ant_init(void);
int board_rf_combo_init(void);

/** Physical RF preparation result, not MGMR/Coex integration readiness. */
int board_rf_init_status_get(void);

/** single antenna with spdt
 *  ┌────────────┐       ┌──────┐
 *  │   BT PATH ─┼──────►| SPDT |      ┌────────────┐
 *  │   2G PATH ─┼──────►|      |─────►│  2G / 5G   │
 *  │            │       └──────┘      │            │──► Antenna
 *  │   5G PATH ─┼────────────────────►│  Diplexer  │
 *  └────────────┘                     └────────────┘
 *
 * @param spdt_gpio The single control GPIO, supplied by the board integrator.
 *
 * GPIO_FUNC_SPDT outputs high on an even GPIO and low on an odd GPIO when BT
 * wins PTA arbitration. The GPIO parity and the external switch truth table
 * must match the wiring. No GPIO or polarity is guessed for another board.
 * Leaves the switch fixed to BT until an approved runtime recipe takes over.
 * */
int board_rf_single_ant_spdt_init(int spdt_gpio);

/** Legacy full RF diagnostic initializer, not a GPIO-only operation.
 * Not part of the board_rf_init_status_get()/wifi_bt_init startup contract.
 * No shell alias; do not call while radio services are running.
 * */
void board_rf_single_ant_spdt_force_bt_init(int pin_bt_path, int pin_2g_path);

/** Legacy full RF diagnostic initializer: selects combo path and recalibrates.
 * Not part of the board_rf_init_status_get()/wifi_bt_init startup contract.
 * No shell alias; do not call while radio services are running.
 * */
void board_rf_single_ant_spdt_force_2g_init(int pin_bt_path, int pin_2g_path);

/** dual antenna
 *  ┌────────────┐
 *  │   BT PATH ─┼───────────────────────► Antenna
 *  │            │       ┌────────────┐
 *  │   2G PATH ─┼──────►│  2G / 5G   │
 *  │   5G PATH ─┼──────►│  Diplexer  │──► Antenna
 *  └────────────┘       └────────────┘ 
 * */
int board_rf_dual_ant_init(void);

#endif
#endif
