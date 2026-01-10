#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/debug.h>
#include <mbedtls/x509.h>

/* Use the authorized client ID for AWS IoT (must match policy) */
#define AWS_IOT_ENDPOINT "a1bbl4jnp9a7wi-ats.iot.us-east-1.amazonaws.com"
#define AWS_IOT_PORT     8883
#define AWS_CLIENT_ID    "test-thing-bl616-8"
#define AWS_TOPIC        "test/topic"

#define AWS_WILL_TOPIC   "device/status"
#define AWS_WILL_MSG_CONNECTED \
    "{\"state\": {\"reported\": {\"welcome\": \"aws-iot\",\"temperture\": \"2222\",\"connect\": \"false\"}}}"
#define AWS_WILL_MSG_DISCONNECTED \
    "{\"state\": {\"reported\": {\"welcome\": \"aws-iot\",\"temperture\": \"3333\",\"connect\": \"false\"}}}"
// #define SUBSCRIBE_TOPIC "sdk/test/python"

struct mbedtls_context {
    mbedtls_net_context net_ctx;
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_conf;

    mbedtls_x509_crt ca_crt;
    mbedtls_x509_crt client_crt;
    mbedtls_pk_context client_key;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
};
struct mbedtls_context tls_ctx;

/**
 * @brief The function will be called whenever a PUBLISH message is received.
 */
static void aws_receive(void **unused, struct mqtt_response_publish *published);

/**
 * @brief The client's refresher. This function triggers back-end routines to
 *        handle ingress/egress traffic to the broker.
 *
 * @note All this function needs to do is call \ref __mqtt_recv and
 *       \ref __mqtt_send every so often. I've picked 100 ms meaning that
 *       client ingress/egress traffic will be handled every 100 ms.
 */
static void client_refresher(void *client);
