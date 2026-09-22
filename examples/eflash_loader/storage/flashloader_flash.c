#include "flashloader_flash.h"

#include <stddef.h>
#include <string.h>

#include "flashloader_config.h"
#include "bflb_flash.h"
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_OTP) != 0U
#include "bflb_flash_secreg.h"
#include "bflb_flash_secreg_port.h"
#endif
#include "bflb_name.h"
#include "bflb_sec_sha.h"
#include "bflb_sf_cfg.h"
#include "bflb_sflash.h"
#include "bflb_xip_sflash.h"
#include "flashloader_port.h"

static uint8_t g_flash_ready;
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XIP) != 0U
static uint8_t g_xip_active;
#endif
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_OTP) != 0U
static uint8_t g_flash_otp_parameter_custom;
static bflb_flash_secreg_param_t g_flash_otp_parameter;
#endif
static struct bflb_device_s *g_sha;
static struct bflb_sha256_ctx_s g_sha_ctx
    __attribute__((section(".nocache_noinit_ram"), aligned(32)));
static uint8_t g_sha_buffer[FLASHLOADER_SHA_CHUNK_SIZE]
    __attribute__((section(".nocache_noinit_ram"), aligned(32)));

static int range_valid(uint32_t address, uint32_t length)
{
    uint32_t size = bflb_flash_get_size();

    return (length <= size) && (address <= (size - length));
}

static int get_flash_cfg(spi_flash_cfg_type **cfg)
{
    uint8_t *raw_cfg = NULL;
    uint32_t length = 0U;

    if ((cfg == NULL) || (g_flash_ready == 0U)) {
        return -1;
    }
    bflb_flash_get_cfg(&raw_cfg, &length);
    if ((raw_cfg == NULL) || (length != sizeof(spi_flash_cfg_type))) {
        return -1;
    }
    *cfg = (spi_flash_cfg_type *)raw_cfg;
    return 0;
}

int flashloader_flash_init(void)
{
    if (g_flash_ready != 0U) {
        return 0;
    }

    if ((flashloader_port_flash_init() != 0) || (bflb_flash_init() != 0)) {
        return -1;
    }

    g_flash_ready = 1U;
    return 0;
}

int flashloader_flash_ready(void)
{
    return g_flash_ready != 0U;
}

uint32_t flashloader_flash_size(void)
{
    return g_flash_ready != 0U ? bflb_flash_get_size() : 0U;
}

uint32_t flashloader_flash_jedec_id(void)
{
    return g_flash_ready != 0U ? bflb_flash_get_jedec_id() : 0U;
}

int flashloader_flash_erase(uint32_t start, uint32_t end_inclusive,
                            flashloader_flash_progress_cb progress)
{
    uint32_t size;
    uint32_t address;
    uint32_t erase_length;

    size = bflb_flash_get_size();
    if ((g_flash_ready == 0U) || (size == 0U) || (start > end_inclusive) ||
        (end_inclusive >= size)) {
        return -1;
    }

    address = start & ~(FLASHLOADER_FLASH_SECTOR_SIZE - 1U);
    erase_length = end_inclusive - address + 1U;
    if (progress != NULL) {
        progress();
    }
    return bflb_flash_erase(address, erase_length);
}

int flashloader_flash_chip_erase(flashloader_flash_progress_cb progress)
{
    uint32_t size;

    if (g_flash_ready == 0U) {
        return -1;
    }
    size = bflb_flash_get_size();
    if (size == 0U) {
        return -1;
    }
    return flashloader_flash_erase(0U, size - 1U, progress);
}

int flashloader_flash_write(uint32_t address, const uint8_t *data,
                            uint32_t length)
{
    if ((g_flash_ready == 0U) || (data == NULL) || (length == 0U) ||
        !range_valid(address, length)) {
        return -1;
    }
    return bflb_flash_write(address, (uint8_t *)(uintptr_t)data, length);
}

int flashloader_flash_read(uint32_t address, uint8_t *data, uint32_t length)
{
    if ((g_flash_ready == 0U) || (data == NULL) ||
        !range_valid(address, length)) {
        return -1;
    }
    if (length == 0U) {
        return 0;
    }
    return bflb_flash_read(address, data, length);
}

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XIP) != 0U
int flashloader_flash_xip_start(void)
{
    if (g_flash_ready == 0U) {
        return -1;
    }
    if (g_xip_active != 0U) {
        return 0;
    }
    if (bflb_flash_set_cache(1U, 1U, 0x0FU, 0U) != 0) {
        return -1;
    }
    g_xip_active = 1U;
    return 0;
}

