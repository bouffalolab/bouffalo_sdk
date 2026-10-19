/**
 * @file mr_sdio_drv.h
 * @brief SDIO driver wrapper header file
 */

#ifndef MR_SDIO_DRV_H
#define MR_SDIO_DRV_H

#include "mr_msg_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern mr_msg_ctrl_ops_t msg_ctrl_sdio_ops;

/**
 * @brief Prepare SDIO driver for low power entry
 * @retval 0 Success
 * @retval -1 Driver is not initialized
 */
int mr_sdio_drv_lowpower_prepare(void);

/**
 * @brief Restore SDIO driver after low power exit
 * @retval 0 Success
 * @retval -1 Driver is not initialized
 */
int mr_sdio_drv_lowpower_restore(void);

#ifdef __cplusplus
}
#endif

#endif /* MR_SDIO_DRV_H */
