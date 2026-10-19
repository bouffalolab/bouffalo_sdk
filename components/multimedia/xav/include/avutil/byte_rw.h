/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __BYTE_RW_H__
#define __BYTE_RW_H__

#include "avutil/common.h"

__BEGIN_DECLS__

uint16_t byte_r16be(const uint8_t *buf);

uint32_t byte_r24be(const uint8_t *buf);

uint32_t byte_r32be(const uint8_t *buf);

uint64_t byte_r64be(const uint8_t *buf);

uint16_t byte_r16le(const uint8_t *buf);

uint32_t byte_r24le(const uint8_t *buf);

uint32_t byte_r32le(const uint8_t *buf);

uint64_t byte_r64le(const uint8_t *buf);

__END_DECLS__

#endif /* __BYTE_RW_H__ */

