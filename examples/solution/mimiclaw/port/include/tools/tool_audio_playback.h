#pragma once

#include "esp_err.h"

#include <stddef.h>

esp_err_t tool_play_audio_execute(const char *input_json, char *output, size_t output_size);
esp_err_t tool_stop_audio_execute(const char *input_json, char *output, size_t output_size);
esp_err_t tool_audio_status_execute(const char *input_json, char *output, size_t output_size);

