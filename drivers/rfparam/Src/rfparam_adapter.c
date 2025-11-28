#include "rfparam_adapter.h"
#include "wl_api.h"

#if defined(BL616)
#include "bl616_aon.h"
#include "bl616_mfg_media.h"
#include "bl616_hbn.h"
#endif

#if defined(BL616D)
#include "bl616d_aon.h"
#include "bl616d_hbn.h"
#endif

#if defined(BL616L)
#include "bl616l_aon.h"
#include "bl616l_hbn.h"
#endif

#define DBG_TAG "rfparam"
#include "log.h"

BFLB_LOG_DEFINE_TAG(RFPARAM, DBG_TAG, true);

#undef BFLB_LOG_TAG
#define BFLB_LOG_TAG           BFLB_LOG_GET_TAG(RFPARAM)

#define rfparam_printf(...) LOG_I(__VA_ARGS__)

// static uint8_t g_rfparam_buf[RFPARAM_WL_API_MEM_SIZE] = {0};
static struct wl_cfg_t *g_rfparam_cfg = NULL;
static uint32_t g_tlv_base_addr;

void rfparam_array_printf(char *str, void *buf, uint32_t buf_len, int type)
{
    uint32_t i;
    uint8_t *a = NULL;
    int8_t *b = NULL;
    uint16_t *c = NULL;
    int16_t *d = NULL;

    rfparam_printf("%s[%d]: ", str, buf_len);

    switch (type) {
        case TYPE_UINT8:
            a = (uint8_t *)buf;
            for (i = 0; i < buf_len; i++) {
                printf("%d,", (int)a[i]);
            }
            break;
        case TYPE_INT8:
            b = (int8_t *)buf;
            for (i = 0; i < buf_len; i++) {
                printf("%d,", (int)b[i]);
            }
            break;
        case TYPE_UINT16:
            c = (uint16_t *)buf;
            for (i = 0; i < buf_len; i++) {
                printf("%d,", (int)c[i]);
            }
            break;
        case TYPE_INT16:
            d = (int16_t *)buf;
            for (i = 0; i < buf_len; i++) {
                printf("%d,", (int)d[i]);
            }
            break;
        default:
            break;
    }
    printf("\r\n");
}

/****************************************************************************/ /**
 * @brief  get wifi power offset by mode option
 *
 * @param  base_addr: rftlv base addr
 *         pwr_offset:return value
 *
 * @return RFPARAM_SUSS is suss , other is err
 *
*******************************************************************************/
int32_t rfparam_get_wlan_pwroffset_with_option(uint32_t base_addr, int8_t pwr_offset[14], uint8_t reload)
{
    uint8_t pwrmode[RFTLV_MAXLEN_PWR_MODE + 1] = { 0 };
    int8_t pwroffset_tmp[RFTLV_MAXLEN_PWR_OFFSET] = { 0 };

    /* pwr_offset_mode */
    if (rfparam_tlv_get(base_addr, RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, pwrmode) > 0) {
        rfparam_printf("pwr_mode is %s\r\n", pwrmode);
    } else {
        rfparam_printf("pwr_mode null\r\n");
        return RFPARAM_ERR_PWR_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_PWR_MODE; i++) {
        switch (pwrmode[i]) {
            /* read pwr offset from efuse */
            case 'B':
            case 'b': {
                if (0 == mfg_media_read_hp_poweroffset_with_lock(pwroffset_tmp, reload)) {
                    rfparam_array_printf((char *)"efuse wlan pwr_offset", pwroffset_tmp, sizeof(pwroffset_tmp), TYPE_INT8);
                    if ('B' == pwrmode[i]) {
                        memcpy(pwr_offset, pwroffset_tmp, sizeof(pwroffset_tmp));
                        return RFPARAM_SUSS;
                    } else if ('b' == pwrmode[i]) {
                        for (int i = 0; i < sizeof(pwroffset_tmp); i++) {
                            pwr_offset[i] += pwroffset_tmp[i];
                        }
                    }
                } else {
                    rfparam_printf("no pwr_offset in efuse\r\n");
                }

            } break;

            /* read pwr offset from rftlv */
            case 'F':
            case 'f': {
                if (rfparam_tlv_get(base_addr, RFTLV_TYPE_PWR_OFFSET, RFTLV_MAXLEN_PWR_OFFSET, pwroffset_tmp) > 0) {
                    for (int i = 0; i < 14; i++) {
                        pwroffset_tmp[i] -= 16;
                    }
                    rfparam_array_printf((char *)"tlv wlan pwr_offset", pwroffset_tmp, sizeof(pwroffset_tmp), TYPE_INT8);
                    if ('F' == pwrmode[i]) {
                        memcpy(pwr_offset, pwroffset_tmp, sizeof(pwroffset_tmp));
                        return RFPARAM_SUSS;
                    } else if ('f' == pwrmode[i]) {
                        for (int i = 0; i < sizeof(pwroffset_tmp); i++) {
                            pwr_offset[i] += pwroffset_tmp[i];
                        }
                    }
                } else {
                    rfparam_printf("tlv pwr_offset null\r\n");
                }
            } break;

            default: {
                rfparam_printf("not support mode\r\n");
            }
        }
    }

    return RFPARAM_SUSS;
}

