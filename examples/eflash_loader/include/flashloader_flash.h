#ifndef FLASHLOADER_FLASH_H
#define FLASHLOADER_FLASH_H

#include <stdint.h>

typedef void (*flashloader_flash_progress_cb)(void);

int flashloader_flash_init(void);
int flashloader_flash_ready(void);
uint32_t flashloader_flash_size(void);
uint32_t flashloader_flash_jedec_id(void);
int flashloader_flash_erase(uint32_t start, uint32_t end_inclusive,
                            flashloader_flash_progress_cb progress);
int flashloader_flash_chip_erase(flashloader_flash_progress_cb progress);
int flashloader_flash_write(uint32_t address, const uint8_t *data,
                            uint32_t length);
int flashloader_flash_read(uint32_t address, uint8_t *data, uint32_t length);
int flashloader_flash_xip_start(void);
int flashloader_flash_xip_read(uint32_t address, uint8_t *data,
                               uint32_t length);
int flashloader_flash_xip_finish(void);
int flashloader_flash_set_parameter(uint32_t setting, const uint8_t *parameter,
                                    uint16_t parameter_length);
int flashloader_flash_read_status(uint8_t command, uint8_t *data,
                                  uint8_t length);
int flashloader_flash_write_status(uint8_t command, const uint8_t *data,
                                   uint8_t length);
int flashloader_flash_sha256(uint32_t address, uint32_t length,
                             uint8_t output[32]);
int flashloader_flash_xip_sha256(uint32_t address, uint32_t length,
                                 uint8_t output[32]);
int flashloader_flash_otp_set_parameter(const uint8_t *parameter,
                                        uint16_t parameter_length);
int flashloader_flash_otp_get_parameter(uint8_t *parameter,
                                        uint16_t parameter_capacity);
int flashloader_flash_otp_erase(uint32_t address, uint32_t length);
int flashloader_flash_otp_write(uint32_t address, const uint8_t *data,
                                uint32_t length);
int flashloader_flash_otp_read(uint32_t address, uint8_t *data,
                               uint32_t length);
int flashloader_flash_otp_lock(uint32_t start, uint32_t end_inclusive);
int flashloader_flash_otp_lock_by_index(uint32_t index);
int flashloader_flash_otp_erase_by_index(uint32_t index);
int flashloader_flash_otp_write_by_index(uint32_t index, const uint8_t *data,
                                         uint32_t length);
int flashloader_flash_otp_read_by_index(uint32_t index, uint8_t *data,
                                        uint32_t length);

#endif
