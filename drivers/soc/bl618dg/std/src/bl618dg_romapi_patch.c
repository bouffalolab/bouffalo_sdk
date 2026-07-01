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
#include "bl618dg_aon.h"
#include "bl618dg_hbn.h"
#include "bl618dg_pds.h"
#include "bflb_ef_ctrl.h"

#define  WHO_AM_I    (0x60f82800)
#define  I_AM_A0     (0x0616d001)
#define  PSRAM_X8_CTRL_WAIT_TIMEOUT 1000

#if defined(CPU_MODEL_A0)
#define EFUSE_TRIM_PSRAM_OFFSET      0xB4
#define EFUSE_TRIM_LDO_RC32M_OFFSET  0xB8
#define EFUSE_TRIM_XTAL_OFFSET       0xBC
#else
#define EFUSE_TRIM_PSRAM_OFFSET      0x1F0
#define EFUSE_TRIM_LDO_RC32M_OFFSET  0x1F4
#define EFUSE_TRIM_XTAL_OFFSET       0x1F8
#endif

static void PSram_Ctrl_Request(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;

    tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_CONFIG_REQ, 1);
    BL_WR_REG(psram_base, PSRAM_CONFIGURE, tmpVal);

    do {
        tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            break;
        }
    } while (!BL_IS_REG_BIT_SET(tmpVal, PSRAM_REG_CONFIG_GNT));
}

static void PSram_Ctrl_Release(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_CONFIG_REQ, 0);
    BL_WR_REG(psram_base, PSRAM_CONFIGURE, tmpVal);
}

BL_Err_Type PSram_Ctrl_Winbond_Hybrid_Sleep_Set(PSRAM_ID_Type PSRAM_ID, PSRAM_Hybrid_Sleep_Mode sleepMode)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;

    CHECK_PARAM(IS_PSRAM_ID_TYPE(PSRAM_ID));
    CHECK_PARAM(IS_PSRAM_HYBRID_SLEEP_MODE(sleepMode));

    PSram_Ctrl_Request(PSRAM_ID);

    tmpVal = BL_RD_REG(psram_base, PSRAM_WINBOND_PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_WB_HYBRID_SLP, sleepMode);
    BL_WR_REG(psram_base, PSRAM_WINBOND_PSRAM_CONFIGURE, tmpVal);

    tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_WB_REG_SEL, PSRAM_WINBOND_REG_CR1);
    BL_WR_REG(psram_base, PSRAM_CONFIGURE, tmpVal);

    tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_CONFIG_W_PUSLE, 1);
    BL_WR_REG(psram_base, PSRAM_CONFIGURE, tmpVal);

    do {
        tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            PSram_Ctrl_Release(PSRAM_ID);
            return TIMEOUT;
        }
    } while (!BL_IS_REG_BIT_SET(tmpVal, PSRAM_STS_CONFIG_W_DONE));

    PSram_Ctrl_Release(PSRAM_ID);

    return SUCCESS;
}

BL_Err_Type PSram_Ctrl_Winbond_Enter_Hybrid_Sleep(PSRAM_ID_Type PSRAM_ID)
{
    return PSram_Ctrl_Winbond_Hybrid_Sleep_Set(PSRAM_ID, PSRAM_HYBRID_SLEEP_ENABLE);
}

BL_Err_Type PSram_Ctrl_Winbond_Exit_Hybrid_Sleep(PSRAM_ID_Type PSRAM_ID)
{
    return PSram_Ctrl_Winbond_Hybrid_Sleep_Set(PSRAM_ID, PSRAM_HYBRID_SLEEP_DISABLE);
}