/****************************************************************************/ /**
 * @brief  get wifi power offset by mode option
 *
 * @param  base_addr: rftlv base addr
 *         pwr_offset:return value
 *
 * @return RFPARAM_SUSS is suss , other is err
 *
*******************************************************************************/
int32_t rfparam_get_wlan_pwroffset_lp_with_option(uint32_t base_addr, int8_t pwr_offset[14], uint8_t reload)
{
    uint8_t pwrmode[RFTLV_MAXLEN_PWR_MODE + 1] = { 0 };
    int8_t pwroffset_tmp[RFTLV_MAXLEN_PWR_OFFSET] = { 0 };

    /* pwr_offset_mode */
    if (rfparam_tlv_get(base_addr, RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, pwrmode) > 0) {
    } else {
        return RFPARAM_ERR_PWR_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_PWR_MODE; i++) {
        switch (pwrmode[i]) {
            /* read pwr offset from efuse */
            case 'B':
            case 'b': {
                if (0 == mfg_media_read_lp_poweroffset_with_lock(pwroffset_tmp, reload)) {
                    rfparam_array_printf((char *)"efuse wlan lp pwr_offset", pwroffset_tmp, sizeof(pwroffset_tmp), TYPE_INT8);
                    if ('B' == pwrmode[i]) {
                        memcpy(pwr_offset, pwroffset_tmp, sizeof(pwroffset_tmp));
                        return RFPARAM_SUSS;
                    } else if ('b' == pwrmode[i]) {
                        for (int i = 0; i < sizeof(pwroffset_tmp); i++) {
                            pwr_offset[i] += pwroffset_tmp[i];
                        }
                    }
                } else {
                    rfparam_printf("no lp pwr_offset in efuse\r\n");
                }

            } break;

            /* read pwr offset from rftlv */
            case 'F':
            case 'f': {
                if (rfparam_tlv_get(base_addr, RFTLV_TYPE_PWR_OFFSET_LP, RFTLV_MAXLEN_PWR_OFFSET_LP, pwroffset_tmp) > 0) {
                    for (int i = 0; i < 14; i++) {
                        pwroffset_tmp[i] -= 16;
                    }
                    rfparam_array_printf((char *)"tlv wlan lp pwr_offset", pwroffset_tmp, sizeof(pwroffset_tmp), TYPE_INT8);
                    if ('F' == pwrmode[i]) {
                        memcpy(pwr_offset, pwroffset_tmp, sizeof(pwroffset_tmp));
                        return RFPARAM_SUSS;
                    } else if ('f' == pwrmode[i]) {
                        for (int i = 0; i < sizeof(pwroffset_tmp); i++) {
                            pwr_offset[i] += pwroffset_tmp[i];
                        }
                    }
                } else {
                    rfparam_printf("tlv lp pwr_offset null\r\n");
                }
            } break;

            default: {
                rfparam_printf("not support mode\r\n");
            }
        }
    }

    return RFPARAM_SUSS;
}

/****************************************************************************/ /**
 * @brief  get bz power offset by mode option
 *
 * @param  base_addr: rftlv base addr
 *         pwr_offset:return value
 *
 * @return RFPARAM_SUSS is suss , other is err
 *
*******************************************************************************/
int32_t rfparam_get_bz_pwroffset_with_option(uint32_t base_addr, int8_t pwr_offset[5], uint8_t reload)
{
    uint8_t pwrmode[RFTLV_MAXLEN_PWR_MODE + 1] = { 0 };
    int8_t pwroffset_tmp[RFTLV_MAXLEN_BLE_PWR_OFFSET] = { 0 };

    /* pwr_offset_mode */
    if (rfparam_tlv_get(base_addr, RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, pwrmode) > 0) {
    } else {
        return RFPARAM_ERR_PWR_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_PWR_MODE; i++) {
        switch (pwrmode[i]) {
            /* read pwr offset from efuse */
            case 'B':
            case 'b': {
                if (0 == mfg_media_read_bz_poweroffset_with_lock(pwroffset_tmp, reload)) {
                    rfparam_array_printf((char *)"efuse bz pwr_offset", pwroffset_tmp, sizeof(pwroffset_tmp), TYPE_INT8);
                    if ('B' == pwrmode[i]) {
                        memcpy(pwr_offset, pwroffset_tmp, sizeof(pwroffset_tmp));
                        return RFPARAM_SUSS;
                    } else if ('b' == pwrmode[i]) {
                        for (int i = 0; i < RFTLV_MAXLEN_BLE_PWR_OFFSET; i++) {
                            pwr_offset[i] += pwroffset_tmp[i];
                        }
                    }
                } else {
                    rfparam_printf("no bz pwr_offset in efuse\r\n");
                }

            } break;

            /* read pwr offset from rftlv */
            case 'F':
            case 'f': {
                if (rfparam_tlv_get(base_addr, RFTLV_TYPE_PWR_OFFSET_BLE, RFTLV_MAXLEN_BLE_PWR_OFFSET, pwroffset_tmp) > 0) {
                    for (int i = 0; i < RFTLV_MAXLEN_BLE_PWR_OFFSET; i++) {
                        pwroffset_tmp[i] -= 16;
                    }
                    rfparam_array_printf((char *)"tlv bz pwr_offset", pwroffset_tmp, sizeof(pwroffset_tmp), TYPE_INT8);
                    if ('F' == pwrmode[i]) {
                        memcpy(pwr_offset, pwroffset_tmp, sizeof(pwroffset_tmp));
                        return RFPARAM_SUSS;
                    } else if ('f' == pwrmode[i]) {
                        for (int i = 0; i < RFTLV_MAXLEN_BLE_PWR_OFFSET; i++) {
                            pwr_offset[i] += pwroffset_tmp[i];
                        }
                    }
                } else {
                    rfparam_printf("tlv pwr_offset null\r\n");
                }
            } break;

            default: {
                rfparam_printf("not support mode\r\n");
            }
        }
    }

    return RFPARAM_SUSS;
}

