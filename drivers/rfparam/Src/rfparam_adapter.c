#include "rfparam_adapter.h"
#include "wl_api.h"

#if defined(RFPARAM_BL616)
#include "bl616_aon.h"
#include "bl616_mfg_media.h"
#include "bl616_hbn.h"
#endif

#if defined(RFPARAM_WB03)
#include "wb03_aon.h"
#include "wb03_mfg_media.h"
#include "wb03_hbn.h"
#endif
#ifdef BL_PSM_RFPARAM
#include "easyflash.h"
#endif


#define DBG_TAG "rfparam"
#include "log.h"

BFLOG_DEFINE_TAG(RFPARAM, DBG_TAG, true);

#undef BFLOG_TAG
#define BFLOG_TAG BFLOG_GET_TAG(RFPARAM)
#ifdef BL_PSM_RFPARAM
#define PSM_RFPARAM_ENABLE_SIZE     10
#define PSM_RF_PARAM_SIZE       50
int8_t rfparam_load_psm(struct wl_param_t *param);
#endif

#define rfparam_printf(...)  printf(__VA_ARGS__)// LOG_I(__VA_ARGS__)

// static uint8_t g_rfparam_buf[RFPARAM_WL_API_MEM_SIZE] = {0};
static struct wl_cfg_t *g_rfparam_cfg = NULL;
static uint32_t g_tlv_base_addr;

void rfparam_array_printf(char* str, void * buf, uint32_t buf_len, int type)
{
    uint32_t i;
    uint8_t *a=NULL;
    int8_t *b=NULL;
    uint16_t *c=NULL;
    int16_t *d=NULL;

    rfparam_printf("%s[%ld]: ", str, buf_len);

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
    puts("\r\n");
}


