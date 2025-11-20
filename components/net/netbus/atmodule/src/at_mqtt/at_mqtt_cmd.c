/**
  ******************************************************************************
  * @file    at_mqtt_cmd.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "at_main.h"
#include "at_core.h"
#include "mqtt.h"
#include "lwip/netdb.h"
#include "at_mqtt_config.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else 
#include MBEDTLS_CONFIG_FILE
#endif 

#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/net.h"
#include "at_net_ssl.h"
#include "at_fs.h"

#define AT_MQTT_LINK_MAX      (1)
#define AT_MQTT_ALPN_MAX      (6)
#define AT_MQTT_SUB_TOPIC_MAX (32)

#define AT_MQTT_STATE_NOT_INIT           0
#define AT_MQTT_STATE_USERCFG            1
#define AT_MQTT_STATE_CONNCFG            2
#define AT_MQTT_STATE_DISCONNECT         3
#define AT_MQTT_STATE_CONNECTED          4
#define AT_MQTT_STATE_CONNECTED_NO_TOPIC 5
#define AT_MQTT_STATE_CONNECTED_TOPIC    6

#define AT_MQTT_OVER_TCP                 0
#define AT_MQTT_OVER_TLS_NOT_AUTH        1
#define AT_MQTT_OVER_TLS_SERVER_AUTH     2
#define AT_MQTT_OVER_TLS_CLIENT_AUTH     3
#define AT_MQTT_OVER_TLS_BOTH_AUTH       4

#define AT_MQTT_CLIENTID_MAX_LEN         23
#define AT_MQTT_USERNAME_MAX_LEN         128
#define AT_MQTT_PASSWD_MAX_LEN           128
#define AT_MQTT_BNUFFER_SIZE_MAX         (3*1024)
#define _at_offset(type, member) ((size_t) &((type *)0)->member)

#define _at_container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - _at_offset(type,member)))

typedef struct at_mqtt {
    struct custom_socket_handle sockfd;
    ssl_param_t *ctx;

    uint8_t scheme;
    uint8_t reconnect;
    uint8_t state;
    uint8_t refresher_run;
    struct mqtt_client client;
    TaskHandle_t client_task;

    char *ssl_hostname;
    char *ssl_alpn[AT_MQTT_ALPN_MAX];
    int ssl_alpn_num;

    char *client_id;
    char *user_name;
    char *password;
    uint16_t keepalive;
    uint16_t connect_flags;
    char will_topic[128];
    char will_message[128];
    char topic[128];
    char remote_host[128];
    char remote_port[8];
    uint8_t *p_sendbuf;
    uint8_t *p_recvbuf;

    struct {
        char *topic;
        uint8_t qos;
    } sub_topic[AT_MQTT_SUB_TOPIC_MAX];

} at_mqtt_t;

static __attribute__((section(".wifi_ram."))) at_mqtt_t g_at_mqtt[AT_MQTT_LINK_MAX];

static int at_ssl_sni_set(int linkid, const char *sni)
{
    if (g_at_mqtt[linkid].ssl_hostname) {
        at_free(g_at_mqtt[linkid].ssl_hostname);
    }
    g_at_mqtt[linkid].ssl_hostname = strdup(sni);
    if (g_at_mqtt[linkid].ssl_hostname == NULL) {
        return -1;
    }
    return 0;
}

static char *at_ssl_sni_get(int linkid)
{
    return g_at_mqtt[linkid].ssl_hostname;
}

static int at_ssl_alpn_set(int linkid, int alpn_num, const char *alpn)
{
    if (alpn == NULL) {
        at_free(g_at_mqtt[linkid].ssl_alpn[alpn_num]);
        g_at_mqtt[linkid].ssl_alpn[alpn_num] = NULL;
    } else {
        if (g_at_mqtt[linkid].ssl_alpn[alpn_num]) {
            at_free(g_at_mqtt[linkid].ssl_alpn[alpn_num]);
        }
        g_at_mqtt[linkid].ssl_alpn[alpn_num] = strdup(alpn);
        if (g_at_mqtt[linkid].ssl_alpn[alpn_num] == NULL) {
            return -1;
        }
    }

    g_at_mqtt[linkid].ssl_alpn_num = 0;
    for (int i = 0; i < AT_MQTT_ALPN_MAX; i++) {
        if (g_at_mqtt[linkid].ssl_alpn[i]) {
            g_at_mqtt[linkid].ssl_alpn_num++;
        }
    }
    return 0;
}

static char **at_ssl_alpn_get(int linkid, int *alpn_num)
{
    if (alpn_num) {
        *alpn_num = g_at_mqtt[linkid].ssl_alpn_num;
    }
    return g_at_mqtt[linkid].ssl_alpn;
}

static int initserver_tcp(const char* addr, const char* port)
{
    int sockfd;
    struct addrinfo hints = {0};

    hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
    int rv;
    struct addrinfo *p, *servinfo;

    /* get address information */
    rv = getaddrinfo(addr, port, &hints, &servinfo);
    if(rv != 0) {
        printf("Failed to open socket (getaddrinfo): %d\r\n", rv);
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

static int initserver_tls(ssl_param_t **ctx, const char* addr, const char* port, const char *sni, const char **alpn, int alpn_num)
{
    int rv = 0;
    mbedtls_net_context net_ctx;
    ssl_conn_param_t ssl_param = {0};

    net_ctx.fd = initserver_tcp(addr, port);
    if (net_ctx.fd < 0) {
        printf("TCP connect fail:%d\r\n", net_ctx.fd);
        return -1;
    }

    rv = mbedtls_net_set_block(&net_ctx);
    if (rv != 0) {
        return -1;
    }

    at_load_file(at_mqtt_config->ssl_conf.ca_file, &ssl_param.ca_cert, &ssl_param.ca_cert_len);
    at_load_file(at_mqtt_config->ssl_conf.cert_file, &ssl_param.own_cert, &ssl_param.own_cert_len);
    at_load_file(at_mqtt_config->ssl_conf.key_file, &ssl_param.private_cert, &ssl_param.private_cert_len);
    ssl_param.alpn = (char **)alpn;
    ssl_param.alpn_num = alpn_num;
    ssl_param.sni = (char *)sni;
    printf("ca_cert:%p, own_cert:%p private_key:%p alpn:%p alpn_num:%d sni:%p\r\n", 
           ssl_param.ca_cert, ssl_param.own_cert, ssl_param.private_cert, 
           ssl_param.alpn, ssl_param.alpn_num, ssl_param.sni);

    *ctx = mbedtls_ssl_connect(net_ctx.fd, &ssl_param);

    if (*ctx == NULL) {
        printf("mbedtls_ssl_connect handle NULL, fd:%d\r\n", net_ctx.fd);
        close(net_ctx.fd);
        net_ctx.fd = -1;
    }

    rv = mbedtls_net_set_nonblock(&net_ctx);
    if (rv != 0) {
        close(net_ctx.fd);
        net_ctx.fd = -1;
    }
    //mbedtls_ssl_conf_read_timeout(&(*ctx)->conf, 200);

    at_free(ssl_param.ca_cert);
    at_free(ssl_param.own_cert);
    at_free(ssl_param.private_cert);

    return net_ctx.fd;
}

static int open_nb_socket(struct custom_socket_handle *sockfd, int linkid, uint8_t type, const char* addr, const char* port) 
{
    int ret = -1;

    switch (type)
    {
      case MQTTC_PAL_CONNTION_TYPE_TCP:
        ret = initserver_tcp(addr, port);
        sockfd->ctx.fd = ret;
        printf("TCP fd %d\r\n", sockfd->ctx.fd);
        break;

      case MQTTC_PAL_CONNTION_TYPE_TLS:
        ret = initserver_tls(&g_at_mqtt[linkid].ctx, addr, port, 
                                       (const char *)at_ssl_sni_get(linkid), 
                                       (const char **)g_at_mqtt[linkid].ssl_alpn, 
                                        g_at_mqtt[linkid].ssl_alpn_num);
        if (ret > 0) {
            sockfd->ctx.ssl_ctx = &g_at_mqtt[linkid].ctx->ssl;
        } else {
            sockfd->ctx.ssl_ctx = NULL;
        }
        printf("TLS %p\r\n", sockfd->ctx.ssl_ctx);
        break;

      default:
        sockfd->ctx.fd = -1;
        break;
    }

    if (ret < 0) {
        printf("ERRPR! socket failed.\r\n");
        return -1;
    }

    /* Set client connection type */

    sockfd->type = type;

    return 0;
}

static void client_refresher_task(void* arg);

static void socketfd_close(mqtt_pal_socket_handle sockfd)
{
    if (sockfd->type == MQTTC_PAL_CONNTION_TYPE_TCP && sockfd->ctx.fd > 0) {
        close(sockfd->ctx.fd);
        sockfd->ctx.fd = 0;
    }

    if (sockfd->type == MQTTC_PAL_CONNTION_TYPE_TLS && sockfd->ctx.ssl_ctx) {
        ssl_param_t *ssl_param = (ssl_param_t *)_at_container_of(sockfd->ctx.ssl_ctx, ssl_param_t, ssl);
        mbedtls_ssl_close(ssl_param);
        close(ssl_param->net.fd);
        ssl_param->net.fd = 0;
        sockfd->ctx.ssl_ctx = NULL;
    }
}

static uint8_t socketfd_is_connected(mqtt_pal_socket_handle sockfd)
{
    if (sockfd->type == MQTTC_PAL_CONNTION_TYPE_TCP && sockfd->ctx.fd > 0) {
        return 1;
    }

    if (sockfd->type == MQTTC_PAL_CONNTION_TYPE_TLS && sockfd->ctx.ssl_ctx) {
        return 1;
    }
    return 0;
}

static void mqtt_buf_free(at_mqtt_t *pat_mqtt)
{
    at_free(pat_mqtt->p_sendbuf);
    pat_mqtt->p_sendbuf = NULL;
    at_free(pat_mqtt->p_recvbuf);
    pat_mqtt->p_recvbuf = NULL;
}

/**
 * @brief Safelty closes the \p sockfd and cancels the \p client_daemon before \c exit.
 */
static void mqtt_close(int linkid)
{
    if (socketfd_is_connected(&g_at_mqtt[linkid].sockfd)) {
        at_response_string("+MQTT:DISCONNECTED,%d\r\n", linkid);
    }
    socketfd_close(&g_at_mqtt[linkid].sockfd);

    g_at_mqtt[linkid].state = AT_MQTT_STATE_DISCONNECT;
    
    if ((!g_at_mqtt[linkid].reconnect) && g_at_mqtt[linkid].client_task) {
        g_at_mqtt[linkid].refresher_run = 0;
    }
}

static void _mqtt_disconnect(struct mqtt_client *client)
{
    printf("mqtt disconenct\r\n");
    mqtt_close(0);
}

static void _mqtt_connected_event(int linkid)
{
    g_at_mqtt[linkid].state = AT_MQTT_STATE_CONNECTED;
    at_response_string("+MQTT:CONNECTED,%d\r\n", linkid);
}

static void client_refresher_task(void* arg)
{
    at_mqtt_t *pat_mqtt = (at_mqtt_t *)arg;

    while (pat_mqtt->refresher_run) {
        if (mqtt_sync((struct mqtt_client*)&pat_mqtt->client) != MQTT_OK) {
            if (pat_mqtt->client.error == MQTT_ERROR_SOCKET_ERROR ||
                pat_mqtt->client.error == MQTT_ERROR_CONNECTION_REFUSED) {
                printf("connect error: %s\r\n", mqtt_error_str(pat_mqtt->client.error));
                // socket disconnect ???
                _mqtt_disconnect(&pat_mqtt->client);
            }
        }
        vTaskDelay(200);
    }
    mqtt_buf_free(pat_mqtt);
    pat_mqtt->client_task = NULL;
    vTaskDelete(NULL);
}

static void reconnect_client(struct mqtt_client* client, void **reconnect_state_vptr)
{
    int ret;
    struct at_mqtt *reconnect_state = *((struct at_mqtt**) reconnect_state_vptr);

    /* Close the clients socket if this isn't the initial reconnect call */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT) {
        if (socketfd_is_connected(client->socketfd)) {
            at_response_string("+MQTT:DISCONNECTED,%d\r\n", 0);
        }
        socketfd_close(client->socketfd);

        reconnect_state->state = AT_MQTT_STATE_DISCONNECT;
    }

    /* Perform error handling here. */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT) {
        printf("reconnect_client: called while client was in error state \"%s\"\n",
               mqtt_error_str(client->error)
        );
    }

    /* Open a new socket. */
    ret = open_nb_socket(&reconnect_state->sockfd, 0, 
                     (reconnect_state->scheme == AT_MQTT_OVER_TCP)?MQTTC_PAL_CONNTION_TYPE_TCP:MQTTC_PAL_CONNTION_TYPE_TLS,
                     reconnect_state->remote_host, reconnect_state->remote_port);
    if (ret < 0) {
        perror("Failed to open socket: ");
        return;
    }
    
    /* Reinitialize the client. */
    mqtt_reinit(client, &reconnect_state->sockfd,
                reconnect_state->p_sendbuf, AT_MQTT_BNUFFER_SIZE_MAX,
                reconnect_state->p_recvbuf, AT_MQTT_BNUFFER_SIZE_MAX);

    /* Send connection request to the broker. */
    ret = mqtt_connect(&reconnect_state->client, 
                      reconnect_state->client_id, 
                      strlen(reconnect_state->will_topic)?reconnect_state->will_topic:NULL, 
                      strlen(reconnect_state->will_message)?reconnect_state->will_message:NULL, 
                      strlen(reconnect_state->will_message), 
                      reconnect_state->user_name, 
                      reconnect_state->password, 
                      reconnect_state->connect_flags, 
                      reconnect_state->keepalive);
    if (ret != MQTT_OK) {
        return; 
    }
    /* check that we don't have any errors */
    if (reconnect_state->client.error != MQTT_OK) {
        return; 
    }

    _mqtt_connected_event(0);
}

