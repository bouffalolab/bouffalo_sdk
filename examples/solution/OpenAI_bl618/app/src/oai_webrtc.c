/* Converted webrtc.c */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <opus/opus.h>
#include "peer_connection.h"
#include <FreeRTOS.h>
#include <task.h>
#include <oai_config.h>
#include <easyflash.h>
#include <bl616_sys.h>

#define TICK_INTERVAL 15
#define GREETING                                                    \
  "{\"type\": \"response.create\", \"response\": {\"modalities\": " \
  "[\"audio\", \"text\"], \"instructions\": \"Say '我是你的智能语音小助手'\"}}"

PeerConnection *peer_connection = NULL;


void oai_send_audio_task(void *user_data)
{
    //oai_init_audio_encoder();

void oai_audio_record_start(void);

    oai_audio_record_start();
    vTaskDelay(pdMS_TO_TICKS(TICK_INTERVAL * 2));
    while (1) {
void oai_send_audio(PeerConnection *peer_connection);
        oai_send_audio(peer_connection);
        vTaskDelay(pdMS_TO_TICKS(TICK_INTERVAL));
    }
}

static void oai_audio_track_callback(uint8_t *data, size_t size, void *userdata)
{
    oai_audio_decode(data, size);
}

static void oai_ondatachannel_onmessage_task(char *msg, size_t len,
                                             void *userdata, uint16_t sid)
{
#ifdef LOG_DATACHANNEL_MESSAGES
    printf("DataChannel Message: %s\r\n", msg);
#endif
}

static void oai_ondatachannel_onopen_task(void *userdata)
{
    if (peer_connection_create_datachannel(peer_connection, DATA_CHANNEL_RELIABLE,
                                         0, 0, (char *)"oai-events",
                                         (char *)"") != -1) {
        printf("DataChannel created\r\n");
        peer_connection_datachannel_send(peer_connection, (char *)GREETING,
                                     strlen(GREETING));
    } else {
        printf("Failed to create DataChannel\r\n");
    }
}

static void oai_onconnectionstatechange_task(PeerConnectionState state,
                                             void *user_data)
{
    static TaskHandle_t oai_audio_task;
    printf("PeerConnectionState: %s\r\n", peer_connection_state_to_string(state));

    if (state == PEER_CONNECTION_DISCONNECTED ||
        state == PEER_CONNECTION_CLOSED) {
        printf("disconnect\r\n");
        bl_sys_reset_por();
    } else if (state == PEER_CONNECTION_CONNECTED) {
        xTaskCreate(oai_send_audio_task, "audio_publisher", 20000,
                                  NULL, 7, &oai_audio_task);
    }
}

static void oai_on_icecandidate_task(char *description, void *user_data)
{
    char local_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};
    oai_http_request(description, local_buffer);
    peer_connection_set_remote_description(peer_connection, local_buffer);
}

uint8_t openai_api_key[256] = {0};

void oai_webrtc(void *arg)
{
    PeerConfiguration peer_connection_config = {
        .ice_servers = {},
        .audio_codec = CODEC_OPUS,
        .video_codec = CODEC_NONE,
        .datachannel = DATA_CHANNEL_STRING,
        .onaudiotrack = oai_audio_track_callback,
        .onvideotrack = NULL,
        .on_request_keyframe = NULL,
        .user_data = NULL,
    };

    while (1) {
        memset(openai_api_key, 0, 256);
        ef_get_env_blob("OPENAI_API_KEY", openai_api_key, 256, NULL);
        if (openai_api_key[0]) {
            printf("set OPENAI_API_KEY success :%s\r\n", openai_api_key);
            break;
        }
        printf("please set OPENAI_API_KEY\r\n");
        vTaskDelay(2000);
    }

    peer_connection = peer_connection_create(&peer_connection_config);
    if (peer_connection == NULL) {
        printf("Failed to create peer connection\r\n");
        bl_sys_reset_por();
    }

    peer_connection_oniceconnectionstatechange(peer_connection,
                                             oai_onconnectionstatechange_task);
    peer_connection_onicecandidate(peer_connection, oai_on_icecandidate_task);
    peer_connection_ondatachannel(peer_connection,
                                oai_ondatachannel_onmessage_task,
                                oai_ondatachannel_onopen_task, NULL);

    peer_connection_create_offer(peer_connection);

    while (1) {
        peer_connection_loop(peer_connection);
        vTaskDelay(pdMS_TO_TICKS(TICK_INTERVAL));
    }
}

static TaskHandle_t webrtc_task;

void webrtc_main(void)
{
    xTaskCreate(oai_webrtc, (char *)"webrtc", 4096, NULL, 15, &webrtc_task);
}
