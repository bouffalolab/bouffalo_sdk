#include "flashloader_protocol.h"

uint16_t flashloader_frame_parse(const uint8_t *data, size_t data_length,
                                 struct flashloader_frame *frame)
{
    uint16_t payload_length;
    uint32_t checksum = 0;

    if ((data == NULL) || (frame == NULL) || (data_length < 4U)) {
        return FLASHLOADER_CMD_LEN_ERROR;
    }

    payload_length = (uint16_t)data[2] | ((uint16_t)data[3] << 8);
    if ((payload_length > FLASHLOADER_MAX_PAYLOAD) ||
        (data_length != (size_t)payload_length + 4U)) {
        return FLASHLOADER_CMD_LEN_ERROR;
    }

    if (data[1] != 0U) {
        for (size_t i = 2U; i < data_length; i++) {
            checksum += data[i];
        }
        if ((uint8_t)checksum != data[1]) {
            return FLASHLOADER_CMD_CRC_ERROR;
        }
    }

    frame->command = data[0];
    frame->payload = &data[4];
    frame->payload_length = payload_length;
    return FLASHLOADER_SUCCESS;
}

size_t flashloader_response_encode_ack(uint16_t status, uint8_t *output,
                                       size_t output_capacity)
{
    if (output == NULL) {
        return 0U;
    }
    if (status == FLASHLOADER_SUCCESS) {
        if (output_capacity < 2U) {
            return 0U;
        }
        output[0] = 'O';
        output[1] = 'K';
        return 2U;
    }
    if (output_capacity < 4U) {
        return 0U;
    }
    output[0] = 'F';
    output[1] = 'L';
    output[2] = (uint8_t)status;
    output[3] = (uint8_t)(status >> 8);
    return 4U;
}

size_t flashloader_response_encode_data_header(uint16_t data_length,
                                               uint8_t *output,
                                               size_t output_capacity)
{
    if ((output == NULL) || (output_capacity < 4U)) {
        return 0U;
    }
    output[0] = 'O';
    output[1] = 'K';
    output[2] = (uint8_t)data_length;
    output[3] = (uint8_t)(data_length >> 8);
    return 4U;
}
