#ifndef __RFPARAM_ADAPTER_H__
#define __RFPARAM_ADAPTER_H__

#include "rfparam_rftlv.h"
#include "wl_api.h"

/* Standalone component builds (e.g. the Bluetooth controller library) do not
 * include the SDK-generated autoconfig.h; keep the v2 compatibility entries
 * enabled when the symbol is absent. */
#ifndef CONFIG_COMPAT_V2
#define CONFIG_COMPAT_V2 1
#endif

/* err code */
#define RFPARAM_SUSS                           0
#define RFPARAM_ERR_INVALID                    -1
#define RFPARAM_ERR_PWR_MODE_NULL              -2
#define RFPARAM_ERR_PWR_OFFSET_NULL            -3
#define RFPARAM_ERR_PWR_11B_NULL               -4
#define RFPARAM_ERR_PWR_11G_NULL               -5
#define RFPARAM_ERR_PWR_11N_NULL               -6
#define RFPARAM_ERR_CAP_CODE_MODE_NULL         -7
#define RFPARAM_ERR_CAP_CODE_NULL              -8
#define RFPARAM_ERR_TCAL_EN_NULL               -9
#define RFPARAM_ERR_TCAL_LINEAR_OR_FOLLOW_NULL -10
#define RFPARAM_ERR_TCAL_CHANNELS_NULL         -11
#define RFPARAM_ERR_TCAL_CHANNEL_OS_NULL       -12
#define RFPARAM_ERR_TCAL_CHANNEL_OS_LOW_NULL   -13
#define RFPARAM_ERR_TCAL_ROOM_OS_NULL          -14
#define RFPARAM_ERR_PWR_BLE_NULL               -15
#define RFPARAM_ERR_PWR_OFFSET_BZ_NULL         -16
#define RFPARAM_ERR_PWR_11N_HT40_NULL          -17
#define RFPARAM_ERR_PWR_11AC_VHT20_NULL        -18
#define RFPARAM_ERR_PWR_11AC_VHT40_NULL        -19
#define RFPARAM_ERR_PWR_11AC_VHT80_NULL        -20
#define RFPARAM_ERR_PWR_11AX_HE20_NULL         -21
#define RFPARAM_ERR_PWR_11AX_HE40_NULL         -22
#define RFPARAM_ERR_PWR_11AX_HE80_NULL         -23
#define RFPARAM_ERR_PWR_11AX_HE160_NULL        -24
#define RFPARAM_ERR_PWR_BT_NULL                -25
#define RFPARAM_ERR_PWR_ZIGBEE_NULL            -26
#define RFPARAM_ERR_COUNTRY_CODE_NULL          -27
#define RFPARAM_ERR_TLV_BASE_ADDR              -28
#define RFPARAM_ERR_PWR_OFFSET_LP_NULL         -29
#define RFPARAM_ERR_EN_TCAP                    -30
#define RFPARAM_ERR_TCAP_TSEN                  -31
#define RFPARAM_ERR_TCAP_CAP                   -32

#define RFPARAM_ERR_PARAM_CHECK                -50

/* type flag */
#define TYPE_UINT16                            0
#define TYPE_INT16                             1
#define TYPE_UINT8                             2
#define TYPE_INT8                              3

/* tlv store type */
#define RFPARAM_TLV_BASE_ADDR_MASK             0xF0000000
#define RFPARAM_TLV_BASE_ADDR_RAM_NO_CACHE     0x22FC0400
#define RFPARAM_TLV_BASE_ADDR_RAM_CACHE        0x62FC0400
#define RFPARAM_TLV_BASE_ADDR_XIP_FLASH        0xA0000400

#define RFPARAM_WL_API_MEM_SIZE                544

/* Compatibility entry: empty when CONFIG_SYSINIT owns cold-start RF init. */
#if defined(CONFIG_COMPAT_V2) && CONFIG_COMPAT_V2
int32_t rfparam_init(uint32_t base_addr, void *rf_para, uint32_t apply_flag) __attribute__((deprecated(
    "handled by SYSINIT; will be removed in Bouffalo SDK v3")));
#endif /* defined(CONFIG_COMPAT_V2) && CONFIG_COMPAT_V2 */

struct wl_cfg_t *rfparam_cfg_get(void);
int32_t rfparam_get_wlan_pwroffset_with_option(uint32_t base_addr, int8_t pwr_offset[14], uint8_t reload);
int32_t rfparam_get_wlan_pwroffset_lp_with_option(uint32_t base_addr, int8_t pwr_offset[14], uint8_t reload);
int32_t rfparam_get_bz_pwroffset_with_option(uint32_t base_addr, int8_t pwr_offset[5], uint8_t reload);
int32_t rfparam_get_cap_code_with_option(uint32_t base_addr, uint8_t *capcode_in, uint8_t *capcode_out,
                                         uint8_t reloasd);
uint32_t rfparam_tlv_base_addr_get(void);

void rfparam_get_capcode(uint8_t *capcode_in, uint8_t *capcode_out);
void rfparam_set_capcode(uint8_t capcode_in, uint8_t capcode_out);
struct wl_param_t;
int8_t rfparam_load(struct wl_param_t *param);

#endif
