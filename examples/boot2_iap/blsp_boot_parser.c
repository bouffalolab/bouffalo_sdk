/**
  ******************************************************************************
  * @file    blsp_boot_parser.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case c file
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

#include "blsp_common.h"
#include "bflb_platform.h"
#include "blsp_bootinfo.h"
#include "blsp_boot_parser.h"
#include "softcrc.h"
#include "blsp_port.h"
#include "hal_sec_hash.h"
#include "hal_sec_ecdsa.h"


extern uint32_t g_user_hash_ignored;
struct device *dev_check_hash;

/****************************************************************************/ /**
 * @brief  Check if the input public key is the same as  burned in the efuse
 *
 * @param  cpu_type: CPU Type
 * @param  pkhash: Public key hash pointer
 *
 * @return 1 for valid and 0 for invalid
 *
*******************************************************************************/
static uint32_t blsp_boot_parse_is_pkhash_valid(uint8_t cpu_type, uint8_t *pkhash)
{
    if (cpu_type == BFLB_BOOT2_CPU_0) {
        if (0 == memcmp(g_efuse_cfg.pk_hash_cpu0, pkhash, BFLB_BOOT2_PK_HASH_SIZE)) {
            return 1;
        }
    } else if (cpu_type == BFLB_BOOT2_CPU_1) {
        if (0 == memcmp(g_efuse_cfg.pk_hash_cpu1, pkhash, BFLB_BOOT2_PK_HASH_SIZE)) {
            return 1;
        }
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  Check if the input public key is the same as  burned in the efuse
 *
 * @param  g_boot_img_cfg: Boot image config pointer
 * @param  data: Image data pointer
 *
 * @return boot_error_code type
 *
*******************************************************************************/
int32_t blsp_boot_parse_bootheader(boot2_image_config *boot_img_cfg, uint8_t *data)
{
    boot_header_config *header = (boot_header_config *)data;
    uint32_t crc;
    uint32_t crc_pass = 0;
    uint32_t i = 0;
    uint32_t *phash = (uint32_t *)header->hash;
    int32_t ret;

    if (header->bootCfg.bval.crcIgnore == 1 && header->crc32 == BFLB_BOOT2_DEADBEEF_VAL) {
        MSG("Crc ignored\r\n");
        crc_pass = 1;
    } else {
        crc = BFLB_Soft_CRC32((uint8_t *)header, sizeof(boot_header_config) - sizeof(header->crc32));

        if (header->crc32 == crc) {
            crc_pass = 1;
        }
    }

    if (crc_pass) {
        if (header->bootCfg.bval.notLoadInBoot) {
            return BFLB_BOOT2_IMG_BOOTHEADER_NOT_LOAD_ERROR;
        }

        /* Get which CPU's img it is*/
        for (i = 0; i < BFLB_BOOT2_CPU_MAX; i++) {
            if (0 == memcmp((void *)&header->magicCode, BFLB_BOOT2_CPU0_MAGIC,
                            sizeof(header->magicCode))) {
                break;
            } else if (0 == memcmp((void *)&header->magicCode, BFLB_BOOT2_CPU1_MAGIC,
                                   sizeof(header->magicCode))) {
                break;
            }
        }

        if (i == BFLB_BOOT2_CPU_MAX) {
            /* No cpu img magic match */
            MSG_ERR("Magic code error\r\n");
            return BFLB_BOOT2_IMG_BOOTHEADER_MAGIC_ERROR;
        }

        boot_img_cfg->cpu_type = i;
        boot_img_cfg->entry_point = 0;
        /* Set image valid 0 as default */
        boot_img_cfg->img_valid = 0;

        /* Deal with pll config */

        /* Encrypt and sign */
        boot_img_cfg->encrypt_type = header->bootCfg.bval.encrypt_type;
        boot_img_cfg->sign_type = header->bootCfg.bval.sign;
        boot_img_cfg->key_sel = header->bootCfg.bval.key_sel;

        /* Xip relative */
        boot_img_cfg->no_segment = header->bootCfg.bval.no_segment;
        boot_img_cfg->cache_enable = header->bootCfg.bval.cache_enable;
        boot_img_cfg->aes_region_lock = header->bootCfg.bval.aes_region_lock;
        boot_img_cfg->halt_cpu1 = header->bootCfg.bval.halt_cpu1;
        boot_img_cfg->cache_way_disable = header->bootCfg.bval.cache_way_disable;
        boot_img_cfg->hash_ignore = header->bootCfg.bval.hash_ignore;

        /* Firmware len*/
        boot_img_cfg->img_segment_info.img_len = header->img_segment_info.img_len;

        /* Boot entry and flash offset */
        boot_img_cfg->entry_point = header->bootEntry;
        boot_img_cfg->img_start.flash_offset = header->img_start.flash_offset;

        MSG("sign %d,encrypt:%d\r\n", boot_img_cfg->sign_type,
                boot_img_cfg->encrypt_type);

        /* Check encrypt and sign match*/
        if (g_efuse_cfg.encrypted[i] != 0) {
            if (boot_img_cfg->encrypt_type == 0) {
                MSG_ERR("Encrypt not fit\r\n");
                return BFLB_BOOT2_IMG_BOOTHEADER_ENCRYPT_NOTFIT;
            }
        }

        if (g_efuse_cfg.sign[i] ^ boot_img_cfg->sign_type) {
            MSG_ERR("sign not fit\r\n");
            boot_img_cfg->sign_type = g_efuse_cfg.sign[i];
            return BFLB_BOOT2_IMG_BOOTHEADER_SIGN_NOTFIT;
        }

        if (g_ps_mode == BFLB_PSM_HBN && (!g_efuse_cfg.hbn_check_sign)) {
            /* In HBN Mode, if user select to ignore hash and sign*/
            boot_img_cfg->hash_ignore = 1;
        } else if ((boot_img_cfg->hash_ignore == 1 && *phash != BFLB_BOOT2_DEADBEEF_VAL) ||
                   g_efuse_cfg.sign[i] != 0) {
            /* If signed or user not really want to ignore, hash can't be ignored*/
            boot_img_cfg->hash_ignore = 0;
        }

        if (g_user_hash_ignored) {
            boot_img_cfg->hash_ignore = 1;
        }

        ARCH_MemCpy_Fast(boot_img_cfg->img_hash, header->hash, sizeof(header->hash));

        if (boot_img_cfg->img_segment_info.img_len == 0) {
            return BFLB_BOOT2_IMG_SEGMENT_CNT_ERROR;
        }

        /* Start hash here*/
        //Sec_Eng_SHA256_Init(&g_sha_ctx, SEC_ENG_SHA_ID0, SEC_ENG_SHA256, g_sha_tmp_buf, g_padding);
        //Sec_Eng_SHA_Start(SEC_ENG_SHA_ID0);
        device_unregister("dev_check_hash");
        sec_hash_sha256_register(SEC_HASH0_INDEX,"dev_check_hash");
        dev_check_hash = device_find("dev_check_hash");
        if(dev_check_hash){
            ret = device_open(dev_check_hash, 0);
            if(ret){
                MSG_ERR("hash dev open err\r\n");
                return BFLB_BOOT2_FAIL;
            }
        }else{
            MSG_ERR("hash dev find err\r\n");
            return BFLB_BOOT2_FAIL;
        }

    } else {
        MSG_ERR("bootheader crc error\r\n");
        blsp_dump_data((uint8_t *)&crc, 4);
        return BFLB_BOOT2_IMG_BOOTHEADER_CRC_ERROR;
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Parse public key
 *
 * @param  g_boot_img_cfg: Boot image config pointer
 * @param  data: Public key data pointer
 * @param  own: 1 for current CPU, 0 for other(s)
 *
 * @return boot_error_code type
 *
*******************************************************************************/
int32_t blsp_boot_parse_pkey(boot2_image_config *g_boot_img_cfg, uint8_t *data, uint8_t own)
{
    boot_pk_config *cfg = (boot_pk_config *)data;
    uint32_t pk_hash[BFLB_BOOT2_PK_HASH_SIZE / 4];
    int32_t ret;
    

    if (cfg->crc32 == BFLB_Soft_CRC32((uint8_t *)cfg, sizeof(boot_pk_config) - 4)) {
        /* Check public key with data info in OTP*/
        //Sec_Eng_SHA256_Update(&g_sha_ctx, SEC_ENG_SHA_ID0, data, BFLB_BOOT2_ECC_KEYXSIZE + BFLB_BOOT2_ECC_KEYYSIZE);
        //Sec_Eng_SHA256_Finish(&g_sha_ctx, SEC_ENG_SHA_ID0, (uint8_t *)pk_hash);

        //Sec_Eng_SHA256_Init(&g_sha_ctx, SEC_ENG_SHA_ID0, SEC_ENG_SHA256, g_sha_tmp_buf, g_padding);
        //Sec_Eng_SHA_Start(SEC_ENG_SHA_ID0);
        
        device_write(dev_check_hash, 0, data, BFLB_BOOT2_ECC_KEYXSIZE + BFLB_BOOT2_ECC_KEYYSIZE);
        device_read(dev_check_hash,0,pk_hash,0);
    
        device_unregister("dev_check_hash");
        sec_hash_sha256_register(SEC_HASH0_INDEX,"dev_check_hash");
        dev_check_hash = device_find("dev_check_hash");
        if(dev_check_hash){
            ret = device_open(dev_check_hash, 0);
            if(ret){
                MSG_ERR("hash dev open err\r\n");
                return BFLB_BOOT2_FAIL;
            }
        }else{
            MSG_ERR("hash dev find err\r\n");
            return BFLB_BOOT2_FAIL;
        }

        /* Check pk is valid */
        if (own == 1) {
            if (1 != blsp_boot_parse_is_pkhash_valid(g_boot_img_cfg->cpu_type,
                                                     (uint8_t *)pk_hash)) {
                MSG_ERR("PK sha error\r\n");
                return BFLB_BOOT2_IMG_PK_HASH_ERROR;
            }
        }

        if (own == 1) {
            ARCH_MemCpy_Fast(g_boot_img_cfg->eckye_x, cfg->eckye_x, sizeof(cfg->eckye_x));
            ARCH_MemCpy_Fast(g_boot_img_cfg->eckey_y, cfg->eckey_y, sizeof(cfg->eckey_y));
        }
    } else {
        MSG_ERR("PK crc error\r\n");
        return BFLB_BOOT2_IMG_PK_CRC_ERROR;
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Parse signature
 *
 * @param  g_boot_img_cfg: Boot image config pointer
 * @param  data: Signature data pointer
 * @param  own: 1 for current CPU, 0 for other(s)
 *
 * @return boot_error_code type
 *
*******************************************************************************/
int32_t blsp_boot_parse_signature(boot2_image_config *g_boot_img_cfg, uint8_t *data, uint8_t own)
{
    boot_sign_config *cfg = (boot_sign_config *)data;
    uint32_t crc;

    if (cfg->sig_len > sizeof(g_boot_img_cfg->signature)) {
        return BFLB_BOOT2_IMG_SIGNATURE_LEN_ERROR;
    }

    /* CRC include sig_len*/
    crc = BFLB_Soft_CRC32((uint8_t *)&cfg->sig_len, cfg->sig_len + sizeof(cfg->sig_len));

    if (memcmp(&crc, &cfg->signature[cfg->sig_len], 4) == 0) {
        if (own == 1) {
            ARCH_MemCpy_Fast(g_boot_img_cfg->signature, cfg->signature, cfg->sig_len);
            g_boot_img_cfg->sig_len = cfg->sig_len;
        } else {
            ARCH_MemCpy_Fast(g_boot_img_cfg->signature2, cfg->signature, cfg->sig_len);
            g_boot_img_cfg->sig_len2 = cfg->sig_len;
        }
    } else {
        MSG_ERR("SIG crc error\r\n");
        return BFLB_BOOT2_IMG_SIGNATURE_CRC_ERROR;
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Parse ASE IV
 *
 * @param  g_boot_img_cfg: Boot image config pointer
 * @param  data: AES IV data pointer
 *
 * @return boot_error_code type
 *
*******************************************************************************/
int32_t blsp_boot_parse_aesiv(boot2_image_config *g_boot_img_cfg, uint8_t *data)
{
    boot_aes_config *cfg = (boot_aes_config *)data;

    if (cfg->crc32 == BFLB_Soft_CRC32(cfg->aes_iv, sizeof(cfg->aes_iv))) {
        memcpy(g_boot_img_cfg->aes_iv, cfg->aes_iv, sizeof(boot_aes_config));

        /* Update image hash */
        if (!g_boot_img_cfg->hash_ignore) {
            //Sec_Eng_SHA256_Update(&g_sha_ctx, SEC_ENG_SHA_ID0, data, sizeof(boot_aes_config));
            device_write(dev_check_hash, 0, data, sizeof(boot_aes_config));
        }
    } else {
        MSG_ERR("AES IV crc error\r\n");
        return BFLB_BOOT2_IMG_AES_IV_CRC_ERROR;
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************//**
 * @brief  Check signature is valid
 *
 * @param  g_boot_img_cfg: Boot image config pointer
 *
 * @return boot_error_code type
 *
*******************************************************************************/
int32_t blsp_boot_parser_check_signature(boot2_image_config *g_boot_img_cfg)
{
    int32_t ret = 0;
    uint64_t startTime = 0;
    sec_ecdsa_handle_t ecdsaHandle;

    MSG("ps_mode %d,efuse hbn_check_sign %d\r\n", g_ps_mode, g_efuse_cfg.hbn_check_sign);

    if(g_ps_mode == BFLB_PSM_HBN && (!g_efuse_cfg.hbn_check_sign))
    {
        return BFLB_BOOT2_SUCCESS;
    }

    if(g_boot_img_cfg->sign_type)
    {
        MSG("Check sig1\r\n");
        startTime = bflb_platform_get_time_ms();

        sec_ecdsa_init(&ecdsaHandle, ECP_SECP256R1);
        ecdsaHandle.publicKeyx = (uint32_t *)g_boot_img_cfg->eckye_x;
        ecdsaHandle.publicKeyy = (uint32_t *)g_boot_img_cfg->eckey_y;
        sec_ecdsa_verify(&ecdsaHandle, (uint32_t *)g_boot_img_cfg->img_hash, sizeof(g_boot_img_cfg->img_hash) / 4,
                          (uint32_t *)g_boot_img_cfg->signature, (uint32_t *)&g_boot_img_cfg->signature[32]);

        //ret=bflb_ecdsa_verify(0,(uint32_t *)g_boot_img_cfg->img_hash,sizeof(g_boot_img_cfg->img_hash),
        //                        (uint32_t *)g_boot_img_cfg->eckye_x,(uint32_t *)g_boot_img_cfg->eckey_y,
        //                        (uint32_t *)g_boot_img_cfg->signature,(uint32_t *)&g_boot_img_cfg->signature[32]);
        if(ret != 0)
        {
            MSG_ERR("verify failed\n");
            return BFLB_BOOT2_IMG_SIGN_ERROR;
        }

        MSG("Sign suss,Time=%d ms\r\n", (unsigned int)(bflb_platform_get_time_ms() - startTime));
    }

    return BFLB_BOOT2_SUCCESS;
}


/****************************************************************************/ /**
 * @brief  Check hash is valid
 *
 * @param  g_boot_img_cfg: Boot image config pointer
 *
 * @return boot_error_code type
 *
*******************************************************************************/
int32_t blsp_boot_parser_check_hash(boot2_image_config *g_boot_img_cfg)
{
    uint32_t img_hash_cal[BFLB_BOOT2_HASH_SIZE / 4];

    if (!g_boot_img_cfg->hash_ignore) {
        //Sec_Eng_SHA256_Finish(&g_sha_ctx, SEC_ENG_SHA_ID0, (uint8_t *)img_hash_cal);
        device_read(dev_check_hash,0,img_hash_cal,0);
        device_close(dev_check_hash);

        if (memcmp(img_hash_cal, g_boot_img_cfg->img_hash,
                   sizeof(g_boot_img_cfg->img_hash)) != 0) {
            MSG_ERR("Hash error\r\n");
            blsp_dump_data(img_hash_cal, BFLB_BOOT2_HASH_SIZE);
            blsp_dump_data(g_boot_img_cfg->img_hash, BFLB_BOOT2_HASH_SIZE);
            return BFLB_BOOT2_IMG_HASH_ERROR;
        } else {
            MSG("Hash Success\r\n");
        }
    }

    return BFLB_BOOT2_SUCCESS;
}





