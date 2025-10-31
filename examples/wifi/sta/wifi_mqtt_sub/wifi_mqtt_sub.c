
/**
 * @file
 * A simple program that subscribes to a topic.
 */
#include "FreeRTOS_POSIX.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "utils_getopt.h"

#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/debug.h>
#include <mbedtls/x509.h>


#include "mqtt.h"
#include "shell.h"

uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */

shell_sig_func_ptr abort_exec;
static TaskHandle_t client_daemon;
mqtt_pal_socket_handle test_sockfd;
static const char* Addr;
static const char* Port;
static const char* Topic;

struct mbedtls_context {
    mbedtls_net_context net_ctx;
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_conf;
    mbedtls_x509_crt ca_crt;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
};
struct mbedtls_context tls_ctx;

static const char *test_ca_crt = {
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDwzCCAqugAwIBAgIUT2Cy6wsGGycw6WvNoHot5I+GlTQwDQYJKoZIhvcNAQEL\r\n"
    "BQAwcTELMAkGA1UEBhMCQ04xEDAOBgNVBAgMB0JlaWppbmcxEDAOBgNVBAcMB0Jl\r\n"
    "aWppbmcxDzANBgNVBAoMBk15IE9yZzEWMBQGA1UECwwNQ0EgRGVwYXJ0bWVudDEV\r\n"
    "MBMGA1UEAwwMTVFUVCBSb290IENBMB4XDTI1MDcxNzEwMzU1MVoXDTM1MDcxNTEw\r\n"
    "MzU1MVowcTELMAkGA1UEBhMCQ04xEDAOBgNVBAgMB0JlaWppbmcxEDAOBgNVBAcM\r\n"
    "B0JlaWppbmcxDzANBgNVBAoMBk15IE9yZzEWMBQGA1UECwwNQ0EgRGVwYXJ0bWVu\r\n"
    "dDEVMBMGA1UEAwwMTVFUVCBSb290IENBMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\r\n"
    "MIIBCgKCAQEAzMguxj26xEFb1N5u6slSgEp62eak+dqkmq6BoxVW48V2GO62caCt\r\n"
    "n7MZywknnKm1sFM9kOv/mdr4fjHF3ZhvZ4MCqu0EPy2crjROoV8ofsjMDoIp2PSh\r\n"
    "VZ7wmDC5zu7ndCMX85C/vv3Ikows5sMr/r6Ieh7lgoLeXTFDbGixLM3u1vB2I/OQ\r\n"
    "gepYnPVkUHmEWsRFtzwwBgbAkSlmf3UreCT/lDbcOsKGl3YY4PZtJjk3wwtJqLAE\r\n"
    "M/dNJdleU9ENwwelotScu4CGex+OaPNfn1EwLgwBrX2FByYyfpw+DUTnhdXsPbGM\r\n"
    "jly24NUcg7CavTJxow5ltuWLahN5oQZZ4wIDAQABo1MwUTAdBgNVHQ4EFgQUGIBW\r\n"
    "MZwM5z40nEZde/GSQxszsDEwHwYDVR0jBBgwFoAUGIBWMZwM5z40nEZde/GSQxsz\r\n"
    "sDEwDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEALp0qj9qWnc9l\r\n"
    "9GWivdOHVSfIHWWuTUN7+2QXGEHZ7vyC6dPoMNvr0Wg35Wj3VLbmZm6vlIubQ0JW\r\n"
    "3nt5Kru9Keh/BALAxFa6onRVdYahXWRzD1fjTMUfIpDjaRtFWE6luA3ztfj2ZtiL\r\n"
    "q665MZulKQLFhIQeAXNfR7hpxaiR+qu5YVYvWQN7gigjB787Y+JUk+h3rnlZ5vPX\r\n"
    "j/yf9BP2u9tK1VkF2Ir1c+Y0xcp0qomd25QOm0H/qwgEh5wnO3du8GrTS5WMlYq1\r\n"
    "LJSLM7dmBKgvTPngAgewFiwNPyhtywM6Vdc/mrcNwqAcfmyV0U8oU/DReKbX4ed2\r\n"
    "+zlke05LKA==\r\n"
    "-----END CERTIFICATE-----\r\n"
};

