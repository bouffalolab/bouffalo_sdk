/**
 * *****************************************************************************
 * @file bflb_port_boot2.c
 * @version 0.1
 * @date 2022-11-23
 * @brief
 * *****************************************************************************
 * @attention
 *
 *  <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *    3. Neither the name of Bouffalo Lab nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * *****************************************************************************
 */
#include "bflb_port_boot2.h"
#include "bflb_efuse.h"
#include "bflb_ef_ctrl.h"
#include "bl616cl_hbn.h"
#include "bl616cl_glb.h"
#include "bl616cl_tzc_sec.h"

#include "bflb_clock.h"
#include "bl616cl_psram.h"
#include "bflb_flash.h"
#include "bflb_flash_secreg.h"
#include "bflb_sp_common.h"
#include "bflb_sp_port.h"

#define HAL_BOOT2_TLVC_CRC32_SIZE          4U
#define HAL_BOOT2_TLVC_MIN_SIZE            (sizeof(struct boot2_tlvc_hdr_t) + HAL_BOOT2_TLVC_CRC32_SIZE)
#define HAL_BOOT2_TLVC_TOTAL_SIZE(vlen)    (sizeof(struct boot2_tlvc_hdr_t) + (uint32_t)(vlen) + HAL_BOOT2_TLVC_CRC32_SIZE)
#define HAL_BOOT2_FLASH_OTP_PK_HASH_REGION 0U

static uint8_t g_boot2_runtime_sign_cfg_loaded = 0;
extern uint8_t __etext_final__[];

/****************************************************************************/ /**
 * @brief  read one little-endian 16-bit value from byte stream
 *
 * @param  data: input byte pointer
 *
 * @return decoded 16-bit value
 *
 *******************************************************************************/
static uint16_t hal_boot2_get_le16(const uint8_t *data)
{
    return ((uint16_t)data[0]) | ((uint16_t)data[1] << 8);
}

/****************************************************************************/ /**
 * @brief  read one little-endian 32-bit value from byte stream
 *
 * @param  data: input byte pointer
 *
 * @return decoded 32-bit value
 *
 *******************************************************************************/
