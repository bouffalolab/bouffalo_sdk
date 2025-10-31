#include "rfparam_adapter.h"
#include "wl_api.h"

#include "bl602_aon.h"
#include "bl602_hbn.h"

#define DBG_TAG "rfparam"
#include "log.h"

BFLB_LOG_DEFINE_TAG(RFPARAM, DBG_TAG, true);

#undef BFLB_LOG_TAG
#define BFLB_LOG_TAG           BFLB_LOG_GET_TAG(RFPARAM)
#define XTAL_ORDER_ADDR_LEN_MAX      (2)

#define rfparam_printf(...) LOG_I(__VA_ARGS__)

#define TCAL_PARA_CHANNELS          5

typedef struct{
    uint16_t Tchannels[TCAL_PARA_CHANNELS];
    int16_t Tchannel_os[TCAL_PARA_CHANNELS];
    int16_t Tchannel_os_low[TCAL_PARA_CHANNELS];
    int16_t Troom_os;
    uint8_t en_tcal;
    uint8_t linear_or_follow;
} tcal_param_struct;

enum {
    E_RF_TCAL_UPDATE_PARAM = 0,
};

void rf_pri_update_tcal_param(uint8_t operation);//FIXME

static void log_buf_int8(uint8_t *buf, size_t buf_len)
{
    int i;
    for (i = 0; i < buf_len; i++) {
        putchar(buf[i]);
    }
    puts("\r\n");
}

static void rfparam_array_printf(char *str, void *buf, uint32_t buf_len, int type)
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

static int bl_efuse_read_pwroft(int8_t poweroffset[14])
{
    //TODO: Tobe added
    return -1;
}

static int bl_efuse_read_capcode(uint8_t *capcode)
{
    //TODO: Tobe added
    return -1;
}

static int tcl_param_update(uint32_t rftlv_addr, tcal_param_struct *param)
{
    uint8_t tmp_buf[32] = { 0 };

    /*tcal*/
    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_EN_TCAL, RFTLV_MAXLEN_EN_TCAL, tmp_buf) > 0) {
        param->en_tcal = tmp_buf[0];
        rfparam_printf("tcal.en_tcal = %d\r\n", (int)param->en_tcal);
    } else {
        rfparam_printf("tcal.en_tcal null\r\n");
        return RFPARAM_ERR_TCAL_EN_NULL;
    }

    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_LINEAR_OR_FOLLOW, RFTLV_MAXLEN_LINEAR_OR_FOLLOW, tmp_buf) > 0) {
        param->linear_or_follow = tmp_buf[0];
        rfparam_printf("tcal.linear_or_follow = %d\r\n", (int)param->linear_or_follow);
    } else {
        rfparam_printf("tcal.linear_or_follow null\r\n");
        return RFPARAM_ERR_TCAL_LINEAR_OR_FOLLOW_NULL;
    }

    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_TCHANNELS, RFTLV_MAXLEN_TCHANNELS, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNELS / 2; i++) {
            param->Tchannels[i] = tmp_buf[i * 2] | (tmp_buf[i * 2 + 1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannels", (void *)param->Tchannels, sizeof(param->Tchannels) / 2, TYPE_UINT16);
    } else {
        rfparam_printf("tcal.Tchannels null\r\n");
        return RFPARAM_ERR_TCAL_CHANNELS_NULL;
    }

    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_TCHANNEL_OS, RFTLV_MAXLEN_TCHANNEL_OS, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS / 2; i++) {
            param->Tchannel_os[i] = tmp_buf[i * 2] | (tmp_buf[i * 2 + 1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannel_os", (void *)param->Tchannel_os, sizeof(param->Tchannel_os) / 2, TYPE_INT16);
    } else {
        rfparam_printf("tcal.Tchannel_os null\r\n");
        return RFPARAM_ERR_TCAL_CHANNEL_OS_NULL;
    }

    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_TCHANNEL_OS_LOW, RFTLV_MAXLEN_TCHANNEL_OS_LOW, tmp_buf) > 0) {
        for (int i = 0; i < RFTLV_MAXLEN_TCHANNEL_OS_LOW / 2; i++) {
            param->Tchannel_os_low[i] = tmp_buf[i * 2] | (tmp_buf[i * 2 + 1] << 8);
        }
        rfparam_array_printf((char *)"tcal.Tchannel_os_low", (void *)param->Tchannel_os_low, sizeof(param->Tchannel_os_low) / 2, TYPE_INT16);
    } else {
        rfparam_printf("tcal.Tchannel_os_low null\r\n");
        return RFPARAM_ERR_TCAL_CHANNEL_OS_LOW_NULL;
    }

    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_TROOM_OS, RFTLV_MAXLEN_TROOM_OS, tmp_buf) > 0) {
        param->Troom_os = (tmp_buf[0] | (tmp_buf[1] << 8)) - 256;
        rfparam_printf("tcal.Troom_os = %d\r\n", (int)param->Troom_os);
    } else {
        rfparam_printf("tcal.Troom_os null\r\n");
        return RFPARAM_ERR_TCAL_ROOM_OS_NULL;
    }
    return RFPARAM_SUSS;
}


