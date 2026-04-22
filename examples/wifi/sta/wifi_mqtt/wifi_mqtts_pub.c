#include "FreeRTOS_POSIX.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <lwip/errno.h>
#include <netdb.h>
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/net_sockets.h"

#include "utils_getopt.h"

#include "mqtt.h"
#include "shell.h"
#include "certs.h"

static uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
static uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */

static shell_sig_func_ptr abort_exec;
static int test_sockfd;
static const char* addr;


typedef struct {
    int ssl_inited;
    mbedtls_net_context net;
    mbedtls_x509_crt ca_cert;
    mbedtls_x509_crt owncert;
    mbedtls_ssl_config conf;
    mbedtls_ssl_context ssl;
    mbedtls_pk_context pkey;
} ssl_param_t;

typedef struct {
    char *ca_cert;
    int ca_cert_len;
    char *own_cert;
    int own_cert_len;
    char *private_cert;
    int private_cert_len;

    char **alpn;
    int alpn_num;

    char *psk;
    int psk_len;
    char *pskhint;
    int pskhint_len;

    char *sni;
} ssl_conn_param_t;

static int ssl_random(void *prng, unsigned char *output, size_t output_len)
{
    (void)prng;
    bflb_trng_readlen(output, output_len);
    return 0;
}


static int tcp_client_connect(char *ip, char *port_str)
{
    int fd;
    int res;

    struct sockaddr_in addr;
    ip4_addr_t remote_ip;
    int port = atoi(port_str);

    ip4addr_aton(ip, &remote_ip);

    printf("tcp client connect %s:%d\r\n", ip4addr_ntoa(&remote_ip), port);

    {
        if ( (fd =  socket(AF_INET, SOCK_STREAM, 0))  < 0) {
            printf("socket create failed\r\n");
            return -2;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_len = sizeof(addr);
        addr.sin_port = htons(port);
        //addr.sin_addr.s_addr = ((struct in_addr *) hostinfo->h_addr)->s_addr;
        addr.sin_addr.s_addr = ip4_addr_get_u32(&remote_ip);
    }

    printf("tcp_client_connect fd:%d\r\n", fd);

    int on= 1;
    res = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    if (res != 0) {
        printf("setsockopt failed, res:%d\r\n", res);
    }

    res = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (res < 0) {
        printf("connect failed, res:%d\r\n", res);
        close(fd);
    }

    return fd;
}

static int ssl_client_connect(char *ip, char *port, ssl_param_t *ssl_param)
{
    int fd, ret;
    ssl_conn_param_t param = {0};
    //int mbedtls_platform_set_printf(int (*printf_func)(const char *, ...));
    //mbedtls_platform_set_printf(printf);

    memset(ssl_param, 0, sizeof(*ssl_param));
    memset(&param, 0, sizeof(param));

    param.ca_cert = ca_cert;
    param.ca_cert_len = sizeof(ca_cert);

    param.own_cert = own_cert;
    param.own_cert_len = sizeof(own_cert);

    param.private_cert = private_cert;
    param.private_cert_len = sizeof(private_cert);

    fd = tcp_client_connect(ip, port);
    if (fd < 0) {
        printf("tcp_client_connect fd:%d\r\n", fd);
        return -1;
    }

    param.sni = "server.local";

    ssl_param->ssl_inited = 1;
    /*
     * Initialize the connection
     */
    ssl_param->net.fd = fd;

    mbedtls_ssl_config_init(&ssl_param->conf);
    mbedtls_ssl_init(&ssl_param->ssl);
    mbedtls_x509_crt_init(&ssl_param->ca_cert);
    mbedtls_x509_crt_init(&ssl_param->owncert);
    mbedtls_pk_init(&ssl_param->pkey);

    ret = mbedtls_x509_crt_parse(&ssl_param->ca_cert, (unsigned char *)param.ca_cert, (size_t)param.ca_cert_len);
    if (ret < 0) {
        printf("[MBEDTLS] ssl connect: root parse failed- 0x%x\r\n", -ret);
        goto err;
    }
    // ret = mbedtls_x509_crt_parse(&ssl_param->owncert, (unsigned char *)param.own_cert, (size_t)param.own_cert_len);
    // if (ret < 0) {
    //  printf("[MBEDTLS] ssl connect: x509 parse failed- 0x%x\r\n", -ret);
    //  goto err;
    // }
    // ret = mbedtls_pk_parse_key(&ssl_param->pkey, (unsigned char *)param.private_cert, param.private_cert_len, NULL, 0);
    // if (ret != 0) {
    //  printf("[MBEDTLS] ssl connect: x509 parse failed- 0x%x\r\n", -ret);
    //     goto err;
    // }
    ret = mbedtls_ssl_config_defaults(&ssl_param->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        printf("[MBEDTLS] ssl connect: x509 config failed- 0x%x\r\n", -ret);
        goto err;
    }
    //mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&ssl_param->conf, &ssl_param->ca_cert, NULL);
    // mbedtls_ssl_conf_own_cert(&ssl_param->conf, &ssl_param.owncert, &ssl_param.pkey);
    mbedtls_ssl_conf_rng(&ssl_param->conf, ssl_random, NULL);
    //mbedtls_ssl_conf_dbg(&ssl_param->conf, my_debug, NULL);
    //mbedtls_ssl_conf_alpn_protocols(&ssl_param->conf, (const char **)alpn_str);
    //mbedtls_ssl_conf_psk(&ssl_param->conf, (const unsigned char *)"psk123456", 10, (const unsigned char *)"identity", 9);
    if((ret = mbedtls_ssl_setup(&ssl_param->ssl, &ssl_param->conf)) != 0) {
        printf("mbedtls ssl_setup fail \r\n", -ret);
        goto err;
    }

    if (param.sni) {
        mbedtls_ssl_set_hostname(&ssl_param->ssl, param.sni);
    }
    mbedtls_ssl_set_bio(&ssl_param->ssl, &ssl_param->net, mbedtls_net_send, mbedtls_net_recv, NULL);

    /*
     * handshake
     */
    printf("[MBEDTLS] Performing the SSL/TLS handshake ... \r\n");
    while ((ret = mbedtls_ssl_handshake(&ssl_param->ssl)) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            printf("[MBEDTLS] ssl connect: mbedtls_ssl_handshake returned -0x%x\r\n", -ret);
            goto err;
        }
    }

    /*
     * verify the server certificate
     */
    printf("[MBEDTLS] ...... Verifying peer X.509 certificate ... \r\n");
    ret = mbedtls_ssl_get_verify_result(&ssl_param->ssl);
    if (ret != 0) {
        printf("[MBEDTLS] ssl connect: verify result not confirmed - %d\r\n", -ret);
        goto err;
    }

    printf("[MBEDTLS] ssl connect ok\r\n");

        /* make non-blocking */
    if (fd != -1) {
        int iMode = 1;
        ioctlsocket(fd, FIONBIO, &iMode);
    }


        return fd;

err:
        mbedtls_ssl_close_notify(&ssl_param->ssl);
    mbedtls_x509_crt_free(&ssl_param->ca_cert);
    mbedtls_ssl_free(&ssl_param->ssl);
    mbedtls_ssl_config_free(&ssl_param->conf);

    close(ssl_param->net.fd);
        return -1;
}

