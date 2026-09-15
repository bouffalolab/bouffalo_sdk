#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
#include <stdbool.h>

#include "bflb_kys_v2.h"
#include "bflb_irq.h"
#endif
#include "bflb_mtimer.h"
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
#include "bflb_rtc.h"
#endif
#include "bflb_uart.h"
#include "board.h"
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP) || \
    defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
#include "board_gpio.h"
#endif
#include "shell.h"
#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
#include "bl618dg_aon.h"
#endif
#include "bl618dg_hbn.h"
#include "bl618dg_pds.h"
#include "bl618dg_pm.h"
#include "bl618dg_lp.h"
#include "cli_helper.h"

#define PM_DEMO_PRINT_DELAY_US 500U

/* Timer wakeup source selector for the optional app_pm_enter argument. */
#define PM_DEMO_TIMER_SRC_RTC       0U
#define PM_DEMO_TIMER_SRC_PDS_TIMER 1U

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
#define PM_DEMO_RTC_HZ 32768ULL
#define PM_DEMO_HBN_RTC_STATUS_MASK (1UL << HBN_INT_RTC)
#else
#define PM_DEMO_HBN_RTC_STATUS_MASK 0UL
#endif

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
#define PM_DEMO_HBN_GPIO_MASK 0x000000ffUL
#define PM_DEMO_PDS_GPIO_MASK (~(uint64_t)PM_DEMO_HBN_GPIO_MASK)
#endif

#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
#define PM_DEMO_HBN_BOD_STATUS_MASK (1UL << HBN_INT_BOD)
#else
#define PM_DEMO_HBN_BOD_STATUS_MASK 0UL
#endif

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP) || \
    defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP) || \
    defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
#define PM_DEMO_HBN_IRQ_OUT_WAKEUP
#endif

static struct bflb_device_s *g_uart;
#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
#define PM_DEMO_KEYSCAN_REQUIRED_PRESSES 2U
#define PM_DEMO_KEYSCAN_POLL_TIMEOUT    100U

static struct bflb_device_s *g_kys;
static const struct board_kys_v2_gpio_config_s *g_kys_gpio;
static struct bflb_kys_v2_result_s g_kys_result;
static uint8_t g_kys_press_count;
#if !defined(CONFIG_EXAMPLE_PM_KEYSCAN_POLL_MODE)
static volatile bool g_kys_result_ready;
#endif

static struct bflb_kys_v2_config_s g_kys_config = {
    .deglitch_en = 1,
    .deglitch_cnt = 3,
    .idle_duration = 0,
    .ghost_en = 1,
    .pds_drive_value = 0,
    .pds_input_pull_en = 1,
};
#endif

static void pm_demo_print_rtc_time(void)
{
    uint64_t rtc_time = bflb_rtc_get_time(NULL);

    printf("Current RTC time: %lu.%03lu\r\n",
           (unsigned long)(rtc_time >> 15),
           (unsigned long)((rtc_time & 0x7FFF) * 1000 / 32768));
}

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
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

static int pm_demo_pds_level_valid(uint32_t level)
{
    return (level == PM_PDS_LEVEL_1) || (level == PM_PDS_LEVEL_2) ||
           (level == PM_PDS_LEVEL_3) || (level == PM_PDS_LEVEL_7) ||
           (level == PM_PDS_LEVEL_15);
}

