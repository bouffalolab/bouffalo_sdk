
#if __has_include("board_rf_overlay.h")
/* Use board_rf_overlay.c instead of this file */
#else

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include "bflb_gpio.h"
#include "bflb_irq.h"
#include "board_rf.h"

#include "wl_api.h"
#include "rfparam_adapter.h"
#include "bl618dg_hbn.h"

#define USER_UNUSED(a) ((void)(a))

extern void cmd_set_btble_standalone(int argc, char **argv);
extern void cmd_set_btble_combo(int argc, char **argv);

static struct wl_cfg_t * board_rf_phyrf_cfg_get(void)
{
#if defined(WL_API_RMEM_EN) && WL_API_RMEM_EN
    return wl_cfg_get((uint8_t *)WL_API_RMEM_ADDR);
#else
    return wl_cfg_get();
#endif
}

static int ctl_rf_configuration(enum board_ctl_ops ops, va_list args)
{
  struct wl_cfg_t *wl_cfg = board_rf_phyrf_cfg_get();
  int ret;

  switch (ops) {
  case BRD_CTL_RF_RESET_DEFAULT:
      do {
        uint32_t xtal_value;
        int full_cal = va_arg(args, int);

        HBN_Get_Xtal_Value(&xtal_value);

        /* reset to default param */
        wl_cfg->en_param_load = full_cal ? 1 : 0;
        wl_cfg->en_full_cal = full_cal ? 1 : 0;
        wl_cfg->mode = WL_API_MODE_ALL;
        wl_cfg->param.xtalfreq_hz = xtal_value;
        //wl_cfg->param.xtalcapcode_in = 32;
        //wl_cfg->param.xtalcapcode_out = 32;
        wl_cfg->capcode_set = rfparam_set_capcode;
        wl_cfg->capcode_get = rfparam_get_capcode;
        wl_cfg->param_load = rfparam_load;
      } while(0);
      break;

  case BRD_CTL_RF_INIT_WLAN:
    wl_cfg->mode = WL_API_MODE_WLAN;
    break;

  case BRD_CTL_RF_INIT_BZ:
    wl_cfg->mode = WL_API_MODE_BZ;
    break;

  case BRD_CTL_RF_INIT_ALL:
    wl_cfg->mode = WL_API_MODE_ALL;
    break;

  case BRD_CTL_RF_SET_XTAL:
    do {
      int xtal = va_arg(args, int);
      wl_cfg->param.xtalfreq_hz = xtal;
    } while (0);
    break;

  case BRD_CTL_RF_SET_CAPCODE:
    do {
      int cap_in = va_arg(args, int);
      int cap_out = va_arg(args, int);
      wl_cfg->param.xtalcapcode_in = cap_in;
      wl_cfg->param.xtalcapcode_out = cap_out;
    } while (0);
    break;

  default:
    break;
  }

  ret = wl_init();
  if (ret != WL_API_STATUS_OK) {
    return -EINVAL;
  }
  wl_cfg->en_param_load = 0;
  wl_cfg->en_full_cal = 0;

  return 0;
}

/* board configuration */
int board_rf_ctl(enum board_ctl_ops ops, ...)
{
  va_list ops_arg;
  int ret = -ENOSYS;

  va_start(ops_arg, ops);

  if (ops >= BRD_CTL_RF_RESET_DEFAULT && ops <= BRD_CTL_RF_SET_CAPCODE) {
    ret = ctl_rf_configuration(ops, ops_arg);
  } else {
    va_end(ops_arg);
    return -ENOSYS;
  }

  va_end(ops_arg);
  return ret;
}

/* Retain a failed RF attempt: partial PHY initialization requires a reboot. */
static int board_rf_init_result = -EAGAIN;

int board_rf_init_status_get(void)
{
    uintptr_t flags = bflb_irq_save();
    int ret = board_rf_init_result;

    bflb_irq_restore(flags);
    return ret;
}

/* Bound once before stack startup; runtime callbacks never query FHOST. */
static int board_rf_spdt_gpio = -1;