void my_mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str){
    printf("%s:%04d: %s", file, line, str);
}

static void cert_verify_failed(uint32_t rv)
{
    char buf[512];
    mbedtls_x509_crt_verify_info(buf, sizeof(buf), "\t", rv);
    printf("Certificate verification failed (%0" PRIx32 ") \r\n%s\r\n", rv, buf);
}

static int initserver_tls(const char *Certificate, struct mbedtls_context *ctx)
{
    const unsigned char *additional = (const unsigned char *)"MQTT-C";
    size_t additional_len = 6;
    int rv;
    int crt_len;
    char buf[50];

    mbedtls_net_context *net_ctx = &ctx->net_ctx;
    mbedtls_ssl_context *ssl_ctx = &ctx->ssl_ctx;
    mbedtls_ssl_config *ssl_conf = &ctx->ssl_conf;
    mbedtls_x509_crt *ca_crt = &ctx->ca_crt;
    mbedtls_entropy_context *entropy = &ctx->entropy;
    mbedtls_ctr_drbg_context *ctr_drbg = &ctx->ctr_drbg;

    mbedtls_ssl_conf_dbg(ssl_conf, my_mbedtls_debug, stdout);
    mbedtls_debug_set_threshold(4);

    printf("TLS Context Ininting ...\r\n");

    //Init radom number seed init
    //Entropy source initialization
    mbedtls_entropy_init(entropy);
    //Random number generator initialization
    mbedtls_ctr_drbg_init(ctr_drbg);
    //Populate the random number generator with entropy sources and other seeds
    rv = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, entropy,
                                additional, additional_len);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ctr_drbg_seed failed with %x (%s)\n", -rv, buf);
        return -1;
    }


    //Init certifaca
    mbedtls_x509_crt_init(ca_crt);
    crt_len = strlen(Certificate);
    //Parse the certificate and populate it into the CA certificate (ca_crt)
    rv = mbedtls_x509_crt_parse(ca_crt, (unsigned char *)Certificate, crt_len+1);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_x509_crt_parse_file failed with %x (%s)\n", -rv, buf);
        return -1;
    }

    //SSL/TLS init
    mbedtls_ssl_config_init(ssl_conf);
    rv = mbedtls_ssl_config_defaults(ssl_conf, MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_min_version(ssl_conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ssl_config_defaults failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    //Set the certificate chain for the server to verify the previously initialized CA certificate
    mbedtls_ssl_conf_ca_chain(ssl_conf, ca_crt, NULL);
    //Configure certificate verification mode
    mbedtls_ssl_conf_authmode(ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    //Bind the previously initialized random number generator
    mbedtls_ssl_conf_rng(ssl_conf, mbedtls_ctr_drbg_random, ctr_drbg);


    //Connection init
    //Initialize network context
    mbedtls_net_init(net_ctx);
    printf("Start tcp connection\r\n");
    //Establish a TCP network layer connection
    rv = mbedtls_net_connect(net_ctx, Addr, Port, MBEDTLS_NET_PROTO_TCP);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_net_connect failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    //Set the socket to non-blocking mode
    rv = mbedtls_net_set_nonblock(net_ctx);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_net_set_nonblock failed with %x (%s)\n", -rv, buf);
        return -1;
    }

    mbedtls_ssl_init(ssl_ctx);
    // Application configuration
    rv = mbedtls_ssl_setup(ssl_ctx, ssl_conf);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ssl_setup failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    // Set the server name to NULL; host name verification is not performed here.
    rv = mbedtls_ssl_set_hostname(ssl_ctx, NULL);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ssl_set_hostname failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    // Set the underlying I/O functions
    mbedtls_ssl_set_bio(ssl_ctx,
                        net_ctx,
                        mbedtls_net_send,
                        mbedtls_net_recv,
                        NULL);

    //Connection establish
    printf("Start Handshake\r\n");
    // TLS connection handshake
    for (;;) {
        rv = mbedtls_ssl_handshake(ssl_ctx);
        uint32_t want = 0;
        if (rv == MBEDTLS_ERR_SSL_WANT_READ) {
            want |= MBEDTLS_NET_POLL_READ;
        } else if (rv == MBEDTLS_ERR_SSL_WANT_WRITE) {
            want |= MBEDTLS_NET_POLL_WRITE;
        } else {
            break;
        }
        rv = mbedtls_net_poll(net_ctx, want, (uint32_t)-1);
        if (rv < 0) {
            mbedtls_strerror(rv, buf, sizeof(buf));
            printf("mbedtls_net_poll failed with %x (%s)\n", -rv, buf);
            return -1;
        }
    }

    uint32_t result = mbedtls_ssl_get_verify_result(ssl_ctx);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ssl_handshake failed with %x (%s)\r\n", -rv, buf);

        if (result == (uint32_t)-1) {
            mbedtls_strerror(rv, buf, sizeof(buf));
            printf("mbedtls_ssl_get_verify_result failed with %x (%s)\n\r", -rv, buf);
            return -1;
        } else {
            cert_verify_failed(result);
        }
        return -1;
    } else {
    printf("HandShake OK\r\n");
    }

    printf("TLS version: %s\n\r", mbedtls_ssl_get_version(ssl_ctx));
    printf("Cipher: %s\n\r", mbedtls_ssl_get_ciphersuite(ssl_ctx));
    return 0;
}

