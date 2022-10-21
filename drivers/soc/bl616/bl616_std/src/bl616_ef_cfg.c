/**
  ******************************************************************************
  * @file    bl616_ef_ctrl.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
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

#include "string.h"
#include "bl616_ef_ctrl.h"
#include "bl616_ef_cfg.h"
#include "ef_data_reg.h"

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SEC_EF_CTRL
 *  @{
 */

/** @defgroup  SEC_EF_CTRL_Private_Macros
 *  @{
 */
#define EF_CTRL_LOAD_BEFORE_READ_R0 EF_Ctrl_Load_Efuse_R0()

/*@} end of group SEC_EF_CTRL_Private_Macros */

/** @defgroup  SEC_EF_CTRL_Private_Types
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Private_Types */

/** @defgroup  SEC_EF_CTRL_Private_Variables
 *  @{
 */
static Efuse_Common_Trim_Cfg trim_lit[] = {
    {
        .name = "ldo15",
        .en_addr = 0x68 * 8 + 31,
        .parity_addr = 0x68 * 8 + 30,
        .value_addr = 0x68 * 8 + 27,
        .value_len = 3,
    },
    {
        .name = "iptat",
        .en_addr = 0x74 * 8 + 31,
        .parity_addr = 0x74 * 8 + 30,
        .value_addr = 0x68 * 8 + 22,
        .value_len = 5,
    },
    {
        .name = "icx",
        .en_addr = 0x74 * 8 + 29,
        .parity_addr = 0x74 * 8 + 28,
        .value_addr = 0x74 * 8 + 22,
        .value_len = 6,
    },
    {
        .name = "dcdc_trim",
        .en_addr = 0x78 * 8 + 31,
        .parity_addr = 0x78 * 8 + 30,
        .value_addr = 0x78 * 8 + 26,
        .value_len = 4,
    },
    {
        .name = "ldo18_sel",
        .en_addr = 0x78 * 8 + 25,
        .parity_addr = 0x78 * 8 + 24,
        .value_addr = 0x78 * 8 + 20,
        .value_len = 4,
    },
    {
        .name = "ldo18_trim",
        .en_addr = 0x78 * 8 + 19,
        .parity_addr = 0x78 * 8 + 18,
        .value_addr = 0x78 * 8 + 14,
        .value_len = 4,
    },
    {
        .name = "ldo33",
        .en_addr = 0x78 * 8 + 13,
        .parity_addr = 0x78 * 8 + 12,
        .value_addr = 0x78 * 8 + 8,
        .value_len = 4,
    },
    {
        .name = "ldo11",
        .en_addr = 0x78 * 8 + 7,
        .parity_addr = 0x78 * 8 + 6,
        .value_addr = 0x78 * 8 + 2,
        .value_len = 4,
    },
    {
        .name = "rc32m",
        .en_addr = 0x78 * 8 + 1,
        .parity_addr = 0x78 * 8 + 0,
        .value_addr = 0x7C * 8 + 4,
        .value_len = 8,
    },
    {
        .name = "auadc_gain",
        .en_addr = 0xDC * 8 + 25,
        .parity_addr = 0xDC * 8 + 24,
        .value_addr = 0xDC * 8 + 0,
        .value_len = 24,
    },
    {
        .name = "auadc_offset",
        .en_addr = 0xE0 * 8 + 25,
        .parity_addr = 0xE0 * 8 + 24,
        .value_addr = 0xE0 * 8 + 0,
        .value_len = 24,
    },
    {
        .name = "psram_trim",
        .en_addr = 0xE8 * 8 + 12,
        .parity_addr = 0xE8 * 8 + 11,
        .value_addr = 0xE8 * 8 + 0,
        .value_len = 11,
    },
    {
        .name = "rc32k",
        .en_addr = 0xEC * 8 + 19,
        .parity_addr = 0xEC * 8 + 18,
        .value_addr = 0xEC * 8 + 8,
        .value_len = 10,
    },
    {
        .name = "xtal1",
        .en_addr = 0xEC * 8 + 7,
        .parity_addr = 0xEC * 8 + 6,
        .value_addr = 0xEC * 8 + 0,
        .value_len = 6,
    },
    {
        .name = "xtal2",
        .en_addr = 0xF0 * 8 + 31,
        .parity_addr = 0xF0 * 8 + 30,
        .value_addr = 0xF4 * 8 + 26,
        .value_len = 6,
    },
    {
        .name = "xtal3",
        .en_addr = 0xF0 * 8 + 29,
        .parity_addr = 0xF0 * 8 + 28,
        .value_addr = 0xF4 * 8 + 20,
        .value_len = 6,
    },
    {
        .name = "gpadc_gain",
        .en_addr = 0xF0 * 8 + 27,
        .parity_addr = 0xF0 * 8 + 26,
        .value_addr = 0xF0 * 8 + 14,
        .value_len = 12,
    },
    {
        .name = "tsen",
        .en_addr = 0xF0 * 8 + 13,
        .parity_addr = 0xF0 * 8 + 12,
        .value_addr = 0xF0 * 8 + 0,
        .value_len = 12,
    },
    {
        .name = "dcdc_dis",
        .en_addr = 0xF4 * 8 + 19,
        .parity_addr = 0xF4 * 8 + 18,
        .value_addr = 0xF4 * 8 + 17,
        .value_len = 1,
    },
    {
        .name = "dcdc_vout",
        .en_addr = 0xF4 * 8 + 16,
        .parity_addr = 0xF4 * 8 + 15,
        .value_addr = 0xF4 * 8 + 10,
        .value_len = 5,
    },
    {
        .name = "ldo18_bypass",
        .en_addr = 0xF4 * 8 + 9,
        .parity_addr = 0xF4 * 8 + 8,
        .value_addr = 0xF4 * 8 + 4,
        .value_len = 1,
    },
    {
        .name = "usb20",
        .en_addr = 0xF8 * 8 + 15,
        .parity_addr = 0xF8 * 8 + 14,
        .value_addr = 0xF8 * 8 + 8,
        .value_len = 6,
    }
};
/*@} end of group SEC_EF_CTRL_Private_Variables */