/****************************************************************************//**
 * @brief  get wifi power offset by mode option
 *
 * @param  base_addr: rftlv base addr
 *         pwr_offset:return value
 *
 * @return RFPARAM_SUSS is suss , other is err
 *
*******************************************************************************/
int32_t rfparam_get_wlan_pwroffset_with_option(uint32_t base_addr,int8_t pwr_offset[14],uint8_t reload)
{


    uint8_t pwrmode[RFTLV_MAXLEN_PWR_MODE+1]={0};
    int8_t pwroffset_tmp[RFTLV_MAXLEN_PWR_OFFSET]={0};

    /* pwr_offset_mode */
    if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, pwrmode) > 0) {
        rfparam_printf("pwr_mode is %s\r\n", pwrmode);
    }else{
    	rfparam_printf("pwr_mode null\r\n");
    	return RFPARAM_ERR_PWR_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_PWR_MODE; i++) {
        switch (pwrmode[i]) {
        	/* read pwr offset from efuse */
			case 'B':
			case 'b':
			{
			    if(0 == mfg_media_read_hp_poweroffset_with_lock(pwroffset_tmp,reload)){
				    rfparam_array_printf((char *)"efuse wlan pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('B' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('b' == pwrmode[i]){
						for(int i = 0; i < sizeof(pwroffset_tmp); i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("no pwr_offset in efuse\r\n");
				}


			}
			break;

			/* read pwr offset from rftlv */
            case 'F':
            case 'f':
            {
				if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_OFFSET, RFTLV_MAXLEN_PWR_OFFSET, pwroffset_tmp) > 0) {
				    for(int i = 0; i < 14; i++){
			            pwroffset_tmp[i] -= 16;
			        }
					rfparam_array_printf((char *)"tlv wlan pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('F' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('f' == pwrmode[i]){
						for(int i = 0; i < sizeof(pwroffset_tmp); i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("tlv pwr_offset null\r\n");
				}
            }
            break;

            default:
            {
                rfparam_printf("not support mode\r\n");
            }
        }
    }



    return RFPARAM_SUSS;

}

/****************************************************************************//**
 * @brief  get wifi power offset by mode option
 *
 * @param  base_addr: rftlv base addr
 *         pwr_offset:return value
 *
 * @return RFPARAM_SUSS is suss , other is err
 *
*******************************************************************************/
int32_t rfparam_get_wlan_pwroffset_lp_with_option(uint32_t base_addr,int8_t pwr_offset[14],uint8_t reload)
{


    uint8_t pwrmode[RFTLV_MAXLEN_PWR_MODE+1]={0};
    int8_t pwroffset_tmp[RFTLV_MAXLEN_PWR_OFFSET]={0};

    /* pwr_offset_mode */
    if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, pwrmode) > 0) {
    }else{
    	return RFPARAM_ERR_PWR_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_PWR_MODE; i++) {
        switch (pwrmode[i]) {
        	/* read pwr offset from efuse */
			case 'B':
			case 'b':
			{
			    if(0 == mfg_media_read_lp_poweroffset_with_lock(pwroffset_tmp,reload)){
				    rfparam_array_printf((char *)"efuse wlan lp pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('B' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('b' == pwrmode[i]){
						for(int i = 0; i < sizeof(pwroffset_tmp); i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("no lp pwr_offset in efuse\r\n");
				}


			}
			break;

			/* read pwr offset from rftlv */
            case 'F':
            case 'f':
            {
				if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_OFFSET_LP, RFTLV_MAXLEN_PWR_OFFSET_LP, pwroffset_tmp) > 0) {
				    for(int i = 0; i < 14; i++){
			            pwroffset_tmp[i] -= 16;
			        }
					rfparam_array_printf((char *)"tlv wlan lp pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('F' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('f' == pwrmode[i]){
						for(int i = 0; i < sizeof(pwroffset_tmp); i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("tlv lp pwr_offset null\r\n");
				}
            }
            break;

            default:
            {
                rfparam_printf("not support mode\r\n");
            }
        }
    }


    return RFPARAM_SUSS;


}


/****************************************************************************//**
 * @brief  get bz power offset by mode option
 *
 * @param  base_addr: rftlv base addr
 *         pwr_offset:return value
 *
 * @return RFPARAM_SUSS is suss , other is err
 *
*******************************************************************************/
int32_t rfparam_get_bz_pwroffset_with_option(uint32_t base_addr,int8_t pwr_offset[5],uint8_t reload)
{


    uint8_t pwrmode[RFTLV_MAXLEN_PWR_MODE+1] = {0};
    int8_t pwroffset_tmp[RFTLV_MAXLEN_BLE_PWR_OFFSET] = {0};

    /* pwr_offset_mode */
    if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, pwrmode) > 0) {
    }else{
    	return RFPARAM_ERR_PWR_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_PWR_MODE; i++) {
        switch (pwrmode[i]) {
        	/* read pwr offset from efuse */
			case 'B':
			case 'b':
			{
			    if(0 == mfg_media_read_bz_poweroffset_with_lock(pwroffset_tmp,reload)){

				    rfparam_array_printf((char *)"efuse bz pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('B' == pwrmode[i]){
					    memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('b' == pwrmode[i]){
						for(int i = 0; i < RFTLV_MAXLEN_BLE_PWR_OFFSET; i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("no bz pwr_offset in efuse\r\n");
				}


			}
			break;

			/* read pwr offset from rftlv */
            case 'F':
            case 'f':
            {
				if (rfparam_tlv_get(base_addr,RFTLV_TYPE_PWR_OFFSET_BLE, RFTLV_MAXLEN_BLE_PWR_OFFSET, pwroffset_tmp) > 0) {
				    for(int i = 0; i < RFTLV_MAXLEN_BLE_PWR_OFFSET; i++){
			            pwroffset_tmp[i] -= 16;
			        }
					rfparam_array_printf((char *)"tlv bz pwr_offset",pwroffset_tmp,sizeof(pwroffset_tmp),TYPE_INT8);
					if('F' == pwrmode[i]){
						memcpy(pwr_offset,pwroffset_tmp,sizeof(pwroffset_tmp));
						return RFPARAM_SUSS;
					}else if('f' == pwrmode[i]){
						for(int i = 0; i < RFTLV_MAXLEN_BLE_PWR_OFFSET; i++){
							pwr_offset[i] += pwroffset_tmp[i];
						}
					}
				}else{
					rfparam_printf("tlv pwr_offset null\r\n");
				}
            }
            break;

            default:
            {
                rfparam_printf("not support mode\r\n");
            }
        }
    }


    return RFPARAM_SUSS;

}

/****************************************************************************//**
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
    uint8_t capmode[RFTLV_MAXLEN_XTAL_MODE+1]={0};
    // uint8_t capcode[RFTLV_MAXLEN_XTAL/4]={0};
    uint8_t tmp_buf[32];

    if (rfparam_tlv_get(base_addr,RFTLV_TYPE_XTAL_MODE, RFTLV_MAXLEN_XTAL_MODE, capmode) > 0) {
        rfparam_printf("capcode mode is %s\r\n",capmode);
    }else{
    	rfparam_printf("capcode mode null\r\n");
    	return RFPARAM_ERR_CAP_CODE_MODE_NULL;
    }

    for (int i = 0; i < RFTLV_MAXLEN_XTAL_MODE; i++) {
        switch (capmode[i]) {
            case 'F':
            {
                if (rfparam_tlv_get(base_addr,RFTLV_TYPE_XTAL, RFTLV_MAXLEN_XTAL, tmp_buf) > 0) {
                    *capcode_in = tmp_buf[0];
                    *capcode_out = tmp_buf[4];
                    rfparam_printf("tlv capcode_in %d,capcode_out %d\r\n",*capcode_in,*capcode_out);
                    return RFPARAM_SUSS;
                }else{
                	rfparam_printf("tlv capcode null\r\n");
                }
            }
            break;
            case 'M':
            {
                if(0 == mfg_media_read_xtal_capcode_with_lock(capcode_in, reload)){
                    *capcode_out = *capcode_in;
                    rfparam_printf("efuse capcode_in %d,capcode_out %d\r\n",*capcode_in,*capcode_out);
                    return RFPARAM_SUSS;
                }else{
                    rfparam_printf("no capcode in efuse\r\n");
                }
            }
            break;
            default:
            {
                rfparam_printf("not support mode\r\n");
            }
        }
    }

    return RFPARAM_SUSS;
}

void rfparam_get_capcode(uint8_t* capcode_in, uint8_t* capcode_out)
{
    *capcode_in = *capcode_out = AON_Get_Xtal_CapCode();
}

void rfparam_set_capcode(uint8_t capcode_in, uint8_t capcode_out)
{
    AON_Set_Xtal_CapCode(capcode_in, capcode_out);
}

/****************************************************************************//**
 * @brief  rfparam_load
 *
 * @param  rf_para:   param return
 *
 * @return RFPARAM_SUSS is suss, other is err
 *
*******************************************************************************/
int8_t rfparam_load(struct wl_param_t *param)
{

    uint8_t tmp_buf[32]={0};
#ifdef BL_PSM_RFPARAM
    /*TODO need FIX btble controller init call btble rf init call rfparam_load again because of btblecontroller lib so old*/
    char value[PSM_RFPARAM_ENABLE_SIZE + 1] = {0};
    size_t get_size = 0;

    get_size = ef_get_env_blob("psm_rfparam_enable", value, PSM_RFPARAM_ENABLE_SIZE, NULL);
    printf("psm_rfparam_enable get size %d string %s\r\n", get_size, value);
    if((get_size > 0) && strlen(value) == 1 && (value[0] == '1')){
        return rfparam_load_psm(param);
    }
#endif


    if(((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_RAM_CACHE & RFPARAM_TLV_BASE_ADDR_MASK)){
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_RAM_CACHE;
    }else if(((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_RAM_NO_CACHE & RFPARAM_TLV_BASE_ADDR_MASK)){
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_RAM_NO_CACHE;
    }else if(((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_XIP_FLASH & RFPARAM_TLV_BASE_ADDR_MASK)){
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_XIP_FLASH;
    }else{
        return RFPARAM_ERR_TLV_BASE_ADDR;
    }

    if(param == NULL){
        return RFPARAM_ERR_PARAM_CHECK;
    }

    /* 1. get all rf params */
    /* pwr_offset_mode */

    if (RFPARAM_SUSS == rfparam_get_wlan_pwroffset_with_option(g_tlv_base_addr, param->pwrcal.channel_pwrcomp_wlan,1)){
        rfparam_array_printf((char *)"wlan pwr_offset",(void *)param->pwrcal.channel_pwrcomp_wlan,sizeof(param->pwrcal.channel_pwrcomp_wlan),TYPE_INT8);
    }else{
        rfparam_printf("wlan pwr_offset err\r\n");
        return RFPARAM_ERR_PWR_OFFSET_NULL;
    }

    if (RFPARAM_SUSS == rfparam_get_wlan_pwroffset_lp_with_option(g_tlv_base_addr, param->pwrcal.channel_lp_pwrcomp_wlan,1)){
        rfparam_array_printf((char *)"wlan lp pwr_offset",(void *)param->pwrcal.channel_lp_pwrcomp_wlan,sizeof(param->pwrcal.channel_lp_pwrcomp_wlan),TYPE_INT8);
    }else{
        rfparam_printf("wlan lp pwr_offset err\r\n");
        return RFPARAM_ERR_PWR_OFFSET_NULL;
    }

    if (RFPARAM_SUSS == rfparam_get_bz_pwroffset_with_option(g_tlv_base_addr, param->pwrcal.channel_pwrcomp_bz,1)){
        rfparam_array_printf((char *)"bz pwr_offset",(void *)param->pwrcal.channel_pwrcomp_bz,sizeof(param->pwrcal.channel_pwrcomp_bz),TYPE_INT8);
    }else{
        rfparam_printf("bz pwr_offset err\r\n");
        return RFPARAM_ERR_PWR_OFFSET_NULL;
    }



    /* pwr table */
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11B, RFTLV_MAXLEN_PWR_TABLE_11B, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11b,tmp_buf,sizeof(param->pwrtarget.pwr_11b));

        rfparam_array_printf((char *)"pwr_11b",(void *)param->pwrtarget.pwr_11b,sizeof(param->pwrtarget.pwr_11b),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11b null\r\n");
    	return RFPARAM_ERR_PWR_11B_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11G, RFTLV_MAXLEN_PWR_TABLE_11G, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11g,tmp_buf,sizeof(param->pwrtarget.pwr_11g));
        rfparam_array_printf((char *)"pwr_11g",(void *)param->pwrtarget.pwr_11g,sizeof(param->pwrtarget.pwr_11g),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11g null\r\n");
    	return RFPARAM_ERR_PWR_11G_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11N, RFTLV_MAXLEN_PWR_TABLE_11N, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11n_ht20,tmp_buf,sizeof(param->pwrtarget.pwr_11n_ht20));
        rfparam_array_printf((char *)"pwr_11n_ht20",(void *)param->pwrtarget.pwr_11n_ht20,sizeof(param->pwrtarget.pwr_11n_ht20),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11n_ht20 null\r\n");
    	return RFPARAM_ERR_PWR_11N_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11N_HT40, RFTLV_MAXLEN_PWR_TABLE_11N_HT40, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11n_ht40,tmp_buf,sizeof(param->pwrtarget.pwr_11n_ht40));
        rfparam_array_printf((char *)"pwr_11n_ht40",(void *)param->pwrtarget.pwr_11n_ht40,sizeof(param->pwrtarget.pwr_11n_ht40),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11n_ht40 null\r\n");
    	return RFPARAM_ERR_PWR_11N_HT40_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AC_VHT20, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT20, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ac_vht20,tmp_buf,sizeof(param->pwrtarget.pwr_11ac_vht20));
        rfparam_array_printf((char *)"pwr_11ac_vht20",(void *)param->pwrtarget.pwr_11ac_vht20,sizeof(param->pwrtarget.pwr_11ac_vht20),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ac_vht20 null\r\n");
    	return RFPARAM_ERR_PWR_11AC_VHT20_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AC_VHT40, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT40, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ac_vht40,tmp_buf,sizeof(param->pwrtarget.pwr_11ac_vht40));
        rfparam_array_printf((char *)"pwr_11ac_vht40",(void *)param->pwrtarget.pwr_11ac_vht40,sizeof(param->pwrtarget.pwr_11ac_vht40),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ac_vht40 null\r\n");
    	return RFPARAM_ERR_PWR_11AC_VHT40_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AC_VHT80, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT80, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ac_vht80,tmp_buf,sizeof(param->pwrtarget.pwr_11ac_vht80));
        rfparam_array_printf((char *)"pwr_11ac_vht80",(void *)param->pwrtarget.pwr_11ac_vht80,sizeof(param->pwrtarget.pwr_11ac_vht80),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ac_vht80 null\r\n");
    	return RFPARAM_ERR_PWR_11AC_VHT80_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE20, RFTLV_MAXLEN_PWR_TABLE_11AX_HE20, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he20,tmp_buf,sizeof(param->pwrtarget.pwr_11ax_he20));
        rfparam_array_printf((char *)"pwr_11ax_he20",(void *)param->pwrtarget.pwr_11ax_he20,sizeof(param->pwrtarget.pwr_11ax_he20),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ax_he20 null\r\n");
    	return RFPARAM_ERR_PWR_11AX_HE20_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE40, RFTLV_MAXLEN_PWR_TABLE_11AX_HE40, tmp_buf) > 0) {
        memcpy(param->pwrtarget.pwr_11ax_he40,tmp_buf,sizeof(param->pwrtarget.pwr_11ax_he40));
        rfparam_array_printf((char *)"pwr_11ax_he40",(void *)param->pwrtarget.pwr_11ax_he40,sizeof(param->pwrtarget.pwr_11ax_he40),TYPE_INT8);
    }else{
    	rfparam_printf("pwr_11ax_he40 null\r\n");
    	return RFPARAM_ERR_PWR_11AX_HE40_NULL;
    }

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

    /* capcode */
    if(RFPARAM_SUSS == rfparam_get_cap_code_with_option(g_tlv_base_addr,&param->xtalcapcode_in,&param->xtalcapcode_out,1)){
        rfparam_printf("capcode_in %d,capcode_out %d\r\n",param->xtalcapcode_in,param->xtalcapcode_out);
    }else{
        rfparam_printf("capcode err\r\n");
    }

    /*tcal*/
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_EN_TCAL, RFTLV_MAXLEN_EN_TCAL, tmp_buf) > 0) {
		param->tcal.en_tcal=tmp_buf[0];
		rfparam_printf("tcal.en_tcal = %d\r\n", (int)param->tcal.en_tcal);
    }else{
    	rfparam_printf("tcal.en_tcal null\r\n");
    	return RFPARAM_ERR_TCAL_EN_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_LINEAR_OR_FOLLOW, RFTLV_MAXLEN_LINEAR_OR_FOLLOW, tmp_buf) > 0) {
        param->tcal.linear_or_follow=tmp_buf[0];
        rfparam_printf("tcal.linear_or_follow = %d\r\n", (int)param->tcal.linear_or_follow);
    }else{
    	rfparam_printf("tcal.linear_or_follow null\r\n");
    	return RFPARAM_ERR_TCAL_LINEAR_OR_FOLLOW_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCHANNELS, RFTLV_MAXLEN_TCHANNELS, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNELS/2; i++){
            param->tcal.Tchannels[i]= tmp_buf[i*2] | (tmp_buf[i*2+1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannels",(void *)param->tcal.Tchannels,sizeof(param->tcal.Tchannels)/2,TYPE_UINT16);
    }else{
    	rfparam_printf("tcal.Tchannels null\r\n");
    	return RFPARAM_ERR_TCAL_CHANNELS_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCHANNEL_OS, RFTLV_MAXLEN_TCHANNEL_OS, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS/2; i++){
            param->tcal.Tchannel_os[i]=tmp_buf[i*2] | (tmp_buf[i*2+1]<<8);
        }
        rfparam_array_printf((char *)"tcal.Tchannel_os",(void *)param->tcal.Tchannel_os,sizeof(param->tcal.Tchannel_os)/2,TYPE_INT16);
    }else{
    	rfparam_printf("tcal.Tchannel_os null\r\n");
    	return RFPARAM_ERR_TCAL_CHANNEL_OS_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCHANNEL_OS_LOW, RFTLV_MAXLEN_TCHANNEL_OS_LOW, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS_LOW/2; i++){
            param->tcal.Tchannel_os_low[i]=tmp_buf[i*2] | (tmp_buf[i*2+1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannel_os_low",(void *)param->tcal.Tchannel_os_low,sizeof(param->tcal.Tchannel_os_low)/2,TYPE_INT16);
    }else{
    	rfparam_printf("tcal.Tchannel_os_low null\r\n");
    	return RFPARAM_ERR_TCAL_CHANNEL_OS_LOW_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TROOM_OS, RFTLV_MAXLEN_TROOM_OS, tmp_buf) > 0) {
        param->tcal.Troom_os=(tmp_buf[0] | (tmp_buf[1]<<8))-256;
		rfparam_printf("tcal.Troom_os = %d\r\n", (int)param->tcal.Troom_os);
    }else{
    	rfparam_printf("tcal.Troom_os null\r\n");
    	return RFPARAM_ERR_TCAL_ROOM_OS_NULL;
    }

    /*ble*/
    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_BLE, RFTLV_MAXLEN_PWR_TABLE_BLE, tmp_buf) > 0) {
        param->pwrtarget.pwr_ble = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8) | (tmp_buf[2] << 16) | (tmp_buf[3] << 24));
        if((uint8_t)param->pwrtarget.pwr_ble>128){
        	param->pwrtarget.pwr_ble-=256;
        }

        rfparam_printf("pwr_ble = %d \r\n", (int)param->pwrtarget.pwr_ble);

    }else{
        rfparam_printf("pwr_ble null\r\n");
        return RFPARAM_ERR_PWR_BLE_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_BT, RFTLV_MAXLEN_PWR_TABLE_BT, tmp_buf) > 0) {
        for(uint32_t i = 0; i < 3; i++){
            param->pwrtarget.pwr_bt[i] = (int8_t)(tmp_buf[i * 4] | (tmp_buf[i * 4 + 1] << 8) | (tmp_buf[i * 4 + 2] << 16) | (tmp_buf[i * 4 + 3] << 24));
            if((uint8_t)param->pwrtarget.pwr_bt[i]>128){
                param->pwrtarget.pwr_bt[i]-=256;
            }
        }

        rfparam_array_printf((char *)"pwr_bt",(void *)param->pwrtarget.pwr_bt,sizeof(param->pwrtarget.pwr_bt),TYPE_INT8);

    }else{
        rfparam_printf("pwr_bt null\r\n");
        return RFPARAM_ERR_PWR_BT_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_ZIGBEE, RFTLV_MAXLEN_PWR_TBALE_ZIGBEE, tmp_buf) > 0) {
        param->pwrtarget.pwr_zigbee = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8) | (tmp_buf[2] << 16) | (tmp_buf[3] << 24));
        if((uint8_t)param->pwrtarget.pwr_zigbee>128){
        	param->pwrtarget.pwr_zigbee-=256;
        }
        rfparam_printf("pwr_zigbee = %d \r\n", (int)param->pwrtarget.pwr_zigbee);

    }else{
        rfparam_printf("pwr_zigbee null\r\n");
        return RFPARAM_ERR_PWR_ZIGBEE_NULL;
    }

    if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_COUNTRY_CODE, RFTLV_MAXLEN_COUNTRY_CODE, tmp_buf) > 0) {
        param->country_code = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8));

        rfparam_printf("country_code = %d \r\n", (int)param->country_code);

    }else{
        rfparam_printf("country_code null\r\n");
        return RFPARAM_ERR_COUNTRY_CODE_NULL;
    }

    return RFPARAM_SUSS;


}

