#ifndef NETHUB_WIFI_PARSER_AT_H
#define NETHUB_WIFI_PARSER_AT_H

#include "nethub_wifi_types.h"

int nethub_at_parse_cwlap(const char *line, nethub_wifi_ap_info_t *ap);
int nethub_at_parse_cwjap(const char *line, nethub_wifi_status_t *status);
int nethub_at_parse_cwjap_error(const char *line, int *wifi_reason);
int nethub_at_parse_cipsta(const char *line, nethub_wifi_status_t *status);
int nethub_at_parse_gotip(const char *line, nethub_wifi_status_t *status);
int nethub_at_parse_wifi_error(const char *line);

#endif /* NETHUB_WIFI_PARSER_AT_H */
