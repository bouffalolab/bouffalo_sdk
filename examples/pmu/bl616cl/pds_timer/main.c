#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_uart.h"
#include "board.h"
#include "shell.h"
#include "bl616cl_glb.h"
#include "bl616cl_pds.h"
#include "bl616cl_hbn.h"
#include "bl616cl_aon.h"
#include "bl616cl_pm.h"

#include <stdlib.h>
#include <string.h>

#define SLEEP_TIME  (32768)

uint32_t pds_level = PM_PDS_LEVEL_15;
uint32_t wakeup_count;
uint32_t sf_pin_select;
struct bflb_device_s *gpio;
struct bflb_device_s *uart0;
static PM_LOWPOWER_CFG_Type app_lowpower_cfg;

static int app_lowpower_mode_load(uint8_t mode)
{
    const PM_LOWPOWER_CFG_Type *mode_cfg;

    mode_cfg = pm_power_mode_cfg_get(mode);
    if (mode_cfg == NULL) {
        return -1;
    }

    app_lowpower_cfg = *mode_cfg;

    return 0;
}

/* Get flash IO select from efuse */
static uint32_t get_sf_pin_select(void)
{
    uint32_t tmpVal;
    /* Read from efuse sw usage 0 */
    tmpVal = BL_RD_WORD(0x20056000 + 0x74);
    return (tmpVal >> 5) & 0x3f;
}

/* Check if the pin is flash IO */
static int is_flash_io(uint8_t pin, uint32_t sf_pin_select)
{
    if (sf_pin_select & (1 << 2)) {
        /* Flash IO is GPIO 6-11 */
        return (pin >= GPIO_PIN_6 && pin <= GPIO_PIN_11);
    } else if (sf_pin_select & (1 << 3)) {
        /* Flash IO is GPIO 24-29 */
        return (pin >= GPIO_PIN_24 && pin <= GPIO_PIN_29);
    }
    return 0;
}

static void pds_timer_enter_once(uint32_t level, uint32_t sleep_time)
{
    uint8_t flash_io_start = 0, flash_io_end = 0;

    /* Get flash IO configuration from efuse */
    sf_pin_select = get_sf_pin_select();
    if (sf_pin_select & (1 << 2)) {
        flash_io_start = GPIO_PIN_6;
        flash_io_end = GPIO_PIN_11;
        printf("Flash IO: GPIO %d-%d\r\n", flash_io_start, flash_io_end);
    } else if (sf_pin_select & (1 << 3)) {
        flash_io_start = GPIO_PIN_24;
        flash_io_end = GPIO_PIN_29;
        printf("Flash IO: GPIO %d-%d\r\n", flash_io_start, flash_io_end);
    } else {
        printf("Flash IO: internal or unknown config (sf_pin_select=0x%x)\r\n", sf_pin_select);
    }

    printf("PDS wake up by timer start\r\n");

    PDS_Mask_All_Wakeup_Src();
    HBN_Pin_WakeUp_Mask(0xF);
    wakeup_count = HBN_Get_Version();
    printf("wake up count = %u, ", wakeup_count);
    HBN_Set_Version(wakeup_count + 1);

    if (level == PM_PDS_LEVEL_1) {
        pm_pds_irq_register();
    }
    printf("enter pds_%u mode\r\n", level);

    /* Wait for all log output complete before configuring IO */
    arch_delay_ms(10);

    /* Configure all PDS GPIO (6-36) to analog + float mode, except flash IO */
    /* Enable all PDS GPIO keep for low power consumption */
    for (uint8_t i = GPIO_PIN_6; i < GPIO_PIN_MAX; i++) {
        if (!is_flash_io(i, sf_pin_select)) {
            bflb_gpio_init(gpio, i, GPIO_ANALOG | GPIO_FLOAT | GPIO_DRV_0);
            PDS_Enable_GPIO_Keep(i);
        }
    }

    arch_delay_us(200);
    pm_pds_mode_enter(level, sleep_time, &app_lowpower_cfg);
}

static int parse_u8_arg(const char *arg, uint8_t *value)
{
    char *endptr;
    unsigned long val;

    if ((arg == NULL) || (value == NULL)) {
        return -1;
    }

    val = strtoul(arg, &endptr, 0);
    if ((endptr == arg) || (*endptr != '\0') || (val > 0xFF)) {
        return -1;
    }

    *value = (uint8_t)val;
    return 0;
}