/** @defgroup  SEC_EF_CTRL_Global_Variables
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Global_Variables */

/** @defgroup  SEC_EF_CTRL_Private_Fun_Declaration
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Private_Fun_Declaration */

/** @defgroup  SEC_EF_CTRL_Private_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Efuse get zero bit count
 *
 * @param  val: Value to count
 *
 * @return Zero bit count
 *
*******************************************************************************/
static uint32_t EF_Cfg_Get_Byte_Zero_Cnt(uint8_t val)
{
    uint32_t cnt = 0;
    uint32_t i = 0;

    for (i = 0; i < 8; i++) {
        if ((val & (1 << i)) == 0) {
            cnt += 1;
        }
    }

    return cnt;
}

/****************************************************************************/ /**
 * @brief  Efuse read device info
 *
 * @param  deviceInfo: info pointer
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Device_Info(Efuse_Device_Info_Type *deviceInfo)
{
    uint32_t tmpVal;
    uint32_t *p = (uint32_t *)deviceInfo;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_WIFI_MAC_HIGH);
    *p = tmpVal;
}

/****************************************************************************/ /**
 * @brief  Efuse get trim list
 *
 * @param  trim_list: Trim list pointer
 *
 * @return Trim list count
 *
*******************************************************************************/
uint32_t EF_Ctrl_Get_Common_Trim_List(Efuse_Common_Trim_Cfg **trim_list)
{
    *trim_list = &trim_lit[0];
    return sizeof(trim_lit) / sizeof(trim_lit[0]);
}

/****************************************************************************/ /**
 * @brief  Efuse read common trim value
 *
 * @param  name: Trim name
 * @param  trim: Trim data pointer
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Common_Trim(char *name, Efuse_Common_Trim_Type *trim)
{
    uint32_t tmpVal;
    uint32_t i = 0;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    trim->en = 0;
    trim->parity = 0;
    trim->empty = 1;
    trim->len = 0;

    for (i = 0; i < sizeof(trim_lit) / sizeof(trim_lit[0]); i++) {
        if (memcmp(name, trim_lit[i].name, strlen(name)) == 0) {
            trim->len = trim_lit[i].value_len;
            tmpVal = BL_RD_WORD(EF_DATA_BASE + (trim_lit[i].en_addr / 32) * 4);
            if (tmpVal & (1 << (trim_lit[i].en_addr % 32))) {
                trim->en = 1;
            }
            tmpVal = BL_RD_WORD(EF_DATA_BASE + (trim_lit[i].parity_addr / 32) * 4);
            if (tmpVal & (1 << (trim_lit[i].parity_addr % 32))) {
                trim->parity = 1;
            }
            tmpVal = BL_RD_WORD(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
            tmpVal = tmpVal >> (trim_lit[i].value_addr % 32);
            trim->value = tmpVal & ((1 << trim_lit[i].value_len) - 1);
            if (trim->en == 0 && trim->parity == 0 && trim->value == 0) {
                trim->empty = 1;
            } else {
                trim->empty = 0;
            }
        }
    }
}

/****************************************************************************/ /**
 * @brief  Efuse write common trim value
 *
 * @param  name: Trim name
 * @param  trim: Trim data pointer
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Write_Common_Trim(char *name, uint32_t value, uint8_t program)
{
    uint32_t tmpVal;
    uint32_t i = 0;
    uint8_t parity = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    for (i = 0; i < sizeof(trim_lit) / sizeof(trim_lit[0]); i++) {
        if (memcmp(name, trim_lit[i].name, strlen(name)) == 0) {
            tmpVal = BL_RD_WORD(EF_DATA_BASE + (trim_lit[i].en_addr / 32) * 4);
            tmpVal |= (1 << (trim_lit[i].en_addr % 32));
            BL_WR_WORD(EF_DATA_BASE + (trim_lit[i].en_addr / 32) * 4, tmpVal);


            parity = EF_Ctrl_Get_Trim_Parity(value, trim_lit[i].value_len);
            if (parity) {
                tmpVal = BL_RD_WORD(EF_DATA_BASE + (trim_lit[i].parity_addr / 32) * 4);
                tmpVal |= (1 << (trim_lit[i].parity_addr % 32));
                BL_WR_WORD(EF_DATA_BASE + (trim_lit[i].parity_addr / 32) * 4, tmpVal);
            }

            tmpVal = BL_RD_WORD(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4);
            tmpVal |= (value << (trim_lit[i].value_addr % 32));
            BL_WR_WORD(EF_DATA_BASE + (trim_lit[i].value_addr / 32) * 4, tmpVal);
        }
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Whether MAC address slot is empty
 *
 * @param  slot: MAC address slot
 * @param  reload: whether  reload to check
 *
 * @return 0 for all slots full,1 for others
 *
*******************************************************************************/
uint8_t EF_Ctrl_Is_MAC_Address_Slot_Empty(uint8_t slot, uint8_t reload)
{
    uint32_t tmp1 = 0xffffffff, tmp2 = 0xffffffff;
    uint32_t part1Empty = 0, part2Empty = 0;

    if (slot == 0) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_WIFI_MAC_LOW);
        tmp2 = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_WIFI_MAC_HIGH);
    } else if (slot == 1) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_SW_USAGE_2);
        tmp2 = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_SW_USAGE_3);
    } else if (slot == 2) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_KEY_SLOT_11_W1);
        tmp2 = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_KEY_SLOT_11_W2);
    }

    part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 0, 32));
    part2Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp2, 0, 22));

    return (part1Empty && part2Empty);
}

