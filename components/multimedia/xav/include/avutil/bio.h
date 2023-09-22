/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __BIO_H__
#define __BIO_H__

#include "avutil/byte_rw.h"

__BEGIN_DECLS__

typedef struct buf_io {
    size_t                            pos;
    uint8_t                           *buf;
    size_t                            size;
} bio_t;

int bio_init(bio_t *bio, uint8_t *buf, size_t size);

int bio_reset(bio_t *bio);

int bio_read(bio_t *bio, uint8_t *buf, size_t count);

int bio_skip(bio_t *bio, int32_t offset);

int bio_tell(bio_t *bio);

uint8_t bio_r8(bio_t *bio);

uint16_t bio_r16be(bio_t *bio);

uint32_t bio_r24be(bio_t *bio);

uint32_t bio_r32be(bio_t *bio);

uint64_t bio_r64be(bio_t *bio);

uint16_t bio_r16le(bio_t *bio);

uint32_t bio_r24le(bio_t *bio);

uint32_t bio_r32le(bio_t *bio);

uint64_t bio_r64le(bio_t *bio);

__END_DECLS__

#endif /* __BIO_H__ */

