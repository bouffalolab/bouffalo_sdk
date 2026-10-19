#ifndef FLASHLOADER_PORT_H
#define FLASHLOADER_PORT_H

#include <stdint.h>

#include "flashloader_config.h"

void flashloader_port_init(void);
int flashloader_port_flash_init(void);
int flashloader_port_flash_configure(uint32_t setting);
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_RESET) != 0U
void flashloader_port_reset(void) __attribute__((noreturn));
#endif
int flashloader_port_efuse_read(uint32_t address, uint8_t *data,
                               uint32_t length);
int flashloader_port_efuse_write(uint32_t address, const uint8_t *data,
                                uint32_t length);
int flashloader_port_read_mac(uint8_t mac[6]);
int flashloader_port_write_mac(const uint8_t mac[6]);
uint32_t flashloader_port_crc32(const void *data, uint32_t length);
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_HOST_ACK) != 0U
uint64_t flashloader_port_time_ms(void);
#endif

#endif