int flashloader_flash_xip_read(uint32_t address, uint8_t *data,
                               uint32_t length)
{
    if ((g_xip_active == 0U) || (data == NULL) ||
        !range_valid(address, length)) {
        return -1;
    }
    if (length == 0U) {
        return 0;
    }
    return bflb_xip_sflash_read_via_cache_need_lock(
        BFLB_FLASH_XIP_BASE + address, data, length, 0, 0);
}

int flashloader_flash_xip_finish(void)
{
    spi_flash_cfg_type *cfg;

    if (g_xip_active == 0U) {
        return 0;
    }
    if (get_flash_cfg(&cfg) != 0) {
        return -1;
    }
    bflb_sf_ctrl_set_owner(SF_CTRL_OWNER_SAHB);
    bflb_sflash_xip_read_disable();
    bflb_sflash_reset_continue_read(cfg);
    bflb_sflash_disable_burst_wrap(cfg);
    g_xip_active = 0U;
    return 0;
}
#endif

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_PARAMETER) != 0U
int flashloader_flash_set_parameter(uint32_t setting, const uint8_t *parameter,
                                    uint16_t parameter_length)
{
    spi_flash_cfg_type *cfg;
    uint8_t io_mode = (uint8_t)(setting >> 16);

    if ((parameter_length != 0U) &&
        ((parameter == NULL) ||
         (parameter_length != sizeof(spi_flash_cfg_type)))) {
        return -1;
    }
    if ((flashloader_port_flash_configure(setting) != 0) ||
        (bflb_flash_init() != 0)) {
        g_flash_ready = 0U;
        return -1;
    }
    g_flash_ready = 1U;
    if (get_flash_cfg(&cfg) != 0) {
        g_flash_ready = 0U;
        return -1;
    }
    if (parameter_length != 0U) {
        memcpy(cfg, parameter, sizeof(*cfg));
    }
    bflb_flash_set_iomode(io_mode);
    return 0;
}
#endif

int flashloader_flash_read_status(uint8_t command, uint8_t *data,
                                  uint8_t length)
{
    spi_flash_cfg_type *cfg;
    uint32_t offset = 0U;
    int result;

    if ((data == NULL) || (length == 0U) || (get_flash_cfg(&cfg) != 0) ||
        (bflb_xip_sflash_state_save(cfg, &offset, 0, 0) != 0)) {
        return -1;
    }
    result = bflb_sflash_read_reg_with_cmd(cfg, command, data, length);
    if (bflb_xip_sflash_state_restore(cfg, offset, 0, 0) != 0) {
        result = -1;
    }
    return result;
}

int flashloader_flash_write_status(uint8_t command, const uint8_t *data,
                                   uint8_t length)
{
    spi_flash_cfg_type *cfg;
    uint32_t offset = 0U;
    int result;

    if ((data == NULL) || (length == 0U) || (get_flash_cfg(&cfg) != 0) ||
        (bflb_xip_sflash_state_save(cfg, &offset, 0, 0) != 0)) {
        return -1;
    }
    if (bflb_sflash_write_enable(cfg) != 0) {
        result = -1;
    } else {
        result = bflb_sflash_write_reg_with_cmd(
            cfg, command, (uint8_t *)(uintptr_t)data, length);
    }
    if (bflb_xip_sflash_state_restore(cfg, offset, 0, 0) != 0) {
        result = -1;
    }
    return result;
}

static int flash_sha256(uint32_t address, uint32_t length, uint8_t output[32],
                        uint8_t xip)
{
    uint32_t chunk;
    int read_result;

    if ((output == NULL) || !range_valid(address, length)) {
        return -1;
    }
    if (g_sha == NULL) {
        g_sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);
        if ((g_sha == NULL) || (bflb_group0_request_sha_access(g_sha) != 0)) {
            g_sha = NULL;
            return -1;
        }
    }

    bflb_sha_init(g_sha, SHA_MODE_SHA256);
    bflb_sha256_start(g_sha, &g_sha_ctx);
    while (length != 0U) {
        chunk = length > sizeof(g_sha_buffer) ? sizeof(g_sha_buffer) : length;
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XIP) != 0U
        read_result = xip != 0U
                          ? flashloader_flash_xip_read(address, g_sha_buffer,
                                                       chunk)
                          : flashloader_flash_read(address, g_sha_buffer, chunk);
