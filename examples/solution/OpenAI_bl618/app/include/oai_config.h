#ifndef __OAI_CONFIG__
#define __OAI_CONFIG__

#include <stdio.h>

#define MAX_HTTP_OUTPUT_BUFFER 2048
#define OPENAI_REALTIMEAPI ("https://api.openai.com/v1/realtime?model=gpt-4o-mini-realtime-preview-2024-12-17")
extern uint8_t openai_api_key[256];
void oai_init_audio_encoder(void);
void oai_init_audio_decoder(void);
int oai_audio_init(void);
void oai_audio_decode(uint8_t *data, size_t size);
void oai_http_request(char *offer, char *answer);
#endif