static const bflb_ef_ctrl_com_trim_cfg_t trim_list[] = {
    {
        .name = "auadc_gain_ext",
        .en_addr = 0x6C * 8 + 24,
        .parity_addr = 0x6C * 8 + 25,
        .value_addr = 0x6C * 8 + 0,
        .value_len = 24,
    },
    {
        .name = "auadc_offset",
        .en_addr = 0x70 * 8 + 24,
        .parity_addr = 0x70 * 8 + 25,
        .value_addr = 0x70 * 8 + 0,
        .value_len = 24,
    },
    {
        .name = "usb20",
        .en_addr = 0x74 * 8 + 30,
        .parity_addr = 0x74 * 8 + 31,
        .value_addr = 0x74 * 8 + 23,
        .value_len = 7,
    },
    {
        .name = "dcdc12_vout",
        .en_addr = 0x74 * 8 + 21,
        .parity_addr = 0x74 * 8 + 22,
        .value_addr = 0x74 * 8 + 17,
        .value_len = 4,
    },
    {
        .name = "rc32k",
        .en_addr = 0x74 * 8 + 15,
        .parity_addr = 0x74 * 8 + 16,
        .value_addr = 0x74 * 8 + 11,
        .value_len = 4,
    },
    {
        .name = "rcal_iptat",
        .en_addr = 0x74 * 8 + 9,
        .parity_addr = 0x74 * 8 + 10,
        .value_addr = 0x74 * 8 + 4,
        .value_len = 5,
    },
    {
        .name = "gpadc2_offset",
        .en_addr = 0x78 * 8 + 16,
        .parity_addr = 0x78 * 8 + 17,
        .value_addr = 0x78 * 8 + 0,
        .value_len = 16,
    },
    {
        .name = "psram_trim",
        .en_addr = EFUSE_TRIM_PSRAM_OFFSET * 8 + 12,
        .parity_addr = EFUSE_TRIM_PSRAM_OFFSET * 8 + 11,
        .value_addr = EFUSE_TRIM_PSRAM_OFFSET * 8 + 0,
        .value_len = 11,
    },
    {
        .name = "ldo09",
        .en_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 30,
        .parity_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 31,
        .value_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 26,
        .value_len = 4,
    },
    {
        .name = "ldo08",
        .en_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 24,
        .parity_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 25,
        .value_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 20,
        .value_len = 4,
    },
    {
        .name = "ldo18",
        .en_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 18,
        .parity_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 19,
        .value_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 14,
        .value_len = 4,
    },
    {
        .name = "rc32m",
        .en_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 12,
        .parity_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 13,
        .value_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 4,
        .value_len = 8,
    },
    {
        .name = "xtal0",
        .en_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 30,
        .parity_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 31,
        .value_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 22,
        .value_len = 8,
    },
    {
        .name = "xtal1",
        .en_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 20,
        .parity_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 21,
        .value_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 12,
        .value_len = 8,
    },
    {
        .name = "xtal2",
        .en_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 10,
        .parity_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 11,
        .value_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 2,
        .value_len = 8,
    },
    {
        .name = "tsen",
        .en_addr = 0xD0 * 8 + 30,
        .parity_addr = 0xD0 * 8 + 31,
        .value_addr = 0xD0 * 8 + 18,
        .value_len = 12,
    },
    {
        .name = "gpadc_gain",
        .en_addr = 0xD0 * 8 + 16,
        .parity_addr = 0xD0 * 8 + 17,
        .value_addr = 0xD0 * 8 + 4,
        .value_len = 12,
    },
    {
        .name = "gpadc_vref",
        .en_addr = 0xD4 * 8 + 30,
        .parity_addr = 0xD4 * 8 + 31,
        .value_addr = 0xD4 * 8 + 24,
        .value_len = 6,
    },
    {
        .name = "gpadc_offset",
        .en_addr = 0xD4 * 8 + 16,
        .parity_addr = 0xD4 * 8 + 17,
        .value_addr = 0xD4 * 8 + 0,
        .value_len = 16,
    },
    {
        .name = "gpadc2_gain",
        .en_addr = 0xD8 * 8 + 30,
        .parity_addr = 0xD8 * 8 + 31,
        .value_addr = 0xD8 * 8 + 18,
        .value_len = 12,
    },
    {
        .name = "gpadc2_vref",
        .en_addr = 0xD8 * 8 + 16,
        .parity_addr = 0xD8 * 8 + 17,
        .value_addr = 0xD8 * 8 + 10,
        .value_len = 6,
    },
    {
        .name = "rcal_icx",
        .en_addr = 0xD8 * 8 + 8,
        .parity_addr = 0xD8 * 8 + 9,
        .value_addr = 0xD8 * 8 + 2,
        .value_len = 6,
    },
    {
        .name = "auadc_gain_int",
        .en_addr = 0xDC * 8 + 24,
        .parity_addr = 0xDC * 8 + 25,
        .value_addr = 0xDC * 8 + 0,
        .value_len = 24,
    }
};

BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo09_Soc_Slow_Pulldown(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_LDO09SOC);

    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_LDO09SOC_SLOW_PLD_AON);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO09SOC_SLOW_PLD_AON);
    }

    BL_WR_REG(AON_BASE, AON_LDO09SOC, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION HBN_Enable_Dcdc09(uint8_t gpio)
{
    uint32_t tmpVal;
    uint32_t pwrReqMask;
    uint32_t pwrReqEn;

    CHECK_PARAM((gpio <= 7));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_2);

    pwrReqMask = BL_GET_REG_BITS_VAL(tmpVal, HBN_GPIO_PWR_REQ_MASK);
    pwrReqEn = BL_GET_REG_BITS_VAL(tmpVal, HBN_GPIO_PWR_REQ_EN);

    pwrReqMask &= ~(1U << gpio);
    pwrReqEn |= (1U << gpio);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_PWR_REQ_MASK, pwrReqMask);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_PWR_REQ_EN, pwrReqEn);

    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_2, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo09_Aon_Force_Off(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_2);

    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_LDO09_AON_4S0);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_LDO09_AON_4S0);
    }

    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_2, tmpVal);

    return SUCCESS;
}

uint32_t bflb_ef_ctrl_get_common_trim_list(const bflb_ef_ctrl_com_trim_cfg_t **ptrim_list)
{
    *ptrim_list = &trim_list[0];
    return sizeof(trim_list) / sizeof(trim_list[0]);
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
    uint32_t regval;

    regval = getreg32(WHO_AM_I);
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
        bflb_ef_ctrl_write_direct(NULL, 0x14, &tmpval, 1, program);
    } else if (slot == 1) {
        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_write_direct(NULL, 0xC0, &tmpval, 1, program);
        }else
        {
            bflb_ef_ctrl_write_direct(NULL, 0x110, &tmpval, 1, program);
        }
    } else if (slot == 2) {
        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_write_direct(NULL, 0xC8, &tmpval, 1, program);
        }else
        {
            bflb_ef_ctrl_write_direct(NULL, 0x118, &tmpval, 1, program);
        }
    }

    /* The high 16 bits */
    tmpval = machigh[0] + (machigh[1] << 8);
    cnt = 0;

    for (i = 0; i < 6; i++) {
        cnt += bflb_ef_ctrl_get_byte_zero_cnt(mac[i]);
    }

    tmpval |= ((cnt & 0x3f) << 16);

    if (slot == 0) {
        bflb_ef_ctrl_write_direct(NULL, 0x18, &tmpval, 1, program);
    } else if (slot == 1) {
        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_write_direct(NULL, 0xC4, &tmpval, 1, program);
        }else
        {
            bflb_ef_ctrl_write_direct(NULL, 0x114, &tmpval, 1, program);
        }
    } else if (slot == 2) {
        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_write_direct(NULL, 0xCC, &tmpval, 1, program);
        }else
        {
            bflb_ef_ctrl_write_direct(NULL, 0x11C, &tmpval, 1, program);
        }
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  Trim LDO_SOC According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Trim_Ldo_Soc_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo09", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_LDO09SOC);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO09SOC_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_LDO09SOC, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim Ldo08_Aon According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Trim_Ldo08_Aon_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo08", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO08AON_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_0, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim Ldo18_Aon According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Trim_Ldo18_Aon_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo18", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18AON_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_0, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim RC32M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Trim_RC32M(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "rc32m", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 8)) {
            tmpVal = BL_RD_REG(PDS_BASE, PDS_RC32M_CTRL0);
            tmpVal = BL_SET_REG_BIT(tmpVal, PDS_RC32M_EXT_CODE_EN);
            BL_WR_REG(PDS_BASE, PDS_RC32M_CTRL0, tmpVal);
            arch_delay_us(2);
            tmpVal = BL_RD_REG(AON_BASE, AON_RC32M_CTRL2_AON);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_RC32M_CODE_FR_EXT2, trim.value);
            BL_WR_REG(AON_BASE, AON_RC32M_CTRL2_AON, tmpVal);
            tmpVal = BL_RD_REG(AON_BASE, AON_RC32M_CTRL2_AON);
            tmpVal = BL_SET_REG_BIT(tmpVal, AON_RC32M_EXT_CODE_SEL);
            BL_WR_REG(AON_BASE, AON_RC32M_CTRL2_AON, tmpVal);
            /* hw_5T + sw_5T  */
            arch_delay_us(1);
            return SUCCESS;
        }
    }

    return ERROR;
}

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