#else
        (void)xip;
        read_result = flashloader_flash_read(address, g_sha_buffer, chunk);
#endif
        if ((read_result != 0) ||
            (bflb_sha256_update(g_sha, &g_sha_ctx, g_sha_buffer, chunk) != 0)) {
            return -1;
        }
        address += chunk;
        length -= chunk;
    }
    bflb_sha256_finish(g_sha, &g_sha_ctx, output);
    return 0;
}

int flashloader_flash_sha256(uint32_t address, uint32_t length,
                             uint8_t output[32])
{
    return flash_sha256(address, length, output, 0U);
}

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XIP) != 0U
int flashloader_flash_xip_sha256(uint32_t address, uint32_t length,
                                 uint8_t output[32])
{
    return flash_sha256(address, length, output, 1U);
}
#endif

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_OTP) != 0U
static int flash_otp_parameter_valid(
    const bflb_flash_secreg_param_t *parameter)
{
    uint32_t lock_bit_count;
    uint32_t lock_bit_offset;
    uint32_t status_length;

    if ((parameter == NULL) || (parameter->region_count == 0U) ||
        (parameter->region_count > 8U) ||
        (parameter->region_size == 0U) || (parameter->secreg_size == 0U) ||
        (parameter->secreg_size > parameter->region_size) ||
        (parameter->api_type > BFLB_FLASH_SECREG_API_TYPE_MXIC) ||
        (parameter->lb_share > 1U) || (parameter->lb_read_loop == 0U) ||
        (parameter->lb_read_loop > 3U) ||
        (parameter->lb_read_len == 0U) ||
        (parameter->lb_write_len == 0U)) {
        return 0;
    }

    status_length = (uint32_t)parameter->lb_read_len *
                    (uint32_t)parameter->lb_read_loop;
    if ((status_length > sizeof(uint32_t)) ||
        ((uint32_t)parameter->lb_write_len > status_length)) {
        return 0;
    }

    lock_bit_count = parameter->lb_share != 0U ? 1U : parameter->region_count;
    lock_bit_offset = parameter->lb_offset;
    if (status_length == 1U) {
        lock_bit_offset %= 8U;
    }
    if ((lock_bit_offset >= status_length * 8U) ||
        (lock_bit_count > status_length * 8U - lock_bit_offset)) {
        return 0;
    }
    return 1;
}

static int flash_otp_get_config(bflb_flash_otp_config_t *config)
{
    spi_flash_cfg_type *flash_cfg;
    const bflb_flash_secreg_param_t *parameter;

    if ((config == NULL) || (get_flash_cfg(&flash_cfg) != 0)) {
        return -1;
    }
    if (g_flash_otp_parameter_custom != 0U) {
        parameter = &g_flash_otp_parameter;
    } else if (bflb_flash_secreg_get_param(bflb_flash_get_jedec_id(),
                                            &parameter) != 0) {
        return -1;
    }
    if (!flash_otp_parameter_valid(parameter)) {
        return -1;
    }

    config->flash_cfg = flash_cfg;
    config->param = parameter;
    return 0;
}

static int flash_otp_range_valid(const bflb_flash_secreg_param_t *parameter,
                                 uint32_t address, uint32_t length)
{
    uint32_t size = (uint32_t)parameter->region_count *
                    (uint32_t)parameter->secreg_size * 256U;

    return (length <= size) && (address <= size - length);
}

int flashloader_flash_otp_set_parameter(const uint8_t *parameter,
                                        uint16_t parameter_length)
{
    bflb_flash_secreg_param_t candidate;

    if ((parameter == NULL) ||
        (parameter_length != sizeof(g_flash_otp_parameter))) {
        return -1;
    }
    memcpy(candidate.raw, parameter, sizeof(candidate.raw));
    if (!flash_otp_parameter_valid(&candidate)) {
        return -1;
    }
    g_flash_otp_parameter = candidate;
    g_flash_otp_parameter_custom = 1U;
    return 0;
}

