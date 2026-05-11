/**
  ******************************************************************************
  * @file    bl616cl_romapi_patch.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2024 Bouffalo Lab</center></h2>
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

#include "bl616cl_romapi_patch.h"
#include "bl616cl_romdriver_e907.h"

BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ext_DCDC_Auto_Ctrl(uint8_t dcdc_idx, BL_Fun_Type enable, uint8_t aon_gpio)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_EXT_DCDC_CFG);

    if (enable) {
        CHECK_PARAM(IS_HBN_AON_PAD_TYPE(aon_gpio));

        HBN_Aon_Pad_Cfg_Set(0, aon_gpio);
    }

    if (dcdc_idx == 1) {
        if (enable) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_EXT_DCDC1_AON_GPIO_SEL, aon_gpio);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_EXT_DCDC1_AON_CTRL_HW_EN);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_HW_EXT_DCDC1_ON_EN);
        } else {
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_EXT_DCDC1_AON_CTRL_HW_EN);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_HW_EXT_DCDC1_ON_EN);
        }
    } else if (dcdc_idx == 2) {
        if (enable) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_EXT_DCDC2_AON_GPIO_SEL, aon_gpio);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_EXT_DCDC2_AON_CTRL_HW_EN);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_HW_EXT_DCDC2_ON_EN);
        } else {
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_EXT_DCDC2_AON_CTRL_HW_EN);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_HW_EXT_DCDC2_ON_EN);
        }
    } else {
        return ERROR;
    }

    BL_WR_REG(HBN_BASE, HBN_EXT_DCDC_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Configure external DCDC1 automatic control in HBN mode
 *
 * @param  enable: ENABLE or DISABLE
 * @param  aon_gpio: AON GPIO used to control external DCDC1 when enabled.
 *          This parameter can be one of the following values:
 *           @arg HBN_AON_PAD_GPIO0
 *           @arg HBN_AON_PAD_GPIO1
 *           @arg HBN_AON_PAD_GPIO2
 *           @arg HBN_AON_PAD_GPIO3
 *           @arg HBN_AON_PAD_GPIO4
 *           @arg HBN_AON_PAD_GPIO5
 *
 * @return SUCCESS or ERROR
 *
 * @note   When enabled, the selected AON GPIO is driven low in HBN mode and
 *         driven high again after HBN wakeup.
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ext_DCDC1_Auto_Ctrl(BL_Fun_Type enable, uint8_t aon_gpio)
{
    return HBN_Set_Ext_DCDC_Auto_Ctrl(1, enable, aon_gpio);
}

/****************************************************************************/ /**
 * @brief  Configure external DCDC2 automatic control in HBN mode
 *
 * @param  enable: ENABLE or DISABLE
 * @param  aon_gpio: AON GPIO used to control external DCDC2 when enabled.
 *          This parameter can be one of the following values:
 *           @arg HBN_AON_PAD_GPIO0
 *           @arg HBN_AON_PAD_GPIO1
 *           @arg HBN_AON_PAD_GPIO2
 *           @arg HBN_AON_PAD_GPIO3
 *           @arg HBN_AON_PAD_GPIO4
 *           @arg HBN_AON_PAD_GPIO5
 *
 * @return SUCCESS or ERROR
 *
 * @note   When enabled, the selected AON GPIO is driven low in HBN mode and
 *         driven high again after HBN wakeup.
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ext_DCDC2_Auto_Ctrl(BL_Fun_Type enable, uint8_t aon_gpio)
{
    return HBN_Set_Ext_DCDC_Auto_Ctrl(2, enable, aon_gpio);
}

/****************************************************************************/ /**
 * @brief  Set RC3K R_CODE
 *
 * @param  r_code
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_RC32K_R_Code(uint32_t r_code)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_CAL_AON, r_code);
    BL_WR_REG(HBN_BASE, HBN_RC32K_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get RC3K R_CODE
 *
 * @param  NULL
 *
 * @return r_code
 *
*******************************************************************************/
uint32_t ATTR_CLOCK_SECTION HBN_Get_RC32K_R_Code(void)
{
    uint32_t tmpVal;
    uint32_t r_code;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_0);
    r_code = BL_GET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_CAL_AON);

    return r_code;
}

