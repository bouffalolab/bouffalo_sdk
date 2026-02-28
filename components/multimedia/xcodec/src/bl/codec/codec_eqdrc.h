/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __CODEC_EQDRC_H__
#define __CODEC_EQDRC_H__

#include <stdint.h>


int auo_eq_set(uint32_t *eq_filt_coef, int bytes);
int auo_eq_show(void);
int auo_drc_set(uint32_t *drc_filt_coef, int bytes);
void auo_eq_set_example(void);

#endif /* __CODEC_EQDRC_H__ */
