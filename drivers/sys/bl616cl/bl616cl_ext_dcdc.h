#ifndef __BL616CL_EXT_DCDC_H__
#define __BL616CL_EXT_DCDC_H__

#include <stdint.h>

#define BL_EXT_DCDC_KEEP_PIN_MAX 4
#define BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED 0xFF

#define BL616CL_POWER_SUPPLY_INTERNAL_LDO     0
#define BL616CL_POWER_SUPPLY_EXT_DCDC_SOC     1
#define BL616CL_POWER_SUPPLY_EXT_DCDC_SYS     2
#define BL616CL_POWER_SUPPLY_EXT_DCDC_SOC_SYS 3

#ifndef BL_EXT_DCDC_OUTPUT_CTRL_PIN
#define BL_EXT_DCDC_OUTPUT_CTRL_PIN BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED
#endif

#ifndef BL616CL_POWER_SUPPLY_MODE
#define BL616CL_POWER_SUPPLY_MODE BL616CL_POWER_SUPPLY_INTERNAL_LDO
#endif

#define BL616CL_POWER_SUPPLY_SOC_USE_EXT_DCDC(mode) \
    (((mode) == BL616CL_POWER_SUPPLY_EXT_DCDC_SOC) || ((mode) == BL616CL_POWER_SUPPLY_EXT_DCDC_SOC_SYS))

#define BL616CL_POWER_SUPPLY_SYS_USE_EXT_DCDC(mode) \
    (((mode) == BL616CL_POWER_SUPPLY_EXT_DCDC_SYS) || ((mode) == BL616CL_POWER_SUPPLY_EXT_DCDC_SOC_SYS))

#define BL616CL_POWER_SUPPLY_USE_EXT_DCDC(mode) \
    (mode != BL616CL_POWER_SUPPLY_INTERNAL_LDO)

void bl_ext_dcdc_apply_power_supply_mode(void);

#if (BL_EXT_DCDC_OUTPUT_CTRL_PIN != BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED)
typedef int (*bl_ext_dcdc_pds_cb_t)(void *arg);

typedef struct {
    uint8_t keep_pin_num;
    uint8_t keep_pins[BL_EXT_DCDC_KEEP_PIN_MAX];
    bl_ext_dcdc_pds_cb_t enter_pds_cb;
    void *enter_pds_arg;
    bl_ext_dcdc_pds_cb_t exit_pds_cb;
    void *exit_pds_arg;
} bl_ext_dcdc_pds_cfg_t;

int bl_ext_dcdc_pds_register(const bl_ext_dcdc_pds_cfg_t *cfg);
void bl_ext_dcdc_pds_unregister(void);
int bl_ext_dcdc_pds_prepare_enter(void);
int bl_ext_dcdc_pds_restore_exit(void);
#endif
#endif
