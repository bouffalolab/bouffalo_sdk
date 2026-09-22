#include "flashloader_commands.h"

#include <stddef.h>

#include "flashloader_config.h"
#include "flashloader_flash.h"
#include "flashloader_port.h"
#include "flashloader_protocol.h"
#include "flashloader_transport.h"
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XZ) != 0U
#include "xz.h"
#endif

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XZ) != 0U
#define XZ_ADDRESS_START_FLAG 0x80000000UL
#define XZ_ADDRESS_MASK       0x7FFFFFFFUL

struct flashloader_xz_state {
    struct xz_dec *decoder;
    struct xz_buf buffer;
    uint32_t base_address;
    uint32_t compressed_length;
    uint32_t write_address;
    uint8_t active;
};

static struct flashloader_xz_state g_xz;
static uint8_t g_xz_heap[FLASHLOADER_XZ_HEAP_SIZE]
    __attribute__((aligned(32)));
static uint8_t g_xz_output[FLASHLOADER_XZ_OUTPUT_SIZE]
    __attribute__((aligned(32)));
#endif

static uint16_t g_write_error;

static uint32_t get_le32(const uint8_t *data)
{
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_EFUSE) != 0U
static void put_le32(uint8_t *data, uint32_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8);
    data[2] = (uint8_t)(value >> 16);
    data[3] = (uint8_t)(value >> 24);
}
#endif

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_RESET) != 0U
static int bytes_are_zero(const uint8_t *data, uint16_t length)
{
    for (uint16_t index = 0U; index < length; index++) {
        if (data[index] != 0U) {
            return 0;
        }
    }
    return 1;
}
#endif

static struct flashloader_command_result command_result(uint16_t status)
{
    struct flashloader_command_result result = {
        .status = status,
        .data_length = 0U,
        .has_data = 0U,
        .reset = 0U,
    };

    return result;
}

static struct flashloader_command_result data_result(uint16_t length)
{
    struct flashloader_command_result result = command_result(
        FLASHLOADER_SUCCESS);

    result.data_length = length;
    result.has_data = 1U;
    return result;
}

static int flash_is_ready(void)
{
    return flashloader_flash_ready() || (flashloader_flash_init() == 0);
}

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XZ) != 0U
static void xz_stop(void)
{
    if (g_xz.decoder != NULL) {
        xz_dec_end(g_xz.decoder);
    }
    g_xz.decoder = NULL;
    g_xz.active = 0U;
    g_xz.buffer.out_pos = 0U;
}

static int xz_start(uint32_t address)
{
    xz_stop();
    if (address >= flashloader_flash_size()) {
        return -1;
    }
    simple_malloc_init(g_xz_heap, sizeof(g_xz_heap));
    xz_crc32_init();
    g_xz.decoder = xz_dec_init(XZ_PREALLOC, 1U << 15);
    if (g_xz.decoder == NULL) {
        return -1;
    }

    g_xz.base_address = address;
    g_xz.compressed_length = 0U;
    g_xz.write_address = address;
    g_xz.buffer.out = g_xz_output;
    g_xz.buffer.out_pos = 0U;
    g_xz.buffer.out_size = sizeof(g_xz_output);
    g_xz.active = 1U;
    return 0;
}

static int xz_flush_output(void)
{
    if (g_xz.buffer.out_pos == 0U) {
        return 0;
    }
    if (flashloader_flash_write(g_xz.write_address, g_xz_output,
                                g_xz.buffer.out_pos) != 0) {
        return -1;
    }
    g_xz.write_address += (uint32_t)g_xz.buffer.out_pos;
    g_xz.buffer.out_pos = 0U;
    return 0;
}

