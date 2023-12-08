/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __SF_H__
#define __SF_H__

#include "avutil/common.h"

__BEGIN_DECLS__

/*
 *  0     1  big_endian 0-1
 *  1     1  is_signed  0-1
 *  2     1  is_planar  0-1
 *  3-6   4  bits >> 3  0-15
 *  7-10  4  channels   0-15
 *  11-28 17 rate       0-131071
 *  29    1  is_float   0-1
 */
typedef unsigned int sf_t;

#define SF_BIGENDIAN_MASK      (0x00000001)
#define SF_SIGNED_MASK         (0x00000002)
#define SF_PLANAR_MASK         (0x00000004)
#define SF_BITS_MASK           (0x00000078)
#define SF_CHANNELS_MASK       (0x00000780)
#define SF_RATE_MASK           (0x1ffff800)
#define SF_FLOAT_MASK          (0x20000000)

#define SF_BIGENDIAN_SHIFT     (0)
#define SF_SIGNED_SHIFT        (1)
#define SF_PLANAR_SHIFT        (2)
#define SF_BITS_SHIFT          (0)
#define SF_CHANNELS_SHIFT      (7)
#define SF_RATE_SHIFT          (11)
#define SF_FLOAT_SHIFT         (29)

#define sf_make_signed(val)    (((val) << SF_SIGNED_SHIFT   ) & SF_SIGNED_MASK)
#define sf_make_bit(val)       (((val) << SF_BITS_SHIFT     ) & SF_BITS_MASK)
#define sf_make_planar(val)    (((val) << SF_PLANAR_SHIFT   ) & SF_PLANAR_MASK)
#define sf_make_rate(val)      (((val) << SF_RATE_SHIFT     ) & SF_RATE_MASK)
#define sf_make_channel(val)   (((val) << SF_CHANNELS_SHIFT ) & SF_CHANNELS_MASK)
#define sf_make_bigendian(val) (((val) << SF_BIGENDIAN_SHIFT) & SF_BIGENDIAN_MASK)
#define sf_make_float(val)     (((val) << SF_FLOAT_SHIFT    ) & SF_FLOAT_MASK)

#define sf_get_bigendian(sf)   (((sf) & SF_BIGENDIAN_MASK) >> SF_BIGENDIAN_SHIFT)
#define sf_get_signed(sf)      (((sf) & SF_SIGNED_MASK   ) >> SF_SIGNED_SHIFT)
#define sf_get_planar(sf)      (((sf) & SF_PLANAR_MASK   ) >> SF_PLANAR_SHIFT)
#define sf_get_bit(sf)         (((sf) & SF_BITS_MASK     ) >> SF_BITS_SHIFT)
#define sf_get_channel(sf)     (((sf) & SF_CHANNELS_MASK ) >> SF_CHANNELS_SHIFT)
#define sf_get_rate(sf)        (((sf) & SF_RATE_MASK     ) >> SF_RATE_SHIFT)
#define sf_get_float(sf)       (((sf) & SF_FLOAT_MASK    ) >> SF_FLOAT_SHIFT)

#define sf_get_sample_size(sf) (sf_get_bit((sf)) >> 3)
#define sf_get_frame_size(sf)  (sf_get_sample_size((sf)) * sf_get_channel((sf)))
#define sf_get_bps(sf)         (sf_get_rate((sf)) * sf_get_bit((sf)) * sf_get_channel((sf)))

typedef enum {
    ESF_NONE,
    ESF_S8,                    ///< signed 8 bits
    ESF_S16LE,                 ///< signed 16 bits, little endian
    ESF_S16BE,                 ///< signed 16 bits, big endian
    ESF_S24LE,                 ///< signed 16 bits, little endian
    ESF_S32LE,                 ///< signed 32 bits, little endian
    ESF_S32BE,                 ///< signed 32 bits, big endian
    ESF_U8,                    ///< unsigned 8 bits
    ESF_U16LE,                 ///< unsigned 16 bits, little endian
    ESF_U16BE,                 ///< unsigned 16 bits, big endian
    ESF_U32LE,                 ///< unsigned 32 bits, little endian
    ESF_U32BE,                 ///< unsigned 32 bits, big endian
    ESF_F32LE,                 ///< float 32 bits, little endian
    ESF_F32BE,                 ///< float 32 bits, big endian
    ESF_F64LE,                 ///< float 64 bits, little endian
    ESF_F64BE,                 ///< float 64 bits, big endian

    ESF_S8P,                   ///< planar, signed 8 bits
    ESF_S16LEP,                ///< planar, signed 16 bits, little endian
    ESF_S16BEP,                ///< planar, signed 16 bits, big endian
    ESF_S32LEP,                ///< planar, signed 32 bits, little endian
    ESF_S32BEP,                ///< planar, signed 32 bits, big endian
    ESF_U8P,                   ///< planar, unsigned 8 bits
    ESF_U16LEP,                ///< planar, unsigned 16 bits, little endian
    ESF_U16BEP,                ///< planar, unsigned 16 bits, big endian
    ESF_U32LEP,                ///< planar, unsigned 32 bits, little endian
    ESF_U32BEP,                ///< planar, unsigned 32 bits, big endian
} esf_t;

typedef struct {
    uint8_t                   be;         ///< big endian
    uint8_t                   bits;
    uint8_t                   issign;     ///< is signed
    uint8_t                   planar;     ///< is signed
    uint8_t                   isfloat;    ///< is signed
} aformat_t;

/**
 * @brief  get aformat info from sf_t
 * @param  [in] sf
 * @param  [in] outf
 * @return
 */
void get_aformat_from_sf(sf_t sf, aformat_t *outf);

/**
 * @brief  get esf type from sf_t
 * @param  [in] sf
 * @return ESF_NONE on error
 */
esf_t get_esf_type(sf_t sf);

/**
 * @brief  get sf name
 * @param  [in] sf
 * @return "unknown" on error
 */
const char* get_sf_name(sf_t sf);

#define SF_FORMAT_STR_SIZE_MAX    (128)

/**
 * @brief  get format str of sf
 * @param  [in] buf : min of the buf is SF_FORMAT_STR_SIZE_MAX
 * @param  [in] sf
 * @return not null
 */
char* sf_get_format(char buf[SF_FORMAT_STR_SIZE_MAX], sf_t sf);

#define sf_get_format_str(sf) sf_get_format((char[SF_FORMAT_STR_SIZE_MAX]){0}, sf)

/**
 * @brief  get esf name
 * @param  [in] esf
 * @return "unknown" on error
 */
const char* get_esf_name(esf_t esf);

/**
 * @brief  get buffer size of given number samples
 * @param  [out] linesize
 * @param  [in] sf
 * @param  [in] nb_samples
 * @return -1 on err
 */
int sf_get_buffer_size(int *linesize, sf_t sf, int nb_samples);

__END_DECLS__

#endif /* __SF_H__ */