static uint32_t hal_boot2_get_le32(const uint8_t *data)
{
    return ((uint32_t)data[0]) | ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

/****************************************************************************/ /**
 * @brief  calculate crc32 of one boot2 tlvc record
 *
 * @param  tlvc_data: TLVC item header pointer
 * @param  value_len: TLVC value length
 *
 * @return crc32 value
 *
 *******************************************************************************/
static uint32_t hal_boot2_calc_tlvc_crc(const uint8_t *tlvc_data, uint16_t value_len)
{
    return bflb_soft_crc32((uint8_t *)tlvc_data, sizeof(struct boot2_tlvc_hdr_t) + value_len);
}

/****************************************************************************/ /**
 * @brief  read crc32 value from one boot2 tlvc record
 *
 * @param  crc_ptr: TLVC crc32 address
 *
 * @return crc32 value
 *
 *******************************************************************************/
static uint32_t hal_boot2_get_tlvc_crc32(const uint8_t *crc_ptr)
{
    return hal_boot2_get_le32(crc_ptr);
}

/****************************************************************************/ /**
 * @brief  decode one boot2 tlvc record into parsed fields
 *
 * @param  tlvc_data: TLVC item header pointer
 * @param  tlvc_item: decoded TLVC item output
 *
 * @return none
 *
 *******************************************************************************/
static void hal_boot2_get_tlvc_item(const uint8_t *tlvc_data, boot2_tlvc_item_t *tlvc_item)
{
    tlvc_item->magic = hal_boot2_get_le32(tlvc_data);
    tlvc_item->type = hal_boot2_get_le16(tlvc_data + sizeof(uint32_t));
    tlvc_item->length = hal_boot2_get_le16(tlvc_data + sizeof(uint32_t) + sizeof(uint16_t));
    tlvc_item->value = tlvc_data + sizeof(struct boot2_tlvc_hdr_t);
    tlvc_item->crc32 = 0;
}

/****************************************************************************/ /**
 * @brief  parse one tlvc record and validate it inside the specified tlvc area
 *
 * @param  tlvc: TLVC record pointer
 * @param  tlvc_area_end: End address of the TLVC area
 * @param  tlvc_item: decoded TLVC item output
 * @param  tlvc_total_len: TLVC total length output
 *
 * @return 0 for valid and negative value for invalid
 *
 *******************************************************************************/
static int32_t hal_boot2_parse_tlvc_record(const uint8_t *tlvc, const uint8_t *tlvc_area_end,
                                           boot2_tlvc_item_t *tlvc_item, uint32_t *tlvc_total_len)
{
    if (((uintptr_t)tlvc_area_end - (uintptr_t)tlvc) < HAL_BOOT2_TLVC_MIN_SIZE) {
        return -1;
    }

    hal_boot2_get_tlvc_item(tlvc, tlvc_item);
    if (tlvc_item->magic != HAL_BOOT2_TLVC_MAGIC) {
        return -2;
    }

    *tlvc_total_len = HAL_BOOT2_TLVC_TOTAL_SIZE(tlvc_item->length);
    if (((uintptr_t)tlvc_area_end - (uintptr_t)tlvc) < *tlvc_total_len) {
        return -3;
    }

    tlvc_item->crc32 = hal_boot2_get_tlvc_crc32(tlvc + *tlvc_total_len - HAL_BOOT2_TLVC_CRC32_SIZE);
    if (hal_boot2_calc_tlvc_crc(tlvc, tlvc_item->length) != tlvc_item->crc32) {
        return -4;
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  clear one app public key hash slot
 *
 * @param  efuse_cfg: boot2 efuse config pointer
 * @param  group: cpu group index
 *
 * @return none
 *
 *******************************************************************************/
static void hal_boot2_clear_app_pkhash(boot2_efuse_hw_config *efuse_cfg, uint32_t group)
{
    memset(efuse_cfg->pk_hash_cpu[group], 0, sizeof(efuse_cfg->pk_hash_cpu[group]));
}

/****************************************************************************/ /**
 * @brief  get TLVC type and default app public key hash length from sign type
 *
 * @param  efuse_cfg: boot2 efuse config pointer
 * @param  group: cpu group index
 * @param  tlvc_type: output TLVC type, NULL if unused
 * @param  hash_len: output hash length, NULL if unused
 *
 * @return 0 on success, negative value on failure
 *
 *******************************************************************************/
static int32_t hal_boot2_get_app_pkhash_tlvc_info(const boot2_efuse_hw_config *efuse_cfg, uint32_t group,
                                                  uint16_t *tlvc_type, uint32_t *hash_len)
{
    if (efuse_cfg->sign[group] == HAL_BOOT_SIGN_TYPE_ECC_SHA256) {
        if (tlvc_type != NULL) {
            *tlvc_type = HAL_BOOT2_TLVC_TYPE_PKHA_SHA256;
        }
        if (hash_len != NULL) {
            *hash_len = HAL_BOOT2_PK_HASH_SIZE;
        }
        return 0;
    }
#if HAL_BOOT2_SUPPORT_SIGN_SHA384
    if (efuse_cfg->sign[group] == HAL_BOOT_SIGN_TYPE_ECC_SHA384) {
        if (tlvc_type != NULL) {
            *tlvc_type = HAL_BOOT2_TLVC_TYPE_PKHA_SHA384;
        }
        if (hash_len != NULL) {
            *hash_len = HAL_BOOT2_PK_HASH_SIZE_SHA384;
        }
        return 0;
    }
#endif

    return -3;
}

/****************************************************************************/ /**
 * @brief  parse one matched tlvc record and load app public key hash
 *
 * @param  tlvc: TLVC record pointer
 * @param  efuse_cfg: boot2 efuse config pointer
 * @param  group: cpu group index
 *
 * @return 0 on success, negative value on failure
 *
 *******************************************************************************/
static int32_t hal_boot2_parse_app_pkhash_tlvc(const uint8_t *tlvc, boot2_efuse_hw_config *efuse_cfg, uint32_t group)
{
    boot2_tlvc_item_t tlvc_item;
    uint32_t hash_idx = 0;
    uint32_t hash_len = 0;
    uint16_t expected_type = 0;
    int32_t ret = 0;

    hal_boot2_get_tlvc_item(tlvc, &tlvc_item);
    ret = hal_boot2_get_app_pkhash_tlvc_info(efuse_cfg, group, &expected_type, &hash_len);
    if (ret != 0) {
        return ret;
    }

    if (tlvc_item.type != expected_type) {
        return -4;
    }

    if ((uint32_t)tlvc_item.length != hash_len) {
        return -5;
    }

    hal_boot2_clear_app_pkhash(efuse_cfg, group);
    for (hash_idx = 0; hash_idx < hash_len; hash_idx++) {
        efuse_cfg->pk_hash_cpu[group][hash_idx] = tlvc_item.value[hash_idx];
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  load app public key hash from flash otp region0
 *
 * @param  efuse_cfg: boot2 efuse config pointer
 * @param  group: cpu group index
 *
 * @return 0 on success, negative value on failure
 *
 *******************************************************************************/
static int32_t hal_boot2_load_app_pkhash_from_flash_otp(boot2_efuse_hw_config *efuse_cfg, uint32_t group)
{
    const bflb_flash_secreg_param_t *param = NULL;
    bflb_flash_otp_config_t flash_otp_cfg;
    uint8_t *flash_cfg = NULL;
    uint32_t flash_cfg_len = 0;
    uint32_t jedec_id = 0;
    uint32_t hash_len = 0;
    uint16_t tlvc_type = 0;
    int32_t ret = 0;

    ret = hal_boot2_get_app_pkhash_tlvc_info(efuse_cfg, group, &tlvc_type, &hash_len);
    if (ret != 0) {
        return ret;
    }
    (void)tlvc_type;

    bflb_flash_get_cfg(&flash_cfg, &flash_cfg_len);
    if (flash_cfg == NULL) {
        return -6;
    }
    (void)flash_cfg_len;

    jedec_id = bflb_flash_get_jedec_id();
    ret = bflb_flash_secreg_get_param(jedec_id, &param);
    if ((ret != 0) || (param == NULL)) {
        return -7;
    }

    flash_otp_cfg.flash_cfg = (spi_flash_cfg_type *)flash_cfg;
    flash_otp_cfg.param = param;

    hal_boot2_clear_app_pkhash(efuse_cfg, group);
    ret = bflb_flash_secreg_read_by_idx(&flash_otp_cfg, HAL_BOOT2_FLASH_OTP_PK_HASH_REGION, 0,
                                        efuse_cfg->pk_hash_cpu[group], hash_len);
    if (ret != 0) {
        return -8;
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  load app public key hash from boot2 tail tlvc records
 *
 * @param  efuse_cfg: boot2 efuse config pointer
 * @param  group: cpu group index
 *
 * @return 0 on success, negative value on failure
 *
 *******************************************************************************/
static int32_t hal_boot2_load_app_pkhash_from_tlvc(boot2_efuse_hw_config *efuse_cfg, uint32_t group)
{
    struct hal_bootheader_t boot2_hdr;
    uint32_t boot2_start_addr = HAL_BOOT2_FLASH_XIP_BASE + hal_boot2_get_bootheader_offset();
    const uint8_t *tlvc_start_raw = __etext_final__;
    const uint8_t *tlvc_start = NULL;
    const uint8_t *tlvc_end = NULL;
    const uint8_t *cursor = NULL;
    uint32_t boot2_img_len = 0;
    uint16_t expected_type = 0;
    uint32_t hash_len = 0;
    int32_t ret = -9;

    if (bflb_flash_read(hal_boot2_get_bootheader_offset(), (uint8_t *)&boot2_hdr, sizeof(boot2_hdr)) != 0) {
        return ret;
    }

    boot2_img_len = boot2_hdr.basic_cfg.img_len_cnt;
    if (boot2_img_len == 0) {
        return ret;
    }

    tlvc_start = (const uint8_t *)((((uintptr_t)tlvc_start_raw) + 0x0FU) & (~((uintptr_t)0x0FU)));
    tlvc_end = (const uint8_t *)(boot2_start_addr + HAL_BOOT2_FW_IMG_OFFSET_AFTER_HEADER + boot2_img_len);
    if ((uintptr_t)tlvc_end <= (uintptr_t)tlvc_start) {
        return ret;
    }

    ret = hal_boot2_get_app_pkhash_tlvc_info(efuse_cfg, group, &expected_type, &hash_len);
    if (ret != 0) {
        return ret;
    }

    cursor = tlvc_start;
    while (((uintptr_t)tlvc_end - (uintptr_t)cursor) >= HAL_BOOT2_TLVC_MIN_SIZE) {
        boot2_tlvc_item_t tlvc_item;
        uint32_t tlvc_total_len = 0;

        ret = hal_boot2_parse_tlvc_record(cursor, tlvc_end, &tlvc_item, &tlvc_total_len);
        if (ret != 0) {
            return -10;
        }

        if ((tlvc_item.type == expected_type) && ((uint32_t)tlvc_item.length == hash_len)) {
            return hal_boot2_parse_app_pkhash_tlvc(cursor, efuse_cfg, group);
        }

        cursor += tlvc_total_len;
    }

    return -11;
}

/****************************************************************************/ /**
 * @brief  load individual app sign config after flash init
 *
 * @param  efuse_cfg: boot2 efuse config pointer
 *
 * @return 0 on success, negative value on failure
 *
 *******************************************************************************/
int32_t hal_boot2_load_individual_sign_cfg(boot2_efuse_hw_config *efuse_cfg)
{
    uint32_t i = 0;
    int32_t ret = 0;

    if (efuse_cfg->app_sign_type != HAL_APP_SIGN_INDIVIDUAL) {
        return 0;
    }

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        ret = hal_boot2_load_app_pkhash_from_tlvc(efuse_cfg, i);
        if (ret != 0) {
            return ret;
        }
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  load same-as-boot2 sign config from flash otp after flash init
 *
 * @param  efuse_cfg: boot2 efuse config pointer
 *
 * @return 0 on success, negative value on failure
 *
 *******************************************************************************/
int32_t hal_boot2_load_sign_cfg_from_flash_otp(boot2_efuse_hw_config *efuse_cfg)
{
    uint32_t i = 0;
    int32_t ret = 0;

    if (efuse_cfg->app_sign_type != HAL_APP_SIGN_SAME_AS_BOOT2) {
        return 0;
    }

    if (efuse_cfg->pkhash_sel != HAL_APP_SIGN_PKHASH_FROM_FLASH_OTP) {
        return 0;
    }

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        if (efuse_cfg->sign[i] == HAL_BOOT_SIGN_TYPE_NONE) {
            continue;
        }

        ret = hal_boot2_load_app_pkhash_from_flash_otp(efuse_cfg, i);
        if (ret != 0) {
            return ret;
        }
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  load runtime sign config after flash init
 *
 * @param  efuse_cfg: boot2 efuse config pointer
 *
 * @return 0 on success, negative value on failure
 *
 *******************************************************************************/
int32_t hal_boot2_load_runtime_sign_cfg(boot2_efuse_hw_config *efuse_cfg)
{
    int32_t ret = 0;

    if (g_boot2_runtime_sign_cfg_loaded != 0) {
        return 0;
    }

    ret = hal_boot2_load_individual_sign_cfg(efuse_cfg);
    if (ret != 0) {
        return ret;
    }

    if (efuse_cfg->pkhash_sel == HAL_APP_SIGN_PKHASH_FROM_FLASH_OTP) {
        ret = hal_boot2_load_sign_cfg_from_flash_otp(efuse_cfg);
        if (ret != 0) {
            return ret;
        }
    }

    g_boot2_runtime_sign_cfg_loaded = 1;
    return 0;
}
/****************************************************************************/ /**
 * @brief  init boot2 system clock
 *
 * @param
 *
 * @return
 *
*******************************************************************************/
void hal_boot2_init_clock(void)
{
#if 0
    CPU_Reset_MTimer();
    /* set mtimer clock 1M */
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_CLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK) / 1000000 - 1);
    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_XCLK, 0);
    PERIPHERAL_CLOCK_TIMER0_1_WDG_ENABLE();
#endif

    /* wifipll */
    GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL);
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M);
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) / 1000000 - 1);

    // PERIPHERAL_CLOCK_USB_ENABLE();
    // GLB_Set_USB_CLK_From_WIFIPLL(1);
}

/****************************************************************************/ /**
 * @brief  init psram gpio
 *
 * @param
 *
 * @return
 *
*******************************************************************************/
static void _bflb_init_psram_gpio(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name(BFLB_NAME_GPIO);
    for (uint8_t i = 0; i < 12; i++) {
        bflb_gpio_init(gpio, (46 + i), GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);
    }
}

/****************************************************************************/ /**
 * @brief  psram_winbond_init_dqs
 *
 * @param
 *
 * @return
 *
*******************************************************************************/
static uint16_t psram_winbond_init_dqs(int8_t burst_len, uint8_t is_fix_latency, uint8_t latency, uint16_t dqs_delay)
{
    uint16_t reg_read = 0;
    PSRAM_Ctrl_Cfg_Type default_psram_ctrl_cfg = {
        .vendor = PSRAM_CTRL_VENDOR_WINBOND,
        .ioMode = PSRAM_CTRL_X8_MODE,
        .size = PSRAM_SIZE_4MB,
        .dqs_delay = 0xfff0,
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
        .burstLen = PSRAM_WINBOND_BURST_LENGTH_64_BYTES,
        .burstType = PSRAM_WRAPPED_BURST,
        .latency = PSRAM_WINBOND_6_CLOCKS_LATENCY,
        .driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M,
    };

    default_winbond_cfg.burstLen = burst_len;
    default_winbond_cfg.fixedLatency = is_fix_latency;
    default_winbond_cfg.latency = latency;

    default_psram_ctrl_cfg.dqs_delay = dqs_delay;
    PSram_Ctrl_Init(PSRAM0_ID, &default_psram_ctrl_cfg);
    // PSram_Ctrl_Winbond_Reset(PSRAM0_ID);
    PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR0, &default_winbond_cfg);

    PSram_Ctrl_Winbond_Read_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_ID0, &reg_read);

    if (HAL_BOOT2_PSRAM_ID1_WINBOND_4MB == reg_read) {
        default_psram_ctrl_cfg.size = PSRAM_SIZE_4MB;
        default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M;
    } else if (HAL_BOOT2_PSRAM_ID4_WINBOND_8MB == reg_read) {
        default_psram_ctrl_cfg.size = PSRAM_SIZE_8MB;
        default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_25_OHMS_FOR_8M;
    } else if (HAL_BOOT2_PSRAM_ID3_WINBOND_16MB == reg_read) {
        default_psram_ctrl_cfg.size = PSRAM_SIZE_16MB;
        default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_25_OHMS_FOR_16M;
    } else if (HAL_BOOT2_PSRAM_ID2_WINBOND_32MB == reg_read) {
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

#define TEST_MEMORY_SIZE (1 * 1024 * 1024)

uint32_t test_pattern_val[] = {
    0xaa55aa55,
    0x33cc33cc,
    0x55aa55aa,
    0xcc33cc33,
};

/****************************************************************************/ /**
 * @brief  hal_boot2_psram_rw_check
 *
 * @param  None
 *
 * @return uint8_t
 *
*******************************************************************************/
uint8_t hal_boot2_psram_rw_check(void)
{
    uint32_t i = 0;
    volatile uint32_t *p = (uint32_t *)BFLB_PSRAM_BASE;
    volatile uint32_t q = 0;

    p[0] = 0xaa55aa55;
    p[1] = 0x33cc33cc;
    p[2] = 0x55aa55aa;
    p[3] = 0xcc33cc33;

    __ISB();

    for (i = 0; i < 4; i++) {
        q = p[i];
        if (q != test_pattern_val[i]) {
            /* MSG("RW ERROR 0x%x != 0x%x\r\n", i, q); */
            return ERROR;
        }
    }

    /* clear PSRAM 1M Byte */
    for (i = 0; i < TEST_MEMORY_SIZE; i++) {
        p[i] = 0xffffffff;
    }

    /* check PSRAM 1M Byte */
    for (i = 0; i < TEST_MEMORY_SIZE; i++) {
        p[i] = i;
    }

    for (i = 0; i < TEST_MEMORY_SIZE; i++) {
        q = p[i];

        if (q != i) {
            /* MSG("RW ERROR 0x%x != 0x%x\r\n", i, q); */
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  x8 psram calibration
 *
 * @param  psram_dqs_win_num
 *
 * @return uint16_t
 *
*******************************************************************************/
static uint16_t hal_boot2_x8_psram_calibration(int32_t *psram_dqs_win_num, uint16_t *psram_id)
{
    int16_t before_ef = 0;
    int32_t left_flag = 0, right_flag = 0, c_val = 0;
    int32_t dqs_win_min = 16, dqs_win_max = 0;
    uint32_t temp_min = 16;
    uint32_t temp_len = 0;
    uint32_t dqs_win_max_len = 0;

    uint16_t dqs_val[] = {
        0x8000,
        0xC000,
        0xE000,
        0xF000,
        0xF800,
        0xFC00,
        0xFE00,
        0xFF00,
        0xFF80,
        0xFFC0,
        0xFFE0,
        0xFFF0,
        0xFFF8,
        0xFFFC,
        0xFFFE,
        0xFFFF,
    };

#ifdef CONFIG_DEBUG
    printf("r ef:0x%08lx\r\n", g_efuse_cfg.psram_dqs_cfg);
#endif
    before_ef = g_efuse_cfg.psram_dqs_cfg;
    if (g_efuse_cfg.psram_dqs_cfg != 0xffff) {
        left_flag = ((g_efuse_cfg.psram_dqs_cfg & (0xf0)) >> 0x4);
        right_flag = (g_efuse_cfg.psram_dqs_cfg & (0xf));
        c_val = ((left_flag + right_flag) >> 0x1);
        *psram_dqs_win_num = right_flag - left_flag;
        BOOT2_MSG_DBG("ef window: 0x%x ~ 0x%x; c_val: 0x%x; dqs:0x%x; code num:%d\r\n", left_flag, right_flag, c_val, dqs_val[c_val], (right_flag - left_flag));
        *psram_id = psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0, PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val[c_val]);
        if ((*psram_id != HAL_BOOT2_PSRAM_ID1_WINBOND_4MB) && (*psram_id != HAL_BOOT2_PSRAM_ID2_WINBOND_32MB) &&
            (*psram_id != HAL_BOOT2_PSRAM_ID3_WINBOND_16MB) && (*psram_id != HAL_BOOT2_PSRAM_ID4_WINBOND_8MB)) {
            return ERROR;
        }
    } else {
        BOOT2_MSG_DBG("do psram trim\r\n");
        for (uint32_t dqs_index = 0; dqs_index < 16; dqs_index++) {
            *psram_id = psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0, PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val[dqs_index]);
            // BOOT2_MSG_DBG("psram id:%04x\r\n", psram_id);
            if ((*psram_id == HAL_BOOT2_PSRAM_ID1_WINBOND_4MB) || (*psram_id == HAL_BOOT2_PSRAM_ID2_WINBOND_32MB) ||
                (*psram_id == HAL_BOOT2_PSRAM_ID3_WINBOND_16MB) || (*psram_id == HAL_BOOT2_PSRAM_ID4_WINBOND_8MB)) {
                if (hal_boot2_psram_rw_check() == SUCCESS) {
                    temp_len++;
                    if (temp_len == 1) { // new
                        temp_min = dqs_index;
                    }
                    if (temp_len > dqs_win_max_len) { // max new
                        dqs_win_min = temp_min;
                        dqs_win_max = dqs_index;
                        dqs_win_max_len = temp_len;
                    }
                    BOOT2_MSG_DBG("Y ");
                } else {
                    temp_len = 0;
                    BOOT2_MSG_DBG("N ");
                }
            } else {
                temp_len = 0;
                BOOT2_MSG_DBG("* ");
            }
        }
        BOOT2_MSG_DBG("\r\ndqs_win_min:%d, dqs_win_max:%d\r\n", dqs_win_min, dqs_win_max);

        left_flag = dqs_win_min;
        right_flag = dqs_win_max;
        c_val = ((left_flag + right_flag) >> 1);
        BOOT2_MSG_DBG("window: 0x%x ~ 0x%x; c_val: 0x%x; dqs:0x%x; code num:%d\r\n", left_flag, right_flag, c_val, dqs_val[c_val], (right_flag - left_flag));

        g_efuse_cfg.psram_dqs_cfg = (((left_flag << 0x4) | (right_flag)) & (0xff));

#ifdef CONFIG_DEBUG
        printf("c ef:0x%08lx\r\n", g_efuse_cfg.psram_dqs_cfg);
        /* printf("window: 0x%02x ~ 0x%02x; c_val: 0x%02x; dqs:0x%08x; code num:%d\r\n", left_flag, right_flag, c_val, dqs_val[c_val], (right_flag - left_flag)); */
#endif
        *psram_dqs_win_num = right_flag - left_flag;
        if (((*psram_dqs_win_num) <= 1) || ((*psram_dqs_win_num) > 0xf)) {
            return ERROR;
        }
        *psram_id = psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0, PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val[c_val]);
        if ((*psram_id != HAL_BOOT2_PSRAM_ID1_WINBOND_4MB) && (*psram_id != HAL_BOOT2_PSRAM_ID2_WINBOND_32MB) &&
            (*psram_id != HAL_BOOT2_PSRAM_ID3_WINBOND_16MB) && (*psram_id != HAL_BOOT2_PSRAM_ID4_WINBOND_8MB)) {
            return ERROR;
        }
        /* to do write efuse psram dqs delay */
        (void)before_ef;
        // if (!(before_ef & 0x1fff)) {
        //     bflb_ef_ctrl_write_common_trim(NULL, "psram_trim", g_efuse_cfg.psram_dqs_cfg, 1);
        // }
    }
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  customer init
 *
 * @param  custom_param
 *
 * @return ret
 *
*******************************************************************************/
uint32_t hal_boot2_custom(void *custom_param)
{
    uint16_t ret = 0;
    int32_t psram_dqs_win_num = 0;
    uint16_t psram_id = 0;
    if ((g_efuse_cfg.dev_info.psram_info) != 0) {
        /* power init */
        AON_LDO18_IO_Switch_Psram(1);
        /* clk init */
        GLB_Set_PSRAMB_CLK_Sel(ENABLE, GLB_PSRAMB_EMI_WIFIPLL_320M, 0);

        /* gpio init */
        _bflb_init_psram_gpio();
        Tzc_Sec_PSRAMB_Access_Release();
        ret = hal_boot2_x8_psram_calibration(&psram_dqs_win_num, &psram_id);

        /* Flush i-cache in case branch prediction logic is wrong when
        psram is not inited by hal_boot2_custom but cpu has already prefetch psram */
        __ISB();
        if (ret == ERROR) {
            BOOT2_MSG("psram error:%d\r\n", (int)psram_dqs_win_num);
        } else {
            BOOT2_MSG("psram suss:%d \r\n", (int)psram_dqs_win_num);
        }
    }

    return psram_id;
}

/****************************************************************************/ /**
 * @brief  get efuse Boot2 config
 *
 * @param  g_efuse_cfg
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_get_efuse_cfg(boot2_efuse_hw_config *efuse_cfg)
{
    uint32_t i = 0;
    uint32_t app_encrypt_sign = 0;
    struct boot_efuse_sw_cfg0_t sw_cfg0;
    struct boot_efuse_sw_cfg1_t sw_cfg1;
    uint32_t tmpval = 0;

    /* get app encrypt and sign type */
    bflb_ef_ctrl_read_direct(NULL, 0x7C, (uint32_t *)&app_encrypt_sign, 1, 1);
    g_boot2_runtime_sign_cfg_loaded = 0;

    efuse_cfg->app_encrypt_type = ((app_encrypt_sign >> 12) & 0x3);
    efuse_cfg->app_sign_type = ((app_encrypt_sign >> 10) & 0x3);

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        switch (efuse_cfg->app_encrypt_type) {
            case HAL_APP_ENCRYPT_SAME_AS_BOOT2:
                /* get hw cfg (signature and aes type) */
                bflb_ef_ctrl_read_direct(NULL, 0x00, &tmpval, 1, 1);
                efuse_cfg->encrypted[i] = tmpval & 0x3;
                if (efuse_cfg->encrypted[i] == EF_CTRL_SF_AES_192) {
                    efuse_cfg->encrypted[i] = SF_CTRL_AES_192BITS + 1;
                } else if (efuse_cfg->encrypted[i] == EF_CTRL_SF_AES_256) {
                    efuse_cfg->encrypted[i] = SF_CTRL_AES_256BITS + 1;
                }
                break;
            case HAL_APP_ENCRYPT_INDIVIDUAL_AES128:
                efuse_cfg->encrypted[i] = SF_CTRL_AES_128BITS + 1;
                break;
            case HAL_APP_NO_ENCRYPT:
                efuse_cfg->encrypted[i] = EF_CTRL_SF_AES_NONE;
                break;
            default:
                BOOT2_MSG_DBG("APP encrypt flag is invalid, deadbeef!\r\n");
                while (1)
                    ;
        }
    }

    /* get sw uasge 0 */
    bflb_ef_ctrl_read_direct(NULL, 0x70, (uint32_t *)&sw_cfg0, 1, 0);
    /* get sw uasge 1 */
    bflb_ef_ctrl_read_direct(NULL, 0x74, (uint32_t *)&sw_cfg1, 1, 0);
    efuse_cfg->pkhash_sel = (uint8_t)sw_cfg1.pkhash_sel;
    efuse_cfg->pkhash_len = (uint8_t)sw_cfg1.pkhash_len;

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        /* get public key hash */
        switch (efuse_cfg->app_sign_type) {
            case HAL_APP_SIGN_SAME_AS_BOOT2:
                hal_boot2_clear_app_pkhash(efuse_cfg, i);
                efuse_cfg->sign[i] = (uint8_t)sw_cfg0.sign_cfg;
                switch (efuse_cfg->pkhash_sel) {
                    case HAL_APP_SIGN_PKHASH_FROM_EFUSE:
                        /* read from efuse directly */
                        if (sw_cfg1.pkhash_len == 0) {
                            /* pkhash len is 192 bits */
                            bflb_ef_ctrl_read_direct(NULL, 0x48, (uint32_t *)efuse_cfg->pk_hash_cpu[i],
                                                     HAL_BOOT2_PK_HASH_SIZE_LEN192 / 4, 0);
                        } else {
                            bflb_ef_ctrl_read_direct(NULL, 0x40, (uint32_t *)efuse_cfg->pk_hash_cpu[i],
                                                     HAL_BOOT2_PK_HASH_SIZE / 4, 0);
                        }
#if HAL_BOOT2_SUPPORT_SIGN_SHA384
                        if (efuse_cfg->sign[i] == HAL_BOOT_SIGN_TYPE_ECC_SHA384) {
                            /* Read additional 16 bytes (4 words) for SHA384 pk hash at offset 32 bytes */
                            bflb_ef_ctrl_read_direct(
                                NULL, 0x1C, (uint32_t *)((uint8_t *)efuse_cfg->pk_hash_cpu[i] + HAL_BOOT2_PK_HASH_SIZE),
                                4, 0);
                        }
#endif
                        break;
                    case HAL_APP_SIGN_PKHASH_FROM_FLASH_OTP:
                        /* defer public key hash loading until flash init is complete */
                        break;
                    default:
                        BOOT2_MSG_DBG("Invalid pkhash_sel: %d\r\n", efuse_cfg->pkhash_sel);
                        while (1)
                            ;
                        break;
                }
                break;
            case HAL_APP_SIGN_INDIVIDUAL:
                hal_boot2_clear_app_pkhash(efuse_cfg, i);
                efuse_cfg->sign[i] = (uint8_t)sw_cfg0.sign_cfg;
                break;
            case HAL_APP_NO_SIGN:
                hal_boot2_clear_app_pkhash(efuse_cfg, i);
                efuse_cfg->sign[i] = HAL_BOOT_SIGN_TYPE_NONE;
                break;
            default:
                BOOT2_MSG_DBG("Invalid app_sign_type: %d\r\n", efuse_cfg->app_sign_type);
                while (1)
                    ;
        }
    }
    for (i = 1; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        efuse_cfg->encrypted[i] = EF_CTRL_SF_AES_NONE;
    }

    efuse_cfg->hbn_check_sign = (uint8_t)(sw_cfg1.hbn_check_sign);
    efuse_cfg->sf_pin_cfg = (uint8_t)(sw_cfg1.sf_pin_cfg);
    efuse_cfg->uart_download_cfg = (uint8_t)(sw_cfg0.uart_download_cfg);
    efuse_cfg->keep_dbg_port_closed = (uint8_t)(sw_cfg1.keep_dbg_port_closed);
    efuse_cfg->boot_pin_cfg = (uint8_t)(sw_cfg0.boot_pin_cfg);

    // /* get psram dqs delay info */
    // EF_Ctrl_Read_Psram_Trim((Efuse_Psram_Trim_Type *)&efuse_cfg->psram_dqs_cfg);
    bflb_ef_ctrl_com_trim_t trim;
    efuse_cfg->psram_dqs_cfg = 0xffff;
    bflb_ef_ctrl_read_common_trim(NULL, "psram_trim", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            efuse_cfg->psram_dqs_cfg = trim.value;
        }
    }

    /* get device info */
    bflb_efuse_get_device_info((bflb_efuse_device_info_type *)&efuse_cfg->dev_info);

    /* get chip id */
    bflb_efuse_get_chipid(efuse_cfg->chip_id);
}

