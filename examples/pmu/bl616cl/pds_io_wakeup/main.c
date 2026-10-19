#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "shell.h"
#include "board.h"
#include "log.h"
#include <assert.h>
#include "bl616cl_glb.h"
#include "bl616cl_pds.h"
#include "bl616cl_hbn.h"
#include "bl616cl_aon.h"
#include "bl616cl_pm.h"
#include "bflb_flash.h"

#include <stdlib.h>
#include <string.h>

volatile lp_gpio_cfg_type lp_wake_io_cfg;

struct bflb_device_s *gpio;
static PM_LOWPOWER_CFG_Type app_lowpower_cfg;

static int app_lowpower_mode_load(uint8_t mode)
{
    const PM_LOWPOWER_CFG_Type *mode_cfg = pm_power_mode_cfg_get(mode);

    if (mode_cfg == NULL) {
        return -1;
    }

    app_lowpower_cfg = *mode_cfg;
    return 0;
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

    printf("usage: pm_power list\r\n");
    printf("usage: pm_power get\r\n");
    printf("usage: pm_power <mode> <dcdc_sys_gpio> <dcdc_soc_gpio> <dcdc_soc_vsel_gpio>\r\n");
    printf("usage: pm_power <mode> <dcdc_sys_gpio> <dcdc_soc_gpio> <dcdc_soc_vsel_gpio> <lp_mask> <pds_clk>\r\n");
    printf("       use 0xff for unused GPIO\r\n");
    printf("       lp_mask bit0=pds_gpio_keep bit1=hbn_gpio_keep bit2=pds_flash_off bit3=pll_off bit4=rf_off bit5=ram_ret bit6=hbn_flash_off bit7=ldo18io_down\r\n");
    printf("       pds_clk: 0=f32k, 1=rc32m, 2=xtal, 3=xtal_lp, 4=rc8m, 5=rc16m\r\n");
    return -1;
}

static int app_power_gpio_conflicts_with(uint32_t pin)
{
    return (pin == app_lowpower_cfg.sys_cfg.dcdc_sys_enable_pin) ||
           (pin == app_lowpower_cfg.soc_cfg.dcdc_soc_enable_pin) ||
           (pin == app_lowpower_cfg.soc_cfg.dcdc_soc_vsel_pin);
}

static void app_print_wakeup_source(void)
{
    if (SET == PDS_Get_IntStatus(PDS_INT_WAKEUP)) {
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_PDS_GPIO_INT)) {
            for (uint32_t i = 6; i < GPIO_PIN_MAX; i++) {
                if (PDS_Get_GPIO_Pad_IntStatus(i)) {
                    printf("gpio_%d wakeup pds\r\n", i);
                    PDS_Set_GPIO_Pad_IntClr(i);
                    bflb_gpio_int_clear(gpio, i);
                }
            }
        }

        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_HBN_IRQ_OUT)) {
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO0)) {
                HBN_Clear_IRQ(HBN_INT_GPIO0);
                printf("gpio_%d wakeup pds\r\n", 0);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO1)) {
                HBN_Clear_IRQ(HBN_INT_GPIO1);
                printf("gpio_%d wakeup pds\r\n", 1);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO2)) {
                HBN_Clear_IRQ(HBN_INT_GPIO2);
                printf("gpio_%d wakeup pds\r\n", 2);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO3)) {
                HBN_Clear_IRQ(HBN_INT_GPIO3);
                printf("gpio_%d wakeup pds\r\n", 3);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO4)) {
                HBN_Clear_IRQ(HBN_INT_GPIO4);
                printf("gpio_%d wakeup pds\r\n", 4);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO5)) {
                HBN_Clear_IRQ(HBN_INT_GPIO5);
                printf("gpio_%d wakeup pds\r\n", 5);
            }
        }
        PDS_IntClear();
    } else {
        printf("first power on\r\n");
    }
}

void pm_irq_callback(uint32_t event)
{
    bflb_gpio_uart_init(gpio, GPIO_PIN_34, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_35, GPIO_UART_FUNC_UART0_RX);
    if (event < PM_PDS_WAKEUP_EVENT_MIN) {
        printf("wakeup event by HBN, event = %u\r\n", event);
    } else if (event < PM_PDS_WAKEUP_EVENT_MAX) {
        printf("wakeup event by PDS, event = %u\r\n", event - PM_PDS_WAKEUP_EVENT_MIN);
    } else {
        printf(" this event has no process, event = %u\r\n", event);
    }
}