/****************************************************************************/ /**
 * @brief  Efuse set AES key read/write lock
 *
 * @param  key_index: AES key index
 * @param  rd_lock: read lock enable
 * @param  wr_lock: write lock enable
 *
 * @return 0 for success, -1 for unsupported key index
 *
*******************************************************************************/
int bflb_efuse_rw_lock_aes_key(uint8_t key_index, uint8_t rd_lock, uint8_t wr_lock)
{
    uint32_t tmpval = 0;

    if(0 == key_index){
        if(wr_lock){
            tmpval |= (1 << 17);
        }
        if(rd_lock){
            tmpval |= (1 << 27);
        }
        bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
    }else if(1 == key_index){
        if(wr_lock){
            tmpval |= (1 << 18);
        }
        if(rd_lock){
            tmpval |= (1 << 28);
        }
        bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
    }else if(2 == key_index){
         if(wr_lock){
            tmpval |= (1 << 19);
        }
        if(rd_lock){
            tmpval |= (1 << 29);
        }
        bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
    }else if(3 == key_index){
         if(wr_lock){
            tmpval |= (1 << 20);
        }
        if(rd_lock){
            tmpval |= (1 << 30);
        }
        bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
    }else if(4 == key_index){
        if(wr_lock){
            tmpval |= (1 << 15);
        }
        if(rd_lock){
            tmpval |= (1 << 25);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else if(5 == key_index){
         if(wr_lock){
            tmpval |= (1 << 16);
        }
        if(rd_lock){
            tmpval |= (1 << 26);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else if(6 == key_index){
         if(wr_lock){
            tmpval |= (1 << 17);
        }
        if(rd_lock){
            tmpval |= (1 << 27);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else if(7 == key_index){
         if(wr_lock){
            tmpval |= (1 << 18);
        }
        if(rd_lock){
            tmpval |= (1 << 28);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else if(10 == key_index){
         if(wr_lock){
            tmpval |= (1 << 21);
        }
        if(rd_lock){
            tmpval |= (1 << 31);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else{
        return -1;
    }
    return 0;
}

void Tzc_Sec_Set_CPU_Group(uint8_t cpu, uint8_t group)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = BL_RD_REG(TZ1_BASE, TZC_SEC_TZC_BMX_TZMID);
    tmpVal2 = BL_RD_REG(TZ1_BASE, TZC_SEC_TZC_BMX_TZMID_LOCK);

    if (cpu == GLB_CORE_ID_AP) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_CPU_TZMID, group);
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, TZC_SEC_TZC_CPU_TZMID_LOCK, 1);
    } else if (cpu == GLB_CORE_ID_NP) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_WMCU_TZMID, group);
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, TZC_SEC_TZC_WMCU_TZMID_LOCK, 1);
    } else if (cpu == GLB_CORE_ID_LP) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_PICO_TZMID, group);
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, TZC_SEC_TZC_PICO_TZMID_LOCK, 1);
    }

    BL_WR_REG(TZ1_BASE, TZC_SEC_TZC_BMX_TZMID, tmpVal);
    BL_WR_REG(TZ1_BASE, TZC_SEC_TZC_BMX_TZMID_LOCK, tmpVal2);
}