/****************************************************************************/ /**
 * @brief  Efuse write optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[6]: MAC address buffer
 * @param  program: Whether program
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Write_MAC_Address_Opt(uint8_t slot, uint8_t mac[6], uint8_t program)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal;
    uint32_t i = 0, cnt;

    if (slot >= 3) {
        return ERROR;
    }

    /* Change to local order */
    for (i = 0; i < 3; i++) {
        tmpVal = mac[i];
        mac[i] = mac[5 - i];
        mac[5 - i] = tmpVal;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* The low 32 bits */
    if (slot == 0) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_EF_WIFI_MAC_LOW, BL_RDWD_FRM_BYTEP(maclow));
    } else if (slot == 1) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_EF_SW_USAGE_2, BL_RDWD_FRM_BYTEP(maclow));
    } else if (slot == 2) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_EF_KEY_SLOT_11_W1, BL_RDWD_FRM_BYTEP(maclow));
    }

    /* The high 16 bits */
    tmpVal = machigh[0] + (machigh[1] << 8);
    cnt = 0;

    for (i = 0; i < 6; i++) {
        cnt += EF_Cfg_Get_Byte_Zero_Cnt(mac[i]);
    }

    tmpVal |= ((cnt & 0x3f) << 16);

    if (slot == 0) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_EF_WIFI_MAC_HIGH, tmpVal);
    } else if (slot == 1) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_EF_SW_USAGE_3, tmpVal);
    } else if (slot == 2) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_EF_KEY_SLOT_11_W2, tmpVal);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[6]: MAC address buffer
 * @param  reload: Whether reload
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Read_MAC_Address_Opt(uint8_t slot, uint8_t mac[6], uint8_t reload)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;

    if (slot >= 3) {
        return ERROR;
    }

    /* Trigger read data from efuse */
    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_WIFI_MAC_LOW);
    } else if (slot == 1) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_SW_USAGE_2);
    } else if (slot == 2) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_KEY_SLOT_11_W1);
    }

    BL_WRWD_TO_BYTEP(maclow, tmpVal);

    if (slot == 0) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_WIFI_MAC_HIGH);
    } else if (slot == 1) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_SW_USAGE_3);
    } else if (slot == 2) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_EF_KEY_SLOT_11_W2);
    }

    machigh[0] = tmpVal & 0xff;
    machigh[1] = (tmpVal >> 8) & 0xff;

    /* Check parity */
    for (i = 0; i < 6; i++) {
        cnt += EF_Cfg_Get_Byte_Zero_Cnt(mac[i]);
    }

    if ((cnt & 0x3f) == ((tmpVal >> 16) & 0x3f)) {
        /* Change to network order */
        for (i = 0; i < 3; i++) {
            tmpVal = mac[i];
            mac[i] = mac[5 - i];
            mac[5 - i] = tmpVal;
        }

        return SUCCESS;
    } else {
        return ERROR;
    }
}

/*@} end of group SEC_EF_CTRL_Public_Functions */

/*@} end of group SEC_EF_CTRL */

/*@} end of group BL616_Peripheral_Driver */