static int cmd_pm_power(int argc, char **argv)
{
    uint8_t mode;
    uint8_t dcdc_sys_enable_pin;
    uint8_t dcdc_soc_enable_pin;
    uint8_t dcdc_soc_vsel_pin;
    uint8_t lp_mask;
    uint8_t pds_clk;
    const PM_LOWPOWER_CFG_Type *cfg;
    PM_LOWPOWER_CFG_Type new_cfg;

    if ((argc == 2) && (strcmp(argv[1], "list") == 0)) {
        for (uint8_t i = 0; i < PM_POWER_MODE_MAX; i++) {
            cfg = pm_power_mode_cfg_get(i);
            if (cfg != NULL) {
                printf("%u: %s\r\n", i, cfg->name);
            }
        }
        return 0;
    }

    if ((argc == 2) && (strcmp(argv[1], "get") == 0)) {
        printf("pm_power cfg: %s\r\n", app_lowpower_cfg.name ? app_lowpower_cfg.name : "");
        printf("  dcdc_sys_gpio          : 0x%02x\r\n", app_lowpower_cfg.sys_cfg.dcdc_sys_enable_pin);
        printf("  dcdc_sys_pds_enable    : %u\r\n", app_lowpower_cfg.sys_cfg.dcdc_sys_pds_enable);
        printf("  ldo_sys_active_level   : %u\r\n", app_lowpower_cfg.sys_cfg.ldo_sys_active_level);
        printf("  ldo_sys_pds_level      : %u\r\n", app_lowpower_cfg.sys_cfg.ldo_sys_pds_level);
        printf("  dcdc_soc_gpio          : 0x%02x\r\n", app_lowpower_cfg.soc_cfg.dcdc_soc_enable_pin);
        printf("  dcdc_soc_vsel_gpio     : 0x%02x\r\n", app_lowpower_cfg.soc_cfg.dcdc_soc_vsel_pin);
        printf("  dcdc_soc_pds_enable    : %u\r\n", app_lowpower_cfg.soc_cfg.dcdc_soc_pds_enable);
        printf("  dcdc_soc_pds_level     : %u\r\n", app_lowpower_cfg.soc_cfg.dcdc_soc_pds_level);
        printf("  ldo_soc_active_level   : %u\r\n", app_lowpower_cfg.soc_cfg.ldo_soc_active_level);
        printf("  ldo_soc_enter_pds      : %u\r\n", app_lowpower_cfg.soc_cfg.ldo_soc_enter_pds_level);
        printf("  ldo_soc_pds_level      : %u\r\n", app_lowpower_cfg.soc_cfg.ldo_soc_pds_level);
        printf("  pds_gpio_keep_en       : %u\r\n", app_lowpower_cfg.lp_cfg.pds_gpio_keep_en);
        printf("  hbn_gpio_keep_en       : %u\r\n", app_lowpower_cfg.lp_cfg.hbn_gpio_keep_en);
        printf("  pds_flash_power_off    : %u\r\n", app_lowpower_cfg.lp_cfg.pds_flash_power_off);
        printf("  pll_power_off          : %u\r\n", app_lowpower_cfg.lp_cfg.pll_power_off);
        printf("  rf_power_off           : %u\r\n", app_lowpower_cfg.lp_cfg.rf_power_off);
        printf("  clk_default_sel        : %u\r\n", app_lowpower_cfg.lp_cfg.clk_default_sel);
        printf("  set_all_ram_ret_en     : %u\r\n", app_lowpower_cfg.lp_cfg.set_all_ram_ret_en);
        printf("  hbn_flash_power_off    : %u\r\n", app_lowpower_cfg.lp_cfg.hbn_flash_power_off);
        printf("  ldo18io_power_down     : %u\r\n", app_lowpower_cfg.lp_cfg.ldo18io_power_down);
        return 0;
    }

    if ((argc == 5) || (argc == 7)) {
        if ((parse_u8_arg(argv[1], &mode) != 0) ||
            (parse_u8_arg(argv[2], &dcdc_sys_enable_pin) != 0) ||
            (parse_u8_arg(argv[3], &dcdc_soc_enable_pin) != 0) ||
            (parse_u8_arg(argv[4], &dcdc_soc_vsel_pin) != 0)) {
            printf("invalid argument\r\n");
            return -1;
        }

        cfg = pm_power_mode_cfg_get(mode);
        if (cfg == NULL) {
            printf("invalid mode\r\n");
            return -1;
        }
        new_cfg = *cfg;

        if (((new_cfg.sys_cfg.dcdc_sys_enable_pin == 0xFF) && (dcdc_sys_enable_pin != 0xFF)) ||
            ((new_cfg.soc_cfg.dcdc_soc_enable_pin == 0xFF) && (dcdc_soc_enable_pin != 0xFF)) ||
            ((new_cfg.soc_cfg.dcdc_soc_vsel_pin == 0xFF) && (dcdc_soc_vsel_pin != 0xFF)) ||
            ((dcdc_sys_enable_pin != 0xFF) && (dcdc_sys_enable_pin >= GPIO_PIN_MAX)) ||
            ((dcdc_soc_enable_pin != 0xFF) && (dcdc_soc_enable_pin >= GPIO_PIN_MAX)) ||
            ((dcdc_soc_vsel_pin != 0xFF) && (dcdc_soc_vsel_pin >= GPIO_PIN_MAX)) ||
            ((dcdc_sys_enable_pin != 0xFF) && (dcdc_sys_enable_pin == dcdc_soc_enable_pin)) ||
            ((dcdc_sys_enable_pin != 0xFF) && (dcdc_sys_enable_pin == dcdc_soc_vsel_pin)) ||
            ((dcdc_soc_enable_pin != 0xFF) && (dcdc_soc_enable_pin == dcdc_soc_vsel_pin))) {
            printf("invalid GPIO config for current mode\r\n");
            return -1;
        }

        new_cfg.sys_cfg.dcdc_sys_enable_pin = dcdc_sys_enable_pin;
        new_cfg.soc_cfg.dcdc_soc_enable_pin = dcdc_soc_enable_pin;
        new_cfg.soc_cfg.dcdc_soc_vsel_pin = dcdc_soc_vsel_pin;

        if (argc == 7) {
            if ((parse_u8_arg(argv[5], &lp_mask) != 0) ||
                (parse_u8_arg(argv[6], &pds_clk) != 0) ||
                (pds_clk > PM_PDS_CLK_RC16M)) {
                printf("invalid LP config\r\n");
                return -1;
            }

            new_cfg.lp_cfg.pds_gpio_keep_en = (lp_mask >> 0) & 0x1;
            new_cfg.lp_cfg.hbn_gpio_keep_en = (lp_mask >> 1) & 0x1;
            new_cfg.lp_cfg.pds_flash_power_off = (lp_mask >> 2) & 0x1;
            new_cfg.lp_cfg.pll_power_off = (lp_mask >> 3) & 0x1;
            new_cfg.lp_cfg.rf_power_off = (lp_mask >> 4) & 0x1;
            new_cfg.lp_cfg.clk_default_sel = pds_clk;
            new_cfg.lp_cfg.set_all_ram_ret_en = (lp_mask >> 5) & 0x1;
            new_cfg.lp_cfg.hbn_flash_power_off = (lp_mask >> 6) & 0x1;
            new_cfg.lp_cfg.ldo18io_power_down = (lp_mask >> 7) & 0x1;
        }

        app_lowpower_cfg = new_cfg;
        pm_dcdc_sys_exit_pds(&app_lowpower_cfg.sys_cfg);
        pm_dcdc_soc_exit_pds(&app_lowpower_cfg.soc_cfg);
        printf("power config updated: %u %s\r\n", mode, app_lowpower_cfg.name ? app_lowpower_cfg.name : "");
        return 0;
    }

    {
        printf("usage: pm_power list\r\n");
        printf("usage: pm_power get\r\n");
        printf("usage: pm_power <mode> <dcdc_sys_gpio> <dcdc_soc_gpio> <dcdc_soc_vsel_gpio>\r\n");
        printf("usage: pm_power <mode> <dcdc_sys_gpio> <dcdc_soc_gpio> <dcdc_soc_vsel_gpio> <lp_mask> <pds_clk>\r\n");
        printf("       use 0xff for unused GPIO\r\n");
        printf("       lp_mask bit0=pds_gpio_keep bit1=hbn_gpio_keep bit2=pds_flash_off bit3=pll_off bit4=rf_off bit5=ram_ret bit6=hbn_flash_off bit7=ldo18io_down\r\n");
        printf("       pds_clk: 0=f32k, 1=rc32m, 2=xtal, 3=xtal_lp, 4=rc8m, 5=rc16m\r\n");
        return -1;
    }
}

static int cmd_pds_enter(int argc, char **argv)
{
    uint32_t level = PM_PDS_LEVEL_15;
    uint32_t sleep_time = SLEEP_TIME;

    if (argc >= 2) {
        level = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        sleep_time = (uint32_t)strtoul(argv[2], NULL, 0);
    }

    pds_timer_enter_once(level, sleep_time);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_pm_power, pm_power, cmd pm_power);
SHELL_CMD_EXPORT_ALIAS(cmd_pds_enter, pds_enter, pds enter <level> [sleep_cnt]);

int main(void)
{
    int ch;

    PDS_Disable_ALL_GPIO_Keep();
    board_init();
    app_lowpower_mode_load(PM_LDO13_LDO07_PDSLDO07);
    HBN_32K_Sel(HBN_32K_RC);

    gpio = bflb_device_get_by_name("gpio");
    uart0 = bflb_device_get_by_name("uart0");

    printf("PDS timer shell start\r\n");
    printf("Use pm_power to configure power, then pds_enter 15 32768\r\n");
    shell_init();

    while (1) {
        if ((ch = bflb_uart_getchar(uart0)) != -1) {
            shell_handler(ch);
        }
    }
}
