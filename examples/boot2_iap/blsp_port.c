/**
  ******************************************************************************
  * @file    blsp_port.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "blsp_port.h"
#include "bflb_platform.h"
#include "blsp_bootinfo.h"
#include "blsp_common.h"
#include "blsp_media_boot.h"
#include "tzc_sec_reg.h"

/** @addtogroup  BL606_BLSP_Boot2
 *  @{
 */

/** @addtogroup  BLSP_PORT
 *  @{
 */

/** @defgroup  BLSP_PORT_Private_Macros
 *  @{
 */

/*@} end of group BLSP_PORT_Private_Macros */

/** @defgroup  BLSP_PORT_Private_Types
 *  @{
 */

/*@} end of group BLSP_PORT_Private_Types */

/** @defgroup  BLSP_PORT_Private_Variables
 *  @{
 */

/*@} end of group BLSP_PORT_Private_Variables */

/** @defgroup  BLSP_PORT_Global_Variables
 *  @{
 */

/*@} end of group BLSP_PORT_Global_Variables */

/** @defgroup  BLSP_PORT_Private_Fun_Declaration
 *  @{
 */

/*@} end of group BLSP_PORT_Private_Fun_Declaration */

/** @defgroup  BLSP_PORT_Private_Functions_User_Define
 *  @{
 */

/*@} end of group BLSP_PORT_Private_Functions_User_Define */

/** @defgroup  BLSP_PORT_Private_Functions
 *  @{
 */

/*@} end of group BLSP_PORT_Private_Functions */

/** @defgroup  BLSP_PORT_Public_Functions
 *  @{
 */
/****************************************************************************/ /**
 * @brief  Boot2 init timer for cal boot time
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void blsp_boot2_init_timer(void)
{
    TIMER_CFG_Type timer_cfg = {
        TIMER_CH0,               /* timer channel 0 */
        TIMER_CLKSRC_FCLK,       /* timer clock source:bus clock */
        TIMER_PRELOAD_TRIG_NONE, /* reaload on comaparator 2  */
        TIMER_COUNT_FREERUN,     /* preload when match occur */
        159,                     /* clock divider */
        0xfffffffe,              /* match value 0  */
        0xfffffffe,              /* match value 1 */
        0xfffffffe,              /* match value 2 */
        0,                       /* preload value */
    };

    /* Disable all interrupt */
    TIMER_IntMask(timer_cfg.timerCh, TIMER_INT_ALL, MASK);

    /* Disable timer before config */
    TIMER_Disable(timer_cfg.timerCh);

    /* Timer init with default configuration */
    TIMER_Init(&timer_cfg);

    /* Enable timer */
    TIMER_Enable(timer_cfg.timerCh);
}

/****************************************************************************/ /**
* @brief  Boot2 Disable other CPU cache
*
* @param  None
*
* @return None
*
*******************************************************************************/
void blsp_boot2_disable_other_cache(void)
{
}

/****************************************************************************/ /**
* @brief  Boot2 Flash Boot2 cache
*
* @param  None
*
* @return None
*
*******************************************************************************/
void blsp_boot2_flush_xip_cache()
{
    L1C_Cache_Flush(0xf);
}

