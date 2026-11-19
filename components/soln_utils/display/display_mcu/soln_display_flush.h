#ifndef SOLN_DISPLAY_FLUSH_H
#define SOLN_DISPLAY_FLUSH_H

#include "display.h"
#include "soln_fbq.h"

#define SOLN_DISPLAY_BAND_ROWS 16u

/* One task exclusively owns the display and both staging buffers. */
int soln_disp_dbi_init(void);
const display_info_t *soln_disp_dbi_info(void);
/* RGB888, packed YUV444 and YUYV only, tightly packed, always left/top aligned.
 * Oversize input is rejected except fixed 640x480 YUYV -> 480x320 sampling.
 * Returns only after all submitted pixels are complete; the source stays read-only. */
int soln_disp_flush_frame(const fbq_elem_t *frame);
int soln_disp_draw_text(uint16_t x, uint16_t y, uint32_t fg, uint32_t bg, const char *text);

#endif