/**
  ******************************************************************************
  * @file    blsp_bootinfo.h
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
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
#ifndef __BLSP_BOOTINFO_H__
#define __BLSP_BOOTINFO_H__

#include "stdint.h"
#include "blsp_port.h"
#include "hal_flash.h"
#include "hal_boot2.h"


#define BFLB_BOOT2_CPU0_MAGIC        "BFNP"
#define BFLB_BOOT2_CPU1_MAGIC        "BFAP"
#define BFLB_BOOT2_FLASH_CFG_MAGIC   "FCFG"
#define BFLB_BOOT2_PLL_CFG_MAGICCODE "PCFG"
#define BFLB_BOOT2_FLASH_TZC_MAGIC   "TCFG"
#define BFLB_BOOT2_DEADBEEF_VAL      0xdeadbeef

#define BFLB_BOOT2_READBUF_SIZE         4 * 1024
#define BFLB_FW_IMG_OFFSET_AFTER_HEADER 4 * 1024


/* Image owner type */
#define BFLB_BOOT2_CPU_0   0
#define BFLB_BOOT2_CPU_1   1
#define BFLB_BOOT2_CPU_MAX HAL_EFUSE_CPU_MAX

/* Public key hash size */
#define BFLB_BOOT2_PK_HASH_SIZE HAL_EFUSE_PK_HASH_SIZE
#define BFLB_BOOT2_HASH_SIZE    256 / 8
/* Public key type */
#define BFLB_BOOT2_ECC_KEYXSIZE 256 / 8
#define BFLB_BOOT2_ECC_KEYYSIZE 256 / 8
#define BFLB_BOOT2_SIGN_MAXSIZE 2048 / 8

/* Power save define */
#define BFLB_PSM_ACTIVE 0
#define BFLB_PSM_HBN    1



typedef enum {
    BFLB_BOOT2_SUCCESS = 0x00,

    /* Flash*/
    BFLB_BOOT2_FLASH_INIT_ERROR = 0x0001,
    BFLB_BOOT2_FLASH_ERASE_PARA_ERROR = 0x0002,
    BFLB_BOOT2_FLASH_ERASE_ERROR = 0x0003,
    BFLB_BOOT2_FLASH_WRITE_PARA_ERROR = 0x0004,
    BFLB_BOOT2_FLASH_WRITE_ADDR_ERROR = 0x0005,
    BFLB_BOOT2_FLASH_WRITE_ERROR = 0x0006,
    BFLB_BOOT2_FLASH_BOOT_PARA = 0x0007,
    BFLB_BOOT2_FLASH_READ_ERROR = 0x0008,

    /* Image*/
    BFLB_BOOT2_IMG_BOOTHEADER_LEN_ERROR = 0x0201,
    BFLB_BOOT2_IMG_BOOTHEADER_NOT_LOAD_ERROR = 0x0202,
    BFLB_BOOT2_IMG_BOOTHEADER_MAGIC_ERROR = 0x0203,
    BFLB_BOOT2_IMG_BOOTHEADER_CRC_ERROR = 0x0204,
    BFLB_BOOT2_IMG_BOOTHEADER_ENCRYPT_NOTFIT = 0x0205,
    BFLB_BOOT2_IMG_BOOTHEADER_SIGN_NOTFIT = 0x0206,
    BFLB_BOOT2_IMG_SEGMENT_CNT_ERROR = 0x0207,
    BFLB_BOOT2_IMG_AES_IV_LEN_ERROR = 0x0208,
    BFLB_BOOT2_IMG_AES_IV_CRC_ERROR = 0x0209,
    BFLB_BOOT2_IMG_PK_LEN_ERROR = 0x020a,
    BFLB_BOOT2_IMG_PK_CRC_ERROR = 0x020b,
    BFLB_BOOT2_IMG_PK_HASH_ERROR = 0x020c,
    BFLB_BOOT2_IMG_SIGNATURE_LEN_ERROR = 0x020d,
    BFLB_BOOT2_IMG_SIGNATURE_CRC_ERROR = 0x020e,
    BFLB_BOOT2_IMG_SECTIONHEADER_LEN_ERROR = 0x020f,
    BFLB_BOOT2_IMG_SECTIONHEADER_CRC_ERROR = 0x0210,
    BFLB_BOOT2_IMG_SECTIONHEADER_DST_ERROR = 0x0211,
    BFLB_BOOT2_IMG_SECTIONDATA_LEN_ERROR = 0x0212,
    BFLB_BOOT2_IMG_SECTIONDATA_DEC_ERROR = 0x0213,
    BFLB_BOOT2_IMG_SECTIONDATA_TLEN_ERROR = 0x0214,
    BFLB_BOOT2_IMG_SECTIONDATA_CRC_ERROR = 0x0215,
    BFLB_BOOT2_IMG_HALFBAKED_ERROR = 0x0216,
    BFLB_BOOT2_IMG_HASH_ERROR = 0x0217,
    BFLB_BOOT2_IMG_SIGN_PARSE_ERROR = 0x0218,
    BFLB_BOOT2_IMG_SIGN_ERROR = 0x0219,
    BFLB_BOOT2_IMG_DEC_ERROR = 0x021a,
    BFLB_BOOT2_IMG_ALL_INVALID_ERROR = 0x021b,
    BFLB_BOOT2_IMG_Roll_Back = 0x021c,

    /* MISC*/
    BFLB_BOOT2_MEM_ERROR = 0xfffb,
    BFLB_BOOT2_PLL_ERROR = 0xfffc,
    BFLB_BOOT2_INVASION_ERROR = 0xfffd,
    BFLB_BOOT2_POLLING = 0xfffe,
    BFLB_BOOT2_FAIL = 0xffff,

} boot_error_code;

