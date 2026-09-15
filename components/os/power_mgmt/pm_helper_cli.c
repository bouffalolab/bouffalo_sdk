#include "pm_helper_cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <lwip/etharp.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>

#include <bl_lp.h>
#include <pm_manager.h>
#include <shell.h>
#include <wifi_mgmr.h>
#include <wifi_mgmr_ext.h>

#if defined(BL616CL)
#include <bflb_gpio.h>
#include <bl616cl_pm.h>
#endif

#define PM_HELPER_CLI_DEFAULT_DTIM               10U
#define PM_HELPER_CLI_DEFAULT_ARP_PERIOD_SECONDS 55U
#define PM_HELPER_CLI_DEBUG_IO_MAX               34U

static pm_helper_cli_cfg_t g_pm_helper_cli_cfg;
static TimerHandle_t g_pm_helper_arp_timer;
#if !defined(CONFIG_WL80211)
static TimerHandle_t g_pm_helper_keepalive_timer;
#endif
static bool g_pm_helper_cli_initialized;

#if defined(BL616CL)
static PM_LOWPOWER_CFG_Type g_pm_helper_power_cfg;
static bool g_pm_helper_power_cfg_initialized;
#endif

static int pm_helper_cli_parse_u32(const char *arg, uint32_t *value)
{
    char *endptr;
    unsigned long parsed;

    if (arg == NULL || value == NULL) {
        return -1;
    }

    parsed = strtoul(arg, &endptr, 0);
    if (endptr == arg || *endptr != '\0' || parsed > UINT32_MAX) {
        return -1;
    }

    *value = (uint32_t)parsed;
    return 0;
}

#if defined(BL616CL)
static int pm_helper_cli_power_cfg_init(void)
{
    const PM_LOWPOWER_CFG_Type *cfg;

    if (g_pm_helper_power_cfg_initialized) {
        return 0;
    }

    cfg = pm_power_mode_cfg_get(PM_LDO13_LDO07_PDSLDO07);
    if (cfg == NULL) {
        return -1;
    }

    g_pm_helper_power_cfg = *cfg;
    g_pm_helper_power_cfg_initialized = true;
    return 0;
}

const PM_LOWPOWER_CFG_Type *bl616cl_lowpower_cfg_get(void)
{
    if (pm_helper_cli_power_cfg_init() != 0) {
        return NULL;
    }

    return &g_pm_helper_power_cfg;
}
#endif

static int pm_helper_cli_send_arp(void)
{
    const ip4_addr_t *target;
    ip4_addr_t target_copy;
    err_t ret;

    if (!wifi_mgmr_sta_state_get()) {
        return -1;
    }

    if (netif_default == NULL || ip4_addr_isany_val(*netif_ip4_addr(netif_default))) {
        printf("IP address not assigned. ARP skipped.\r\n");
        return -1;
    }

    if (g_pm_helper_cli_cfg.arp_target == PM_HELPER_CLI_ARP_LOCAL_IP) {
        target = netif_ip4_addr(netif_default);
    } else {
        target = netif_ip4_gw(netif_default);
    }
    target_copy = *target;

    LOCK_TCPIP_CORE();
    ret = etharp_request(netif_default, &target_copy);
    UNLOCK_TCPIP_CORE();

    if (ret != ERR_OK) {
        printf("Failed to send ARP request: %d\r\n", ret);
        return -1;
    }

    return 0;
}

static void pm_helper_cli_arp_timer_cb(TimerHandle_t timer)
{
    (void)timer;
    (void)pm_helper_cli_send_arp();
}

int pm_helper_cli_arp_timer_start(uint32_t period_seconds)
{
    if (!g_pm_helper_cli_initialized || period_seconds == 0 || g_pm_helper_arp_timer != NULL) {
        return -1;
    }

    if (g_pm_helper_cli_cfg.arp_send_immediately) {
        (void)pm_helper_cli_send_arp();
    }

    g_pm_helper_arp_timer = xTimerCreate("traffic probe", pdMS_TO_TICKS(period_seconds * 1000UL),
                                         g_pm_helper_cli_cfg.arp_periodic ? pdTRUE : pdFALSE,
                                         NULL, pm_helper_cli_arp_timer_cb);
    if (g_pm_helper_arp_timer == NULL) {
        return -1;
    }

    if (xTimerStart(g_pm_helper_arp_timer, 0) != pdPASS) {
        xTimerDelete(g_pm_helper_arp_timer, 0);
        g_pm_helper_arp_timer = NULL;
        return -1;
    }

    return 0;
}