/****************************************************************************/ /**
 * @brief  get cap code by mode option
 *
 * @param  base_addr: rftlv base addr
 *         capcode_in,capcode_out:return value
 *
 * @return RFPARAM_SUSS is suss , other is err
 *
*******************************************************************************/
int32_t rfparam_get_cap_code_with_option(uint32_t base_addr, uint8_t *capcode_in, uint8_t *capcode_out, uint8_t reload)
{
    uint8_t capmode[RFTLV_MAXLEN_XTAL_MODE + 1] = { 0 };
    // uint8_t capcode[RFTLV_MAXLEN_XTAL/4]={0};
    uint8_t tmp_buf[32];

    if (rfparam_tlv_get(base_addr, RFTLV_TYPE_XTAL_MODE, RFTLV_MAXLEN_XTAL_MODE, capmode) > 0) {
        rfparam_printf("capcode mode is %s\r\n", capmode);
    } else {
        rfparam_printf("capcode mode null\r\n");
        return RFPARAM_ERR_CAP_CODE_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_XTAL_MODE; i++) {
        switch (capmode[i]) {
            case 'F': {
                if (rfparam_tlv_get(base_addr, RFTLV_TYPE_XTAL, RFTLV_MAXLEN_XTAL, tmp_buf) > 0) {
                    *capcode_in = tmp_buf[0];
                    *capcode_out = tmp_buf[4];
                    rfparam_printf("tlv capcode_in %d,capcode_out %d\r\n", *capcode_in, *capcode_out);
                    return RFPARAM_SUSS;
                } else {
                    rfparam_printf("tlv capcode null\r\n");
                }
            } break;
            case 'M': {
                if (0 == mfg_media_read_xtal_capcode_with_lock(capcode_in, reload)) {
                    *capcode_out = *capcode_in;
                    rfparam_printf("efuse capcode_in %d,capcode_out %d\r\n", *capcode_in, *capcode_out);
                    return RFPARAM_SUSS;
                } else {
                    rfparam_printf("no capcode in efuse\r\n");
                }
            } break;
            default: {
                rfparam_printf("not support mode\r\n");
            }
        }
    }

    return RFPARAM_SUSS;
}

void rfparam_get_capcode(uint8_t *capcode_in, uint8_t *capcode_out)
{
    *capcode_in = *capcode_out = AON_Get_Xtal_CapCode();
}

void rfparam_set_capcode(uint8_t capcode_in, uint8_t capcode_out)
{
    uint32_t tmpVal = 0;
#if defined(BL616L) || defined(BL616D)
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CORE0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON, capcode_in);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_OUT_AON, capcode_out);
    BL_WR_REG(AON_BASE, AON_XTAL_CORE0, tmpVal);
#elif defined(BL616)
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON, capcode_in);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_OUT_AON, capcode_out);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);
#endif

    //arch_delay_us(100);

    //return SUCCESS;

    //AON_Set_Xtal_CapCode(capcode_in, capcode_out);
}

