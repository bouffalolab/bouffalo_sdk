#include "bflb_sec_irq.h"
#include "hardware/sec_eng_reg.h"

struct bflb_sec_irq_callback {
    void (*handler)(void *arg);
    void *arg;
};

#if defined(BL602)
/* BL602 */
#include "bl602_irq.h"
#include "bl602_memorymap.h"

#define BFLB_SEC_ENG_BASE                  SEC_ENG_BASE
#define BFLB_SEC_ENG_IRQ_MODE_SOURCE_SPLIT 1
#define BFLB_SEC_ENG_IRQ_SRC_GMAC          BL602_IRQ_SEC_GMAC
#define BFLB_SEC_ENG_IRQ_SRC_CDET          BL602_IRQ_SEC_CDET
#define BFLB_SEC_ENG_IRQ_SRC_PKA           BL602_IRQ_SEC_PKA
#define BFLB_SEC_ENG_IRQ_SRC_TRNG          BL602_IRQ_SEC_TRNG
#define BFLB_SEC_ENG_IRQ_SRC_AES           BL602_IRQ_SEC_AES
#define BFLB_SEC_ENG_IRQ_SRC_SHA           BL602_IRQ_SEC_SHA

#elif defined(BL702)
/* BL702 */
#include "bl702_irq.h"
#include "bl702_memorymap.h"

#define BFLB_SEC_ENG_BASE                  SEC_ENG_BASE
#define BFLB_SEC_ENG_IRQ_MODE_SOURCE_SPLIT 1
#define BFLB_SEC_ENG_IRQ_SRC_GMAC          BL702_IRQ_SEC_GMAC
#define BFLB_SEC_ENG_IRQ_SRC_CDET          BL702_IRQ_SEC_CDET
#define BFLB_SEC_ENG_IRQ_SRC_PKA           BL702_IRQ_SEC_PKA
#define BFLB_SEC_ENG_IRQ_SRC_TRNG          BL702_IRQ_SEC_TRNG
#define BFLB_SEC_ENG_IRQ_SRC_AES           BL702_IRQ_SEC_AES
#define BFLB_SEC_ENG_IRQ_SRC_SHA           BL702_IRQ_SEC_SHA

#elif defined(BL702L)
/* BL702L */
#include "bl702l_irq.h"
#include "bl702l_memorymap.h"

#define BFLB_SEC_ENG_BASE                  SEC_ENG_BASE
#define BFLB_SEC_ENG_IRQ_MODE_SOURCE_SPLIT 1
#define BFLB_SEC_ENG_IRQ_SRC_GMAC          BL702L_IRQ_SEC_GMAC
#define BFLB_SEC_ENG_IRQ_SRC_CDET          BL702L_IRQ_SEC_CDET
#define BFLB_SEC_ENG_IRQ_SRC_PKA           BL702L_IRQ_SEC_PKA
#define BFLB_SEC_ENG_IRQ_SRC_TRNG          BL702L_IRQ_SEC_TRNG
#define BFLB_SEC_ENG_IRQ_SRC_AES           BL702L_IRQ_SEC_AES
#define BFLB_SEC_ENG_IRQ_SRC_SHA           BL702L_IRQ_SEC_SHA

#elif defined(BL616)
/* BL616 */
#include "bl616_irq.h"
#include "bl616_memorymap.h"

#define BFLB_SEC_ENG_BASE                              SEC_ENG_BASE
#define BFLB_SEC_ENG_IRQ_MODE_ID_SPLIT                 1
#define BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID1 BL616_IRQ_SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC
#define BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID0 BL616_IRQ_SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC
#define BFLB_SEC_ENG_IRQ_SRC_CDET_ID1                  BL616_IRQ_SEC_ENG_ID1_CDET
#define BFLB_SEC_ENG_IRQ_SRC_CDET_ID0                  BL616_IRQ_SEC_ENG_ID0_CDET

#elif defined(BL616CL)
/* BL616CL */
#include "bl616cl_irq.h"
#include "bl616cl_memorymap.h"

#define BFLB_SEC_ENG_BASE                              SEC_ENG_BASE
#define BFLB_SEC_ENG_IRQ_MODE_ID_SPLIT                 1
#define BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID1 BL616CL_IRQ_SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC
#define BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID0 BL616CL_IRQ_SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC
#define BFLB_SEC_ENG_IRQ_SRC_CDET_ID1                  BL616CL_IRQ_SEC_ENG_ID1_CDET
#define BFLB_SEC_ENG_IRQ_SRC_CDET_ID0                  BL616CL_IRQ_SEC_ENG_ID0_CDET

#elif defined(BL618DG)
/* BL618DG */
#include "bl618dg_irq.h"
#include "bl618dg_memorymap.h"

#define BFLB_SEC_ENG_BASE                              SEC_ENG_BASE
#define BFLB_SEC_ENG_IRQ_MODE_ID_SPLIT                 1
#define BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID1 BL618DG_IRQ_SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC
#define BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID0 BL618DG_IRQ_SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC
#define BFLB_SEC_ENG_IRQ_SRC_CDET_ID1                  BL618DG_IRQ_SEC_ENG_ID1_CDET
#define BFLB_SEC_ENG_IRQ_SRC_CDET_ID0                  BL618DG_IRQ_SEC_ENG_ID0_CDET

#else
#error "No support SEC_IRQ for this chip"
#endif

struct bflb_sec_irq_callback sec_eng_callback[6];

