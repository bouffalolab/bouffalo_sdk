#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bflb_mtimer.h"
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
#include "bflb_rtc.h"
#endif
#include "bflb_uart.h"
#include "board.h"
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
#include "board_gpio.h"
#endif
#include "shell.h"
#include "bl616cl_hbn.h"
#include "bl616cl_pds.h"
#include "bl616cl_pm.h"
#include "bl_lp.h"
#include "cli_helper.h"

#define PM_DEMO_PRINT_DELAY_US 500U
#define PM_DEMO_TIMER_SRC_RTC       0U
#define PM_DEMO_TIMER_SRC_PDS_TIMER 1U

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
#define PM_DEMO_RTC_HZ 32768ULL
#endif

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
#define PM_DEMO_HBN_GPIO_MASK ((1ULL << 6) - 1ULL)
#define PM_DEMO_PDS_GPIO_MASK (((1ULL << GPIO_PIN_MAX) - 1ULL) & \
                               ~PM_DEMO_HBN_GPIO_MASK)
#endif

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP) || \
    defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP) || \
    defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
#define PM_DEMO_HBN_IRQ_OUT_WAKEUP
#endif

#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
#define PM_DEMO_HBN_WDT_TIMEOUT_TICKS 32768U
#define PM_DEMO_HBN_WDT_TIMEOUT_MS    1000U
#define PM_DEMO_HBN_WDT_FEED_MS       250U
#define PM_DEMO_HBN_WDT_RESET_EVENT   (1U << 5)

static uint64_t g_hbn_wdt_last_feed_ms;
static int g_hbn_wdt_reset;
#endif

static struct bflb_device_s *g_uart;
static PM_LOWPOWER_CFG_Type g_lowpower_cfg;
static volatile uint32_t g_pds_wakeup_sources;
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
static volatile uint32_t g_pds_gpio_status;
#endif

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
static void pm_demo_print_rtc_time(void)
{
    uint64_t rtc_time = bflb_rtc_get_time(NULL);

    printf("Current RTC time: %lu.%03lu\r\n",
           (unsigned long)(rtc_time >> 15),
           (unsigned long)((rtc_time & 0x7FFF) * 1000 / 32768));
}

static uint64_t pm_demo_ms_to_rtc_ticks(uint32_t wake_ms)
{
    return ((uint64_t)wake_ms * PM_DEMO_RTC_HZ + 999ULL) / 1000ULL;
}
#endif

static int pm_demo_parse_u32(const char *text, uint32_t *value)
{
    char *end;
    unsigned long parsed;

    errno = 0;
    parsed = strtoul(text, &end, 0);
    if ((errno != 0) || (end == text) || (*end != '\0') ||
        (parsed > UINT32_MAX)) {
        return -EINVAL;
    }

    *value = (uint32_t)parsed;
    return 0;
}

static int pm_demo_lowpower_mode_load(uint8_t mode)
{
    const PM_LOWPOWER_CFG_Type *mode_cfg = pm_power_mode_cfg_get(mode);

    if (mode_cfg == NULL) {
        return -ENODEV;
    }
    g_lowpower_cfg = *mode_cfg;
    return 0;
}

static int pm_demo_pds_level_valid(uint32_t level)
{
    return (level == PM_PDS_LEVEL_1) || (level == PM_PDS_LEVEL_2) ||
           (level == PM_PDS_LEVEL_3) || (level == PM_PDS_LEVEL_7) ||
           (level == PM_PDS_LEVEL_11) || (level == PM_PDS_LEVEL_15);
}

#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
static void pm_demo_hbn_wdt_boot_latch(void)
{
    uint8_t reset_event = 0U;

    HBN_Wdg_Stop();
    HBN_Get_Reset_Event(&reset_event);
    g_hbn_wdt_reset = ((reset_event & PM_DEMO_HBN_WDT_RESET_EVENT) != 0U);
    HBN_Clr_Reset_Event();
}

static void pm_demo_hbn_wdt_feed(void)
{
    HBN_Wdg_Reset_Countervalue();
    g_hbn_wdt_last_feed_ms = bflb_mtimer_get_time_ms();
}

static void pm_demo_hbn_wdt_configure(void)
{
    HBN_Wdg_Init(HBN_WDG_MODE_RESET, PM_DEMO_HBN_WDT_TIMEOUT_TICKS);
    printf("PM demo HBN WDT configured: timeout=%u ms, feed interval=%u ms\r\n",
           PM_DEMO_HBN_WDT_TIMEOUT_MS, PM_DEMO_HBN_WDT_FEED_MS);
}