/****************************************************************************/ /**
 * @brief  rfparam_load
 *
 * @param  rf_para:   param return
 *
 * @return RFPARAM_SUSS is suss, other is err
 *
*******************************************************************************/
int8_t rfparam_load(struct wl_param_t *param)
{
    uint8_t tmp_buf[32] = { 0 };

#ifdef CONFIG_RFPARAM_TLV_FIXED_ADDRESS

    if (((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_RAM_CACHE & RFPARAM_TLV_BASE_ADDR_MASK)) {
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_RAM_CACHE;
    } else if (((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_RAM_NO_CACHE & RFPARAM_TLV_BASE_ADDR_MASK)) {
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_RAM_NO_CACHE;
    } else if (((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_XIP_FLASH & RFPARAM_TLV_BASE_ADDR_MASK)) {
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_XIP_FLASH;
    } else {
        rfparam_printf("ERROR: No tlv address found!\r\n");
        return RFPARAM_ERR_TLV_BASE_ADDR;
    }
#else

    extern uint32_t _ld_symbol_rftlv_address;
    g_tlv_base_addr = (uint32_t)(uintptr_t)&_ld_symbol_rftlv_address;
#endif

    rfparam_printf("RF_PARAM TLV ADDRESS: 0x%08X\r\n", g_tlv_base_addr);

    if (param == NULL) {
        return RFPARAM_ERR_PARAM_CHECK;
    }

    /* 1. get all rf params */
    /* pwr_offset_mode */

    if (RFPARAM_SUSS == rfparam_get_wlan_pwroffset_with_option(g_tlv_base_addr, param->pwrcal.channel_pwrcomp_wlan, 1)) {
        rfparam_array_printf((char *)"wlan pwr_offset", (void *)param->pwrcal.channel_pwrcomp_wlan, sizeof(param->pwrcal.channel_pwrcomp_wlan), TYPE_INT8);
    } else {
        rfparam_printf("wlan pwr_offset err\r\n");
        return RFPARAM_ERR_PWR_OFFSET_NULL;
    }

    if (RFPARAM_SUSS == rfparam_get_wlan_pwroffset_lp_with_option(g_tlv_base_addr, param->pwrcal.channel_lp_pwrcomp_wlan, 1)) {
        rfparam_array_printf((char *)"wlan lp pwr_offset", (void *)param->pwrcal.channel_lp_pwrcomp_wlan, sizeof(param->pwrcal.channel_lp_pwrcomp_wlan), TYPE_INT8);
    } else {
        rfparam_printf("wlan lp pwr_offset err\r\n");
        return RFPARAM_ERR_PWR_OFFSET_NULL;
    }

    if (RFPARAM_SUSS == rfparam_get_bz_pwroffset_with_option(g_tlv_base_addr, param->pwrcal.channel_pwrcomp_bz, 1)) {
        rfparam_array_printf((char *)"bz pwr_offset", (void *)param->pwrcal.channel_pwrcomp_bz, sizeof(param->pwrcal.channel_pwrcomp_bz), TYPE_INT8);
    } else {
        rfparam_printf("bz pwr_offset err\r\n");
        return RFPARAM_ERR_PWR_OFFSET_NULL;
    }

    /* pwr table */
    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_11B, RFTLV_MAXLEN_PWR_TABLE_11B, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11b, tmp_buf, sizeof(param->pwrtarget.pwr_11b));

        rfparam_array_printf((char *)"pwr_11b", (void *)param->pwrtarget.pwr_11b, sizeof(param->pwrtarget.pwr_11b), TYPE_INT8);
    } else {
        rfparam_printf("pwr_11b null\r\n");
        return RFPARAM_ERR_PWR_11B_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_11G, RFTLV_MAXLEN_PWR_TABLE_11G, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11g, tmp_buf, sizeof(param->pwrtarget.pwr_11g));
        rfparam_array_printf((char *)"pwr_11g", (void *)param->pwrtarget.pwr_11g, sizeof(param->pwrtarget.pwr_11g), TYPE_INT8);
    } else {
        rfparam_printf("pwr_11g null\r\n");
        return RFPARAM_ERR_PWR_11G_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_11N, RFTLV_MAXLEN_PWR_TABLE_11N, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11n_ht20, tmp_buf, sizeof(param->pwrtarget.pwr_11n_ht20));
        rfparam_array_printf((char *)"pwr_11n_ht20", (void *)param->pwrtarget.pwr_11n_ht20, sizeof(param->pwrtarget.pwr_11n_ht20), TYPE_INT8);
    } else {
        rfparam_printf("pwr_11n_ht20 null\r\n");
        return RFPARAM_ERR_PWR_11N_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_11N_HT40, RFTLV_MAXLEN_PWR_TABLE_11N_HT40, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11n_ht40, tmp_buf, sizeof(param->pwrtarget.pwr_11n_ht40));
        rfparam_array_printf((char *)"pwr_11n_ht40", (void *)param->pwrtarget.pwr_11n_ht40, sizeof(param->pwrtarget.pwr_11n_ht40), TYPE_INT8);
    } else {
        rfparam_printf("pwr_11n_ht40 null\r\n");
        return RFPARAM_ERR_PWR_11N_HT40_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_11AC_VHT20, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT20, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ac_vht20, tmp_buf, sizeof(param->pwrtarget.pwr_11ac_vht20));
        rfparam_array_printf((char *)"pwr_11ac_vht20", (void *)param->pwrtarget.pwr_11ac_vht20, sizeof(param->pwrtarget.pwr_11ac_vht20), TYPE_INT8);
    } else {
        rfparam_printf("pwr_11ac_vht20 null\r\n");
        return RFPARAM_ERR_PWR_11AC_VHT20_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_11AC_VHT40, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT40, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ac_vht40, tmp_buf, sizeof(param->pwrtarget.pwr_11ac_vht40));
        rfparam_array_printf((char *)"pwr_11ac_vht40", (void *)param->pwrtarget.pwr_11ac_vht40, sizeof(param->pwrtarget.pwr_11ac_vht40), TYPE_INT8);
    } else {
        rfparam_printf("pwr_11ac_vht40 null\r\n");
        return RFPARAM_ERR_PWR_11AC_VHT40_NULL;
    }
