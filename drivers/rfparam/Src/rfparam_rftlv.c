
#include "rfparam_rftlv.h"

/*
 * return : 1-valid, other-invalid
 */
int rfparam_tlv_valid(uint32_t tlv_addr)
{
    if ((RFTLF_TYPE_MAGIC_FW_HEAD1 == (*((uint64_t *)tlv_addr))) && \
        (RFTLF_TYPE_MAGIC_FW_HEAD2 == (*((uint64_t *)(tlv_addr + RFTLV_SIZE_HEAD1))))) {
        return 1;
    }
    return 0;
}

/*
 * input:
 *        type: type
 *        value_len: value max len
 *        value: point value
 * return : ">0"-success, "<0"-invalid and end, "==0"-invalid and can next
 */
int rfparam_tlv_get(uint32_t tlv_addr, uint16_t type, uint32_t value_len, void *value)
{
    uint16_t type_tmp;
    uint16_t length_tmp;
    uint32_t addr_tmp;
    if(1 != rfparam_tlv_valid(tlv_addr)){
        return -1;
    }

    memset(value,0,value_len);
    addr_tmp = tlv_addr + RFTLV_SIZE_HEAD;

    if ((RFTLV_TYPE_INVALID == type) || (!value)) {
        rftlv_print("rftlv_get arg error, type = 0x%x, value = %p\r\n", type, value);
        return -1;
    }
    while (1) {
        if ((addr_tmp - tlv_addr) >= (RFTLV_SIZE_PARTION - RFTLV_SIZE_TL)) {
            rftlv_print("rftlv_get overflow !!!\r\n");
            return -1;
        }
        RFTLV_TYPE((void *)&type_tmp, (void *)addr_tmp, RFTLV_SIZE_TYPE);
        RFTLV_LENGTH((void *)&length_tmp, (void *)(addr_tmp + RFTLV_SIZE_TYPE), RFTLV_SIZE_LENGTH);
        if (type_tmp == RFTLV_TYPE_INVALID) {
            return -1;
        } else if (type_tmp != type) {
            addr_tmp += (RFTLV_SIZE_TL + length_tmp);
            continue;
        }
        if (length_tmp > value_len) {
            return 0;
        }

        //XXX potential access out of aera
        RFTLV_VALUE((void *)value, (void *)(addr_tmp + RFTLV_SIZE_TL), length_tmp);
        return 1;
    }
    return 0;
}

