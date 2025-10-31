/** @file
 *  @brief Bluetooth Mesh Proxy APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MESH_PROXY_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_MESH_PROXY_H_

/**
 * @brief Bluetooth Mesh Proxy
 * @defgroup bt_mesh_proxy Bluetooth Mesh Proxy
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Enable advertising with Node Identity.
 *
 *  This API requires that GATT Proxy support has been enabled. Once called
 *  each subnet will start advertising using Node Identity for the next
 *  60 seconds.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_proxy_identity_enable(void);

#if defined(CONFIG_BT_MESH_V1d1)
/** @brief Enable advertising with Private Node Identity.
 *
 *  This API requires that GATT Proxy support has been enabled. Once called
 *  each subnet will start advertising using Private Node Identity for the next
 *  60 seconds.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_proxy_private_identity_enable(void);
#endif /* CONFIG_BT_MESH_V1d1 */

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MESH_PROXY_H_ */