static uint16_t xz_write(uint32_t packet_address, const uint8_t *data,
                         uint32_t length)
{
    enum xz_ret xz_result;
    uint8_t output_flushed;
    size_t old_input;
    size_t old_output;
    uint32_t address = packet_address & XZ_ADDRESS_MASK;

    if ((packet_address & XZ_ADDRESS_START_FLAG) != 0U) {
        g_write_error = FLASHLOADER_SUCCESS;
        if (xz_start(address) != 0) {
            g_write_error = FLASHLOADER_FLASH_DECOMPRESS_ERROR;
            return g_write_error;
        }
    } else if ((g_xz.active == 0U) ||
               (g_xz.compressed_length > UINT32_MAX - g_xz.base_address) ||
               (address != g_xz.base_address + g_xz.compressed_length)) {
        g_write_error = FLASHLOADER_CMD_SEQ_ERROR;
        return g_write_error;
    }

    if (length > UINT32_MAX - g_xz.compressed_length) {
        g_write_error = FLASHLOADER_CMD_SEQ_ERROR;
        xz_stop();
        return g_write_error;
    }

    g_xz.buffer.in = data;
    g_xz.buffer.in_pos = 0U;
    g_xz.buffer.in_size = length;
    for (;;) {
        old_input = g_xz.buffer.in_pos;
        old_output = g_xz.buffer.out_pos;
        xz_result = xz_dec_run(g_xz.decoder, &g_xz.buffer);

        output_flushed = 0U;
        if (g_xz.buffer.out_pos == g_xz.buffer.out_size) {
            if (xz_flush_output() != 0) {
                g_write_error = FLASHLOADER_FLASH_WRITE_XZ_ERROR;
                xz_stop();
                return g_write_error;
            }
            output_flushed = 1U;
        }

        if (xz_result == XZ_STREAM_END) {
            if ((g_xz.buffer.in_pos != g_xz.buffer.in_size) ||
                (xz_flush_output() != 0)) {
                g_write_error = FLASHLOADER_FLASH_WRITE_XZ_ERROR;
                xz_stop();
                return g_write_error;
            }
            g_xz.compressed_length += length;
            xz_stop();
            return FLASHLOADER_SUCCESS;
        }
        if (xz_result != XZ_OK) {
            g_write_error = FLASHLOADER_FLASH_DECOMPRESS_ERROR;
            xz_stop();
            return g_write_error;
        }
        /* The decoder can consume all input while retaining more output. */
        if (output_flushed != 0U) {
            continue;
        }
        if (g_xz.buffer.in_pos == g_xz.buffer.in_size) {
            g_xz.compressed_length += length;
            return FLASHLOADER_SUCCESS;
        }
        if ((g_xz.buffer.in_pos == old_input) &&
            (g_xz.buffer.out_pos == old_output)) {
            g_write_error = FLASHLOADER_FLASH_DECOMPRESS_ERROR;
            xz_stop();
            return g_write_error;
        }
    }
}
#else
static void xz_stop(void)
{
}
#endif

void flashloader_commands_init(void)
{
    g_write_error = FLASHLOADER_SUCCESS;
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XZ) != 0U
    g_xz.decoder = NULL;
    g_xz.active = 0U;
#endif
}

struct flashloader_command_result flashloader_command_execute(
    uint8_t command, const uint8_t *payload, uint16_t payload_length,
    uint8_t *response_data, uint16_t response_capacity)
{
    struct flashloader_command_result result = command_result(
        FLASHLOADER_SUCCESS);
    uint32_t address;
    uint32_t length;
    uint32_t value;

    switch (command) {
        case FLASHLOADER_CMD_CHANGE_RATE:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            }
            break;

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_RESET) != 0U
        case FLASHLOADER_CMD_RESET:
            if (payload_length != 0U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else {
                result.reset = 1U;
            }
            break;
#endif

        case FLASHLOADER_CMD_CLOCK_SET:
        case FLASHLOADER_CMD_OPTION_FINISH:
            break;

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XIP) != 0U
        case FLASHLOADER_CMD_XIP_READ_START:
            if ((payload_length != 0U) || !flash_is_ready() ||
                (flashloader_flash_xip_start() != 0)) {
                result.status = FLASHLOADER_FAIL;
            }
            break;

        case FLASHLOADER_CMD_XIP_READ_FINISH:
            if ((payload_length != 0U) ||
                (flashloader_flash_xip_finish() != 0)) {
                result.status = FLASHLOADER_FAIL;
            }
            break;
