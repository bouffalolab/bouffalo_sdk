/* Copyright (c) 2026 Bouffalo Lab. SPDX-License-Identifier: Apache-2.0 */
#ifndef RTSP_SERVER_JPEG_H
#define RTSP_SERVER_JPEG_H
#include <stddef.h>
#include <stdint.h>
typedef struct {
    const uint8_t *scan;
    size_t scan_size;
    const uint8_t *quant[2];
    uint8_t type, width, height;
} rtsp_server_jpeg_t;
int rtsp_server_jpeg_parse(const uint8_t *data, size_t size, rtsp_server_jpeg_t *jpeg);
/* Build one RFC 2435 payload. Offset is into scan, not the JPEG file. The
 * first fragment carries both quantization tables. Never reads past scan.
 * Returns payload length, or zero on invalid arguments/capacity/offset.
 */
size_t rtsp_server_jpeg_payload(const rtsp_server_jpeg_t *jpeg, size_t offset,
                               uint8_t *dst, size_t capacity, size_t *consumed);
#endif
