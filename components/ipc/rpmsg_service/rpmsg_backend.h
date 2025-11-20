/*
 * Copyright (c) 2021, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RPMSG_BACKEND_H
#define RPMSG_BACKEND_H

#include <openamp/rpmsg_virtio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Initialize RPMsg backend
 *
 * @param io   Shared memory IO region. This is an output parameter providing
 *             a pointer to an actual shared memory IO region structure.
 *             Caller of this function shall pass an address at which the
 *             pointer to the shared memory IO region structure is stored.
 * @param vdev Pointer to the virtio device initialized by this function.
 *
 * @retval 0 Initialization successful
 * @retval <0 Initialization error reported by OpenAMP
 */
int rpmsg_backend_init(struct metal_io_region **io, struct virtio_device *vdev);

#ifdef __cplusplus
}
#endif

#endif /* RPMSG_BACKEND_H */