#endif

        case FLASHLOADER_CMD_FLASH_ERASE:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_FLASH_ERASE_PARA_ERROR;
                break;
            }
            g_write_error = FLASHLOADER_SUCCESS;
            xz_stop();
            if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_erase(
                           get_le32(payload), get_le32(payload + 4U),
                           flashloader_transport_send_pending) != 0) {
                result.status = FLASHLOADER_FLASH_ERASE_ERROR;
            }
            break;

        case FLASHLOADER_CMD_FLASH_WRITE:
            if (payload_length <= 4U) {
                result.status = FLASHLOADER_FLASH_WRITE_PARA_ERROR;
                break;
            }
            xz_stop();
            address = get_le32(payload);
            length = payload_length - 4U;
            if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_write(address, payload + 4U,
                                               length) != 0) {
                result.status = FLASHLOADER_FLASH_WRITE_ERROR;
            }
            if (result.status != FLASHLOADER_SUCCESS) {
                g_write_error = result.status;
            }
            break;

        case FLASHLOADER_CMD_FLASH_READ:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_FLASH_WRITE_PARA_ERROR;
                break;
            }
            address = get_le32(payload);
            length = get_le32(payload + 4U);
            if ((length > response_capacity) || (length > UINT16_MAX)) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_read(address, response_data,
                                              length) != 0) {
                result.status = FLASHLOADER_FAIL;
            } else {
                result = data_result((uint16_t)length);
            }
            break;

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XIP) != 0U
        case FLASHLOADER_CMD_FLASH_XIP_READ:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_FLASH_WRITE_PARA_ERROR;
                break;
            }
            address = get_le32(payload);
            length = get_le32(payload + 4U);
            if ((length > response_capacity) || (length > UINT16_MAX)) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_xip_read(address, response_data,
                                                  length) != 0) {
                result.status = FLASHLOADER_FAIL;
            } else {
                result = data_result((uint16_t)length);
            }
            break;
#endif

        case FLASHLOADER_CMD_FLASH_SWITCH_BANK:
            if ((payload_length != 4U) || (get_le32(payload) != 0U)) {
                result.status = FLASHLOADER_FLASH_SWITCH_BANK_ERROR;
            }
            break;

        case FLASHLOADER_CMD_FLASH_READ_JID:
            if (payload_length != 0U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (response_capacity < 4U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else {
                value = (flashloader_flash_jedec_id() & 0x00FFFFFFU) |
                        0x80000000U;
                response_data[0] = (uint8_t)value;
                response_data[1] = (uint8_t)(value >> 8);
                response_data[2] = (uint8_t)(value >> 16);
                response_data[3] = (uint8_t)(value >> 24);
                result = data_result(4U);
            }
            break;

        case FLASHLOADER_CMD_FLASH_READ_STATUS:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_FLASH_READ_STATUS_ERROR;
                break;
            }
            length = get_le32(payload + 4U);
            if ((length == 0U) || (length > UINT8_MAX) ||
                (length > response_capacity) || !flash_is_ready() ||
                (flashloader_flash_read_status(
                     (uint8_t)get_le32(payload), response_data,
                     (uint8_t)length) != 0)) {
                result.status = FLASHLOADER_FLASH_READ_STATUS_ERROR;
            } else {
                result = data_result((uint16_t)length);
            }
            break;

        case FLASHLOADER_CMD_FLASH_WRITE_STATUS:
            if (payload_length < 9U) {
                result.status = FLASHLOADER_FLASH_WRITE_STATUS_ERROR;
                break;
            }
            length = get_le32(payload + 4U);
            if ((length == 0U) || (length > UINT8_MAX) ||
                (length > (uint32_t)payload_length - 8U) ||
                !flash_is_ready() ||
                (flashloader_flash_write_status(
                     (uint8_t)get_le32(payload), payload + 8U,
                     (uint8_t)length) != 0)) {
                result.status = FLASHLOADER_FLASH_WRITE_STATUS_ERROR;
            }
            break;

        case FLASHLOADER_CMD_FLASH_WRITE_CHECK:
            if (payload_length != 0U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else {
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XZ) != 0U
                if ((g_write_error == FLASHLOADER_SUCCESS) &&
                    (g_xz.active != 0U)) {
                    g_write_error = FLASHLOADER_FLASH_DECOMPRESS_ERROR;
                    xz_stop();
                }
#endif
                result.status = g_write_error;
            }
            break;

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_PARAMETER) != 0U
        case FLASHLOADER_CMD_FLASH_SET_PARA:
            if (payload_length < 4U) {
                result.status = FLASHLOADER_FLASH_SET_PARA_ERROR;
            } else if (flashloader_flash_set_parameter(
                           get_le32(payload), payload + 4U,
                           payload_length - 4U) != 0) {
                result.status = FLASHLOADER_FLASH_SET_PARA_ERROR;
            }
            break;
