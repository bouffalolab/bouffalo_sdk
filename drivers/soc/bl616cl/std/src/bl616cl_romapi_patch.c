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