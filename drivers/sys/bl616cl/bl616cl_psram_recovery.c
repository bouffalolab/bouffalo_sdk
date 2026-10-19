#include "bl616cl_psram_recovery.h"
#include "bflb_efuse.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_ef_ctrl.h"
#include "bl616cl_psram.h"
#include "bl616cl_glb.h"
#include "bl616cl_pm.h"
#include "bl616cl_tzc_sec.h"
#include "bl616cl_lp.h"
#include <stdio.h>

static ATTR_NOCACHE_RAM_SECTION bl_lp_psram_cfg_t g_psram_para = { 0 };

/* Restore PSRAM by using the provided recovery configuration. */
static int ATTR_TCM_SECTION psram_fast_recovery_init(const bl_lp_psram_cfg_t *config)
{
    uint16_t reg_read = 0;
    uint64_t verify_start;

    PSRAM_Ctrl_Cfg_Type psram_ctrl_cfg = {
        .vendor = PSRAM_CTRL_VENDOR_WINBOND,
        .ioMode = PSRAM_CTRL_X8_MODE,
        .size = config->size_enum,
        .dqs_delay = config->dqs_delay,
    };

    PSRAM_Winbond_Cfg_Type winbond_cfg = {
        .rst = DISABLE,
        .clockType = PSRAM_CLOCK_DIFF,
        .inputPowerDownMode = DISABLE,
        .hybridSleepMode = DISABLE,
        .linear_dis = ENABLE,
        .PASR = PSRAM_PARTIAL_REFRESH_FULL,
        .disDeepPowerDownMode = ENABLE,
        .fixedLatency = DISABLE,
        .burstLen = PSRAM_WINBOND_BURST_LENGTH_64_BYTES,
        .burstType = PSRAM_WRAPPED_BURST,
        .latency = PSRAM_WINBOND_6_CLOCKS_LATENCY,
        .driveStrength = config->drive_strength,
    };

    /* Single init cycle with the saved PSRAM parameters. */
    PSram_Ctrl_Init(PSRAM0_ID, &psram_ctrl_cfg);

    PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR0, &winbond_cfg);
    verify_start = bflb_mtimer_get_time_us();

    do {
        PSram_Ctrl_Winbond_Read_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_ID0, &reg_read);
        if (reg_read == config->psram_id) {
            break;
        }
        if ((bflb_mtimer_get_time_us() - verify_start) > 300) {
            return -1;
        }
        bflb_mtimer_delay_us(10);
    } while (1);

    return 0;
}

/* PSRAM init with DQS value using standard API */
static uint16_t ATTR_TCM_SECTION psram_winbond_init_dqs(PSRAM_Winbond_Burst_Length burst_len, uint8_t half_rate_mode,
                                                        PSRAM_Latency_Winbond_Type latency, uint16_t dqs_delay)
{
    uint16_t reg_read = 0;

    PSRAM_Ctrl_Cfg_Type default_psram_ctrl_cfg = {
        .vendor = PSRAM_CTRL_VENDOR_WINBOND,
        .ioMode = PSRAM_CTRL_X8_MODE,
        .size = PSRAM_SIZE_4MB,
        .dqs_delay = dqs_delay,
    };

    PSRAM_Winbond_Cfg_Type default_winbond_cfg = {
        .rst = DISABLE,
        .clockType = PSRAM_CLOCK_DIFF,
        .inputPowerDownMode = DISABLE,
        .hybridSleepMode = DISABLE,
        .linear_dis = ENABLE,
        .PASR = PSRAM_PARTIAL_REFRESH_FULL,
        .disDeepPowerDownMode = ENABLE,
        .fixedLatency = DISABLE,
        .burstLen = burst_len,
        .burstType = PSRAM_WRAPPED_BURST,
        .latency = latency,
        .driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M,
    };

    PSram_Ctrl_Init(PSRAM0_ID, &default_psram_ctrl_cfg);
    PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR0, &default_winbond_cfg);
    PSram_Ctrl_Winbond_Read_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_ID0, &reg_read);

    if (PSRAM_ID_WINBOND_4MB == reg_read) {
        default_psram_ctrl_cfg.size = PSRAM_SIZE_4MB;
        default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M;
    } else if (PSRAM_ID_WINBOND_8MB == reg_read) {
        default_psram_ctrl_cfg.size = PSRAM_SIZE_8MB;
        default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_25_OHMS_FOR_8M;
    } else if (PSRAM_ID_WINBOND_16MB == reg_read) {
        default_psram_ctrl_cfg.size = PSRAM_SIZE_16MB;
        default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_25_OHMS_FOR_16M;
    } else if (PSRAM_ID_WINBOND_32MB == reg_read) {
        default_psram_ctrl_cfg.size = PSRAM_SIZE_32MB;
        default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_34_OHMS_FOR_32M;
    } else {
        return reg_read;
    }
    /* init again */
    PSram_Ctrl_Init(PSRAM0_ID, &default_psram_ctrl_cfg);
    PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR0, &default_winbond_cfg);
    PSram_Ctrl_Winbond_Read_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_ID0, &reg_read);

    return reg_read;
}