/****************************************************************************/ /**
 * @brief  USB PHY enter suspend mode for low power
 *
 * @note   This function configures USB PHY to enter suspend mode, allowing
 *         XTAL to be turned off during PDS sleep. Used for USB low power
 *         scenarios where current consumption is critical.
 *         Registers: PDS_U2PHY_1 (0x2000E508)
 *         - SUSPENDM0_USE_REG (bit 0): Set to 1 for register control
 *         - SUSPENDM0 (bit 23): Clear to 0 for suspend state
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_USB_PHY_Suspend(void)
{
    uint32_t tmpVal = 0;

    /* Configure USB PHY suspend via PDS_U2PHY_1 register */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_U2PHY_1);
    /* Set SUSPENDM0_USE_REG to 1: use register control for suspend */
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_SUSPENDM0_USE_REG);
    /* Clear SUSPENDM0 to 0: enter suspend state */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_SUSPENDM0);
    BL_WR_REG(PDS_BASE, PDS_U2PHY_1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  USB PHY exit suspend mode
 *
 * @note   This function restores USB PHY from suspend mode to normal
 *         operation after PDS wakeup.
 *         Registers: PDS_U2PHY_1 (0x2000E508)
 *         - SUSPENDM0_USE_REG (bit 0): Clear to 0 to disable register control
 *         - SUSPENDM0 (bit 23): Set to 1 to exit suspend (active)
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_USB_PHY_Resume(void)
{
    uint32_t tmpVal = 0;

    /* Restore USB PHY from suspend via PDS_U2PHY_1 register */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_U2PHY_1);
    /* Clear SUSPENDM0_USE_REG to 0: disable register control */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_SUSPENDM0_USE_REG);
    /* Set SUSPENDM0 to 1: exit suspend state (active) */
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_SUSPENDM0);
    BL_WR_REG(PDS_BASE, PDS_U2PHY_1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read adc vref trim
 *
 * @param [in] dev ADC device handle
 *
 * @return vref trim value
 *
*******************************************************************************/
uint32_t bflb_efuse_get_adc_vref_trim(struct bflb_device_s *dev)
{
    bflb_ef_ctrl_com_trim_t trim;
    /* use hardware default value, also the median value. */
    uint32_t default_value = 32;

    bflb_ef_ctrl_read_common_trim(NULL, "gpadc_vref", &trim, 1);

    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            default_value = trim.value;
        }
    }

    return default_value;
}

/****************************************************************************/ /**
 * @brief  Efuse read adc offset trim
 *
 * @param [in] dev ADC device handle
 *
 * @return offset trim value
 *
*******************************************************************************/
int32_t bflb_efuse_get_adc_offset_trim(struct bflb_device_s *dev)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t default_value = 0;

    bflb_ef_ctrl_read_common_trim(NULL, "gpadc_offset", &trim, 1);

    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            /* trim.value is a 9-bits signed value */
            if (trim.value & 0x100) {
                default_value = trim.value | 0xFFFFFE00;
            } else {
                default_value = trim.value & 0x1FF;
            }
        }
    }

    return default_value;
}

/****************************************************************************/ /**
 * @brief  Efuse read adc gain trim
 *
 * @param [in] dev ADC device handle
 *
 * @return coe
 *
*******************************************************************************/
float bflb_efuse_get_adc_gain_trim(struct bflb_device_s *dev)
{
    bflb_ef_ctrl_com_trim_t trim;
    uint32_t tmp;

    float coe = 1.0;

    bflb_ef_ctrl_read_common_trim(NULL, "gpadc_gain", &trim, 1);

    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            tmp = trim.value;

            if (tmp & 0x800) {
                tmp = ~tmp;
                tmp += 1;
                tmp = tmp & 0xfff;
                coe = (1.0f + ((float)tmp / 2048.0f));
            } else {
                coe = (1.0f - ((float)tmp / 2048.0f));
            }
        }
    }

    return coe;
}

