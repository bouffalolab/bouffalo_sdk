#include "hal_acomp.h"
#include "bl702_acomp.h"
#include "bl702_hbn.h"

void acomp_init(acomp_device_t *device)
{
    uint32_t tmpVal;

    if (device->id == AON_ACOMP0_ID) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal &= ~(1 << 20);
        tmpVal &= ~(1 << 21);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

        /* Disable ACOMP first */
        tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP0_CTRL);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_ACOMP0_EN);
        tmpVal = BL_WR_REG(AON_BASE, AON_ACOMP0_CTRL, tmpVal);

        /* Set ACOMP config */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_MUXEN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_POS_SEL, device->pos_ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_NEG_SEL, device->neg_ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_LEVEL_SEL, AON_ACOMP_LEVEL_FACTOR_1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_BIAS_PROG, AON_ACOMP_BIAS_POWER_MODE1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_HYST_SELP, device->pos_hysteresis_vol);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_HYST_SELN, device->neg_hysteresis_vol);
        /*enable acomp*/
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_ACOMP0_EN);
        tmpVal = BL_WR_REG(AON_BASE, AON_ACOMP0_CTRL, tmpVal);

        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal |= (1 << 20);
        tmpVal |= (1 << 21);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

        HBN_Clear_IRQ(HBN_INT_ACOMP0);
    } else {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal &= ~(1 << 22);
        tmpVal &= ~(1 << 23);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

        /* Disable ACOMP first */
        tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP1_CTRL);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_ACOMP1_EN);
        tmpVal = BL_WR_REG(AON_BASE, AON_ACOMP1_CTRL, tmpVal);

        /* Set ACOMP config */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_MUXEN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_POS_SEL, device->pos_ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_NEG_SEL, device->neg_ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_LEVEL_SEL, AON_ACOMP_LEVEL_FACTOR_1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_BIAS_PROG, AON_ACOMP_BIAS_POWER_MODE1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_HYST_SELP, device->pos_hysteresis_vol);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_HYST_SELN, device->neg_hysteresis_vol);
        /*enable acomp*/
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_ACOMP1_EN);
        tmpVal = BL_WR_REG(AON_BASE, AON_ACOMP1_CTRL, tmpVal);

        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal |= (1 << 22);
        tmpVal |= (1 << 23);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

        HBN_Clear_IRQ(HBN_INT_ACOMP1);
    }
}

int acomp_get_result(acomp_device_t *device)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP_CTRL);

    /* Disable ACOMP first */
    if (device->id == AON_ACOMP0_ID) {
        if (BL_IS_REG_BIT_SET(tmpVal, AON_ACOMP0_OUT_RAW)) {
            return SET;
        } else {
            return RESET;
        }
    } else {
        if (BL_IS_REG_BIT_SET(tmpVal, AON_ACOMP1_OUT_RAW)) {
            return SET;
        } else {
            return RESET;
        }
    }
}