int flashloader_flash_otp_get_parameter(uint8_t *parameter,
                                        uint16_t parameter_capacity)
{
    bflb_flash_otp_config_t config;

    if ((parameter == NULL) ||
        (parameter_capacity < sizeof(g_flash_otp_parameter)) ||
        (flash_otp_get_config(&config) != 0)) {
        return -1;
    }
    memcpy(parameter, config.param->raw, sizeof(g_flash_otp_parameter.raw));
    return 0;
}

int flashloader_flash_otp_erase(uint32_t address, uint32_t length)
{
    bflb_flash_otp_config_t config;

    if ((length == 0U) || (flash_otp_get_config(&config) != 0) ||
        !flash_otp_range_valid(config.param, address, length)) {
        return -1;
    }
    return bflb_flash_secreg_erase(&config, address, length);
}

int flashloader_flash_otp_write(uint32_t address, const uint8_t *data,
                                uint32_t length)
{
    bflb_flash_otp_config_t config;

    if ((data == NULL) || (length == 0U) ||
        (flash_otp_get_config(&config) != 0) ||
        !flash_otp_range_valid(config.param, address, length)) {
        return -1;
    }
    return bflb_flash_secreg_write(&config, address, data, length);
}

int flashloader_flash_otp_read(uint32_t address, uint8_t *data,
                               uint32_t length)
{
    bflb_flash_otp_config_t config;

    if ((data == NULL) || (flash_otp_get_config(&config) != 0) ||
        !flash_otp_range_valid(config.param, address, length)) {
        return -1;
    }
    if (length == 0U) {
        return 0;
    }
    return bflb_flash_secreg_read(&config, address, data, length);
}

int flashloader_flash_otp_lock(uint32_t start, uint32_t end_inclusive)
{
    bflb_flash_otp_config_t config;
    uint32_t region_size;
    uint32_t size;
    uint32_t first;
    uint32_t last;

    if ((flash_otp_get_config(&config) != 0) || (start > end_inclusive)) {
        return -1;
    }
    region_size = (uint32_t)config.param->secreg_size * 256U;
    size = (uint32_t)config.param->region_count * region_size;
    if (end_inclusive >= size) {
        return -1;
    }
    first = start / region_size;
    last = end_inclusive / region_size;
    if (config.param->lb_share != 0U) {
        return bflb_flash_secreg_set_locked(&config, (uint8_t)first);
    }
    for (uint32_t index = first; index <= last; index++) {
        if (bflb_flash_secreg_set_locked(&config, (uint8_t)index) != 0) {
            return -1;
        }
    }
    return 0;
}

int flashloader_flash_otp_erase_by_index(uint32_t index)
{
    bflb_flash_otp_config_t config;

    if ((index > UINT8_MAX) || (flash_otp_get_config(&config) != 0)) {
        return -1;
    }
    return bflb_flash_secreg_erase_by_idx(&config, (uint8_t)index);
}

int flashloader_flash_otp_lock_by_index(uint32_t index)
{
    bflb_flash_otp_config_t config;

    if ((index > UINT8_MAX) || (flash_otp_get_config(&config) != 0)) {
        return -1;
    }
    return bflb_flash_secreg_set_locked(&config, (uint8_t)index);
}

int flashloader_flash_otp_write_by_index(uint32_t index, const uint8_t *data,
                                         uint32_t length)
{
    bflb_flash_otp_config_t config;

    if ((index > UINT8_MAX) || (data == NULL) || (length == 0U) ||
        (flash_otp_get_config(&config) != 0)) {
        return -1;
    }
    return bflb_flash_secreg_write_by_idx(&config, (uint8_t)index, 0U, data,
                                           length);
}

int flashloader_flash_otp_read_by_index(uint32_t index, uint8_t *data,
                                        uint32_t length)
{
    bflb_flash_otp_config_t config;

    if ((index > UINT8_MAX) || (data == NULL) ||
        (flash_otp_get_config(&config) != 0)) {
        return -1;
    }
    return bflb_flash_secreg_read_by_idx(&config, (uint8_t)index, 0U, data,
                                          length);
}
#endif