#if 0
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AC_VHT80, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT80, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ac_vht80,tmp_buf,sizeof(param->pwrtarget.pwr_11ac_vht80));
        rfparam_array_printf((char *)"pwr_11ac_vht80",(void *)param->pwrtarget.pwr_11ac_vht80,sizeof(param->pwrtarget.pwr_11ac_vht80),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ac_vht80 null\r\n");
    	return RFPARAM_ERR_PWR_11AC_VHT80_NULL;
    }
#endif
    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_11AX_HE20, RFTLV_MAXLEN_PWR_TABLE_11AX_HE20, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he20, tmp_buf, sizeof(param->pwrtarget.pwr_11ax_he20));
        rfparam_array_printf((char *)"pwr_11ax_he20", (void *)param->pwrtarget.pwr_11ax_he20, sizeof(param->pwrtarget.pwr_11ax_he20), TYPE_INT8);
    } else {
        rfparam_printf("pwr_11ax_he20 null\r\n");
        return RFPARAM_ERR_PWR_11AX_HE20_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_11AX_HE40, RFTLV_MAXLEN_PWR_TABLE_11AX_HE40, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he40, tmp_buf, sizeof(param->pwrtarget.pwr_11ax_he40));
        rfparam_array_printf((char *)"pwr_11ax_he40", (void *)param->pwrtarget.pwr_11ax_he40, sizeof(param->pwrtarget.pwr_11ax_he40), TYPE_INT8);
    } else {
        rfparam_printf("pwr_11ax_he40 null\r\n");
        return RFPARAM_ERR_PWR_11AX_HE40_NULL;
    }
#if 0
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE80, RFTLV_MAXLEN_PWR_TABLE_11AX_HE80, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he80,tmp_buf,sizeof(param->pwrtarget.pwr_11ax_he80));
        rfparam_array_printf((char *)"pwr_11ax_he80",(void *)param->pwrtarget.pwr_11ax_he80,sizeof(param->pwrtarget.pwr_11ax_he80),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ax_he80 null\r\n");
    	return RFPARAM_ERR_PWR_11AX_HE80_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE160, RFTLV_MAXLEN_PWR_TABLE_11AX_HE160, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he160,tmp_buf,sizeof(param->pwrtarget.pwr_11ax_he160));
        rfparam_array_printf((char *)"pwr_11ax_he160",(void *)param->pwrtarget.pwr_11ax_he160,sizeof(param->pwrtarget.pwr_11ax_he160),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ax_he160 null\r\n");
    	return RFPARAM_ERR_PWR_11AX_HE160_NULL;
    }
