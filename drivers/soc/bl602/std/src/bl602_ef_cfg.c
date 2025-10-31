/**
  ******************************************************************************
  * @file    bl602_ef_cfg.c
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
#include "bflb_efuse.h"
#include "bl602_ef_cfg.h"
#include "bl602_glb.h"
#include "hardware/ef_data_reg.h"

static ATTR_TCM_CONST_SECTION bflb_ef_ctrl_com_trim_cfg_t trim_list[] = {
    {
        .name = "rc32m",
        .en_addr = 0x0C * 8 + 19,
        .parity_addr = 0x0C * 8 + 18,
        .value_addr = 0x0C * 8 + 10,
        .value_len = 8,
    },
    {
        .name = "rc32k",
        .en_addr = 0x0C * 8 + 31,
        .parity_addr = 0x0C * 8 + 30,
        .value_addr = 0x0C * 8 + 20,
        .value_len = 10,
    },
    {
        .name = "gpadc_gain",
        .en_addr = 0x78 * 8 + 14,
        .parity_addr = 0x78 * 8 + 13,
        .value_addr = 0x78 * 8 + 1,
        .value_len = 12,
    },
    {
        .name = "tsen",
        .en_addr = 0x78 * 8 + 0,
        .parity_addr = 0x7C * 8 + 12,
        .value_addr = 0x7C * 8 + 0,
        .value_len = 12,
    },
    {
        .name = "xtal0",
        .en_addr = 0x0C * 8 + 9,
        .parity_addr = 0x0C * 8 + 8,
        .value_addr = 0x0C * 8 + 2,
        .value_len = 6,
    },
    {
        .name = "xtal1",
        .en_addr = 0x70 * 8 + 29,
        .parity_addr = 0x70 * 8 + 28,
        .value_addr = 0x70 * 8 + 22,
        .value_len = 6,
    },
    {
        .name = "xtal2",
        .en_addr = 0x08 * 8 + 29,
        .parity_addr = 0x08 * 8 + 28,
        .value_addr = 0x08 * 8 + 22,
        .value_len = 6,
    },
    {
        .name = "key_enable_dac_dvdd",
        .en_addr = 0x58 * 8 + 16,
        .parity_addr = 0x58 * 8 + 15,
        .value_addr = 0x58 * 8 + 13,
        .value_len = 2,
    },
    {
        .name = "key_enable_ldo11",
        .en_addr = 0x58 * 8 + 12,
        .parity_addr = 0x58 * 8 + 11,
        .value_addr = 0x58 * 8 + 7,
        .value_len = 4,
    },
    {
        .name = "key_enable_tx_power_res",
        .en_addr = 0x58 * 8 + 6,
        .parity_addr = 0x58 * 8 + 5,
        .value_addr = 0x58 * 8 + 0,
        .value_len = 5,
    },
    {
        .name = "key_disable_dac_dvdd",
        .en_addr = 0x68 * 8 + 16,
        .parity_addr = 0x68 * 8 + 15,
        .value_addr = 0x68 * 8 + 13,
        .value_len = 2,
    },
    {
        .name = "key_disable_ldo11",
        .en_addr = 0x68 * 8 + 12,
        .parity_addr = 0x68 * 8 + 11,
        .value_addr = 0x68 * 8 + 7,
        .value_len = 4,
    },
    {
        .name = "key_disable_tx_power_res",
        .en_addr = 0x68 * 8 + 6,
        .parity_addr = 0x68 * 8 + 5,
        .value_addr = 0x68 * 8 + 0,
        .value_len = 5,
    }
};

static GLB_ROOT_CLK_Type rtClk;
static HBN_XCLK_CLK_Type xclk;
static uint8_t bdiv, hdiv;

/****************************************************************************/ /**
 * @brief  Efuse read write switch clock save
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bflb_efuse_switch_cpu_clock_save(void)
{
    /* all API should be place at tcm section */
    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
    rtClk = GLB_Get_Root_CLK_Sel();
    xclk = HBN_Get_XCLK_CLK_Sel();
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);
    GLB_Set_System_CLK_Div(0, 0);
}

/****************************************************************************/ /**
 * @brief  Efuse read write switch clock restore
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bflb_efuse_switch_cpu_clock_restore(void)
{
    /* all API should be place at tcm section */
    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
    HBN_Set_XCLK_CLK_Sel(xclk);
}

