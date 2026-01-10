/**
 * @file
 * A simple program that subscribes to a topic.
 */
#include "FreeRTOS_POSIX.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "utils_getopt.h"

#include "mqtt.h"
#include "shell.h"
#include "aws_iot.h"

uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */

shell_sig_func_ptr abort_exec;
static TaskHandle_t client_daemon;
mqtt_pal_socket_handle test_sockfd;
static const char *Addr;
static char Port[16];
static const char *Topic;
static struct mqtt_client client = { 0 };
bool connected = false;

/* Device certificate */
static const char DEVICE_CERT[] = "-----BEGIN CERTIFICATE-----\n"
    #warning "put your device cert here."
                                  "-----END CERTIFICATE-----\n";

/* Device private key */
static const char DEVICE_PRIVATE_KEY[] = "-----BEGIN RSA PRIVATE KEY-----\n"
    #warning "put your device private key here."
                                         "-----END RSA PRIVATE KEY-----\n";

/* AWS root CA certificate */
static const char ROOT_CA[] = "-----BEGIN CERTIFICATE-----\n"
                              "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
                              "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
                              "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
                              "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
                              "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
                              "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
                              "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
                              "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
                              "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
                              "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
                              "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
                              "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
                              "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
                              "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
                              "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
                              "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
                              "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
                              "rqXRfboQnoZsG4q5WTP468SQvvG5\n"
                              "-----END CERTIFICATE-----\n";

void my_mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    /* Only print errors (level 0-1) to avoid too much output */
    if (level <= 1) {
        printf("mbedTLS: %s:%04d: %s", file, line, str);
    }
}

static void cert_verify_failed(uint32_t rv)
{
    char buf[512];
    mbedtls_x509_crt_verify_info(buf, sizeof(buf), "\t", rv);
    printf("Certificate verification failed (%0" PRIx32 ") \r\n%s\r\n", rv, buf);
}

