#ifndef __BL616_LP_INTERNAL_H__
#define __BL616_LP_INTERNAL_H__

#include "bflb_core.h"
#include "bl616_pm.h"
#include "bflb_rtc.h"
#include "bl_sys.h"
#include "bl616_pds.h"
#include "bl616_hbn.h"
#include "hardware/timer_reg.h"
#include "ef_data_reg.h"
#include "bl616_glb_gpio.h"
#include "bflb_dma.h"
#include "bl616_aon.h"
#include "bflb_acomp.h"
#include "bflb_sec_sha.h"
#include "bl616_tzc_sec.h"
#include "tzc_sec_reg.h"
#include "bl616_l1c.h"
#include "bl616_clock.h"
#include "bl616_xip_recovery.h"

#if (0 && !defined(BL_WIFI_LP_FW))
#define BL_LP_LOG        printf
#define BL_LP_TIME_DEBUG 0
#else
#define BL_LP_LOG(...)
#define BL_LP_TIME_DEBUG 0
#endif

extern int32_t lpfw_calculate_beacon_delay(lp_fw_bcn_delay_t *p_bcn_delay, uint64_t beacon_timestamp_us,
                                           uint64_t rtc_timestamp_us, uint32_t mode);
extern int32_t lpfw_beacon_delay_sliding_win_get_average(lp_fw_bcn_delay_t *p_beacon_delay_win);
extern int32_t AON_Set_LDO11_SOC_Sstart_Delay(uint8_t delay);

#ifdef BL_WIFI_LP_FW
extern uint64_t (*shared_CPU_Get_MTimer_Counter)(void);
extern void (*shared_arch_delay_ms)(uint32_t);
extern void (*shared_arch_delay_us)(uint32_t);
extern int32_t (*shared_CPU_Reset_MTimer)(void);
extern int32_t (*shared_lpfw_calculate_beacon_delay)(uint32_t *, uint64_t, uint64_t, uint32_t);
extern int32_t (*shared_lpfw_beacon_delay_sliding_win_get_average)(uint32_t *);
extern int32_t (*shared_AON_Set_LDO11_SOC_Sstart_Delay)(uint32_t);
extern int32_t (*shared_PDS_Default_Level_Config)(uint32_t *, uint32_t);
#endif

extern uint32_t *shared_func_array[32];

void shared_func_init(void);
void bl_lp_fw_wifi_init(void);
void bl_lp_fw_bcn_delay_init(void);
void bl_lp_fw_bcn_loss_init(void);
void bl_lp_io_wakeup_init(bl_lp_io_cfg_t *io_wakeup_cfg);
void bl_lp_acomp_wakeup_init(bl_lp_acomp_cfg_t *acomp_wakeup_cfg);
int bl_lp_wakeup_check_allow(void);
uint64_t bl_lp_check_gpio_int(void);
uint8_t bl_lp_check_acomp_int(void);

struct lp_env *bl_lp_env_get(void);
void bl_lp_env_init(void);
void bl616_lp_soft_irq_trigger(void);
void bl616_lp_wakeup_prepare(void);

#endif
