/*
 * Copyright (c) 2024 Bouffalolab.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SOLUTION_H__
#define __SOLUTION_H__

#include <stdint.h>

typedef struct {
	uint32_t dbi_disp;
	uint32_t rgb_disp;
	uint32_t dvp;
	uint32_t mjpeg_enc;
	uint32_t mjpeg_dec;
	uint32_t dvp_mjpeg_enc;
	uint32_t uvc;
	uint32_t avi_sd;
	uint32_t hb_tx;
	uint32_t hb_rx;
	uint32_t sample_elapsed_ms;
} solution_fps_stats_t;

void soln_init(void);

int soln_fps_stats_get(solution_fps_stats_t *stats);
int soln_fps_str_get(char *str_buff_total, uint32_t buff_size);

#endif
