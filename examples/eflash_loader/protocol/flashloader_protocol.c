#include "flashloader_protocol.h"

#include "flashloader_commands.h"
#include "flashloader_port.h"
#include "flashloader_transport.h"

static uint8_t g_request[FLASHLOADER_REQUEST_BUFFER_SIZE];
static uint8_t g_response[FLASHLOADER_RESPONSE_BUFFER_SIZE];

void flashloader_protocol_run(void)
{
    for (;;) {
        struct flashloader_command_result result = { 0 };
        struct flashloader_frame frame;
        size_t request_length = 0U;
        size_t response_length;
        uint16_t status;
        uint8_t handshake = 0U;

        if (flashloader_transport_receive(g_request, sizeof(g_request),
                                          &request_length, &handshake) != 0) {
            continue;
        }

        if (handshake != 0U) {
            response_length = flashloader_response_encode_ack(
                FLASHLOADER_SUCCESS, g_response, sizeof(g_response));
        } else {
            status = flashloader_frame_parse(g_request, request_length, &frame);
            if (status == FLASHLOADER_SUCCESS) {
                result = flashloader_command_execute(
                    frame.command, frame.payload, frame.payload_length,
                    &g_response[4], sizeof(g_response) - 4U);
                status = result.status;
            }

            if ((status == FLASHLOADER_SUCCESS) && (result.has_data != 0U)) {
                response_length = flashloader_response_encode_data_header(
                    result.data_length, g_response, sizeof(g_response));
                response_length += result.data_length;
            } else {
                response_length = flashloader_response_encode_ack(
                    status, g_response, sizeof(g_response));
            }
        }

        (void)flashloader_transport_send(g_response, response_length);
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_RESET) != 0U
        if (result.reset != 0U) {
#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_HOST_ACK) != 0U
            flashloader_transport_wait_response_consumed();
#endif
            flashloader_port_reset();
        }
#endif
    }
}