/****************************************************************************/ /**
 * @brief  reset sec eng module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_reset_sec_eng(void)
{
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SEC_ENG);
    GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_MCU_MUXPLL_160M);
}

/****************************************************************************/ /**
 * @brief  system soft reset
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_sw_system_reset(void)
{
    GLB_SW_System_Reset();
}

/****************************************************************************/ /**
 * @brief  set power down sleep status flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_set_psmode_status(uint32_t flag)
{
    HBN_Set_Status_Flag(flag);
}

/****************************************************************************/ /**
 * @brief  get power down sleep status flag
 *
 * @param  None
 *
 * @return flag
 *
*******************************************************************************/
uint32_t hal_boot2_get_psmode_status(void)
{
    return HBN_Get_Status_Flag();
}

/****************************************************************************/ /**
 * @brief  get user fw flag
 *
 * @param  None
 *
 * @return flag
 *
*******************************************************************************/
uint32_t hal_boot2_get_user_fw(void)
{
    return BL_RD_WORD(HBN_BASE + HBN_RSV0_OFFSET);
}

/****************************************************************************/ /**
 * @brief  clear user fw flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_clr_user_fw(void)
{
    uint32_t *p = (uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET);
    *p = 0;
}

/****************************************************************************/ /**
 * @brief  sboot finish
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION hal_boot2_sboot_finish(void)
{
}

/****************************************************************************/ /**
 * @brief  init uart gpio
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_uart_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name(BFLB_NAME_GPIO);
    bflb_gpio_uart_init(gpio, GPIO_PIN_34, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_35, GPIO_UART_FUNC_UART0_RX);
}

/****************************************************************************/ /**
 * @brief  init uart tx gpio
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_debug_uart_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name(BFLB_NAME_GPIO);
    bflb_gpio_uart_init(gpio, GPIO_PIN_34, GPIO_UART_FUNC_UART0_TX);
}

#if HAL_BOOT2_SUPPORT_USB_IAP
/****************************************************************************/ /**
 * @brief  init usb gpio
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_debug_usb_port_init(void)
{
}
#endif

/****************************************************************************/ /**
 * @brief  deinit uart gpio
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_debug_uart_gpio_deinit(void)
{
}

/****************************************************************************/ /**
 * @brief  Check bootheader crc
 *
 * @param  data: bootheader data pointer
 *
 * @return boot_error_code type
 *
*******************************************************************************/
static uint32_t hal_boot_check_bootheader(struct hal_bootheader_t *header)
{
    uint32_t crc_pass = 0;
    uint32_t crc;

    if (header->basic_cfg.crc_ignore == 1 && header->crc32 == HAL_BOOT2_DEADBEEF_VAL) {
        //MSG("Crc ignored\r\n");
        crc_pass = 1;
    } else {
        crc = BFLB_Soft_CRC32((uint8_t *)header, sizeof(struct hal_bootheader_t) - sizeof(header->crc32));

        if (header->crc32 == crc) {
            crc_pass = 1;
        }
    }
    return crc_pass;
}

