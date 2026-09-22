#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli_helper.h"
#include "shell.h"

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP) || \
    defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
static int pm_demo_cli_parse_u32(const char *text, uint32_t *value)
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
#endif

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
#include "bl616cl_pds.h"
#include "bl616cl_pm.h"

static struct board_lp_gpio_wakeup_config_s g_gpio_wakeup;

static int pm_demo_cli_gpio_configure(
    const struct board_lp_gpio_wakeup_config_s *wakeup)
{
    lp_gpio_cfg_type gpio_cfg = { 0 };
    const uint64_t valid_gpio_mask = (1ULL << GPIO_PIN_MAX) - 1ULL;

    if ((((wakeup->io_ie | wakeup->io_pu | wakeup->io_pd |
           wakeup->io_wakeup_unmask) & ~valid_gpio_mask) != 0U) ||
        ((wakeup->io_wakeup_unmask & ~wakeup->io_ie) != 0U) ||
        ((wakeup->io_pu & wakeup->io_pd) != 0U)) {
        return -EINVAL;
    }

    gpio_cfg.io_ie = wakeup->io_ie;
    gpio_cfg.io_pu = wakeup->io_pu;
    gpio_cfg.io_pd = wakeup->io_pd;
    memcpy(gpio_cfg.io_0_36_trig_mode, wakeup->io_0_36_trig_mode,
           sizeof(gpio_cfg.io_0_36_trig_mode));
    gpio_cfg.io_wakeup_unmask = wakeup->io_wakeup_unmask;

    return pm_lowpower_gpio_cfg(&gpio_cfg);
}

static int pm_demo_cli_gpio_parse_trigger(const char *text, uint8_t *trigger)
{
    if (strcmp(text, "falling") == 0) {
        *trigger = PDS_GPIO_INT_ASYNC_FALLING_EDGE;
    } else if (strcmp(text, "rising") == 0) {
        *trigger = PDS_GPIO_INT_ASYNC_RISING_EDGE;
    } else if (strcmp(text, "low") == 0) {
        *trigger = PDS_GPIO_INT_ASYNC_LOW_LEVEL;
    } else if (strcmp(text, "high") == 0) {
        *trigger = PDS_GPIO_INT_ASYNC_HIGH_LEVEL;
    } else {
        return -EINVAL;
    }
    return 0;
}

static int pm_demo_cli_gpio_parse_pull(const char *text, uint8_t *pull)
{
    if (strcmp(text, "none") == 0) {
        *pull = 0;
    } else if (strcmp(text, "up") == 0) {
        *pull = 1;
    } else if (strcmp(text, "down") == 0) {
        *pull = 2;
    } else {
        return -EINVAL;
    }
    return 0;
}

int pm_demo_cli_gpio_init(
    const struct board_lp_gpio_wakeup_config_s *board_config)
{
    if (board_config == NULL) {
        return -ENODEV;
    }

    g_gpio_wakeup = *board_config;
    return 0;
}

int pm_demo_cli_gpio_apply(void)
{
    return pm_demo_cli_gpio_configure(&g_gpio_wakeup);
}

const struct board_lp_gpio_wakeup_config_s *pm_demo_cli_gpio_get(void)
{
    return &g_gpio_wakeup;
}

