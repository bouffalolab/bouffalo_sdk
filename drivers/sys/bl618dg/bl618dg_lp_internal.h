#ifndef __BL618DG_LP_INTERNAL_H__
#define __BL618DG_LP_INTERNAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bl618dg_lp.h"
#include "bl618dg_hbn.h"
#include "bl618dg_pds.h"
#include "bl618dg_pm.h"
#include "bl618dg_xip_recovery.h"

#if (1)
#define BL_LP_LOG        printf
#define BL_LP_TIME_DEBUG 0
#else
#define BL_LP_LOG(...)
#define BL_LP_TIME_DEBUG 0
#endif

typedef lp_gpio_cfg_type lp_fw_gpio_cfg_t;

void bl_lp_io_wakeup_init(lp_fw_gpio_cfg_t *cfg);

struct lp_env *bl_lp_env_get(void);
void bl_lp_env_init(void);
void bl618dg_lp_soft_irq_trigger(void);
void bl618dg_lp_io_wakeup_prepare(void);

void lp_fw_save_cpu_para(uint32_t save_addr);
void lp_fw_restore_cpu_para(uint32_t save_addr);

#endif