#endif

        case FLASHLOADER_CMD_FLASH_CHIP_ERASE:
            if (payload_length != 0U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_chip_erase(
                           flashloader_transport_send_pending) != 0) {
                result.status = FLASHLOADER_FLASH_ERASE_ERROR;
            } else {
                g_write_error = FLASHLOADER_SUCCESS;
                xz_stop();
            }
            break;

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_EFUSE) != 0U
        case FLASHLOADER_CMD_EFUSE_WRITE:
            if ((payload_length <= 4U) ||
                (((uint32_t)payload_length - 4U) & 3U) != 0U) {
                result.status = FLASHLOADER_EFUSE_WRITE_PARA_ERROR;
                break;
            }
            address = get_le32(payload);
            length = (uint32_t)payload_length - 4U;
            if ((address & 3U) != 0U) {
                result.status = FLASHLOADER_EFUSE_WRITE_ADDR_ERROR;
            } else if (flashloader_port_efuse_write(address, payload + 4U,
                                                    length) != 0) {
                result.status = FLASHLOADER_EFUSE_WRITE_ERROR;
            }
            break;

        case FLASHLOADER_CMD_EFUSE_READ:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_EFUSE_READ_PARA_ERROR;
                break;
            }
            address = get_le32(payload);
            length = get_le32(payload + 4U);
            if (((address | length) & 3U) != 0U) {
                result.status = FLASHLOADER_EFUSE_READ_ADDR_ERROR;
            } else if ((length > response_capacity) ||
                       (length > UINT16_MAX)) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (flashloader_port_efuse_read(address, response_data,
                                                   length) != 0) {
                result.status = FLASHLOADER_EFUSE_READ_ERROR;
            } else {
                result = data_result((uint16_t)length);
            }
            break;

        case FLASHLOADER_CMD_EFUSE_READ_MAC:
            if ((payload_length != 0U) || (response_capacity < 10U) ||
                (flashloader_port_read_mac(response_data) != 0)) {
                result.status = FLASHLOADER_EFUSE_READ_MAC_ERROR;
            } else {
                value = flashloader_port_crc32(response_data, 6U);
                put_le32(response_data + 6U, value);
                result = data_result(10U);
            }
            break;

        case FLASHLOADER_CMD_EFUSE_WRITE_MAC:
            if (payload_length != 6U) {
                result.status = FLASHLOADER_EFUSE_WRITE_PARA_ERROR;
            } else if (flashloader_port_write_mac(payload) != 0) {
                result.status = FLASHLOADER_EFUSE_WRITE_ERROR;
            }
            break;
#endif

        case FLASHLOADER_CMD_FLASH_READ_SHA:
            if ((payload_length != 8U) || (response_capacity < 32U)) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_sha256(
                           get_le32(payload), get_le32(payload + 4U),
                           response_data) != 0) {
                result.status = FLASHLOADER_FAIL;
            } else {
                result = data_result(32U);
            }
            break;

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XIP) != 0U
        case FLASHLOADER_CMD_FLASH_XIP_READ_SHA:
            if ((payload_length != 8U) || (response_capacity < 32U)) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_xip_sha256(
                           get_le32(payload), get_le32(payload + 4U),
                           response_data) != 0) {
                result.status = FLASHLOADER_FAIL;
            } else {
                result = data_result(32U);
            }
            break;