static int board_rf_spdt_fixed(int pin, bool bt)
{
    struct bflb_device_s *gpio;
    volatile uint32_t *config;
    uint32_t value;

    if (pin < 0 || pin >= GPIO_PIN_MAX) {
        return -EINVAL;
    }
    gpio = bflb_device_get_by_name("gpio");
    if (gpio == NULL) {
        return -ENODEV;
    }

    /* Single-write handover: direct-output mode exposes a stable output latch.
     * Do not replay SET/CLR/interrupt-clear bits from a register snapshot. */
    config = (volatile uint32_t *)(gpio->reg_base + GLB_GPIO_CFG0_OFFSET + 4 * pin);
    value = *config & ~(GLB_REG_GPIO_0_MODE_MSK | GLB_REG_GPIO_0_FUNC_SEL_MSK |
                       GLB_REG_GPIO_0_IE_MSK | GLB_REG_GPIO_0_O_MSK |
                       GLB_REG_GPIO_0_SET_MSK | GLB_REG_GPIO_0_CLR_MSK |
                       GLB_REG_GPIO_0_INT_CLR_MSK | GLB_REG_GPIO_0_PU_MSK |
                       GLB_REG_GPIO_0_PD_MSK | GLB_REG_GPIO_0_DRV_MSK);
    value |= ((GPIO_FUNC_GPIO >> GPIO_FUNC_SHIFT) << GLB_REG_GPIO_0_FUNC_SEL_POS) |
             GLB_REG_GPIO_0_OE_MSK | GLB_REG_GPIO_0_SMT_MSK |
             GLB_REG_GPIO_0_INT_MASK_MSK;
    /* The board wiring must match the hardware PTA output parity. */
    if (bt == ((pin & 1) == 0)) {
        value |= GLB_REG_GPIO_0_O_MSK;
    }
    *config = value;
    return 0;
}

static int board_rf_spdt_apply(int pin, enum board_rf_spdt_mode mode)
{
    struct bflb_device_s *gpio;

    if (mode == BOARD_RF_SPDT_FIXED_BT || mode == BOARD_RF_SPDT_FIXED_2G) {
        return board_rf_spdt_fixed(pin, mode == BOARD_RF_SPDT_FIXED_BT);
    }
    if (mode != BOARD_RF_SPDT_DYNAMIC_PTA || pin < 0 || pin >= GPIO_PIN_MAX) {
        return -EINVAL;
    }
    gpio = bflb_device_get_by_name("gpio");
    if (gpio == NULL) {
        return -ENODEV;
    }
    /* Only a serialized recipe executor may hand the pin over to ready PTA. */
    bflb_gpio_init(gpio, pin, GPIO_FUNC_SPDT | GPIO_ALTERNATE);
    return 0;
}

static int board_rf_spdt_verify(int pin, enum board_rf_spdt_mode mode)
{
    struct bflb_device_s *gpio;
    uint32_t value;
    uint32_t mask = GLB_REG_GPIO_0_FUNC_SEL_MSK | GLB_REG_GPIO_0_OE_MSK |
                    GLB_REG_GPIO_0_MODE_MSK;
    uint32_t expected;

    if (pin < 0 || pin >= GPIO_PIN_MAX ||
        (mode != BOARD_RF_SPDT_FIXED_BT &&
         mode != BOARD_RF_SPDT_FIXED_2G &&
         mode != BOARD_RF_SPDT_DYNAMIC_PTA)) {
        return -EINVAL;
    }
    gpio = bflb_device_get_by_name("gpio");
    if (gpio == NULL) {
        return -ENODEV;
    }
    value = *(volatile uint32_t *)(gpio->reg_base + GLB_GPIO_CFG0_OFFSET + 4 * pin);
    if (mode != BOARD_RF_SPDT_DYNAMIC_PTA) {
        mask |= GLB_REG_GPIO_0_IE_MSK | GLB_REG_GPIO_0_O_MSK;
        expected = ((GPIO_FUNC_GPIO >> GPIO_FUNC_SHIFT) <<
                    GLB_REG_GPIO_0_FUNC_SEL_POS) | GLB_REG_GPIO_0_OE_MSK;
        if ((mode == BOARD_RF_SPDT_FIXED_BT) == ((pin & 1) == 0)) {
            expected |= GLB_REG_GPIO_0_O_MSK;
        }
    } else {
        expected = (GPIO_FUNC_SPDT >> GPIO_FUNC_SHIFT) <<
                    GLB_REG_GPIO_0_FUNC_SEL_POS;
        expected |= 1u << GLB_REG_GPIO_0_MODE_POS;
    }
    return (value & mask) == expected ? 0 : -EIO;
}

int board_rf_spdt_mode_apply(enum board_rf_spdt_mode mode)
{
    if (board_rf_init_status_get() != 0) {
        return -EAGAIN;
    }
    return board_rf_spdt_apply(board_rf_spdt_gpio, mode);
}

