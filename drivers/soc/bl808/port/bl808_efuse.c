#include "bflb_efuse.h"
#include "bl808_ef_ctrl.h"

float bflb_efuse_get_adc_trim(void)
{
    Efuse_ADC_Gain_Coeff_Type trim;
    uint32_t tmp;

    float coe = 1.0;

    EF_Ctrl_Read_ADC_Gain_Trim(&trim);

    if (trim.adcGainCoeffEn) {
        if (trim.adcGainCoeffParity == EF_Ctrl_Get_Trim_Parity(trim.adcGainCoeff, 12)) {
            tmp = trim.adcGainCoeff;

            if (tmp & 0x800) {
                tmp = ~tmp;
                tmp += 1;
                tmp = tmp & 0xfff;
                coe = (1.0 + ((float)tmp / 2048.0));
            } else {
                coe = (1.0 - ((float)tmp / 2048.0));
            }
        }
    }

    return coe;
}

uint32_t bflb_efuse_get_adc_tsen_trim(void)
{
    Efuse_TSEN_Refcode_Corner_Type trim;

    EF_Ctrl_Read_TSEN_Trim(&trim);
    if (trim.tsenRefcodeCornerEn) {
        if (trim.tsenRefcodeCornerParity == EF_Ctrl_Get_Trim_Parity(trim.tsenRefcodeCorner, 12)) {
            return trim.tsenRefcodeCorner;
        }
    }

    return 2042;
}

void bflb_efuse_write_aes_key(uint8_t index, uint8_t *data, uint32_t len)
{
    EF_Ctrl_Write_AES_Key(index, (uint32_t *)data, len, 1);
}

void bflb_efuse_read_aes_key(uint8_t index, uint8_t *data, uint32_t len)
{
    EF_Ctrl_Read_AES_Key(index, (uint32_t *)data, len);
}