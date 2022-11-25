#include "bflb_efuse.h"
#include "bflb_ef_ctrl.h"
float bflb_efuse_get_adc_trim(void)
{
    bflb_ef_ctrl_com_trim_type trim;
    uint32_t tmp;

    float coe = 1.0;

    //EF_Ctrl_Read_ADC_Gain_Trim(&trim);
    bflb_ef_ctrl_read_common_trim(NULL, "gpadc_gain", &trim, 1);

    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            tmp = trim.value;

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
    bflb_ef_ctrl_com_trim_type trim;

    //EF_Ctrl_Read_TSEN_Trim(&trim);
    bflb_ef_ctrl_read_common_trim(NULL, "tsen", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            return trim.value;
        }
    }

    return 2042;
}

void bflb_efuse_write_aes_key(uint8_t index, uint8_t *data, uint32_t len)
{
    if ((index <= 3) || (index == 11)) {
        index = ((index == 11) ? 5 : index);
        /* Every key is 4 words len*/
        bflb_ef_ctrl_write_direct(NULL, 0x1C + index * 4, (uint32_t *)data, len, 1);
    } else if ((index < 11) && (index > 3)) {
        index = index - 4;
        /* Every key is 4 words len*/
        bflb_ef_ctrl_write_direct(NULL, 0x80 + index * 4, (uint32_t *)data, len, 1);
    }
}

void bflb_efuse_lock_aes_key_write(uint8_t index)
{
    uint32_t lock = 0;

    if ((index <= 3) || (index == 11)) {
        index = ((index == 11) ? 8 : index);
        lock |= (1 << (index + 17));
        bflb_ef_ctrl_write_direct(NULL, 0x7c, (uint32_t *)lock, 1, 1);
    } else if ((index < 11) && (index > 3)) {
        index = index - 4;
        lock |= (1 << (index + 15));
        bflb_ef_ctrl_write_direct(NULL, 0xfc, (uint32_t *)lock, 1, 1);
    }
}

void bflb_efuse_lock_aes_key_read(uint8_t index)
{
    uint32_t lock = 0;

    if ((index <= 3) || (index == 11)) {
        index = ((index == 11) ? 4 : index);
        lock |= (1 << (index + 27));
        bflb_ef_ctrl_write_direct(NULL, 0x7c, (uint32_t *)lock, 1, 1);
    } else if ((index < 11) && (index > 3)) {
        index = index - 4;
        lock |= (1 << (index + 25));
        bflb_ef_ctrl_write_direct(NULL, 0xfc, (uint32_t *)lock, 1, 1);
    }
}

void bflb_efuse_read_aes_key(uint8_t index, uint8_t *data, uint32_t len)
{
    if ((index <= 3) || (index == 11)) {
        index = ((index == 11) ? 5 : index);
        /* Every key is 4 words len*/
        bflb_ef_ctrl_read_direct(NULL, 0x1C + index * 4, (uint32_t *)data, len, 1);
    } else if ((index < 11) && (index > 3)) {
        index = index - 4;
        /* Every key is 4 words len*/
        bflb_ef_ctrl_read_direct(NULL, 0x80 + index * 4, (uint32_t *)data, len, 1);
    }
}