#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
static uint32_t pm_demo_get_and_clear_hbn_status(void)
{
    uint32_t status_mask = PM_DEMO_HBN_RTC_STATUS_MASK |
                           PM_DEMO_HBN_BOD_STATUS_MASK;

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    /* Runtime GPIO configuration is lost across HBN reset. Read all HBN
     * GPIO status bits so a CLI-configured pin is still reported. */
    status_mask |= PM_DEMO_HBN_GPIO_MASK;
#endif
    uint32_t status = HBN_Get_All_INT_State() & status_mask;

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    for (uint8_t i = HBN_INT_GPIO0; i <= HBN_INT_GPIO7; i++) {
        if ((status & (1UL << i)) != 0U) {
            HBN_Clear_IRQ(i);
        }
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    if ((status & (1UL << HBN_INT_RTC)) != 0U) {
        HBN_Clear_RTC_INT();
        HBN_Clear_IRQ(HBN_INT_RTC);
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
    if ((status & (1UL << HBN_INT_BOD)) != 0U) {
        HBN_Clear_IRQ(HBN_INT_BOD);
    }
#endif

    return status;
}

static void pm_demo_print_hbn_status(uint32_t status)
{
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    for (uint8_t i = HBN_INT_GPIO0; i <= HBN_INT_GPIO7; i++) {
        if ((status & (1UL << i)) != 0U) {
            printf("  HBN GPIO%u\r\n", i);
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

    if (status != 0U) {
        printf("PM demo HBN wakeup sources:\r\n");
        pm_demo_print_hbn_status(status);
        return;
    }
#endif
    printf("PM demo cold boot\r\n");
}

#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
static void pm_demo_keyscan_print_result(
    const struct bflb_kys_v2_result_s *result)
{
    if ((result->status & BFLB_KYS_V2_INT_GHOST) != 0U) {
        printf("PM demo keyscan ghost\r\n");
    }
    if (result->count == 0U) {
        return;
    }

    printf("PM demo keyscan: key_num=%u", result->count);
    for (uint32_t i = 0; i < result->count; i++) {
        uint8_t keycode = result->keycode[i];

        printf(" key[%u]=%u row=%u col=%u", (unsigned)i,
               (unsigned)keycode, (unsigned)(keycode / g_kys_config.col),
               (unsigned)(keycode % g_kys_config.col));
    }
    printf("\r\n");
}

#if !defined(CONFIG_EXAMPLE_PM_KEYSCAN_POLL_MODE)
static void pm_demo_keyscan_isr(int irq, void *arg)
{
    uint32_t status = bflb_kys_v2_get_int_status(g_kys);

    (void)irq;
    (void)arg;
    if ((status & (BFLB_KYS_V2_INT_DONE | BFLB_KYS_V2_INT_FIFO_FULL |
                   BFLB_KYS_V2_INT_GHOST)) == 0U) {
        return;
    }

    bflb_kys_v2_get_result(g_kys, &g_kys_result);
    bflb_kys_v2_disable(g_kys);
    bflb_kys_v2_int_clear(g_kys, BFLB_KYS_V2_INT_CLEAR_ALL);
    g_kys_result_ready = true;
}
#endif

#if !defined(CONFIG_EXAMPLE_PM_KEYSCAN_POLL_MODE)
static void pm_demo_keyscan_start(void)
{
    g_kys_result_ready = false;
    g_kys_result = (struct bflb_kys_v2_result_s){ 0 };
    bflb_kys_v2_int_clear(g_kys, BFLB_KYS_V2_INT_CLEAR_ALL);
    bflb_irq_clear_pending(g_kys->irq_num);
    bflb_kys_v2_int_enable(g_kys,
                           BFLB_KYS_V2_INT_DONE |
                               BFLB_KYS_V2_INT_FIFO_FULL |
                               BFLB_KYS_V2_INT_GHOST,
                           true);
    bflb_irq_enable(g_kys->irq_num);
    bflb_kys_v2_enable(g_kys);
}
#endif

static void pm_demo_keyscan_stop(void)
{
    bflb_kys_v2_disable(g_kys);
#if !defined(CONFIG_EXAMPLE_PM_KEYSCAN_POLL_MODE)
    bflb_kys_v2_int_enable(g_kys,
                           BFLB_KYS_V2_INT_DONE |
                               BFLB_KYS_V2_INT_FIFO_FULL |
                               BFLB_KYS_V2_INT_GHOST,
                           false);
    bflb_irq_disable(g_kys->irq_num);
    bflb_irq_clear_pending(g_kys->irq_num);
#endif
}

static int pm_demo_keyscan_wait_for_presses(void)
{
    printf("PM demo keyscan: press %u keys before shell startup\r\n",
           PM_DEMO_KEYSCAN_REQUIRED_PRESSES);
    g_kys_press_count = 0;

#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_POLL_MODE)
    while (g_kys_press_count < PM_DEMO_KEYSCAN_REQUIRED_PRESSES) {
        int ret = bflb_kys_v2_trigger_poll(g_kys, &g_kys_result,
                                           PM_DEMO_KEYSCAN_POLL_TIMEOUT);

        if (ret == -ETIMEDOUT) {
            continue;
        }
        if (ret != 0) {
            printf("PM demo keyscan poll failed: %d\r\n", ret);
            return ret;
        }
        pm_demo_keyscan_print_result(&g_kys_result);
        if (g_kys_result.count != 0U) {
            g_kys_press_count++;
            printf("PM demo keyscan press count: %u/%u\r\n",
                   g_kys_press_count, PM_DEMO_KEYSCAN_REQUIRED_PRESSES);
        }
    }
#else
    pm_demo_keyscan_start();
    while (g_kys_press_count < PM_DEMO_KEYSCAN_REQUIRED_PRESSES) {
        if (g_kys_result_ready) {
            pm_demo_keyscan_print_result(&g_kys_result);
            if (g_kys_result.count != 0U) {
                g_kys_press_count++;
                printf("PM demo keyscan press count: %u/%u\r\n",
                       g_kys_press_count, PM_DEMO_KEYSCAN_REQUIRED_PRESSES);
                /* wait for key release */
                bflb_mtimer_delay_ms(1000);
            }
            g_kys_result_ready = false;
            if (g_kys_press_count < PM_DEMO_KEYSCAN_REQUIRED_PRESSES) {
                bflb_irq_clear_pending(g_kys->irq_num);
                bflb_kys_v2_enable(g_kys);
            }
        }
        bflb_mtimer_delay_ms(1);
    }
    pm_demo_keyscan_stop();
#endif

    printf("PM demo keyscan: %u key presses detected\r\n",
           PM_DEMO_KEYSCAN_REQUIRED_PRESSES);
    return 0;
}

static int pm_demo_keyscan_init(void)
{
    g_kys_gpio = board_kys_v2_gpio_config_get();
    if ((g_kys_gpio == NULL) || (g_kys_gpio->row_count < 1U) ||
        (g_kys_gpio->row_count > BFLB_KYS_V2_MAX_KEYS) ||
        (g_kys_gpio->col_count < 1U) ||
        (g_kys_gpio->col_count > BFLB_KYS_V2_MAX_KEYS) ||
        (g_kys_gpio->row_pins == NULL) || (g_kys_gpio->col_pins == NULL)) {
        return -ENODEV;
    }

    g_kys = bflb_device_get_by_name(BFLB_NAME_KYS_V2);
    if (g_kys == NULL) {
        return -ENODEV;
    }

    g_kys_config.row = g_kys_gpio->row_count;
    g_kys_config.col = g_kys_gpio->col_count;
    bflb_kys_v2_matrix_init(g_kys_config.row, g_kys_config.col,
                            g_kys_gpio->row_pins, g_kys_gpio->col_pins);
    bflb_kys_v2_init(g_kys, &g_kys_config);
#if !defined(CONFIG_EXAMPLE_PM_KEYSCAN_POLL_MODE)
    if (bflb_irq_attach(g_kys->irq_num, pm_demo_keyscan_isr, NULL) != 0) {
        return -EIO;
    }
#endif
    return 0;
}
#endif

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

    if (wake_ms == 0U) {
        return;
    }

    bflb_rtc_set_time(NULL, pm_demo_ms_to_rtc_ticks(wake_ms));
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
               (unsigned long)(pm_demo_cli_gpio_get()->io_wakeup_unmask & UINT32_MAX));
        enabled = 1;
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
    printf(" BOD");
    enabled = 1;
#endif
#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    printf(" PDS-keyscan");
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

static void pm_demo_print_pds_wakeup(void)
{
#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    struct bflb_kys_v2_pds_wakeup_s keyscan = { 0 };
#endif
    uint32_t pds_sources = bl_lp_pds_get_wakeup_source();
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    uint64_t pds_gpio = bl_lp_pds_get_wakeup_gpio() &
                        pm_demo_cli_gpio_get()->io_wakeup_unmask & PM_DEMO_PDS_GPIO_MASK;
#endif
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    uint32_t hbn_status = pm_demo_get_and_clear_hbn_status();
#endif

#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    bflb_kys_v2_pds_get_wakeup_result(g_kys, &keyscan);
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
        if (pds_gpio == 0U) {
            printf("  PDS GPIO INT (detail unavailable)\r\n");
        }
        for (uint8_t pin = 8; pin < GPIO_PIN_MAX; pin++) {
            if ((pds_gpio & (1ULL << pin)) != 0U) {
                printf("  PDS GPIO%u\r\n", pin);
            }
        }
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    if ((pds_sources & (1UL << PDS_WAKEUP_BY_KEYSCAN)) != 0U) {
        printf("  keyscan row=%u col=%u row_gpio=%u col_gpio=%u\r\n",
               keyscan.row, keyscan.col, keyscan.row_gpio, keyscan.col_gpio);
    }
#endif
    if ((pds_sources & (1UL << PDS_WAKEUP_BY_PDS_TIMER)) != 0U) {
        printf("  PDS timer\r\n");
    }
}

static int pm_demo_enter_pds(uint32_t level, uint32_t wake_ms,
                             uint32_t timer_src)
{
    int ret;
    uint32_t pds_sleep_time = 0U;

#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    pm_demo_keyscan_stop();
#endif
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    printf("Enter PDS%lu: %s delay=%lu ms (0=disabled)\r\n",
           (unsigned long)level,
           (timer_src == PM_DEMO_TIMER_SRC_PDS_TIMER) ? "PDS-timer" : "RTC",
           (unsigned long)wake_ms);
#else
    printf("Enter PDS%lu\r\n", (unsigned long)level);
#endif
    arch_delay_us(PM_DEMO_PRINT_DELAY_US);

    PDS_Mask_All_Wakeup_Src();
    PDS_IntClear();
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    (void)pm_demo_get_and_clear_hbn_status();
#endif
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    PDS_Set_All_GPIO_Pad_IntClr();
#endif

#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    if (level == PM_PDS_LEVEL_15) {
        bflb_kys_v2_pds_gpio_configure(g_kys_gpio->col_count,
                                       g_kys_gpio->col_pins);
    }
    bflb_kys_v2_pds_enable(g_kys, true);

    /* Clear latches after KYD has settled and immediately before sleep. */
    bflb_kys_v2_pds_clear_wakeup(g_kys);
#endif
    PDS_IntClear();
#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP) || \
    defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);
#endif
#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    pm_demo_pds_gpio_wakeup_arm();
#endif
#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_KEYSCAN, UNMASK);
#endif

    pm_pds_irq_register();
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    (void)pm_demo_get_and_clear_hbn_status();
#endif
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    if (timer_src == PM_DEMO_TIMER_SRC_PDS_TIMER) {
        uint64_t ticks = pm_demo_ms_to_rtc_ticks(wake_ms);

        if (ticks > UINT32_MAX) {
            printf("PDS timer wake_ms too large\r\n");
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

    /* RTC wake routes through HBN_IRQ_OUT; the PDS timer uses its own source. */
    ret = bl_lp_pds_enter_with_restore(level, pds_sleep_time);

    pm_demo_print_rtc_time();
    pm_demo_print_pds_wakeup();
#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    bflb_kys_v2_pds_enable(g_kys, false);
    bflb_kys_v2_pds_clear_wakeup(g_kys);
    PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_KEYSCAN, MASK);
    if (level == PM_PDS_LEVEL_15) {
        bflb_kys_v2_pds_gpio_deconfigure(g_kys_gpio->col_count,
                                         g_kys_gpio->col_pins);
    }
#endif
    PDS_Mask_All_Wakeup_Src();
    PDS_IntClear();

#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    bflb_kys_v2_matrix_init(g_kys_config.row, g_kys_config.col,
                            g_kys_gpio->row_pins, g_kys_gpio->col_pins);
    bflb_kys_v2_init(g_kys, &g_kys_config);
#endif
    shell_init();

    return ret;
}

static int pm_demo_enter_hbn(uint32_t wake_ms)
{
#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    pm_demo_keyscan_stop();
#endif
#if defined(PM_DEMO_HBN_IRQ_OUT_WAKEUP)
    (void)pm_demo_get_and_clear_hbn_status();
#endif

#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    printf("Enter HBN0: RTC delay=%lu ms (0=disabled)\r\n",
           (unsigned long)wake_ms);
#else
    printf("Enter HBN0\r\n");
#endif
    arch_delay_us(PM_DEMO_PRINT_DELAY_US);
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    pm_hbn_mode_enter(PM_HBN_LEVEL_0, pm_demo_ms_to_rtc_ticks(wake_ms));
#else
    (void)wake_ms;
    pm_hbn_mode_enter(PM_HBN_LEVEL_0, 0);
#endif
    return 0;
}

