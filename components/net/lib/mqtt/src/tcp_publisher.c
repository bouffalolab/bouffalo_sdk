/****************************************************************************
 * apps/examples/mqttc/mqttc_pub.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <lwip/errno.h>
#include <netdb.h>

#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509.h>

#include "utils_getopt.h"

#include <mqtt.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CONFIG_EXAMPLES_MQTTC_TXSIZE    (256)
#define CONFIG_EXAMPLES_MQTTC_RXSIZE    (256)

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct mbedtls_context {
    mbedtls_net_context net_ctx;
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_conf;
    mbedtls_x509_crt ca_crt;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
};

struct mqttc_cfg_s
{
  struct mqtt_client client;
  const char *host;
  const char *port;
  const char *topic;
  const char *msg;
  const char *id;
  const char *username;
  const char *passwd;
  const char *ca;
  uint8_t sendbuf[CONFIG_EXAMPLES_MQTTC_TXSIZE];
  uint8_t recvbuf[CONFIG_EXAMPLES_MQTTC_RXSIZE];
  uint32_t tmo;
  uint8_t flags;
  uint8_t qos;
  enum connection_type type;
};

static TaskHandle_t mqtt_deamon;

static const char *test_ca_crt = {
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDEzCCAfugAwIBAgIBAjANBgkqhkiG9w0BAQsFADA/MQswCQYDVQQGEwJDTjER\r\n"
    "MA8GA1UECAwIaGFuZ3pob3UxDDAKBgNVBAoMA0VNUTEPMA0GA1UEAwwGUm9vdENB\r\n"
    "MB4XDTIwMDUwODA4MDcwNVoXDTMwMDUwNjA4MDcwNVowPzELMAkGA1UEBhMCQ04x\r\n"
    "ETAPBgNVBAgMCGhhbmd6aG91MQwwCgYDVQQKDANFTVExDzANBgNVBAMMBlNlcnZl\r\n"
    "cjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALNeWT3pE+QFfiRJzKmn\r\n"
    "AMUrWo3K2j/Tm3+Xnl6WLz67/0rcYrJbbKvS3uyRP/stXyXEKw9CepyQ1ViBVFkW\r\n"
    "Aoy8qQEOWFDsZc/5UzhXUnb6LXr3qTkFEjNmhj+7uzv/lbBxlUG1NlYzSeOB6/RT\r\n"
    "8zH/lhOeKhLnWYPXdXKsa1FL6ij4X8DeDO1kY7fvAGmBn/THh1uTpDizM4YmeI+7\r\n"
    "4dmayA5xXvARte5h4Vu5SIze7iC057N+vymToMk2Jgk+ZZFpyXrnq+yo6RaD3ANc\r\n"
    "lrc4FbeUQZ5a5s5Sxgs9a0Y3WMG+7c5VnVXcbjBRz/aq2NtOnQQjikKKQA8GF080\r\n"
    "BQkCAwEAAaMaMBgwCQYDVR0TBAIwADALBgNVHQ8EBAMCBeAwDQYJKoZIhvcNAQEL\r\n"
    "BQADggEBAJefnMZpaRDHQSNUIEL3iwGXE9c6PmIsQVE2ustr+CakBp3TZ4l0enLt\r\n"
    "iGMfEVFju69cO4oyokWv+hl5eCMkHBf14Kv51vj448jowYnF1zmzn7SEzm5Uzlsa\r\n"
    "sqjtAprnLyof69WtLU1j5rYWBuFX86yOTwRAFNjm9fvhAcrEONBsQtqipBWkMROp\r\n"
    "iUYMkRqbKcQMdwxov+lHBYKq9zbWRoqLROAn54SRqgQk6c15JdEfgOOjShbsOkIH\r\n"
    "UhqcwRkQic7n1zwHVGVDgNIZVgmJ2IdIWBlPEC7oLrRrBD/X1iEEXtKab6p5o22n\r\n"
    "KB5mN+iQaE+Oe2cpGKZJiJRdM+IqDDQ=\r\n"
    "-----END CERTIFICATE-----\r\n"
};


/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: client_refresher
 *
 * Description:
 *   The client's refresher. This function triggers back-end routines to
 *   handle ingress/egress traffic to the broker.
 *
 ****************************************************************************/

static void client_refresher(void *data)
{
  while (1)
    {
      mqtt_sync((struct mqtt_client *)data);
      vTaskDelay(100);
    }
}

static void client_publish(void *data)
{

}
/****************************************************************************
 * Name: parsearg
 *
 * Description:
 *   Parse command line arguments.
 *
 ****************************************************************************/

