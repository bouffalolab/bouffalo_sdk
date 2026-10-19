#pragma once

#include "esp_err.h"

#include <stdbool.h>
#include <stddef.h>

esp_err_t mimiclaw_audio_init(void);
esp_err_t mimiclaw_audio_play_file(const char *path, char *output, size_t output_size);
esp_err_t mimiclaw_audio_stop(char *output, size_t output_size);
esp_err_t mimiclaw_audio_status(char *output, size_t output_size);
bool mimiclaw_audio_is_playing(void);

