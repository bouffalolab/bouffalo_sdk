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

#include "bl702_ef_cfg.h"
#include "bl702_glb.h"
#include "bl702_hbn.h"
#include "hardware/ef_data_reg.h"

extern int bflb_efuse_read_mac_address_opt(uint8_t slot, uint8_t mac[6], uint8_t reload);

static const bflb_ef_ctrl_com_trim_cfg_t trim_list[] = {
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
    }
};

static HBN_XCLK_CLK_Type hbnXclkClk; // base HBN clock selector
static HBN_ROOT_CLK_Type hbnRootClk; // HBN root clock selector, may use DLL

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
    hbnXclkClk = HBN_Get_XCLK_CLK_Sel();
    hbnRootClk = HBN_Get_ROOT_CLK_Sel();
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);
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
    switch (hbnXclkClk) {
        case HBN_XCLK_CLK_RC32M:
            HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);
            break;
        case HBN_XCLK_CLK_XTAL:
            HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);
            break;
        default:
            return;
    }

    if (hbnRootClk != HBN_Get_ROOT_CLK_Sel()) {
        HBN_Set_ROOT_CLK_Sel(hbnRootClk);
    }
}

/****************************************************************************/ /**
 * @brief  Efuse get trim list
 *
 * @param  trim_list: Trim list pointer
 *
 * @return Trim list count
 *
*******************************************************************************/
uint32_t bflb_ef_ctrl_get_common_trim_list(const bflb_ef_ctrl_com_trim_cfg_t **ptrim_list)
{
    *ptrim_list = &trim_list[0];
    return sizeof(trim_list) / sizeof(trim_list[0]);
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

    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_WIFI_MAC_HIGH_OFFSET, p, 1, 1);
}

