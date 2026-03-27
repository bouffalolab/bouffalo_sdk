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
static uint32_t bflb_sp_boot_parse_is_pkhash_valid(uint8_t pk_src, uint8_t *pkhash, uint8_t sign_type)
{
    uint32_t i = 0;
    uint32_t hash_size;

#if HAL_BOOT2_SUPPORT_SIGN_SHA384
    if (sign_type == HAL_BOOT_SIGN_TYPE_ECC_SHA384) {
        hash_size = HAL_BOOT2_PK_HASH_SIZE_SHA384; /* 48 bytes */
    } else
#endif
    {
        hash_size = HAL_BOOT2_PK_HASH_SIZE; /* 32 bytes */
    }

    if (pk_src >= HAL_BOOT2_CPU_GROUP_MAX) {
        return 0;
    }

#if defined(CHIP_BL616CL)
    if ((sign_type == HAL_BOOT_SIGN_TYPE_ECC_SHA256) &&
        (g_efuse_cfg.pkhash_sel == HAL_APP_SIGN_PKHASH_FROM_EFUSE) &&
        (g_efuse_cfg.pkhash_len == 0)) {
        hash_size = HAL_BOOT2_PK_HASH_SIZE_LEN192;
    }
#endif

    for (i = 0; i < HAL_BOOT2_CPU_GROUP_MAX; i++) {
        if ((pk_src == i) && (0 == memcmp(g_efuse_cfg.pk_hash_cpu[i], pkhash, hash_size))) {
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
#if HAL_BOOT2_SUPPORT_SIGN_SHA384
    uint32_t pk_hash[HAL_BOOT2_PK_HASH_SIZE_SHA384 / 4]; /* 48 bytes */
#else
    uint32_t pk_hash[HAL_BOOT2_PK_HASH_SIZE / 4]; /* 32 bytes */
#endif

#if HAL_BOOT2_SUPPORT_SIGN_SHA384
    if (g_boot_img_cfg->basic_cfg.sign_type == HAL_BOOT_SIGN_TYPE_ECC_SHA384) {
        boot_pk_sha384_config *cfg = (boot_pk_sha384_config *)data;

        if (cfg->crc32 == BFLB_Soft_CRC32((uint8_t *)cfg, sizeof(boot_pk_sha384_config) - 4)) {
            /* Check public key with data info in OTP using SHA384 */
            bflb_sha512_update(sha, &ctx_sha384, data, HAL_BOOT2_ECC_KEYXSIZE_SHA384 + HAL_BOOT2_ECC_KEYYSIZE_SHA384);
            bflb_sha512_finish(sha, &ctx_sha384, (uint8_t *)pk_hash);

            bflb_sha_init(sha, SHA_MODE_SHA384);
            bflb_sha512_start(sha, &ctx_sha384);

            /* Check pk is valid */
            if (own == 1) {
                if (1 != bflb_sp_boot_parse_is_pkhash_valid(g_boot_img_cfg->pk_src, (uint8_t *)pk_hash,
                                                            g_boot_img_cfg->basic_cfg.sign_type)) {
                    BOOT2_MSG("PK SHA384 sha error\r\n");
                    return BFLB_BOOT2_IMG_PK_HASH_ERROR;
                }
            }

            if (own == 1) {
                arch_memcpy_fast(g_boot_img_cfg->eckey_x, cfg->eckey_x, sizeof(cfg->eckey_x));
                arch_memcpy_fast(g_boot_img_cfg->eckey_y, cfg->eckey_y, sizeof(cfg->eckey_y));
            }
        } else {
            BOOT2_MSG("PK SHA384 crc error\r\n");
            return BFLB_BOOT2_IMG_PK_CRC_ERROR;
        }
    } else
#endif
    {
        /* SHA256 */
        boot_pk_config *cfg = (boot_pk_config *)data;

        if (cfg->crc32 == BFLB_Soft_CRC32((uint8_t *)cfg, sizeof(boot_pk_config) - 4)) {
            /* Check public key with data info in OTP using SHA256 */
            bflb_sha256_update(sha, &ctx_sha256, data, HAL_BOOT2_ECC_KEYXSIZE + HAL_BOOT2_ECC_KEYYSIZE);
            bflb_sha256_finish(sha, &ctx_sha256, (uint8_t *)pk_hash);

            bflb_sha_init(sha, SHA_MODE_SHA256);
            bflb_sha256_start(sha, &ctx_sha256);

            /* Check pk is valid */
            if (own == 1) {
                if (1 != bflb_sp_boot_parse_is_pkhash_valid(g_boot_img_cfg->pk_src, (uint8_t *)pk_hash,
                                                            g_boot_img_cfg->basic_cfg.sign_type)) {
                    BOOT2_MSG("PK sha error\r\n");
                    return BFLB_BOOT2_IMG_PK_HASH_ERROR;
                }
            }

            if (own == 1) {
                arch_memcpy_fast(g_boot_img_cfg->eckey_x, cfg->eckey_x, sizeof(cfg->eckey_x));
                arch_memcpy_fast(g_boot_img_cfg->eckey_y, cfg->eckey_y, sizeof(cfg->eckey_y));
            }
        } else {
            BOOT2_MSG("PK crc error\r\n");
            return BFLB_BOOT2_IMG_PK_CRC_ERROR;
        }
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
    uint32_t max_sig_len;

#if HAL_BOOT2_SUPPORT_SIGN_SHA384
    if (g_boot_img_cfg->basic_cfg.sign_type == HAL_BOOT_SIGN_TYPE_ECC_SHA384) {
        max_sig_len = HAL_BOOT2_SIGN_MAXSIZE_SHA384;
    } else
#endif
    {
        max_sig_len = HAL_BOOT2_SIGN_MAXSIZE;
    }

    if (cfg->sig_len > max_sig_len) {
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
        arch_memcpy_fast(g_boot_img_cfg->aes_iv, cfg->aes_iv, sizeof(boot_aes_config));

        /* Update image hash */
        if (!g_boot_img_cfg->basic_cfg.hash_ignore) {
#if HAL_BOOT2_SUPPORT_SIGN_SHA384
            if (g_boot_img_cfg->basic_cfg.sign_type == HAL_BOOT_SIGN_TYPE_ECC_SHA384) {
                bflb_sha512_update(sha, &ctx_sha384, data, sizeof(boot_aes_config));
            } else
#endif
            {
#ifndef CHIP_WB03
                bflb_sha256_update(sha, &ctx_sha256, data, sizeof(boot_aes_config));
#endif
            }
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
    uint32_t curve_type;
    uint32_t signature_size;
    uint32_t hash_len_words;
#if HAL_BOOT2_SUPPORT_SIGN_SHA384
    uint32_t pk_hash[HAL_BOOT2_PK_HASH_SIZE_SHA384 / 4]; /* 48 bytes */
#else
    uint32_t pk_hash[HAL_BOOT2_PK_HASH_SIZE / 4]; /* 32 bytes */
#endif

    BOOT2_MSG_DBG("ps_mode %d,efuse hbn_check_sign %d\r\n", g_ps_mode, g_efuse_cfg.hbn_check_sign);

    if (g_ps_mode == BFLB_PSM_HBN && (!g_efuse_cfg.hbn_check_sign)) {
        return BFLB_BOOT2_SUCCESS;
    }

    if (g_boot_img_cfg->basic_cfg.sign_type) {
#if HAL_BOOT2_SUPPORT_SIGN_SHA384
        if (g_boot_img_cfg->basic_cfg.sign_type == HAL_BOOT_SIGN_TYPE_ECC_SHA384) {
            curve_type = ECP_SECP384R1;
            signature_size = 48; /* 384 bits = 48 bytes */
            hash_len_words = 12; /* 48 bytes / 4 = 12 words */
            arch_memcpy_fast(pk_hash, (uint32_t *)g_boot_img_cfg->basic_cfg.hash, HAL_BOOT2_PK_HASH_SIZE);
            arch_memcpy_fast(&pk_hash[HAL_BOOT2_PK_HASH_SIZE / 4], g_boot_img_cfg->hash_384_ext, 16);
        } else
#endif
        {
            curve_type = ECP_SECP256R1;
            signature_size = 32; /* 256 bits = 32 bytes */
            hash_len_words = 8;  /* 32 bytes / 4 = 8 words */
            arch_memcpy_fast(pk_hash, (uint32_t *)g_boot_img_cfg->basic_cfg.hash, HAL_BOOT2_PK_HASH_SIZE);
        }

        BOOT2_MSG_DBG("Check sig1\r\n");
        startTime = bflb_mtimer_get_time_ms();
        bflb_sec_ecdsa_init(&ecdsa_handle, curve_type);

        ecdsa_handle.publicKeyx = (uint32_t *)g_boot_img_cfg->eckey_x;
        ecdsa_handle.publicKeyy = (uint32_t *)g_boot_img_cfg->eckey_y;

        ret = bflb_sec_ecdsa_verify(&ecdsa_handle, pk_hash, hash_len_words, (uint32_t *)g_boot_img_cfg->signature,
                                    (uint32_t *)&g_boot_img_cfg->signature[signature_size]);
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
#if HAL_BOOT2_SUPPORT_SIGN_SHA384
    uint32_t img_hash_cal[HAL_BOOT2_IMG_HASH_SIZE_SHA384 / 4];
    uint32_t img_header_hash[12] = { 0 }; /* Max 48 bytes for SHA384 */
#else
    uint32_t img_hash_cal[HAL_BOOT2_IMG_HASH_SIZE / 4];
    uint32_t img_header_hash[8] = { 0 }; /* 32 bytes for SHA256 */
#endif
    uint32_t hash_size;

    if (!g_boot_img_cfg->basic_cfg.hash_ignore) {
        /* Copy first 32 bytes from basic_cfg.hash */
        arch_memcpy_fast(img_header_hash, g_boot_img_cfg->basic_cfg.hash, HAL_BOOT2_IMG_HASH_SIZE);

#if HAL_BOOT2_SUPPORT_SIGN_SHA384
        if (g_boot_img_cfg->basic_cfg.sign_type == HAL_BOOT_SIGN_TYPE_ECC_SHA384) {
            arch_memcpy_fast(&img_header_hash[8], g_boot_img_cfg->hash_384_ext, 16);
            bflb_sha512_finish(sha, &ctx_sha384, (uint8_t *)img_hash_cal);
            hash_size = HAL_BOOT2_IMG_HASH_SIZE_SHA384; /* 48 bytes */
        } else
#endif
        {
            bflb_sha256_finish(sha, &ctx_sha256, (uint8_t *)img_hash_cal);
            hash_size = HAL_BOOT2_IMG_HASH_SIZE; /* 32 bytes */
        }

        if (memcmp(img_hash_cal, img_header_hash, hash_size) != 0) {
            BOOT2_MSG_ERR("Hash error\r\n");
            return BFLB_BOOT2_IMG_HASH_ERROR;
        } else {
            BOOT2_MSG_DBG("Hash Success\r\n");
        }
    }

    return BFLB_BOOT2_SUCCESS;
}
