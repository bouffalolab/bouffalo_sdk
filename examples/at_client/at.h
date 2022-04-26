#ifndef __AT_H_
#define __AT_H_

enum at_resp_status {
    AT_RESP_OK = 0,         /* AT response end is OK */
    AT_RESP_ERROR = -1,     /* AT response end is ERROR */
    AT_RESP_TIMEOUT = -2,   /* AT response is timeout */
    AT_RESP_BUFF_FULL = -3, /* AT response buffer is full */
};

struct at_client {
    /* the current received one line data buffer */
    uint8_t *recv_line_buf;
    /* The length of the currently received one line data */
    uint32_t recv_line_len;
    /* The maximum supported receive data length */
    uint32_t recv_bufsz;

    uint32_t timeout;
    const char *resp_succ;
    const char *resp_err;
    enum at_resp_status resp_status;
};

int at_exe_cmd(struct at_client *client, char *cmd_expr, ...);
int at_write_recv(uint8_t *send_buffer, uint32_t send_len, uint8_t *recv_buffer, uint32_t *recv_len, uint32_t timeout);
void at_dump_hex(const void *ptr, uint32_t buflen);
void at_client_init(void);
#endif