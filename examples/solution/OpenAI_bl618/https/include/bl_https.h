#ifndef __BL_HTTPS_H__
#define __BL_HTTPS_H__

#include <stdint.h>

#define BL_HTTPS_RET_TIMEOUT                (15000)  /* 5000 ms */

#define BL_HTTPSC_OK                        (0)
#define BL_HTTPSC_RET_TIMEOUT               (-1)    /* request timeout */
#define BL_HTTPSC_RET_ERR                   (-2)
#define BL_HTTPSC_RET_ERR_MEM               (-3)
#define BL_HTTPSC_RET_HTTP_ERR              (-4)    /* response do not look like http */
#define BL_HTTPSC_RET_ERR_BUF_TOO_SMALL     (-5)
#define BL_HTTPSC_HEAD_END_NOT_FOUND        (-6)

int cmd_https_test(int argc, char **argv);
int https_request(const char *server, uint16_t port, const uint8_t *request, int req_len, uint8_t *response, int *res_len);
#endif
