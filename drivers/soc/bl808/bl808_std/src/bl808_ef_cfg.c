/**
  ******************************************************************************
  * @file    bl808_ef_cfg.c
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
#include "bflb_ef_ctrl.h"
#include "bl808_ef_cfg.h"

/** @addtogroup  BL808_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SEC_EF_CTRL
 *  @{
 */

/** @defgroup  SEC_EF_CTRL_Private_Macros
 *  @{
 */

static const bflb_ef_ctrl_com_trim_cfg trim_list[] = {
    {
        .name = "rc32m",
        .en_addr = 0x78 * 8 + 1,
        .parity_addr = 0x78 * 8 + 0,
        .value_addr = 0x7C * 8 + 4,
        .value_len = 8,
    },
    {
        .name = "rc32k",
        .en_addr = 0xEC * 8 + 19,
        .parity_addr = 0xEC * 8 + 18,
        .value_addr = 0xEC * 8 + 8,
        .value_len = 10,
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
    }
};

#define EF_DATA_EF_CFG_0_OFFSET (0x0)
/* 0x14 : ef_wifi_mac_low */
#define EF_DATA_EF_WIFI_MAC_LOW_OFFSET (0x14)
/* 0x18 : ef_wifi_mac_high */
#define EF_DATA_EF_WIFI_MAC_HIGH_OFFSET (0x18)
/* 0x1C : ef_key_slot_0_w0 */
#define EF_DATA_EF_KEY_SLOT_0_W0_OFFSET (0x1C)
/* 0x20 : ef_key_slot_0_w1 */
#define EF_DATA_EF_KEY_SLOT_0_W1_OFFSET (0x20)
/* 0x24 : ef_key_slot_0_w2 */
#define EF_DATA_EF_KEY_SLOT_0_W2_OFFSET (0x24)
/* 0x28 : ef_key_slot_0_w3 */
#define EF_DATA_EF_KEY_SLOT_0_W3_OFFSET (0x28)
/* 0x2C : ef_key_slot_1_w0 */
#define EF_DATA_EF_KEY_SLOT_1_W0_OFFSET (0x2C)
/* 0x30 : ef_key_slot_1_w1 */
#define EF_DATA_EF_KEY_SLOT_1_W1_OFFSET (0x30)
/* 0x34 : ef_key_slot_1_w2 */
#define EF_DATA_EF_KEY_SLOT_1_W2_OFFSET (0x34)
/* 0x38 : ef_key_slot_1_w3 */
#define EF_DATA_EF_KEY_SLOT_1_W3_OFFSET (0x38)
/* 0x3C : ef_key_slot_2_w0 */
#define EF_DATA_EF_KEY_SLOT_2_W0_OFFSET (0x3C)
/* 0x40 : ef_key_slot_2_w1 */
#define EF_DATA_EF_KEY_SLOT_2_W1_OFFSET (0x40)
/* 0x44 : ef_key_slot_2_w2 */
#define EF_DATA_EF_KEY_SLOT_2_W2_OFFSET (0x44)
/* 0x48 : ef_key_slot_2_w3 */
#define EF_DATA_EF_KEY_SLOT_2_W3_OFFSET (0x48)
/* 0x4C : ef_key_slot_3_w0 */
#define EF_DATA_EF_KEY_SLOT_3_W0_OFFSET (0x4C)
/* 0x50 : ef_key_slot_3_w1 */
#define EF_DATA_EF_KEY_SLOT_3_W1_OFFSET (0x50)
/* 0x54 : ef_key_slot_3_w2 */
#define EF_DATA_EF_KEY_SLOT_3_W2_OFFSET (0x54)
/* 0x58 : ef_key_slot_3_w3 */
#define EF_DATA_EF_KEY_SLOT_3_W3_OFFSET (0x58)
/* 0x5C : ef_sw_usage_0 */
#define EF_DATA_EF_SW_USAGE_0_OFFSET (0x5C)
/* 0x60 : ef_sw_usage_1 */
#define EF_DATA_EF_SW_USAGE_1_OFFSET (0x60)
/* 0x64 : ef_sw_usage_2 */
#define EF_DATA_EF_SW_USAGE_2_OFFSET (0x64)
/* 0x68 : ef_sw_usage_3 */
#define EF_DATA_EF_SW_USAGE_3_OFFSET (0x68)
/* 0x6C : ef_key_slot_11_w0 */
#define EF_DATA_EF_KEY_SLOT_11_W0_OFFSET (0x6C)
/* 0x70 : ef_key_slot_11_w1 */
#define EF_DATA_EF_KEY_SLOT_11_W1_OFFSET (0x70)
/* 0x74 : ef_key_slot_11_w2 */
#define EF_DATA_EF_KEY_SLOT_11_W2_OFFSET (0x74)
/* 0x78 : ef_key_slot_11_w3 */
#define EF_DATA_EF_KEY_SLOT_11_W3_OFFSET (0x78)

/*@} end of group SEC_EF_CTRL_Private_Macros */

/** @defgroup  SEC_EF_CTRL_Private_Types
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Private_Types */

/** @defgroup  SEC_EF_CTRL_Private_Variables
 *  @{
 */

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
 * @brief  Efuse read device info
 *
 * @param  deviceInfo: info pointer
 *
 * @return None
 *