static void update_poweroffset_config_rftv(uint32_t tlv_addr, const char *pw_mode)
{
    int i, set, len, j;
    int8_t poweroffset[14], poweroffset_tmp[14];

    memset(poweroffset, 0, sizeof(poweroffset));
    memset(poweroffset_tmp, 0, sizeof(poweroffset_tmp));
    set = 0;
    len = strlen(pw_mode);
    for (i = 0; i < XTAL_ORDER_ADDR_LEN_MAX && i < len; i++) {
        switch (pw_mode[i]) {
            case 'B':
            case 'b':
            {
                if (0 == bl_efuse_read_pwroft(poweroffset_tmp)) {
                    set = 1;
                    rfparam_printf("get pwr offset from B(b) ready\r\n");
                    log_buf_int8(poweroffset_tmp, sizeof(poweroffset_tmp));
                    if ('B' == pw_mode[i]) {
                        /*non-incremental mode*/
                        for (j = 0; j < sizeof(poweroffset); j++) {
                            poweroffset[j] = poweroffset_tmp[j];
                        }
                        rfparam_printf("Use pwr offset from B only\r\n");
                        goto break_scan;
                    } else {
                        /*incremental mode*/
                        rfparam_printf("Use pwr offset from b in incremental mode\r\n");
                        for (j = 0; j < sizeof(poweroffset); j++) {
                            poweroffset[j] += poweroffset_tmp[j];
                        }
                    }
                } else {
                    rfparam_printf("get pwr offset from B(b) failed\r\n");
                }
            }
            break;
            case 'F':
            case 'f':
            {
                if (rfparam_tlv_get(tlv_addr, RFTLV_TYPE_PWR_OFFSET, sizeof(poweroffset_tmp), poweroffset_tmp) > 0) {
                    set = 1;
                    rfparam_printf("get pwr offset from F(f) ready\r\n");
                    if ('F' == pw_mode[i]) {
                        /*non-incremental mode*/
                        for (j = 0; j < sizeof(poweroffset); j++) {
                            poweroffset[j] = (poweroffset_tmp[j] - 10)*4;
                        }
                        rfparam_printf("Use pwr offset from F only\r\n");
                        goto break_scan;
                    } else {
                        /*incremental mode*/
                        rfparam_printf("Use pwr offset from f in incremental mode\r\n");
                        for (j = 0; j < sizeof(poweroffset); j++) {
                            poweroffset[j] += (poweroffset_tmp[j] - 10)*4;
                        }
                    }
                    goto break_scan;
                } else {
                    rfparam_printf("get pwr offset from F(f) failed\r\n");
                }
            }
            break;
            default:
            {
                while(1);
            }
        }
    }
break_scan:
    if (0 == set) {
        rfparam_printf("Using Default pwr offset\r\n");//all zeros actually
    }
    log_buf_int8(poweroffset, sizeof(poweroffset));
#ifdef CFG_BLE_ENABLE
    extern void ble_rf_set_pwr_offset_table(int8_t *poweroffset_table);
	ble_rf_set_pwr_offset_table(poweroffset);
#endif
    phy_powroffset_set(poweroffset);
}

static int update_xtal_config_get_mac_from_factory(uint32_t capcode[5])
{
    uint8_t capcode_efuse = 0;

    if (bl_efuse_read_capcode(&capcode_efuse)) {
        return -1;
    }
    /*efuse only have one capcode entry, so we fill the left with hardcode*/
    capcode[0] = capcode_efuse;
    capcode[1] = capcode_efuse;
    capcode[2] = 1;
    capcode[3] = 60;
    capcode[4] = 60;

    return 0;
}

static void update_xtal_config_rftv(uint32_t tlv_addr)
{
    int i, set, len;
    uint8_t buffer[20] = {0};
    uint32_t capcode[5] = {0};
    char xtal_mode[3] = {0};

    if (rfparam_tlv_get(tlv_addr, RFTLV_TYPE_XTAL_MODE, 3, xtal_mode) > 0) {
        xtal_mode[sizeof(xtal_mode) - 1] = '\0';
        rfparam_printf("xtal_mode is %s\r\n", xtal_mode);
    }

    set = 0;
    len = strlen(xtal_mode);
    for (i = 0; i < XTAL_ORDER_ADDR_LEN_MAX && i < len; i++) {
        switch (xtal_mode[i]) {
            case 'F':
            {
                if (rfparam_tlv_get(tlv_addr, RFTLV_TYPE_XTAL, sizeof(buffer), buffer) > 0) {
                    capcode[0] = *(uint32_t *)buffer;
                    capcode[1] = *(uint32_t *)(buffer + 4);
                    capcode[2] = *(uint32_t *)(buffer + 8);
                    capcode[3] = *(uint32_t *)(buffer + 12);
                    capcode[4] = *(uint32_t *)(buffer + 16);
                    set = 1;
                    rfparam_printf("get xtal from F ready %d %d %d %d %d\r\n",
                            capcode[0],
                            capcode[1],
                            capcode[2],
                            capcode[3],
                            capcode[4]);
                    goto break_scan;
                } else {
                    rfparam_printf("get xtal from F failed\r\n");
                }
            }
            break;
            case 'M':
            {
                if (0 == update_xtal_config_get_mac_from_factory(capcode)) {
                    set = 1;
                    rfparam_printf("get xtal from M ready %d %d %d %d %d\r\n",
                            capcode[0],
                            capcode[1],
                            capcode[2],
                            capcode[3],
                            capcode[4]);
                    goto break_scan;
                } else {
                    rfparam_printf("get xtal from M failed\r\n");
                }
            }
            break;
            default:
            {
                while(1);
            }
        }
    }
break_scan:
    if (0 == set) {
        rfparam_printf("Using Default xtal\r\n");
        capcode[0] = 50;
        capcode[1] = 50;
        capcode[2] = 1;
        capcode[3] = 60;
        capcode[4] = 60;
    }
    AON_Set_Xtal_CapCode(capcode[0], capcode[1]);
}