static int app_pm_enter(int argc, char **argv)
{
    uint32_t level;
    uint32_t wake_ms;
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
    uint32_t timer_src = PM_DEMO_TIMER_SRC_RTC;
#endif

    if ((argc != 4) && (argc != 5)) {
        printf("Usage: app_pm_enter <PDS|HBN> <level> <wake_ms> [rtc|timer]\r\n");
        printf("  PDS levels: 1, 2, 3, 7, 15\r\n");
        printf("  HBN levels: 0\r\n");
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
        } else if (strcmp(argv[4], "rtc") == 0) {
            timer_src = PM_DEMO_TIMER_SRC_RTC;
        } else {
            printf("Timer source must be rtc or timer\r\n");
            return -EINVAL;
        }
    }
#else
    if (wake_ms != 0U) {
        printf("RTC wakeup is disabled; wake_ms must be 0\r\n");
        return -EINVAL;
    }
#endif

    if ((strcmp(argv[1], "PDS") == 0) || (strcmp(argv[1], "pds") == 0)) {
        if (!pm_demo_pds_level_valid(level)) {
            printf("PDS level must be 1, 2, 3, 7, or 15\r\n");
            return -EINVAL;
        }
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
        return pm_demo_enter_pds(level, wake_ms, timer_src);
#else
        return pm_demo_enter_pds(level, wake_ms, PM_DEMO_TIMER_SRC_RTC);
#endif
    }

    if ((strcmp(argv[1], "HBN") == 0) || (strcmp(argv[1], "hbn") == 0)) {
        if (level != PM_HBN_LEVEL_0) {
            printf("HBN level must be 0\r\n");
            return -EINVAL;
        }
#if defined(CONFIG_EXAMPLE_PM_RTC_WAKEUP)
        if (timer_src == PM_DEMO_TIMER_SRC_PDS_TIMER) {
            printf("HBN only supports the rtc timer source\r\n");
            return -EINVAL;
        }
#endif
        return pm_demo_enter_hbn(wake_ms);
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

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    const struct board_lp_gpio_wakeup_config_s *board_gpio_wakeup;
#endif

    board_init();

    printf("PM demo starting...\r\n");

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    board_gpio_wakeup = board_lp_gpio_wakeup_config_get();
#endif
    pm_demo_print_boot_wakeup();

    pm_demo_print_rtc_time();
    bflb_mtimer_delay_ms(500);
    pm_demo_print_rtc_time();

#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    if (pm_demo_keyscan_init() != 0) {
        printf("PM demo keyscan board configuration is unavailable\r\n");
        return -ENODEV;
    }
    bflb_kys_v2_pds_configure(g_kys, &g_kys_config);
