#include "utils_tlv_bl.h"

int utils_tlv_bl_pack_bool(uint32_t *buf, int buf_sz, bool val)
{
    if (buf_sz < CFG_ELEMENT_TYPE_SIZE_BOOLEAN) {
        return UTILS_TLV_BL_ERROR_CODE_BUF_TOO_SMALL;
    }
    *buf = val;//XXX caution for unaligned access

    return CFG_ELEMENT_TYPE_SIZE_BOOLEAN;
}

int utils_tlv_bl_pack_uint32(uint32_t *buf, int buf_sz, uint32_t val)
{
    if (buf_sz < CFG_ELEMENT_TYPE_SIZE_UINT32) {
        return UTILS_TLV_BL_ERROR_CODE_BUF_TOO_SMALL;
    }
    *buf = val;//XXX caution for unaligned access

    return CFG_ELEMENT_TYPE_SIZE_UINT32;
}

int utils_tlv_bl_unpack_bool(uint32_t *buf, int buf_sz, bool *val)
{
    if (buf_sz < CFG_ELEMENT_TYPE_SIZE_BOOLEAN) {
        return UTILS_TLV_BL_ERROR_CODE_BUF_TOO_SMALL;
    }
    *val = (*buf) ? true : false;// XXX caution for unaligned access

    return CFG_ELEMENT_TYPE_SIZE_BOOLEAN;
}

int utils_tlv_bl_unpack_uint32(uint32_t *buf, int buf_sz, uint32_t *val)
{
    if (buf_sz < CFG_ELEMENT_TYPE_SIZE_UINT32) {
        return UTILS_TLV_BL_ERROR_CODE_BUF_TOO_SMALL;
    }
    *val = (*buf);//XXX caution for unaligned access

    return CFG_ELEMENT_TYPE_SIZE_BOOLEAN;
}

int utils_tlv_bl_pack_auto(uint32_t *buf, int buf_sz, uint16_t type, void *arg1)
{
    int ret = UTILS_TLV_BL_ERROR_CODE_UNKOWN;

    switch (type) {
        case CFG_ELEMENT_TYPE_BOOLEAN:
        {
            ret = utils_tlv_bl_pack_bool(buf, buf_sz, *(bool*)arg1 ? true : false);
        }
        break;
        case CFG_ELEMENT_TYPE_SINT8:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_UINT8:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_SINT16:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_UINT16:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_SINT32:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_UINT32:
        {
            ret = utils_tlv_bl_pack_uint32(buf, buf_sz, *(uint32_t*)arg1);
        }
        break;
        case CFG_ELEMENT_TYPE_STRING:
        {
            //TODO
        }
        break;
        default:
        {
            /*empty*/
        }
    }

    return ret;
}

int utils_tlv_bl_unpack_auto(uint32_t *buf, int buf_sz, uint16_t type, void *arg1)
{
    int ret = UTILS_TLV_BL_ERROR_CODE_UNKOWN;

    switch (type) {
        case CFG_ELEMENT_TYPE_BOOLEAN:
        {
            bool val = true;

            ret = utils_tlv_bl_unpack_bool(buf, buf_sz, &val);
            *(bool*)arg1 = val;
        }
        break;
        case CFG_ELEMENT_TYPE_SINT8:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_UINT8:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_SINT16:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_UINT16:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_SINT32:
        {
            //TODO
        }
        break;
        case CFG_ELEMENT_TYPE_UINT32:
        {
            uint32_t val = 0;

            ret = utils_tlv_bl_unpack_uint32(buf, buf_sz, &val);
            *(uint32_t*)arg1 = val;
        }
        break;
        case CFG_ELEMENT_TYPE_STRING:
        {
            //TODO
        }
        break;
        default:
        {
            /*empty*/
        }
    }

    return ret;
}