#ifdef BL_PSM_RFPARAM

static void parse_string(char* str, void* arr, int max_len, const char* type) {
    char* tok = strtok(str, ",");
    int i = 0;
    while (tok != NULL && i < max_len) {
        if (strcmp(type, "int8_t") == 0) {
            ((int8_t*) arr)[i] = (int8_t) atoi(tok);
        } else if (strcmp(type, "uint8_t") == 0) {
            ((uint8_t*) arr)[i] = (uint8_t) atoi(tok);
        } else if (strcmp(type, "uint16_t") == 0) {
            ((uint16_t*) arr)[i] = (uint16_t) atoi(tok);
        } else if (strcmp(type, "int16_t") == 0) {
            ((int16_t*) arr)[i] = (int16_t) atoi(tok);
        }
        i++;
        tok = strtok(NULL, ",");
    }
    if (i == 1 && str[strlen(str) - 1] != ',') {
        if (strcmp(type, "int8_t") == 0) {
            ((int8_t*) arr)[0] = (int8_t) atoi(str);
        } else if (strcmp(type, "uint8_t") == 0) {
            ((uint8_t*) arr)[0] = (uint8_t) atoi(str);
        } else if (strcmp(type, "uint16_t") == 0) {
            ((uint16_t*) arr)[0] = (uint16_t) atoi(str);
        } else if (strcmp(type, "int16_t") == 0) {
            ((int16_t*) arr)[0] = (int16_t) atoi(str);
        }
    }
}

