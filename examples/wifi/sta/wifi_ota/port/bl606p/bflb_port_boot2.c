#include "bflb_port_boot2.h"
#include "bflb_flash.h"
#include "bl606p_ef_ctrl.h"
#include "bl606p_ef_cfg.h"
#include "bl606p_hbn.h"
#include "bl606p_glb.h"
#include "bl606p_pds.h"
#include "bl606p_tzc_sec.h"
#include "bflb_gpio.h"

#include "bl606p_psram.h"
#include "bflb_efuse.h"
#include "ef_data_0_reg.h"
#include "log.h"

/****************************************************************************/ /**
 * @brief  init clock
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_init_clock(void)
{
    CPU_Reset_MTimer();
    /* set mtimer clock 1M */
    CPU_Set_MTimer_CLK(ENABLE, CPU_Get_MTimer_Source_Clock() / 1000000 - 1);
    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_XCLK, 0);
}

/****************************************************************************/ /**
 * @brief  init psram gpio
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void hal_boot2_init_psram_gpio(void)
{

    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    for (uint8_t i = 0; i < 12; i++) {
        bflb_gpio_init(gpio, 52 + i, GPIO_INPUT | GPIO_PULL_NONE | GPIO_SMT_EN | GPIO_DRV_0);
    }
}

/****************************************************************************/ /**
 * @brief  init psram data queue strobe clock
 *
 * @param  burst_len: burst length
 *         is_fixLatency: fixedLatency or not
 *         latency: clocks latency
 *         dqs_delay: data queue strobe clock delay
 *
 * @return reg id0 value
 *
*******************************************************************************/
static uint16_t hal_boot2_psram_winbond_init_dqs(int8_t burst_len, uint8_t is_fixLatency, uint8_t latency, uint16_t dqs_delay)
{
    uint16_t reg_read = 0;
    PSRAM_Ctrl_Cfg_Type psramCtrlCfg = {
        .vendor = PSRAM_CTRL_VENDOR_WINBOND,
        .ioMode = PSRAM_CTRL_X8_MODE,
        .size = PSRAM_SIZE_32MB,
        .dqs_delay = 0xffc0,
    };

    PSRAM_Winbond_Cfg_Type winbondCfg = {
        .rst = DISABLE,
        .clockType = PSRAM_CLOCK_DIFF,
        .inputPowerDownMode = DISABLE,
        .hybridSleepMode = DISABLE,
        .linear_dis = ENABLE,
        .PASR = PSRAM_PARTIAL_REFRESH_FULL,
        .disDeepPowerDownMode = ENABLE,
        .fixedLatency = DISABLE,
        .brustLen = PSRAM_WINBOND_BURST_LENGTH_64_BYTES,
        .brustType = PSRAM_WRAPPED_BURST,
        .latency = PSRAM_WINBOND_6_CLOCKS_LATENCY,
        .driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M_115_OHMS_FOR_8M,
    };

    winbondCfg.brustLen = burst_len;
    winbondCfg.fixedLatency = is_fixLatency;
    winbondCfg.latency = latency;

    psramCtrlCfg.dqs_delay = dqs_delay;
    PSram_Ctrl_Init(PSRAM0_ID, &psramCtrlCfg);
    /*PSram_Ctrl_Winbond_Reset(PSRAM0_ID); */ /*Different psram there are differences, in order to unify the change to not configured*/
    if (0 != PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR0, &winbondCfg)) {
        pm_pds_mode_enter(7, 32768 * 2 /*2s*/); 
    }

    if (0 != PSram_Ctrl_Winbond_Read_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_ID0, &reg_read)) {
        pm_pds_mode_enter(7, 32768 * 2 /*2s*/); 
    }
    return reg_read;
}

#define PSRAM_BASIC_ADDR (0x54000000)
#define TEST_MEMORY_SIZE (1 * 1024 * 1024)

