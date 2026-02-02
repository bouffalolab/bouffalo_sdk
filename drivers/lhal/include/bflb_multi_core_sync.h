#ifndef _BFLB_MULTI_CORE_SYNC_H
#define _BFLB_MULTI_CORE_SYNC_H

#ifdef CONFIG_IPC
/**
 * @brief Erase flash with NP suspend/resume management
 * @param erase_addr Flash erase address
 * @param len Erase length in bytes
 * @return 0 on success, negative on failure
 */
int bflb_flash_erase_mcs(uint32_t erase_addr, uint32_t len);

/**
 * @brief Write flash with NP suspend/resume management
 * @param write_addr Flash write address
 * @param data Pointer to data buffer
 * @param len Write length in bytes
 * @return 0 on success, negative on failure
 */
int bflb_flash_write_mcs(uint32_t write_addr, const uint8_t *data, uint32_t len);

/**
 * @brief Read flash with NP suspend/resume management
 * @param addr Flash read address
 * @param data Pointer to data buffer
 * @param len Read length in bytes
 * @return 0 on success, negative on failure
 */
int bflb_flash_read_mcs(uint32_t addr, uint8_t *data, uint32_t len);

/**
 * @brief System reboot with NP suspend management
 * @note Suspends NP before reboot to ensure safe shutdown
 */
void bflb_sys_reboot_mcs(void);
#endif

#endif