
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
#include "bflb_mtimer.h"
#include "https_client.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif 

const char ca_cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDAzCCAeugAwIBAgIUSIynf9c+C4TGvBNrpOGW0EsWKRUwDQYJKoZIhvcNAQEL\n"
    "BQAwETEPMA0GA1UEAwwGTXlDQV8xMB4XDTI0MDUyMzAxNTIzOFoXDTI1MDUyMzAx\n"
    "NTIzOFowETEPMA0GA1UEAwwGTXlDQV8xMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\n"
    "MIIBCgKCAQEAj7Xp94L7VLyo6m4AdvKIa1pOjZQUzPnfgkAJhdHmupIMhdx8/L/i\n"
    "Yn55tZhPOzk7S+CYuYoH1Bx/tvmP+HJ9J8qeNOPm+QbiYNUSJcs2vXkKMFgiSqh4\n"
    "zE6B80+clSSR0de80s7td0PTL9k/emQSu+mV1q2JrXyhcP6/fumt1TzrmTb2SxiB\n"
    "rMQh76v8NfV0a0Y78QDd+S0JIhJ27S610DCSnl+B1pgI8sMuwtSSNOyNYFbym2PO\n"
    "LYUYaN9Lup7i2pxPi/p/tIwBq3mbs01JawBMFArTkidxIK+xkf4NVUT1weurw+s8\n"
    "3yC+pomUtvKpCNxdgSslnCxNhoUYxxCrawIDAQABo1MwUTAdBgNVHQ4EFgQUz1kI\n"
    "khTZxHjVYlOk16H7v57pJCowHwYDVR0jBBgwFoAUz1kIkhTZxHjVYlOk16H7v57p\n"
    "JCowDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEASyul5PoQ8V0M\n"
    "+lKOCxeL8qbYTTLJzcIGcN7FQzTWHYqfmdlc+9IZ7mHk1RR+fxNll3EpHaqA/U9J\n"
    "bRUiBIzsMRWaCl0g3NCcZOILQ/hfgh6TgFyo5bOUKNOjBKGrV4/mnOOyPvPc5n/H\n"
    "BW3LjqmfE2CFeUruXutOPIkY8YDiPRFwCSaNapO4Bc6+kgYQz7dPEWOF0b2IcJ7O\n"
    "vD2JPeKSoUxPs71TUNzGBQx+mbXxULrhFkSs9jRdPUJQpJqaq+SVLLXOU+RsQab0\n"
    "dqud+4N3VgjA1tI/Oi23sF4xMotaWH4VrWIxuu0O8I3GYx+ZhFyhRa33ma7JxZtu\n"
    "VlA0gHsH5Q==\n"
    "-----END CERTIFICATE-----\n";

const char own_cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIC9DCCAdygAwIBAgIUZp6RfB5tG+5MCi/Wtjk2eAxtk3QwDQYJKoZIhvcNAQEL\n"
    "BQAwETEPMA0GA1UEAwwGTXlDQV8xMB4XDTI0MDUyMzAxNTIzOFoXDTI1MDUyMzAx\n"
    "NTIzOFowEzERMA8GA1UEAwwIY2xpZW50XzEwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n"
    "DwAwggEKAoIBAQCo2b69q96nG1eO7/PZCYRg78mk1Z9KbWLtQftZDJJIuIv/Xa/P\n"
    "suSUajK8bJJ5CUlP05cowcccBqZ41mlP4nC0iGqesnJd5rDo/QIo09Oms67KkW9y\n"
    "wB8vGgo5nQjeT/q05iX4di2Bo3Npj5brKocbkH4rTWidKh1LoDETW709IJICWTkU\n"
    "4Sj5IQnAuY8a9MREBkjo03KctDWG2qY9JFOdb5E6K+UmbXL7t68ItOdAlK80HUvn\n"
    "+AyhXjabfeP9JD3/UABm37Lmm3hyfbGMCJgCkVt/TXTZun+30fHzfE5Ok9+iKdki\n"
    "DZMw9DfSp+7jh4lyb01S5VN+T/5pOpEUjfi3AgMBAAGjQjBAMB0GA1UdDgQWBBSE\n"
    "t5Zai9tEI2DNfbocINJowkpNAjAfBgNVHSMEGDAWgBTPWQiSFNnEeNViU6TXofu/\n"
    "nukkKjANBgkqhkiG9w0BAQsFAAOCAQEAcIzEp40BLHawKNzEQ18iO4MWcSVmCvvG\n"
    "CqPrmuHSQ5TUkKUwlu4Kf1SqRlhy2W5dt04Ws4z7olHu6pIzKKGSvHUVRPM85+CH\n"
    "ZzfT2zA7mD4UgX8LO9g2HhhBzNKZQFzZgt+FPy1XxMRD+zunnDDXrmTNW2whBZsP\n"
    "AKIro78SpJbwUpQW/Go6a2YYMYY6QGIXcoNFyhkL2Wk6SIMqmvrHXQwF/LqQZupQ\n"
    "CYVdac/j7bQPKweIclsq7b9O1dixurCCSjJ6V7CBTZbUM53jBs2BZA60lRmwVHFl\n"
    "oGtU9KCUhH7IJXQYAm1sqxnfICaVJdxb0a5ahCj5LMsZQXFvabOWNg==\n"
    "-----END CERTIFICATE-----\n";