static void parsearg(int argc, char *argv[], struct mqttc_cfg_s *cfg, int *n)
{
  int opt;
  int q;
  getopt_env_t getopt_env;

  utils_getopt_init(&getopt_env, 0);

  while ((opt = utils_getopt(&getopt_env, argc, argv, ":u:k:h:p:s:m:t:n:q:")) != -1)
    {
      switch (opt)
        {
          case 'u':
            cfg->username = getopt_env.optarg;
            break;

          case 'k':
            cfg->passwd = getopt_env.optarg;
            break;

          case 'h':
            cfg->host = getopt_env.optarg;
            break;

          case 'p':
            cfg->port = getopt_env.optarg;
            break;

          case 's':
            cfg->type = strtol(getopt_env.optarg, NULL, 10);
            break;

          case 'm':
            cfg->msg = getopt_env.optarg;
            break;

          case 't':
            cfg->topic = getopt_env.optarg;
            break;

          case 'n':
            *n = strtol(getopt_env.optarg, NULL, 10);
            break;

          case 'q':
            q = strtol(getopt_env.optarg, NULL, 10);
            switch (q)
              {
                case 0:
                  q = MQTT_PUBLISH_QOS_0;
                  break;

                case 1:
                  q = MQTT_PUBLISH_QOS_1;
                  break;

                case 2:
                  q = MQTT_PUBLISH_QOS_2;
                  break;

                default:
                  printf("QOS error, default QOS 0\r\n");
                  q = MQTT_PUBLISH_QOS_0;
                  break;
              }
            cfg->qos = q;
            break;

          default:
            printf("ERROR: Unrecognized option\r\n");
            break;
        }
    }
}

/****************************************************************************
 * Name: initserver_tcp
 *
 * Description:
 *   Resolve server's name and try to establish a connection.
 *
 ****************************************************************************/

static int initserver_tcp(const struct mqttc_cfg_s *cfg)
{
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *itr;
  int fd;
  int ret;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family  = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  printf("Connecting to %s:%s...\r\n", cfg->host, cfg->port);

  ret = getaddrinfo(cfg->host, cfg->port, &hints, &servinfo);
  if (ret != ERR_OK)
    {
      printf("ERROR! getaddrinfo() failed: %d\r\n", ret);
      return -1;
    }

  itr = servinfo;
  do
    {
      fd = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
      if (fd < 0)
        {
          continue;
        }

      ret = connect(fd, itr->ai_addr, itr->ai_addrlen);
      if (ret == 0)
        {
          break;
        }

      close(fd);
      fd = -1;
    }
  while ((itr = itr->ai_next) != NULL);

  freeaddrinfo(servinfo);

  if (fd < 0)
    {
      printf("ERROR! Couldn't create socket\r\n");
      return -1;
    }

  ret = fcntl(fd, F_GETFL, 0);
  if (ret < 0)
    {
      printf("ERROR! fcntl() F_GETFL failed, errno: %d\r\n", errno);
      return -1;
    }

  ret = fcntl(fd, F_SETFL, ret | O_NONBLOCK);
  if (ret < 0)
    {
      printf("ERROR! fcntl() F_SETFL failed, errno: %d\r\n", errno);
      return -1;
    }
  return fd;
}

/****************************************************************************
 * Name: cert_verify_failed
 *
 * Description:
 *
 ****************************************************************************/

static void cert_verify_failed(uint32_t rv)
{
  char buf[512];
  mbedtls_x509_crt_verify_info(buf, sizeof(buf), "\t", rv);
  printf("Certificate verification failed (%0" PRIx32 ") \r\n%s\r\n", rv, buf);
}

/****************************************************************************
 * Name: initserver_tls
 *
 * Description:
 *   Resolve server's name and try to establish a connection.
 *
 ****************************************************************************/

