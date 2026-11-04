/* Copyright (c) 2026 Bouffalo Lab. SPDX-License-Identifier: Apache-2.0 */
#include "rtsp_server_jpeg.h"
#include <string.h>
#include "rtsp_server_huffman.h"

static unsigned read16(const uint8_t *p) { return (unsigned)p[0] * 256U + p[1]; }

int rtsp_server_jpeg_parse(const uint8_t *data, size_t size, rtsp_server_jpeg_t *j)
{
    if (!data || !j || size < 4 || data[0] != 255 || data[1] != 0xd8) return -1;
    memset(j, 0, sizeof(*j));
    size_t pos = 2;
    unsigned sof = 0, uv_table = 1;
    while (size - pos >= 4) {
        if (data[pos++] != 255) return -1;
        while (pos < size && data[pos] == 255) pos++;
        if (size - pos < 3) return -1;
        unsigned marker = data[pos++], length = read16(data + pos);
        if (length < 2 || length > size - pos) return -1;
        const uint8_t *p = data + pos + 2;
        size_t left = length - 2;
        if (marker == 0xdb) {
            while (left) {
                if (left < 65 || p[0] > 1) return -1;
                for (unsigned k = 1; k <= 64; k++) if (!p[k]) return -1;
                j->quant[p[0]] = p + 1;
                p += 65; left -= 65;
            }
        } else if (marker == 0xc0) {
            if (sof || left != 15 || p[0] != 8 || p[5] != 3 ||
                p[6] != 1 || (p[7] != 0x21 && p[7] != 0x22) || p[8] != 0 ||
                p[9] != 2 || p[10] != 0x11 || p[11] > 1 ||
                p[12] != 3 || p[13] != 0x11 || p[14] != p[11]) return -1;
            unsigned height = read16(p + 1), width = read16(p + 3);
            if (!width || !height || width > 2040 || height > 2040 ||
                (width & 7U) || (height & 7U)) return -1;
            j->width = (uint8_t)(width / 8); j->height = (uint8_t)(height / 8);
            j->type = p[7] == 0x22;
            uv_table = p[11]; sof = 1;
        } else if (marker == 0xc4) {
            /* RFC 2435 receivers reconstruct the default Huffman tables.
             * Reject optimized/custom tables instead of producing bad video.
             * An absent DHT is common with UVC and implies those defaults. */
            while (left) {
                if (left < 17) return -1;
                const uint8_t *expected;
                size_t expected_size;
                switch (p[0]) {
                case 0x00: expected = huffman_00; expected_size = sizeof(huffman_00); break;
                case 0x01: expected = huffman_01; expected_size = sizeof(huffman_01); break;
                case 0x10: expected = huffman_10; expected_size = sizeof(huffman_10); break;
                case 0x11: expected = huffman_11; expected_size = sizeof(huffman_11); break;
                default: return -1;
                }
                if (expected_size > left - 1 || memcmp(p + 1, expected, expected_size)) return -1;
                p += expected_size + 1; left -= expected_size + 1;
            }
        } else if (marker == 0xda) {
            if (!sof || left != 10 || p[0] != 3 || p[1] != 1 || p[2] != 0 ||
                p[3] != 2 || p[4] != 0x11 || p[5] != 3 || p[6] != 0x11 ||
                p[7] != 0 || p[8] != 63 || p[9] != 0 || !j->quant[0] ||
                !j->quant[uv_table]) return -1;
            j->quant[1] = j->quant[uv_table];
            j->scan = data + pos + length;
            /* Find a real EOI, allowing entropy stuffing but no restart/multiple
             * scans. The scan may contain FF00; never treat that as a marker. */
            for (size_t i = pos + length; i + 1 < size; i++) {
                if (data[i] != 255) continue;
                if (data[i + 1] == 0) { i++; continue; }
                if (data[i + 1] != 0xd9) return -1;
                j->scan_size = (size_t)(data + i - j->scan);
                return j->scan_size && j->scan_size <= 0xffffffU ? 0 : -1;
            }
            return -1;
        } else if (marker == 0xdd) {
            if (left != 2 || read16(p)) return -1;
        } else if (!((marker >= 0xe0 && marker <= 0xef) || marker == 0xfe)) {
            return -1;
        }
        pos += length;
    }
    return -1;
}

size_t rtsp_server_jpeg_payload(const rtsp_server_jpeg_t *j, size_t offset,
                               uint8_t *dst, size_t capacity, size_t *consumed)
{
    size_t header = offset ? 8U : 140U;
    if (!j || !dst || !consumed || !j->scan || !j->quant[0] || !j->quant[1] ||
        offset >= j->scan_size || offset > 0xffffffU || capacity <= header) return 0;
    dst[0] = 0; dst[1] = (uint8_t)(offset >> 16);
    dst[2] = (uint8_t)(offset >> 8); dst[3] = (uint8_t)offset;
    dst[4] = j->type; dst[5] = 255; dst[6] = j->width; dst[7] = j->height;
    if (!offset) {
        dst[8] = dst[9] = dst[10] = 0; dst[11] = 128;
        memcpy(dst + 12, j->quant[0], 64); memcpy(dst + 76, j->quant[1], 64);
    }
    size_t n = j->scan_size - offset;
    if (n > capacity - header) n = capacity - header;
    memcpy(dst + header, j->scan + offset, n);
    *consumed = n;
    return header + n;
}
