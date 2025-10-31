/**
  ******************************************************************************
  * @file    bl616l_ef_cfg.c
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
#include "bl616l_common.h"
#include "bflb_efuse.h"

static const bflb_ef_ctrl_com_trim_cfg_t trim_list[] = {
    {
        .name = "rcal",
        .en_addr = 0x60 * 8 + 29,
        .parity_addr = 0x60 * 8 + 28,
        .value_addr = 0x60 * 8 + 17,
        .value_len = 11,
    },
    {
        .name = "psram_trim",
        .en_addr = 0x7C * 8 + 9,
        .parity_addr = 0x7C * 8 + 8,
        .value_addr = 0x7C * 8 + 0,
        .value_len = 8,
    },
    {
        .name = "gpadc_offset",
        .en_addr = 0x60 * 8 + 16,
        .parity_addr = 0x60 * 8 + 15,
        .value_addr = 0x60 * 8 + 6,
        .value_len = 9,
    },
    {
        .name = "rc32k_cap",
        .en_addr = 0x60 * 8 + 5,
        .parity_addr = 0x60 * 8 + 4,
        .value_addr = 0x60 * 8 + 0,
        .value_len = 4,
    },
    {
        .name = "rc32m",
        .en_addr = 0x64 * 8 + 31,
        .parity_addr = 0x64 * 8 + 30,
        .value_addr = 0x64 * 8 + 22,
        .value_len = 8,
    },
    {
        .name = "usb20",
        .en_addr = 0x64 * 8 + 21,
        .parity_addr = 0x64 * 8 + 20,
        .value_addr = 0x64 * 8 + 13,
        .value_len = 7,
    },
    {
        .name = "ldo08_trim",
        .en_addr = 0x68 * 8 + 31,
        .parity_addr = 0x68 * 8 + 30,
        .value_addr = 0x68 * 8 + 26,
        .value_len = 4,
    },
    {
        .name = "ldo18_trim",
        .en_addr = 0x68 * 8 + 25,
        .parity_addr = 0x68 * 8 + 24,
        .value_addr = 0x68 * 8 + 20,
        .value_len = 4,
    },
    {
        .name = "ldosoc_trim",
        .en_addr = 0x68 * 8 + 19,
        .parity_addr = 0x68 * 8 + 18,
        .value_addr = 0x68 * 8 + 14,
        .value_len = 4,
    },
    {
        .name = "ldo13_trim",
        .en_addr = 0x68 * 8 + 13,
        .parity_addr = 0x68 * 8 + 12,
        .value_addr = 0x68 * 8 + 8,
        .value_len = 4,
    },
    {
        .name = "gpadc_vref",
        .en_addr = 0x68 * 8 + 7,
        .parity_addr = 0x68 * 8 + 6,
        .value_addr = 0x68 * 8 + 0,
        .value_len = 6,
    },
    {
        .name = "tsen",
        .en_addr = 0x6C * 8 + 31,
        .parity_addr = 0x6C * 8 + 30,
        .value_addr = 0x6C * 8 + 18,
        .value_len = 12,
    },
    {
        .name = "gpadc_gain",
        .en_addr = 0x6C * 8 + 17,
        .parity_addr = 0x6C * 8 + 16,
        .value_addr = 0x6C * 8 + 4,
        .value_len = 12,
    },
    /* ==== Added: XTAL capcode trims ==== */
    {
        .name = "xtal0",              /* 0x20: xtal_capcode_1_inout */
        .en_addr = 0x20 * 8 + 31,     /* en */
        .parity_addr = 0x20 * 8 + 30, /* parity */
        .value_addr = 0x20 * 8 + 22,  /* [29:22] */
        .value_len = 8,
    },
    {
        .name = "xtal1",              /* 0x28: xtal_capcode_2_inout */
        .en_addr = 0x28 * 8 + 31,
        .parity_addr = 0x28 * 8 + 30,
        .value_addr = 0x28 * 8 + 22,  /* [29:22] */
        .value_len = 8,
    },
    {
        .name = "xtal2",              /* 0x38/0x3C: xtal_capcode_3_inout */
        .en_addr = 0x38 * 8 + 5,      /* 0x38[5] en */
        .parity_addr = 0x38 * 8 + 4,  /* 0x38[4] parity */
        .value_addr = 0x3C * 8 + 24,  /* 0x3C[31:24] */
        .value_len = 8,
    },

    /* ==== Added: Temperature MP trims ==== */
    {
        .name = "temp_mp0",           /* MP1: 0x38/0x3C */
        .en_addr = 0x38 * 8 + 3,      /* 0x38[3] */
        .parity_addr = 0x38 * 8 + 2,  /* 0x38[2] */
        .value_addr = 0x3C * 8 + 16,  /* 0x3C[23:16] */
        .value_len = 8,
    },
    {
        .name = "temp_mp1",           /* MP2: 0x38/0x3C */
        .en_addr = 0x38 * 8 + 1,      /* 0x38[1] */
        .parity_addr = 0x38 * 8 + 0,  /* 0x38[0] */
        .value_addr = 0x3C * 8 + 8,   /* 0x3C[15:8] */
        .value_len = 8,
    },
    {
        .name = "temp_mp2",           /* MP3: 0x40/0x3C */
        .en_addr = 0x40 * 8 + 31,     /* 0x40[31] */
        .parity_addr = 0x40 * 8 + 30, /* 0x40[30] */
        .value_addr = 0x3C * 8 + 0,   /* 0x3C[7:0] */
        .value_len = 8,
    },
    {
        .name = "temp_mp3",           /* MP4: 0x58/0x5C */
        .en_addr = 0x58 * 8 + 28,     /* 0x58[28] */
        .parity_addr = 0x58 * 8 + 27, /* 0x58[27] */
        .value_addr = 0x5C * 8 + 16,  /* 0x5C[23:16] */
        .value_len = 8,
    },
    {
        .name = "temp_mp4",           /* MP5: 0x58/0x5C */
        .en_addr = 0x58 * 8 + 1,      /* 0x58[1] */
        .parity_addr = 0x58 * 8 + 0,  /* 0x58[0] */
        .value_addr = 0x5C * 8 + 24,  /* 0x5C[31:24] */
        .value_len = 8,
    },
    {
        .name = "temp_mp5",           /* MP6: 0x5C */
        .en_addr = 0x5C * 8 + 15,     /* 0x5C[15] */
        .parity_addr = 0x5C * 8 + 14, /* 0x5C[14] */
        .value_addr = 0x5C * 8 + 6,   /* 0x5C[13:6] */
        .value_len = 8,
    },
    /* ==== Poweroffset trims (HP/LP: 15 bits = high|mid|low) ==== */
    { .name = "hp_poffset0", .en_addr = 0x2C*8+31, .parity_addr = 0x2C*8+30, .value_addr = 0x2C*8+15, .value_len = 15 },
    { .name = "hp_poffset1", .en_addr = 0x30*8+31, .parity_addr = 0x30*8+30, .value_addr = 0x2C*8+0,  .value_len = 15 },
    { .name = "hp_poffset2", .en_addr = 0x44*8+31, .parity_addr = 0x44*8+30, .value_addr = 0x40*8+15, .value_len = 15 },
    { .name = "hp_poffset3", .en_addr = 0x48*8+31, .parity_addr = 0x48*8+30, .value_addr = 0x48*8+15, .value_len = 15 },
    { .name = "hp_poffset4", .en_addr = 0x4C*8+31, .parity_addr = 0x4C*8+30, .value_addr = 0x48*8+0,  .value_len = 15 },
    { .name = "hp_poffset5", .en_addr = 0x50*8+31, .parity_addr = 0x50*8+30, .value_addr = 0x4C*8+15, .value_len = 15 },

    { .name = "lp_poffset0", .en_addr = 0x34*8+31, .parity_addr = 0x34*8+30, .value_addr = 0x30*8+15, .value_len = 15 },
    { .name = "lp_poffset1", .en_addr = 0x34*8+29, .parity_addr = 0x34*8+28, .value_addr = 0x30*8+0,  .value_len = 15 },
    { .name = "lp_poffset2", .en_addr = 0x44*8+29, .parity_addr = 0x44*8+28, .value_addr = 0x40*8+0,  .value_len = 15 },
    { .name = "lp_poffset3", .en_addr = 0x54*8+29, .parity_addr = 0x54*8+28, .value_addr = 0x4C*8+0,  .value_len = 15 },
    { .name = "lp_poffset4", .en_addr = 0x54*8+27, .parity_addr = 0x54*8+26, .value_addr = 0x50*8+15, .value_len = 15 },
    { .name = "lp_poffset5", .en_addr = 0x54*8+31, .parity_addr = 0x54*8+30, .value_addr = 0x50*8+0,  .value_len = 15 },

    /* ==== BZ poweroffset trims (25 bits = 2476|2464|2448|2432|2412) ==== */
    { .name = "bz_poffset0", .en_addr = 0x34*8+27, .parity_addr = 0x34*8+26, .value_addr = 0x34*8+1,  .value_len = 25 },
    { .name = "bz_poffset1", .en_addr = 0x34*8+0,  .parity_addr = 0x38*8+31, .value_addr = 0x38*8+6,  .value_len = 25 },
    { .name = "bz_poffset2", .en_addr = 0x44*8+2,  .parity_addr = 0x44*8+1,  .value_addr = 0x44*8+3,  .value_len = 25 },
    { .name = "bz_poffset3", .en_addr = 0x54*8+0,  .parity_addr = 0x58*8+31, .value_addr = 0x54*8+1,  .value_len = 25 },
    { .name = "bz_poffset4", .en_addr = 0x58*8+30, .parity_addr = 0x58*8+29, .value_addr = 0x58*8+2,  .value_len = 25 },

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
void bflb_efuse_get_device_info(bflb_efuse_device_info_type *device_info)
{
    uint32_t tmpval;

    bflb_ef_ctrl_read_direct(NULL, 0x08, &tmpval, 1, 1);
    device_info->package = (tmpval >> 22) & 3;
    device_info->flash_info = (tmpval >> 26) & 7;
    device_info->psram_info = (tmpval >> 24) & 3;
    device_info->version = (tmpval >> 29) & 7;

    switch (device_info->package) {
        case 0:
            device_info->package_name = "QFN24";
            break;
        case 1:
            device_info->package_name = "QFN32";
            break;
        case 2:
            device_info->package_name = "QFN40";
            break;
        case 3:
            device_info->package_name = "QFN48";
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
            device_info->flash_info_name = "0.5MB";
            break;
        case 2:
            device_info->flash_info_name = "1MB";
            break;
        case 3:
            device_info->flash_info_name = "2MB";
            break;
        case 4:
            device_info->flash_info_name = "4MB";
            break;
        case 5:
            device_info->flash_info_name = "8MB";
            break;
        case 6:
            device_info->flash_info_name = "16MB";
            break;
        case 7:
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


    bflb_ef_ctrl_read_direct(NULL, 0x4, &tmpval, 1, reload);

    BL_WRWD_TO_BYTEP(maclow, tmpval);

    bflb_ef_ctrl_read_direct(NULL, 0x8, &tmpval, 1, reload);

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

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, 0x04, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, 0x08, &tmp2, 1, reload);
    } else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, 0x1C, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, 0x20, &tmp2, 1, reload);
    } else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, 0x24, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, 0x28, &tmp2, 1, reload);
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
        bflb_ef_ctrl_read_direct(NULL, 0x04, &tmpval, 1, reload);
    }else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, 0x1C, &tmpval, 1, reload);
    }else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, 0x24, &tmpval, 1, reload);
    }
    BL_WRWD_TO_BYTEP(maclow, tmpval);

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, 0x08, &tmpval, 1, reload);
    }else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, 0x20, &tmpval, 1, reload);
    }else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, 0x28, &tmpval, 1, reload);
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
    *aes = tmpval & 0x07;

    bflb_ef_ctrl_read_direct(NULL, 0x70, &tmpval, 1, 1);
    *sign = (tmpval >> 25) & 0x07;
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
        tmpval |= (xts_mode<<2);
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
    }else if(1 == key_index){
        if(wr_lock){
            tmpval |= (1 << 18);
        }
        if(rd_lock){
            tmpval |= (1 << 28);
        }
    }else if(2 == key_index){
        if(wr_lock){
            tmpval |= ((1 << 19) | (1 << 20) |(1 << 21));
        }
        if(rd_lock){
            tmpval |= ((1 << 29) | (1 << 30) |(1 << 31));
        }
    }

    bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);

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
        tmpval |= (1 << 19);
    }

    if(rd_lock){
        tmpval |= (1 << 29);
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

    if (256 == pkhash_len) {
        // lock 0x40-0x44, 0x48-0x4C, 0x50-0x5C
        tmpval = (1 << 22) | (1 << 23) | (1 << 24);
    } else if (192 == pkhash_len) {
        // 192-bit(=256-64) -> lock 0x48-0x4C, 0x50-0x5C
        tmpval = (1 << 23) | (1 << 24);
    } else if (384 == pkhash_len) {
        tmpval = (1 << 22) | (1 << 23) | (1 << 24) | (1 << 18);
    }

    bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);

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
    uint32_t tmpval = (1 << 26);

    bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);

    return 0;
}