static void publish_callback_1(void** arg, struct mqtt_response_publish *published)
{
    char *buffer;
    uint32_t len;
    int offset = 0;
    int linkid = (int)(*arg);

    len = published->topic_name_size + published->application_message_size + 32;
    buffer = (char*)at_malloc(len);

    if (!buffer) {
        printf("[MQTT] Error: failed to allocate buffer\r\n");
        return;
    }

    offset = snprintf(buffer, len, "+MQTT:SUBRECV:%d,%d,%d,\"%.*s\",", 
             linkid, 
             (int)published->topic_name_size,  
             (int)published->application_message_size,
             (int)published->topic_name_size, published->topic_name); 
    if (offset <= 0 || offset >= len) {
        printf("[MQTT] Error: snprintf failed\r\n");
        at_free(buffer);
        return;
    }
    
    if (offset + published->application_message_size + 2 > len) {
        printf("[MQTT] Error: buffer too small\r\n");
        at_free(buffer);
        return;
    }
    memcpy(buffer + offset, published->application_message, published->application_message_size);
    offset += published->application_message_size;
    memcpy(buffer + offset, "\r\n", 2);
    offset += 2;

    AT_CMD_DATA_SEND(buffer, offset);

    at_free(buffer);
}


static int mqtt_linkid_valid(int linkid)
{
    return (int)(linkid >= AT_MQTT_LINK_MAX);
}

