#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
// #include <aos/kernel.h>
#include <stdlib.h>
#include <string.h>
// #include <cli.h>
// #include <utils_log.h>

#include "https.h"
#include "bl_error.h"
#include "bl_https.h"
#include "mbedtls/ssl.h"

#include "bflb_mtimer.h"

#define DBG_TAG "HTTP/S"
#include "log.h"

void *memmem(const void *haystack, size_t n, const void *needle, size_t m);

void *memmem(const void *haystack, size_t n, const void *needle, size_t m)
{
	const unsigned char *y = (const unsigned char *)haystack;
	const unsigned char *x = (const unsigned char *)needle;

	size_t j, k, l;

	if (m > n || !m || !n)
		return NULL;

	if (1 != m) {
		if (x[0] == x[1]) {
			k = 2;
			l = 1;
		} else {
			k = 1;
			l = 2;
		}

		j = 0;
		while (j <= n - m) {
			if (x[1] != y[j + 1]) {
				j += k;
			} else {
				if (!memcmp(x + 2, y + j + 2, m - 2)
				    && x[0] == y[j])
					return (void *)&y[j];
				j += l;
			}
		}
	} else
		do {
			if (*y == *x)
				return (void *)y;
			y++;
		} while (--n);

	return NULL;
}

static int isdigit_c(int c)
{
    return c >= '0' && c <= '9';
}

static int proc_http_head(const uint8_t *buf, int buf_len, int *status_code, int *body_start_off)
{
    char status_code_buf[4] = { '\0', '\0', '\0', '\0' };
    void *head_end_pos = NULL;

    if (buf_len < 16) { // "HTTP/1.1 XYZ\r\n\r\n"
        return BL_HTTPSC_RET_HTTP_ERR;
    }
    if (!((memcmp(buf, "HTTP/1.0 ", 9) == 0) || (0 == memcmp(buf, "HTTP/1.1 ", 9)))) {
        return BL_HTTPSC_RET_HTTP_ERR;
    }

    memcpy(status_code_buf, buf + 9, 3);
    if (!(isdigit_c(status_code_buf[0]) && isdigit_c(status_code_buf[1]) && isdigit_c(status_code_buf[2]))) {
        return BL_HTTPSC_RET_HTTP_ERR;
    }
    *status_code = atoi(status_code_buf);

    if ((head_end_pos = memmem(buf, buf_len, "\r\n\r\n", 4)) == NULL) {
        return BL_HTTPSC_HEAD_END_NOT_FOUND;
    }
    *body_start_off = (uint8_t *)head_end_pos - buf + 4;

    return BL_HTTPSC_OK;
}

/* parm server(in):       hostname or ip address of https server
 * parm port(in):         https server port
 * parm request(in):      https request
 * parm req_len(in):      https request len(excluding '\0')
 * parm response(out):    https response(excluding http header)
 * parm res_len(in, out): pass in addr of A, A stores capacity of buffer response
 *                        pass out https response body length(ie no http header)
 * out:                   HTTP status code or BL_HTTPSC error code
 */
int https_request(const char *server, uint16_t port, const uint8_t *request, int req_len, uint8_t *response, int *res_len)
{
    int count = 0;
    int ret_val = BL_HTTPSC_OK;
    uint8_t *rcv_buf = NULL;
    int buf_sz = 16*1024;
    int32_t fd, ret, send_ret, rcv_ret;
    int resp_write_off = 0;
    int http_head_proced = 0;
    int status_code = BL_HTTPSC_RET_ERR;
    int write_len;

    rcv_buf = malloc(buf_sz);
    if (rcv_buf == NULL) {
        free(rcv_buf);
        LOG_E("rcv_buf do not have space\r\n");
        return BL_HTTPSC_RET_ERR_MEM;
    }
    memset(rcv_buf, 0, sizeof(rcv_buf));

    uint32_t start_time = 0, stop_time = 0;
    start_time = xTaskGetTickCount();
    fd = bl_TcpSslConnect(server, port);

    LOG_I("bouffalo connect fd = 0x%08lx\r\n", fd);

    if (fd == BL_TCP_CREATE_CONNECT_ERR) {
        LOG_E("ssl connect error\r\n");
        ret_val = BL_HTTPSC_RET_ERR;
        goto exit;
    } else {
        ret = bl_TcpSslState(fd);
    }

    LOG_I("ret = %ld\r\n", ret);

    while (ret != BL_TCP_NO_ERROR)
    {
        // count += 100;
        // vTaskDelay(100);
        if (ret == BL_TCP_CONNECTING) {
            ret = bl_TcpSslState(fd);
        } else if(ret == BL_TCP_CONNECT_ERR) {
            LOG_E("ssl tcp connect failed\r\n");
            ret_val = BL_HTTPSC_RET_ERR;
            goto exit;
        }
        if (BL_HTTPS_RET_TIMEOUT < count) {
            LOG_E("ssl tcp connect timeout\r\n");
            ret_val = BL_HTTPSC_RET_TIMEOUT;
            goto exit;
        }
    }

    LOG_I("ret = %ld\r\n", ret);

    stop_time = xTaskGetTickCount();

    LOG_I("total time:%d ms\r\n", (stop_time - start_time));

    if (ret == BL_TCP_NO_ERROR) {
        send_ret = bl_TcpSslSend(fd, request, req_len);
        if (send_ret > 0) {
            while(1) {
                rcv_ret = bl_TcpSslRead(fd, rcv_buf, buf_sz);
                if (rcv_ret > 0) {
                    LOG_I("rcv_ret = %ld\r\n", rcv_ret);
                    if (!http_head_proced) {
                        int body_start_off = -1;
                        int proc_head_r = proc_http_head(rcv_buf, rcv_ret, &status_code, &body_start_off);
                        LOG_I("proc_head_r %d, status_code %d, body_start_off %d\r\n", proc_head_r, status_code, body_start_off);
                        if (proc_head_r == BL_HTTPSC_OK) {
                            write_len = rcv_ret - body_start_off;
                            if (write_len + resp_write_off > *res_len) {
                                ret_val = BL_HTTPSC_RET_ERR_BUF_TOO_SMALL;
                                break;
                            }
                            http_head_proced = 1;
                            LOG_I("Copy to resp @off %d, len %d, 1st char %02X\r\n", resp_write_off, write_len, *(rcv_buf + body_start_off));
                            memcpy(response, rcv_buf + body_start_off, write_len);
                            resp_write_off += write_len;
                        }
                    } else {
                        write_len = rcv_ret;
                        if (write_len + resp_write_off > *res_len) {
                            ret_val = BL_HTTPSC_RET_ERR_BUF_TOO_SMALL;
                            break;
                        }
                        LOG_I("Copy to resp @off %d, len %d, 1st char %02X\r\n", resp_write_off, write_len, *rcv_buf);
                        memcpy(response + resp_write_off, rcv_buf, write_len);
                        resp_write_off += write_len;
                    }
                    ret_val = status_code;
                    break;
                } else if (rcv_ret < 0) {
                    LOG_I("rcv_ret = %ld\r\n", rcv_ret);
                    ret_val = BL_HTTPSC_RET_ERR;
                    break;
                } // rcv_ret>0
            } // while(1)
        } else {
            LOG_E("ssl tcp send data failed\r\n");
        }
    }
exit:
    bl_TcpSslDisconnect(fd);
    *res_len = resp_write_off;
    free(rcv_buf);
    return ret_val;
}