/****************************************************************************/ /**
 * @brief  Efuse write optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[6]: MAC address buffer
 * @param  program: Whether program
 *
 * @return 0 or -1
 *
*******************************************************************************/
int bflb_efuse_write_mac_address_opt(uint8_t slot, uint8_t mac[6], uint8_t program)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpval;
    uint32_t i = 0, cnt;

    if (slot >= 3) {
        return -1;
    }

    /* Change to local order */
    for (i = 0; i < 3; i++) {
        tmpval = mac[i];
        mac[i] = mac[5 - i];
        mac[5 - i] = tmpval;
    }
    /* The low 32 bits */

    tmpval = BL_RDWD_FRM_BYTEP(maclow);
    if (slot == 0) {
        bflb_ef_ctrl_write_direct(NULL, 0x04, &tmpval, 1, program);
    } else if (slot == 1) {
        bflb_ef_ctrl_write_direct(NULL, 0x1C, &tmpval, 1, program);
    } else if (slot == 2) {
        bflb_ef_ctrl_write_direct(NULL, 0x24, &tmpval, 1, program);
    }

    /* The high 16 bits */
    tmpval = machigh[0] + (machigh[1] << 8);
    cnt = 0;

    for (i = 0; i < 6; i++) {
        cnt += bflb_ef_ctrl_get_byte_zero_cnt(mac[i]);
    }

    tmpval |= ((cnt & 0x3f) << 16);
    if (slot == 0) {
        bflb_ef_ctrl_write_direct(NULL, 0x08, &tmpval, 1, program);
    } else if (slot == 1) {
        bflb_ef_ctrl_write_direct(NULL, 0x20, &tmpval, 1, program);
    } else if (slot == 2) {
        bflb_ef_ctrl_write_direct(NULL, 0x28, &tmpval, 1, program);
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  HBN enable GPIO interrupt
 *
 * @param  pad: GPIO pin number, this parameter can be one of the following values:
 *           @arg GPIO_PIN_0
 *           @arg GPIO_PIN_1
 *           @arg GPIO_PIN_2
 *           @arg GPIO_PIN_3
 *           @arg GPIO_PIN_4
 *           @arg GPIO_PIN_5
 * @param  gpioIntTrigType: HBN GPIO interrupt trigger type, this parameter can be one of the following values:
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_GPIO_INT_Enable(uint8_t pad, uint8_t gpioIntTrigType)
{
    uint32_t tmpVal;
    uint32_t reg_addr;
    uint32_t shift;

    CHECK_PARAM(IS_HBN_GPIO_INT_TRIGGER_TYPE(gpioIntTrigType));

    switch (pad) {
        case GPIO_PIN_0:
            reg_addr = HBN_BASE + HBN_IRQ_MODE_OFFSET;
            shift = HBN_PIN_WAKEUP_MODE_POS;
            break;
        case GPIO_PIN_1:
            reg_addr = HBN_BASE + HBN_IRQ_MODE_OFFSET;
            shift = 10;
            break;
        case GPIO_PIN_2:
            reg_addr = HBN_BASE + HBN_IRQ_MODE_OFFSET;
            shift = 20;
            break;
        case GPIO_PIN_3:
            reg_addr = HBN_BASE + HBN_IRQ_MODE_OFFSET;
            shift = 28;
            break;
        case GPIO_PIN_4:
            reg_addr = HBN_BASE + HBN_IRQ_MODE2_OFFSET;
            shift = HBN_PIN4_WAKEUP_MODE_POS;
            break;
        case GPIO_PIN_5:
            reg_addr = HBN_BASE + HBN_IRQ_MODE2_OFFSET;
            shift = HBN_PIN5_WAKEUP_MODE_POS;
            break;
        default:
            return ERROR;
    }

    tmpVal = BL_RD_WORD(reg_addr);
    tmpVal &= ~(0xFU << shift);
    tmpVal |= ((uint32_t)gpioIntTrigType << shift);
    BL_WR_WORD(reg_addr, tmpVal);

    return SUCCESS;

}

/****************************************************************************/ /**
 * @brief  HBN set AON_IO interrupt Trigger Mode
 *
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~5
 * @param  gpioIntTrigType: HBN GPIO interrupt trigger type, this parameter can be one of the following values:
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_SYNC_RISING_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL
 *           @arg HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Aon_Pad_IntMode(uint8_t pad, uint8_t gpioIntTrigType)
{
    uint32_t tmpVal;
    uint32_t reg_addr;
    uint32_t shift;

    CHECK_PARAM(IS_HBN_GPIO_INT_TRIGGER_TYPE(gpioIntTrigType));

    switch (pad) {
        case GPIO_PIN_0:
            reg_addr = HBN_BASE + HBN_IRQ_MODE_OFFSET;
            shift = HBN_PIN_WAKEUP_MODE_POS;
            break;
        case GPIO_PIN_1:
            reg_addr = HBN_BASE + HBN_IRQ_MODE_OFFSET;
            shift = 10;
            break;
        case GPIO_PIN_2:
            reg_addr = HBN_BASE + HBN_IRQ_MODE_OFFSET;
            shift = 20;
            break;
        case GPIO_PIN_3:
            reg_addr = HBN_BASE + HBN_IRQ_MODE_OFFSET;
            shift = 28;
            break;
        case GPIO_PIN_4:
            reg_addr = HBN_BASE + HBN_IRQ_MODE2_OFFSET;
            shift = HBN_PIN4_WAKEUP_MODE_POS;
            break;
        case GPIO_PIN_5:
            reg_addr = HBN_BASE + HBN_IRQ_MODE2_OFFSET;
            shift = HBN_PIN5_WAKEUP_MODE_POS;
            break;
        default:
            return ERROR;
    }

    tmpVal = BL_RD_WORD(reg_addr);
    tmpVal &= ~(0xFU << shift);
    tmpVal |= ((uint32_t)gpioIntTrigType << shift);
    BL_WR_WORD(reg_addr, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN aon pad debbug pull config
 *
 * @param  puPdEn: Enable or disable aon pad pull down and pull up
 * @param  rsvd: reserved
 * @param  maskVal: int mask
 * @param  dlyEn: Enable or disable aon pad wakeup delay function
 * @param  dlySec: aon pad wakeup delay sec 1 to 7
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Aon_Pad_WakeUpCfg(BL_Fun_Type puPdEn, uint8_t rsvd, uint32_t maskVal, BL_Fun_Type dlyEn, uint8_t dlySec)
{
    uint32_t tmpVal;

    CHECK_PARAM(((dlySec >= 1) && (dlySec <= 7)));
    CHECK_PARAM((maskVal <= 0x3FF));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_EN, dlyEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_SEL, dlySec);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_EN_HW_PU_PD, puPdEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read/write lock AES key
 *
 * @param  key_index: AES key slot index
 * @param  rd_lock: read lock enable
 * @param  wr_lock: write lock enable
 *
 * @return 0 for success
 *
*******************************************************************************/
int bflb_efuse_rw_lock_aes_key(uint8_t key_index, uint8_t rd_lock, uint8_t wr_lock)
{
    uint32_t tmpval = 0;

    if (0 == key_index) {
        if (wr_lock) {
            tmpval |= (1 << 17);
        }
        if (rd_lock) {
            tmpval |= (1 << 27);
        }
    } else if (1 == key_index) {
        if (wr_lock) {
            tmpval |= (1 << 18);
        }
        if (rd_lock) {
            tmpval |= (1 << 28);
        }
    } else if (2 == key_index) {
        if (wr_lock) {
            tmpval |= ((1 << 19) | (1 << 20) | (1 << 21));
        }
        if (rd_lock) {
            tmpval |= ((1 << 29) | (1 << 30) | (1 << 31));
        }
    } else if (3 == key_index) {
        if (wr_lock) {
            tmpval |= ((1 << 22) | (1 << 23));
        }
        if (rd_lock) {
            tmpval |= (1 << 14);
        }
    } else if (4 == key_index) {
        if (wr_lock) {
            tmpval |= (1 << 24);
        }
        if (rd_lock) {
            tmpval |= (1 << 25);
        }
    }
    bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);

    return 0;
}