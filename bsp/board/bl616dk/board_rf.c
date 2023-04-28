#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#include "board_rf.h"

#include "wl_api.h"
#include "rfparam_adapter.h"
#include "bl616_hbn.h"

#define USER_UNUSED(a) ((void)(a))

#if defined(WL_API_RMEM_EN) && WL_API_RMEM_EN
#define WL_API_RMEM_ADDR    0x20010600
#endif

static int ctl_rf_configuration(enum board_ctl_ops ops, va_list args)
{
  struct wl_cfg_t *wl_cfg;
  int ret;

#if defined(WL_API_RMEM_EN) && WL_API_RMEM_EN
  wl_cfg = wl_cfg_get((uint8_t *)WL_API_RMEM_ADDR);
#else
  wl_cfg = wl_cfg_get();
#endif

  switch (ops) {
  case BRD_CTL_RF_RESET_DEFAULT:
      do {
        uint32_t xtal_value;
        int full_cal = va_arg(args, int);

        HBN_Get_Xtal_Value(&xtal_value);

        /* reset to default param */
        wl_cfg->en_param_load = full_cal ? 1 : 0;
        wl_cfg->en_full_cal = full_cal ? 1 : 0;
        wl_cfg->mode = WL_API_MODE_ALL;
        wl_cfg->param.xtalfreq_hz = xtal_value;
        //wl_cfg->param.xtalcapcode_in = 32;
        //wl_cfg->param.xtalcapcode_out = 32;
        wl_cfg->capcode_set = rfparam_set_capcode;
        wl_cfg->capcode_get = rfparam_get_capcode;
        wl_cfg->param_load = rfparam_load;
      } while(0);
      break;

  case BRD_CTL_RF_INIT_WLAN:
    wl_cfg->mode = WL_API_MODE_WLAN;
    break;

  case BRD_CTL_RF_INIT_BZ:
    wl_cfg->mode = WL_API_MODE_BZ;
    break;

  case BRD_CTL_RF_INIT_ALL:
    wl_cfg->mode = WL_API_MODE_ALL;
    break;

  case BRD_CTL_RF_SET_XTAL:
    do {
      int xtal = va_arg(args, int);
      wl_cfg->param.xtalfreq_hz = xtal;
    } while (0);
    break;

  case BRD_CTL_RF_SET_CAPCODE:
    do {
      int cap_in = va_arg(args, int);
      int cap_out = va_arg(args, int);
      wl_cfg->param.xtalcapcode_in = cap_in;
      wl_cfg->param.xtalcapcode_out = cap_out;
    } while (0);
    break;

  default:
    break;
  }

  ret = wl_init();
  if (ret != WL_API_STATUS_OK) {
    return -EINVAL;
  }
  wl_cfg->en_param_load = 0;
  wl_cfg->en_full_cal = 0;

  return 0;
}

/* board configuration */
int board_rf_ctl(enum board_ctl_ops ops, ...)
{
  va_list ops_arg;
  int ret = -ENOSYS;

  va_start(ops_arg, ops);

  if (ops >= BRD_CTL_RF_RESET_DEFAULT && ops <= BRD_CTL_RF_SET_CAPCODE) {
    ret = ctl_rf_configuration(ops, ops_arg);
  } else {
    va_end(ops_arg);
    return -ENOSYS;
  }

  va_end(ops_arg);
  return ret;
}