int pm_helper_cli_arp_timer_stop(void)
{
    if (g_pm_helper_arp_timer == NULL) {
        return 0;
    }

    if (xTimerStop(g_pm_helper_arp_timer, 0) != pdPASS) {
        return -1;
    }
    if (xTimerDelete(g_pm_helper_arp_timer, portMAX_DELAY) != pdPASS) {
        return -1;
    }

    g_pm_helper_arp_timer = NULL;
    return 0;
}

#if !defined(CONFIG_WL80211)
static void pm_helper_cli_keepalive_timer_cb(TimerHandle_t timer)
{
    (void)timer;
    wifi_mgmr_null_data_send();
}
#endif

int pm_helper_cli_keepalive_timer_start(uint32_t period_seconds)
{
#if defined(CONFIG_WL80211)
    (void)period_seconds;
    return -1;
#else
    TickType_t period;

    if (!g_pm_helper_cli_initialized || period_seconds == 0) {
        return -1;
    }

    period = pdMS_TO_TICKS(period_seconds * 1000UL);
    if (g_pm_helper_keepalive_timer == NULL) {
        g_pm_helper_keepalive_timer = xTimerCreate("keepalive_timer", period, pdTRUE, NULL,
                                                   pm_helper_cli_keepalive_timer_cb);
        if (g_pm_helper_keepalive_timer == NULL) {
            return -1;
        }
        if (xTimerStart(g_pm_helper_keepalive_timer, 0) != pdPASS) {
            xTimerDelete(g_pm_helper_keepalive_timer, 0);
            g_pm_helper_keepalive_timer = NULL;
            return -1;
        }
        return 0;
    }

    return xTimerChangePeriod(g_pm_helper_keepalive_timer, period, 0) == pdPASS ? 0 : -1;
#endif
}

int pm_helper_cli_keepalive_timer_stop(void)
{
#if defined(CONFIG_WL80211)
    return -1;
#else
    if (g_pm_helper_keepalive_timer == NULL) {
        return 0;
    }

    if (xTimerStop(g_pm_helper_keepalive_timer, 0) != pdPASS ||
        xTimerDelete(g_pm_helper_keepalive_timer, 0) != pdPASS) {
        return -1;
    }

    g_pm_helper_keepalive_timer = NULL;
    return 0;
#endif
}

int pm_helper_cli_twt_setup(int setup_type, int flow_type, int wake_int_exp,
                            int min_twt_wake_dur, int wake_int_mantissa)
{
#if defined(CONFIG_WL80211)
    (void)setup_type;
    (void)flow_type;
    (void)wake_int_exp;
    (void)min_twt_wake_dur;
    (void)wake_int_mantissa;
    return -1;
#else
    twt_setup_params_struct_t param = {
        .setup_type = setup_type,
        .flow_type = flow_type,
        .wake_int_exp = wake_int_exp,
        .wake_dur_unit = 0,
        .min_twt_wake_dur = min_twt_wake_dur,
        .wake_int_mantissa = wake_int_mantissa,
    };

    return wifi_mgmr_sta_twt_setup(&param);
#endif
}

int pm_helper_cli_init(const pm_helper_cli_cfg_t *cfg)
{
    if (cfg == NULL || cfg->wakeup_timer_cb == NULL) {
        return -1;
    }

#if defined(BL616CL)
    if (pm_helper_cli_power_cfg_init() != 0) {
        return -1;
    }
#endif

    g_pm_helper_cli_cfg = *cfg;
    if (g_pm_helper_cli_cfg.arp_period_seconds == 0) {
        g_pm_helper_cli_cfg.arp_period_seconds = PM_HELPER_CLI_DEFAULT_ARP_PERIOD_SECONDS;
    }
    g_pm_helper_cli_initialized = true;

    return 0;
}