/****************************************************************************/ /**
 * @brief  psram read write check
 *
 * @param  None
 *
 * @return Suss or Error
 *
*******************************************************************************/
uint8_t hal_boot2_psram_rw_check(void)
{
    uint32_t i = 0;
    uint32_t test_pattern_val[] = {
        0xaa55aa55,
        0x33cc33cc,
        0x55aa55aa,
        0xcc33cc33,
    };
    volatile uint32_t *p = (uint32_t *)PSRAM_BASIC_ADDR;
    volatile uint32_t q = 0;

    p[0] = 0xaa55aa55;
    p[1] = 0x33cc33cc;
    p[2] = 0x55aa55aa;
    p[3] = 0xcc33cc33;

    __ISB();

    for (i = 0; i < 4; i++) {
        q = p[i];
        if (q != test_pattern_val[i]) {
            /* printf("RW ERROR 0x%x != 0x%x\r\n", i, q); */
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
            /* printf("RW ERROR 0x%x != 0x%x\r\n", i, q); */
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  psram calibration
 *
 * @param  psram_data queue strock clock window number 
 *
 * @return Error or psram id number
 *
*******************************************************************************/
static uint16_t hal_boot2_x8_psram_calibration(int32_t *psram_dqs_win_num)
{
    int16_t psram_id = 0, before_ef = 0;
    int32_t left_flag = 0, right_flag = 0, c_val = 0;
    int32_t dqs_win_min = 16, dqs_win_max = 0;

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

    LOG_F("r ef:0x%08lx\r\n", g_efuse_cfg.psram_dqs_cfg);
    before_ef = g_efuse_cfg.psram_dqs_cfg;
    if (g_efuse_cfg.psram_dqs_cfg != 0xffff) {
        left_flag = ((g_efuse_cfg.psram_dqs_cfg & (0xf0)) >> 0x4);
        right_flag = (g_efuse_cfg.psram_dqs_cfg & (0xf));
        c_val = ((left_flag + right_flag) >> 0x1);
        *psram_dqs_win_num = right_flag - left_flag;
        // printf("ef window: 0x%02x ~ 0x%02x; c_val: 0x%02x; dqs:0x%04x; code num:%d\r\n", left_flag, right_flag, c_val, dqs_val[c_val], (right_flag - left_flag));
        psram_id = hal_boot2_psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0, PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val[c_val]);
        if ((psram_id != HAL_BOOT2_PSRAM_ID1_WINBOND_4MB) && (psram_id != HAL_BOOT2_PSRAM_ID2_WINBOND_16MB) && (psram_id != HAL_BOOT2_PSRAM_ID3_WINBOND_32MB)) {
            return ERROR;
        }
    } else {
        for (uint32_t dqs_index = 0; dqs_index < 16; dqs_index++) {
            psram_id = hal_boot2_psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0, PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val[dqs_index]);
            if ((psram_id == HAL_BOOT2_PSRAM_ID1_WINBOND_4MB) || (psram_id == HAL_BOOT2_PSRAM_ID2_WINBOND_16MB) || (psram_id == HAL_BOOT2_PSRAM_ID3_WINBOND_32MB)) {
                if (hal_boot2_psram_rw_check() == SUCCESS) {
                    if (dqs_index < dqs_win_min) {
                        dqs_win_min = dqs_index;
                        dqs_win_max = dqs_index;
                    } else if (dqs_index > dqs_win_max) {
                        dqs_win_max = dqs_index;
                    }
                }
            }
        }

        left_flag = dqs_win_min;
        right_flag = dqs_win_max;
        c_val = ((left_flag + right_flag) >> 1);

        g_efuse_cfg.psram_dqs_cfg = (((left_flag << 0x4) | (right_flag)) & (0xff));
        
        LOG_F("c ef:0x%08lx\r\n", g_efuse_cfg.psram_dqs_cfg);
        *psram_dqs_win_num = right_flag - left_flag;
        if (((*psram_dqs_win_num) <= 4) || ((*psram_dqs_win_num) > 0xf)) {
            return ERROR;
        }
        psram_id = hal_boot2_psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0, PSRAM_WINBOND_6_CLOCKS_LATENCY, dqs_val[c_val]);
        if ((psram_id != HAL_BOOT2_PSRAM_ID1_WINBOND_4MB) && (psram_id != HAL_BOOT2_PSRAM_ID2_WINBOND_16MB) && (psram_id != HAL_BOOT2_PSRAM_ID3_WINBOND_32MB)) {
            return ERROR;
        }
        /* to do write efuse psram dqs delay */
        if (!(before_ef & 0x1fff)) {
            bflb_ef_ctrl_write_common_trim(NULL,"psram",g_efuse_cfg.psram_dqs_cfg,1);
        }
    }
    return psram_id;
}


