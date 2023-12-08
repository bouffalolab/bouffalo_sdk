/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ASF_RW_H__
#define __ASF_RW_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"

__BEGIN_DECLS__

#define ASF_GUID_SIZE           (16)
#define IS_ASF_GUID_EQUAL(a, b) (memcmp((const void *)(a), (const void *)(b), ASF_GUID_SIZE) == 0)

typedef enum {
    ASF_GUID_UNKNOWN,

    ASF_GUID_HEADER,
    ASF_GUID_DATA,
    ASF_GUID_FILE_PROPERTIES,
    ASF_GUID_STREAM_PROPERTIES,
    ASF_GUID_CONTENT_DESCRIPTION,
    ASF_GUID_HEADER_EXTENSION,
    ASF_GUID_CODEC_LIST,
    ASF_GUID_STREAM_BITRATE_PROPERTIES,
    ASF_GUID_PADDING,
    ASF_GUID_EXTENDED_CONTENT_DESCRIPTION,
    ASF_GUID_METADATA,
    ASF_GUID_STREAM_PRIORITIZATION,
    ASF_GUID_STREAM_TYPE_AUDIO,
} asf_guid_t;

/**
* @brief  get asf guid type
* @param  [in] guid
* @return ASF_GUID_UNKNOWN on err
*/
asf_guid_t asf_guid_get_type(const uint8_t guid[ASF_GUID_SIZE]);



__END_DECLS__

#endif /* __ASF_RW_H__ */

