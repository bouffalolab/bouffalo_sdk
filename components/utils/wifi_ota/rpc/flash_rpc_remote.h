/****************************************************************************
 * Flash RPC Remote (NP Core)
 *
 * NP core (Remote) sends Flash operation commands to AP core (Master)
 * and receives results via XRAM shared memory ringbuffer.
 *
 ****************************************************************************/

#ifndef __FLASH_RPC_REMOTE_H__
#define __FLASH_RPC_REMOTE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Initialize Flash RPC Remote on NP core
 *
 * @return 0 on success, negative error code on failure
 */
int flash_rpc_remote_init(void);

/**
 * @brief Deinitialize Flash RPC Remote
 *
 * @return 0 on success, negative error code on failure
 */
int flash_rpc_remote_deinit(void);

/**
 * @brief Erase Flash via RPC
 *
 * @param addr Flash address to erase
 * @param len Length to erase in bytes
 * @return 0 on success, negative error code on failure
 */
int flash_rpc_erase(uint32_t addr, uint32_t len);

/**
 * @brief Write Flash via RPC
 *
 * @param addr Flash address to write
 * @param data Data buffer to write
 * @param len Length to write in bytes
 * @return 0 on success, negative error code on failure
 */
int flash_rpc_write(uint32_t addr, const uint8_t *data, uint32_t len);

/**
 * @brief Read Flash via RPC
 *
 * @param addr Flash address to read
 * @param data Data buffer to store read data
 * @param len Length to read in bytes
 * @return 0 on success, negative error code on failure
 */
int flash_rpc_read(uint32_t addr, uint8_t *data, uint32_t len);

/**
 * @brief Request system reboot via RPC
 *
 * NP core sends a reboot command to AP core, which will execute
 * the actual system reset (including WiFi/BT module reset).
 *
 * @return 0 on success, negative error code on failure
 */
int flash_rpc_reboot(void);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_RPC_REMOTE_H__ */