void closeserver_tls(struct mbedtls_context *ctx)
{
    int rv = 0;
    if (ctx == NULL)
        return;

    do {
        rv = mbedtls_ssl_close_notify(&(ctx->ssl_ctx));
    } while (rv == MBEDTLS_ERR_SSL_WANT_READ || rv == MBEDTLS_ERR_SSL_WANT_WRITE);

    mbedtls_net_free(&(ctx->net_ctx));
    mbedtls_x509_crt_free(&(ctx->ca_crt));
    mbedtls_ssl_free(&(ctx->ssl_ctx));
    mbedtls_ssl_config_free(&(ctx->ssl_conf));
    mbedtls_ctr_drbg_free(&(ctx->ctr_drbg));
    mbedtls_entropy_free(&(ctx->entropy));
}


/*
    A template for opening a non-blocking POSIX socket.
*/
static int open_nb_socket(const char* addr, const char* port);

static int open_nb_socket(const char* addr, const char* port) {
    struct addrinfo hints = {0};

    hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
    int sockfd = -1;
    int rv;
    struct addrinfo *p, *servinfo;

    /* get address information */
    rv = getaddrinfo(addr, port, &hints, &servinfo);
    if(rv != 0) {
        printf("Failed to open socket (getaddrinfo): %s\r\n", rv);
        return -1;
    }

    /* open the first possible socket */
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) continue;

        /* connect to server */
        rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if(rv == -1) {
          close(sockfd);
          sockfd = -1;
          continue;
        }
        break;
    }

    /* free servinfo */
    freeaddrinfo(servinfo);

    /* make non-blocking */
    if (sockfd != -1) {
        int iMode = 1;
        ioctlsocket(sockfd, FIONBIO, &iMode);
    }

    return sockfd;
}

/**
 * @brief The function will be called whenever a PUBLISH message is received.
 */
static void publish_callback_1(void** unused, struct mqtt_response_publish *published);

/**
 * @brief The client's refresher. This function triggers back-end routines to
 *        handle ingress/egress traffic to the broker.
 *
 * @note All this function needs to do is call \ref __mqtt_recv and
 *       \ref __mqtt_send every so often. I've picked 100 ms meaning that
 *       client ingress/egress traffic will be handled every 100 ms.
 */
static void client_refresher(void* client);