/****************************************************************************/ /**
 * @brief  Boot2 Get fw clock config
 *
 * @param  cfg: Clock config pointer
 *
 * @return boot_error_code
 *
*******************************************************************************/
int32_t ATTR_TCM_SECTION blsp_boot2_get_clk_cfg(boot_clk_config *cfg)
{
    XIP_SFlash_Read_Via_Cache_Need_Lock(8 + sizeof(boot_flash_config) + BLSP_BOOT2_XIP_BASE,
                                        (uint8_t *)cfg, sizeof(boot_clk_config));

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
* @brief  Boot2 show timer for cal boot time
*
* @param  None
*
* @return None
*
*******************************************************************************/
void blsp_boot2_show_timer(void)
{
    MSG("Counter value=%d\n", (unsigned int)bflb_platform_get_time_ms());
}

/****************************************************************************/ /**
* @brief  Boot2 get efuse config security
*
* @param  None
*
* @return None
*
*******************************************************************************/
void ATTR_TCM_SECTION blsp_boot2_get_efuse_cfg(boot_efuse_hw_config *g_efuse_cfg)
{
    uint32_t tmp;

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    /* Get sign and aes type*/
    EF_Ctrl_Read_Secure_Boot((EF_Ctrl_Sign_Type *)g_efuse_cfg->sign, (EF_Ctrl_SF_AES_Type *)g_efuse_cfg->encrypted);
    /* Get hash:aes key slot 0 and slot1*/
    EF_Ctrl_Read_AES_Key(0, (uint32_t *)g_efuse_cfg->pk_hash_cpu0, 8);
    EF_Ctrl_Read_Chip_ID(g_efuse_cfg->chip_id);
    /* Get HBN check sign config */
    EF_Ctrl_Read_Sw_Usage(0, &tmp);
    g_efuse_cfg->hbn_check_sign = (tmp >> 22) & 0x01;

    GLB_Set_System_CLK_Div(0, 1);
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_DLL);
}

/****************************************************************************/ /**
* @brief  Boot2 reset sec_eng module
*
* @param  None
*
* @return None
*
*******************************************************************************/
void blsp_boot2_reset_sec_eng(void)
{
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_SEC);
}

/****************************************************************************/ /**
* @brief  Boot2 init sec_eng PKA module
*
* @param  None
*
* @return None
*
*******************************************************************************/
void BLSP_Boot2_Init_Sec_Eng_PKA(void)
{
    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();
}

/****************************************************************************/ /**
* @brief  Boot2 get cpu count info
*
* @param  None
*
* @return None
*
*******************************************************************************/
uint32_t blsp_boot2_get_cpu_count(void)
{
    return 1;
}

/****************************************************************************/ /**
 * @brief  Read power save mode
 *
 * @param  None
 *
 * @return BFLB_PSM_ACTIVE or BFLB_PSM_HBN
 *
*******************************************************************************/
uint8_t blsp_read_power_save_mode(void)
{
    if (HBN_Get_Status_Flag() == HBN_STATUS_WAKEUP_FLAG) {
        return BFLB_PSM_HBN;
    } else {
        return BFLB_PSM_ACTIVE;
    }
}