static inline uint32_t Clock_SPI3_Clk_Mux_Output(uint8_t sel)
{
    uint32_t tmpVal;
    uint8_t muxpll_160m_sel;

    if (sel == 0) {
        /* muxpll 160MHz */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);
        muxpll_160m_sel = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_TOP_MUXPLL_160M_SEL);

        if (muxpll_160m_sel == 0) {
            return 160 * 1000 * 1000;
        } else {
            return 0;
        }
    } else {
        /* mini xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MINI_XCLK);
    }
}

/****************************************************************************/ /**

 * @brief  Get Peripheral1 Clock
 *
 * @param  type: Peripheral1 clock type
 *
 * @return Peripheral1 clock value
 *
*******************************************************************************/
uint32_t Clock_Peripheral_Clock_Get(BL_Peripheral_Type type)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    if (type == BL_PERIPHERAL_CLOCK_SPI3) {
        uint32_t tmpVal;
        uint8_t clk_sel;

        tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1);
        clk_sel = BL_GET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_SPI_CLK_SEL);
        div = BL_GET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_SPI_CLK_DIV);
        clock = Clock_SPI3_Clk_Mux_Output(clk_sel);
        
        return clock / (div + 1);
    } else {
        return RomDriver_Clock_Peripheral_Clock_Get(type);
    }
}

uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t idx)
{
    if (type == BFLB_DEVICE_TYPE_SPI && idx == 3) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_SPI3);
    } else {
        return RomDriver_bflb_clk_get_peripheral_clock(type, idx);
    }
}

/****************************************************************************/ /**
 * @brief      delay us
 *
 * @param[in]  core:  systemcoreclock
 *
 * @param[in]  cnt:  delay cnt us
 *
 * @return none
 *
 *******************************************************************************/
#ifdef ARCH_RISCV
__WEAK
void ATTR_TCM_SECTION ASM_Delay_Us(uint32_t core, uint32_t cnt, uint32_t loopT)
{
    volatile uint32_t divVal = loopT;
    volatile uint32_t speed = 0;
    volatile uint32_t cycNum = 0;

    /* 1M=100K*10, so multiple is 10 */
    /* loop function take 4 instructions, so instructionNum is 4 */
    /* divVal = multiple*instructionNum */

    if (core >= 1 * 1000 * 1000) {
        /* CPU clock >= 1MHz */
        speed = core / (1000 * 1000);
        cycNum = speed * cnt;
        cycNum = cycNum / divVal;
        /* cycNum >= 0 */
    } else {
        /* CPU clock < 1MHz */
        speed = core / 1000;
        cycNum = speed * cnt;
        cycNum = cycNum / 1000;
        cycNum = cycNum / divVal;
        /* cycNum >= 0 */
    }

    if (!cycNum) {
        return;
    }

    __asm__ __volatile__(
        "mv       a4,%0\n\t"
        "li       a5,0x0\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        ".align 4\n\t"
        "1  :\n"
#if defined(CPU_MODEL_A0)
        "beq      a5,a4,2f\n\t"
        "addi     a5,a5,0x1\n\t"
        "j        1b\n\t"
#else
        "addi     a5,a5,0x1\n\t"
        "bne      a5,a4,1b\n\t"
#endif
        "nop\n\t"
        "nop\n\t"
        "2   :\n\t"
        "nop\n"
        :             /* output */
        : "r"(cycNum) /* input */
        : "a4", "a5"  /* destruct description */
    );
}
#endif

/****************************************************************************/ /**
 * @brief      delay us
 *
 * @param[in]  cnt:  delay cnt us
 *
 * @return none
 *
 *******************************************************************************/
