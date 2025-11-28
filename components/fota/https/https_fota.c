/**
  ******************************************************************************
  * @file    https_fota.c
  * @version V1.0
  * @date
  * @brief   This file is https OTA feature
  ******************************************************************************
  */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <lwip/api.h>
#include <lwip/arch.h>
#include <lwip/opt.h>
#include <lwip/inet.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "shell.h"
#include "utils_getopt.h"
#include "https_client.h"
#include "https_fota.h"
#include "ota/ota.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif

#define HTTPS_FOTA_STATUS_CALLBACK(handle, s)                 \
        if (handle->status != s) {                            \
            handle->status  = s;                              \
            if (handle->cb) {                                 \
                handle->cb(handle->cb_arg, handle->status);   \
            }                                                 \
        }


struct https_fota {
    const char *url;
    ota_handle_t ota;
    size_t file_size;
    struct https_fota_config config;
    pfn_https_fota_t cb;
    void *cb_arg;
    uint8_t status;
    uint8_t auto_finish;
};

static int payload_cb(int sock, struct http_request *req, void *user_data)
{
	const char *content[] = {
		"foobar",
		"chunked",
		"last"
	};
	char tmp[64];
	int i, pos = 0;

	for (i = 0; i < ARRAY_SIZE(content); i++) {
		pos += snprintf(tmp + pos, sizeof(tmp) - pos,
				"%x\r\n%s\r\n",
				(unsigned int)strlen(content[i]),
				content[i]);
	}

	pos += snprintf(tmp + pos, sizeof(tmp) - pos, "0\r\n\r\n");

	(void)zsock_send(sock, tmp, pos, 0);

	return pos;
}

static void response_cb(struct http_response *rsp,
			enum http_final_call final_data,
			void *user_data)
{
    struct https_fota *fota = (struct https_fota *)user_data;

    //rsp->body_frag_start, rsp->body_frag_len

    LOG_I("\r\nbody_frag_len:%d recv_buf_len:%d  data_len:%d content_length:%d\r\n", rsp->body_frag_len, rsp->recv_buf_len, rsp->data_len, rsp->content_length);

    HTTPS_FOTA_STATUS_CALLBACK(fota, HTTPS_FOTA_PROCESS_TRANSFER)

    ota_update(fota->ota, rsp->body_frag_start, rsp->body_frag_len);
    fota->file_size += rsp->body_frag_len;

	if (final_data == HTTP_DATA_MORE) {

	} else if (final_data == HTTP_DATA_FINAL) {

        if (fota->file_size == rsp->content_length) {
            HTTPS_FOTA_STATUS_CALLBACK(fota, HTTPS_FOTA_TRANSFER_FINISH)
        }
		printf("HTTP_DATA_FINAL All the data received (%zd bytes)\r\n", rsp->data_len);
	}
}


static void fota_service_start(void *arg)
{
    int ret;
    struct https_client_request req;

    struct https_fota *fota = (struct https_fota *)arg;

    memset(&req, 0, sizeof(req));
	req.method           = HTTP_GET;
	req.url              = fota->url;
	req.protocol         = "HTTP/1.1";
	req.response         = response_cb;
	req.ca_pem           = fota->config.ca_pem;
	req.ca_len           = fota->config.ca_len;
	req.client_cert_pem  = fota->config.client_cert_pem;
	req.client_cert_len  = fota->config.client_cert_len;
	req.client_key_pem   = fota->config.client_key_pem;
	req.client_key_len   = fota->config.client_key_len;

	req.buffer_size      = HTTPS_FOTA_BUFFER_SIZE;

    fota->file_size      = 0;
    HTTPS_FOTA_STATUS_CALLBACK(fota, HTTPS_FOTA_START)
	ret = https_client_request(&req, HTTPS_FOTA_REQUEST_TIMEOUT_MS, fota);
    if (ret < 0) {
        LOG_E("FOTA http request fail ret:%d\r\n", ret);
        HTTPS_FOTA_STATUS_CALLBACK(fota, HTTPS_FOTA_SERVER_CONNECTE_FAIL)
    }

    if (fota->status != HTTPS_FOTA_TRANSFER_FINISH) {
        https_fota_abort(fota);
    }

    if (fota->auto_finish && fota->status == HTTPS_FOTA_TRANSFER_FINISH) {
        https_fota_finish(fota, 0);
    }

    vTaskDelete(NULL);
}

https_fota_handle_t https_fota_init(const char *url, const struct https_fota_config *config)
{
    int ret;
    struct https_fota *fota = calloc(sizeof(struct https_fota), 1);
    if (!fota) {
        LOG_E("FOTA alloc fail ret\r\n");
		return NULL;
    }

	fota->ota = ota_start();
	if (!fota->ota) {
        free(fota);
		return NULL;
	}

    fota->url = strdup(url);
    if (config) {
        fota->config = *config;
        https_fota_callback_register(fota, config->callback, config->user_arg);
    }

    return fota;
}

int https_fota_start(https_fota_handle_t fota)
{
    int ret = 0;

    ret = xTaskCreate(fota_service_start, "fota_service", 2048, (const void *)fota, 10, NULL);
    if (ret != pdPASS) {
        LOG_E("FOTA start fail ret:%d\r\n", ret);
        return -1;
    }

	return 0;
}

int https_fota_callback_register(https_fota_handle_t fota, pfn_https_fota_t pfn, void *arg)
{
	struct https_fota *https_fota = (struct https_fota *)fota;

	https_fota->cb = pfn;
	https_fota->cb_arg = arg;
	return 0;
}

int https_fota_finish(https_fota_handle_t fota, bool reboot)
{
    int ret = 0;
	struct https_fota *https_fota = (struct https_fota *)fota;

    if (https_fota->status != HTTPS_FOTA_TRANSFER_FINISH) {
        LOG_E("Transmission not completed\r\n");
        return -HTTPS_FOTA_PROCESS_TRANSFER;
    }
    HTTPS_FOTA_STATUS_CALLBACK(https_fota, HTTPS_FOTA_IMAGE_VERIFY)
    ret = ota_finish(https_fota->ota, 1, reboot);
    if (ret != 0) {
        HTTPS_FOTA_STATUS_CALLBACK(https_fota, HTTPS_FOTA_IMAGE_VERIFY_FAIL)
        ret = -HTTPS_FOTA_IMAGE_VERIFY_FAIL;
        ota_abort(https_fota->ota);
        goto __end;
    }
    HTTPS_FOTA_STATUS_CALLBACK(https_fota, HTTPS_FOTA_SUCCESS)
    ret = HTTPS_FOTA_SUCCESS;
__end:
    free(https_fota->url);
    free(https_fota);
    return ret;
}

int https_fota_abort(https_fota_handle_t fota)
{
	struct https_fota *https_fota = (struct https_fota *)fota;

    ota_abort(https_fota->ota);
    free(https_fota->url);
    free(https_fota);
    return 0;
}

int https_fota(const char *url, const struct https_fota_config *config)
{
    int ret;
    struct https_fota *handle = (struct https_fota *)https_fota_init(url, config);

    if (handle == NULL) {
        LOG_E("FOTA init fail\r\n");
        return -HTTPS_FOTA_ABORT;
    }

    handle->auto_finish = 1;
    ret = https_fota_start(handle);
    if (ret != 0) {
        https_fota_abort(handle);
        return -HTTPS_FOTA_ABORT;
    }

    return 0;
}

int https_ota_rollback(void)
{
    int ret;
    ret = ota_rollback();
    return ret;
}

