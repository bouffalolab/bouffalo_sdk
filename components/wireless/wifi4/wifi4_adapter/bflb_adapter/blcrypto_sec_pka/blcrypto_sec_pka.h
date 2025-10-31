#ifndef BLCRYPTO_SEC_PKA_H_
#define BLCRYPTO_SEC_PKA_H_
#include <bl602_common.h>
#include "hardware/sec_eng_reg.h"
#include "bflb_sec_aes.h"
#include "bflb_sec_pka.h"

extern struct bflb_device_s *bflb_bignum_pka;

#define Sec_Eng_PKA_MADD(dt, di, s0t, s0i, s1t, s1i, s2t, s2i, lastop) bflb_pka_madd(bflb_bignum_pka, s0i, s0t, di, dt, s1i, s1t, s2i, s2t, lastop)
#define Sec_Eng_PKA_MSUB(dt, di, s0t, s0i, s1t, s1i, s2t, s2i, lastop) bflb_pka_msub(bflb_bignum_pka, s0i, s0t, di, dt, s1i, s1t, s2i, s2t, lastop)
#define Sec_Eng_PKA_MMUL(dt, di, s0t, s0i, s1t, s1i, s2t, s2i, lastop) bflb_pka_mmul(bflb_bignum_pka, s0i, s0t, di, dt, s1i, s1t, s2i, s2t, lastop)
#define Sec_Eng_PKA_Write_Data(dt, di, data, len, lastop) bflb_pka_write(bflb_bignum_pka, di, dt, data, len, lastop)
#define Sec_Eng_PKA_Read_Data(dt, di, data, len) bflb_pka_read(bflb_bignum_pka, di, dt, data, len)
#define Sec_Eng_PKA_Move_Data(dt, di, s0t, s0i, lastop) bflb_pka_movdat(bflb_bignum_pka, s0i, s0t, di, dt, lastop)
#define Sec_Eng_PKA_MREM(dt, di, s0t, s0i, s2t, s2i, lastop) bflb_pka_mrem(bflb_bignum_pka, s0i, s0t, di, dt, s2i, s2t, lastop)
#define Sec_Eng_PKA_MINV(dt, di, s0t, s0i, s2t, s2i, lastop) bflb_pka_minv(bflb_bignum_pka, s0i, s0t, di, dt, s2i, s2t, lastop)
#define Sec_Eng_PKA_MSQR(dt, di, s0t, s0i, s2t, s2i, lastop) bflb_pka_msqr(bflb_bignum_pka, s0i, s0t, di, dt, s2i, s2t, lastop)
#define Sec_Eng_PKA_CREG(dt, di, size, lastop) bflb_pka_clir(bflb_bignum_pka, di, dt, size, lastop)
#define Sec_Eng_PKA_Write_Immediate(dt, di, data, lastop) bflb_pka_slir(bflb_bignum_pka, di, dt, data, lastop)
#define Sec_Eng_PKA_LMUL2N(dt, di, s0t, s0i, bitshift, lastop) bflb_pka_lmul2n(bflb_bignum_pka, s0i, s0t, di, dt, bitshift, lastop)
#define Sec_Eng_PKA_Reset() bflb_bignum_pka = bflb_device_get_by_name(BFLB_NAME_SEC_PKA); bflb_pka_init(bflb_bignum_pka)
#define Sec_Eng_PKA_GF2Mont(dt, di, st, si, size, tt, ti, pt, pi) bflb_pka_gf2mont(bflb_bignum_pka, si, st, di, dt, ti, tt, pi, pt ,size)
#define Sec_Eng_PKA_MEXP(dt, di, s0t, s0i, s1t, s1i, s2t, s2i, lastop) bflb_pka_mexp(bflb_bignum_pka, s0i, s0t, di, dt, s1i, s1t, s2i, s2t, lastop)
#define Sec_Eng_PKA_Mont2GF(dt, di, at, ai, invrt, invri, tt, ti, pt, pi) bflb_pka_mont2gf(bflb_bignum_pka, ai, at, di, dt, invri, invrt, ti, tt, pi, pt)
#define Sec_Eng_PKA_BigEndian_Enable()
#define AES_ID SEC_ENG_AES_ID0

#undef MSG
#ifndef CFG_SDIO_WIFI
#define MSG printf
#else
#include "phy_dbg.h"
#define MSG phy_dbg
#endif

#define SEC_ENG_SE_AES_0_LINK_MODE_POS                          (15U)
#define SEC_ENG_AES_BUSY_TIMEOUT_COUNT                          (100*160*1000)
#define SEC_ENG_SE_AES_0_BUSY_POS                               (0U)
#define SEC_ENG_SE_AES_0_EN_POS                                 (2U)
#define SEC_ENG_SE_AES_0_TRIG_1T_POS                            (1U)
#define SEC_ENG_SE_AES_0_EN_UMSK                                (~(((1U << SEC_ENG_SE_AES_0_EN_LEN) - 1) << SEC_ENG_SE_AES_0_EN_POS))
#define SEC_ENG_SE_AES_0_EN_LEN                                 (1U)

