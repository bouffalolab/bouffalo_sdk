#include "bflb_mjdec_v2.h"
#include "bflb_core.h"

void format_convert_yvyu_to_yuyv(uint8_t *p, uint32_t length);
void format_convert_uyvy_to_yuyv(uint8_t *p, uint32_t length);
void format_convert_vyuy_to_yuyv(uint8_t *p, uint32_t length);
void format_convert_rgb565_to_rgb16(uint8_t *p, uint32_t length);
void format_convert_bgr565_to_rgb16(uint8_t *p, uint32_t length);
void format_convert_rgb888_to_rgb24(uint8_t *p, uint32_t length);
void format_convert_bgr888_to_rgb24(uint8_t *p, uint32_t length);
void format_convert_nrgb8888_to_rgb32(uint8_t *p, uint32_t length);
void format_convert_nbgr8888_to_rgb32(uint8_t *p, uint32_t length);
void format_print_data(uint8_t *data, uint32_t length);
void format_convert(struct bflb_mjdec_config_s *config, struct bflb_mjdec_parse_s *parse);
