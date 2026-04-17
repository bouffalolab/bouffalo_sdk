#ifndef __BL616CL_EXT_DCDC_H__
#define __BL616CL_EXT_DCDC_H__

#include <stdint.h>

#define BL_EXT_DCDC_KEEP_PIN_MAX 4
#define BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED 0xFF

#ifndef BL_EXT_DCDC_OUTPUT_CTRL_PIN
#define BL_EXT_DCDC_OUTPUT_CTRL_PIN BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED
#endif

typedef int (*bl_ext_dcdc_pds_cb_t)(void *arg);

typedef struct {
    uint8_t keep_pin_num;
    uint8_t keep_pins[BL_EXT_DCDC_KEEP_PIN_MAX];
    uint16_t settle_time_us;
    bl_ext_dcdc_pds_cb_t enter_pds_cb;
    void *enter_pds_arg;
    bl_ext_dcdc_pds_cb_t exit_pds_cb;
    void *exit_pds_arg;
} bl_ext_dcdc_pds_cfg_t;

int bl_ext_dcdc_pds_register(const bl_ext_dcdc_pds_cfg_t *cfg);
void bl_ext_dcdc_pds_unregister(void);
int bl_ext_dcdc_pds_is_keep_pin(uint8_t pin);
int bl_ext_dcdc_pds_prepare_enter(void);
int bl_ext_dcdc_pds_restore_exit(void);

#endif