static void mqtt_unsub_all(int linkid)
{
    int i;
    for (i = 0; i < AT_MQTT_SUB_TOPIC_MAX; i++) {
        if (g_at_mqtt[linkid].sub_topic[i].topic) {
            at_free(g_at_mqtt[linkid].sub_topic[i].topic);
            g_at_mqtt[linkid].sub_topic[i].topic = NULL;
        }
    }
}

static void mqtt_handle_clear(int linkid)
{
    g_at_mqtt[linkid].reconnect = 0;
    g_at_mqtt[linkid].state = 0;
#if 0
    g_at_mqtt[linkid].scheme = 0;
    g_at_mqtt[linkid].keepalive = 0;
    g_at_mqtt[linkid].connect_flags = MQTT_CONNECT_CLEAN_SESSION;

    if (g_at_mqtt[linkid].client_id) {
        at_free(g_at_mqtt[linkid].client_id);
        g_at_mqtt[linkid].client_id = NULL;
    }

    if (g_at_mqtt[linkid].user_name) {
        at_free(g_at_mqtt[linkid].user_name);
        g_at_mqtt[linkid].user_name = NULL;
    }

    if (g_at_mqtt[linkid].password) {
        at_free(g_at_mqtt[linkid].password);
        g_at_mqtt[linkid].password = NULL;
    }

    for (int i = 0; i < AT_MQTT_SUB_TOPIC_MAX; i++) {
        if (g_at_mqtt[linkid].sub_topic[i].topic) {
            at_free(g_at_mqtt[linkid].sub_topic[i].topic);
            g_at_mqtt[linkid].sub_topic[i].topic = NULL;
        }
    }

    memset(g_at_mqtt[linkid].will_topic, 0, sizeof(g_at_mqtt[linkid].will_topic));
    memset(g_at_mqtt[linkid].will_message, 0, sizeof(g_at_mqtt[linkid].will_message));
    memset(g_at_mqtt[linkid].topic, 0, sizeof(g_at_mqtt[linkid].topic));
#endif 

    mqtt_unsub_all(linkid);
    memset(g_at_mqtt[linkid].remote_host, 0, sizeof(g_at_mqtt[linkid].remote_host));
    memset(g_at_mqtt[linkid].remote_port, 0, sizeof(g_at_mqtt[linkid].remote_port));
}

