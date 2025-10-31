/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __TS_RW_H__
#define __TS_RW_H__

#include "avutil/sf.h"
#include "avutil/av_typedef.h"

__BEGIN_DECLS__

#define TS_PACKET_SIZE         (188)

#define PAT_TID                (0x0)
#define PMT_TID                (0x2)

#define NB_PID_MAX             (8192)
#define SECTION_SIZE_MAX       (4096)

#define PES_START_SIZE         (6)
#define PES_HEADER_SIZE        (9)

/* es type define */
#define ES_TYPE_UNKNOWN        (0x0)
#define ES_TYPE_MP3            (0x04)
#define ES_TYPE_AAC            (0x0f)

typedef struct {
    uint32_t                   es_type;
    avmedia_type_t             media_type;
    avcodec_id_t               codec_id;
} es_map_t;

struct ts_hdr {
    uint16_t                   sync_byte:8;
    uint16_t                   transport_error_indicator:1;
    uint16_t                   payload_unit_start_indicator:1;
    uint16_t                   transport_priority:1;
    uint16_t                   pid:13;
    uint16_t                   transport_scrambling_control:2;
    uint16_t                   adaption_field_control:2;
    uint16_t                   continuity_counter:4;
};

/**
 * @brief  get ts header info
 * @param  [in] buf
 * @param  [in] hdr
 * @return 0/-1
 */
int ts_hdr_get(const uint8_t* buf, struct ts_hdr *hdr);

/**
 * @brief  get codecid by es type
 * @param  [in] es_type
 * @return
 */
avcodec_id_t get_codecid_by_es(uint32_t es_type);

/**
 * @brief  get media type by es type
 * @param  [in] es_type
 * @return
 */
avmedia_type_t get_media_type_by_es(uint32_t es_type);

#endif /* __TS_RW_H__ */

