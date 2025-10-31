/**
  ******************************************************************************
  * @file    bflb_sp_boot_parser.c
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

#include "bflb_sp_common.h"
#include "bflb_sp_bootinfo.h"
#include "bflb_sp_boot_parser.h"

#include "bflb_sp_port.h"
#include "bflb_sec_sha.h"
#include "bflb_sec_ecdsa.h"

extern uint32_t g_user_hash_ignored;
/****************************************************************************/ /**
 * @brief  Check if the input public key is the same as  burned in the efuse
 *
 * @param  cpu_type: CPU Type
 * @param  pkhash: Public key hash pointer
 *
 * @return 1 for valid and 0 for invalid
 *
*******************************************************************************/
static uint32_t bflb_sp_boot_parse_is_pkhash_valid(uint8_t pk_src, uint8_t *pkhash)
{
    uint32_t i = 0;

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        if ((pk_src == i) && (0 == memcmp(g_efuse_cfg.pk_hash_cpu[i], pkhash, BFLB_BOOT2_PK_HASH_SIZE))) {
            return 1;
        }
    }

    return 0;
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
int32_t bflb_sp_boot_parse_pkey(boot2_image_config *g_boot_img_cfg, uint8_t *data, uint8_t own)
{
    boot_pk_config *cfg = (boot_pk_config *)data;
    uint32_t pk_hash[BFLB_BOOT2_PK_HASH_SIZE / 4];

    if (cfg->crc32 == BFLB_Soft_CRC32((uint8_t *)cfg, sizeof(boot_pk_config) - 4)) {
        /* Check public key with data info in OTP*/
        bflb_sha256_update(sha, &ctx_sha256, data, HAL_BOOT2_ECC_KEYXSIZE + HAL_BOOT2_ECC_KEYYSIZE);
        bflb_sha256_finish(sha, &ctx_sha256, (uint8_t *)pk_hash);

        bflb_sha_init(sha, SHA_MODE_SHA256);
        bflb_sha256_start(sha, &ctx_sha256);
        /* Check pk is valid */
        if (own == 1) {
            if (1 != bflb_sp_boot_parse_is_pkhash_valid(g_boot_img_cfg->pk_src,
                                                     (uint8_t *)pk_hash)) {
                BOOT2_MSG("PK sha error\r\n");
                return BFLB_BOOT2_IMG_PK_HASH_ERROR;
            }
        }

        if (own == 1) {
            arch_memcpy_fast(g_boot_img_cfg->eckye_x, cfg->eckye_x, sizeof(cfg->eckye_x));
            arch_memcpy_fast(g_boot_img_cfg->eckey_y, cfg->eckey_y, sizeof(cfg->eckey_y));
        }
    } else {
        BOOT2_MSG("PK crc error\r\n");
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
int32_t bflb_sp_boot_parse_signature(boot2_image_config *g_boot_img_cfg, uint8_t *data, uint8_t own)
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
            arch_memcpy_fast(g_boot_img_cfg->signature, cfg->signature, cfg->sig_len);
            //g_boot_img_cfg->sig_len = cfg->sig_len;
        } else {
            arch_memcpy_fast(g_boot_img_cfg->signature2, cfg->signature, cfg->sig_len);
            //g_boot_img_cfg->sig_len2 = cfg->sig_len;
        }
    } else {
        BOOT2_MSG("SIG crc error\r\n");
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
int32_t bflb_sp_boot_parse_aesiv(boot2_image_config *g_boot_img_cfg, uint8_t *data)
{
    boot_aes_config *cfg = (boot_aes_config *)data;

    if (cfg->crc32 == BFLB_Soft_CRC32(cfg->aes_iv, sizeof(cfg->aes_iv))) {
        memcpy(g_boot_img_cfg->aes_iv, cfg->aes_iv, sizeof(boot_aes_config));

        /* Update image hash */
        if (!g_boot_img_cfg->basic_cfg.hash_ignore) {
            //Sec_Eng_SHA256_Update(&g_sha_ctx, SEC_ENG_SHA_ID0, data, sizeof(boot_aes_config));
#ifndef CHIP_WB03
            bflb_sha256_update(sha, &ctx_sha256, data, sizeof(boot_aes_config));
#endif
        }
    } else {
        BOOT2_MSG("AES IV crc error\r\n");
        return BFLB_BOOT2_IMG_AES_IV_CRC_ERROR;
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Check signature is valid
 *
 * @param  g_boot_img_cfg: Boot image config pointer
 *
 * @return boot_error_code type
 *
*******************************************************************************/
int32_t bflb_sp_boot_parser_check_signature(boot2_image_config *g_boot_img_cfg)
{
    int32_t ret = 0;
    uint64_t startTime = 0;
    struct bflb_ecdsa_s ecdsa_handle;

    BOOT2_MSG_DBG("ps_mode %d,efuse hbn_check_sign %d\r\n", g_ps_mode, g_efuse_cfg.hbn_check_sign);

    if (g_ps_mode == BFLB_PSM_HBN && (!g_efuse_cfg.hbn_check_sign)) {
        return BFLB_BOOT2_SUCCESS;
    }

    if (g_boot_img_cfg->basic_cfg.sign_type) {
        BOOT2_MSG_DBG("Check sig1\r\n");
        startTime = bflb_mtimer_get_time_ms();
        bflb_sec_ecdsa_init(&ecdsa_handle, ECP_SECP256R1);

        ecdsa_handle.publicKeyx = (uint32_t *)g_boot_img_cfg->eckye_x;
        ecdsa_handle.publicKeyy = (uint32_t *)g_boot_img_cfg->eckey_y;

        ret = bflb_sec_ecdsa_verify(&ecdsa_handle, (uint32_t *)g_boot_img_cfg->basic_cfg.hash, 8,
                                    (uint32_t *)g_boot_img_cfg->signature, (uint32_t *)&g_boot_img_cfg->signature[32]);
        if (ret != 0) {
            BOOT2_MSG_DBG("verify failed\r\n");
            return BFLB_BOOT2_IMG_SIGN_ERROR;
        }

        BOOT2_MSG_DBG("Sign suss,Time=%d ms\r\n", (unsigned int)(bflb_mtimer_get_time_ms() - startTime));
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
int32_t bflb_sp_boot_parser_check_hash(boot2_image_config *g_boot_img_cfg)
{
    uint32_t img_hash_cal[HAL_BOOT2_IMG_HASH_SIZE / 4];

    if (!g_boot_img_cfg->basic_cfg.hash_ignore) {
        //Sec_Eng_SHA256_Finish(&g_sha_ctx, SEC_ENG_SHA_ID0, (uint8_t *)img_hash_cal);
        bflb_sha256_finish(sha, &ctx_sha256, (uint8_t *)img_hash_cal);

        if (memcmp(img_hash_cal, g_boot_img_cfg->basic_cfg.hash, 32) != 0) {
            BOOT2_MSG_ERR("Hash error\r\n");
            return BFLB_BOOT2_IMG_HASH_ERROR;
        } else {
            BOOT2_MSG_DBG("Hash Success\r\n");
        }
    }

    return BFLB_BOOT2_SUCCESS;
}
