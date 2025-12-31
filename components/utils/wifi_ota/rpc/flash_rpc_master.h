/****************************************************************************
 * Flash RPC Master (AP Core)
 *
 * AP core (Master) receives Flash operation commands from NP core (Remote)
 * and executes them using bflb_flash APIs via XRAM shared memory.
 *
 ****************************************************************************/

#ifndef __FLASH_RPC_MASTER_H__
#define __FLASH_RPC_MASTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initialize Flash RPC Master on AP core
 *
 * @return 0 on success, negative error code on failure
 */
int flash_rpc_master_init(void);

/**
 * @brief Deinitialize Flash RPC Master
 *
 * @return 0 on success, negative error code on failure
 */
int flash_rpc_master_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_RPC_MASTER_H__ */
