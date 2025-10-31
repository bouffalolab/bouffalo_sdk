#include "bflb_mjdec_v2.h"
#include "bflb_core.h"

char *format_string[] = {
    [MJDEC_FORMAT_GRAY]          = "MJDEC_FORMAT_GRAY",
    [MJDEC_FORMAT_YUYV]          = "MJDEC_FORMAT_YUYV",
    [MJDEC_FORMAT_YVYU]          = "MJDEC_FORMAT_YVYU",
    [MJDEC_FORMAT_UYVY]          = "MJDEC_FORMAT_UYVY",
    [MJDEC_FORMAT_VYUY]          = "MJDEC_FORMAT_VYUY",
    [MJDEC_FORMAT_NV16_OR_NV12]  = "MJDEC_FORMAT_NV16_OR_NV12",
    [MJDEC_FORMAT_NV61_OR_NV21]  = "MJDEC_FORMAT_NV61_OR_NV21",
    [MJDEC_FORMAT_RGB888]        = "MJDEC_FORMAT_RGB888",
    [MJDEC_FORMAT_RGB565]        = "MJDEC_FORMAT_RGB565",
    [MJDEC_FORMAT_NRGB8888]      = "MJDEC_FORMAT_NRGB8888",
    [MJDEC_FORMAT_BGR888]        = "MJDEC_FORMAT_BGR888",
    [MJDEC_FORMAT_BGR565]        = "MJDEC_FORMAT_BGR565",
    [MJDEC_FORMAT_NBGR8888]      = "MJDEC_FORMAT_NBGR8888",
};

void format_convert_yvyu_to_yuyv(uint8_t *p, uint32_t length)
{
    uint8_t temp;

    if (length % 4) {
        printf("YVYU to YUYV length must be a multiple of 4.\r\n");
        return;
    }
    for (uint32_t i = 0; i < length; i += 4) {
        temp = p[i + 1];
        p[i + 1] = p[i + 3];
        p[i + 3] = temp;
    }
}

void format_convert_uyvy_to_yuyv(uint8_t *p, uint32_t length)
{
    uint8_t temp;

    if (length % 4) {
        printf("UYVY to YUYV length must be a multiple of 4.\r\n");
        return;
    }
    for (uint32_t i = 0; i < length; i += 2) {
        temp = p[i];
        p[i] = p[i + 1];
        p[i + 1] = temp;
    }
}

void format_convert_vyuy_to_yuyv(uint8_t *p, uint32_t length)
{
    uint8_t temp;

    if (length % 4) {
        printf("VYUY to YUYV length must be a multiple of 4.\r\n");
        return;
    }
    for (uint32_t i = 0; i < length; i += 4) {
        temp = p[i];
        p[i] = p[i + 1];
        p[i + 1] = p[i + 2];
        p[i + 2] = p[i + 3];
        p[i + 3] = temp;
    }
}

void format_convert_rgb565_to_rgb16(uint8_t *p, uint32_t length)
{
    return;
}

void format_convert_bgr565_to_rgb16(uint8_t *p, uint32_t length)
{
    uint16_t r, g, b;
    uint16_t temp;

    if (length % 2) {
        printf("BGR565 to RGB16 length must be a multiple of 2.\r\n");
        return;
    }
    for (uint32_t i = 0; i < length / 2; i++) {
        temp = ((uint16_t *)p)[i];
        r = (temp >> 11) & 0x1F;
        g = (temp & 0x07E0);
        b = (temp & 0x1F) << 11;
        ((uint16_t *)p)[i] = (r | g | b);
    }
}

void format_convert_rgb888_to_rgb24(uint8_t *p, uint32_t length)
{
    return;
}

void format_convert_bgr888_to_rgb24(uint8_t *p, uint32_t length)
{
    uint8_t temp;

    if (length % 3) {
        printf("BGR888 to RGB24 length must be a multiple of 3.\r\n");
        return;
    }
    for (uint32_t i = 0; i < length; i += 3) {
        temp = p[i];
        p[i] = p[i + 2];
        p[i + 2] = temp;
    }
}

void format_convert_nrgb8888_to_rgb32(uint8_t *p, uint32_t length)
{
    return;
}

void format_convert_nbgr8888_to_rgb32(uint8_t *p, uint32_t length)
{
    uint8_t temp;

    if (length % 4) {
        printf("NRGB8888 to RGB32 length must be a multiple of 4.\r\n");
        return;
    }
    for (uint32_t i = 0; i < length; i += 4) {
        temp = p[i];
        p[i] = p[i + 2];
        p[i + 2] = temp;
    }
}

void format_print_data(uint8_t *data, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++) {
        if (i % 16 == 0) {
            printf("\r\n%02X", data[i]);
        } else {
            printf(" %02X", data[i]);
        }
    }
}

void format_convert(struct bflb_mjdec_config_s *config, struct bflb_mjdec_parse_s *parse)
{
    uint8_t *p;
    uint32_t length = 0;
    uint32_t x, y;

    p = (uint8_t *)(config->output_bufaddr0);
    x = parse->resolution_x;
    y = parse->resolution_y;
    switch (config->format) {
        case MJDEC_FORMAT_GRAY:
            length = x * y;
            break;
        case MJDEC_FORMAT_YUYV:
            length = x * y * 2;
            break;
        case MJDEC_FORMAT_YVYU:
            length = x * y * 2;
            format_convert_yvyu_to_yuyv(p, length);
            break;
        case MJDEC_FORMAT_UYVY:
            length = x * y * 2;
            format_convert_uyvy_to_yuyv(p, length);
            break;
        case MJDEC_FORMAT_VYUY:
            length = x * y * 2;
            format_convert_vyuy_to_yuyv(p, length);
            break;
        case MJDEC_FORMAT_NV16_OR_NV12:
            length = x * y * 2;
            break;
        case MJDEC_FORMAT_NV61_OR_NV21:
            length = x * y * 2;
            break;
        case MJDEC_FORMAT_RGB565:
            length = x * y * 2;
            format_convert_rgb565_to_rgb16(p, length);
            break;
        case MJDEC_FORMAT_BGR565:
            length = x * y * 2;
            format_convert_bgr565_to_rgb16(p, length);
            break;
        case MJDEC_FORMAT_RGB888:
            length = x * y * 3;
            format_convert_rgb888_to_rgb24(p, length);
            break;
        case MJDEC_FORMAT_BGR888:
            length = x * y * 3;
            format_convert_bgr888_to_rgb24(p, length);
            break;
        case MJDEC_FORMAT_NRGB8888:
            length = x * y * 4;
            format_convert_nrgb8888_to_rgb32(p, length);
            break;
        case MJDEC_FORMAT_NBGR8888:
            length = x * y * 4;
            format_convert_nbgr8888_to_rgb32(p, length);
            break;
        default:
            break;
    }
    printf("format: %s\r\n", format_string[config->format]);
    printf("addr: 0x%08lX\r\n", p);
    printf("length: %u\r\n", length);
    format_print_data(p, length);
}