static void cmd_pm_helper_tickless(int argc, char **argv)
{
    uint32_t dtim = PM_HELPER_CLI_DEFAULT_DTIM;
    uint32_t broadcast = 0;

    if (argc > 1 && pm_helper_cli_parse_u32(argv[1], &dtim) != 0) {
        printf("usage: tickless [dtim] [broadcast:0|1]\r\n");
        return;
    }
    if (argc > 2 && (pm_helper_cli_parse_u32(argv[2], &broadcast) != 0 || broadcast > 1)) {
        printf("usage: tickless [dtim] [broadcast:0|1]\r\n");
        return;
    }
    if (argc > 3) {
        printf("usage: tickless [dtim] [broadcast:0|1]\r\n");
        return;
    }

    lpfw_cfg.dtim_origin = dtim;
    lpfw_cfg.bcmc_dtim_mode = broadcast;
    enable_multicast_broadcast = broadcast;

    printf("dtim_origin: %d\r\n", lpfw_cfg.dtim_origin);
    printf("broadcast: %lu\r\n", (unsigned long)broadcast);
    if (g_pm_helper_cli_cfg.tickless_cb != NULL) {
        if (g_pm_helper_cli_cfg.tickless_cb(g_pm_helper_cli_cfg.arg) != 0) {
            printf("failed to enter tickless mode.\r\n");
        }
    } else {
        pm_enable_tickless();
    }
}

static void cmd_pm_helper_wakeup_timer(int argc, char **argv)
{
    uint32_t timeout_ms;
    uint32_t broadcast;

    if (argc != 3 || pm_helper_cli_parse_u32(argv[1], &timeout_ms) != 0 || timeout_ms == 0 ||
        pm_helper_cli_parse_u32(argv[2], &broadcast) != 0 || broadcast > 1) {
        printf("usage: wakeup_timer <timeout_ms> <broadcast:0|1>\r\n");
        return;
    }
    if (!g_pm_helper_cli_initialized) {
        printf("pm helper cli is not initialized.\r\n");
        return;
    }

    enable_multicast_broadcast = (int)broadcast;
    if (g_pm_helper_cli_cfg.wakeup_timer_cb(timeout_ms, (int)broadcast,
                                            g_pm_helper_cli_cfg.arg) != 0) {
        printf("failed to start low power wakeup timer.\r\n");
    }
}

static void cmd_pm_helper_set_dtim(int argc, char **argv)
{
    uint32_t dtim = PM_HELPER_CLI_DEFAULT_DTIM;

    if (argc > 1 && pm_helper_cli_parse_u32(argv[1], &dtim) != 0) {
        printf("usage: wifi_lp_set_dtim [dtim]\r\n");
        return;
    }
    if (argc > 2) {
        printf("usage: wifi_lp_set_dtim [dtim]\r\n");
        return;
    }

    set_dtim_config((int)dtim);
}

static void cmd_pm_helper_io_debug(int argc, char **argv)
{
    uint32_t io;

    if (argc != 2 || pm_helper_cli_parse_u32(argv[1], &io) != 0) {
        printf("usage: io_debug <gpio|0xff>\r\n");
        return;
    }

#ifdef BL616
    iot2lp_para->debug_io = io <= PM_HELPER_CLI_DEBUG_IO_MAX ? io : 0xFF;
#else
    iot2lp_para->wifi_debug_io = io <= PM_HELPER_CLI_DEBUG_IO_MAX ? io : 0xFF;
#endif
}