static void pm_demo_hbn_wdt_start(void)
{
    HBN_Wdg_Start();
    pm_demo_hbn_wdt_feed();
}

static void pm_demo_hbn_wdt_service(void)
{
    uint64_t now_ms = bflb_mtimer_get_time_ms();

    if ((now_ms - g_hbn_wdt_last_feed_ms) >= PM_DEMO_HBN_WDT_FEED_MS) {
        pm_demo_hbn_wdt_feed();
    }
}
#endif

#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
static uint32_t pm_demo_get_and_clear_hbn_status(void)
{
    uint32_t status = 0U;

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    for (uint8_t pin = GPIO_PIN_0; pin <= GPIO_PIN_5; pin++) {
        if (HBN_Get_INT_State(pin) == SET) {
            status |= 1UL << pin;
            HBN_Clear_IRQ(pin);
        }
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    if (HBN_Get_INT_State(HBN_INT_RTC) == SET) {
        status |= 1UL << HBN_INT_RTC;
        HBN_Clear_RTC_INT();
        HBN_Clear_IRQ(HBN_INT_RTC);
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
    if (HBN_Get_INT_State(HBN_INT_BOD) == SET) {
        status |= 1UL << HBN_INT_BOD;
        HBN_Clear_IRQ(HBN_INT_BOD);
    }
#endif
    return status;
}

static void pm_demo_print_hbn_status(uint32_t status)
{
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    for (uint8_t pin = GPIO_PIN_0; pin <= GPIO_PIN_5; pin++) {
        if ((status & (1UL << pin)) != 0U) {
            printf("  HBN GPIO%u\r\n", pin);
        }
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    if ((status & (1UL << HBN_INT_RTC)) != 0U) {
        printf("  HBN RTC\r\n");
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
    if ((status & (1UL << HBN_INT_BOD)) != 0U) {
        printf("  HBN BOD\r\n");
    }
#endif
}
#endif

static void pm_demo_print_boot_wakeup(void)
{
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    uint32_t status = pm_demo_get_and_clear_hbn_status();
#endif

#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
    if (g_hbn_wdt_reset != 0) {
        printf("PM demo reset source: HBN WDT\r\n");
        return;
    }
#endif
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    if (status != 0U) {
        printf("PM demo HBN wakeup sources:\r\n");
        pm_demo_print_hbn_status(status);
        return;
    }
#endif
    printf("PM demo cold boot\r\n");
}

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
static void pm_demo_pds_gpio_wakeup_arm(void)
{
    uint64_t wakeup_mask = pm_demo_cli_gpio_get()->io_wakeup_unmask;

    if ((wakeup_mask & PM_DEMO_HBN_GPIO_MASK) != 0U) {
        PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);
    }
    if ((wakeup_mask & PM_DEMO_PDS_GPIO_MASK) != 0U) {
        PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_PDS_GPIO_INT, UNMASK);
    }
}
#endif

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
static void pm_demo_rtc_configure(uint32_t wake_ms)
{
    HBN_Clear_RTC_INT();
    HBN_Clear_IRQ(HBN_INT_RTC);
    if (wake_ms != 0U) {
        bflb_rtc_set_time(NULL, pm_demo_ms_to_rtc_ticks(wake_ms));
    }
}
#endif

static void pm_demo_print_enabled_wakeup_sources(void)
{
    int enabled = 0;

    printf("PM demo wakeup sources:");
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    if (pm_demo_cli_gpio_get()->io_wakeup_unmask != 0U) {
        printf(" GPIO-mask=0x%08lx%08lx",
               (unsigned long)(pm_demo_cli_gpio_get()->io_wakeup_unmask >> 32),
               (unsigned long)(pm_demo_cli_gpio_get()->io_wakeup_unmask &
                               UINT32_MAX));
        enabled = 1;
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
    printf(" BOD");
    enabled = 1;
#endif
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    printf(" RTC");
    enabled = 1;
#endif
    if (enabled == 0) {
        printf(" none");
    }
    printf("\r\n");
}

void pm_irq_callback(uint32_t event)
{
    if ((event >= PM_PDS_WAKEUP_EVENT_MIN) &&
        (event < PM_PDS_WAKEUP_EVENT_MAX)) {
        g_pds_wakeup_sources |= 1UL << (event - PM_PDS_WAKEUP_EVENT_MIN);
    }
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    if (event == PM_PDS_GPIO6_36_WAKEUP_EVENT) {
        g_pds_gpio_status |= PDS_Get_All_GPIO_Pad_IntStatus();
    }
#endif
}

static void pm_demo_print_pds_wakeup(void)
{
    uint32_t pds_sources = g_pds_wakeup_sources;
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    uint32_t hbn_status = pm_demo_get_and_clear_hbn_status();
#endif

    printf("PDS wakeup sources: 0x%08lx\r\n", (unsigned long)pds_sources);
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    if ((pds_sources & (1UL << PDS_WAKEUP_BY_HBN_IRQ_OUT)) != 0U) {
        if (hbn_status != 0U) {
            pm_demo_print_hbn_status(hbn_status);
        } else {
            printf("  HBN IRQ OUT (detail unavailable)\r\n");
        }
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    if ((pds_sources & (1UL << PDS_WAKEUP_BY_PDS_GPIO_INT)) != 0U) {
        uint32_t status = g_pds_gpio_status;

        if (status == 0U) {
            printf("  PDS GPIO INT (detail unavailable)\r\n");
        }
        for (uint8_t pin = GPIO_PIN_6; pin < GPIO_PIN_MAX; pin++) {
            if ((status & (1UL << (pin - GPIO_PIN_6))) != 0U) {
                printf("  PDS GPIO%u\r\n", pin);
            }
        }
    }
#endif
    if ((pds_sources & (1UL << PDS_WAKEUP_BY_PDS_TIMER)) != 0U) {
        printf("  PDS timer\r\n");
    }
}

static int pm_demo_enter_pds(uint32_t level, uint32_t wake_ms,
                             uint32_t timer_src)
{
    uint32_t pds_sleep_time = 0U;
    int ret;

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    printf("Enter PDS%lu: %s delay=%lu ms (0=disabled)\r\n",
           (unsigned long)level,
           (timer_src == PM_DEMO_TIMER_SRC_PDS_TIMER) ? "PDS-timer" : "RTC",
           (unsigned long)wake_ms);
#else
    printf("Enter PDS%lu\r\n", (unsigned long)level);
#endif
    arch_delay_us(PM_DEMO_PRINT_DELAY_US);

    g_pds_wakeup_sources = 0U;
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    g_pds_gpio_status = 0U;
#endif
    PDS_Mask_All_Wakeup_Src();
    PDS_IntClear();
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    PDS_Set_All_GPIO_Pad_IntClr();
#endif
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    (void)pm_demo_get_and_clear_hbn_status();
#endif

#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP) || \
    defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);
#endif
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    pm_demo_pds_gpio_wakeup_arm();
#endif

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    if (timer_src == PM_DEMO_TIMER_SRC_PDS_TIMER) {
        uint64_t ticks = pm_demo_ms_to_rtc_ticks(wake_ms);

        if ((ticks > UINT32_MAX) ||
            ((ticks != 0U) && (ticks <= PDS_WARMUP_LATENCY_CNT))) {
            printf("PDS timer wake_ms is out of range\r\n");
            return -EINVAL;
        }
        pds_sleep_time = (uint32_t)ticks;
    } else {
        pm_demo_rtc_configure(wake_ms);
    }
#else
    (void)wake_ms;
    (void)timer_src;
#endif

    pm_pds_irq_register();
#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
    pm_demo_hbn_wdt_feed();
#endif
    ret = bl_lp_pds_enter_with_restore(level, pds_sleep_time,
                                       &g_lowpower_cfg);
#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
    pm_demo_hbn_wdt_feed();
#endif

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    pm_demo_print_rtc_time();
#endif
    pm_demo_print_pds_wakeup();
    PDS_Mask_All_Wakeup_Src();
    PDS_IntClear();
    shell_init();
    return ret;
}

static int pm_demo_enter_hbn(uint32_t level, uint32_t wake_ms)
{
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    (void)pm_demo_get_and_clear_hbn_status();
#endif
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    printf("Enter HBN%lu: RTC delay=%lu ms (0=disabled)\r\n",
           (unsigned long)level, (unsigned long)wake_ms);
#else
    printf("Enter HBN%lu\r\n", (unsigned long)level);
#endif
    arch_delay_us(PM_DEMO_PRINT_DELAY_US);
#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
    pm_demo_hbn_wdt_feed();
#endif
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    pm_hbn_mode_enter(level, pm_demo_ms_to_rtc_ticks(wake_ms));
#else
    (void)wake_ms;
    pm_hbn_mode_enter(level, 0U);
#endif
    return 0;
}

static int app_pm_enter(int argc, char **argv)
{
    uint32_t level;
    uint32_t wake_ms;
    uint32_t timer_src = PM_DEMO_TIMER_SRC_RTC;

    if ((argc != 4) && (argc != 5)) {
        printf("Usage: app_pm_enter <PDS|HBN> <level> <wake_ms> [rtc|timer]\r\n");
        printf("  PDS levels: 1, 2, 3, 7, 11, 15\r\n");
        printf("  HBN levels: 0, 1\r\n");
        printf("  timer source: rtc (default) or timer (PDS only)\r\n");
        return -EINVAL;
    }
    if ((pm_demo_parse_u32(argv[2], &level) != 0) ||
        (pm_demo_parse_u32(argv[3], &wake_ms) != 0)) {
        printf("Invalid level or wake_ms\r\n");
        return -EINVAL;
    }
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    if (argc == 5) {
        if (strcmp(argv[4], "timer") == 0) {
            timer_src = PM_DEMO_TIMER_SRC_PDS_TIMER;
        } else if (strcmp(argv[4], "rtc") != 0) {
            printf("Timer source must be rtc or timer\r\n");
            return -EINVAL;
        }
    }
#else
    if ((argc == 5) || (wake_ms != 0U)) {
        printf("RTC wakeup is disabled; omit timer source and use wake_ms 0\r\n");
        return -EINVAL;
    }
#endif

    if ((strcmp(argv[1], "PDS") == 0) || (strcmp(argv[1], "pds") == 0)) {
        if (!pm_demo_pds_level_valid(level)) {
            printf("PDS level must be 1, 2, 3, 7, 11, or 15\r\n");
            return -EINVAL;
        }
        return pm_demo_enter_pds(level, wake_ms, timer_src);
    }
    if ((strcmp(argv[1], "HBN") == 0) || (strcmp(argv[1], "hbn") == 0)) {
        if ((level != PM_HBN_LEVEL_0) && (level != PM_HBN_LEVEL_1)) {
            printf("HBN level must be 0 or 1\r\n");
            return -EINVAL;
        }
        if (timer_src == PM_DEMO_TIMER_SRC_PDS_TIMER) {
            printf("HBN only supports the rtc timer source\r\n");
            return -EINVAL;
        }
        return pm_demo_enter_hbn(level, wake_ms);
    }

    printf("Power mode must be PDS or HBN\r\n");
    return -EINVAL;
}
SHELL_CMD_EXPORT_ALIAS(app_pm_enter, app_pm_enter,
                       enter PM mode: app_pm_enter <PDS|HBN> <level> <wake_ms> [rtc|timer]);

int main(void)
{
    int ch;
    int ret;

    board_init();
#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
    pm_demo_hbn_wdt_boot_latch();
#endif
    printf("PM demo starting...\r\n");
    pm_demo_print_boot_wakeup();

    ret = pm_demo_lowpower_mode_load(PM_LDO13_LDO07_PDSLDO07);
    if (ret != 0) {
        printf("PM demo low-power configuration is unavailable: %d\r\n", ret);
        return ret;
    }

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    pm_demo_print_rtc_time();
    bflb_mtimer_delay_ms(500);
    pm_demo_print_rtc_time();
#endif
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    ret = pm_demo_cli_gpio_init(board_lp_gpio_wakeup_config_get());
    if (ret == 0) {
        ret = pm_demo_cli_gpio_apply();
    }
    if (ret != 0) {
        printf("PM demo GPIO wakeup configuration failed: %d\r\n", ret);
        return ret;
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
    ret = pm_demo_cli_bod_init();
    if (ret != 0) {
        printf("PM demo BOD wakeup configuration failed: %d\r\n", ret);
        return ret;
    }
#endif

    g_uart = bflb_device_get_by_name("uart0");
    if (g_uart == NULL) {
        printf("PM demo UART0 is unavailable\r\n");
        return -ENODEV;
    }

    pm_demo_print_enabled_wakeup_sources();
#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
    pm_demo_hbn_wdt_configure();
#endif
    printf("PM demo ready: app_pm_enter <PDS|HBN> <level> <wake_ms> [rtc|timer]\r\n");
    shell_init();
#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
    pm_demo_hbn_wdt_start();
#endif
    while (1) {
#if defined(CONFIG_EXAMPLE_PM_HBN_WDT)
        pm_demo_hbn_wdt_service();
#endif
        ch = bflb_uart_getchar(g_uart);
        if (ch != -1) {
            shell_handler(ch);
        }
    }
}
