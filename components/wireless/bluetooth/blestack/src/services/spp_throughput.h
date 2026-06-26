/*
 * SPP Throughput Test Header
 *
 * Copyright (c) 2021-2022 Bouffalolab Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SPP_THROUGHPUT_H_
#define _SPP_THROUGHPUT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Start SPP TX throughput test.
 * idx < 0 starts every connected link; otherwise only the given link index.
 * len is the per-send payload size (default 672 when 0 or out of range).
 * rate_bps is the per-link target rate in bytes/sec; 0 = unlimited (full speed).
 * Returns the number of links started.
 */
int spp_tp_start(int idx, uint16_t len, uint32_t rate_bps);

/* Stop the SPP throughput test. idx < 0 stops all links. */
void spp_tp_stop(int idx);

#ifdef __cplusplus
}
#endif

#endif /* _SPP_THROUGHPUT_H_ */