/* Build PSRAM recovery parameters from efuse trim or an ID-based DQS scan. */
static int ATTR_TCM_SECTION psram_config_get(bl_lp_psram_cfg_t *config)
{
    bflb_ef_ctrl_com_trim_t trim;
    uint16_t dqs_val[] = {
        0x8000, 0xC000, 0xE000, 0xF000, 0xF800, 0xFC00, 0xFE00, 0xFF00,
        0xFF80, 0xFFC0, 0xFFE0, 0xFFF0, 0xFFF8, 0xFFFC, 0xFFFE, 0xFFFF,
    };
    int32_t left_flag = 0, right_flag = 0, c_val = 0;
    int32_t dqs_win_min = 16, dqs_win_max = 0;
    uint16_t psram_id_value = 0;
    uint32_t temp_min = 16;
    uint32_t temp_len = 0;
    uint32_t dqs_win_max_len = 0;

    bflb_ef_ctrl_read_common_trim(NULL, "psram_trim", &trim, 1);

    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            c_val = ((((trim.value & 0xf0) >> 4) + (trim.value & 0x0f)) >> 1);
        } else {
            printf("\r\nPSRAM trim is corrupted\r\n");
            return -1;
        }
    } else {
        printf("\r\n!!!!!!PSRAM INIT WITHOUT PSRAM TRIM!!!!!!!!!!!!!!!!!!\r\nDo PSRAM Trim\r\n");
        for (uint32_t dqs_index = 0; dqs_index < 16; dqs_index++) {
            psram_id_value = psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0,
                                                    PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val[dqs_index]);
            if ((psram_id_value == PSRAM_ID_WINBOND_4MB) || (psram_id_value == PSRAM_ID_WINBOND_8MB) ||
                (psram_id_value == PSRAM_ID_WINBOND_16MB) || (psram_id_value == PSRAM_ID_WINBOND_32MB)) {
                temp_len++;
                if (temp_len == 1) {
                    temp_min = dqs_index;
                }
                if (temp_len > dqs_win_max_len) {
                    dqs_win_min = temp_min;
                    dqs_win_max = dqs_index;
                    dqs_win_max_len = temp_len;
                }
                printf("Y ");
            } else {
                temp_len = 0;
                printf("* ");
            }
        }

        left_flag = dqs_win_min;
        right_flag = dqs_win_max;
        c_val = ((left_flag + right_flag) >> 1);
        printf("\r\nwindow: 0x%02x ~ 0x%02x; c_val: 0x%02x; dqs:0x%04x; code num:%d\r\n", left_flag, right_flag, c_val,
               dqs_val[c_val], (right_flag - left_flag));
        if (((right_flag - left_flag) <= 1) || ((right_flag - left_flag) > 0xf)) {
            return -1;
        }
    }

    psram_id_value =
        psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0, PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val[c_val]);

    if ((psram_id_value != PSRAM_ID_WINBOND_4MB) && (psram_id_value != PSRAM_ID_WINBOND_8MB) &&
        (psram_id_value != PSRAM_ID_WINBOND_16MB) && (psram_id_value != PSRAM_ID_WINBOND_32MB)) {
        return -1;
    }

    /* Fill config output */
    config->psram_id = psram_id_value;
    config->c_val = c_val;
    config->dqs_delay = dqs_val[c_val];

    switch (psram_id_value) {
        case PSRAM_ID_WINBOND_4MB:
            config->size_enum = PSRAM_SIZE_4MB;
            config->drive_strength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M;
            break;
        case PSRAM_ID_WINBOND_8MB:
            config->size_enum = PSRAM_SIZE_8MB;
            config->drive_strength = PSRAM_WINBOND_DRIVE_STRENGTH_25_OHMS_FOR_8M;
            break;
        case PSRAM_ID_WINBOND_16MB:
            config->size_enum = PSRAM_SIZE_16MB;
            config->drive_strength = PSRAM_WINBOND_DRIVE_STRENGTH_25_OHMS_FOR_16M;
            break;
        case PSRAM_ID_WINBOND_32MB:
            config->size_enum = PSRAM_SIZE_32MB;
            config->drive_strength = PSRAM_WINBOND_DRIVE_STRENGTH_34_OHMS_FOR_32M;
            break;
        default:
            return -1;
    }

    return 0;
}