int main(void)
{
    int ch;
    struct bflb_device_s *uartx;

    board_init();
    if (app_lowpower_mode_load(PM_LDO13_LDO07_PDSLDO07) != 0) {
        printf("load default power mode failed\r\n");
        while (1) {
        }
    }
    HBN_32K_Sel(HBN_32K_RC);

    gpio = bflb_device_get_by_name("gpio");

    /* gpio latch clear - clear PDS GPIO latch for GPIO 6-36 */
    BL_WR_REG(PDS_BASE, PDS_GPIO_LAT_EN, 0);

    app_print_wakeup_source();
    pm_pds_mask_all_wakeup_src();
    HBN_Pin_WakeUp_Mask(0xF);

    for (uint8_t i = GPIO_PIN_0; i <= GPIO_PIN_5; i++) {
        HBN_Set_Aon_Pad_IntMask(i, MASK);
        HBN_Clear_IRQ(i);
    }
    for (uint8_t i = GPIO_PIN_6; i < GPIO_PIN_MAX; i++) {
        PDS_Set_GPIO_Pad_IntMask(i, MASK);
        PDS_Set_GPIO_Pad_IntClr(i);
    }

    uartx = bflb_device_get_by_name("uart0");
    shell_init();
    printf("\r\nUsage: pds <pds_mode> <test_io> <trig_type> <pull>\r\n");
    while (1) {
        if ((ch = bflb_uart_getchar(uartx)) != -1) {
            shell_handler(ch);
        }
    }
}

int pds_io_wakeup_test(int argc, char **argv)
{
    uint32_t trig_mode = 0;
    uint32_t test_io = GPIO_PIN_0;
    uint32_t pds_mode = PM_PDS_LEVEL_15;
    uint32_t pull = 0;

    if (argc != 5) {
        printf("Usage: pds <pds_mode> <test_io> <trig_type> <pull>\r\n");
        return 0;
    }

    pds_mode = atoi(argv[1]);
    test_io = atoi(argv[2]);
    if (test_io >= GPIO_PIN_MAX) {
        printf("test_io must be less than %d\r\n", GPIO_PIN_MAX);
        return 0;
    }
    if (app_power_gpio_conflicts_with(test_io)) {
        printf("test_io conflicts with DCDC control GPIO\r\n");
        return 0;
    }
    trig_mode = atoi(argv[3]);
    pull = atoi(argv[4]);
    if (pull >= 3) {
        printf("pull must be less than %d\r\n", 3);
        return 0;
    }

    /* lp_wake_io_cfg init */
    memset((void *)&lp_wake_io_cfg, 0x00, sizeof(lp_wake_io_cfg));

    printf("pds_mode: PDS_%d\r\n", pds_mode);
    printf("test_io: %d\r\n", test_io);
    printf("test_trig_type:%s\r\n", pm_get_trig_mode_desc(trig_mode));
    if (1 == pull) {
        printf("pull: up\r\n");
        lp_wake_io_cfg.io_pu = (uint64_t)1 << test_io;
        lp_wake_io_cfg.io_pd = (uint64_t)0 << test_io;
    } else if (2 == pull) {
        printf("pull: down\r\n");
        lp_wake_io_cfg.io_pu = (uint64_t)0 << test_io;
        lp_wake_io_cfg.io_pd = (uint64_t)1 << test_io;
    } else {
        printf("pull: none\r\n");
        lp_wake_io_cfg.io_pu = (uint64_t)0 << test_io;
        lp_wake_io_cfg.io_pd = (uint64_t)0 << test_io;
    }
    arch_delay_us(500);

    lp_wake_io_cfg.io_ie = (uint64_t)1 << test_io;
    lp_wake_io_cfg.io_wakeup_unmask = (uint64_t)1 << test_io;
    if (test_io < GPIO_PIN_MAX) {
        lp_wake_io_cfg.io_0_36_trig_mode[test_io] = trig_mode;
    } else {
        printf("[ERR]test_io >= %d\r\n", GPIO_PIN_MAX);
        return 0;
    }
    printf("GPIO Ready\r\n");
    arch_delay_us(500);
    pm_lowpower_gpio_cfg((lp_gpio_cfg_type *)&lp_wake_io_cfg);
    PDS_Set_All_GPIO_Pad_IntClr();

    switch (pds_mode) {
        case 1:
            pm_pds_irq_register();
            pm_pds_mode_enter(PM_PDS_LEVEL_1, 0, &app_lowpower_cfg);
            bflb_gpio_uart_init(gpio, GPIO_PIN_34, GPIO_UART_FUNC_UART0_TX);
            bflb_gpio_uart_init(gpio, GPIO_PIN_35, GPIO_UART_FUNC_UART0_RX);
            break;
        case 2:
            pm_pds_mode_enter(PM_PDS_LEVEL_2, 0, &app_lowpower_cfg);
            break;
        case 3:
            pm_pds_mode_enter(PM_PDS_LEVEL_3, 0, &app_lowpower_cfg);
            break;
        case 7:
            pm_pds_mode_enter(PM_PDS_LEVEL_7, 0, &app_lowpower_cfg);
            break;
        case 15:
            pm_pds_mode_enter(PM_PDS_LEVEL_15, 0, &app_lowpower_cfg);
            break;
        default:
            pm_pds_mode_enter(PM_PDS_LEVEL_15, 0, &app_lowpower_cfg);
            break;
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_pm_power, pm_power, cmd pm_power);
SHELL_CMD_EXPORT_ALIAS(pds_io_wakeup_test, pds, pds io wakeup test);