const char private_cert[] =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCo2b69q96nG1eO\n"
    "7/PZCYRg78mk1Z9KbWLtQftZDJJIuIv/Xa/PsuSUajK8bJJ5CUlP05cowcccBqZ4\n"
    "1mlP4nC0iGqesnJd5rDo/QIo09Oms67KkW9ywB8vGgo5nQjeT/q05iX4di2Bo3Np\n"
    "j5brKocbkH4rTWidKh1LoDETW709IJICWTkU4Sj5IQnAuY8a9MREBkjo03KctDWG\n"
    "2qY9JFOdb5E6K+UmbXL7t68ItOdAlK80HUvn+AyhXjabfeP9JD3/UABm37Lmm3hy\n"
    "fbGMCJgCkVt/TXTZun+30fHzfE5Ok9+iKdkiDZMw9DfSp+7jh4lyb01S5VN+T/5p\n"
    "OpEUjfi3AgMBAAECggEAQcs/IOdUxibIUoE7somb1K37E8jN3hGLp8m7KDLW2ZFX\n"
    "s/UMqdEacp6DEJr55EHzGDDpyM7hSBFrUtCxjpg8tIwnh5kfKWnX66GS8te/tEh1\n"
    "xqcvFza3sAHklBiVuRLPLSg1CYD7MHXBZhO2igIzTVFbaIIsTnl3mt1b4iAwOB3S\n"
    "3rE5yRDvYXo9SM8aoL7iDaQoWd4z8hLiQmxMBDwI9sjp0BFUTLBjWu/qcikcWwOx\n"
    "Z9MtYwwl6Dldvf3tJ9cTY4/6wQKMMsLqS3Jov2k8xV0DvGU0Xkq6N96L++iaE3Iz\n"
    "bz+AbWz1J3UAHJ5T844TJtn2KxNWdqIgUryVkbqlKQKBgQDoYsBUYZnth735AKnl\n"
    "kLaP0jBKbVMaMa3ihA1qNUcCKkKfxP1xenrbS94UWRJzLq/txehb4YsptJ1AJ624\n"
    "HeMUm85A9Ka4vFxfBmMOYyxMx9OMFQbxxijv2oRP+3irupBAAEVuAbpaI4YWL4Wn\n"
    "mfeFzLFggt4bTGiUO7jvQOT4CwKBgQC6AjLtep8iUSQz/xo5ULnw/lHMf8lOin5t\n"
    "LqoA3qwx/ootOZp5WG9rVaKimELxdQwO4DtH9ZGJfbKeJDh6AbePjcHShfK2UzI0\n"
    "/xvZLzI+Wgz2ybX9HJ6y/rUy83eXMI5/G5zJEXRjgLZdDBkKP5FUllzn9dYQKq4n\n"
    "froofr0xhQKBgGUEsIi92NUJwgphQHm7u7CvdTCVb5+eYvFp6A74VALb7hRX5LiU\n"
    "vp1f0I1V3xBVBkM9WfG/DOi3S/hch0bXLySBSlqwP11k/F6Ofzb844AJCawGY/tr\n"
    "4dXKTuQZ8+3q7tPzijr041ZVxO8V/2rJInF2dtD2cqi5QWjWwWC3/BDbAoGBALhw\n"
    "Zb9Ez3Zfrt5AHeinOzjTdJiqPRo8CZCJrNif826/TpP5gkrAffqYN2OyRu7VeNdn\n"
    "UGDSBzWmBe75S2bvPOyDS6VMXJadeDNEooCAY/mfoSs0Z9hLALHMDjAvam3oH+O5\n"
    "pYVaYWYowqsQy+N1lM2jKwx2n4P4TieraTuuMDthAoGBALNnxVZyGsUj4pqTIbnh\n"
    "9CbfDkf2os2KwuLQTtkmMTRQe88mDHaVlY7dFBeuAhF9gf3Z/TapiesBlgPxehiu\n"
    "1aNylDK6ScLXxzq/ZEIX7Ou4gP3hBAHrtHbRYS8klYkV7tNCW0fmiz1M3C/8HP0V\n"
    "FHFBlzbNHqryPsFm7VVC4TyY\n"
    "-----END PRIVATE KEY-----\n";

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