/****************************************************************************/ /**
 * @brief  trim dcdc and ldo
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_efuse_trim(void)
{
    uint32_t tmpVal = 0;

    bflb_ef_ctrl_com_trim_t trim = {0};
    bflb_ef_ctrl_read_common_trim(NULL, "dcdc11", &trim, 1);
    if (trim.en) {
        if(trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value,trim.len)){
            tmpVal = BL_RD_REG(AON_BASE, AON_DCDC_TOP_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC11_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_DCDC_TOP_0, tmpVal);
        }
    }
    
    bflb_ef_ctrl_read_common_trim(NULL, "dcdc18", &trim, 1);
    if (trim.en) {
        if(trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value,trim.len)){
            tmpVal = BL_RD_REG(AON_BASE, AON_DCDC18_TOP_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC18_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_DCDC18_TOP_0, tmpVal);
        }
    }
    
    bflb_ef_ctrl_read_common_trim(NULL, "ldo28cis", &trim, 1);
    if (trim.en) {
        if(trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value,trim.len)){
            tmpVal = BL_RD_REG(GLB_BASE, GLB_LDO28CIS);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LDO28CIS_VOUT_TRIM, trim.value);
            BL_WR_REG(GLB_BASE, GLB_LDO28CIS, tmpVal);
        }
    }

    bflb_ef_ctrl_read_common_trim(NULL, "ldo15cis", &trim, 1);
    if (trim.en) {
        if(trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value,trim.len)){
            tmpVal = BL_RD_REG(GLB_BASE, GLB_LDO15CIS);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LDO15CIS_VOUT_TRIM, trim.value);
            BL_WR_REG(GLB_BASE, GLB_LDO15CIS, tmpVal);
        }
    }

    bflb_ef_ctrl_read_common_trim(NULL, "ldo18flash", &trim, 1);
    if (trim.en) {
        if(trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value,trim.len)){
            tmpVal = BL_RD_REG(GLB_BASE, GLB_LDO18FLASH);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LDO18FLASH_VOUT_TRIM, trim.value);
        BL_WR_REG(GLB_BASE, GLB_LDO18FLASH, tmpVal);
        }
    }

    bflb_ef_ctrl_read_common_trim(NULL, "usb20", &trim, 1);
    if (trim.en) {
        if(trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value,trim.len)){
            tmpVal = BL_RD_REG(AON_BASE, AON_PSW_IRRCV);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_USB20_RCAL_CODE_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_PSW_IRRCV, tmpVal);
        }
    }

    bflb_ef_ctrl_read_common_trim(NULL, "ldo18io", &trim, 1);
    if (trim.en) {
        if(trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value,trim.len)){
            tmpVal = BL_RD_REG(AON_BASE, AON_LDO18IO);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18IO_VOUT_SEL_AON, 0x2);
            BL_WR_REG(AON_BASE, AON_LDO18IO, tmpVal);
        }
    }
    
    
}



/****************************************************************************/ /**
 * @brief  hal_boot2_custom
 *
 * @param  custom_param
 *
 * @return 0
 *
*******************************************************************************/
uint32_t hal_boot2_custom(void *custom_param)
{
    int32_t psram_dqs_win_num = 0;

    hal_boot2_efuse_trim();
    
    /* for pds wakeup clock related to RF */
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_PDS);
    PDS_Power_On_MM_System();

    /* psram calibration */
    uint32_t ret = 0;
    if (((g_efuse_cfg.dev_info & HAL_BOOT2_PSRAM_INFO_MASK) >> HAL_BOOT2_PSRAM_INFO_POS) != 0) {
        hal_boot2_init_psram_gpio();
        /* ToDo: if WIFIPLL_320m not work */
        GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_EMI_WIFIPLL_320M);
        GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_EMI_CPUPLL_400M);
        GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_EMI_AUPLL_DIV1);
        //GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_PSRAM0_CTRL | GLB_AHB_CLOCK_PSRAM1_CTRL);
        GLB_PSRAM_PER_Clock_UnGate();
        GLB_Set_PSram_CLK(1, ENABLE, GLB_PSRAM_EMI_AUPLL_DIV1, 1);

        hal_boot2_psram_winbond_init_dqs(PSRAM_WINBOND_BURST_LENGTH_64_BYTES, 0, PSRAM_WINBOND_6_CLOCKS_LATENCY, 0xffc0);
        Tzc_Sec_PSRAMB_Access_Release();
        ret = hal_boot2_x8_psram_calibration(&psram_dqs_win_num);
        if (ret == ERROR) {
            while (1) {
                LOG_F("psram:%d\r\n", (int)psram_dqs_win_num);
                arch_delay_ms(500);
            }
        }
        LOG_F("psram:%d \r\n", (int)psram_dqs_win_num);

        /* Flush i-cache in case branch prediction logic is wrong when
       psram is not inited by hal_boot2_custom but cpu has already prefetch psram */
        __ISB();
    }
    return ret;
}

