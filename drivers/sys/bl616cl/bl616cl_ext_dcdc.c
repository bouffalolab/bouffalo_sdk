#include <string.h>
#include "bl616cl_ext_dcdc.h"
#include "bl616cl_pm.h"
#include "bl616cl_pds.h"
#include "bl616cl_glb.h"

static bl_ext_dcdc_pds_cfg_t g_ext_dcdc_cfg;
static uint8_t g_ext_dcdc_enabled;

static int bl_ext_dcdc_validate_cfg(const bl_ext_dcdc_pds_cfg_t *cfg)
{
    if (cfg == NULL) {
        return -1;
    }

    if (cfg->keep_pin_num > BL_EXT_DCDC_KEEP_PIN_MAX) {
        return -1;
    }

    for (uint8_t i = 0; i < cfg->keep_pin_num; i++) {
        if (cfg->keep_pins[i] >= GPIO_PIN_MAX) {
            return -1;
        }
    }

    return 0;
}

static void bl_ext_dcdc_keep_pin_enable(void)
{
    for (uint8_t i = 0; i < g_ext_dcdc_cfg.keep_pin_num; i++) {
        PDS_Enable_GPIO_Keep(g_ext_dcdc_cfg.keep_pins[i]);
    }
}

static void bl_ext_dcdc_keep_pin_disable(void)
{
    for (uint8_t i = 0; i < g_ext_dcdc_cfg.keep_pin_num; i++) {
        pm_disable_gpio_keep(g_ext_dcdc_cfg.keep_pins[i]);
    }
}

int bl_ext_dcdc_pds_register(const bl_ext_dcdc_pds_cfg_t *cfg)
{
    if (bl_ext_dcdc_validate_cfg(cfg) != 0) {
        return -1;
    }

    memcpy(&g_ext_dcdc_cfg, cfg, sizeof(g_ext_dcdc_cfg));
    g_ext_dcdc_enabled = 1;

    return 0;
}

void bl_ext_dcdc_pds_unregister(void)
{
    memset(&g_ext_dcdc_cfg, 0, sizeof(g_ext_dcdc_cfg));
    g_ext_dcdc_enabled = 0;
}

int bl_ext_dcdc_pds_is_keep_pin(uint8_t pin)
{
    if (!g_ext_dcdc_enabled) {
        return 0;
    }

    for (uint8_t i = 0; i < g_ext_dcdc_cfg.keep_pin_num; i++) {
        if (g_ext_dcdc_cfg.keep_pins[i] == pin) {
            return 1;
        }
    }

    return 0;
}

int bl_ext_dcdc_pds_prepare_enter(void)
{
    int ret = 0;

    if (!g_ext_dcdc_enabled) {
        return 0;
    }

    if (g_ext_dcdc_cfg.enter_pds_cb) {
        ret = g_ext_dcdc_cfg.enter_pds_cb(g_ext_dcdc_cfg.enter_pds_arg);
        if (ret != 0) {
            return ret;
        }
    }

    bl_ext_dcdc_keep_pin_enable();

    if (g_ext_dcdc_cfg.settle_time_us) {
        arch_delay_us(g_ext_dcdc_cfg.settle_time_us);
    }

    return 0;
}

int bl_ext_dcdc_pds_restore_exit(void)
{
    int ret = 0;

    if (!g_ext_dcdc_enabled) {
        return 0;
    }

    if (g_ext_dcdc_cfg.exit_pds_cb) {
        ret = g_ext_dcdc_cfg.exit_pds_cb(g_ext_dcdc_cfg.exit_pds_arg);
    }

    bl_ext_dcdc_keep_pin_disable();

    return ret;
}