#endif

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XZ) != 0U
        case FLASHLOADER_CMD_FLASH_XZ_WRITE:
            if (payload_length <= 4U) {
                result.status = FLASHLOADER_FLASH_WRITE_PARA_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else {
                result.status = xz_write(get_le32(payload), payload + 4U,
                                         payload_length - 4U);
            }
            break;
#endif

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_OTP) != 0U
        case FLASHLOADER_CMD_FLASH_OTP_ERASE:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_otp_erase(
                           get_le32(payload), get_le32(payload + 4U)) != 0) {
                result.status = FLASHLOADER_FAIL;
            }
            break;

        case FLASHLOADER_CMD_FLASH_OTP_WRITE:
            if (payload_length <= 4U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_otp_write(
                           get_le32(payload), payload + 4U,
                           (uint32_t)payload_length - 4U) != 0) {
                result.status = FLASHLOADER_FAIL;
            }
            break;

        case FLASHLOADER_CMD_FLASH_OTP_READ:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
                break;
            }
            length = get_le32(payload + 4U);
            if ((length > response_capacity) || (length > UINT16_MAX)) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_otp_read(
                           get_le32(payload), response_data, length) != 0) {
                result.status = FLASHLOADER_FAIL;
            } else {
                result = data_result((uint16_t)length);
            }
            break;

        case FLASHLOADER_CMD_FLASH_OTP_SET_PARA:
            if (flashloader_flash_otp_set_parameter(payload,
                                                    payload_length) != 0) {
                result.status = FLASHLOADER_FLASH_SET_PARA_ERROR;
            }
            break;

        case FLASHLOADER_CMD_FLASH_OTP_GET_PARA:
            if ((payload_length != 0U) || (response_capacity < 12U)) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_otp_get_parameter(
                           response_data, response_capacity) != 0) {
                result.status = FLASHLOADER_FAIL;
            } else {
                result = data_result(12U);
            }
            break;

        case FLASHLOADER_CMD_FLASH_OTP_LOCK_INDEX:
            if (payload_length != 4U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_otp_lock_by_index(
                           get_le32(payload)) != 0) {
                result.status = FLASHLOADER_FAIL;
            }
            break;

        case FLASHLOADER_CMD_FLASH_OTP_LOCK_ADDR:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_otp_lock(
                           get_le32(payload), get_le32(payload + 4U)) != 0) {
                result.status = FLASHLOADER_FAIL;
            }
            break;

        case FLASHLOADER_CMD_FLASH_OTP_ERASE_INDEX:
            if (payload_length != 4U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_otp_erase_by_index(
                           get_le32(payload)) != 0) {
                result.status = FLASHLOADER_FAIL;
            }
            break;

        case FLASHLOADER_CMD_FLASH_OTP_WRITE_INDEX:
            if (payload_length <= 4U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_otp_write_by_index(
                           get_le32(payload), payload + 4U,
                           (uint32_t)payload_length - 4U) != 0) {
                result.status = FLASHLOADER_FAIL;
            }
            break;

        case FLASHLOADER_CMD_FLASH_OTP_READ_INDEX:
            if (payload_length != 8U) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
                break;
            }
            length = get_le32(payload + 4U);
            if ((length > response_capacity) || (length > UINT16_MAX)) {
                result.status = FLASHLOADER_CMD_LEN_ERROR;
            } else if (!flash_is_ready()) {
                result.status = FLASHLOADER_FLASH_INIT_ERROR;
            } else if (flashloader_flash_otp_read_by_index(
                           get_le32(payload), response_data, length) != 0) {
                result.status = FLASHLOADER_FAIL;
            } else {
                result = data_result((uint16_t)length);
            }
            break;
#endif

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_RESET) != 0U
        case FLASHLOADER_CMD_FLASH_BOOT:
            if (((payload_length != 0U) && (payload_length != 4U)) ||
                !bytes_are_zero(payload, payload_length)) {
                result.status = FLASHLOADER_FLASH_BOOT_PARA_ERROR;
            } else {
                result.reset = 1U;
            }
            break;
#endif

        default:
            result.status = FLASHLOADER_CMD_ID_ERROR;
            break;
    }

    return result;
}
