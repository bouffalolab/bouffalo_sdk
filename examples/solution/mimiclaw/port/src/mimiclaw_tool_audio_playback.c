#include "tools/tool_audio_playback.h"

#include "mimiclaw_audio_player.h"

#include "cJSON.h"

#include <stdio.h>
#include <string.h>

esp_err_t tool_play_audio_execute(const char *input_json, char *output, size_t output_size)
{
    cJSON *root = NULL;
    cJSON *path_item;
    esp_err_t ret;

    if (!output || output_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!input_json || input_json[0] == '\0') {
        snprintf(output, output_size, "Invalid input: missing JSON");
        return ESP_ERR_INVALID_ARG;
    }

    root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "Invalid JSON");
        return ESP_ERR_INVALID_ARG;
    }

    path_item = cJSON_GetObjectItem(root, "path");
    if (!cJSON_IsString(path_item) || !path_item->valuestring || path_item->valuestring[0] == '\0') {
        cJSON_Delete(root);
        snprintf(output, output_size, "Invalid input: path is required");
        return ESP_ERR_INVALID_ARG;
    }

    ret = mimiclaw_audio_play_file(path_item->valuestring, output, output_size);
    cJSON_Delete(root);
    return ret;
}

esp_err_t tool_stop_audio_execute(const char *input_json, char *output, size_t output_size)
{
    (void)input_json;
    return mimiclaw_audio_stop(output, output_size);
}

esp_err_t tool_audio_status_execute(const char *input_json, char *output, size_t output_size)
{
    (void)input_json;
    return mimiclaw_audio_status(output, output_size);
}