static int hal_board_load_rftv_info(uint32_t rftlv_addr)
{
    int i;
    uint8_t *p_buffer;

    /* set tx_pwr_tbl */
    int8_t pwr_table[24];

    int pwr_table_ble = 0;

    if (!rfparam_tlv_valid(rftlv_addr)) {
        return -2;
    }

    p_buffer = pvPortMalloc(80);
    if (p_buffer == NULL) {
        return -1;
    }

    /* set xtal */
    update_xtal_config_rftv(rftlv_addr);

    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_PWR_TABLE_11B, 80, p_buffer) > 0) {
        for (i = 0; i < 4; i++) {
            pwr_table[i] = *(int8_t *)(p_buffer + i);
        }
        rfparam_printf("pwr_table_11b :%u %u %u %u\r\n",
            pwr_table[0],
            pwr_table[1],
            pwr_table[2],
            pwr_table[3]
        );
        trpc_update_power_11b((int8_t*)pwr_table);
    } else {
        LOG_E("RFTLV_TYPE_PWR_TABLE_11B NULL\r\n");
    }
    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_PWR_TABLE_11G, 80, p_buffer) > 0) {
        for (i = 0; i < 8; i++) {
            pwr_table[i] = *(int8_t *)(p_buffer + i);
        }
        rfparam_printf("pwr_table_11g :%u %u %u %u %u %u %u %u\r\n",
            pwr_table[0],
            pwr_table[1],
            pwr_table[2],
            pwr_table[3],
            pwr_table[4],
            pwr_table[5],
            pwr_table[6],
            pwr_table[7]
        );
        trpc_update_power_11g((int8_t*)pwr_table);
    } else {
        LOG_E("RFTLV_TYPE_PWR_TABLE_11G NULL\r\n");
    }
    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_PWR_TABLE_11N, 80, p_buffer) > 0) {
        for (i = 0; i < 8; i++) {
            pwr_table[i] = *(int8_t *)(p_buffer + i);
        }
        rfparam_printf("pwr_table_11n :%u %u %u %u %u %u %u %u\r\n",
            pwr_table[0],
            pwr_table[1],
            pwr_table[2],
            pwr_table[3],
            pwr_table[4],
            pwr_table[5],
            pwr_table[6],
            pwr_table[7]
        );
        trpc_update_power_11n((int8_t*)pwr_table);
    } else {
        LOG_E("RFTLV_TYPE_PWR_TABLE_11N NULL\r\n");
    }

    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_PWR_MODE, 80, p_buffer) > 0) {
        p_buffer[2] = '\0';
        update_poweroffset_config_rftv(rftlv_addr, (const char *)p_buffer);
    } else {
        LOG_E("RFTLV_TYPE_PWR_MODE NULL\r\n");
    }

    if (rfparam_tlv_get(rftlv_addr, RFTLV_TYPE_PWR_TABLE_BLE, 80, p_buffer) > 0) {
        pwr_table_ble = *(int8_t *)p_buffer;
        rfparam_printf("set pwr_table_ble = %ld in dts\r\n", pwr_table_ble);
    } else {
        LOG_E("RFTLV_TYPE_PWR_TABLE_BLE NULL\r\n");
    }
#ifdef CFG_BLE_ENABLE
        ble_controller_set_tx_pwr(pwr_table_ble);
#endif

    vPortFree(p_buffer);

    return 0;
}

extern uint32_t _ld_symbol_rftlv_address;
extern tcal_param_struct* tcal_param;

static int __try_load_rftlv(void)
{
    tcal_param_struct tcal_param_tmp;

    int ret = hal_board_load_rftv_info((uint32_t)&_ld_symbol_rftlv_address);

    ret |= tcl_param_update((uint32_t)&_ld_symbol_rftlv_address, &tcal_param_tmp);
    if (!ret) {
        memcpy(tcal_param, &tcal_param_tmp, sizeof(tcal_param_tmp));
        rf_pri_update_tcal_param(E_RF_TCAL_UPDATE_PARAM);
    }

    return ret;
}

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
    int ret = 0;
    ret = __try_load_rftlv();
    return ret;
}