void bflb_efuse_get_chipid(uint8_t chipid[8])
{
    bflb_efuse_read_mac_address_opt(0, chipid, 1);
    chipid[6] = 0;
    chipid[7] = 0;
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

    part1Empty = (bflb_ef_ctrl_is_all_bits_zero(tmp1, 16, 16));
    part2Empty = (bflb_ef_ctrl_is_all_bits_zero(tmp2,  0, 32));

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
    uint8_t *maclow = (uint8_t *)(mac);
    uint8_t *machigh = (uint8_t *)(mac + 2);
    uint32_t tmpval;
    uint32_t i = 0, cnt = 0;

    if (slot >= 3) {
        return -1;
    }

    /* Change to local order */
    for (i = 0; i < 3; i++) {
        tmpval = mac[i];
        mac[i] = mac[5 - i];
        mac[5 - i] = tmpval;
    }

    /* The low 16 bits */
    tmpval  = maclow[0] << 16;
    tmpval |= maclow[1] << 24;

    cnt += bflb_ef_ctrl_get_byte_zero_cnt((tmpval >> 0) & 0xff);
    cnt += bflb_ef_ctrl_get_byte_zero_cnt((tmpval >> 8) & 0xff);

    if (slot == 0) {
        bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_WIFI_MAC_LOW_OFFSET, &tmpval, 1, program);
    } else if (slot == 1) {
        bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W0_OFFSET, &tmpval, 1, program);
    } else if (slot == 2) {
        bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_DBG_PWD_LOW_OFFSET, &tmpval, 1, program);
    }

    /* The high 32 bits */
    tmpval = BL_RDWD_FRM_BYTEP(machigh);

    /* Compute parity */
    for (i = 0; i < 6; i++) {
        cnt += bflb_ef_ctrl_get_byte_zero_cnt(mac[i]);
    }

    /* Get current parity data */
    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W2_OFFSET, &tmpval, 1, 0);

    if (slot == 0) {
        tmpval &= ~(0x3f << 0);
        tmpval |= (cnt & 0x3f) << 0;
    } else if (slot == 1) {
        tmpval &= ~(0x3f << 6);
        tmpval |= (cnt & 0x3f) << 6;
    } else if (slot == 2) {
        tmpval &= ~(0x3f << 12);
        tmpval |= (cnt & 0x3f) << 12;
    }

    /* Set new parity data */
    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W2_OFFSET, &tmpval, 1, 0);

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
    uint8_t *maclow = (uint8_t *)(mac);
    uint8_t *machigh = (uint8_t *)(mac + 2);
    uint32_t tmpval = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;
    uint32_t crc = 0;

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

    cnt += bflb_ef_ctrl_get_byte_zero_cnt((tmpval >> 0) & 0xff);
    cnt += bflb_ef_ctrl_get_byte_zero_cnt((tmpval >> 8) & 0xff);

    maclow[0] = (tmpval >> 16) & 0xff;
    maclow[1] = (tmpval >> 24) & 0xff;

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_WIFI_MAC_HIGH_OFFSET, &tmpval, 1, reload);
    } else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W1_OFFSET, &tmpval, 1, reload);
    } else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_DBG_PWD_HIGH_OFFSET, &tmpval, 1, reload);
    }

    BL_WRWD_TO_BYTEP(machigh, tmpval);

    /* Compute parity */
    for (i = 0; i < 6; i++) {
        cnt += bflb_ef_ctrl_get_byte_zero_cnt(mac[i]);
    }

    /* Get original parity */
    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_KEY_SLOT_5_W2_OFFSET, &tmpval, 1, reload);

    if (slot == 0) {
        crc = ((tmpval >> 0) & 0x3f);
    } else if (slot == 1) {
        crc = ((tmpval >> 6) & 0x3f);
    } else if (slot == 2) {
        crc = ((tmpval >> 12) & 0x3f);
    }

    /* Check parity */
    if ((cnt & 0x3f) == crc) {
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

    float coe = 1.0f;

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

uint32_t bflb_efuse_get_adc_tsen_trim(void)
{
    bflb_ef_ctrl_com_trim_t trim;

    bflb_ef_ctrl_read_common_trim(NULL, "tsen", &trim, 1);
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
    *aes = ((tmpval & EF_DATA_EF_SF_AES_MODE_MSK) >> EF_DATA_EF_SF_AES_MODE_POS);
}

void bflb_efuse_write_aes_key(uint8_t index, uint8_t *data, uint32_t len)
{
    if (index > 5) {
        return;
    }

    bflb_ef_ctrl_write_direct(NULL, 0x1C + index * 4, (uint32_t *)data, len, 1);
}

void bflb_efuse_read_aes_key(uint8_t index, uint8_t *data, uint32_t len)
{
    if (index > 5) {
        return;
    }

    bflb_ef_ctrl_read_direct(NULL, 0x1C + index * 4, (uint32_t *)data, len, 1);
}

void bflb_efuse_lock_aes_key_write(uint8_t index)
{
    uint32_t lock = 0;

    if (index > 5) {
        return;
    }
    /* write lock */
    if (index <= 3) {
        lock |= (1 << (index + 19));
    } else {
        lock |= (1 << (index + 19));
        lock |= (1 << (index - 4 + 13));
    }
    bflb_ef_ctrl_write_direct(NULL, 0x7c, &lock, 1, 1);
}

void bflb_efuse_lock_aes_key_read(uint8_t index)
{
    uint32_t lock = 0;

    if (index > 5) {
        return;
    }
    /* read lock */
    lock |= (1 << (index + 26));

    bflb_ef_ctrl_write_direct(NULL, 0x7c, &lock, 1, 1);
}

void bflb_efuse_write_sw_usage(uint32_t index, uint32_t usage, uint8_t program)
{
    if (index != 0) {
        return;
    }
    bflb_ef_ctrl_write_direct(NULL, EF_DATA_EF_SW_USAGE_0_OFFSET + index * 4, &usage, 1, program);
}

void bflb_efuse_read_sw_usage(uint32_t index, uint32_t *usage)
{
    if (index != 0) {
        return;
    }
    bflb_ef_ctrl_read_direct(NULL, EF_DATA_EF_SW_USAGE_0_OFFSET + index * 4, (uint32_t *)usage, 1, 1);
}