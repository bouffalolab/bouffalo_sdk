#ifndef __BL616L_XIP_RECOVERY_H__
#define __BL616L_XIP_RECOVERY_H__
#include <stdint.h>

typedef struct {
    void *flash_cfg;
    void *flash_io_cs_clk_delay_cfg;
    uint32_t flash_jdec_id;
    uint32_t flash_offset;
    uint8_t flash_clk;
    uint8_t flash_clk_div;
    uint8_t flash_pin_cfg;
    uint8_t do_xip_recovery;
} sf_recovery_para_t;

void bl_lp_xip_para_save(void);
void bl_lp_xip_recovery(void);

#endif
