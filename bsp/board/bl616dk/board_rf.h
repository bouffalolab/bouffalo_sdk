#ifndef __BOARD_RF_H__
#define __BOARD_RF_H__

#if __has_include("board_rf_overlay.h")
#include "board_rf_overlay.h"
#else

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

#endif
#endif