void sec_eng_isr(int irq, void *arg)
{
    uint32_t regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    if (regval & SEC_ENG_SE_AES_0_INT) {
        regval |= SEC_ENG_SE_AES_0_INT_CLR_1T;
        putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
        if (sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_AES].handler) {
            sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_AES].handler(sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_AES].arg);
        }
    }

    regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    if (regval & SEC_ENG_SE_SHA_0_INT) {
        regval |= SEC_ENG_SE_SHA_0_INT_CLR_1T;
        putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        if (sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_SHA].handler) {
            sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_SHA].handler(sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_SHA].arg);
        }
    }

    regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
    if (regval & SEC_ENG_SE_PKA_0_INT) {
        regval |= SEC_ENG_SE_PKA_0_INT_CLR_1T;
        putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
        if (sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_PKA].handler) {
            sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_PKA].handler(sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_PKA].arg);
        }
    }

    regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    if (regval & SEC_ENG_SE_TRNG_0_INT) {
        regval |= SEC_ENG_SE_TRNG_0_INT_CLR_1T;
        putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
        if (sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_TRNG].handler) {
            sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_TRNG].handler(sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_TRNG].arg);
        }
    }

    regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
    if (regval & SEC_ENG_SE_GMAC_0_INT) {
        regval |= SEC_ENG_SE_GMAC_0_INT_CLR_1T;
        putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
        if (sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_GMAC].handler) {
            sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_GMAC].handler(sec_eng_callback[BFLB_SEC_ENG_IRQ_TYPE_GMAC].arg);
        }
    }
}

void bflb_sec_int_mask(uint8_t sec_type, bool mask)
{
    uint32_t regval;

    switch (sec_type) {
        case BFLB_SEC_ENG_IRQ_TYPE_AES:
            regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
            if (mask) {
                regval |= SEC_ENG_SE_AES_0_INT_MASK;
            } else {
                regval &= ~SEC_ENG_SE_AES_0_INT_MASK;
            }
            putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_AES_0_CTRL_OFFSET);
            break;
        case BFLB_SEC_ENG_IRQ_TYPE_SHA:
            regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            if (mask) {
                regval |= SEC_ENG_SE_SHA_0_INT_MASK;
            } else {
                regval &= ~SEC_ENG_SE_SHA_0_INT_MASK;
            }
            putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
            break;
        case BFLB_SEC_ENG_IRQ_TYPE_PKA:
            regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
            if (mask) {
                regval |= SEC_ENG_SE_PKA_0_INT_MASK;
            } else {
                regval &= ~SEC_ENG_SE_PKA_0_INT_MASK;
            }
            putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_PKA_0_CTRL_0_OFFSET);
            break;
        case BFLB_SEC_ENG_IRQ_TYPE_TRNG:
            regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
            if (mask) {
                regval |= SEC_ENG_SE_TRNG_0_INT_MASK;
            } else {
                regval &= ~SEC_ENG_SE_TRNG_0_INT_MASK;
            }
            putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
            break;
        case BFLB_SEC_ENG_IRQ_TYPE_GMAC:
            regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
            if (mask) {
                regval |= SEC_ENG_SE_GMAC_0_INT_MASK;
            } else {
                regval &= ~SEC_ENG_SE_GMAC_0_INT_MASK;
            }
            putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_GMAC_0_CTRL_0_OFFSET);
            break;
        case BFLB_SEC_ENG_IRQ_TYPE_CDET:
            regval = getreg32(BFLB_SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
            if (mask) {
                regval |= SEC_ENG_SE_CDET_0_INT_MASK;
            } else {
                regval &= ~SEC_ENG_SE_CDET_0_INT_MASK;
            }
            putreg32(regval, BFLB_SEC_ENG_BASE + SEC_ENG_SE_CDET_0_CTRL_0_OFFSET);
            break;

        default:
            break;
    }
}

void bflb_sec_irq_attach(uint8_t sec_type, void (*callback)(void *arg), void *arg)
{
    sec_eng_callback[sec_type].handler = callback;
    sec_eng_callback[sec_type].arg = arg;

#if defined(BFLB_SEC_ENG_IRQ_MODE_SOURCE_SPLIT)
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_GMAC, sec_eng_isr, NULL);
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_CDET, sec_eng_isr, NULL);
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_PKA, sec_eng_isr, NULL);
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_TRNG, sec_eng_isr, NULL);
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_AES, sec_eng_isr, NULL);
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_SHA, sec_eng_isr, NULL);

    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_GMAC);
    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_CDET);
    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_PKA);
    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_TRNG);
    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_AES);
    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_SHA);

#elif defined(BFLB_SEC_ENG_IRQ_MODE_ID_SPLIT)
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID1, sec_eng_isr, NULL);
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID0, sec_eng_isr, NULL);
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_CDET_ID1, sec_eng_isr, NULL);
    bflb_irq_attach(BFLB_SEC_ENG_IRQ_SRC_CDET_ID0, sec_eng_isr, NULL);

    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID1);
    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_SHA_AES_TRNG_PKA_GMAC_ID0);
    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_CDET_ID1);
    bflb_irq_enable(BFLB_SEC_ENG_IRQ_SRC_CDET_ID0);
#endif

    bflb_sec_int_mask(sec_type, false);
}

void bflb_sec_irq_detach(uint8_t sec_type)
{
    sec_eng_callback[sec_type].handler = NULL;
    sec_eng_callback[sec_type].arg = NULL;
    bflb_sec_int_mask(sec_type, true);
}
