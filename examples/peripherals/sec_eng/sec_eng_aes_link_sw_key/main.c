#include "bflb_mtimer.h"
#include "bflb_sec_aes.h"
#include "board.h"

const uint8_t aes_ecb_128bit_pt[16] = {
    0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
    0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
};

const uint8_t aes_ecb_128bit_ct[16] = {
    0x39, 0x25, 0x84, 0x1d, 0x02, 0xDc, 0x09, 0xfb,
    0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32
};

const uint8_t aes_cbc_128bit_pt[48] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};

const uint8_t aes_cbc_128bit_ct[48] = {
    0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
    0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
    0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee,
    0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
    0x7f, 0xee, 0xdb, 0x79, 0xc5, 0x87, 0x1f, 0x16,
    0x59, 0xd4, 0x60, 0xa6, 0x76, 0xd1, 0xc0, 0x18
};

const uint8_t aes_ctr_128bit_pt[16] = {
    0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
    0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A
};

const uint8_t aes_ctr_128bit_ct[16] = {
    0x87, 0x4D, 0x61, 0x91, 0xB6, 0x20, 0xE3, 0x26,
    0x1B, 0xEF, 0x68, 0x64, 0x99, 0x0D, 0xB6, 0xCE
};

const uint8_t aes_xts_128bit_pt[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t aes_xts_128bit_ct[64] = {
    0x91, 0x7c, 0xf6, 0x9e, 0xbd, 0x68, 0xb2, 0xec, 0x9b, 0x9f, 0xe9, 0xa3, 0xea, 0xdd, 0xa6, 0x92,
    0xcd, 0x43, 0xd2, 0xf5, 0x95, 0x98, 0xed, 0x85, 0x8c, 0x02, 0xc2, 0x65, 0x2f, 0xbf, 0x92, 0x2e,
    0xc4, 0xcf, 0xc5, 0xa3, 0x8b, 0x75, 0x84, 0x30, 0xd5, 0x16, 0x51, 0x18, 0x7f, 0x65, 0xbb, 0xe6,
    0x16, 0x6b, 0xc3, 0x60, 0xd7, 0xbb, 0x56, 0xe9, 0xbb, 0xe4, 0x54, 0x10, 0x72, 0x2e, 0x46, 0x75
};

const uint8_t aes_xts_192bit_pt[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const uint8_t aes_xts_192bit_ct[64] = {
    0x82, 0xba, 0x63, 0x7a, 0xd6, 0xfb, 0x48, 0xe1, 0x5c, 0xc2, 0x3e, 0x8e, 0x36, 0xf9, 0x63, 0x53,
    0x0c, 0xd8, 0xda, 0x20, 0x87, 0x90, 0xdf, 0x03, 0x82, 0x97, 0xe5, 0x65, 0xee, 0xe2, 0x54, 0xa5,
    0xe8, 0x3d, 0x20, 0xa1, 0x23, 0xd2, 0xcc, 0x22, 0xe1, 0x6e, 0xc2, 0x14, 0x49, 0x19, 0xdc, 0x09,
    0x4c, 0x1c, 0xdb, 0xe1, 0xbd, 0xc1, 0x5b, 0xa6, 0xdf, 0x7a, 0x71, 0xc7, 0x9e, 0x11, 0x62, 0xdb

};

const uint8_t aes_xts_256bit_pt[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* key1=000102030405060708090a0b0c0d0e0ffffefdfcfbfaf9f8f7f6f5f4f3f2f1f0
   key2=fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0000102030405060708090a0b0c0d0e0f*/
const uint8_t aes_xts_256bit_ct[64] = {
    0x06, 0x87, 0xea, 0x7b, 0xf9, 0x87, 0x4b, 0x5a, 0xa6, 0xa5, 0xf1, 0x23, 0x6e, 0xda, 0x73, 0x0a,
    0x73, 0x8d, 0x30, 0x6c, 0x9d, 0x4f, 0xc1, 0x6b, 0xfb, 0x47, 0x2c, 0x58, 0x68, 0x16, 0xfd, 0x07,
    0xe9, 0x59, 0x84, 0xd9, 0x36, 0x84, 0xcd, 0xfc, 0x77, 0xce, 0x30, 0xdf, 0x6d, 0x49, 0xef, 0x54,
    0x66, 0xb2, 0x7f, 0x06, 0x46, 0xbb, 0xb2, 0x46, 0x00, 0xf8, 0x6b, 0x0c, 0xc0, 0x5f, 0xb5, 0x20
};

ATTR_NOCACHE_RAM_SECTION struct bflb_aes_link_s aes_ecb_link = {
    .aes_key = AES_LINK_KEY_128BITS, /* 128-bit key mode select */
    .aes_dec_en = 0,                 /* Encode */
    .aes_newkey_dis = 0,             /* Use new key */
    .aes_hwkey_en = 0,               /* Use sw key */
    .aes_intclr = 0,                 /* Not clear intrrupt */
    .aes_intset = 0,                 /* Not set interrupt */
    .aes_mode = AES_MODE_ECB,        /* ECB mode select */
    .aes_newiv_dis = 0,              /* Use new iv */
    .aes_xts = 0,                    /* XTS mode select */
    .aes_msglen = 1,                 /* Number of 128-bit block */
    .aes_srcaddr = 0,                /* Message source address */
    .aes_dstaddr = 0,                /* Message destination address */
    .aes_iv0 = 0,                    /* Big endian initial vector(MSB) */
    .aes_iv1 = 0,                    /* Big endian initial vector */
    .aes_iv2 = 0,                    /* Big endian initial vector */
    .aes_iv3 = 0,                    /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
    .aes_key0 = 0x16157e2b,          /* Big endian aes key(aes-128/256 key MSB) */
    .aes_key1 = 0xa6d2ae28,          /* Big endian aes key */
    .aes_key2 = 0x8815f7ab,          /* Big endian aes key */
    .aes_key3 = 0x3c4fcf09,          /* Big endian aes key(aes-128 key LSB) */
    .aes_key4 = 0,                   /* Big endian aes key */
    .aes_key5 = 0,                   /* Big endian aes key */
    .aes_key6 = 0,                   /* Big endian aes key */
    .aes_key7 = 0                    /* Big endian aes key(aes-256 key LSB) */
};

ATTR_NOCACHE_RAM_SECTION struct bflb_aes_link_s aes_cbc_link = {
    .aes_key = AES_LINK_KEY_128BITS, /* 128-bit key mode select */
    .aes_dec_en = 0,                 /* Encode */
    .aes_newkey_dis = 0,             /* Use new key */
    .aes_hwkey_en = 0,               /* Use sw key */
    .aes_intclr = 0,                 /* Not clear intrrupt */
    .aes_intset = 0,                 /* Not set interrupt */
    .aes_mode = AES_MODE_CBC,        /* ECB mode select */
    .aes_newiv_dis = 0,              /* Use new iv */
    .aes_xts = 0,                    /* XTS mode select */
    .aes_msglen = 1,                 /* Number of 128-bit block */
    .aes_srcaddr = 0,                /* Message source address */
    .aes_dstaddr = 0,                /* Message destination address */
    .aes_iv0 = 0x03020100,           /* Big endian initial vector(MSB) */
    .aes_iv1 = 0x07060504,           /* Big endian initial vector */
    .aes_iv2 = 0x0b0a0908,           /* Big endian initial vector */
    .aes_iv3 = 0x0f0e0d0c,           /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
    .aes_key0 = 0x16157e2b,          /* Big endian aes key(aes-128/256 key MSB) */
    .aes_key1 = 0xa6d2ae28,          /* Big endian aes key */
    .aes_key2 = 0x8815f7ab,          /* Big endian aes key */
    .aes_key3 = 0x3c4fcf09,          /* Big endian aes key(aes-128 key LSB) */
    .aes_key4 = 0,                   /* Big endian aes key */
    .aes_key5 = 0,                   /* Big endian aes key */
    .aes_key6 = 0,                   /* Big endian aes key */
    .aes_key7 = 0                    /* Big endian aes key(aes-256 key LSB) */
};

ATTR_NOCACHE_RAM_SECTION struct bflb_aes_link_s aes_cbc_link2 = {
    .aes_key = AES_LINK_KEY_128BITS, /* 128-bit key mode select */
    .aes_dec_en = 0,                 /* Encode */
    .aes_newkey_dis = 0,             /* Use new key */
    .aes_hwkey_en = 0,               /* Use sw key */
    .aes_intclr = 0,                 /* Not clear intrrupt */
    .aes_intset = 0,                 /* Not set interrupt */
    .aes_mode = AES_MODE_CBC,        /* ECB mode select */
    .aes_newiv_dis = 0,              /* Use new iv */
    .aes_xts = 0,                    /* XTS mode select */
    .aes_msglen = 1,                 /* Number of 128-bit block */
    .aes_srcaddr = 0,                /* Message source address */
    .aes_dstaddr = 0,                /* Message destination address */
    .aes_iv0 = 0x03020100,           /* Big endian initial vector(MSB) */
    .aes_iv1 = 0x07060504,           /* Big endian initial vector */
    .aes_iv2 = 0x0b0a0908,           /* Big endian initial vector */
    .aes_iv3 = 0x0f0e0d0c,           /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
    .aes_key0 = 0x16157e2b,          /* Big endian aes key(aes-128/256 key MSB) */
    .aes_key1 = 0xa6d2ae28,          /* Big endian aes key */
    .aes_key2 = 0x8815f7ab,          /* Big endian aes key */
    .aes_key3 = 0x3c4fcf09,          /* Big endian aes key(aes-128 key LSB) */
    .aes_key4 = 0,                   /* Big endian aes key */
    .aes_key5 = 0,                   /* Big endian aes key */
    .aes_key6 = 0,                   /* Big endian aes key */
    .aes_key7 = 0                    /* Big endian aes key(aes-256 key LSB) */
};

ATTR_NOCACHE_RAM_SECTION struct bflb_aes_link_s aes_ctr_link = {
    .aes_key = AES_LINK_KEY_128BITS, /* 128-bit key mode select */
    .aes_dec_en = 0,                 /* Encode */
    .aes_newkey_dis = 0,             /* Use new key */
    .aes_hwkey_en = 0,               /* Use sw key */
    .aes_intclr = 0,                 /* Not clear intrrupt */
    .aes_intset = 0,                 /* Not set interrupt */
    .aes_mode = AES_MODE_CTR,        /* ECB mode select */
    .aes_newiv_dis = 0,              /* Use new iv */
    .aes_xts = 0,                    /* XTS mode select */
    .aes_msglen = 1,                 /* Number of 128-bit block */
    .aes_srcaddr = 0,                /* Message source address */
    .aes_dstaddr = 0,                /* Message destination address */
    .aes_iv0 = 0xf3f2f1f0,           /* Big endian initial vector(MSB) */
    .aes_iv1 = 0xf7f6f5f4,           /* Big endian initial vector */
    .aes_iv2 = 0xfbfaf9f8,           /* Big endian initial vector */
    .aes_iv3 = 0xfffefdfc,           /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
    .aes_key0 = 0x16157e2b,          /* Big endian aes key(aes-128/256 key MSB) */
    .aes_key1 = 0xa6d2ae28,          /* Big endian aes key */
    .aes_key2 = 0x8815f7ab,          /* Big endian aes key */
    .aes_key3 = 0x3c4fcf09,          /* Big endian aes key(aes-128 key LSB) */
    .aes_key4 = 0,                   /* Big endian aes key */
    .aes_key5 = 0,                   /* Big endian aes key */
    .aes_key6 = 0,                   /* Big endian aes key */
    .aes_key7 = 0                    /* Big endian aes key(aes-256 key LSB) */
};

ATTR_NOCACHE_RAM_SECTION struct bflb_aes_xts_link_s aes_xts_link1 = {
    .aes_key = AES_LINK_KEY_128BITS, /* 128-bit key mode select */
    .aes_dec_en = 0,                 /* Encode */
    .aes_newkey_dis = 0,             /* Use new key */
    .aes_hwkey_en = 0,               /* Use sw key */
    .aes_intclr = 0,                 /* Not clear intrrupt */
    .aes_intset = 0,                 /* Not set interrupt */
    .aes_mode = AES_MODE_XTS,        /* ECB mode select */
    .aes_newiv_dis = 0,              /* Use new iv */
    .aes_xts = AES_LINK_XTS_MODE1,   /* XTS mode select */
    .aes_msglen = 1,                 /* Number of 128-bit block */
    .aes_srcaddr = 0,                /* Message source address */
    .aes_dstaddr = 0,                /* Message destination address */
    .aes_iv0 = 0x00000000,           /* Big endian initial vector(MSB) */
    .aes_iv1 = 0x00000000,           /* Big endian initial vector */
    .aes_iv2 = 0x00000000,           /* Big endian initial vector */
    .aes_iv3 = 0x00000000,           /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
    .aes_key10 = 0x00000000,         /* Big endian aes key(aes-128/256 key MSB) */
    .aes_key11 = 0x00000000,         /* Big endian aes key */
    .aes_key12 = 0x00000000,         /* Big endian aes key */
    .aes_key13 = 0x00000000,         /* Big endian aes key(aes-128 key LSB) */
    .aes_key14 = 0x00000000,         /* Big endian aes key */
    .aes_key15 = 0x00000000,         /* Big endian aes key */
    .aes_key16 = 0x00000000,         /* Big endian aes key */
    .aes_key17 = 0x00000000,         /* Big endian aes key(aes-256 key LSB) */
    .aes_unitlen = 2,                /* Big endian aes unit len*/
    .aes_key20 = 0x00000000,         /* Big endian aes key2(aes-128/256 key MSB) */
    .aes_key21 = 0x00000000,         /* Big endian aes key2 */
    .aes_key22 = 0x00000000,         /* Big endian aes key2 */
    .aes_key23 = 0x00000000,         /* Big endian aes key2(aes-128 key LSB) */
    .aes_key24 = 0x00000000,         /* Big endian aes key2 */
    .aes_key25 = 0x00000000,         /* Big endian aes key2 */
    .aes_key26 = 0x00000000,         /* Big endian aes key2 */
    .aes_key27 = 0x00000000          /* Big endian aes key2(aes-256 key LSB) */
};

ATTR_NOCACHE_RAM_SECTION struct bflb_aes_xts_link_s aes_xts_link2 = {
    .aes_key = AES_LINK_KEY_192BITS, /* 128-bit key mode select */
    .aes_dec_en = 0,                 /* Encode */
    .aes_newkey_dis = 0,             /* Use new key */
    .aes_hwkey_en = 0,               /* Use sw key */
    .aes_intclr = 0,                 /* Not clear intrrupt */
    .aes_intset = 0,                 /* Not set interrupt */
    .aes_mode = AES_MODE_XTS,        /* ECB mode select */
    .aes_newiv_dis = 0,              /* Use new iv */
    .aes_xts = AES_LINK_XTS_MODE1,   /* XTS mode select */
    .aes_msglen = 1,                 /* Number of 128-bit block */
    .aes_srcaddr = 0,                /* Message source address */
    .aes_dstaddr = 0,                /* Message destination address */
    .aes_iv0 = 0x00000000,           /* Big endian initial vector(MSB) */
    .aes_iv1 = 0x00000000,           /* Big endian initial vector */
    .aes_iv2 = 0x00000000,           /* Big endian initial vector */
    .aes_iv3 = 0x00000000,           /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
    .aes_key10 = 0x03020100,         /* Big endian aes key(aes-128/256 key MSB) */
    .aes_key11 = 0x07060504,         /* Big endian aes key */
    .aes_key12 = 0x0b0a0908,         /* Big endian aes key */
    .aes_key13 = 0x0f0e0d0c,         /* Big endian aes key(aes-128 key LSB) */
    .aes_key14 = 0xfcfdfeff,         /* Big endian aes key */
    .aes_key15 = 0xf8f9fafb,         /* Big endian aes key */
    .aes_key16 = 0x00000000,         /* Big endian aes key */
    .aes_key17 = 0x00000000,         /* Big endian aes key(aes-256 key LSB) */
    .aes_unitlen = 2,                /* Big endian aes unit len*/
    .aes_key20 = 0x03020100,         /* Big endian aes key2(aes-128/256 key MSB) */
    .aes_key21 = 0x07060504,         /* Big endian aes key2 */
    .aes_key22 = 0x0b0a0908,         /* Big endian aes key2 */
    .aes_key23 = 0x0f0e0d0c,         /* Big endian aes key2(aes-128 key LSB) */
    .aes_key24 = 0xfcfdfeff,         /* Big endian aes key2 */
    .aes_key25 = 0xf8f9fafb,         /* Big endian aes key2 */
    .aes_key26 = 0x00000000,         /* Big endian aes key2 */
    .aes_key27 = 0x00000000          /* Big endian aes key2(aes-256 key LSB) */
};

ATTR_NOCACHE_RAM_SECTION struct bflb_aes_xts_link_s aes_xts_link3 = {
    .aes_key = AES_LINK_KEY_256BITS, /* 128-bit key mode select */
    .aes_dec_en = 0,                 /* Encode */
    .aes_newkey_dis = 0,             /* Use new key */
    .aes_hwkey_en = 0,               /* Use sw key */
    .aes_intclr = 0,                 /* Not clear intrrupt */
    .aes_intset = 0,                 /* Not set interrupt */
    .aes_mode = AES_MODE_XTS,        /* ECB mode select */
    .aes_newiv_dis = 0,              /* Use new iv */
    .aes_xts = AES_LINK_XTS_MODE1,   /* XTS mode select */
    .aes_msglen = 1,                 /* Number of 128-bit block */
    .aes_srcaddr = 0,                /* Message source address */
    .aes_dstaddr = 0,                /* Message destination address */
    .aes_iv0 = 0x00000000,           /* Big endian initial vector(MSB) */
    .aes_iv1 = 0x00000000,           /* Big endian initial vector */
    .aes_iv2 = 0x00000000,           /* Big endian initial vector */
    .aes_iv3 = 0x00000000,           /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
    .aes_key10 = 0x03020100,         /* Big endian aes key(aes-128/256 key MSB) */
    .aes_key11 = 0x07060504,         /* Big endian aes key */
    .aes_key12 = 0x0b0a0908,         /* Big endian aes key */
    .aes_key13 = 0x0f0e0d0c,         /* Big endian aes key(aes-128 key LSB) */
    .aes_key14 = 0xfcfdfeff,         /* Big endian aes key */
    .aes_key15 = 0xf8f9fafb,         /* Big endian aes key */
    .aes_key16 = 0xf4f5f6f7,         /* Big endian aes key */
    .aes_key17 = 0xf0f1f2f3,         /* Big endian aes key(aes-256 key LSB) */
    .aes_unitlen = 2,                /* Big endian aes unit len*/
    .aes_key20 = 0xfcfdfeff,         /* Big endian aes key2(aes-128/256 key MSB) */
    .aes_key21 = 0xf8f9fafb,         /* Big endian aes key2 */
    .aes_key22 = 0xf4f5f6f7,         /* Big endian aes key2 */
    .aes_key23 = 0xf0f1f2f3,         /* Big endian aes key2(aes-128 key LSB) */
    .aes_key24 = 0x03020100,         /* Big endian aes key2 */
    .aes_key25 = 0x07060504,         /* Big endian aes key2 */
    .aes_key26 = 0x0b0a0908,         /* Big endian aes key2 */
    .aes_key27 = 0x0f0e0d0c          /* Big endian aes key2(aes-256 key LSB) */
};

static uint32_t bflb_data_compare(const uint8_t *expected, uint8_t *input, uint32_t len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        if (input[i] != expected[i]) {
            printf("Compare fail at %d,input %02x, but expect %02x\r\n", i, input[i], expected[i]);
            while (1) {
            }
        }
    }

    return 0;
}

ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_enc_buf[512];
ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_dec_buf[512];

int main(void)
{
    board_init();

    struct bflb_device_s *aes;

    aes = bflb_device_get_by_name("aes");

    bflb_group0_request_aes_access(aes);

    bflb_aes_link_init(aes);

    bflb_aes_link_update(aes, (uint32_t)&aes_ecb_link, aes_ecb_128bit_pt, aes_enc_buf, 16);
    bflb_data_compare(aes_ecb_128bit_ct, aes_enc_buf, 16);
    printf("aes link ecb 128 success\r\n");

    bflb_aes_link_update(aes, (uint32_t)&aes_cbc_link, aes_cbc_128bit_pt, aes_enc_buf, 16);
    bflb_data_compare(aes_cbc_128bit_ct, aes_enc_buf, 16);
    printf("aes link cbc 128 success\r\n");

    bflb_aes_link_update(aes, (uint32_t)&aes_cbc_link2, aes_cbc_128bit_pt, aes_enc_buf, 48);
    bflb_data_compare(aes_cbc_128bit_ct, aes_enc_buf, 48);
    printf("aes link cbc 128 success\r\n");

    bflb_aes_link_update(aes, (uint32_t)&aes_ctr_link, aes_ctr_128bit_pt, aes_enc_buf, 16);
    bflb_data_compare(aes_ctr_128bit_ct, aes_enc_buf, 16);
    printf("aes link ctr 128 success\r\n");

    bflb_aes_link_update(aes, (uint32_t)&aes_xts_link1, aes_xts_128bit_pt, aes_enc_buf, 16);
    bflb_data_compare(aes_xts_128bit_ct, aes_enc_buf, 16);
    printf("aes link xts 128 success\r\n");

#if !defined(BL702L)
    bflb_aes_link_update(aes, (uint32_t)&aes_xts_link2, aes_xts_192bit_pt, aes_enc_buf, 32);
    bflb_data_compare(aes_xts_192bit_ct, aes_enc_buf, 32);
    printf("aes link xts 192 success\r\n");

    bflb_aes_link_update(aes, (uint32_t)&aes_xts_link3, aes_xts_256bit_pt, aes_enc_buf, 32);
    bflb_data_compare(aes_xts_256bit_ct, aes_enc_buf, 32);
    printf("aes link xts 256 success\r\n");
#endif
    printf("aes link success\r\n");
    bflb_group0_release_aes_access(aes);
    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}