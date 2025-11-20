/**
 * @file bflb_sdio_drv.h
 * @brief SDIO2 driver wrapper header file
 * @author mlwang
 * @date 2025-07-17
 */

#ifndef BFLB_SDIO_DRV_H
#define BFLB_SDIO_DRV_H

#include "bflb_msg_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize SDIO driver
 * @retval 0 Success
 * @retval -1 Error
 */

#ifdef USE_SDIO2_DRIVER
bflb_msg_ctrl_ops_t *sdio2_driver_init(void);
#elif defined(USE_SDIO3_DRIVER)
bflb_msg_ctrl_ops_t *sdio3_driver_init(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* bflb_SDIO_DRV_H */
