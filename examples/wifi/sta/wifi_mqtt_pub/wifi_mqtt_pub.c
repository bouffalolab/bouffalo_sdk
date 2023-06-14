#include "FreeRTOS_POSIX.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "utils_getopt.h"

#include "mqtt.h"
#include "shell.h"

#include "bflb_rtc.h"

uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */

shell_sig_func_ptr abort_exec;
static TaskHandle_t client_daemon;
int test_sockfd;
const char* addr;

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
    if (test_sockfd)
    {
        close(test_sockfd);
    }
    printf("mqtt_pub stop publish to %s\r\n", addr);

    abort_exec(sig);

    vTaskDelete(client_daemon);

}

static int example_mqtt(int argc, const char *argv[])
{
    const char* port;
    const char* topic;

    int ret = 0;
    // int argc = 0;

    abort_exec = shell_signal(1, test_close);

    struct bflb_device_s *rtc;

    rtc = bflb_device_get_by_name("rtc");
    bflb_rtc_set_time(rtc, 0);
    /* our build utc time 1686873600 2023-06-16 00:00:00 */
    time_t time_stamp = 1686873600;

    /* get address (argv[1] if present) */
    if (argc > 1) {
        addr = argv[1];
    } else {
        addr = "test.mosquitto.org";
    }

    /* get port number (argv[2] if present) */
    if (argc > 2) {
        port = argv[2];
    } else {
        port = "1883";
    }

    /* get the topic name to publish */
    if (argc > 3) {
        topic = argv[3];
    } else {
        topic = "bl618";
    }

    /* open the non-blocking TCP socket (connecting to the broker) */
    test_sockfd = open_nb_socket(addr, port);

    if (test_sockfd < 0) {
        printf("Failed to open socket: %d\r\n", test_sockfd);
        test_close(SHELL_SIGINT);
    }

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

    /* start publishing the time */
    printf("%s is ready to begin publishing the time.\r\n", argv[0]);
    printf("Press ENTER to publish the current time.\r\n");
    printf("Press CTRL-C to exit.\r\n");

    /* block wait CTRL-C exit */
    while(1) {
        /* get the current time */
        time_t time = (BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc)) + time_stamp);
        struct tm *gmt = gmtime(&time);
        char time_buf[26];
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", gmt);
        // printf("%s\r\n", time_buf);

        /* print a message */
        char application_message[256] = {"{\"hello mqtt by bl616/8 !\"}\r\n"};
        sprintf(&application_message[strlen(application_message)], "Now time is %s", time_buf);
        printf("%s published : \"%s\"\r\n", argv[0], application_message);

        /* publish the time */
        mqtt_publish(&client, topic, application_message, strlen(application_message) + 1, MQTT_PUBLISH_QOS_0);

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

int cmd_mqtt_publisher(int argc, const char **argv)
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

SHELL_CMD_EXPORT_ALIAS(cmd_mqtt_publisher, mqtt_pub, mqtt publisher);
#endif
