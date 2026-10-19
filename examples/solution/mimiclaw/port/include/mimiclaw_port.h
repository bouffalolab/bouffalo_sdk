#ifndef MIMICLAW_PORT_H
#define MIMICLAW_PORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint32_t mimiclaw_port_uptime_ms(void);
void mimiclaw_port_sleep_ms(uint32_t ms);

int mimiclaw_kv_set_blob(const char *key, const void *value, size_t len);
int mimiclaw_kv_get_blob(const char *key, void *buf, size_t buf_len, size_t *saved_len);

int mimiclaw_ws_connect(const char *scheme, const char *host, uint16_t port, const char *path);
int mimiclaw_ws_disconnect(void);
bool mimiclaw_ws_is_connected(void);
int mimiclaw_ws_send_text(const char *text);
int mimiclaw_ws_send_binary(const void *data, size_t len);

typedef void (*mimiclaw_ws_text_handler_t)(const char *text, size_t len, bool is_finished);
void mimiclaw_ws_set_text_handler(mimiclaw_ws_text_handler_t handler);

#endif