static int initserver_tls(const struct mqttc_cfg_s *cfg, struct mbedtls_context *ctx)
{
  const unsigned char *additional = (const unsigned char *)"MQTT-C";
  size_t additional_len = 6;
  int rv;
  int crt_len;
  char buf[100];

  mbedtls_net_context *net_ctx = &ctx->net_ctx;
  mbedtls_ssl_context *ssl_ctx = &ctx->ssl_ctx;
  mbedtls_ssl_config *ssl_conf = &ctx->ssl_conf;
  mbedtls_x509_crt *ca_crt = &ctx->ca_crt;
  mbedtls_entropy_context *entropy = &ctx->entropy;
  mbedtls_ctr_drbg_context *ctr_drbg = &ctx->ctr_drbg;

  mbedtls_entropy_init(entropy);
  mbedtls_ctr_drbg_init(ctr_drbg);
  rv = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, entropy,
                             additional, additional_len);
  if (rv != 0) {
    mbedtls_strerror(rv, buf, sizeof(buf));
    printf("mbedtls_ctr_drbg_seed failed with %x (%s)\n", -rv, buf);
    return -1;
  }

  mbedtls_x509_crt_init(ca_crt);
  crt_len = strlen(cfg->ca);

  rv = mbedtls_x509_crt_parse(ca_crt, (unsigned char *)cfg->ca, crt_len + 1);
  if (rv != 0) {
    mbedtls_strerror(rv, buf, sizeof(buf));
    printf("mbedtls_x509_crt_parse_file failed with %x (%s)\n", -rv, buf);
    return -1;
  }

  mbedtls_ssl_config_init(ssl_conf);
  rv = mbedtls_ssl_config_defaults(ssl_conf, MBEDTLS_SSL_IS_CLIENT,
                                   MBEDTLS_SSL_TRANSPORT_STREAM,
                                   MBEDTLS_SSL_PRESET_DEFAULT);
  if (rv != 0) {
    mbedtls_strerror(rv, buf, sizeof(buf));
    printf("mbedtls_ssl_config_defaults failed with %x (%s)\n", -rv, buf);
    return -1;
  }
  mbedtls_ssl_conf_ca_chain(ssl_conf, ca_crt, NULL);
  mbedtls_ssl_conf_authmode(ssl_conf, MBEDTLS_SSL_VERIFY_NONE);
  mbedtls_ssl_conf_rng(ssl_conf, mbedtls_ctr_drbg_random, ctr_drbg);

  mbedtls_net_init(net_ctx);
  rv = mbedtls_net_connect(net_ctx, cfg->host, cfg->port, MBEDTLS_NET_PROTO_TCP);
  if (rv != 0) {
    mbedtls_strerror(rv, buf, sizeof(buf));
    printf("mbedtls_net_connect failed with %x (%s)\n", -rv, buf);
    return -1;
  }
  rv = mbedtls_net_set_nonblock(net_ctx);
  if (rv != 0) {
    mbedtls_strerror(rv, buf, sizeof(buf));
    printf("mbedtls_net_set_nonblock failed with %x (%s)\n", -rv, buf);
    return -1;
  }

  mbedtls_ssl_init(ssl_ctx);
  rv = mbedtls_ssl_setup(ssl_ctx, ssl_conf);
  if (rv != 0) {
    mbedtls_strerror(rv, buf, sizeof(buf));
    printf("mbedtls_ssl_setup failed with %x (%s)\n", -rv, buf);
    return -1;
  }
  rv = mbedtls_ssl_set_hostname(ssl_ctx, cfg->host);
  if (rv != 0) {
    mbedtls_strerror(rv, buf, sizeof(buf));
    printf("mbedtls_ssl_set_hostname failed with %x (%s)\n", -rv, buf);
    return -1;
  }
  mbedtls_ssl_set_bio(ssl_ctx,
                      net_ctx,
                      mbedtls_net_send,
                      mbedtls_net_recv,
                      NULL);

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
  if (rv != 0) {
      mbedtls_strerror(rv, buf, sizeof(buf));
      printf("mbedtls_ssl_handshake failed with %x (%s)\n", -rv, buf);
      return -1;
  }

  uint32_t result = mbedtls_ssl_get_verify_result(ssl_ctx);
  if (result != 0) {
      if (result == (uint32_t)-1) {
          mbedtls_strerror(rv, buf, sizeof(buf));
          printf("mbedtls_ssl_get_verify_result failed with %x (%s)\n", -rv, buf);
          return -1;
      } else {
          cert_verify_failed(result);
      }
  }

  return 0;
}