static int at_query_cmd_mqttusercfg(int argc, const char **argv)
{
    for (int i = 0; i < AT_MQTT_LINK_MAX; i++) {
        at_response_string("+MQTTUSERCFG:%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\r\n", 
                           i, 
                           g_at_mqtt[i].scheme, 
                           g_at_mqtt[i].client_id ? g_at_mqtt[i].client_id : "",
                           g_at_mqtt[i].user_name ? g_at_mqtt[i].user_name : "",
                           g_at_mqtt[i].password ? g_at_mqtt[i].password : "",
                           at_mqtt_config->ssl_conf.cert_file,
                           at_mqtt_config->ssl_conf.key_file,
                           at_mqtt_config->ssl_conf.ca_file);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttusercfg(int argc, const char **argv)
{
    int linkid;
    int scheme;
    char client_id[AT_MQTT_CLIENTID_MAX_LEN + 1] = {0};
    char user_name[AT_MQTT_USERNAME_MAX_LEN + 1] = {0};
    char password[AT_MQTT_PASSWD_MAX_LEN + 1] = {0};
    char cert_file[32] = {0};
    char key_file[32] = {0};
    char ca_file[32] = {0};
    int user_name_valid = 0, password_valid = 0, cert_file_valid = 0, key_file_valid = 0, ca_file_valid = 0;

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_NUMBER(1, &scheme);
    AT_CMD_PARSE_STRING(2, client_id, sizeof(client_id));
    AT_CMD_PARSE_OPT_STRING(3, user_name, sizeof(user_name), user_name_valid);
    AT_CMD_PARSE_OPT_STRING(4, password, sizeof(password), password_valid);
    AT_CMD_PARSE_OPT_STRING(5, cert_file, sizeof(cert_file), cert_file_valid);
    AT_CMD_PARSE_OPT_STRING(6, key_file, sizeof(key_file), key_file_valid);
    AT_CMD_PARSE_OPT_STRING(7, ca_file, sizeof(ca_file), ca_file_valid);

    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (strlen(client_id) > AT_MQTT_CLIENTID_MAX_LEN) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_LENGTH_MISMATCH);
    }

    if (strlen(user_name) > AT_MQTT_USERNAME_MAX_LEN) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_LENGTH_MISMATCH);
    }

    if (strlen(password) > AT_MQTT_PASSWD_MAX_LEN) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_LENGTH_MISMATCH);
    }
    if (scheme < AT_MQTT_OVER_TCP || scheme > AT_MQTT_OVER_TLS_BOTH_AUTH) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (g_at_mqtt[linkid].client_id) {
        at_free(g_at_mqtt[linkid].client_id);
        g_at_mqtt[linkid].client_id = NULL;
    }

    g_at_mqtt[linkid].client_id = strdup(client_id);
    if (!g_at_mqtt[linkid].client_id) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }
 
    if (user_name_valid) {
        if (g_at_mqtt[linkid].user_name) {
            at_free(g_at_mqtt[linkid].user_name);
            g_at_mqtt[linkid].user_name = NULL;
        }
        g_at_mqtt[linkid].user_name = strdup(user_name);
        if (!g_at_mqtt[linkid].user_name) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
    }
    if (password_valid) {
        if (g_at_mqtt[linkid].password) {
            at_free(g_at_mqtt[linkid].password);
            g_at_mqtt[linkid].password = NULL;
        }
        g_at_mqtt[linkid].password = strdup(password);
        if (!g_at_mqtt[linkid].password) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
    }

    if (scheme == AT_MQTT_OVER_TLS_NOT_AUTH) {
        cert_file[0] = '\0';
        key_file[0] = '\0';
        ca_file[0] = '\0';
    } else if (scheme == AT_MQTT_OVER_TLS_SERVER_AUTH) {
        if ((!ca_file_valid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        cert_file[0] = '\0';
        key_file[0] = '\0';
    } else if (scheme == AT_MQTT_OVER_TLS_CLIENT_AUTH) {
        if ((!key_file_valid) || (!cert_file_valid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
        ca_file[0] = '\0';
    } else if (scheme == AT_MQTT_OVER_TLS_BOTH_AUTH) {
        if ((!key_file_valid) || (!cert_file_valid) || (!ca_file_valid)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
    }

    strlcpy(at_mqtt_config->ssl_conf.cert_file, cert_file, sizeof(at_mqtt_config->ssl_conf.cert_file));
    strlcpy(at_mqtt_config->ssl_conf.key_file, key_file, sizeof(at_mqtt_config->ssl_conf.key_file));
    strlcpy(at_mqtt_config->ssl_conf.ca_file, ca_file, sizeof(at_mqtt_config->ssl_conf.ca_file));

    //if (at->store) {
    //    at_mqtt_config_save(AT_MQTT_SSL_CONF);
    //}

    g_at_mqtt[linkid].scheme = scheme;
    g_at_mqtt[linkid].state = AT_MQTT_STATE_USERCFG;
    
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_mqttclientid(int argc, const char **argv)
{
    for (int i = 0; i < AT_MQTT_LINK_MAX; i++) {
        at_response_string("+MQTTCLIENTID:%d,\"%s\"\r\n", 
                           i, 
                           g_at_mqtt[i].client_id ? g_at_mqtt[i].client_id : "");
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttclientid(int argc, const char **argv)
{
    int ret, linkid, length, recv_num = 0; 

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_NUMBER(1, &length);
    
    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (length < 1 || length > AT_MQTT_CLIENTID_MAX_LEN) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (g_at_mqtt[linkid].client_id) {
        at_free(g_at_mqtt[linkid].client_id);
        g_at_mqtt[linkid].client_id = NULL;
    }

    g_at_mqtt[linkid].client_id = at_malloc(length + 1);
    if (!g_at_mqtt[linkid].client_id) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    at_response_result(AT_RESULT_CODE_OK);
    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);

    while(recv_num < length) {
        recv_num += AT_CMD_DATA_RECV(g_at_mqtt[linkid].client_id + recv_num, length - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    g_at_mqtt[linkid].client_id[length] = '\0';
    if (length == recv_num) {
        ret = AT_RESULT_CODE_SEND_OK;
    } else {
        ret = AT_RESULT_CODE_SEND_FAIL;
    }

    return ret;
}

static int at_query_cmd_mqttusername(int argc, const char **argv)
{
    for (int i = 0; i < AT_MQTT_LINK_MAX; i++) {
        at_response_string("+MQTTUSERNAME:%d,\"%s\"\r\n", 
                           i, 
                           g_at_mqtt[i].user_name ? g_at_mqtt[i].user_name : "");
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttusername(int argc, const char **argv)
{
    int ret, linkid, length, recv_num = 0; 

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_NUMBER(1, &length);
    
    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (length < 1 || length > AT_MQTT_USERNAME_MAX_LEN) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (g_at_mqtt[linkid].user_name) {
        at_free(g_at_mqtt[linkid].user_name);
        g_at_mqtt[linkid].user_name = NULL;
    }

    g_at_mqtt[linkid].user_name = at_malloc(length + 1);
    if (!g_at_mqtt[linkid].user_name) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    at_response_result(AT_RESULT_CODE_OK);
    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);

    while(recv_num < length) {
        recv_num += AT_CMD_DATA_RECV(g_at_mqtt[linkid].user_name + recv_num, length - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    g_at_mqtt[linkid].user_name[length] = '\0';
    if (length == recv_num) {
        ret = AT_RESULT_CODE_SEND_OK;
    } else {
        ret = AT_RESULT_CODE_SEND_FAIL;
    }

    return ret;
}

static int at_query_cmd_mqttpassword(int argc, const char **argv)
{
    for (int i = 0; i < AT_MQTT_LINK_MAX; i++) {
        at_response_string("+MQTTPASSWORD:%d,\"%s\"\r\n", 
                           i, 
                           g_at_mqtt[i].password ? g_at_mqtt[i].password : "");
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttpassword(int argc, const char **argv)
{
    int ret, linkid, length, recv_num = 0; 

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_NUMBER(1, &length);
    
    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (length < 1 || length > AT_MQTT_PASSWD_MAX_LEN) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (g_at_mqtt[linkid].password) {
        at_free(g_at_mqtt[linkid].password);
        g_at_mqtt[linkid].password = NULL;
    }

    g_at_mqtt[linkid].password = at_malloc(length + 1);
    if (!g_at_mqtt[linkid].password) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    at_response_result(AT_RESULT_CODE_OK);
    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);

    while(recv_num < length) {
        recv_num += AT_CMD_DATA_RECV(g_at_mqtt[linkid].password + recv_num, length - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    g_at_mqtt[linkid].password[length] = '\0';
    if (length == recv_num) {
        ret = AT_RESULT_CODE_SEND_OK;
    } else {
        ret = AT_RESULT_CODE_SEND_FAIL;
    }

    return ret;
}

static int at_query_cmd_mqttconncfg(int argc, const char **argv)
{
    for (int i = 0; i < AT_MQTT_LINK_MAX; i++) {
        at_response_string("+MQTTCONNCFG:%d,%d,%d,\"%s\",\"%s\",%d,%d\r\n", 
                           i, 
                           g_at_mqtt[i].keepalive,
                           (g_at_mqtt[i].connect_flags & MQTT_CONNECT_CLEAN_SESSION) ? 1 : 0,
                           g_at_mqtt[i].will_topic,
                           g_at_mqtt[i].will_message,
                           (g_at_mqtt[i].connect_flags >> 3) & 0x3,
                           (g_at_mqtt[i].connect_flags & MQTT_CONNECT_WILL_RETAIN) ? 1 : 0);
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttconncfg(int argc, const char **argv)
{
    uint8_t connect_flags = 0;
    char will_topic[128];
    char will_message[128];
    int linkid, keepalive, disable_clean_session, will_qos, will_retain; 

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_NUMBER(1, &keepalive);
    AT_CMD_PARSE_NUMBER(2, &disable_clean_session);
    AT_CMD_PARSE_STRING(3, will_topic, sizeof(will_topic));
    AT_CMD_PARSE_STRING(4, will_message, sizeof(will_message));
    AT_CMD_PARSE_NUMBER(5, &will_qos);
    AT_CMD_PARSE_NUMBER(6, &will_retain);
    
    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (keepalive <= 0 || keepalive > 7200) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (disable_clean_session != 0 && disable_clean_session != 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (will_qos < 0 || will_qos > 2) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    
    if (will_retain != 0 && will_retain != 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    g_at_mqtt[linkid].keepalive = keepalive;

    if (!disable_clean_session) {
        connect_flags |= MQTT_CONNECT_CLEAN_SESSION;
    }

    strlcpy(g_at_mqtt[linkid].will_topic, will_topic, sizeof(g_at_mqtt[linkid].will_topic));
    strlcpy(g_at_mqtt[linkid].will_message, will_message, sizeof(g_at_mqtt[linkid].will_message));

    if (will_qos == 0) {
        connect_flags |= MQTT_CONNECT_WILL_QOS_0;
    } else if (will_qos == 1) {
        connect_flags |= MQTT_CONNECT_WILL_QOS_1;
    } else if (will_qos == 2) {
        connect_flags |= MQTT_CONNECT_WILL_QOS_2;
    }

    if (will_retain) {
        connect_flags |= MQTT_CONNECT_WILL_RETAIN;
    }

    g_at_mqtt[linkid].connect_flags = connect_flags;
    g_at_mqtt[linkid].state = AT_MQTT_STATE_CONNCFG;

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_mqttconn(int argc, const char **argv)
{
    int i = 0;
    char outbuf[256];

    for (i = 0; i < AT_MQTT_LINK_MAX; i++) {
        snprintf(outbuf, sizeof(outbuf), "+MQTTCONN:%d,%d,%d,\"%s\",%s,%d\r\n", 
                 i, 
                 g_at_mqtt[i].state, 
                 g_at_mqtt[i].scheme,
                 g_at_mqtt[i].remote_host,
                 g_at_mqtt[i].remote_port,
                 g_at_mqtt[i].reconnect);

        AT_CMD_DATA_SEND(outbuf, strlen(outbuf));
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttconn(int argc, const char **argv)
{
    int linkid;
    int ret;
    int remote_port, reconnect = 0;

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_STRING(1, g_at_mqtt[linkid].remote_host, sizeof(g_at_mqtt[linkid].remote_host));
    AT_CMD_PARSE_NUMBER(2, &remote_port);
    AT_CMD_PARSE_NUMBER(3, &reconnect);

    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (remote_port < 1 || remote_port > 65535) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (reconnect != 0 && reconnect != 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (socketfd_is_connected(&g_at_mqtt[linkid].sockfd)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    snprintf(g_at_mqtt[linkid].remote_port, 
             sizeof(g_at_mqtt[linkid].remote_port), "%d", remote_port);
   
    if (g_at_mqtt[linkid].keepalive == 0) {
        g_at_mqtt[linkid].keepalive = 120;
    }

    printf("client_id:%s user_name:%s password:%s\r\n", g_at_mqtt[linkid].client_id, 
           g_at_mqtt[linkid].user_name, g_at_mqtt[linkid].password);

    if (g_at_mqtt[linkid].p_sendbuf || g_at_mqtt[linkid].p_recvbuf) {
        printf("mqtt buffer not free\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    g_at_mqtt[linkid].p_sendbuf = at_calloc(AT_MQTT_BNUFFER_SIZE_MAX, 1);
    if (!g_at_mqtt[linkid].p_sendbuf) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    g_at_mqtt[linkid].p_recvbuf = at_calloc(AT_MQTT_BNUFFER_SIZE_MAX, 1);
    if (!g_at_mqtt[linkid].p_recvbuf) {
        at_free(g_at_mqtt[linkid].p_sendbuf);
        g_at_mqtt[linkid].p_sendbuf = NULL;
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    /* setup a client */ 
    if (reconnect) {

        mqtt_reinit(&g_at_mqtt[linkid].client, 
                  &g_at_mqtt[linkid].sockfd, 
                  g_at_mqtt[linkid].p_sendbuf, 
                  AT_MQTT_BNUFFER_SIZE_MAX, 
                  g_at_mqtt[linkid].p_recvbuf, 
                  AT_MQTT_BNUFFER_SIZE_MAX);
    } else {
 
        /* open the non-blocking TCP socket (connecting to the broker) */
        ret = open_nb_socket(&g_at_mqtt[linkid].sockfd, 
                             linkid, 
                            (g_at_mqtt[linkid].scheme == AT_MQTT_OVER_TCP)?MQTTC_PAL_CONNTION_TYPE_TCP:MQTTC_PAL_CONNTION_TYPE_TLS,
                             g_at_mqtt[linkid].remote_host, 
                             g_at_mqtt[linkid].remote_port);

        if (ret < 0) {
            printf("Failed to open socket: %d\r\n", ret);
            mqtt_close(linkid);
            mqtt_buf_free(&g_at_mqtt[linkid]);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }

        mqtt_reinit(&g_at_mqtt[linkid].client, 
                  &g_at_mqtt[linkid].sockfd, 
                  g_at_mqtt[linkid].p_sendbuf, 
                  AT_MQTT_BNUFFER_SIZE_MAX, 
                  g_at_mqtt[linkid].p_recvbuf, 
                  AT_MQTT_BNUFFER_SIZE_MAX);
 

        /* Send connection request to the broker. */
        ret = mqtt_connect(&g_at_mqtt[linkid].client, 
                           g_at_mqtt[linkid].client_id, 
                           strlen(g_at_mqtt[linkid].will_topic)?g_at_mqtt[linkid].will_topic:NULL, 
                           strlen(g_at_mqtt[linkid].will_message)?g_at_mqtt[linkid].will_message:NULL, 
                           strlen(g_at_mqtt[linkid].will_message), 
                           g_at_mqtt[linkid].user_name, 
                           g_at_mqtt[linkid].password, 
                           g_at_mqtt[linkid].connect_flags, 
                           g_at_mqtt[linkid].keepalive);

        if (ret != MQTT_OK) {
            printf("fail \r\n");
            mqtt_close(linkid);
            mqtt_buf_free(&g_at_mqtt[linkid]);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
        /* check that we don't have any errors */
        if (g_at_mqtt[linkid].client.error != MQTT_OK) {
            printf("error: %s\r\n", mqtt_error_str(g_at_mqtt[linkid].client.error));
            mqtt_close(linkid);
            mqtt_buf_free(&g_at_mqtt[linkid]);
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
        }
        _mqtt_connected_event(linkid);
    }
    g_at_mqtt[linkid].reconnect = reconnect;
    
    /* start a thread to refresh the client (handle egress and ingree client traffic) */
    if (!g_at_mqtt[linkid].client_task) {
        g_at_mqtt[linkid].refresher_run = 1;
        xTaskCreate(client_refresher_task, (char*)"client_ref", 1024,  &g_at_mqtt[linkid], 10, &g_at_mqtt[linkid].client_task);
    }


    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttpub(int argc, const char **argv)
{
    char topic_name[128] = {0};
    char msg[128] = {0};
    uint8_t publish_flags = 0;
    int linkid, qos, retain; 

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_STRING(1, topic_name, sizeof(topic_name));
    AT_CMD_PARSE_STRING(2, msg, sizeof(msg));
    AT_CMD_PARSE_NUMBER(3, &qos);
    AT_CMD_PARSE_NUMBER(4, &retain);
    
    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (qos < 0 || qos > 2) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (!socketfd_is_connected(&g_at_mqtt[linkid].sockfd)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    if (qos == 0) {
        publish_flags |= MQTT_PUBLISH_QOS_0;
    } else if (qos == 1) {
        publish_flags |= MQTT_PUBLISH_QOS_1;
    } else if (qos == 2) {
        publish_flags |= MQTT_PUBLISH_QOS_2;
    }

    if (retain) {
        publish_flags |= MQTT_CONNECT_WILL_RETAIN;
    }
   
    if (mqtt_publish(&g_at_mqtt[linkid].client, (const char *)topic_name, msg, strlen(msg), publish_flags) != MQTT_OK) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttpubraw(int argc, const char **argv)
{
    int ret, linkid, length, recv_num = 0; 
    char *buffer = NULL;
    char topic_name[128] = {0};
    uint8_t publish_flags = 0;
    int qos, retain; 

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_STRING(1, topic_name, sizeof(topic_name));
    AT_CMD_PARSE_NUMBER(2, &length);
    AT_CMD_PARSE_NUMBER(3, &qos);
    AT_CMD_PARSE_NUMBER(4, &retain);
    
    if (length > AT_MQTT_BNUFFER_SIZE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_RESOURCE);
    }

    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (length <= 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (qos < 0 || qos > 2) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (!socketfd_is_connected(&g_at_mqtt[linkid].sockfd)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    if (qos == 0) {
        publish_flags |= MQTT_PUBLISH_QOS_0;
    } else if (qos == 1) {
        publish_flags |= MQTT_PUBLISH_QOS_1;
    } else if (qos == 2) {
        publish_flags |= MQTT_PUBLISH_QOS_2;
    }

    if (retain) {
        publish_flags |= MQTT_CONNECT_WILL_RETAIN;
    }
 
    buffer = at_malloc(length + 1);
    if (!buffer) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_MEMORY);
    }

    at_response_result(AT_RESULT_CODE_OK);
    AT_CMD_RESPONSE(AT_CMD_MSG_WAIT_DATA);

    while(recv_num < length) {
        recv_num += AT_CMD_DATA_RECV(buffer + recv_num, length - recv_num);
    }
    at_response_string("Recv %d bytes\r\n", recv_num);

    buffer[length] = '\0';
    if (length == recv_num) {
        ret = AT_RESULT_CODE_SEND_OK;
    } else {
        ret = AT_RESULT_CODE_SEND_FAIL;
    }
 
    if (mqtt_publish(&g_at_mqtt[linkid].client, (const char *)topic_name, buffer, length, publish_flags) != MQTT_OK) {
        ret = AT_RESULT_CODE_SEND_FAIL;
    }

    at_free(buffer);
    
    return ret;
}

static int at_query_cmd_mqttsub(int argc, const char **argv)
{
    int i, linkid;

    for (linkid = 0; linkid < AT_MQTT_LINK_MAX; linkid++) {
        for (i = 0; i < AT_MQTT_SUB_TOPIC_MAX; i++) {
            if (g_at_mqtt[linkid].sub_topic[i].topic) {
                at_response_string("+MQTTSUB:%d,%d,%s,%d\r\n", 
                                   linkid, 
                                   g_at_mqtt[linkid].state, 
                                   g_at_mqtt[linkid].sub_topic[i].topic, 
                                   g_at_mqtt[linkid].sub_topic[i].qos);
            }
        }
    }
    
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttsub(int argc, const char **argv)
{
    int i;
    int linkid, qos; 
    char topic_name[128] = {0};

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_STRING(1, topic_name, sizeof(topic_name));
    AT_CMD_PARSE_NUMBER(2, &qos);
    
    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (!socketfd_is_connected(&g_at_mqtt[linkid].sockfd)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    for (i = 0; i < AT_MQTT_SUB_TOPIC_MAX; i++) {
        if (g_at_mqtt[linkid].sub_topic[i].topic) {
            if (strcmp(g_at_mqtt[linkid].sub_topic[i].topic, topic_name) == 0) {
                at_response_string("+MQTTSUB:ALREADY_SUBSCRIBE\r\n");
                return AT_RESULT_CODE_OK;
            }
        }
    }

    for (i = 0; i < AT_MQTT_SUB_TOPIC_MAX; i++) {
        if (!g_at_mqtt[linkid].sub_topic[i].topic) {
            g_at_mqtt[linkid].sub_topic[i].topic = strdup(topic_name);
            g_at_mqtt[linkid].sub_topic[i].qos = qos;
            break;
        }
    }
    if (i == AT_MQTT_SUB_TOPIC_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NO_RESOURCE);
    }

    if (mqtt_subscribe(&g_at_mqtt[linkid].client, topic_name, qos) != MQTT_OK) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    g_at_mqtt[linkid].state = AT_MQTT_STATE_CONNECTED_TOPIC;
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttunsub(int argc, const char **argv)
{
    int i, unsub = 0;
    int linkid; 
    char topic_name[128] = {0};

    AT_CMD_PARSE_NUMBER(0, &linkid);
    AT_CMD_PARSE_STRING(1, topic_name, sizeof(topic_name));
    
    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    if (!socketfd_is_connected(&g_at_mqtt[linkid].sockfd)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    for (i = 0; i < AT_MQTT_SUB_TOPIC_MAX; i++) {
        if (g_at_mqtt[linkid].sub_topic[i].topic) {
            if (strcmp(g_at_mqtt[linkid].sub_topic[i].topic, topic_name) == 0) {
                at_free(g_at_mqtt[linkid].sub_topic[i].topic);
                g_at_mqtt[linkid].sub_topic[i].topic = NULL;
                unsub = 1;
            }
        }
    }
    if (!unsub) {
        at_response_string("+MQTTUNSUB:NO_UNSUBSCRIBE\r\n");
        return AT_RESULT_CODE_OK;
    }

    if (mqtt_unsubscribe(&g_at_mqtt[linkid].client, topic_name) != MQTT_OK) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    g_at_mqtt[linkid].state = AT_MQTT_STATE_CONNECTED_NO_TOPIC;
    for (i = 0; i < AT_MQTT_SUB_TOPIC_MAX; i++) {
        if (g_at_mqtt[linkid].sub_topic[i].topic) {
            g_at_mqtt[linkid].state = AT_MQTT_STATE_CONNECTED_NO_TOPIC;
            break;
        }
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttclean(int argc, const char **argv)
{
    int linkid; 

    AT_CMD_PARSE_NUMBER(0, &linkid);
    
    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    if (!socketfd_is_connected(&g_at_mqtt[linkid].sockfd)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
    }

    if (mqtt_disconnect(&g_at_mqtt[linkid].client) == MQTT_OK) {
        mqtt_sync(&g_at_mqtt[linkid].client);
    }

    mqtt_handle_clear(linkid);
    mqtt_close(linkid);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttalpn(int argc, const char **argv)
{
    int i, offset;
    int linkid = 0, count = 0;
    char alpn[255] = {0};

    AT_CMD_PARSE_NUMBER(0, &linkid);

    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }

    AT_CMD_PARSE_NUMBER(1, &count);

    if (count < 0 || count >= AT_MQTT_ALPN_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (count != argc - 2) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_NUM_MISMATCH);
    }
    offset = 2;

    for (i = 0; i < count; i++) {
        AT_CMD_PARSE_STRING(i + offset, alpn, sizeof(alpn));
        at_ssl_alpn_set(linkid, i, alpn);
    }
    for (; i < AT_MQTT_ALPN_MAX; i++) {
        at_ssl_alpn_set(linkid, i, NULL);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_mqttalpn(int argc, const char **argv)
{
    int i, n, offset = 0;
    int count = 0;
    char **alpn;
    char output[255] = {0};

    
    for (i = 0; i < AT_MQTT_LINK_MAX; i++) {
        alpn = at_ssl_alpn_get(i, &count);

        n = snprintf(output + offset, sizeof(output) - offset, "+MQTTALPN:%d", i);
        if (n > 0) {
            offset += n;
        }

        for (i = 0; i < count; i++) {
            n = snprintf(output + offset, sizeof(output) - offset, ",\"%s\"", alpn[i]);
            if (n > 0) {
                offset += n;
            }
        }
        
        n = snprintf(output + offset, sizeof(output) - offset, "%s", "\r\n");
        if (n > 0) {
            offset += n;
        }
        AT_CMD_DATA_SEND(output, strlen(output));
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mqttsni(int argc, const char **argv)
{
    int linkid = 0;
    char hostname[255] = {0};

    AT_CMD_PARSE_NUMBER(0, &linkid);

    if (mqtt_linkid_valid(linkid)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_HANDLE_INVALID);
    }
    AT_CMD_PARSE_STRING(1, hostname, sizeof(hostname));
    
    at_ssl_sni_set(linkid, hostname);

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_mqttsni(int argc, const char **argv)
{
    int i = 0;

    for (i = 0; i < AT_MQTT_LINK_MAX; i++) {
        at_response_string("+MQTTSNI:%d,%s\r\n", i, at_ssl_sni_get(i));
    }

    return AT_RESULT_CODE_OK;
}

static const at_cmd_struct at_mqtt_cmd[] = {
    {"+MQTTUSERCFG",  at_query_cmd_mqttusercfg, at_setup_cmd_mqttusercfg, NULL, 3, 8},
    {"+MQTTCLIENTID", at_query_cmd_mqttclientid, at_setup_cmd_mqttclientid, NULL, 2, 2},
    {"+MQTTUSERNAME", at_query_cmd_mqttusername, at_setup_cmd_mqttusername, NULL, 2, 2},
    {"+MQTTPASSWORD", at_query_cmd_mqttpassword, at_setup_cmd_mqttpassword, NULL, 2, 2},
    {"+MQTTCONNCFG",  at_query_cmd_mqttconncfg, at_setup_cmd_mqttconncfg, NULL, 7, 7},
    {"+MQTTCONN",     at_query_cmd_mqttconn, at_setup_cmd_mqttconn, NULL, 4, 4},
    {"+MQTTALPN",     at_query_cmd_mqttalpn, at_setup_cmd_mqttalpn, NULL, 2, 8},
    {"+MQTTSNI",      at_query_cmd_mqttsni, at_setup_cmd_mqttsni, NULL, 2, 2},
    {"+MQTTPUB",      NULL, at_setup_cmd_mqttpub, NULL, 5, 5},
    {"+MQTTPUBRAW",   NULL, at_setup_cmd_mqttpubraw, NULL, 5, 5},
    {"+MQTTSUB",      at_query_cmd_mqttsub, at_setup_cmd_mqttsub, NULL, 3, 3},
    {"+MQTTUNSUB",    NULL, at_setup_cmd_mqttunsub, NULL, 2, 2},
    {"+MQTTCLEAN",    NULL, at_setup_cmd_mqttclean, NULL, 1, 1},
    {NULL,              NULL, NULL, NULL, 0, 0},
};

int at_mqtt_init()
{
    for (int i = 0; i < AT_MQTT_LINK_MAX; i++) {
        g_at_mqtt[i].connect_flags = MQTT_CONNECT_CLEAN_SESSION;

        g_at_mqtt[i].client.publish_response_callback_state = (void *)i;
        g_at_mqtt[i].client.socketfd = &g_at_mqtt[i].sockfd;
        mqtt_init_reconnect(&g_at_mqtt[i].client,
                            reconnect_client,
                            &g_at_mqtt[i],
                            publish_callback_1);
    }
    return 0;
}

bool at_mqtt_cmd_regist(void)
{
    at_mqtt_config_init();

    at_mqtt_init();

    if (at_cmd_register(at_mqtt_cmd, sizeof(at_mqtt_cmd) / sizeof(at_mqtt_cmd[0])) == 0)
        return true;
    else
        return false;
}