static void cmd_pm_helper_arp(int argc, char **argv)
{
    uint32_t enable;
    uint32_t period_seconds = g_pm_helper_cli_cfg.arp_period_seconds;

    if (argc < 2 || argc > 3 || pm_helper_cli_parse_u32(argv[1], &enable) != 0 || enable > 1 ||
        (argc == 3 && (pm_helper_cli_parse_u32(argv[2], &period_seconds) != 0 || period_seconds == 0))) {
        printf("usage: arp_send <0|1> [period_seconds]\r\n");
        return;
    }

    if (enable != 0) {
        if (pm_helper_cli_arp_timer_start(period_seconds) != 0) {
            printf("ARP timer already created or failed to start.\r\n");
            return;
        }
        printf("create %lus ARP timer success.\r\n", (unsigned long)period_seconds);
    } else if (pm_helper_cli_arp_timer_stop() == 0) {
        printf("Delete ARP timer.\r\n");
    } else {
        printf("Failed to delete ARP timer.\r\n");
    }
}

#if defined(BL616CL)
static int pm_helper_cli_parse_u8(const char *arg, uint8_t *value)
{
    uint32_t parsed;

    if (value == NULL || pm_helper_cli_parse_u32(arg, &parsed) != 0 || parsed > UINT8_MAX) {
        return -1;
    }

    *value = (uint8_t)parsed;
    return 0;
}

