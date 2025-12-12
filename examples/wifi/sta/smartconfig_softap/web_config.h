#ifndef __WEB_CONFIG_H__
#define __WEB_CONFIG_H__

#include <stdint.h>

typedef enum {
    CONN_STATUS_IDLE = 0,
    CONN_STATUS_CONNECTING,
    CONN_STATUS_SUCCESS,
    CONN_STATUS_FAILED,
    CONN_STATUS_AP_CLOSED,
} conn_status_t;

typedef struct {
    volatile conn_status_t conn_status;
} web_config_t;

int web_config_init(web_config_t *ctx);
void web_config_notify_result(int success);
void web_config_do_scan(void);

#endif /* __WEB_CONFIG_H__ */
