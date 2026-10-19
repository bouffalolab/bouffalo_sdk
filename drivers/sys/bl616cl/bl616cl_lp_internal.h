#ifndef __BL616CL_LP_INTERNAL_H__
#define __BL616CL_LP_INTERNAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bl616cl_lp.h"
#include "bl616cl_hbn.h"
#include "bl616cl_pds.h"
#include "bl616cl_pm.h"
#include "bl616cl_psram.h"
#include "bl616cl_xip_recovery.h"
#include "bl616cl_psram_recovery.h"
#include "hardware/uart_reg.h"

#if (1 && !defined(BL_WIFI_LP_FW))
#define BL_LP_LOG        printf
#define BL_LP_TIME_DEBUG 1
#else
#define BL_LP_LOG(...)
#define BL_LP_TIME_DEBUG 0
#endif

typedef lp_gpio_cfg_type lp_fw_gpio_cfg_t;

void *pvPortMalloc(size_t xSize);

void bl_lp_io_wakeup_init(lp_fw_gpio_cfg_t *cfg);
void bl616cl_lp_io_wakeup_prepare(void);
void bl616cl_lp_soft_irq_trigger(void);

void lp_fw_save_cpu_para(uint32_t save_addr);
void ATTR_TCM_SECTION __attribute__((aligned(16))) lp_fw_restore_cpu_para(uint32_t save_addr);

struct lp_env *bl_lp_env_get(void);
void bl_lp_env_init(void);

#if (BL_LP_TIME_DEBUG)
extern lp_fw_time_debug_t time_debug_buff[TIME_DEBUG_NUM_MAX];
#endif

#endif