__WEAK
void ATTR_TCM_SECTION arch_delay_us(uint32_t cnt)
{
    uint32_t coreFreq;
#if defined(CPU_MODEL_A0)
    uint32_t loopTick = 3;
#else
    uint32_t loopTick = 1;
#endif
#if defined(__riscv_xthead) || defined(__riscv_xtheadc)
#if ((__ICACHE_PRESENT == 1U) && (__DCACHE_PRESENT == 1U))
    uint32_t iCacheEn;
    uint32_t dCacheEn;

    /* requirement: icache enable && dcache enable */
    /* otherwise the latency depends on the code address */
    iCacheEn = (__get_MHCR() & CACHE_MHCR_IE_Msk) >> CACHE_MHCR_IE_Pos;
    dCacheEn = (__get_MHCR() & CACHE_MHCR_DE_Msk) >> CACHE_MHCR_DE_Pos;

#if defined(CPU_MODEL_A0)
    if (iCacheEn && dCacheEn) {
        loopTick = 3;
    } else if (iCacheEn && !dCacheEn) {
        loopTick = 4;
    } else if (!iCacheEn && !dCacheEn) {
        loopTick = 24;
    } else {
        /* not recommend */
    }
#else
    if (iCacheEn && dCacheEn) {
        loopTick = 1;
    } else if (iCacheEn && !dCacheEn) {
        loopTick = 4;
    } else if (!iCacheEn && !dCacheEn) {
        loopTick = 24;
    } else {
        /* not recommend */
    }
#endif
#endif
#endif

#if defined(DUAL_CORE)
    GLB_CORE_ID_Type coreID;
    coreID = GLB_Get_Core_Type();
    switch (coreID) {
        case GLB_CORE_ID_AP:
#endif
            coreFreq = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK);
#if defined(DUAL_CORE)
            break;
        default:
            coreFreq = 32 * 1000 * 1000;
            break;
    }
#endif

    coreFreq = coreFreq ? coreFreq : (32 * 1000 * 1000);

    ASM_Delay_Us(coreFreq, cnt, loopTick);
}

/****************************************************************************/ /**
 * @brief      delay ms
 *
 * @param[in]  cnt:  delay cnt ms
 *
 * @return none
 *
 *******************************************************************************/
__WEAK
void ATTR_TCM_SECTION arch_delay_ms(uint32_t cnt)
{
    uint32_t i = 0;
    uint32_t count = 0;

    if (cnt >= 1024) {
        /* delay (n*1024) ms */
        for (i = 0; i < (cnt / 1024); i++) {
            arch_delay_us(1024 * 1000);
        }
    }

    count = cnt & 0x3FF;

    if (count) {
        /* delay (1-1023)ms */
        arch_delay_us(count * 1000);
    }
}

/****************************************************************************/ /**
 * @brief  GLB set CPUPLL to 632.2176M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_CPUPLL_To_632P2176M_By_Sdmin(void)
{
    uint32_t tmpVal, sdmin;
    uint8_t xtalType;

    if (HBN_Get_Xtal_Type(&xtalType) == SUCCESS) {
        switch (xtalType) {
            case GLB_XTAL_24M: sdmin = 0x1A57A; break;
            case GLB_XTAL_32M: sdmin = 0x27837; break;
            case GLB_XTAL_52M: sdmin = 0x1850E; break;
            case GLB_XTAL_40M: sdmin = 0x1F9C6; break;
            case GLB_XTAL_26M: sdmin = 0x1850E; break;
            case GLB_XTAL_RC32M: sdmin = 0x27837; break;
            default: sdmin = 0x1F9C6; break;
        }
    } else {
        sdmin = 0x1F9C6;
    }

    /* set cpupll_sdmin */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_SDM_IN, sdmin);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB set CPUPLL to 638.976M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_CPUPLL_To_638P976M_By_Sdmin(void)
{
    uint32_t tmpVal, sdmin;
    uint8_t xtalType;

    if (HBN_Get_Xtal_Type(&xtalType) == SUCCESS) {
        switch (xtalType) {
            case GLB_XTAL_24M: sdmin = 0x1A9FB; break;
            case GLB_XTAL_32M: sdmin = 0x27EF9; break;
            case GLB_XTAL_52M: sdmin = 0x18937; break;
            case GLB_XTAL_40M: sdmin = 0x1FF2E; break;
            case GLB_XTAL_26M: sdmin = 0x18937; break;
            case GLB_XTAL_RC32M: sdmin = 0x27EF9; break;
            default: sdmin = 0x1FF2E; break;
        }
    } else {
        sdmin = 0x1FF2E;
    }

    /* set cpupll_sdmin */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_SDM_IN, sdmin);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Set All WRAM(320KB) Retention
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_WRAM_Retention(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_RET, 0xFFF);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    return SUCCESS;
}