/****************************************************************************/ /**
 * @brief  hal_boot2_read_secure_boot
 *
 * @param  aes
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_read_secure_boot(uint8_t aes[HAL_BOOT2_CPU_GROUP_MAX])
{
    uint32_t tmpval = 0;
    bflb_ef_ctrl_read_direct(NULL, EF_DATA_0_EF_CFG_0_OFFSET, &tmpval, 1, 1);
    tmpval &= EF_DATA_0_EF_SF_AES_MODE_MSK;
    tmpval = tmpval >> EF_DATA_0_EF_SF_AES_MODE_POS;

    aes[1] = aes[0] = tmpval & 0xff;
}

/****************************************************************************/ /**
 * @brief  hal_boot2_get_efuse_cfg
 *
 * @param  efuse_cfg
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_get_efuse_cfg(boot2_efuse_hw_config *efuse_cfg)
{
    uint32_t i = 0;
    struct boot_efuse_sw_cfg0_t sw_cfg0;

    /* get hw cfg (signature and aes type) */
    hal_boot2_read_secure_boot(efuse_cfg->encrypted);
    

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        if (efuse_cfg->encrypted[i] == EF_CTRL_SF_AES_192) {
            efuse_cfg->encrypted[i] = SF_CTRL_AES_192BITS + 1;
        } else if (efuse_cfg->encrypted[i] == EF_CTRL_SF_AES_256) {
            efuse_cfg->encrypted[i] = SF_CTRL_AES_256BITS + 1;
        }
    }

    /* get sw uasge 0 */
    bflb_efuse_read_sw_usage(0, (uint32_t *)&sw_cfg0);
    /* get sw uasge 1 */
    //EF_Ctrl_Read_Sw_Usage(1, &sw_cfg1);

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        efuse_cfg->sign[i] = ((struct boot_efuse_sw_cfg0_t)sw_cfg0).sign_cfg;
    }
    for (i = 1; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        efuse_cfg->encrypted[i] = EF_CTRL_SF_AES_NONE;
    }

    efuse_cfg->hbn_check_sign = (uint8_t)(sw_cfg0.hbn_check_sign);
    efuse_cfg->sf_pin_cfg = (uint8_t)(sw_cfg0.sf_pin_cfg);
    efuse_cfg->uart_download_cfg = (uint8_t)(sw_cfg0.uart_download_cfg);
    efuse_cfg->keep_dbg_port_closed = (uint8_t)(sw_cfg0.keep_dbg_port_closed);
    efuse_cfg->boot_pin_cfg = (uint8_t)(sw_cfg0.boot_pin_cfg);

    /* get psram dqs delay info */
    bflb_ef_ctrl_com_trim_t trim;
    efuse_cfg->psram_dqs_cfg = 0xffff;
    bflb_ef_ctrl_read_common_trim(NULL, "psram", &trim, 1);
    if (trim.en) {
        if(trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value,trim.len)){
            efuse_cfg->psram_dqs_cfg = trim.value;
        }
    }

    /* get device info */
    bflb_ef_ctrl_get_device_info((bflb_efuse_device_info_type *)&efuse_cfg->dev_info);

    /* get chip id */
    bflb_efuse_get_chipid(efuse_cfg->chip_id);

    /* get public key hash */
    bflb_efuse_read_aes_key(0, (uint8_t *)efuse_cfg->pk_hash_cpu0, HAL_BOOT2_PK_HASH_SIZE / 4);
    bflb_efuse_read_aes_key(8, (uint8_t *)efuse_cfg->pk_hash_cpu1, HAL_BOOT2_PK_HASH_SIZE / 4);

}

/****************************************************************************/ /**
 * @brief  reset sec eng clock
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_reset_sec_eng(void)
{
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SEC_ENG);
    //Sec_Eng_Group0_Request_SHA_Access();
    //Sec_Eng_Group0_Request_AES_Access();
    //Sec_Eng_Group0_Request_Trng_Access();
    //Sec_Eng_Trng_Enable();
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
 * @brief  hal_boot2_set_psmode_status
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
 * @brief  hal_boot2_get_psmode_status
 *
 * @param  None
 *
 * @return status flag
 *
*******************************************************************************/
uint32_t hal_boot2_get_psmode_status(void)
{
    return HBN_Get_Status_Flag();
}