#endif
    /* capcode */
    if (RFPARAM_SUSS == rfparam_get_cap_code_with_option(g_tlv_base_addr, &param->xtalcapcode_in, &param->xtalcapcode_out, 1)) {
        rfparam_printf("capcode_in %d,capcode_out %d\r\n", param->xtalcapcode_in, param->xtalcapcode_out);
    } else {
        rfparam_printf("capcode err\r\n");
    }

    /*tcal*/
    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_EN_TCAL, RFTLV_MAXLEN_EN_TCAL, tmp_buf) > 0) {
        param->tcal.en_tcal = tmp_buf[0];
        rfparam_printf("tcal.en_tcal = %d\r\n", (int)param->tcal.en_tcal);
    } else {
        rfparam_printf("tcal.en_tcal null\r\n");
        return RFPARAM_ERR_TCAL_EN_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_LINEAR_OR_FOLLOW, RFTLV_MAXLEN_LINEAR_OR_FOLLOW, tmp_buf) > 0) {
        param->tcal.linear_or_follow = tmp_buf[0];
        rfparam_printf("tcal.linear_or_follow = %d\r\n", (int)param->tcal.linear_or_follow);
    } else {
        rfparam_printf("tcal.linear_or_follow null\r\n");
        return RFPARAM_ERR_TCAL_LINEAR_OR_FOLLOW_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_TCHANNELS, RFTLV_MAXLEN_TCHANNELS, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNELS / 2; i++) {
            param->tcal.Tchannels[i] = tmp_buf[i * 2] | (tmp_buf[i * 2 + 1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannels", (void *)param->tcal.Tchannels, sizeof(param->tcal.Tchannels) / 2, TYPE_UINT16);
    } else {
        rfparam_printf("tcal.Tchannels null\r\n");
        return RFPARAM_ERR_TCAL_CHANNELS_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_TCHANNEL_OS, RFTLV_MAXLEN_TCHANNEL_OS, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS / 2; i++) {
            param->tcal.Tchannel_os[i] = tmp_buf[i * 2] | (tmp_buf[i * 2 + 1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannel_os", (void *)param->tcal.Tchannel_os, sizeof(param->tcal.Tchannel_os) / 2, TYPE_INT16);
    } else {
        rfparam_printf("tcal.Tchannel_os null\r\n");
        return RFPARAM_ERR_TCAL_CHANNEL_OS_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_TCHANNEL_OS_LOW, RFTLV_MAXLEN_TCHANNEL_OS_LOW, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS_LOW / 2; i++) {
            param->tcal.Tchannel_os_low[i] = tmp_buf[i * 2] | (tmp_buf[i * 2 + 1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannel_os_low", (void *)param->tcal.Tchannel_os_low, sizeof(param->tcal.Tchannel_os_low) / 2, TYPE_INT16);
    } else {
        rfparam_printf("tcal.Tchannel_os_low null\r\n");
        return RFPARAM_ERR_TCAL_CHANNEL_OS_LOW_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_TROOM_OS, RFTLV_MAXLEN_TROOM_OS, tmp_buf) > 0) {
        param->tcal.Troom_os = (tmp_buf[0] | (tmp_buf[1] << 8)) - 256;
        rfparam_printf("tcal.Troom_os = %d\r\n", (int)param->tcal.Troom_os);
    } else {
        rfparam_printf("tcal.Troom_os null\r\n");
        return RFPARAM_ERR_TCAL_ROOM_OS_NULL;
    }

    /*ble*/
    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_BLE, RFTLV_MAXLEN_PWR_TABLE_BLE, tmp_buf) > 0) {
        param->pwrtarget.pwr_ble = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8) | (tmp_buf[2] << 16) | (tmp_buf[3] << 24));
        if ((uint8_t)param->pwrtarget.pwr_ble > 128) {
            param->pwrtarget.pwr_ble -= 256;
        }

        rfparam_printf("pwr_ble = %d \r\n", (int)param->pwrtarget.pwr_ble);

    } else {
        rfparam_printf("pwr_ble null\r\n");
        return RFPARAM_ERR_PWR_BLE_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_BT, RFTLV_MAXLEN_PWR_TABLE_BT, tmp_buf) > 0) {
        for (uint32_t i = 0; i < 3; i++) {
            param->pwrtarget.pwr_bt[i] = (int8_t)(tmp_buf[i * 4] | (tmp_buf[i * 4 + 1] << 8) | (tmp_buf[i * 4 + 2] << 16) | (tmp_buf[i * 4 + 3] << 24));
            if ((uint8_t)param->pwrtarget.pwr_bt[i] > 128) {
                param->pwrtarget.pwr_bt[i] -= 256;
            }
        }

        rfparam_array_printf((char *)"pwr_bt", (void *)param->pwrtarget.pwr_bt, sizeof(param->pwrtarget.pwr_bt), TYPE_INT8);

    } else {
        rfparam_printf("pwr_bt null\r\n");
        return RFPARAM_ERR_PWR_BT_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_PWR_TABLE_ZIGBEE, RFTLV_MAXLEN_PWR_TBALE_ZIGBEE, tmp_buf) > 0) {
        param->pwrtarget.pwr_zigbee = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8) | (tmp_buf[2] << 16) | (tmp_buf[3] << 24));
        if ((uint8_t)param->pwrtarget.pwr_zigbee > 128) {
            param->pwrtarget.pwr_zigbee -= 256;
        }
        rfparam_printf("pwr_zigbee = %d \r\n", (int)param->pwrtarget.pwr_zigbee);

    } else {
        rfparam_printf("pwr_zigbee null\r\n");
        return RFPARAM_ERR_PWR_ZIGBEE_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_COUNTRY_CODE, RFTLV_MAXLEN_COUNTRY_CODE, tmp_buf) > 0) {
        // param->country_code = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8));
        // rfparam_printf("country_code = %d \r\n", (int)param->country_code);
        if ((tmp_buf[0] >= 65) && (tmp_buf[1] >= 65) && (tmp_buf[0] <= 90) && (tmp_buf[1] <= 90)) {
            param->country_code = (tmp_buf[0] | (tmp_buf[1] << 8));
            rfparam_printf("country_code = %c%c \r\n", tmp_buf[0], tmp_buf[1]);
        } else if ((tmp_buf[0] >= 48) && (tmp_buf[1] >= 48) && (tmp_buf[0] <= 57) && (tmp_buf[1] <= 57)) {
            param->country_code = (tmp_buf[0] | (tmp_buf[1] << 8));
            rfparam_printf("country_code = %c%c \r\n", tmp_buf[0], tmp_buf[1]);
        } else {
            param->country_code = (tmp_buf[0] | (tmp_buf[1] << 8));
            rfparam_printf("country_code = %d \r\n", (int)param->country_code);
        }
    } else {
        rfparam_printf("country_code null\r\n");
        return RFPARAM_ERR_COUNTRY_CODE_NULL;
    }

    /* tcap_cal */
    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_EN_TCAPCAL, RFTLV_MAXLEN_EN_TCAPCAL, tmp_buf) > 0) {
        param->tcap.en_tcap = tmp_buf[0];
        rfparam_printf("en_tcap = %d \r\n", (int)param->tcap.en_tcap);

    } else {
        rfparam_printf("en_tcap Null\r\n");
        //return RFPARAM_ERR_EN_TCAP;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_TCAP_TSEN, RFTLV_MAXLEN_TCAP_TSEN, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCAP_TSEN; i++) {
            param->tcap.tcap_tsen[i] = tmp_buf[i];
        }
        rfparam_array_printf((char *)"tcap_tsen", param->tcap.tcap_tsen, RFTLV_MAXLEN_TCAP_TSEN, TYPE_INT8);

    } else {
        rfparam_printf("tcal_tsen Null\r\n");
        //return RFPARAM_ERR_TCAP_TSEN;
    }

    if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_TCAP_CAPCODE, RFTLV_MAXLEN_TCAP_CAPCODE, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCAP_CAPCODE; i++) {
            param->tcap.tcap_cap[i] = tmp_buf[i];
        }
        rfparam_array_printf((char *)"tcap_cap", param->tcap.tcap_cap, RFTLV_MAXLEN_TCAP_CAPCODE, TYPE_INT8);

    } else {
        rfparam_printf("tcap_cap Null\r\n");
        //return RFPARAM_ERR_TCAP_CAP;
    }

    for (int i = 0; i < NUM_WLAN_CHANNELS; i++) {
        if (rfparam_tlv_get(g_tlv_base_addr, RFTLV_TYPE_POWER_LIMIT_2G_EXT_CH1 + i, RFTLV_MAXLEN_PWR_LIMIT_EXT, tmp_buf) > 0) {
            param->pwrlim[i].en = 1;
            param->pwrlim[i].b_dsss = tmp_buf[0];
            param->pwrlim[i].b_cck = tmp_buf[1];
            param->pwrlim[i].g = tmp_buf[2];
            param->pwrlim[i].n20 = tmp_buf[3];
            param->pwrlim[i].ax20 = tmp_buf[5];
            param->pwrlim[i].n40 = tmp_buf[6];
            param->pwrlim[i].ax40 = tmp_buf[8];

            rfparam_printf("pwr_limit channel %d:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", i + 1, tmp_buf[0], tmp_buf[1], tmp_buf[2], tmp_buf[3], tmp_buf[4], tmp_buf[5], tmp_buf[6], tmp_buf[7], tmp_buf[8], tmp_buf[9], tmp_buf[10], tmp_buf[11]);

        } else {
            param->pwrlim[i].en = 0;
            memset(&param->pwrlim[i], 0, sizeof(param->pwrlim[i]));
        }
    }

    return RFPARAM_SUSS;
}