void test_0(void)
{
    const char *host = "10.89.110.120";
    const uint8_t send_buf[] = "GET /1M HTTP/1.1\r\n"
                               //"Host: httpbin.org\r\n"
                               "User-Agent: curl/7.58.0\r\n"
                               //"accept: application/json\r\n\r\n";
                               "\r\n";
    uint8_t *buf;
    int resp_len;
    int status_code;

    buf = malloc(16 * 1024);
    if (NULL == buf) {
        LOG_E("malloc\r\n");
        return;
    }

    resp_len = 16 * 1024;
    status_code = https_request(host, 443, send_buf, sizeof(send_buf) - 1, buf, &resp_len);

    LOG_I("test_0: status_code %d, resp_len %d\r\n", status_code, resp_len);

    free(buf);
}

static void test_1(void)
{
    const char *host = "www.baidu.com";
    const uint8_t send_buf[] = "GET / HTTP/1.1\r\n"
                               "Host: www.baidu.com\r\n"
                               "User-Agent: curl/7.58.0\r\n"
                               "accept: application/json\r\n"
                               "\r\n";
    uint8_t *buf;
    int resp_len;
    int status_code;

    buf = malloc(16 * 1024);
    if (NULL == buf) {
        LOG_E("malloc\r\n");
        return;
    }

    resp_len = 16 * 1024;

    uint32_t start_time = 0, stop_time = 0;
    start_time = xTaskGetTickCount();
    status_code = https_request(host, 443, send_buf, sizeof(send_buf) - 1, buf, &resp_len);
    stop_time = xTaskGetTickCount();

    LOG_I("total time:%d ms\r\n", (stop_time - start_time));
    LOG_I("test_1: status_code %d, resp_len %d\r\n", status_code, resp_len);
    buf[resp_len] = '\0';
    LOG_I("resp body: %s\r\n", buf);

    free(buf);
}

#if 0
static void test_2(void)
{
    const char *host = "10.89.110.120";
    const uint8_t send_buf[] = "GET /403 HTTP/1.1\r\n"
                               "\r\n";
    uint8_t *buf;
    int resp_len;
    int status_code;

    buf = malloc(16 * 1024);
    if (NULL == buf) {
        LOG_E("malloc\r\n");
        return;
    }

    resp_len = 16 * 1024;
    status_code = https_request(host, 443, send_buf, sizeof(send_buf) - 1, buf, &resp_len);

    LOG_I("test_2: status_code %d, resp_len %d\r\n", status_code, resp_len);

    free(buf);
}

static void test_3(void)
{
    const char *host = "10.89.110.120";
    const uint8_t send_buf[] = "GET / HTTP/1.1\r\n"
                               "\r\n";
    uint8_t *buf;
    int resp_len;
    int status_code;

    buf = malloc(16 * 1024);
    if (NULL == buf) {
        LOG_E("malloc\r\n");
        return;
    }

    resp_len = 16 * 1024;
    status_code = https_request(host, 443, send_buf, sizeof(send_buf) - 1, buf, &resp_len);

    LOG_I("test_3: status_code %d, resp_len %d\r\n", status_code, resp_len);

    free(buf);
}
#endif


static void test_https_entry(void *arg)
{
    //test_0();
    test_1();
    //test_2();
    //test_3();

    vTaskDelete(NULL);
}

#ifdef CONFIG_SHELL
#include <shell.h>

extern uint32_t wifi_state;
static int check_wifi_state(void)
{
    if (wifi_state == 1)
    {
        return 0;
    } else {
        return 1;
    }
}

int cmd_https_test(int argc, char **argv)
{
    uint32_t ret = 0;

    ret = check_wifi_state();
    if (ret != 0) {
        printf("your wifi not connected!\r\n");
        return 0;
    }

    xTaskCreate(test_https_entry,(char*)"test_https", 8192, NULL, 10, NULL);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_https_test, wifi_https_test, wifi https client test);
#endif