#endif

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
    ret = pm_demo_cli_gpio_init(board_gpio_wakeup,
#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
                                g_kys_gpio
#else
                                NULL
#endif
                                );
    if (ret != 0) {
        printf("PM demo GPIO wakeup board configuration is unavailable: %d\r\n",
               ret);
        return ret;
    }
    ret = pm_demo_cli_gpio_apply();
    if (ret != 0) {
        printf("PM demo GPIO wakeup configuration failed: %d\r\n", ret);
        return ret;
    }
#endif
#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
    ret = pm_demo_cli_bod_init();
    if (ret != 0) {
        printf("PM demo BOD wakeup configuration failed: %d\r\n", ret);
        return -EIO;
    }
#endif

    g_uart = bflb_device_get_by_name("uart0");
    if (g_uart == NULL) {
        printf("PM demo UART0 is unavailable\r\n");
        return -ENODEV;
    }

    pm_demo_print_enabled_wakeup_sources();
#if defined(CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP)
    ret = pm_demo_keyscan_wait_for_presses();
    if (ret != 0) {
        return ret;
    }
#endif
    printf("PM demo ready: app_pm_enter <PDS|HBN> <level> <wake_ms> [rtc|timer]\r\n");
    shell_init();
    while (1) {
        ch = bflb_uart_getchar(g_uart);
        if (ch != -1) {
            shell_handler(ch);
        }
    }
}