/****************************************************************************
 * Name: closeserver_tls
 *
 * Description:
 *
 ****************************************************************************/

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

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void publish_callback(void** unused, struct mqtt_response_publish *published)
{
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char* topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    printf("Received topic('%s') qos %d: %s\r\n",
           topic_name,
           published->qos_level,
           (const char*) published->application_message);

    free(topic_name);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int demo(int argc, const char *argv[])
{
  enum MQTTErrors mqtterr;
  int n = 5;

  struct mbedtls_context ctx;

  struct mqttc_cfg_s mqtt_cfg =
    {
      .username = "admin",
      .passwd = "12345678",
      .host = "192.168.31.183",
      .port = "8883",
      .topic = "test",
      .msg = "test",
      .flags = MQTT_CONNECT_CLEAN_SESSION,
      .tmo = 400,
      .id = "BDS_MQTT",
      .qos = MQTT_PUBLISH_QOS_0,
      .type = MQTTC_PAL_CONNTION_TYPE_TLS,
      .ca = test_ca_crt
    };

  mqtt_pal_socket_handle sockfd = (mqtt_pal_socket_handle)calloc(1, sizeof(mqtt_pal_socket_handle));

  parsearg(argc, argv, &mqtt_cfg, &n);

  switch (mqtt_cfg.type)
    {
      case MQTTC_PAL_CONNTION_TYPE_TCP:
        sockfd->ctx.fd = initserver_tcp(&mqtt_cfg);
        printf("TCP fd %d\r\n", sockfd->ctx.fd);
        break;

      case MQTTC_PAL_CONNTION_TYPE_TLS:
        sockfd->ctx.fd = initserver_tls(&mqtt_cfg, &ctx);
        printf("TLS %p\r\n", sockfd->ctx.ssl_ctx);
        break;

      default:
        printf("mqtt type error \r\n");
        sockfd->ctx.fd = -1;
        break;
    }

  if (sockfd->ctx.fd < 0)
    {
      printf("ERRPR! socket failed.\r\n");
      return -1;
    }

  /* Set client connection type */

  sockfd->type = mqtt_cfg.type;

  if (mqtt_cfg.type == MQTTC_PAL_CONNTION_TYPE_TLS)
    {
      sockfd->ctx.ssl_ctx = &ctx.ssl_ctx;
    }

  mqtterr = mqtt_init(&mqtt_cfg.client, sockfd,
                      mqtt_cfg.sendbuf, sizeof(mqtt_cfg.sendbuf),
                      mqtt_cfg.recvbuf, sizeof(mqtt_cfg.recvbuf),
                      publish_callback);
  if (mqtterr != MQTT_OK)
    {
      printf("ERRPR! mqtt_init() failed.\r\n");
      goto err_with_socket;
    }

  mqtterr = mqtt_connect(&mqtt_cfg.client, mqtt_cfg.id,
                         NULL, /* Will topic */
                         NULL, /* Will message */
                         0,    /* Will message size */
                         mqtt_cfg.username, /* User name */
                         mqtt_cfg.passwd, /* Password */
                         mqtt_cfg.flags, mqtt_cfg.tmo);

  if (mqtterr != MQTT_OK)
    {
      printf("ERROR! mqtt_connect() failed\r\n");
      goto err_with_socket;
    }

  if (mqtt_cfg.client.error != MQTT_OK)
    {
      printf("error: %s\r\n", mqtt_error_str(mqtt_cfg.client.error));
      goto err_with_socket;
    }
  else
    {
      printf("Success: Connected to broker!\r\n");
    }

  /* Start a thread to refresh the client (handle egress and ingree client
   * traffic)
   */

  if (pdPASS != xTaskCreate(client_refresher, (char *)"mqtt_deamon", 2048, (void *)&mqtt_cfg.client, 7, &mqtt_deamon))
    {
      printf("ERROR! pthread_create() failed.\r\n");
      goto err_with_socket;
    }

  /* subscribe: qos is an integer parameter, do not use MQTTPublishFlags */

  mqtt_subscribe(&mqtt_cfg.client, mqtt_cfg.topic, 2);

  while (n--)
    {
      mqtterr = mqtt_publish(&mqtt_cfg.client, mqtt_cfg.topic,
                             mqtt_cfg.msg, strlen(mqtt_cfg.msg) + 1,
                             mqtt_cfg.qos);
      if (mqtterr != MQTT_OK)
        {
          printf("ERROR! mqtt_publish() %s\r\n", mqtt_error_str(mqtt_cfg.client.error));
          goto err_with_thrd;
        }

      if (mqtt_cfg.client.error != MQTT_OK)
        {
          printf("error: %s\r\n", mqtt_error_str(mqtt_cfg.client.error));
          goto err_with_thrd;
        }
      else
        {
          printf("Success: Published to broker!\r\n");
        }

      vTaskDelay(500);
    }

  printf("\nDisconnecting from %s\n\r\n", mqtt_cfg.host);
  mqtterr = mqtt_disconnect(&mqtt_cfg.client);
  if (mqtterr != MQTT_OK)
    {
      printf("ERROR! mqtt_disconnect() failed\r\n");
    }

  /* Force sending the DISCONNECT, the thread will be canceled before getting
   * the chance to sync this last packet.
   * Note however that close() would cleanly close the connection but only
   * through TCP (i.e. no MQTT DISCONNECT packet).
   */

  mqtt_sync(&mqtt_cfg.client);

err_with_thrd:
  printf("Task exit\r\n");
  vTaskDelete(mqtt_deamon);
err_with_socket:
  if (mqtt_cfg.type)
    {
      closeserver_tls(&ctx);
      printf("close socket %p\r\n", sockfd->ctx.ssl_ctx);
    }
  else
    {
      close(sockfd->ctx.fd);
      printf("close socket %d\r\n", sockfd->ctx.fd);
    }

  free(sockfd);

  printf("Exit MQTT demo\r\n");

  return 0;
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_mqtt_publish(int argc, const char **argv)
{
    demo(argc, argv);
    
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_mqtt_publish, mqtt_publish, mqtt publish);
#endif