static int initserver_tls(struct mbedtls_context *ctx)
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
    mbedtls_x509_crt *client_crt = &ctx->client_crt;
    mbedtls_pk_context *client_key = &ctx->client_key;
    mbedtls_entropy_context *entropy = &ctx->entropy;
    mbedtls_ctr_drbg_context *ctr_drbg = &ctx->ctr_drbg;

    mbedtls_ssl_conf_dbg(ssl_conf, my_mbedtls_debug, stdout);
    mbedtls_debug_set_threshold(4);

    printf("TLS Context Initializing ...\r\n");

    /* Init random number seed init */
    /* Entropy source initialization */
    mbedtls_entropy_init(entropy);
    /* Random number generator initialization */
    mbedtls_ctr_drbg_init(ctr_drbg);
    /* Populate the random number generator with entropy sources and other seeds */
    rv = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, entropy, additional, additional_len);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ctr_drbg_seed failed with %x (%s)\n", -rv, buf);
        return -1;
    }

    /* Init certificate */
    mbedtls_x509_crt_init(ca_crt);
    crt_len = strlen((char *)DEVICE_CERT);
    /* Parse the certificate and populate it into the CA certificate (ca_crt) */
    rv = mbedtls_x509_crt_parse(client_crt, (unsigned char *)DEVICE_CERT, crt_len + 1);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_x509_crt_parse_file failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    crt_len = strlen((char *)ROOT_CA);
    rv = mbedtls_x509_crt_parse(ca_crt, (unsigned char *)ROOT_CA, crt_len + 1);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_x509_crt_parse_file failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    rv = mbedtls_pk_parse_key(client_key, (unsigned char *)DEVICE_PRIVATE_KEY, strlen((char *)DEVICE_PRIVATE_KEY) + 1,
                              NULL, 0);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_pk_parse_keyfile failed with %x (%s)\n", -rv, buf);
        return -1;
    }

    /* SSL/TLS init */
    mbedtls_ssl_config_init(ssl_conf);
    rv = mbedtls_ssl_config_defaults(ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                     MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_min_version(ssl_conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ssl_config_defaults failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    /* Set the certificate chain for the server to verify the previously initialized CA certificate */
    mbedtls_ssl_conf_ca_chain(ssl_conf, ca_crt, NULL);
    /* Set the client certificate and private key for the client to authenticate itself to the server */
    mbedtls_ssl_conf_own_cert(ssl_conf, client_crt, client_key);
    /* Configure certificate verification mode */
    mbedtls_ssl_conf_authmode(ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    /* Bind the previously initialized random number generator */
    mbedtls_ssl_conf_rng(ssl_conf, mbedtls_ctr_drbg_random, ctr_drbg);

    /* Connection init */
    /* Initialize network context */
    mbedtls_net_init(net_ctx);
    printf("Start tcp connection\r\n");
    /* Establish a TCP network layer connection */
    rv = mbedtls_net_connect(net_ctx, Addr, Port, MBEDTLS_NET_PROTO_TCP);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_net_connect failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    /* Set the socket to non-blocking mode */
    rv = mbedtls_net_set_nonblock(net_ctx);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_net_set_nonblock failed with %x (%s)\n", -rv, buf);
        return -1;
    }

    mbedtls_ssl_init(ssl_ctx);
    /* Application configuration */
    rv = mbedtls_ssl_setup(ssl_ctx, ssl_conf);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ssl_setup failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    /* Set the server name to NULL; host name verification is not performed here. */
    rv = mbedtls_ssl_set_hostname(ssl_ctx, NULL);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ssl_set_hostname failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    /* Set the underlying I/O functions */
    mbedtls_ssl_set_bio(ssl_ctx, net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);

    /* Connection establish */
    printf("Start Handshake\r\n");
    /* TLS connection handshake */
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

/**
 * @brief Safelty closes the \p sockfd and cancels the \p client_daemon before \c exit.
 */
static void test_close(int sig)
{
    closeserver_tls(&tls_ctx);
    if (client.socketfd != 0) {
        if (mqtt_disconnect(&client) == MQTT_OK) {
            printf("Device successfully disconnected from %s\r\n", Addr);
        }
    }
    abort_exec(sig);
    connected = false;
    vTaskDelete(client_daemon);
}

static void aws_main_start(void *arg)
{
    int ret = 0;
    abort_exec = shell_signal(1, test_close);

    /* Parameter init */
    printf("Connecting to AWS IoT Core...\r\n");
    struct hostent *host = lwip_gethostbyname(AWS_IOT_ENDPOINT);
    if (host) {
        Addr = inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);
    } else {
        printf("Failed to resolve hostname.\r\n");
        test_close(SHELL_SIGINT);
        return;
    }
    Topic = AWS_TOPIC;
    itoa(AWS_IOT_PORT, Port, 10);
    printf("Addr:%s  Port: %s  Topic:%s \r\n", Addr, Port, Topic);

    /* Creation and configuration of TLS connection */
    ret = initserver_tls(&tls_ctx);
    if (ret != 0) {
        printf("Failed to initialize TLS connection\r\n");
        test_close(SHELL_SIGINT);
        return;
    }
    /* Socket handle for MQTT is the mbedtls_ssl_context pointer */
    test_sockfd = (mqtt_pal_socket_handle)&tls_ctx.ssl_ctx;

    /* Setup a client */
    mqtt_init(&client, test_sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), aws_receive);
    printf("MQTT client initialized\r\n");
    /* Send connection request to the broker. */
    ret = mqtt_connect(&client, AWS_CLIENT_ID, AWS_WILL_TOPIC, AWS_WILL_MSG_DISCONNECTED, 0, NULL, NULL,
                       MQTT_CONNECT_WILL_FLAG, 400);
    if (ret != MQTT_OK) {
        printf("error: %s\r\n", mqtt_error_str(client.error));
        test_close(SHELL_SIGINT);
        return;
    }
    mqtt_publish(&client, AWS_WILL_TOPIC, AWS_WILL_MSG_CONNECTED, strlen(AWS_WILL_MSG_CONNECTED) + 1,
                 MQTT_PUBLISH_QOS_0);
    printf("MQTT connect keepalive: 400 seconds\r\n");
    /* Check that we don't have any errors */
    if (client.error != MQTT_OK) {
        printf("error: %s\r\n", mqtt_error_str(client.error));
        test_close(SHELL_SIGINT);
        return;
    }
    connected = true;

    /* Start a thread to refresh the client (handle egress and ingress client traffic) */
    xTaskCreate(client_refresher, (char *)"client_ref", 1024, &client, 10, &client_daemon);

    /* Subscribe to the topic first */
    mqtt_subscribe(&client, Topic, 0);
    uint8_t rv;
    uint8_t count = 0;
    while (connected) {
        char application_message[256] = { "{\"hello mqtt by bl616/8 !\"}\r\n" };

        /* Publish the time with QoS 0 to avoid complex handshake */
        rv = mqtt_publish(&client, Topic, application_message, strlen(application_message) + 1, MQTT_PUBLISH_QOS_0);
        if (rv == MQTT_OK) {
            printf("%s published : \"%s\"\r\n", Addr, application_message);
        } else {
            count++;
            if (count > 5) {
                printf("Too many publish failures, exiting...\r\n");
                test_close(SHELL_SIGINT);
                return;
            }
            printf("Failed to publish message: %d\r\n", rv);
            printf("error: %s\r\n", mqtt_error_str(client.error));
        }
        vTaskDelay(10000);
    }
    vTaskDelete(NULL);
}

static void aws_receive(void **unused, struct mqtt_response_publish *published)
{
    /* Note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char *topic_name = (char *)malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    char *topic_msg = (char *)malloc(published->application_message_size + 1);
    memcpy(topic_msg, published->application_message, published->application_message_size);
    topic_msg[published->application_message_size] = '\0';

    printf("Received publish('%s'): %s\r\n", topic_name, topic_msg);

    free(topic_name);
    free(topic_msg);
}

static void client_refresher(void *client)
{
    struct mqtt_client *mqtt_client = (struct mqtt_client *)client;
    uint32_t counter = 0;
    while (1) {
        int ret = mqtt_sync(mqtt_client);
        if (ret != MQTT_OK) {
            printf("MQTT sync error #%d: %s\r\n", counter, mqtt_error_str(mqtt_client->error));
        } else {
            /* Print heartbeat every 10 seconds */
            if ((counter % 100) == 0) {
                printf("MQTT heartbeat #%d OK\r\n", counter / 100);
            }
        }
        counter++;
        vTaskDelay(100);
    }
}

#ifdef CONFIG_SHELL
#include <shell.h>

extern uint32_t wifi_state;
static int check_wifi_state(void)
{
    if (wifi_state == 1) {
        return 0;
    } else {
        return 1;
    }
}

int cmd_aws_test(int argc, const char **argv)
{
    uint32_t ret = 0;

    ret = check_wifi_state();
    if (ret != 0) {
        printf("your wifi not connected!\r\n");
        return 0;
    }

    xTaskCreate(aws_main_start, (char *)"test_aws", 8192, argv, 10, NULL);
    /* example_mqtt(argc, argv); */

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_aws_test, aws_test, aws test);
#endif
