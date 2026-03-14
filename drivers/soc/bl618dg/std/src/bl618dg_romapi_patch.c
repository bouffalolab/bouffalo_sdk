/**
  ******************************************************************************
  * @file    bl618dg_romapi_patch.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2021 Bouffalo Lab</center></h2>
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

#include "bl618dg_romapi_patch.h"
#include "bl618dg_romdriver_e907.h"

/****************************************************************************/ /**
 * @brief  swap SPI1 MOSI with MISO
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Swap_MCU_SPI_1_MOSI_With_MISO(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_HW_RSV3);
    if (enable) {
        tmpVal |= (1 << 30);
    } else {
        tmpVal &= ~(1 << 30);
    }
    BL_WR_REG(GLB_BASE, GLB_HW_RSV3, tmpVal);

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

    if (dev->idx == 0) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc_vref", &trim, 1);
    } else if (dev->idx == 1) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc2_vref", &trim, 1);
    } else {
        return default_value;
    }

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

    if (dev->idx == 0) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc_offset", &trim, 1);
    } else if (dev->idx == 1) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc2_offset", &trim, 1);
    } else {
        return default_value;
    }

    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            /* trim.value is a 9-bits signed value */
            if (trim.value & 0x8000) {
                default_value = trim.value | 0xFFFF0000;
            } else {
                default_value = trim.value & 0xFFFF;
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

    float coe = 1.0f;

    if (dev->idx == 0) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc_gain", &trim, 1);
    } else if (dev->idx == 1) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc2_gain", &trim, 1);
    } else {
        return coe;
    }

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
