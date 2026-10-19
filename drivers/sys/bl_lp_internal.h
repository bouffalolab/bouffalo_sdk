#ifndef __BL_LP_INTERNAL_H__
#define __BL_LP_INTERNAL_H__

#include <assert.h>
#include "bl_lp.h"

#if defined(BL616)
#include "bl616_lp_internal.h"
#elif defined(BL616CL)
#include "bl616cl_lp_internal.h"
#elif defined(BL618DG)
#include "bl618dg_lp_internal.h"
#endif

/*
 * Common internal definitions for LP (Low Power) firmware.
 */

#define GET_OFFSET(_type, _member) ((unsigned long)(&((_type *)0)->_member))

struct lp_env {
    void *sys_enter_arg;
    void *user_enter_arg;
    void *sys_exit_arg;
    void *user_exit_arg;

    bl_lp_cb_t sys_pre_enter_callback;
    bl_lp_cb_t user_pre_enter_callback;
    bl_lp_cb_t sys_after_exit_callback;
    bl_lp_cb_t user_after_exit_callback;

    uint8_t wifi_hw_resume;
    uint8_t wifi_fw_ready;
    uint32_t gpio_stat;
};

extern iot2lp_para_t *const iot2lp_para;
extern bl_lp_fw_cfg_t lpfw_cfg;

struct bl_lp_bcn_loss_cfg_desc {
    const lp_fw_bcn_loss_level_t *cfg_table;
    int32_t table_num;
    int32_t loop_start;
    uint32_t loss_max;
};

extern uint32_t *export_get_rx_buffer1_addr(void);

int bl_lpfw_bin_check(void);
int bl_lpfw_ram_verify(void);
void bl_lp_fw_bcn_loss_cfg_dtim_default_common(uint8_t dtim_num, const struct bl_lp_bcn_loss_cfg_desc *dtim1_cfg,
                                               const struct bl_lp_bcn_loss_cfg_desc *dtim3_cfg,
                                               const struct bl_lp_bcn_loss_cfg_desc *dtim6_cfg,
                                               const struct bl_lp_bcn_loss_cfg_desc *dtim10_cfg);
void bl_lp_fw_bcn_loss_cfg_dtim_default(uint8_t dtim_num);

#endif
