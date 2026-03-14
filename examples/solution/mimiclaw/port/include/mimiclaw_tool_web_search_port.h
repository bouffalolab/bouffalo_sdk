#ifndef MIMICLAW_TOOL_WEB_SEARCH_PORT_H
#define MIMICLAW_TOOL_WEB_SEARCH_PORT_H

#include "esp_err.h"

const char *mimiclaw_web_search_get_provider(void);
esp_err_t mimiclaw_web_search_set_provider(const char *provider);

#endif
