#include <stdio.h>
#include <errno.h>

#include "bflb_kys_v2.h"
#include "bflb_irq.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "board_gpio.h"
#include "bl618dg_hbn.h"
#include "bl618dg_lp.h"
#include "bl618dg_pds.h"
#include "bl618dg_pm.h"

#define KEYSCAN_IDLE_DUR     0
#define KEYSCAN_DEGLITCH_CNT 3
#define KEYSCAN_POLL_TIMEOUT 100
#define KEYSCAN_PDS_LEVEL         PM_PDS_LEVEL_15
#define KEYSCAN_PDS_AFTER_PRESSES 8

static struct bflb_device_s *kys;
static struct bflb_kys_v2_result_s keyscan_result;
static uint8_t keyscan_press_count;

#if !defined(CONFIG_EXAMPLE_KYS_V2_POLL_MODE)
static volatile bool keyscan_result_ready;
#endif

static void keyscan_print_result(const struct bflb_kys_v2_config_s *config)
{
    if (keyscan_result.status & BFLB_KYS_V2_INT_GHOST) {
        printf("keyscan ghost\r\n");
    }
    if (keyscan_result.count == 0) {
        return;
    }
    printf("key_num:%u\r\n", keyscan_result.count);
    for (uint32_t i = 0; i < keyscan_result.count; i++) {
        uint8_t code = keyscan_result.keycode[i];
        printf("key[%u]=%u row=%u col=%u\r\n", (unsigned)i, code,
               (unsigned)(code / config->col),
               (unsigned)(code % config->col));
    }
}

#if defined(CONFIG_EXAMPLE_KYS_V2_POLL_MODE)
static void keyscan_print_timeout(void)
{
    printf("keyscan timeout: raw=0x%08lx fifo=%u\r\n",
           (unsigned long)keyscan_result.status, keyscan_result.count);
}
#endif

static void keyscan_enter_pds(const struct bflb_kys_v2_config_s *config,
                              const struct board_kys_v2_gpio_config_s *gpio_config)
{
    struct bflb_kys_v2_pds_wakeup_s wakeup_result;
    uint32_t wakeup_sources;
    uintptr_t irq_flags;

    bflb_kys_v2_disable(kys);
    bflb_kys_v2_int_enable(kys, BFLB_KYS_V2_INT_DONE |
                                  BFLB_KYS_V2_INT_FIFO_FULL |
                                  BFLB_KYS_V2_INT_GHOST, false);
#if !defined(CONFIG_EXAMPLE_KYS_V2_POLL_MODE)
    bflb_irq_disable(kys->irq_num);
    bflb_irq_clear_pending(kys->irq_num);
    keyscan_result_ready = false;
    keyscan_result = (struct bflb_kys_v2_result_s){ 0 };
#endif
    irq_flags = bflb_irq_save();

    PDS_Mask_All_Wakeup_Src();
    HBN_Pin_WakeUp_Mask(0x0f);
    PDS_IntClear();
    /* PDS15 powers down the GPIO domain, so its pad IE/pull settings must be
     * provided through PDS. The shallower PDS levels keep the GPIO domain on. */
    if (KEYSCAN_PDS_LEVEL == PM_PDS_LEVEL_15) {
        bflb_kys_v2_pds_gpio_configure(gpio_config->col_count, gpio_config->col_pins);
    }
    bflb_kys_v2_pds_configure(kys, config);
    bflb_kys_v2_pds_enable(kys, true);

    /* Enabling KYD can update its wakeup latch while the matrix settles. */
    bflb_kys_v2_pds_clear_wakeup(kys);
    PDS_IntClear();
    PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_KEYSCAN, UNMASK);
    pm_pds_irq_register();
    bl_lp_pds_enter_with_restore(KEYSCAN_PDS_LEVEL, 0);

    wakeup_sources = PDS_Get_All_Wakeup_Src();
    bflb_kys_v2_pds_get_wakeup_result(kys, &wakeup_result);
    printf("PDS%u wakeup: sources=0x%08x keyscan=%u",
           KEYSCAN_PDS_LEVEL, (unsigned)wakeup_sources, (unsigned)wakeup_result.wakeup);
    if (wakeup_result.wakeup) {
        printf(" row=%u col=%u row_gpio=%u col_gpio=%u",
               wakeup_result.row, wakeup_result.col,
               wakeup_result.row_gpio, wakeup_result.col_gpio);
        if ((wakeup_result.row < config->row) && (wakeup_result.col < config->col)) {
            printf(" key=%u",
                   (unsigned)(wakeup_result.row * config->col + wakeup_result.col));
        }
    }
    printf("\r\n");

    PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_KEYSCAN, MASK);
    bflb_kys_v2_pds_enable(kys, false);
    bflb_kys_v2_pds_clear_wakeup(kys);
    if (KEYSCAN_PDS_LEVEL == PM_PDS_LEVEL_15) {
        bflb_kys_v2_pds_gpio_deconfigure(gpio_config->col_count, gpio_config->col_pins);
    }
    PDS_IntClear();
    bflb_irq_clear_pending(PDS_WAKEUP_IRQn);
    bflb_irq_clear_pending(kys->irq_num);

    /* Wait for the wakeup key to be released and debounced; otherwise the
     * same press can be reported again after the keyscan is reinitialized. */
    bflb_mtimer_delay_ms(3000);
    bflb_kys_v2_matrix_init(config->row, config->col,
                            gpio_config->row_pins, gpio_config->col_pins);
    bflb_kys_v2_init(kys, config);