int board_rf_spdt_mode_verify(enum board_rf_spdt_mode mode)
{
    if (board_rf_init_status_get() != 0) {
        return -EAGAIN;
    }
    return board_rf_spdt_verify(board_rf_spdt_gpio, mode);
}

int board_rf_modes_get(bool standalone, int spdt_gpio, uint32_t *modes)
{
    if (modes == NULL || spdt_gpio < -1 || spdt_gpio >= GPIO_PIN_MAX ||
        (!standalone && spdt_gpio != -1)) {
        return -EINVAL;
    }
    if (spdt_gpio >= 0 && bflb_device_get_by_name("gpio") == NULL) {
        return -ENODEV;
    }
    /* Cross-path operation on standalone wiring is not qualified yet. */
    *modes = standalone ? BOARD_RF_MODE_STANDALONE_BIT : BOARD_RF_MODE_COMBO_BIT;
    return 0;
}

static int board_rf_initialize(bool standalone, int spdt_gpio)
{
    uintptr_t flags;
    uint32_t modes;
    int ret = board_rf_modes_get(standalone, spdt_gpio, &modes);

    if (ret != 0) {
        return ret;
    }

    flags = bflb_irq_save();
    if (board_rf_init_result != -EAGAIN || rfparam_cfg_get() != NULL) {
        bflb_irq_restore(flags);
        return -EBUSY;
    }
    board_rf_init_result = -EBUSY;
    bflb_irq_restore(flags);

    if (spdt_gpio >= 0) {
        board_rf_spdt_gpio = spdt_gpio;
        ret = board_rf_spdt_fixed(spdt_gpio, false);
        if (ret != 0) {
            goto done;
        }
    }

    board_rf_phyrf_cfg_get();
    cmd_set_btble_combo(0, NULL);
    ret = rfparam_init(0, NULL, 0);
    if (ret != 0) {
        ret = -EIO;
        goto done;
    }

    if (standalone) {
        if (spdt_gpio >= 0) {
            ret = board_rf_spdt_fixed(spdt_gpio, true);
            if (ret != 0) {
                goto done;
            }
        }
        cmd_set_btble_standalone(0, NULL);
        if (rfparam_init(0, NULL, 0) != 0) {
            ret = -EIO;
        }
    } else {
        cmd_set_btble_combo(0, NULL);
    }

done:
    flags = bflb_irq_save();
    board_rf_init_result = ret;
    bflb_irq_restore(flags);
    return ret;
}

int board_rf_combo_init(void)
{
    return board_rf_initialize(false, -1);
}

int board_rf_single_ant_init(void)
{
    return board_rf_combo_init();
}

int board_rf_single_ant_spdt_init(int spdt_gpio)
{
    if (spdt_gpio < 0) {
        return -EINVAL;
    }
    return board_rf_initialize(true, spdt_gpio);
}

void board_rf_single_ant_spdt_force_bt_init(int pin_bt_path, int pin_2g_path)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    if (pin_bt_path >= 0) {
        bflb_gpio_init(gpio, pin_bt_path, GPIO_FUNC_GPIO | GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
        bflb_gpio_reset(gpio, pin_bt_path);
    }
    if (pin_2g_path >= 0) {
        bflb_gpio_init(gpio, pin_2g_path, GPIO_FUNC_GPIO | GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
        bflb_gpio_set(gpio, pin_2g_path);
    }

    board_rf_phyrf_cfg_get();
    cmd_set_btble_combo(0, NULL);
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return;
    }
  
    if (pin_bt_path >= 0) {
        bflb_gpio_set(gpio, pin_bt_path);
    }
    if (pin_2g_path >= 0) {
        bflb_gpio_reset(gpio, pin_2g_path);
    }
    cmd_set_btble_standalone(0, NULL);
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return;
    }
}

void board_rf_single_ant_spdt_force_2g_init(int pin_bt_path, int pin_2g_path)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    if (pin_bt_path >= 0) {
        bflb_gpio_init(gpio, pin_bt_path, GPIO_FUNC_GPIO | GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
        bflb_gpio_reset(gpio, pin_bt_path);
    }
    if (pin_2g_path >= 0) {
        bflb_gpio_init(gpio, pin_2g_path, GPIO_FUNC_GPIO | GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_0);
        bflb_gpio_set(gpio, pin_2g_path);
    }

    board_rf_phyrf_cfg_get();
    cmd_set_btble_combo(0, NULL);
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return;
    }
    cmd_set_btble_combo(0, NULL);
}

int board_rf_dual_ant_init(void)
{
    return board_rf_initialize(true, -1);
}

#endif
