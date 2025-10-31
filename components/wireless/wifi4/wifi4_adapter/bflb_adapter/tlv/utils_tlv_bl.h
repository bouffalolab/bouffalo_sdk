#ifndef __UTILS_TLV_BL_H__
#define __UTILS_TLV_BL_H__
#include <stdint.h>
#include <stdbool.h>

#define CFG_ELEMENT_TYPE_SIZE_BOOLEAN                 (4)
#define CFG_ELEMENT_TYPE_SIZE_UINT32                  (4)
#define UTILS_TLV_BL_ERROR_CODE_BUF_TOO_SMALL         (-1)
#define UTILS_TLV_BL_ERROR_CODE_UNKOWN                (-2)
// Wi-Fi CFG API
enum CFG_ELEMENT_TYPE {
    CFG_ELEMENT_TYPE_UNKNOWN,
    CFG_ELEMENT_TYPE_BOOLEAN,
    CFG_ELEMENT_TYPE_SINT8,
    CFG_ELEMENT_TYPE_UINT8,
    CFG_ELEMENT_TYPE_SINT16,
    CFG_ELEMENT_TYPE_UINT16,
    CFG_ELEMENT_TYPE_SINT32,
    CFG_ELEMENT_TYPE_UINT32,
    CFG_ELEMENT_TYPE_STRING,
};

enum CFG_ELEMENT_TYPE_OPS
{
    CFG_ELEMENT_TYPE_OPS_SET,
    CFG_ELEMENT_TYPE_OPS_GET,
    CFG_ELEMENT_TYPE_OPS_RESET,
    CFG_ELEMENT_TYPE_OPS_DUMP_DEBUG,
    CFG_ELEMENT_TYPE_OPS_UNKNOWN,
};

int utils_tlv_bl_pack_bool(uint32_t *buf, int buf_sz, bool val);
int utils_tlv_bl_pack_uint8(uint32_t *buf, int buf_sz, uint8_t val);
int utils_tlv_bl_pack_int8(uint32_t *buf, int buf_sz, int8_t val);
int utils_tlv_bl_pack_uint16(uint32_t *buf, int buf_sz, uint16_t val);
int utils_tlv_bl_pack_int16(uint32_t *buf, int buf_sz, int16_t val);
int utils_tlv_bl_pack_uint32(uint32_t *buf, int buf_sz, uint32_t val);
int utils_tlv_bl_pack_int32(uint32_t *buf, int buf_sz, int32_t val);
int utils_tlv_bl_pack_string(uint32_t *buf, int buf_sz, const char *str);
int utils_tlv_bl_pack_auto(uint32_t *buf, int buf_sz, uint16_t type, void *arg1);

int utils_tlv_bl_unpack_bool(uint32_t *buf, int buf_sz, bool *val);
int utils_tlv_bl_unpack_uint8(uint32_t *buf, int buf_sz, uint8_t *val);
int utils_tlv_bl_unpack_int8(uint32_t *buf, int buf_sz, int8_t *val);
int utils_tlv_bl_unpack_uint16(uint32_t *buf, int buf_sz, uint16_t *val);
int utils_tlv_bl_unpack_int16(uint32_t *buf, int buf_sz, int16_t *val);
int utils_tlv_bl_unpack_uint32(uint32_t *buf, int buf_sz, uint32_t *val);
int utils_tlv_bl_unpack_int32(uint32_t *buf, int buf_sz, int32_t *val);
int utils_tlv_bl_unpack_string(uint32_t *buf, int buf_sz, char *str, int size);
int utils_tlv_bl_unpack_auto(uint32_t *buf, int buf_sz, uint16_t type, void *arg1);
#endif