#if !defined(CONFIG_EXAMPLE_KYS_V2_POLL_MODE)
    keyscan_result_ready = false;
    keyscan_result = (struct bflb_kys_v2_result_s){ 0 };
    bflb_kys_v2_int_enable(kys, BFLB_KYS_V2_INT_DONE |
                                  BFLB_KYS_V2_INT_FIFO_FULL |
                                  BFLB_KYS_V2_INT_GHOST, true);
    bflb_irq_clear_pending(kys->irq_num);
    bflb_irq_enable(kys->irq_num);
    bflb_kys_v2_enable(kys);
#endif
    bflb_irq_restore(irq_flags);
}

static void keyscan_count_press(void)
{
    if (keyscan_result.count == 0U) {
        return;
    }

    if (keyscan_press_count < KEYSCAN_PDS_AFTER_PRESSES) {
        keyscan_press_count++;
    }
    printf("key press count: %u/%u\r\n", keyscan_press_count,
           KEYSCAN_PDS_AFTER_PRESSES);
}

#if !defined(CONFIG_EXAMPLE_KYS_V2_POLL_MODE)
static void kys_isr(int irq, void *arg)
{
    uint32_t status = bflb_kys_v2_get_int_status(kys);

    (void)irq;
    (void)arg;
    if (status == 0) {
        return;
    }
    if (status & (BFLB_KYS_V2_INT_DONE | BFLB_KYS_V2_INT_FIFO_FULL | BFLB_KYS_V2_INT_GHOST)) {
        bflb_kys_v2_get_result(kys, &keyscan_result);
        bflb_kys_v2_disable(kys);
        bflb_kys_v2_int_clear(kys, BFLB_KYS_V2_INT_CLEAR_ALL);
        keyscan_result_ready = true;
    }
}
#endif

int main(void)
{
    const struct board_kys_v2_gpio_config_s *gpio_config;
    struct bflb_kys_v2_config_s config = {
        .deglitch_en = 1,
        .deglitch_cnt = KEYSCAN_DEGLITCH_CNT,
        .idle_duration = KEYSCAN_IDLE_DUR,
        .ghost_en = 1,
        .pds_drive_value = 0,
        .pds_input_pull_en = 1,
    };

    board_init();
    /* Keep the low-power key detector clock independent of the 40 MHz XTAL. */
    HBN_32K_Sel(HBN_32K_RC);
    gpio_config = board_kys_v2_gpio_config_get();
    if ((gpio_config == NULL) || (gpio_config->row_count < 1U) ||
        (gpio_config->row_count > BFLB_KYS_V2_MAX_KEYS) ||
        (gpio_config->col_count < 1U) ||
        (gpio_config->col_count > BFLB_KYS_V2_MAX_KEYS) ||
        (gpio_config->row_pins == NULL) ||
        (gpio_config->col_pins == NULL)) {
        printf("KYS v2 board GPIO configuration is unavailable\r\n");
        return -ENODEV;
    }
    config.row = gpio_config->row_count;
    config.col = gpio_config->col_count;
    kys = bflb_device_get_by_name(BFLB_NAME_KYS_V2);
    bflb_kys_v2_matrix_init(config.row, config.col,
                            gpio_config->row_pins,
                            gpio_config->col_pins);
    bflb_kys_v2_init(kys, &config);

#if defined(CONFIG_EXAMPLE_KYS_V2_POLL_MODE)
    printf("BL618DG KYS v2 query mode\r\n");
#else
    printf("BL618DG KYS v2 interrupt mode, irq=%u\r\n", kys->irq_num);
    bflb_irq_attach(kys->irq_num, kys_isr, NULL);
    bflb_irq_clear_pending(kys->irq_num);
    bflb_kys_v2_int_enable(kys, BFLB_KYS_V2_INT_DONE | BFLB_KYS_V2_INT_FIFO_FULL | BFLB_KYS_V2_INT_GHOST, true);
    bflb_irq_enable(kys->irq_num);
    bflb_kys_v2_enable(kys);
#endif

    while (1) {
#if defined(CONFIG_EXAMPLE_KYS_V2_POLL_MODE)
        while (1) {
            int ret = bflb_kys_v2_trigger_poll(kys, &keyscan_result, KEYSCAN_POLL_TIMEOUT);

            if (ret == 0) {
                keyscan_print_result(&config);
                keyscan_count_press();
            } else if (ret == -ETIMEDOUT) {
                keyscan_print_timeout();
            } else {
                printf("keyscan poll failed: %d\r\n", ret);
            }
            if (keyscan_press_count >= KEYSCAN_PDS_AFTER_PRESSES) {
                break;
            }
            bflb_mtimer_delay_ms(300);
        }
#else
        while (1) {
            if (keyscan_result_ready) {
                keyscan_print_result(&config);
                keyscan_count_press();
                keyscan_result_ready = false;
                if (keyscan_press_count >= KEYSCAN_PDS_AFTER_PRESSES) {
                    break;
                }
                bflb_irq_clear_pending(kys->irq_num);
                bflb_kys_v2_enable(kys);
            }
            bflb_mtimer_delay_ms(1);
        }
#endif

        printf("%u key presses detected, enter PDS%u; press a matrix key to wake\r\n",
           KEYSCAN_PDS_AFTER_PRESSES, KEYSCAN_PDS_LEVEL);
        bflb_mtimer_delay_ms(1000);
        keyscan_enter_pds(&config, gpio_config);
        keyscan_press_count = 0;
        printf("PDS wakeup, key press count reset\r\n");
    }
}