/****************************************************************************/ /**
 * @brief  Efuse get trim list
 *
 * @param  trim_list: Trim list pointer
 *
 * @return Trim list count
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION bflb_ef_ctrl_get_common_trim_list(const bflb_ef_ctrl_com_trim_cfg_t **ptrim_list)
{
    *ptrim_list = &trim_list[0];
    return sizeof(trim_list) / sizeof(trim_list[0]);
}

/****************************************************************************/ /**
 * @brief  Efuse read device info
 *
 * @param  device_info: info pointer
 *
 * @return None
 *
*******************************************************************************/
void bflb_efuse_get_device_info(bflb_efuse_device_info_type *device_info)
{
    uint32_t tmpval;

    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_WIFI_MAC_HIGH_OFFSET, &tmpval, 1, 1);
    device_info->ext_info = (tmpval >> 24) & 7;
    device_info->flash_info = (tmpval >> 27) & 0x3;
    device_info->mcu_info = (tmpval >> 30) & 0x1;
    device_info->package = (tmpval >> 31) & 0x3;

    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_CFG_0_OFFSET, &tmpval, 1, 1);
    device_info->version = (tmpval >> 8) & 0x7;

    switch (device_info->package) {
        case 0:
            device_info->package_name = "QFN32";
            break;
        case 1:
            device_info->package_name = "QFN40";
            break;
        default:
            device_info->package_name = "ERROR";
            break;
    }
    switch (device_info->flash_info) {
        case 0:
            device_info->flash_info_name = "NO";
            break;
        case 1:
            device_info->flash_info_name = "1MB";
            break;
        case 2:
            device_info->flash_info_name = "2MB";
            break;
        case 3:
            device_info->flash_info_name = "4MB";
            break;
        default:
            device_info->flash_info_name = "ERROR";
            break;
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read chip identification
 *
 * @param  chipid: id pointer
 *
 * @return 0 or -1
 *
*******************************************************************************/
int bflb_efuse_get_chipid(uint8_t chipid[8])
{
    chipid[6] = 0;
    chipid[7] = 0;

    return bflb_efuse_read_mac_address_opt(0, chipid, 1);
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
uint8_t bflb_efuse_is_mac_address_slot_empty(uint8_t slot, uint8_t reload)
{
    uint32_t tmp1 = 0xffffffff, tmp2 = 0xffffffff;
    uint32_t part1Empty = 0, part2Empty = 0;

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_WIFI_MAC_LOW_OFFSET, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_WIFI_MAC_HIGH_OFFSET, &tmp2, 1, reload);
    } else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W0_OFFSET, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W1_OFFSET, &tmp2, 1, reload);
    } else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_DBG_PWD_LOW_OFFSET, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_DBG_PWD_HIGH_OFFSET, &tmp2, 1, reload);
    }

    part1Empty = (bflb_ef_ctrl_is_all_bits_zero(tmp1, 0, 32));
    part2Empty = (bflb_ef_ctrl_is_all_bits_zero(tmp2, 0, 22));

    return (part1Empty && part2Empty);
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
        bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_WIFI_MAC_LOW_OFFSET, &tmpval, 1, program);
    } else if (slot == 1) {
        bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W0_OFFSET, &tmpval, 1, program);
    } else if (slot == 2) {
        bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_DBG_PWD_LOW_OFFSET, &tmpval, 1, program);
    }

    /* The high 16 bits */
    tmpval = machigh[0] + (machigh[1] << 8);
    cnt = 0;

    for (i = 0; i < 6; i++) {
        cnt += bflb_ef_ctrl_get_byte_zero_cnt(mac[i]);
    }

    tmpval |= ((cnt & 0x3f) << 16);

    if (slot == 0) {
        bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_WIFI_MAC_HIGH_OFFSET, &tmpval, 1, program);
    } else if (slot == 1) {
        bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W1_OFFSET, &tmpval, 1, program);
    } else if (slot == 2) {
        bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_DBG_PWD_HIGH_OFFSET, &tmpval, 1, program);
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  Efuse read optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[6]: MAC address buffer
 * @param  reload: Whether reload
 *
 * @return 0 or -1
 *
*******************************************************************************/
int bflb_efuse_read_mac_address_opt(uint8_t slot, uint8_t mac[6], uint8_t reload)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpval = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;

    if (slot >= 3) {
        return -1;
    }

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_WIFI_MAC_LOW_OFFSET, &tmpval, 1, reload);
    } else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W0_OFFSET, &tmpval, 1, reload);
    } else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_DBG_PWD_LOW_OFFSET, &tmpval, 1, reload);
    }

    BL_WRWD_TO_BYTEP(maclow, tmpval);

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_WIFI_MAC_HIGH_OFFSET, &tmpval, 1, reload);
    } else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W1_OFFSET, &tmpval, 1, reload);
    } else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_DBG_PWD_HIGH_OFFSET, &tmpval, 1, reload);
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
        return 0;
    } else {
        return -1;
    }
}

float bflb_efuse_get_adc_trim(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    uint32_t tmp;

    float coe = 1.0;
    char trim_name[] = "gpadc_gain";

    bflb_ef_ctrl_read_common_trim(NULL, trim_name, &trim, 1);

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

uint32_t bflb_efuse_get_adc_tsen_trim(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    char trim_name[] = "tsen";

    bflb_ef_ctrl_read_common_trim(NULL, trim_name, &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            return trim.value;
        }
    }

    return 2042;
}

void bflb_efuse_read_secure_boot(uint8_t *sign, uint8_t *aes)
{
    uint32_t tmpval = 0;

    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_CFG_0_OFFSET, &tmpval, 1, 1);
    *sign = ((tmpval & EF_DATA_EF_SBOOT_SIGN_MODE_MSK) >> EF_DATA_EF_SBOOT_SIGN_MODE_POS) & 0x01;
    if((tmpval & EF_DATA_EF_CPU0_ENC_EN_MSK) >> EF_DATA_EF_CPU0_ENC_EN_POS){
        *aes = ((tmpval & EF_DATA_EF_SF_AES_MODE_MSK) >> EF_DATA_EF_SF_AES_MODE_POS);
    }else{
        *aes = 0;
    }
}