/****************************************************************************//**

 * @brief  rfparam_load_psm

 *

 * @param  rf_para:   param return

 *

 * @return RFPARAM_SUSS is suss, other is err

 *

*******************************************************************************/
int8_t rfparam_load_psm(struct wl_param_t *param)
{
    char ef_env[PSM_RF_PARAM_SIZE + 1] = {0};
    size_t ef_env_size = 0;

    if(param == NULL){
        return RFPARAM_ERR_PARAM_CHECK;
    }

    /* get tlv base address */
    uint8_t tmp_buf[32]={0};

    if(((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_RAM_CACHE & RFPARAM_TLV_BASE_ADDR_MASK)){
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_RAM_CACHE;
    }else if(((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_RAM_NO_CACHE & RFPARAM_TLV_BASE_ADDR_MASK)){
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_RAM_NO_CACHE;
    }else if(((uint32_t)rfparam_load & RFPARAM_TLV_BASE_ADDR_MASK) == (RFPARAM_TLV_BASE_ADDR_XIP_FLASH & RFPARAM_TLV_BASE_ADDR_MASK)){
        g_tlv_base_addr = RFPARAM_TLV_BASE_ADDR_XIP_FLASH;
    }else{
        return RFPARAM_ERR_TLV_BASE_ADDR;
    }

    /* 1. get all rf params, if some rf params not in psm, then use rf params saved in tlv */
    /* pwr_offset mode */
    ef_env_size = ef_get_env_blob("wlan_pwr_offset", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env,param->pwrcal.channel_pwrcomp_wlan,sizeof(param->pwrcal.channel_pwrcomp_wlan),"int8_t");
        rfparam_array_printf((char *)"psm wlan_pwr_offset",(void *)param->pwrcal.channel_pwrcomp_wlan,sizeof(param->pwrcal.channel_pwrcomp_wlan),TYPE_INT8);
    }else{
        if (RFPARAM_SUSS == rfparam_get_wlan_pwroffset_with_option(g_tlv_base_addr, param->pwrcal.channel_pwrcomp_wlan,1)){
            rfparam_array_printf((char *)"final wlan_pwr_offset",(void *)param->pwrcal.channel_pwrcomp_wlan,sizeof(param->pwrcal.channel_pwrcomp_wlan),TYPE_INT8);
        }else{
            rfparam_printf("wlan_pwr_offset err\r\n");
            return RFPARAM_ERR_PWR_OFFSET_NULL;
        }
     } 
        

    /* wlan_lp_pwr_offset */
    ef_env_size = ef_get_env_blob("wlan_lp_pwr_offset", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrcal.channel_lp_pwrcomp_wlan, sizeof(param->pwrcal.channel_lp_pwrcomp_wlan),"int8_t");
        rfparam_array_printf((char *)"psm wlan_lp_pwr_offset",(void *)param->pwrcal.channel_lp_pwrcomp_wlan,sizeof(param->pwrcal.channel_lp_pwrcomp_wlan),TYPE_INT8);
    }else{
        if (RFPARAM_SUSS == rfparam_get_wlan_pwroffset_lp_with_option(g_tlv_base_addr, param->pwrcal.channel_lp_pwrcomp_wlan,1)){
            rfparam_array_printf((char *)"final wlan lp_pwr_offset",(void *)param->pwrcal.channel_lp_pwrcomp_wlan,sizeof(param->pwrcal.channel_lp_pwrcomp_wlan),TYPE_INT8);
        }else{
            rfparam_printf("wlan lp pwr_offset err\r\n");
            return RFPARAM_ERR_PWR_OFFSET_NULL;
        }
    }

    /* bz pwr offset */
    ef_env_size = ef_get_env_blob("bz_pwr_offset", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrcal.channel_pwrcomp_bz, sizeof(param->pwrcal.channel_pwrcomp_bz),"int8_t");
        rfparam_array_printf((char *)"psm bz_pwr_offset",(void *)param->pwrcal.channel_pwrcomp_bz,sizeof(param->pwrcal.channel_pwrcomp_bz),TYPE_INT8);
    }else{
        if (RFPARAM_SUSS == rfparam_get_bz_pwroffset_with_option(g_tlv_base_addr, param->pwrcal.channel_pwrcomp_bz,1)){
            rfparam_array_printf((char *)"final bz_pwr_offset",(void *)param->pwrcal.channel_pwrcomp_bz,sizeof(param->pwrcal.channel_pwrcomp_bz),TYPE_INT8);
        }else{
            rfparam_printf("bz pwr_offset err\r\n");
            return RFPARAM_ERR_PWR_OFFSET_NULL;
        }
    }

    /* pwr table 11b */
    ef_env_size = ef_get_env_blob("pwr_11b", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrtarget.pwr_11b, sizeof(param->pwrtarget.pwr_11b),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11b",(void *)param->pwrtarget.pwr_11b,sizeof(param->pwrtarget.pwr_11b),TYPE_INT8);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11B, RFTLV_MAXLEN_PWR_TABLE_11B, tmp_buf) > 0) {
            memcpy(param->pwrtarget.pwr_11b, tmp_buf, sizeof(param->pwrtarget.pwr_11b));
            rfparam_array_printf((char *)"tlv pwr_11b",(void *)param->pwrtarget.pwr_11b,sizeof(param->pwrtarget.pwr_11b),TYPE_INT8);
        }else{
    	    rfparam_printf("pwr_11b null\r\n");
    	    return RFPARAM_ERR_PWR_11B_NULL;
        }
    }

    /* pwr table 11g */
    ef_env_size = ef_get_env_blob("pwr_11g", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrtarget.pwr_11g, sizeof(param->pwrtarget.pwr_11g),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11g",(void *)param->pwrtarget.pwr_11g,sizeof(param->pwrtarget.pwr_11g),TYPE_INT8);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11G, RFTLV_MAXLEN_PWR_TABLE_11G, tmp_buf) > 0) {
            memcpy(param->pwrtarget.pwr_11g, tmp_buf, sizeof(param->pwrtarget.pwr_11g));
            rfparam_array_printf((char *)"tlv pwr_11g",(void *)param->pwrtarget.pwr_11g,sizeof(param->pwrtarget.pwr_11g),TYPE_INT8);
        }else{
    	    rfparam_printf("pwr_11g null\r\n");
    	    return RFPARAM_ERR_PWR_11G_NULL;
        }
    }

    /* pwr 11n HT20 */
    ef_env_size = ef_get_env_blob("pwr_11n_ht20", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrtarget.pwr_11n_ht20, sizeof(param->pwrtarget.pwr_11n_ht20),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11n_ht20",(void *)param->pwrtarget.pwr_11n_ht20,sizeof(param->pwrtarget.pwr_11n_ht20),TYPE_INT8);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11N, RFTLV_MAXLEN_PWR_TABLE_11N, tmp_buf) > 0) {
            memcpy(param->pwrtarget.pwr_11n_ht20, tmp_buf, sizeof(param->pwrtarget.pwr_11n_ht20));
            rfparam_array_printf((char *)"tlv pwr_11n_ht20",(void *)param->pwrtarget.pwr_11n_ht20,sizeof(param->pwrtarget.pwr_11n_ht20),TYPE_INT8);
        }else{
    	    rfparam_printf("pwr_11n_ht20 null\r\n");
    	    return RFPARAM_ERR_PWR_11N_NULL;
        }
    }

    /* pwr 11n HT40 */
    ef_env_size = ef_get_env_blob("pwr_11n_ht40", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrtarget.pwr_11n_ht40, sizeof(param->pwrtarget.pwr_11n_ht40),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11n_ht40",(void *)param->pwrtarget.pwr_11n_ht40,sizeof(param->pwrtarget.pwr_11n_ht40),TYPE_INT8);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11N_HT40, RFTLV_MAXLEN_PWR_TABLE_11N_HT40, tmp_buf) > 0) {
            memcpy(param->pwrtarget.pwr_11n_ht40, tmp_buf, sizeof(param->pwrtarget.pwr_11n_ht40));
            rfparam_array_printf((char *)"tlv pwr_11n_ht40",(void *)param->pwrtarget.pwr_11n_ht40,sizeof(param->pwrtarget.pwr_11n_ht40),TYPE_INT8);
        }else{
    	    rfparam_printf("pwr_11n_ht40 null\r\n");
    	    return RFPARAM_ERR_PWR_11N_HT40_NULL;
        }
    }

    /* pwr 11ac VHT20 */
    ef_env_size = ef_get_env_blob("pwr_11ac_vht20", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrtarget.pwr_11ac_vht20, sizeof(param->pwrtarget.pwr_11ac_vht20),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11ac_vht20",(void *)param->pwrtarget.pwr_11ac_vht20,sizeof(param->pwrtarget.pwr_11ac_vht20),TYPE_INT8);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AC_VHT20, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT20, tmp_buf) > 0) {
            memcpy(param->pwrtarget.pwr_11ac_vht20, tmp_buf, sizeof(param->pwrtarget.pwr_11ac_vht20));
            rfparam_array_printf((char *)"tlv pwr_11ac_vht20",(void *)param->pwrtarget.pwr_11ac_vht20,sizeof(param->pwrtarget.pwr_11ac_vht20),TYPE_INT8);
        }else{
    	    rfparam_printf("pwr_11ac_vht20 null\r\n");
    	    return RFPARAM_ERR_PWR_11AC_VHT20_NULL;
        }
    }

    /* pwr 11ac VHT40 */
    ef_env_size = ef_get_env_blob("pwr_11ac_vht40", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrtarget.pwr_11ac_vht40, sizeof(param->pwrtarget.pwr_11ac_vht40),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11ac_vht40",(void *)param->pwrtarget.pwr_11ac_vht40,sizeof(param->pwrtarget.pwr_11ac_vht40),TYPE_INT8);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AC_VHT40, RFTLV_MAXLEN_PWR_TABLE_11AC_VHT40, tmp_buf) > 0) {
            memcpy(param->pwrtarget.pwr_11ac_vht40, tmp_buf, sizeof(param->pwrtarget.pwr_11ac_vht40));
            rfparam_array_printf((char *)"tlv pwr_11ac_vht40",(void *)param->pwrtarget.pwr_11ac_vht40,sizeof(param->pwrtarget.pwr_11ac_vht40),TYPE_INT8);
        }else{
    	    rfparam_printf("pwr_11ac_vht40 null\r\n");
    	    return RFPARAM_ERR_PWR_11AC_VHT40_NULL;
        }
    }

    /* pwr 11ax HE20 */
    ef_env_size = ef_get_env_blob("pwr_11ax_he20", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrtarget.pwr_11ax_he20, sizeof(param->pwrtarget.pwr_11ax_he20),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11ax_he20",(void *)param->pwrtarget.pwr_11ax_he20,sizeof(param->pwrtarget.pwr_11ax_he20),TYPE_INT8);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE20, RFTLV_MAXLEN_PWR_TABLE_11AX_HE20, tmp_buf) > 0) {
            memcpy(param->pwrtarget.pwr_11ax_he20, tmp_buf, sizeof(param->pwrtarget.pwr_11ax_he20));
            rfparam_array_printf((char *)"tlv pwr_11ax_he20",(void *)param->pwrtarget.pwr_11ax_he20,sizeof(param->pwrtarget.pwr_11ax_he20),TYPE_INT8);
        }else{
    	    rfparam_printf("pwr_11ax_he20 null\r\n");
    	    return RFPARAM_ERR_PWR_11AX_HE20_NULL;
        }
    }

    /* pwr 11ax HE40 */
    ef_env_size = ef_get_env_blob("pwr_11ax_he40", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrtarget.pwr_11ax_he40, sizeof(param->pwrtarget.pwr_11ax_he40),"int8_t");
        rfparam_array_printf((char *)"psm pwr_11ax_he40",(void *)param->pwrtarget.pwr_11ax_he40,sizeof(param->pwrtarget.pwr_11ax_he40),TYPE_INT8);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_11AX_HE40, RFTLV_MAXLEN_PWR_TABLE_11AX_HE40, tmp_buf) > 0) {
            memcpy(param->pwrtarget.pwr_11ax_he40, tmp_buf, sizeof(param->pwrtarget.pwr_11ax_he40));
            rfparam_array_printf((char *)"tlv pwr_11ax_he40",(void *)param->pwrtarget.pwr_11ax_he40,sizeof(param->pwrtarget.pwr_11ax_he40),TYPE_INT8);
        }else{
    	    rfparam_printf("pwr_11ax_he40 null\r\n");
    	    return RFPARAM_ERR_PWR_11AX_HE40_NULL;
        }
    }

    /* xtal40M capcode , its better not to use this parameter in psm */
    ef_env_size = ef_get_env_blob("capcode_in", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, &param->xtalcapcode_in, sizeof(param->xtalcapcode_in),"uint8_t");
        rfparam_printf("psm capcode_in %d\r\n",param->xtalcapcode_in);
    }else{
        if(RFPARAM_SUSS == rfparam_get_cap_code_with_option(g_tlv_base_addr,&param->xtalcapcode_in,&param->xtalcapcode_out,1)){
            rfparam_printf("final apcode_in: %d, final capcode_out: %d\r\n",param->xtalcapcode_in,param->xtalcapcode_out);
        }else{
            rfparam_printf("capcode err\r\n");
        }
    }


    ef_env_size = ef_get_env_blob("capcode_out", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, &param->xtalcapcode_out, sizeof(param->xtalcapcode_out),"uint8_t");
        rfparam_printf("psm capcode_out %d\r\n",param->xtalcapcode_out);
    }else{
        if(RFPARAM_SUSS == rfparam_get_cap_code_with_option(g_tlv_base_addr,&param->xtalcapcode_in,&param->xtalcapcode_out,1)){
            rfparam_printf("final capcode_in: %d, final capcode_out: %d\r\n",param->xtalcapcode_in,param->xtalcapcode_out);
        }else{
            rfparam_printf("capcode err\r\n");
        }
    }

    // ==================================================
    /*  tcal power, Don't set this parameters in psm */
    ef_env_size = ef_get_env_blob("tcal.en_tcal", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, &param->tcal.en_tcal, sizeof(param->tcal.en_tcal),"uint8_t");
        rfparam_printf("psm tcal.en_tcal %d\r\n",param->tcal.en_tcal);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_EN_TCAL, RFTLV_MAXLEN_EN_TCAL, tmp_buf) > 0) {
		    param->tcal.en_tcal=tmp_buf[0];
		    rfparam_printf("tlv tcal.en_tcal = %d\r\n", (int)param->tcal.en_tcal);
        }else{
    	    rfparam_printf("tcal.en_tcal null\r\n");
    	    return RFPARAM_ERR_TCAL_EN_NULL;
        }
    }

    ef_env_size = ef_get_env_blob("tcal.linear_or_follow", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, &param->tcal.linear_or_follow, sizeof(param->tcal.linear_or_follow),"uint8_t");
        rfparam_printf("psm tcal.linear_or_follow %d\r\n",param->tcal.linear_or_follow);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_LINEAR_OR_FOLLOW, RFTLV_MAXLEN_LINEAR_OR_FOLLOW, tmp_buf) > 0) {
            param->tcal.linear_or_follow=tmp_buf[0];
            rfparam_printf("tlv tcal.linear_or_follow = %d\r\n", (int)param->tcal.linear_or_follow);
        }else{
    	    rfparam_printf("tcal.linear_or_follow null\r\n");
    	    return RFPARAM_ERR_TCAL_LINEAR_OR_FOLLOW_NULL;
        }
    }

    ef_env_size = ef_get_env_blob("tcal.Tchannels", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->tcal.Tchannels, sizeof(param->tcal.Tchannels)/2,"uint16_t");
        rfparam_array_printf((char *)"psm tcal.Tchannels",(void *)param->tcal.Tchannels,sizeof(param->tcal.Tchannels)/2,TYPE_UINT16);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCHANNELS, RFTLV_MAXLEN_TCHANNELS, tmp_buf) > 0) {
            for (int i = 0; i < RFTLV_MAXLEN_TCHANNELS/2; i++){
                param->tcal.Tchannels[i]= tmp_buf[i*2] | (tmp_buf[i*2+1] << 8);
            }
            rfparam_array_printf((char *)"tlv tcal.Tchannels",(void *)param->tcal.Tchannels,sizeof(param->tcal.Tchannels)/2,TYPE_UINT16);
        }else{
    	    rfparam_printf("tcal.Tchannels null\r\n");
    	    return RFPARAM_ERR_TCAL_CHANNELS_NULL;
        }
    }

    ef_env_size = ef_get_env_blob("tcal.Tchannel_os", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->tcal.Tchannel_os, sizeof(param->tcal.Tchannel_os)/2,"int16_t");
        rfparam_array_printf((char *)"psm tcal.Tchannel_os",(void *)param->tcal.Tchannel_os,sizeof(param->tcal.Tchannel_os)/2,TYPE_UINT16);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCHANNEL_OS, RFTLV_MAXLEN_TCHANNEL_OS, tmp_buf) > 0) {
            for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS/2; i++){
                param->tcal.Tchannel_os[i]=tmp_buf[i*2] | (tmp_buf[i*2+1]<<8);
            }
            rfparam_array_printf((char *)"tlv tcal.Tchannel_os",(void *)param->tcal.Tchannel_os,sizeof(param->tcal.Tchannel_os)/2,TYPE_INT16);
        }else{
    	    rfparam_printf("tcal.Tchannel_os null\r\n");
    	    return RFPARAM_ERR_TCAL_CHANNEL_OS_NULL;
        }
    }

    ef_env_size = ef_get_env_blob("tcal.Tchannel_os_low", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->tcal.Tchannel_os_low, sizeof(param->tcal.Tchannel_os_low)/2,"int16_t");
        rfparam_array_printf((char *)"psm tcal.Tchannel_os_low",(void *)param->tcal.Tchannel_os_low,sizeof(param->tcal.Tchannel_os_low)/2,TYPE_UINT16);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TCHANNEL_OS_LOW, RFTLV_MAXLEN_TCHANNEL_OS_LOW, tmp_buf) > 0) {
            for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS_LOW/2; i++){
                param->tcal.Tchannel_os_low[i]=tmp_buf[i*2] | (tmp_buf[i*2+1] << 8);
            }
            rfparam_array_printf((char *)"tlv tcal.Tchannel_os_low",(void *)param->tcal.Tchannel_os_low,sizeof(param->tcal.Tchannel_os_low)/2,TYPE_INT16);
        }else{
    	    rfparam_printf("tcal.Tchannel_os_low null\r\n");
    	    return RFPARAM_ERR_TCAL_CHANNEL_OS_LOW_NULL;
        }
    }

    ef_env_size = ef_get_env_blob("tcal.Troom_os", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, &param->tcal.Troom_os, sizeof(param->tcal.Troom_os)/2,"int16_t");
        rfparam_printf("psm tcal.Troom_os = %d\r\n", (int)param->tcal.Troom_os);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_TROOM_OS, RFTLV_MAXLEN_TROOM_OS, tmp_buf) > 0) {
            param->tcal.Troom_os=(tmp_buf[0] | (tmp_buf[1]<<8))-256;
		    rfparam_printf("tlv tcal.Troom_os = %d\r\n", (int)param->tcal.Troom_os);
        }else{
    	    rfparam_printf("tcal.Troom_os null\r\n");
    	    return RFPARAM_ERR_TCAL_ROOM_OS_NULL;
        }
    }
    // ============ end about tcal-pwr =========================

    /* pwr ble */
    ef_env_size = ef_get_env_blob("pwr_ble", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        // parse_string(ef_env, &param->pwrtarget.pwr_ble, sizeof(param->pwrtarget.pwr_ble)/2,"int8_t");
        // rfparam_printf("psm pwr_ble = %d \r\n", (int)param->pwrtarget.pwr_ble);
        parse_string(ef_env, &param->pwrtarget.pwr_ble, sizeof(param->pwrtarget.pwr_ble),"int8_t");
        rfparam_printf("psm pwr_ble: %d \r\n", (int)param->pwrtarget.pwr_ble);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_BLE, RFTLV_MAXLEN_PWR_TABLE_BLE, tmp_buf) > 0) {
            param->pwrtarget.pwr_ble = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8) | (tmp_buf[2] << 16) | (tmp_buf[3] << 24));
        if((uint8_t)param->pwrtarget.pwr_ble>128){
        	param->pwrtarget.pwr_ble-=256;
        }
        rfparam_printf("tlv pwr_ble: %d \r\n", (int)param->pwrtarget.pwr_ble);
        }else{
        rfparam_printf("pwr_ble null\r\n");
        return RFPARAM_ERR_PWR_BLE_NULL;
        }
    }

    /* pwr bt */
    ef_env_size = ef_get_env_blob("pwr_bt", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, param->pwrtarget.pwr_bt, sizeof(param->pwrtarget.pwr_bt),"int8_t");
        rfparam_array_printf((char *)"psm pwr_bt",(void *)param->pwrtarget.pwr_bt,sizeof(param->pwrtarget.pwr_bt),TYPE_INT8);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_BT, RFTLV_MAXLEN_PWR_TABLE_BT, tmp_buf) > 0) {
            for(uint32_t i = 0; i < 3; i++){
                param->pwrtarget.pwr_bt[i] = (int8_t)(tmp_buf[i * 4] | (tmp_buf[i * 4 + 1] << 8) | (tmp_buf[i * 4 + 2] << 16) | (tmp_buf[i * 4 + 3] << 24));
                if((uint8_t)param->pwrtarget.pwr_bt[i]>128){
                    param->pwrtarget.pwr_bt[i]-=256;
                }
            }
            rfparam_array_printf((char *)"tlv pwr_bt",(void *)param->pwrtarget.pwr_bt,sizeof(param->pwrtarget.pwr_bt),TYPE_INT8);
        }else{
            rfparam_printf("pwr_bt null\r\n");
            return RFPARAM_ERR_PWR_BT_NULL;
        }
    }
    
    /* pwr zigbee */
    ef_env_size = ef_get_env_blob("pwr_zigbee", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, &param->pwrtarget.pwr_zigbee, sizeof(param->pwrtarget.pwr_zigbee),"int8_t");
        rfparam_printf("psm pwr_zigbee: %d \r\n", (int)param->pwrtarget.pwr_zigbee);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_PWR_TABLE_ZIGBEE, RFTLV_MAXLEN_PWR_TBALE_ZIGBEE, tmp_buf) > 0) {
            param->pwrtarget.pwr_zigbee = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8) | (tmp_buf[2] << 16) | (tmp_buf[3] << 24));
            if((uint8_t)param->pwrtarget.pwr_zigbee>128){
        	    param->pwrtarget.pwr_zigbee-=256;
            }
            rfparam_printf("tlv pwr_zigbee: %d \r\n", (int)param->pwrtarget.pwr_zigbee);
        }else{
            rfparam_printf("pwr_zigbee null\r\n");
            return RFPARAM_ERR_PWR_ZIGBEE_NULL;
        }
    }

    /* country code  */
    ef_env_size = ef_get_env_blob("country_code", ef_env, PSM_RF_PARAM_SIZE, NULL);
    if(ef_env_size){
        ef_env[ef_env_size] = '\0';
        parse_string(ef_env, &param->country_code, sizeof(param->country_code)/2,"uint16_t");
        rfparam_printf("psm country_code: %d \r\n", (int)param->country_code);
    }else{
        if (rfparam_tlv_get(g_tlv_base_addr,RFTLV_TYPE_COUNTRY_CODE, RFTLV_MAXLEN_COUNTRY_CODE, tmp_buf) > 0) {
            param->country_code = (int8_t)(tmp_buf[0] | (tmp_buf[1] << 8));
            rfparam_printf("tlv country_code: %d \r\n", (int)param->country_code);
        }else{
            rfparam_printf("country_code null\r\n");
            return RFPARAM_ERR_COUNTRY_CODE_NULL;
        }
    }

    return RFPARAM_SUSS;


}
#endif