struct wl_cfg_t *rfparam_cfg_get()
{
    return g_rfparam_cfg;
}

uint32_t rfparam_tlv_base_addr_get()
{
    return g_tlv_base_addr;
}

#if defined(BL616D)
void wireless_config_init(struct wl_param_t *hw_param) {}
#endif

/****************************************************************************/ /**
 * @brief  rfparam_init
 *
 * @param  base_addr: rftlv base addr
 * 		   rf_para:   param return, can be NULL
 *		   apply_flag:apply any param in rftlv or none
 *
 * @return RFPARAM_SUSS is suss, other is err
 *
*******************************************************************************/
int32_t rfparam_init(uint32_t base_addr, void *rf_para, uint32_t apply_flag)
{
#if defined(BL616D)
    static uint32_t ram[2048];
    wl_cfg = wl_cfg_get((uint8_t*)ram);
    wl_cfg->capcode_get = rfparam_get_capcode;
    wl_cfg->capcode_set = rfparam_set_capcode;
    wl_cfg->param_load = NULL; // rfparam_load;
    wl_cfg->en_full_cal = 1;
    wl_cfg->en_param_load = 0;
    wl_cfg->mode = WL_API_MODE_ALL;
    wl_cfg->log_level = 0;
    wl_cfg->log_printf = NULL;
    wl_cfg->param.xtalfreq_hz = 40*1000*1000; //xtal_value;
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11b)/sizeof(wl_cfg->param.pwrtarget.pwr_11b[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11b[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11g)/sizeof(wl_cfg->param.pwrtarget.pwr_11g[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11g[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11n_ht20)/sizeof(wl_cfg->param.pwrtarget.pwr_11n_ht20[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11n_ht20[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11n_ht40)/sizeof(wl_cfg->param.pwrtarget.pwr_11n_ht40[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11n_ht40[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht20)/sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht20[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11ac_vht20[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht40)/sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht40[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11ac_vht40[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht80)/sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht80[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11ac_vht80[i] = 10;
    }

    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he20)/sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he20[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11ax_he20[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he40)/sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he40[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11ax_he40[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he80)/sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he80[0]); i++)
    {
        wl_cfg->param.pwrtarget.pwr_11ax_he80[i] = 10;
    }

    wl_init();
    return 0;
#endif
    int32_t ret;
    uint32_t xtal_value;
    bflb_ef_ctrl_com_trim_t trim;
#if defined(WL_API_RMEM_EN) && WL_API_RMEM_EN
    g_rfparam_cfg = wl_cfg_get((uint8_t *)WL_API_RMEM_ADDR);
#else
    g_rfparam_cfg = wl_cfg_get();
#endif
    g_rfparam_cfg->capcode_get = rfparam_get_capcode;
    g_rfparam_cfg->capcode_set = rfparam_set_capcode;
    g_rfparam_cfg->param_load = rfparam_load;
    g_rfparam_cfg->en_full_cal = 1;
    g_rfparam_cfg->en_param_load = 1;
    g_rfparam_cfg->mode = WL_API_MODE_ALL;
    g_rfparam_cfg->log_level = WL_LOG_LEVEL_NONE;
    g_rfparam_cfg->log_printf = NULL;

    HBN_Get_Xtal_Value(&xtal_value);
    g_rfparam_cfg->param.xtalfreq_hz = xtal_value;
    rfparam_printf("xtal value %d\r\n", (int)xtal_value);

    bflb_ef_ctrl_read_common_trim(NULL, "dcdc_trim", &trim, 1);
    if (trim.empty) {
        rfparam_printf("dcdc_trim empty\r\n");
        rfparam_printf("dcdc_trim use default value 0x80\r\n"); // request from xueliang
        g_rfparam_cfg->param.ef.dcdc_vout_trim_aon = 0x80;
    } else {
        if (trim.en == 1 && trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            g_rfparam_cfg->param.ef.dcdc_vout_trim_aon = trim.value;
            rfparam_printf("dcdc_trim value %d\r\n", (int)trim.value);
        } else {
            rfparam_printf("dcdc_trim param error\r\n");
            rfparam_printf("dcdc_trim use default value 0x80\r\n"); // request from xueliang
            g_rfparam_cfg->param.ef.dcdc_vout_trim_aon = 0x80;
        }
    }

    bflb_ef_ctrl_read_common_trim(NULL, "icx", &trim, 1);
    if (trim.empty) {
        rfparam_printf("icx empty\r\n");
        rfparam_printf("icx use default value 0x80\r\n"); // request from xueliang
        g_rfparam_cfg->param.ef.icx_code = 0x80;
    } else {
        if (trim.en == 1 && trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            g_rfparam_cfg->param.ef.icx_code = trim.value;
            rfparam_printf("icx value %d\r\n", (int)trim.value);
        } else {
            rfparam_printf("icx param error\r\n");
            rfparam_printf("icx use default value 0x80\r\n"); // request from xueliang
            g_rfparam_cfg->param.ef.icx_code = 0x80;
        }
    }

    bflb_ef_ctrl_read_common_trim(NULL, "iptat", &trim, 1);
    if (trim.empty) {
        rfparam_printf("iptat empty\r\n");
        rfparam_printf("iptat use default value 0x80\r\n"); // request from xueliang
        g_rfparam_cfg->param.ef.iptat_code = 0x80;
    } else {
        if (trim.en == 1 && trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            g_rfparam_cfg->param.ef.iptat_code = trim.value;
            rfparam_printf("iptat value %d\r\n", (int)trim.value);
        } else {
            rfparam_printf("iptat param error\r\n");
            rfparam_printf("iptat use default value 0x80\r\n"); // request from xueliang
            g_rfparam_cfg->param.ef.iptat_code = 0x80;
        }
    }

    bflb_ef_ctrl_read_common_trim(NULL, "tmp_mp2", &trim, 1);
    if (trim.en == 1 && trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
        g_rfparam_cfg->param.pwrcal.Temperature_MP = trim.value;
        rfparam_printf("tmp_mp2 value %d\r\n", (int)trim.value);
    } else {
        bflb_ef_ctrl_read_common_trim(NULL, "tmp_mp1", &trim, 1);
        if (trim.en == 1 && trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            g_rfparam_cfg->param.pwrcal.Temperature_MP = trim.value;
            rfparam_printf("tmp_mp1 value %d\r\n", (int)trim.value);
        } else {
            bflb_ef_ctrl_read_common_trim(NULL, "tmp_mp0", &trim, 1);
            if (trim.en == 1 && trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
                g_rfparam_cfg->param.pwrcal.Temperature_MP = trim.value;
                rfparam_printf("tmp_mp0 value %d\r\n", (int)trim.value);
            } else {
                g_rfparam_cfg->param.pwrcal.Temperature_MP = 35; // request from xueliang
                rfparam_printf("tmp_mp use default value 35\r\n");
            }
        }
    }

    ret = wl_init();

    return ret;
}