/**
 * @brief The function will be called whenever a PUBLISH message is received.
 */
static void publish_callback_1(void** unused, struct mqtt_response_publish *published);

static ssl_param_t g_ssl_param = {0};
static void test_close(int sig)
{
    if (test_sockfd)
    {
        close(test_sockfd);
    }

    printf("mqtt_pub stop publish to %s\r\n", addr);

    abort_exec(sig);
}

static int example_mqtt(int argc, const char *argv[])
{
    const char* port;
    const char* topic;

    int ret = 0;
    // int argc = 0;

    abort_exec = shell_signal(1, test_close);

    /* get address (argv[1] if present) */
    if (argc > 1) {
        addr = argv[1];
    }

    /* get port number (argv[2] if present) */
    if (argc > 2) {
        port = argv[2];
    }

    /* get the topic name to publish */
        topic = "mqtt_test";

    /* open the non-blocking TCP socket (connecting to the broker) */
        test_sockfd = ssl_client_connect(addr, port, &g_ssl_param);
        struct custom_socket_handle handle;
        handle.type = MQTTC_PAL_CONNTION_TYPE_TLS;
        handle.ctx.fd = test_sockfd;
        handle.ctx.ssl_ctx = &g_ssl_param.ssl;

    if (test_sockfd < 0) {
        printf("Failed to open socket: %d\r\n", test_sockfd);
        test_close(SHELL_SIGINT);
    }

    /* setup a client */
    struct mqtt_client client;

    mqtt_init(&client, &handle, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback_1);
    /* Create an anonymous session */
    const char* client_id = NULL;
    /* Ensure we have a clean session */
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    /* Send connection request to the broker. */
    ret = mqtt_connect(&client, client_id, NULL, NULL, 0, NULL, NULL, connect_flags, 400);

    if (ret != MQTT_OK)
    {
        printf("fail \r\n");
    }
    /* check that we don't have any errors */
    if (client.error != MQTT_OK) {
        printf("error: %s\r\n", mqtt_error_str(client.error));
        test_close(SHELL_SIGINT);
    }

    printf("%s is ready to begin publishing hello.\r\n", argv[0]);

    while(1) {
        /* print a message */
        char application_message[256] = {"{\"hello mqtt !\"}\r\n"};
        printf("%s published : \"%s\"\r\n", argv[0], application_message);

        mqtt_publish(&client, topic, application_message, strlen(application_message) + 1, MQTT_PUBLISH_QOS_0);
        mqtt_sync(&client);

        /* check for errors */
        if (client.error != MQTT_OK) {
            printf("error: %s\r\n", mqtt_error_str(client.error));
            test_close(SHELL_SIGINT);
        }
        vTaskDelay(3000);
    }

    /* disconnect */
    /* exit */
    test_close(SHELL_SIGINT);

}

static void publish_callback_1(void** unused, struct mqtt_response_publish *published)
{
    /* not used in this example */
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

static int cmd_mqtts_publisher(int argc, const char **argv)
{
    uint32_t ret = 0;

    ret = check_wifi_state();
    if (ret != 0) {
        printf("your wifi not connected!\r\n");
        return 0;
    }

    // xTaskCreate(example_mqtt,(char*)"test_mqtt", 8192, argv, 10, NULL);
    example_mqtt(argc, argv);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_mqtts_publisher, mqtts_pub, mqtts publisher);
#endif