static int app_pm_gpio(int argc, char **argv)
{
    struct board_lp_gpio_wakeup_config_s candidate;
    uint64_t pin_mask;
    uint32_t pin;
    uint8_t trigger;
    uint8_t pull;
    int ret;

    if ((argc == 2) && (strcmp(argv[1], "reset") == 0)) {
        memset(&candidate, 0, sizeof(candidate));
        ret = pm_demo_cli_gpio_configure(&candidate);
        if (ret != 0) {
            printf("GPIO wakeup reset failed: %d\r\n", ret);
            return ret;
        }
        g_gpio_wakeup = candidate;
        PDS_Set_All_GPIO_Pad_IntClr();
        printf("GPIO wakeup configuration cleared\r\n");
        return 0;
    }

    if (argc != 4) {
        printf("Usage: app_pm_gpio <gpio> <falling|rising|low|high> "
               "<none|up|down>\r\n");
        printf("       app_pm_gpio reset\r\n");
        return -EINVAL;
    }
    if ((pm_demo_cli_parse_u32(argv[1], &pin) != 0) ||
        (pin >= GPIO_PIN_MAX) ||
        (pm_demo_cli_gpio_parse_trigger(argv[2], &trigger) != 0) ||
        (pm_demo_cli_gpio_parse_pull(argv[3], &pull) != 0)) {
        printf("Invalid GPIO wakeup configuration\r\n");
        return -EINVAL;
    }

    candidate = g_gpio_wakeup;
    pin_mask = 1ULL << pin;
    candidate.io_0_36_trig_mode[pin] = trigger;
    candidate.io_ie |= pin_mask;
    candidate.io_pu &= ~pin_mask;
    candidate.io_pd &= ~pin_mask;
    if (pull == 1U) {
        candidate.io_pu |= pin_mask;
    } else if (pull == 2U) {
        candidate.io_pd |= pin_mask;
    }
    candidate.io_wakeup_unmask |= pin_mask;

    ret = pm_demo_cli_gpio_configure(&candidate);
    if (ret != 0) {
        printf("GPIO wakeup configuration failed: %d\r\n", ret);
        return ret;
    }
    g_gpio_wakeup = candidate;
    PDS_Set_All_GPIO_Pad_IntClr();
    printf("GPIO%lu wakeup configured: %s, pull-%s, %s\r\n",
           (unsigned long)pin, argv[2], argv[3],
           (pin <= GPIO_PIN_5) ? "PDS/HBN" : "PDS only");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(app_pm_gpio, app_pm_gpio,
                       configure PM GPIO wakeup or reset all GPIO wakeup);
#endif

#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
#include "bflb_irq.h"
#include "bl616cl_hbn.h"

static uint8_t g_bod_threshold = CONFIG_EXAMPLE_PM_BOD_THRESHOLD;
static const uint16_t g_bod_threshold_mv[] = {
    2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900,
};

static int pm_demo_cli_bod_configure(uint8_t threshold)
{
    /* board_init() installs an active-mode BOD ISR that deliberately stops
     * execution. This demo consumes BOD through the HBN wakeup status. */
    bflb_irq_disable(BOD_IRQn);
    return HBN_Set_BOD_Config(1, threshold, HBN_BOD_MODE_POR_INDEPENDENT);
}

int pm_demo_cli_bod_init(void)
{
    return pm_demo_cli_bod_configure(g_bod_threshold);
}

static int app_pm_bod(int argc, char **argv)
{
    uint32_t threshold;
    uint16_t threshold_mv;
    int ret;

    if ((argc != 2) || (pm_demo_cli_parse_u32(argv[1], &threshold) != 0) ||
        (threshold >= (sizeof(g_bod_threshold_mv) /
                       sizeof(g_bod_threshold_mv[0])))) {
        printf("Usage: app_pm_bod <0-7>\r\n");
        return -EINVAL;
    }

    HBN_Clear_IRQ(HBN_INT_BOD);
    ret = pm_demo_cli_bod_configure((uint8_t)threshold);
    if (ret != 0) {
        printf("BOD threshold configuration failed: %d\r\n", ret);
        return ret;
    }
    g_bod_threshold = (uint8_t)threshold;
    threshold_mv = g_bod_threshold_mv[threshold];
    printf("BOD threshold=%lu (%u.%u V), output=%u\r\n",
           (unsigned long)threshold, threshold_mv / 1000U,
           (threshold_mv % 1000U) / 100U,
           (unsigned int)HBN_Get_BOD_OUT_State());
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(app_pm_bod, app_pm_bod,
                       set PM BOD threshold: app_pm_bod <0-7>);
#endif