struct wl_cfg_t * rfparam_cfg_get()
{
    return g_rfparam_cfg;
}

uint32_t rfparam_tlv_base_addr_get()
{
    return g_tlv_base_addr;
}

/****************************************************************************//**
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
#ifdef BL_PSM_RFPARAM 
    char value[PSM_RFPARAM_ENABLE_SIZE + 1] = {0};
    size_t get_size = 0;
#endif
    int32_t ret;
    uint32_t xtal_value;
#if defined(WL_API_RMEM_EN) && WL_API_RMEM_EN
#define WL_API_RMEM_ADDR  0x20010600
    g_rfparam_cfg = wl_cfg_get((uint8_t*)WL_API_RMEM_ADDR);
#else
    g_rfparam_cfg = wl_cfg_get();
#endif
    g_rfparam_cfg->capcode_get = rfparam_get_capcode;
    g_rfparam_cfg->capcode_set = rfparam_set_capcode;
    g_rfparam_cfg->en_full_cal = 1;
    g_rfparam_cfg->en_param_load = 1;
    g_rfparam_cfg->mode = WL_API_MODE_ALL;
    HBN_Get_Xtal_Value(&xtal_value);
    g_rfparam_cfg->param.xtalfreq_hz = xtal_value;
#ifdef BL_PSM_RFPARAM 
    get_size = ef_get_env_blob("psm_rfparam_enable", value, PSM_RFPARAM_ENABLE_SIZE, NULL);
    printf("psm_rfparam_enable get size %d string %s\r\n", get_size, value);
    if((get_size > 0) && strlen(value) == 1 && (value[0] == '1')){
        g_rfparam_cfg->param_load = rfparam_load_psm;
        puts("psm load from psm\r\n");
    }else{
        g_rfparam_cfg->param_load = rfparam_load;
    }
#else
    g_rfparam_cfg->param_load = rfparam_load;
#endif
    rfparam_printf("xtal value %d\r\n",(int)xtal_value);
    printf("rf param init\r\n");
    ret = wl_init();

    return ret;


}


