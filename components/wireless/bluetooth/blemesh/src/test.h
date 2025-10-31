/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __TEST_H__
#define __TEST_H__

#if defined(CONFIG_BT_MESH_SELF_TEST)
int bt_mesh_test(void);
#else
static inline int bt_mesh_test(void)
{
	return 0;
}
#endif

#endif /*__TEST_H__*/