static int log_output(void *ptr, size_t size)
{
    size_t i;
    for (i = 0; i < size; i++) {
        putchar(((char *)ptr)[i]);
    }
    return i;
}

static void response_cb(struct http_response *rsp,
            enum http_final_call final_data,
            void *user_data)
{
    log_output(rsp->recv_buf, rsp->data_len);
    if (final_data == HTTP_DATA_MORE) {
        //printf("Partial data received (%zd bytes)\r\n", rsp->data_len);
    } else if (final_data == HTTP_DATA_FINAL) {
        //printf("All the data received (%zd bytes)\r\n", rsp->data_len);
    }
}

#define PING_USAGE                                \
    "wifi_https_test [url]\r\n"        \
    "\t url: url or dest server ip\r\n" \

static void https_client_task_entry(void *arg)
{
    int ret;
    char *url;
    struct https_client_request req;
    const char *headers[] = {
        "Transfer-Encoding: chunked\r\n",
        NULL
    };

    url = (char *)arg;

    memset(&req, 0, sizeof(req));
    req.method           = HTTP_GET;
    req.url              = url;
    req.protocol         = "HTTP/1.1";
    req.response         = response_cb;
//	req.ca_pem           = ca_cert;
//	req.ca_len           = sizeof(ca_cert);
    req.client_cert_pem  = own_cert;
    req.client_cert_len  = sizeof(own_cert);
    req.client_key_pem   = private_cert;
    req.client_key_len   = sizeof(private_cert);
    req.buffer_size      = 4096;

    ret = https_client_request(&req, 8*1000, "IPv4 GET");
    if (ret < 0) {
        printf("http_client get request fail ret:%d\r\n", ret);
    } else {
        printf("Http client GET request server success\r\n");
    }

    memset(&req, 0, sizeof(req));

    req.method           = HTTP_POST;
    req.url              = url;
    req.protocol         = "HTTP/1.1";
    req.payload_cb       = payload_cb;
    req.header_fields    = headers;
    req.response         = response_cb;
//	req.ca_pem           = ca_cert;
//	req.ca_len           = sizeof(ca_cert);
    req.client_cert_pem  = own_cert;
    req.client_cert_len  = sizeof(own_cert);
    req.client_key_pem   = private_cert;
    req.client_key_len   = sizeof(private_cert);
    req.buffer_size      = 4096;

    ret = https_client_request(&req, 8*1000, "IPv4 POST");
    if (ret < 0) {
        printf("http_client post request fail ret:%d\r\n", ret);
    } else {
        printf("Http client POST request server success\r\n");
    }

    free(url);
    vTaskDelete(NULL);
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_wifi_https_client(int argc, char **argv)
{
    char *url;

    if (argc < 2) {
        printf("%s", PING_USAGE);
        return;
    }
    url = strdup(argv[1]);

    xTaskCreate(https_client_task_entry, "https_client", 2048, (const void *)url, 10, NULL);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_https_client, wifi_https_test, wifi https client test);
#endif