/****************************************************************************/ /**
 * @brief  hal_boot2_get_user_fw
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint32_t hal_boot2_get_user_fw(void)
{
    return BL_RD_WORD(HBN_BASE + HBN_RSV0_OFFSET);
}

/****************************************************************************/ /**
 * @brief  hal_boot2_clr_user_fw
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
 * @brief  hal_boot2_sboot_finish
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION hal_boot2_sboot_finish(void)
{
    //    uint32_t tmpVal;
    //    if (!g_efuse_cfg.keep_dbg_port_closed) {
    //        /* only after this, JTAG is really open */
    //        tmpVal = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL);
    //        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_SBOOT_DONE, 0xf);
    //        BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL, tmpVal);
    //    }
}

/****************************************************************************/ /**
 * @brief  uart gpio init
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_uart_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, GPIO_PIN_20, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_21, GPIO_UART_FUNC_UART0_RX);
}

/****************************************************************************/ /**
 * @brief  uart gpio init
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void hal_boot2_debug_uart_gpio_init(void)
{
}

/****************************************************************************/ /**
 * @brief  usb init
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#if HAL_BOOT2_SUPPORT_USB_IAP
void hal_boot2_debug_usb_port_init(void)
{
}
#endif

/****************************************************************************/ /**
 * @brief  uart deinit
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
 * @param  g_boot_img_cfg: Boot image config pointer
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
        //MSG_ERR("bootheader crc error\r\n");
        //blsp_dump_data((uint8_t *)&crc, 4);
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
        //MSG_ERR("Magic code error\r\n");
        return 0x0203;
    }

    if (boot_img_cfg == NULL) {
        return 0;
    }

    boot_img_cfg->pk_src = i;
    boot_img_cfg->img_valid = 0;

    arch_memcpy_fast(&boot_img_cfg->basic_cfg, &header->basic_cfg,
                     sizeof(header->basic_cfg));

    /* Check encrypt and sign match*/
    if (g_efuse_cfg.encrypted[i] != boot_img_cfg->basic_cfg.encrypt_type) {
        if (boot_img_cfg->basic_cfg.xts_mode == 0) {
            //("Encrypt not fit\r\n");
            return 0x0205;
        }
    }

    if (g_efuse_cfg.sign[i] != boot_img_cfg->basic_cfg.sign_type) {
        //MSG_ERR("sign not fit\r\n");
        boot_img_cfg->basic_cfg.sign_type = g_efuse_cfg.sign[i];
        return 0x0206;
    }

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
 * @param  
 *
 * @return cache set result
 *
*******************************************************************************/
void ATTR_TCM_SECTION hal_boot2_clean_cache(void)
{
    L1C_DCache_Clean_Invalid_All();
    L1C_ICache_Invalid_All();
}

/****************************************************************************/ /**
 * @brief  set cache 
 *
 * @param  cont_read: continue read mode
 *         boot_img_cfg: cache set param
 *
 * @return cache set result
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION hal_boot2_set_cache(uint8_t cont_read, boot2_image_config *boot_img_cfg)
{
    bflb_flash_set_cache(cont_read, boot_img_cfg->cpu_cfg[0].cache_enable,
                         boot_img_cfg->cpu_cfg[0].cache_way_dis,
                         boot_img_cfg->basic_cfg.group_image_offset);

    return SUCCESS;
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
    *ram_img_cnt = 0;
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
        memcpy(phash, header->basic_cfg.hash, sizeof(header->basic_cfg.hash));
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
    GLB_Set_CPU_Reset_Address(core, boot_addr);
    GLB_Release_CPU(core);
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
        return BL606P_FLASH_XIP_BASE + (flash_addr - img_offset);
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  get if multi-group
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
    return 3;
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
    return HAL_BOOT2_MP_FLAG;
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
    return 0x0;
}
/****************************************************************************/ /**
 * @brief  hal_reboot_config
 *
 * @param  boot source
 *
 * @return bootheader offset
 *
*******************************************************************************/
void hal_reboot_config(hal_reboot_cfg_t rbot)
{
    switch(rbot){
        case HAL_REBOOT_AS_BOOTPIN:
            HBN_Set_Hand_Off_Config(0);
            break;
        case HAL_REBOOT_FROM_INTERFACE:
            HBN_Set_Hand_Off_Config(1);
            break;
        case HAL_REBOOT_FROM_MEDIA:
            HBN_Set_Hand_Off_Config(2);
            break;
        default :
            HBN_Set_Hand_Off_Config(0);
            break;
    }
}
