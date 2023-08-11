#ifndef __BL_TEST_H__
#define __BL_TEST_H__

#include <stdint.h>

#define BL_HTTPS_RET_TIMEOUT                (5000)  /* 5000 ms */

#define BL_HTTPSC_OK                        (0)
#define BL_HTTPSC_RET_TIMEOUT               (-1)    /* request timeout */
#define BL_HTTPSC_RET_ERR                   (-2)
#define BL_HTTPSC_RET_ERR_MEM               (-3)
#define BL_HTTPSC_RET_HTTP_ERR              (-4)    /* response do not look like http */
#define BL_HTTPSC_RET_ERR_BUF_TOO_SMALL     (-5)
#define BL_HTTPSC_HEAD_END_NOT_FOUND        (-6)

int cmd_https_test(int argc, char **argv);

#endif
