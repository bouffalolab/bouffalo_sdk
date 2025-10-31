/**
  ******************************************************************************
  * @file    bl616d_ef_cfg.c
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
#include "bl616d_common.h"
#include "bflb_efuse.h"
#define  WHO_AM_I    (0x60f82800)
#define  I_AM_A0     (0x0616d001)

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
        .en_addr = 0xB4 * 8 + 12,
        .parity_addr = 0xB4 * 8 + 11,
        .value_addr = 0xB4 * 8 + 0,
        .value_len = 11,
    },
    {
        .name = "ldo09",
        .en_addr = 0xB8 * 8 + 30,
        .parity_addr = 0xB8 * 8 + 31,
        .value_addr = 0xB8 * 8 + 26,
        .value_len = 4,
    },
    {
        .name = "ldo08",
        .en_addr = 0xB8 * 8 + 24,
        .parity_addr = 0xB8 * 8 + 25,
        .value_addr = 0xB8 * 8 + 20,
        .value_len = 4,
    },
    {
        .name = "ldo18",
        .en_addr = 0xB8 * 8 + 18,
        .parity_addr = 0xB8 * 8 + 19,
        .value_addr = 0xB8 * 8 + 14,
        .value_len = 4,
    },
    {
        .name = "rc32m",
        .en_addr = 0xB8 * 8 + 12,
        .parity_addr = 0xB8 * 8 + 13,
        .value_addr = 0xB8 * 8 + 4,
        .value_len = 8,
    },
    {
        .name = "xtal0",
        .en_addr = 0xBC * 8 + 30,
        .parity_addr = 0xBC * 8 + 31,
        .value_addr = 0xBC * 8 + 22,
        .value_len = 8,
    },
    {
        .name = "xtal1",
        .en_addr = 0xBC * 8 + 20,
        .parity_addr = 0xBC * 8 + 21,
        .value_addr = 0xBC * 8 + 12,
        .value_len = 8,
    },
    {
        .name = "xtal2",
        .en_addr = 0xBC * 8 + 10,
        .parity_addr = 0xBC * 8 + 11,
        .value_addr = 0xBC * 8 + 2,
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
 * @param  device_info: info pointer
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bflb_efuse_get_device_info(bflb_efuse_device_info_type *device_info)
{
    uint32_t tmpval;

    bflb_ef_ctrl_read_direct(NULL, 0x18, &tmpval, 1, 1);
    device_info->package = (tmpval >> 22) & 3;
    device_info->flash_info = (tmpval >> 26) & 7;
    device_info->psram_info = (tmpval >> 24) & 3;
    device_info->version = (tmpval >> 29) & 7;

    switch (device_info->package) {
        case 0:
            device_info->package_name = "QFN48";
            break;
        case 1:
            device_info->package_name = "QFN56";
            break;
        case 2:
            device_info->package_name = "QFN68";
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
            device_info->flash_info_name = "4MB";
            break;
        case 2:
            device_info->flash_info_name = "8MB";
            break;
        case 3:
            device_info->flash_info_name = "16MB";
            break;
        case 4:
            device_info->flash_info_name = "32MB";
            break;
        default:
            device_info->flash_info_name = "ERROR";
            break;
    }
    switch (device_info->psram_info) {
        case 0:
            device_info->psram_info_name = "NO";
            break;
        case 1:
            device_info->psram_info_name = "WB_4MB";
            break;
        case 2:
            device_info->psram_info_name = "WB_8MB";
            break;
        case 3:
            device_info->psram_info_name = "WB_16MB";
            break;
        default:
            device_info->psram_info_name = "ERROR";
    }
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
int bflb_efuse_read_mac_address(uint8_t mac[6], uint8_t reload)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpval = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;


    bflb_ef_ctrl_read_direct(NULL, 0x14, &tmpval, 1, reload);

    BL_WRWD_TO_BYTEP(maclow, tmpval);

    bflb_ef_ctrl_read_direct(NULL, 0x18, &tmpval, 1, reload);

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

    return bflb_efuse_read_mac_address(chipid, 1);
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
    uint32_t regval;

    regval = getreg32(WHO_AM_I);
    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, 0x14, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, 0x18, &tmp2, 1, reload);
    } else if (slot == 1) {
        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_read_direct(NULL, 0xC0, &tmp1, 1, reload);
            bflb_ef_ctrl_read_direct(NULL, 0xC4, &tmp2, 1, reload);
        }else
        {
            bflb_ef_ctrl_read_direct(NULL, 0x110, &tmp1, 1, reload);
            bflb_ef_ctrl_read_direct(NULL, 0x114, &tmp2, 1, reload);
        }
    } else if (slot == 2) {
        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_read_direct(NULL, 0xC8, &tmp1, 1, reload);
            bflb_ef_ctrl_read_direct(NULL, 0xCC, &tmp2, 1, reload);
        }else
        {
            bflb_ef_ctrl_read_direct(NULL, 0x118, &tmp1, 1, reload);
            bflb_ef_ctrl_read_direct(NULL, 0x11C, &tmp2, 1, reload);
        }
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
    uint32_t regval;

    regval = getreg32(WHO_AM_I);
    if (slot >= 3) {
        return -1;
    }

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, 0x14, &tmpval, 1, reload);
    }else if (slot == 1) {

        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_read_direct(NULL, 0xC0, &tmpval, 1, reload);
        }else
        {
            bflb_ef_ctrl_read_direct(NULL, 0x110, &tmpval, 1, reload);
        }
    }else if (slot == 2) {
        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_read_direct(NULL, 0xC8, &tmpval, 1, reload);
        }else
        {
            bflb_ef_ctrl_read_direct(NULL, 0x118, &tmpval, 1, reload);
        }
    }
    BL_WRWD_TO_BYTEP(maclow, tmpval);

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, 0x18, &tmpval, 1, reload);
    }else if (slot == 1) {
        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_read_direct(NULL, 0xC4, &tmpval, 1, reload);
        }else
        {
            bflb_ef_ctrl_read_direct(NULL, 0x114, &tmpval, 1, reload);
        }
    }else if (slot == 2) {
        if(regval == I_AM_A0)
        {
            bflb_ef_ctrl_read_direct(NULL, 0xCC, &tmpval, 1, reload);
        }else
        {
            bflb_ef_ctrl_read_direct(NULL, 0x11C, &tmpval, 1, reload);
        }
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

/****************************************************************************/ /**
 * @brief  Efuse read adc trim
 *
 * @param  None
 *
 * @return coe
 *
*******************************************************************************/
float bflb_efuse_get_adc_trim(void)
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
 * @brief  Efuse read adc tsen trim
 *
 * @param  None
 *
 * @return int
 *