static void cmd_pm_helper_power(int argc, char **argv)
{
    uint8_t mode;
    uint8_t dcdc_sys_enable_pin;
    uint8_t dcdc_soc_enable_pin;
    uint8_t dcdc_soc_vsel_pin;
    uint8_t lp_mask;
    uint8_t pds_clk;
    const PM_LOWPOWER_CFG_Type *cfg;
    PM_LOWPOWER_CFG_Type new_cfg;

    if (argc == 2 && strcmp(argv[1], "list") == 0) {
        for (uint8_t i = 0; i < PM_POWER_MODE_MAX; i++) {
            cfg = pm_power_mode_cfg_get(i);
            if (cfg != NULL) {
                printf("%u: %s\r\n", i, cfg->name);
            }
        }
        return;
    }

    if (argc == 2 && strcmp(argv[1], "get") == 0) {
        printf("pm_power cfg: %s\r\n", g_pm_helper_power_cfg.name ? g_pm_helper_power_cfg.name : "");
        printf("  dcdc_sys_gpio          : 0x%02x\r\n", g_pm_helper_power_cfg.sys_cfg.dcdc_sys_enable_pin);
        printf("  dcdc_sys_pds_enable    : %u\r\n", g_pm_helper_power_cfg.sys_cfg.dcdc_sys_pds_enable);
        printf("  ldo_sys_active_level   : %u\r\n", g_pm_helper_power_cfg.sys_cfg.ldo_sys_active_level);
        printf("  ldo_sys_pds_level      : %u\r\n", g_pm_helper_power_cfg.sys_cfg.ldo_sys_pds_level);
        printf("  dcdc_soc_gpio          : 0x%02x\r\n", g_pm_helper_power_cfg.soc_cfg.dcdc_soc_enable_pin);
        printf("  dcdc_soc_vsel_gpio     : 0x%02x\r\n", g_pm_helper_power_cfg.soc_cfg.dcdc_soc_vsel_pin);
        printf("  dcdc_soc_pds_enable    : %u\r\n", g_pm_helper_power_cfg.soc_cfg.dcdc_soc_pds_enable);
        printf("  dcdc_soc_pds_level     : %u\r\n", g_pm_helper_power_cfg.soc_cfg.dcdc_soc_pds_level);
        printf("  ldo_soc_active_level   : %u\r\n", g_pm_helper_power_cfg.soc_cfg.ldo_soc_active_level);
        printf("  ldo_soc_enter_pds      : %u\r\n", g_pm_helper_power_cfg.soc_cfg.ldo_soc_enter_pds_level);
        printf("  ldo_soc_pds_level      : %u\r\n", g_pm_helper_power_cfg.soc_cfg.ldo_soc_pds_level);
        printf("  pds_gpio_keep_en       : %u\r\n", g_pm_helper_power_cfg.lp_cfg.pds_gpio_keep_en);
        printf("  hbn_gpio_keep_en       : %u\r\n", g_pm_helper_power_cfg.lp_cfg.hbn_gpio_keep_en);
        printf("  pds_flash_power_off    : %u\r\n", g_pm_helper_power_cfg.lp_cfg.pds_flash_power_off);
        printf("  pll_power_off          : %u\r\n", g_pm_helper_power_cfg.lp_cfg.pll_power_off);
        printf("  rf_power_off           : %u\r\n", g_pm_helper_power_cfg.lp_cfg.rf_power_off);
        printf("  clk_default_sel        : %u\r\n", g_pm_helper_power_cfg.lp_cfg.clk_default_sel);
        printf("  set_all_ram_ret_en     : %u\r\n", g_pm_helper_power_cfg.lp_cfg.set_all_ram_ret_en);
        printf("  hbn_flash_power_off    : %u\r\n", g_pm_helper_power_cfg.lp_cfg.hbn_flash_power_off);
        printf("  ldo18io_power_down     : %u\r\n", g_pm_helper_power_cfg.lp_cfg.ldo18io_power_down);
        return;
    }

    if (argc == 5 || argc == 7) {
        if (pm_helper_cli_parse_u8(argv[1], &mode) != 0 ||
            pm_helper_cli_parse_u8(argv[2], &dcdc_sys_enable_pin) != 0 ||
            pm_helper_cli_parse_u8(argv[3], &dcdc_soc_enable_pin) != 0 ||
            pm_helper_cli_parse_u8(argv[4], &dcdc_soc_vsel_pin) != 0) {
            printf("invalid argument\r\n");
            return;
        }

        cfg = pm_power_mode_cfg_get(mode);
        if (cfg == NULL) {
            printf("invalid mode\r\n");
            return;
        }
        new_cfg = *cfg;

        if ((new_cfg.sys_cfg.dcdc_sys_enable_pin == 0xFF && dcdc_sys_enable_pin != 0xFF) ||
            (new_cfg.soc_cfg.dcdc_soc_enable_pin == 0xFF && dcdc_soc_enable_pin != 0xFF) ||
            (new_cfg.soc_cfg.dcdc_soc_vsel_pin == 0xFF && dcdc_soc_vsel_pin != 0xFF) ||
            (dcdc_sys_enable_pin != 0xFF && dcdc_sys_enable_pin >= GPIO_PIN_MAX) ||
            (dcdc_soc_enable_pin != 0xFF && dcdc_soc_enable_pin >= GPIO_PIN_MAX) ||
            (dcdc_soc_vsel_pin != 0xFF && dcdc_soc_vsel_pin >= GPIO_PIN_MAX) ||
            (dcdc_sys_enable_pin != 0xFF && dcdc_sys_enable_pin == dcdc_soc_enable_pin) ||
            (dcdc_sys_enable_pin != 0xFF && dcdc_sys_enable_pin == dcdc_soc_vsel_pin) ||
            (dcdc_soc_enable_pin != 0xFF && dcdc_soc_enable_pin == dcdc_soc_vsel_pin)) {
            printf("invalid GPIO config for current mode\r\n");
            return;
        }

        new_cfg.sys_cfg.dcdc_sys_enable_pin = dcdc_sys_enable_pin;
        new_cfg.soc_cfg.dcdc_soc_enable_pin = dcdc_soc_enable_pin;
        new_cfg.soc_cfg.dcdc_soc_vsel_pin = dcdc_soc_vsel_pin;

        if (argc == 7) {
            if (pm_helper_cli_parse_u8(argv[5], &lp_mask) != 0 ||
                pm_helper_cli_parse_u8(argv[6], &pds_clk) != 0 || pds_clk > PM_PDS_CLK_RC16M) {
                printf("invalid LP config\r\n");
                return;
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

        g_pm_helper_power_cfg = new_cfg;
        pm_dcdc_sys_exit_pds(&g_pm_helper_power_cfg.sys_cfg);
        pm_dcdc_soc_exit_pds(&g_pm_helper_power_cfg.soc_cfg);
        printf("power config updated: %u %s\r\n", mode,
               g_pm_helper_power_cfg.name ? g_pm_helper_power_cfg.name : "");
        return;
    }

    printf("usage: pm_power list\r\n");
    printf("usage: pm_power get\r\n");
    printf("usage: pm_power <mode> <dcdc_sys_gpio> <dcdc_soc_gpio> <dcdc_soc_vsel_gpio>\r\n");
    printf("usage: pm_power <mode> <dcdc_sys_gpio> <dcdc_soc_gpio> <dcdc_soc_vsel_gpio> <lp_mask> <pds_clk>\r\n");
    printf("       use 0xff for unused GPIO\r\n");
    printf("       lp_mask bit0=pds_gpio_keep bit1=hbn_gpio_keep bit2=pds_flash_off bit3=pll_off bit4=rf_off bit5=ram_ret bit6=hbn_flash_off bit7=ldo18io_down\r\n");
    printf("       pds_clk: 0=f32k, 1=rc32m, 2=xtal, 3=xtal_lp, 4=rc8m, 5=rc16m\r\n");
}
#endif

#if !defined(BL616)
static void cmd_pm_helper_lpfw_uart(int argc, char **argv)
{
    uint32_t debug_log_en;
    uint32_t tx_io;
    uint32_t rx_io;
    uint32_t baudrate;

    if (argc != 5 || pm_helper_cli_parse_u32(argv[1], &debug_log_en) != 0 ||
        pm_helper_cli_parse_u32(argv[2], &tx_io) != 0 ||
        pm_helper_cli_parse_u32(argv[3], &rx_io) != 0 ||
        pm_helper_cli_parse_u32(argv[4], &baudrate) != 0) {
        printf("usage: lpfw_uart <debug_en> <tx_io> <rx_io> <baudrate>\r\n");
        return;
    }

    iot2lp_para->uart_config->debug_log_en = debug_log_en;
    iot2lp_para->uart_config->uart_tx_io = tx_io;
    iot2lp_para->uart_config->uart_rx_io = rx_io;
    iot2lp_para->uart_config->baudrate = baudrate;
}

static void cmd_pm_helper_lpfw_clock(int argc, char **argv)
{
    uint32_t mcu_clk_sel;
    uint32_t hclk_div;
    uint32_t bclk_div;
    uint32_t xclk_sel;

    if (argc != 5 || pm_helper_cli_parse_u32(argv[1], &mcu_clk_sel) != 0 ||
        pm_helper_cli_parse_u32(argv[2], &hclk_div) != 0 ||
        pm_helper_cli_parse_u32(argv[3], &bclk_div) != 0 ||
        pm_helper_cli_parse_u32(argv[4], &xclk_sel) != 0) {
        printf("usage: lpfw_clock <mcu_clk_sel> <hclk_div> <bclk_div> <xclk_sel>\r\n");
        return;
    }

    iot2lp_para->clock_config->mcu_clk_sel = mcu_clk_sel;
    iot2lp_para->clock_config->hclk_div = hclk_div;
    iot2lp_para->clock_config->bclk_div = bclk_div;
    iot2lp_para->clock_config->xclk_sel = xclk_sel;
}
#endif

SHELL_CMD_EXPORT_ALIAS(cmd_pm_helper_tickless, tickless, enter tickless low power mode);
SHELL_CMD_EXPORT_ALIAS(cmd_pm_helper_wakeup_timer, wakeup_timer, timed low power wakeup);
SHELL_CMD_EXPORT_ALIAS(cmd_pm_helper_set_dtim, wifi_lp_set_dtim, configure low power DTIM);
SHELL_CMD_EXPORT_ALIAS(cmd_pm_helper_io_debug, io_debug, configure low power debug GPIO);
SHELL_CMD_EXPORT_ALIAS(cmd_pm_helper_arp, arp_send, configure low power ARP timer);
SHELL_CMD_EXPORT_ALIAS(cmd_pm_helper_arp, send_arp, configure low power ARP timer);
#if defined(BL616CL)
SHELL_CMD_EXPORT_ALIAS(cmd_pm_helper_power, pm_power, configure BL616CL power mode);
#endif
#if !defined(BL616)
SHELL_CMD_EXPORT_ALIAS(cmd_pm_helper_lpfw_uart, lpfw_uart, configure LPFW UART);
SHELL_CMD_EXPORT_ALIAS(cmd_pm_helper_lpfw_clock, lpfw_clock, configure LPFW clock);
#endif