typedef struct
{
    uint32_t magicCode; /*'BFXP'*/
    uint32_t rivison;
    hal_flash_config flash_cfg;
    hal_pll_config clk_cfg;
    __PACKED_UNION
    {
        __PACKED_STRUCT
        {
            uint32_t sign              : 2;  /* [1: 0]      for sign*/
            uint32_t encrypt_type      : 2;  /* [3: 2]      for encrypt */
            uint32_t key_sel           : 2;  /* [5: 4]      for key sel in boot interface*/
            uint32_t rsvd6_7           : 2;  /* [7: 6]      for encrypt*/
            uint32_t no_segment        : 1;  /* [8]         no segment info */
            uint32_t cache_enable      : 1;  /* [9]         for cache */
            uint32_t notLoadInBoot     : 1;  /* [10]        not load this img in bootrom */
            uint32_t aes_region_lock   : 1;  /* [11]        aes region lock */
            uint32_t cache_way_disable : 4;  /* [15: 12]    cache way disable info*/
            uint32_t crcIgnore         : 1;  /* [16]        ignore crc */
            uint32_t hash_ignore       : 1;  /* [17]        hash crc */
            uint32_t halt_cpu1         : 1;  /* [18]        halt ap */
            uint32_t rsvd19_31         : 13; /* [31:19]     rsvd */
        }
        bval;
        uint32_t wval;
    }
    bootCfg;

    __PACKED_UNION
    {
        uint32_t segment_cnt;
        uint32_t img_len;
    }
    img_segment_info;

    uint32_t bootEntry; /* entry point of the image*/
    __PACKED_UNION
    {
        uint32_t ram_addr;
        uint32_t flash_offset;
    }
    img_start;

    uint8_t hash[BFLB_BOOT2_HASH_SIZE]; /*hash of the image*/

    uint32_t rsv1;
    uint32_t rsv2;
    uint32_t crc32;
} boot_header_config;

typedef struct
{
    uint8_t aes_iv[16];
    uint32_t crc32;
} boot_aes_config;

typedef struct
{
    uint8_t eckye_x[BFLB_BOOT2_ECC_KEYXSIZE]; //ec key in boot header
    uint8_t eckey_y[BFLB_BOOT2_ECC_KEYYSIZE]; //ec key in boot header
    uint32_t crc32;
} boot_pk_config;

typedef struct
{
    uint32_t sig_len;
    uint8_t signature[BFLB_BOOT2_SIGN_MAXSIZE];
    uint32_t crc32;
} boot_sign_config;

typedef struct
{
    uint8_t encrypt_type;
    uint8_t sign_type;
    uint8_t key_sel;
    uint8_t img_valid;

    uint8_t no_segment;
    uint8_t cache_enable;
    uint8_t cache_way_disable;
    uint8_t hash_ignore;

    uint8_t aes_region_lock;
    uint8_t halt_cpu1;
    uint8_t cpu_type;
    uint8_t r[1];

    __PACKED_UNION
    {
        uint32_t segment_cnt;
        uint32_t img_len;
    }
    img_segment_info;

    uint32_t msp_val;
    uint32_t entry_point;
    __PACKED_UNION
    {
        uint32_t ram_addr;
        uint32_t flash_offset;
    }
    img_start;
    uint32_t sig_len;
    uint32_t sig_len2;

    uint32_t deal_len;
    uint32_t max_input_len;

    uint8_t img_hash[BFLB_BOOT2_HASH_SIZE]; //hash of the whole (all)images
    uint8_t aes_iv[16 + 4];                 //iv in boot header

    uint8_t eckye_x[BFLB_BOOT2_ECC_KEYXSIZE];  //ec key in boot header
    uint8_t eckey_y[BFLB_BOOT2_ECC_KEYYSIZE];  //ec key in boot header
    uint8_t eckey_x2[BFLB_BOOT2_ECC_KEYXSIZE]; //ec key in boot header
    uint8_t eckey_y2[BFLB_BOOT2_ECC_KEYYSIZE]; //ec key in boot header

    uint8_t signature[BFLB_BOOT2_SIGN_MAXSIZE];  //signature in boot header
    uint8_t signature2[BFLB_BOOT2_SIGN_MAXSIZE]; //signature in boot header

} boot2_image_config;

typedef struct
{
    uint32_t msp_store_addr;
    uint32_t pc_store_addr;
    uint32_t default_xip_addr;
} boot_cpu_config;

typedef void (*pentry_t)(void);

extern boot2_image_config g_boot_img_cfg[2];
extern boot2_efuse_hw_config g_efuse_cfg;
extern uint8_t g_ps_mode;
extern uint8_t g_cpu_count;
extern uint8_t *g_boot2_read_buf;



#endif /* __BLSP_BOOTINFO_H__ */