*******************************************************************************/
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

/****************************************************************************/ /**
 * @brief  Efuse read secure boot
 *
 * @param  sign: sign type
 * @param  aes: aes type
 *
 * @return None
 *
*******************************************************************************/
void bflb_efuse_read_secure_boot(uint8_t *sign, uint8_t *aes)
{
    uint32_t tmpval = 0;

    bflb_ef_ctrl_read_direct(NULL, 0x00, &tmpval, 1, 1);
    *aes = (tmpval & 0x03);

    bflb_ef_ctrl_read_direct(NULL, 0x60, &tmpval, 1, 1);
    *sign = ((tmpval>>18) & 0x03);
}

/****************************************************************************/ /**
 * @brief  Efuse write secure boot AES
 *
 * @param  aes_type: aes type
 * @param  xts_mode: xts type
 *
 * @return 0 for success
 *
*******************************************************************************/
int bflb_efuse_enable_aes(uint8_t aes_type, uint8_t xts_mode)
{
    uint32_t tmpval = aes_type;

    if(xts_mode){
        tmpval |= (xts_mode<<3);
    }

    bflb_ef_ctrl_write_direct(NULL, 0x00, &tmpval, 1, 1);

    return 0;
}

/****************************************************************************/ /**
 * @brief  Efuse write AES key lock
 *
 * @param  aes_type: aes type
 * @param  xts_mode: xts type
 *
 * @return 0 for success
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
    }
    return 0;
}

/****************************************************************************/ /**
 * @brief  Efuse write dbg key lock
 *
 * @param  rd_lock: read lock
 * @param  wr_lock: write lock
 *
 * @return 0 for success
 *
*******************************************************************************/
int bflb_efuse_rw_lock_dbg_key(uint8_t rd_lock, uint8_t wr_lock)
{
    uint32_t tmpval = 0;

    if(wr_lock){
        tmpval |= (1 << 15);
    }

    if(rd_lock){
        tmpval |= (1 << 26);
    }

    bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);

    return 0;
}

/****************************************************************************/ /**
 * @brief  Efuse write public key hash lock
 *
 * @param  pkhash_len: public key length
 *
 * @return 0 for success
 *
*******************************************************************************/
int bflb_efuse_write_lock_pk_hash(uint32_t pkhash_len)
{
    uint32_t tmpval = 0;

    if(256 == pkhash_len){
       tmpval = (1 << 17) | (1 << 18);
       bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
    }else{
       tmpval = (1 << 17) | (1 << 18);
       bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
       tmpval = (1 << 19);
       bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }
    return 0;
}

/****************************************************************************/ /**
 * @brief  Efuse write usb pid vid lock
 *
 * @param  None
 *
 * @return 0 for success
 *
*******************************************************************************/
int bflb_efuse_write_lock_usb_pid_vid(void)
{
    uint32_t tmpval = (1 << 24);

    bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);

    return 0;
}