/**
 * @brief Safelty closes the \p sockfd and cancels the \p client_daemon before \c exit.
 */
static void test_close(int sig)
{


    #ifdef MQTT_USE_MBEDTLS
        closeserver_tls(&tls_ctx);
    #else
        if (test_sockfd)
        {
            close(test_sockfd);
        }
    #endif

    printf("mqtt_sub disconnecting from %s\r\n", Addr);

    abort_exec(sig);

    vTaskDelete(client_daemon);

}

static void  example_mqtt(const void *argv[])
{


    int ret = 0;
    // int argc = 0;

    abort_exec = shell_signal(1, test_close);


    /* open the non-blocking TCP socket (connecting to the broker) */
    printf("Addr:%s  Port:%s  Topic:%s \r\n",Addr,Port,Topic);

        #ifdef MQTT_USE_MBEDTLS
        // Creation and configuration of TLS connection

        ret = initserver_tls(test_ca_crt, &tls_ctx);
        if (ret != 0) {
            printf("Failed to initialize TLS connection\r\n");
            test_close(SHELL_SIGINT);
        }
        test_sockfd = &tls_ctx.ssl_ctx;
        // test_sockfd = tls_ctx.net_ctx.fd;  // Get the file descriptor of the TLS connection
        if (tls_ctx.net_ctx.fd < 0) {
            printf("Failed to open socket: %d\r\n", tls_ctx.net_ctx.fd);
            test_close(SHELL_SIGINT);
        }
    #else
        // Ordinary TCP connection
        test_sockfd = open_nb_socket(Addr, Port);
        if (test_sockfd < 0) {
            printf("Failed to open socket: %d\r\n", test_sockfd);
            test_close(SHELL_SIGINT);
        }
    #endif

    /* setup a client */
    struct mqtt_client client;

    mqtt_init(&client, test_sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback_1);
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

    /* start a thread to refresh the client (handle egress and ingree client traffic) */
    xTaskCreate(client_refresher, (char*)"client_ref", 1024,  &client, 10, &client_daemon);

    /* subscribe */
    mqtt_subscribe(&client, Topic, 0);

    /* start publishing the time */
    printf("%s listening for '%s' messages.\r\n", argv[0], Topic);
    printf("Press CTRL-C to exit.\r\n");

    /* block wait CTRL-C exit */
    while(1) {
        vTaskDelay(100);
    }

    /* disconnect */
    /* exit */
    test_close(SHELL_SIGINT);

}

static void publish_callback_1(void** unused, struct mqtt_response_publish *published)
{
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char* topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    char* topic_msg = (char*) malloc(published->application_message_size + 1);
    memcpy(topic_msg, published->application_message, published->application_message_size);
    topic_msg[published->application_message_size] = '\0';

    printf("Received publish('%s'): %s\r\n", topic_name, topic_msg);

    free(topic_name);
    free(topic_msg);
}

static void client_refresher(void* client)
{
    while(1)
    {
        mqtt_sync((struct mqtt_client*) client);
        vTaskDelay(100);
    }

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

int cmd_mqtt_subscribe(int argc, const char **argv)
{
    uint32_t ret = 0;

    ret = check_wifi_state();
    if (ret != 0) {
        printf("your wifi not connected!\r\n");
        return 0;
    }

            /* get address (argv[1] if present) */
    if (argc > 1) {

        Addr = strdup(argv[1]);

    } else {
        Addr = "test.mosquitto.org";
    }
    /* get port number (argv[2] if present) */
    if (argc > 2) {

        Port = strdup(argv[2]);

    } else {
        Port = "1883";
    }

    /* get the topic name to publish */
    if (argc > 3) {

        Topic = strdup(argv[3]);

    } else {
        Topic = "bl618";
    }
    xTaskCreate(example_mqtt,(char*)"test_mqtt", 8192, argv, 10, NULL);
    // example_mqtt(argc, argv);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_mqtt_subscribe, mqtt_sub, mqtt subscribe);
#endif