/****************************************************************************/ /**
 * @brief  Check if the input public key is the same as  burned in the efuse
 *
 * @param  boot_img_cfg: Boot image config pointer
 * @param  data: Image data pointer
 *
 * @return boot_error_code type
 *
*******************************************************************************/
int32_t hal_boot_parse_bootheader(boot2_image_config *boot_img_cfg, uint8_t *data)
{
    struct hal_bootheader_t *header = (struct hal_bootheader_t *)data;
    uint32_t crc_pass = 0;
    uint32_t i = 0;
    uint32_t *phash = (uint32_t *)header->basic_cfg.hash;

    crc_pass = hal_boot_check_bootheader(header);

    if (!crc_pass) {
        // BOOT2_MSG_ERR("bootheader crc error\r\n");
        //bflb_sp_dump_data((uint8_t *)&crc, 4);
        return 0x0204;
    }

    if (header->basic_cfg.notload_in_bootrom) {
        return 0x0202;
    }

    /* Get which CPU's img it is*/
    for (i = 0; i < HAL_BOOT2_CPU_MAX; i++) {
        if (0 == memcmp((void *)&header->magiccode, HAL_BOOT2_CPU0_MAGIC,
                        sizeof(header->magiccode))) {
            break;
        } else if (0 == memcmp((void *)&header->magiccode, HAL_BOOT2_CPU1_MAGIC,
                               sizeof(header->magiccode))) {
            break;
        }
    }

    if (i == HAL_BOOT2_CPU_MAX) {
        /* No cpu img magic match */
        // BOOT2_MSG_ERR("Magic code error\r\n");
        return 0x0203;
    }

    if (boot_img_cfg == NULL) {
        return 0;
    }

    boot_img_cfg->pk_src = i;
    boot_img_cfg->img_valid = 0;

    arch_memcpy_fast(&boot_img_cfg->basic_cfg, &header->basic_cfg,
                     sizeof(header->basic_cfg));

#if BFLB_SP_BOOT2_SUPPORT_SIGN_ENCRYPT
    BOOT2_MSG_DBG("Encrypt mode:%d\r\n", g_efuse_cfg.app_encrypt_type);
    BOOT2_MSG_DBG("Sign mode:%d\r\n", g_efuse_cfg.app_sign_type);
    /* Check encrypt and sign match*/
    if (g_efuse_cfg.encrypted[i] != boot_img_cfg->basic_cfg.encrypt_type) {
        if (boot_img_cfg->basic_cfg.xts_mode == 0) {
            /* none-xts mode,must match */
            // BOOT2_MSG_ERR("Encrypt not fit\r\n");
            return 0x0205;
        } else if (boot_img_cfg->basic_cfg.encrypt_type == 0) {
            /* xts mode,encrypt_type must >0 */
            // BOOT2_MSG_ERR("Encrypt not fit\r\n");
            return 0x0205;
        }
    }

    if (g_efuse_cfg.sign[i] != boot_img_cfg->basic_cfg.sign_type) {
        // BOOT2_MSG_ERR("sign not fit\r\n");
        boot_img_cfg->basic_cfg.sign_type = g_efuse_cfg.sign[i];
        return 0x0206;
    }
#endif
    if (g_ps_mode == 1 && (!g_efuse_cfg.hbn_check_sign)) {
        /* In HBN Mode, if user select to ignore hash and sign*/
        boot_img_cfg->basic_cfg.hash_ignore = 1;
    } else if ((boot_img_cfg->basic_cfg.hash_ignore == 1 && *phash != HAL_BOOT2_DEADBEEF_VAL) ||
               g_efuse_cfg.sign[i] != 0) {
        /* If signed or user not really want to ignore, hash can't be ignored*/
        boot_img_cfg->basic_cfg.hash_ignore = 0;
    }

    if (g_user_hash_ignored) {
        boot_img_cfg->basic_cfg.hash_ignore = 1;
    }

    if (boot_img_cfg->basic_cfg.img_len_cnt == 0) {
        return 0x0207;
    }

    arch_memcpy_fast(&boot_img_cfg->cpu_cfg, &header->cpu_cfg,
                     sizeof(header->cpu_cfg));
    return 0;
}

