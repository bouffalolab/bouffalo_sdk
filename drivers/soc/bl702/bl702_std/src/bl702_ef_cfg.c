/**
  ******************************************************************************
  * @file    bl702_ef_ctrl.c
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
#include "bl702_ef_cfg.h"
#include "bl702_glb.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SEC_EF_CTRL
 *  @{
 */

/** @defgroup  SEC_EF_CTRL_Private_Macros
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Private_Macros */

/** @defgroup  SEC_EF_CTRL_Private_Types
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Private_Types */

/** @defgroup  SEC_EF_CTRL_Private_Variables
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

#define EF_CTRL_DEVICE_INFO_OFFSET 0x74
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
/* 0x5C : ef_key_slot_4_w0 */
#define EF_DATA_EF_KEY_SLOT_4_W0_OFFSET (0x5C)
/* 0x60 : ef_key_slot_4_w1 */
#define EF_DATA_EF_KEY_SLOT_4_W1_OFFSET (0x60)
/* 0x64 : ef_key_slot_4_w2 */
#define EF_DATA_EF_KEY_SLOT_4_W2_OFFSET (0x64)
/* 0x68 : ef_key_slot_4_w3 */
#define EF_DATA_EF_KEY_SLOT_4_W3_OFFSET (0x68)
/* 0x6C : ef_key_slot_5_w0 */
#define EF_DATA_EF_KEY_SLOT_5_W0_OFFSET (0x6C)
/* 0x70 : ef_key_slot_5_w1 */
#define EF_DATA_EF_KEY_SLOT_5_W1_OFFSET (0x70)
/* 0x74 : ef_key_slot_5_w2 */
#define EF_DATA_EF_KEY_SLOT_5_W2_OFFSET (0x74)
/* 0x78 : ef_key_slot_5_w3 */
#define EF_DATA_EF_KEY_SLOT_5_W3_OFFSET (0x78)

static GLB_ROOT_CLK_Type rtClk;
static uint8_t bdiv, hdiv;

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
 * @brief  Efuse read write switch clock save
 *
 * @param  deviceInfo: info pointer
 *
 * @return None
 *
*******************************************************************************/
void  ATTR_TCM_SECTION bflb_efuse_switch_cpu_clock_save(void)
{
    /* all API should be place at tcm section */
    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
    rtClk = GLB_Get_Root_CLK_Sel();
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);
    GLB_Set_System_CLK_Div(0, 0);
}

/****************************************************************************/ /**
 * @brief  Efuse read write switch clock restore
 *
 * @param  deviceInfo: info pointer
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bflb_efuse_switch_cpu_clock_restore(void)
{
    /* all API should be place at tcm section */
    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
}

/****************************************************************************/ /**
 * @brief  Efuse read device info
 *
 * @param  deviceInfo: info pointer
 *
 * @return None
 *
*******************************************************************************/
void bflb_ef_ctrl_get_device_info(bflb_efuse_device_info_type *deviceInfo)
{
    uint32_t *p = (uint32_t *)deviceInfo;

    bflb_ef_ctrl_read_direct(NULL, EF_CTRL_DEVICE_INFO_OFFSET, p, 1, 1);
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

/*@} end of group SEC_EF_CTRL_Public_Functions */

/*@} end of group SEC_EF_CTRL */

/*@} end of group BL702_Peripheral_Driver */
