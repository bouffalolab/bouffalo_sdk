/*
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	zephyr/sys.c
 * @brief	Zephyr system primitives for libmetal.
 */

#include <metal/io.h>
#include <metal/sys.h>

#include <zephyr/kernel.h>

/**
 * @brief poll function until some event happens
 */
void metal_weak metal_generic_default_poll(void)
{
	k_cpu_idle();
}