/****************************************************************************/ /**
 * @brief  Boot2 Pass parameter to FW
 *
 * @param  data: Data pointer to pass
 * @param  len: Data length
 *
 * @return None
 *
*******************************************************************************/
void blsp_boot2_pass_parameter(void *data, uint32_t len)
{
    static uint8_t *p_parameter = NULL;

    if (len == 0) {
        GLB_Set_EM_Sel(0);
        p_parameter = (uint8_t *)(0x42020000 + 60 * 1024);
        return;
    }

    ARCH_MemCpy_Fast(p_parameter, data, len);
    p_parameter += len;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get XIP offset
 *
 * @param  None
 *
 * @return XIP offset
 *
*******************************************************************************/
uint32_t blsp_boot2_get_xip_offset(void)
{
    return SF_Ctrl_Get_Flash_Image_Offset();
}

/****************************************************************************/ /**
 * @brief  Get User specified firmware to boot up
 *
 * @param  None
 *
 * @return User specified firmware name
 *
*******************************************************************************/
uint8_t *blsp_get_user_specified_fw(void)
{
    return (uint8_t *)(HBN_BASE + HBN_RSV0_OFFSET);
}

/****************************************************************************/ /**
 * @brief  Clear User specified firmware to boot up
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void blsp_clr_user_specified_fw(void)
{
    uint32_t *p = (uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET);

    *p = 0;
}

/****************************************************************************/ /**
 * @brief  Release other CPU
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION blsp_boot2_releae_other_cpu(void)
{
}

/****************************************************************************/ /**
 * @brief  Set encryption config
 *
 * @param  index: Region index
 * @param  g_boot_img_cfg: Boot image config pointer to hold parsed information
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
int32_t ATTR_TCM_SECTION blsp_boot2_set_encrypt(uint8_t index, boot_image_config *g_boot_img_cfg)
{
    uint32_t aes_enabled = 0;
    uint32_t len = 0;

    /* FIXME:,1:lock, should be 1??*/
    if (g_boot_img_cfg->encrypt_type != 0) {
        len = g_boot_img_cfg->img_segment_info.img_len;

        if (len != 0) {
            SF_Ctrl_AES_Set_Key_BE(index, NULL, (SF_Ctrl_AES_Key_Type)(g_boot_img_cfg->encrypt_type - 1));
            SF_Ctrl_AES_Set_IV_BE(index, g_boot_img_cfg->aes_iv, g_boot_img_cfg->img_start.flash_offset);

            SF_Ctrl_AES_Set_Region(index, 1 /*enable this region*/, 1 /*hardware key*/,
                                   g_boot_img_cfg->img_start.flash_offset,
                                   g_boot_img_cfg->img_start.flash_offset + len - 1,
                                   g_boot_img_cfg->aes_region_lock /*lock*/);
            aes_enabled = 1;
        }
    }

    if (aes_enabled) {
        SF_Ctrl_AES_Enable_BE();
        SF_Ctrl_AES_Enable();
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Media boot set cache according to image config
 *
 * @param  None
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
int32_t ATTR_TCM_SECTION blsp_boot2_set_cache(uint8_t cont_read, SPI_Flash_Cfg_Type *flash_cfg, boot_image_config *g_boot_img_cfg)
{
    uint32_t tmp[1];
    uint32_t stat;

    /* To make it simple, exit cont read anyway */
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);

    SFlash_Reset_Continue_Read(flash_cfg);

    if (flash_cfg->cReadSupport == 0) {
        cont_read = 0;
    }

    if (cont_read == 1) {
        stat = SFlash_Read(flash_cfg, flash_cfg->ioMode & 0xf, 1, 0x00000000, (uint8_t *)tmp, sizeof(tmp));

        if (SUCCESS != stat) {
            return BFLB_BOOT2_FAIL;
        }
    }

    /* Set default value */
    L1C_Cache_Enable_Set(0xf);

    if (g_boot_img_cfg[0].cache_enable) {
        if ((g_boot_img_cfg[0].entry_point & 0xFF000000) == BLSP_BOOT2_XIP_BASE) {
            SF_Ctrl_Set_Flash_Image_Offset(g_boot_img_cfg[0].img_start.flash_offset);
            SFlash_Cache_Read_Enable(flash_cfg, flash_cfg->ioMode & 0xf, cont_read, g_boot_img_cfg[0].cache_way_disable);
        } else {
        }
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Check if MSP is valid
 *
 * @param  msp_val: MSP value
 *
 * @return 1 for valid and 0 for invalid
 *
*******************************************************************************/
int32_t blsp_is_msp_valid(uint32_t msp_val)
{
    if (msp_val > BL702_WRAM_BASE && msp_val <= BL702_WRAM_END) {
        return 1;
    } else if (msp_val > BL702_WRAM_REMAP0_BASE && msp_val <= BL702_WRAM_REMAP0_END) {
        return 1;
    } else if (msp_val > BL702_WRAM_REMAP1_BASE && msp_val <= BL702_WRAM_REMAP1_END) {
        return 1;
    } else if (msp_val > BL702_WRAM_REMAP2_BASE && msp_val <= BL702_WRAM_REMAP2_END) {
        return 1;
    } else if (msp_val > BL702_TCM_BASE && msp_val <= BL702_TCM_END) {
        return 1;
    } else if (msp_val > BL702_TCM_REMAP0_BASE && msp_val <= BL702_TCM_REMAP0_END) {
        return 1;
    } else if (msp_val > BL702_TCM_REMAP1_BASE && msp_val <= BL702_TCM_REMAP1_END) {
        return 1;
    } else if (msp_val > BL702_TCM_REMAP2_BASE && msp_val <= BL702_TCM_REMAP2_END) {
        return 1;
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  Check if PC is valid
 *
 * @param  pcVal: PC value
 *
 * @return 1 for valid and 0 for invalid
 *
*******************************************************************************/
int32_t blsp_is_pc_valid(uint32_t pc_val)
{
    /* Check XIP memory */
    if (pc_val >= BL702_FLASH_XIP_BASE && pc_val < BL702_FLASH_XIP_END) {
        return 1;
    } else if (pc_val >= BL702_FLASH_XIP_REMAP0_BASE && pc_val < BL702_FLASH_XIP_REMAP0_END) {
        return 1;
    } else if (pc_val >= BL702_FLASH_XIP_REMAP1_BASE && pc_val < BL702_FLASH_XIP_REMAP1_END) {
        return 1;
    } else if (pc_val >= BL702_FLASH_XIP_REMAP2_BASE && pc_val < BL702_FLASH_XIP_REMAP2_END) {
        return 1;
    }

    /* Check RAM memory */
    if (pc_val >= BL702_WRAM_BASE && pc_val < BL702_WRAM_END) {
        return 1;
    } else if (pc_val >= BL702_WRAM_REMAP0_BASE && pc_val < BL702_WRAM_REMAP0_END) {
        return 1;
    } else if (pc_val >= BL702_WRAM_REMAP1_BASE && pc_val < BL702_WRAM_REMAP1_END) {
        return 1;
    } else if (pc_val >= BL702_WRAM_REMAP2_BASE && pc_val < BL702_WRAM_REMAP2_END) {
        return 1;
    } else if (pc_val >= BL702_TCM_BASE && pc_val < BL702_TCM_END) {
        return 1;
    } else if (pc_val >= BL702_TCM_REMAP0_BASE && pc_val < BL702_TCM_REMAP0_END) {
        return 1;
    } else if (pc_val >= BL702_TCM_REMAP1_BASE && pc_val < BL702_TCM_REMAP1_END) {
        return 1;
    } else if (pc_val >= BL702_TCM_REMAP2_BASE && pc_val < BL702_TCM_REMAP2_END) {
        return 1;
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  Security boot finished
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION blsp_sboot_finish(void)
{
    uint32_t tmp_val;

    tmp_val = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL);

    tmp_val = BL_SET_REG_BITS_VAL(tmp_val, TZC_SEC_TZC_SBOOT_DONE, 0xf);

    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL, tmp_val);
}

/****************************************************************************/ /**
 * @brief  Fix MSP and PC,if they are invalid
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void blsp_fix_invalid_msp_pc(void)
{
    uint32_t i = 0;

    for (i = 0; i < BFLB_BOOT2_CPU_MAX; i++) {
        if (!blsp_is_msp_valid(g_boot_img_cfg[i].msp_val)) {
            g_boot_img_cfg[i].msp_val = 0;
        }

        if (!blsp_is_pc_valid(g_boot_img_cfg[i].entry_point)) {
            g_boot_img_cfg[i].entry_point = 0;
        }
    }
}

typedef void (*pFunc)(void);
extern pFunc __Vectors[];
/****************************************************************************/ /**
 * @brief  Boot2 Get log tx GPIO
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_get_tx_gpio(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[9] + 1);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get UART Port
 *
 * @param  None
 *
 * @return 0(UART0)/1(UART1)
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_get_uart_port(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[9] + 2);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get Feature Flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_get_feature_flag(void)
{
    static uint8_t boot2_flag = 0xff;
    uint8_t *p = ((uint8_t *)&__Vectors[10] + 0);

    if (boot2_flag == 0xff) {
        boot2_flag = *p;
    }

    return boot2_flag;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get log disable Flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_get_log_disable_flag(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[10] + 1);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get 8M Flash support Flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_8m_support_flag(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[10] + 2);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get dump critical data flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_dump_critical_flag(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[10] + 3);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get Baudrate
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION blsp_boot2_get_baudrate(void)
{
    uint32_t *p = ((uint32_t *)&__Vectors[13]);

    return *p;
}

/*@} end of group BLSP_PORT_Public_Functions */

/*@} end of group BLSP_PORT */

/*@} end of group BL606_BLSP_Boot2 */