*******************************************************************************/
void bflb_ef_ctrl_get_device_info(bflb_efuse_device_info_type *chipInfo)
{
    uint32_t tmpval;

    //tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_HIGH);
    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_WIFI_MAC_HIGH_OFFSET, &tmpval, 1, 1);

    chipInfo->chipInfo = (tmpval >> 29) & 0x7;
    chipInfo->memoryInfo = (tmpval >> 27) & 0x3;
    chipInfo->psramInfo = (tmpval >> 25) & 0x3;
    chipInfo->deviceInfo = (tmpval >> 22) & 0x7;

    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_CFG_0_OFFSET, &tmpval, 1, 1);

    chipInfo->psramInfo |= ((tmpval >> 20) & 0x1) << 2;
}

/****************************************************************************/ /**
 * @brief  Efuse get trim list
 *
 * @param  trim_list: Trim list pointer
 *
 * @return Trim list count
 *
*******************************************************************************/
uint32_t bflb_ef_ctrl_get_common_trim_list(const bflb_ef_ctrl_com_trim_cfg **ptrim_list)
{
    *ptrim_list = &trim_list[0];
    return sizeof(trim_list) / sizeof(trim_list[0]);
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
    uint32_t part1Empty = 0, part2Empty = 0;
#if 0
    uint32_t tmp1 = 0xffffffff, tmp2 = 0xffffffff;

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_WIFI_MAC_LOW_OFFSET,&tmp1,1,reload);
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_WIFI_MAC_HIGH_OFFSET,&tmp2,1,reload);
    } else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_SW_USAGE_2_OFFSET,&tmp1,1,reload);
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_SW_USAGE_3_OFFSET,&tmp2,1,reload);
    } else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_KEY_SLOT_11_W1_OFFSET,&tmp1,1,reload);
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_KEY_SLOT_11_W2_OFFSET,&tmp2,1,reload);
    }

    part1Empty = (bflb_ef_ctrl_is_all_bits_zero(tmp1, 0, 32));
    part2Empty = (bflb_ef_ctrl_is_all_bits_zero(tmp2, 0, 22));
#endif
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
#if 0
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpval;
    uint32_t i = 0, cnt;

    if (slot >= 3) {
        return ERROR;
    }

    /* Change to local order */
    for (i = 0; i < 3; i++) {
        tmpval = mac[i];
        mac[i] = mac[5 - i];
        mac[5 - i] = tmpval;
    }

    /* The low 32 bits */
    tmpval=BL_RDWD_FRM_BYTEP(maclow);

    if (slot == 0) {
        bflb_ef_ctrl_write_direct(NULL,EF_DATA_EF_WIFI_MAC_LOW_OFFSET,&tmpval,1,program);
    } else if (slot == 1) {
        bflb_ef_ctrl_write_direct(NULL,EF_DATA_EF_SW_USAGE_2_OFFSET,&tmpval,1,program);
    } else if (slot == 2) {
        bflb_ef_ctrl_write_direct(NULL,EF_DATA_EF_KEY_SLOT_11_W1_OFFSET,&tmpval,1,program);
    }

    /* The high 16 bits */
    tmpval = machigh[0] + (machigh[1] << 8);
    cnt = 0;

    for (i = 0; i < 6; i++) {
        cnt += bflb_ef_ctrl_get_byte_zero_cnt(mac[i]);
    }

    tmpval |= ((cnt & 0x3f) << 16);

    if (slot == 0) {
        bflb_ef_ctrl_write_direct(NULL,EF_DATA_EF_WIFI_MAC_HIGH_OFFSET,&tmpval,1,program);
    } else if (slot == 1) {
        bflb_ef_ctrl_write_direct(NULL,EF_DATA_EF_SW_USAGE_3_OFFSET,&tmpval,1,program);
    } else if (slot == 2) {
        bflb_ef_ctrl_write_direct(NULL,EF_DATA_EF_KEY_SLOT_11_W2_OFFSET,&tmpval,1,program);
    }
#endif
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
#if 0
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpval = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;

    if (slot >= 3) {
        return ERROR;
    }

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_WIFI_MAC_LOW_OFFSET,&tmpval,1,reload);
    } else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_SW_USAGE_2_OFFSET,&tmpval,1,reload);
    } else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_KEY_SLOT_11_W1_OFFSET,&tmpval,1,reload);
    }

    BL_WRWD_TO_BYTEP(maclow, tmpval);

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_WIFI_MAC_HIGH_OFFSET,&tmpval,1,reload);
    } else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_SW_USAGE_3_OFFSET,&tmpval,1,reload);
    } else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL,EF_DATA_EF_KEY_SLOT_11_W2_OFFSET,&tmpval,1,reload);
    }

    machigh[0] = tmpval & 0xff;
    machigh[1] = (tmpval >> 8) & 0xff;

    /* Check parity */
    for (i = 0; i < 6; i++) {
        cnt += bflb_ef_ctrl_get_byte_zero_cnt(mac[i]);
    }

    if ((cnt & 0x3f) == ((tmpval >> 16) & 0x3f)) {
        /* Change to network order */
        for (i = 0; i < 3; i++) {
            tmpval = mac[i];
            mac[i] = mac[5 - i];
            mac[5 - i] = tmpval;
        }
        return SUCCESS;
    } else {
        return ERROR;
    }
#endif
    return SUCCESS;
}

/*@} end of group SEC_EF_CTRL_Public_Functions */

/*@} end of group SEC_EF_CTRL */

/*@} end of group BL808_Peripheral_Driver */