typedef int SEC_ENG_PKA_REG_SIZE_Type;

static inline void Sec_Eng_PKA_LCMP(uint8_t*out, uint8_t s0t, uint8_t s0i, uint8_t s1t, uint8_t s1i)
{
    *out = bflb_pka_lcmp(bflb_bignum_pka, s0i, s0t,s1i, s1t );
}

typedef struct
{
    uint32_t              : 3;  /*!< [2:0]Reserved */
    uint32_t aesMode      : 2;  /*!< [4:3]128-bit/256-bit/192-bit/128-bit-double key mode select */
    uint32_t aesDecEn     : 1;  /*!< [5]Encode or decode */
    uint32_t aesDecKeySel : 1;  /*!< [6]Use new key or use same key as last one */
    uint32_t aesHwKeyEn   : 1;  /*!< [7]Enable or disable using hardware hey */
    uint32_t              : 1;  /*!< [8]Reserved */
    uint32_t aesIntClr    : 1;  /*!< [9]Clear interrupt */
    uint32_t aesIntSet    : 1;  /*!< [10]Set interrupt */
    uint32_t              : 1;  /*!< [11]Reserved */
    uint32_t aesBlockMode : 2;  /*!< [13:12]ECB/CTR/CBC mode select */
    uint32_t aesIVSel     : 1;  /*!< [14]Use new iv or use same iv as last one */
    uint32_t aesXTS       : 1;  /*!< [15]XTS mode select */
    uint32_t aesMsgLen    : 16; /*!< [31:16]Number of 128-bit block */
    uint32_t aesSrcAddr;        /*!< Message source address */
    uint32_t aesDstAddr;        /*!< Message destination address */
    uint32_t aesIV0;            /*!< Big endian initial vector(MSB) */
    uint32_t aesIV1;            /*!< Big endian initial vector */
    uint32_t aesIV2;            /*!< Big endian initial vector */
    uint32_t aesIV3;            /*!< Big endian initial vector(LSB)(CTR mode:counter initial value) */
    uint32_t aesKey0;           /*!< Big endian aes key(aes-128/256 key MSB) */
    uint32_t aesKey1;           /*!< Big endian aes key */
    uint32_t aesKey2;           /*!< Big endian aes key */
    uint32_t aesKey3;           /*!< Big endian aes key(aes-128 key LSB) */
    uint32_t aesKey4;           /*!< Big endian aes key */
    uint32_t aesKey5;           /*!< Big endian aes key */
    uint32_t aesKey6;           /*!< Big endian aes key */
    uint32_t aesKey7;           /*!< Big endian aes key(aes-256 key LSB) */
} __attribute__((aligned(4))) bl_SEC_Eng_AES_Link_Config_Type;

typedef enum {
    BL_AES_ENCRYPT,
    BL_AES_DECRYPT,
} bflb_crypto_sec_aes_op_t;

typedef struct {
    uint32_t guard0_[7];
    bl_SEC_Eng_AES_Link_Config_Type link_cfg;
    uint32_t guard1_[7];
} bflb_crypto_sec_aes_t;

typedef enum {
    SEC_ENG_AES_KEY_128BITS,
    SEC_ENG_AES_KEY_256BITS,
    SEC_ENG_AES_KEY_192BITS,
    SEC_ENG_AES_KEY_DOUBLE_128BITS,
} SEC_ENG_AES_Key_Type;

typedef enum {
    SEC_ENG_AES_USE_NEW,                     /*!< Use new value */
    SEC_ENG_AES_USE_OLD,                     /*!< Use old value same as last one */
}SEC_ENG_AES_ValueUsed_Type;

typedef enum {
    SEC_ENG_AES_ENCRYPTION,                  /*!< AES encryption */
    SEC_ENG_AES_DECRYPTION,                  /*!< AES decryption */
}SEC_ENG_AES_EnDec_Type;

typedef enum {
    SEC_ENG_AES_ECB, /*!< AES mode type:ECB */
    SEC_ENG_AES_CTR, /*!< AES mode type:CTR */
    SEC_ENG_AES_CBC, /*!< AES mode type:CBC */
    SEC_ENG_AES_XTS, /*!< AES mode type:XTS */
} SEC_ENG_AES_Type;

typedef enum {
    SEC_ENG_AES_ID0,
}SEC_ENG_AES_ID_Type;

int bflb_crypto_crypto_aes_acquire_hw();
int bflb_crypto_aes_release_hw();
int bflb_crypto_aes_init(bflb_crypto_sec_aes_t *aes);
int bflb_crypto_aes_set_key(bflb_crypto_sec_aes_t *aes, bflb_crypto_sec_aes_op_t op, const uint8_t *key, size_t key_len);
int bflb_crypto_aes_transform_blocks(bflb_crypto_sec_aes_t *aes, bflb_crypto_sec_aes_op_t op, const uint8_t *input, uint16_t n_blk, uint8_t *output);
int bflb_crypto_aes_transform(bflb_crypto_sec_aes_t *aes, bflb_crypto_sec_aes_op_t op, const uint8_t *input, uint8_t *output);
#endif