/****************************************************************************/ /**
 * @brief  clean cache
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION hal_boot2_clean_cache(void)
{
    bflb_l1c_dcache_clean_invalidate_all();
    bflb_l1c_icache_invalid_all();
}

/****************************************************************************/ /**
 * @brief  set cache
 *
 * @param  cont_read: contiune mode
 * @param  boot_img_cfg: cache param
 *
 * @return 0 suss,other faild
 *
*******************************************************************************/
int32_t ATTR_TCM_SECTION hal_boot2_set_cache(uint8_t cont_read, boot2_image_config *boot_img_cfg)
{
    return bflb_flash_set_cache(cont_read, boot_img_cfg->cpu_cfg[0].cache_enable,
                                boot_img_cfg->cpu_cfg[0].cache_way_dis,
                                boot_img_cfg->basic_cfg.group_image_offset);
}

/****************************************************************************/ /**
 * @brief  get the ram image name and count
 *
 * @param  img_name: ram image name in partition
 * @param  ram_img_cnt: ram image count that support boot from flash
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_get_ram_img_cnt(char *img_name[], uint32_t *ram_img_cnt)
{
    *ram_img_cnt = 2;
    img_name[0] = "lp_fw";
    img_name[1] = "FW";
}

/****************************************************************************/ /**
 * @brief  get the ram image info
 *
 * @param  data: bootheader information
 * @param  image_offset: ram image offset in flash(from of bootheader)
 * @param  img_len: ram image length
 * @param  hash: pointer to hash pointer
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_get_img_info(uint8_t *data, uint32_t *image_offset, uint32_t *img_len, uint8_t **hash)
{
    struct hal_bootheader_t *header = (struct hal_bootheader_t *)data;
    uint32_t crc_pass = 0;
    *img_len = 0;
    uint8_t *phash = *hash;
    uint8_t hash_ignore = 1;

    crc_pass = hal_boot_check_bootheader(header);

    if (!crc_pass) {
        return;
    }
    if (0 != memcmp((void *)&header->magiccode, HAL_BOOT2_CPU0_MAGIC, sizeof(header->magiccode)) &&
        0 != memcmp((void *)&header->magiccode, HAL_BOOT2_CPU1_MAGIC, sizeof(header->magiccode))) {
        return;
    }

    /* for ram image, it always download in iot tab, so
       it share the same group with core 0 but use different image offset and
       boot entry */
    *image_offset = 4096 * 1; //bootheader->basic_cfg.group_image_offset;
    *img_len = header->basic_cfg.img_len_cnt;

    if (g_ps_mode == 1 && (!g_efuse_cfg.hbn_check_sign)) {
        /* In HBN Mode, if user select to ignore hash and sign*/
        hash_ignore = 1;
    } else if ((header->basic_cfg.hash_ignore == 1 && header->basic_cfg.hash[0] != HAL_BOOT2_DEADBEEF_VAL) ||
               g_efuse_cfg.sign[0] != 0) {
        /* If signed or user not really want to ignore, hash can't be ignored*/
        hash_ignore = 0;
    }

    if (hash_ignore == 1) {
        *hash = NULL;
    } else {
        arch_memcpy_fast(phash, header->basic_cfg.hash, sizeof(header->basic_cfg.hash));
#if HAL_BOOT2_SUPPORT_SIGN_SHA384
        /* For SHA384, read the remaining 16 bytes from after the boot header */
        if (HAL_BOOT_SIGN_TYPE_ECC_SHA384 == g_efuse_cfg.sign[0]) {
            uint8_t *hash_ext = data + sizeof(struct hal_bootheader_t);
            arch_memcpy_fast(&phash[HAL_BOOT2_PK_HASH_SIZE], hash_ext, 16);
        }
#endif
    }
}

