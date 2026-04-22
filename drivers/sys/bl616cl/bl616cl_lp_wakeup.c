#include "bl_lp_internal.h"

static ATTR_NOCACHE_RAM_SECTION lp_fw_gpio_cfg_t *gp_lp_io_cfg = NULL;

int bl_lp_io_wakeup_cfg(void *io_wakeup_cfg)
{
    gp_lp_io_cfg = (lp_fw_gpio_cfg_t *)io_wakeup_cfg;
    return 0;
}

void bl_lp_io_wakeup_init(lp_fw_gpio_cfg_t *cfg)
{
    int ret;

    ret = pm_lowpower_gpio_cfg((lp_gpio_cfg_type *)cfg);
    if (ret) {
        BL_LP_LOG("[LP] io wakeup init fail!\r\n");
    } else {
        BL_LP_LOG("[LP] io wakeup init succeed!\r\n");
    }
}

void bl616cl_lp_io_wakeup_prepare(void)
{
    lp_fw_gpio_cfg_t *cfg = gp_lp_io_cfg;

    if (cfg) {
        iot2lp_para->wakeup_source_parameter->io_wakeup_parameter = (void *)cfg;
        BL_LP_LOG("io_wakeup_unmask: 0x%llX\r\n",
                  (unsigned long long)((lp_fw_gpio_cfg_t *)iot2lp_para->wakeup_source_parameter->io_wakeup_parameter)
                      ->io_wakeup_unmask);
        bl_lp_io_wakeup_init((lp_fw_gpio_cfg_t *)iot2lp_para->wakeup_source_parameter->io_wakeup_parameter);
    }
}

int bl_lp_wakeup_io_get_mode(uint8_t io_num)
{
    lp_fw_gpio_cfg_t *b_lp_io_cfg_bak = NULL;
    uint64_t wakeup_io_bits = iot2lp_para->wakeup_reason_info->wakeup_io_bits;
    uint8_t trig_mode;

    b_lp_io_cfg_bak = (lp_fw_gpio_cfg_t *)iot2lp_para->wakeup_source_parameter->io_wakeup_parameter;

    if (io_num >= BL_LP_WAKEUP_IO_MAX_NUM) {
        return -1;
    }

    if ((wakeup_io_bits & ((uint64_t)0x1 << io_num)) == 0) {
        return 0;
    }

    trig_mode = b_lp_io_cfg_bak->io_0_36_trig_mode[io_num];

    if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_FALLING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_FALLING;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_RISING_EDGE || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_RISING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_RISING;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_LOW_LEVEL || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_LOW_LEVEL) {
            return BL_LP_IO_WAKEUP_MODE_LOW;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_HIGH_LEVEL) {
        return BL_LP_IO_WAKEUP_MODE_HIGH;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_RISING_FALLING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_RISING_FALLING;
    } else {
        return -1;
    }
}
