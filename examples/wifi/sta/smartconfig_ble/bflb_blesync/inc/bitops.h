#ifndef __BITOPS_H
#define __BITOPS_H

#ifdef __cplusplus
extern "C" {
#endif

/** \brief bit shift */
#ifndef BIT
#define BIT(bit)                  (1u << (bit))
#endif
/** \brief value shift */
#define SBF(value, field)         ((value) << (field))

/** \brief set bit */
#define BIT_SET(data, bit)        ((data) |= BIT(bit))

/** \brief clear bit */
#define BIT_CLR(data, bit)        ((data) &= ~BIT(bit))

/** \brief set bits according to the bits specified by the mask */
#define BIT_SET_MASK(data, mask)  ((data) |= (mask))

/** \brief clear bits according to the bits specified by the mask */
#define BIT_CLR_MASK(data, mask)  ((data) &= ~(mask))

/** \brief bit flip */
#define BIT_TOGGLE(data, bit)     ((data) ^= BIT(bit))

/** \brief modify bit */
#define BIT_MODIFY(data, bit, value) \
    ((value) ? BIT_SET(data, bit) : BIT_CLR(data, bit))

/** \brief Test if a bit is set */
#define BIT_ISSET(data, bit)      ((data) & BIT(bit))

/** \brief Get bit value */
#define BIT_GET(data, bit)        (BIT_ISSET(data, bit) ? 1 : 0)

/** \brief Get n bits mask value */
#define BITS_MASK(n)              (~((~0u) << (n)))

/** \brief Get bit field value */
#define BITS_GET(data, start, len)  \
    (((data) >> (start)) & BITS_MASK(len))

/** \brief Set bit field value */
#define BITS_SET(data, start, len, value) \
    ((data) = (((data) & ~SBF(BITS_MASK(len), (start))) | \
        SBF((value) & (BITS_MASK(len)), (start))))

#ifdef __cplusplus
}
#endif

#endif /* __BITOPS_H */