/****************************************************************************/ /**
 * @brief  release other cpu to boot up
 *
 * @param  core: core number
 * @param  boot_addr: boot address
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION hal_boot2_release_cpu(uint32_t core, uint32_t boot_addr)
{
}

/****************************************************************************/ /**
 * @brief  get xip address according to flash addr
 *
 * @param  flash_addr: flash address
 *
 * @return XIP Address
 *
*******************************************************************************/
uint32_t hal_boot2_get_xip_addr(uint32_t flash_addr)
{
    uint32_t img_offset = bflb_sf_ctrl_get_flash_image_offset(0, SF_CTRL_FLASH_BANK0);
    if (flash_addr >= img_offset) {
        return HAL_BOOT2_FLASH_XIP_BASE + (flash_addr - img_offset);
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  get multi-group count
 *
 * @param  None
 *
 * @return 1 for multi-group 0 for not
 *
*******************************************************************************/
uint32_t hal_boot2_get_grp_count(void)
{
    return 1;
}

/****************************************************************************/ /**
 * @brief  get cpu count
 *
 * @param  None
 *
 * @return 1 for multi-group 0 for not
 *
*******************************************************************************/
uint32_t hal_boot2_get_cpu_count(void)
{
    return 1;
}

/****************************************************************************/ /**
 * @brief  get cpu count
 *
 * @param  None
 *
 * @return 1 for multi-group 0 for not
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION hal_boot2_get_feature_flag(void)
{
    return HAL_BOOT2_SP_FLAG;
}

/****************************************************************************/ /**
 * @brief  get boot header offset
 *
 * @param  None
 *
 * @return bootheader offset
 *
*******************************************************************************/
uint32_t hal_boot2_get_bootheader_offset(void)
{
    return 0x00;
}

/****************************************************************************/ /**
 * @brief  config reboot option
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_reboot_config(hal_reboot_cfg_t rbot)
{
    switch (rbot) {
        case HAL_REBOOT_AS_BOOTPIN:
            HBN_Set_User_Boot_Config(0);
            break;
        case HAL_REBOOT_FROM_INTERFACE:
            HBN_Set_User_Boot_Config(1);
            break;
        case HAL_REBOOT_FROM_MEDIA:
            HBN_Set_User_Boot_Config(2);
            break;
        default:
            HBN_Set_User_Boot_Config(0);
            break;
    }
}