/*
 * Copyright (c) 2024, STMicroelectronics.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	zephyr/cpu.c
 * @brief	hosted libmetal cpu routines.
 */

#include <metal/sys.h>

void metal_cpu_yield(void)
{
	metal_wait_usec(10);
}
