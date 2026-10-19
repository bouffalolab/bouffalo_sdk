#include <string.h>
#include "bl616cl_ext_dcdc.h"
#include "bl616cl_hbn.h"
#include "bl616cl_pm.h"
#include "bl616cl_pds.h"
#include "bl616cl_glb.h"
#include "bflb_gpio.h"

static const char *bl616cl_power_supply_mode_name(void)
{
    switch (BL616CL_POWER_SUPPLY_MODE) {
        case BL616CL_POWER_SUPPLY_INTERNAL_LDO:
            return "internal_ldo";
        case BL616CL_POWER_SUPPLY_EXT_DCDC_SOC:
            return "ext_dcdc_soc";
        case BL616CL_POWER_SUPPLY_EXT_DCDC_SYS:
            return "ext_dcdc_sys";
        case BL616CL_POWER_SUPPLY_EXT_DCDC_SOC_SYS:
            return "ext_dcdc_soc_sys";
        default:
            return "unknown";
    }
}

static uint8_t bl616cl_soc_ext_dcdc_ctrl_pin_get(void)
{
#if defined(PM_EXT_DCDC_SOC_CTRL_PIN) && (PM_EXT_DCDC_SOC_CTRL_PIN != BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED)
    return PM_EXT_DCDC_SOC_CTRL_PIN;
#else
    return BL_EXT_DCDC_OUTPUT_CTRL_PIN;
#endif
}

void bl_ext_dcdc_apply_power_supply_mode(void)
{
    int soc_ext_power = BL616CL_POWER_SUPPLY_SOC_USE_EXT_DCDC(BL616CL_POWER_SUPPLY_MODE);
    int sys_ext_power = BL616CL_POWER_SUPPLY_SYS_USE_EXT_DCDC(BL616CL_POWER_SUPPLY_MODE);
    uint8_t soc_ctrl_pin = bl616cl_soc_ext_dcdc_ctrl_pin_get();

    printf("[PMU] BL616CL power supply mode: %s (%d)\r\n",
           bl616cl_power_supply_mode_name(),
           BL616CL_POWER_SUPPLY_MODE);
    printf("[PMU] SOC external power: %s\r\n", soc_ext_power ? "enabled" : "disabled");
    printf("[PMU] SYS external power: %s\r\n", sys_ext_power ? "enabled" : "disabled");

    if (sys_ext_power) {
        HBN_SW_Set_Ldo_Sys_Vout(HBN_LDO_SYS_LEVEL_1P200V);
    }

    if (soc_ext_power) {
        if (soc_ctrl_pin != BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED) {
            printf("[PMU] SOC EXT DCDC ctrl GPIO: GPIO_%d\r\n",
                   soc_ctrl_pin);
            static struct bflb_device_s *gpio;
            gpio = bflb_device_get_by_name("gpio");
            bflb_gpio_init(gpio, soc_ctrl_pin, GPIO_OUTPUT | GPIO_FLOAT | GPIO_DRV_0);
            bflb_gpio_set(gpio, soc_ctrl_pin);
            // pm_pds_dcdc_soc_set_0v9(soc_ctrl_pin);
        } else {
            printf("[PMU] SOC EXT DCDC ctrl GPIO: disabled\r\n");
        }

        HBN_SW_Set_Ldo_Soc_Vout(HBN_LDO_SOC_LEVEL_0P650V);
    }
}

#if (BL_EXT_DCDC_OUTPUT_CTRL_PIN != BL_EXT_DCDC_OUTPUT_CTRL_PIN_DISABLED)
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

    return ret;
}
#endif