static int bl_lp_psram_config_get(bl_lp_psram_cfg_t *config)
{
    bflb_efuse_device_info_type device_info;

    bflb_efuse_get_device_info(&device_info);
    if (device_info.psram_info == 0) {
        return -1;
    }

    return psram_config_get(config);
}

void bl_lp_psram_para_save(void)
{
    if (-1 == bl_lp_psram_config_get(&g_psram_para)) {
        printf("psram_config_get failed, psram may not work properly after wakeup\r\n");
        g_psram_para.psram_id = 0;
    }

    /* Share the saved recovery parameters with the LP wakeup path. */
    iot2lp_para->psram_parameter = &g_psram_para;
}

/* Recover PSRAM after LP wakeup with the provided shared parameters. */
uint32_t ATTR_TCM_SECTION bl_lp_psram_recovery_with_config(const bl_lp_psram_cfg_t *config)
{
    uint32_t ret = 0;

    GLB_Set_PSRAMB_CLK_Sel(ENABLE, GLB_PSRAMB_EMI_WIFIPLL_320M, 0);

    /* gpio init */
    struct bflb_device_s *gpio;
    gpio = bflb_device_get_by_name("gpio");
    for (uint8_t i = 0; i < 12; i++) {
        bflb_gpio_init(gpio, (46 + i), GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);
    }

    Tzc_Sec_PSRAMB_Access_Release();

    if (config != NULL) {
        /* Restore PSRAM by using the saved recovery parameters. */
        ret = psram_fast_recovery_init(config);
    } else {
        return -1;
    }

    return ret;
}

int bl_lp_psram_resume(uint32_t pds_level)
{
    int ret = 0;

    if (pds_level == PM_PDS_LEVEL_15) {
        ret = bl_lp_psram_recovery_with_config(iot2lp_para->psram_parameter);
    } else if (BL616CL_PSRAM_INIT_DONE) {
        PSram_Ctrl_Winbond_Exit_Hybrid_Sleep(PSRAM0_ID);
    }

    return ret;
}

uint32_t ATTR_TCM_SECTION bl_lp_psram_recovery(void)
{
    return bl_lp_psram_recovery_with_config(iot2lp_para->psram_parameter);
}
