/**
 * @file channel_vchan.c
 * @brief Virtualchannel implementation for bflbwifid
 * @details This file wraps the nethub_vchan library to conform to the unified
 *          channel_ops_t interface. It provides AT command communication via
 *          virtualchannel (netlink) instead of TTY device.
 */

#include "channel.h"
#include "../include/bflbwifi_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

/* Include nethub_vchan library */
#include "../../virtualchan/nethub_vchan.h"

/* ========== Virtualchannel Context ========== */

/**
 * @brief Virtualchannel context structure
 */
typedef struct {
    bool initialized;                      /* Initialization flag */
    pthread_mutex_t tx_mutex;              /* Send mutex */
    pthread_mutex_t resp_mutex;            /* Response management mutex */
    pthread_cond_t resp_cond;              /* Response condition variable */
    channel_cmd_resp_t cmd_resp;           /* Current command response status */
    char last_resp_line[512];              /* Last received non-OK/ERROR line */
    channel_data_callback_t data_callback; /* Data callback */
    channel_disconnect_callback_t disconnect_callback;
    channel_reconnect_callback_t reconnect_callback;
    void *user_data; /* User data pointer for callback */
} channel_vchan_ctx_t;

/* Global virtualchannel context */
static channel_vchan_ctx_t *g_vchan_ctx = NULL;

/* ========== Forward Declarations ========== */

static void vchan_data_callback(const void *data, size_t len);
static int vchan_wait_for_response(int timeout_ms);
static void vchan_link_event_callback(nethub_vchan_link_state_t link_state, void *user_data);

/* ========== Virtualchannel Implementation ========== */

/**
 * @brief Virtualchannel init wrapper
 */
static int channel_vchan_init(const char *dev, int baudrate, channel_data_callback_t callback, void *user_data)
{
    int ret;

    (void)dev;      /* Virtualchannel doesn't use device path */
    (void)baudrate; /* Virtualchannel doesn't use baudrate */

    BFLB_LOGI("Initializing Virtualchannel channel");

    /* Allocate context */
    g_vchan_ctx = (channel_vchan_ctx_t *)calloc(1, sizeof(channel_vchan_ctx_t));
    if (!g_vchan_ctx) {
        BFLB_LOGE("Failed to allocate virtualchannel context");
        return -1;
    }

    /* Initialize mutexes and condition variable */
    if (pthread_mutex_init(&g_vchan_ctx->tx_mutex, NULL) != 0) {
        BFLB_LOGE("Failed to init tx_mutex");
        free(g_vchan_ctx);
        g_vchan_ctx = NULL;
        return -1;
    }

    if (pthread_mutex_init(&g_vchan_ctx->resp_mutex, NULL) != 0) {
        BFLB_LOGE("Failed to init resp_mutex");
        pthread_mutex_destroy(&g_vchan_ctx->tx_mutex);
        free(g_vchan_ctx);
        g_vchan_ctx = NULL;
        return -1;
    }

    if (pthread_cond_init(&g_vchan_ctx->resp_cond, NULL) != 0) {
        BFLB_LOGE("Failed to init resp_cond");
        pthread_mutex_destroy(&g_vchan_ctx->resp_mutex);
        pthread_mutex_destroy(&g_vchan_ctx->tx_mutex);
        free(g_vchan_ctx);
        g_vchan_ctx = NULL;
        return -1;
    }

    /* Save callback and user data */
    g_vchan_ctx->data_callback = callback;
    g_vchan_ctx->user_data = user_data;
    g_vchan_ctx->cmd_resp = CHANNEL_CMD_RESP_PENDING;

    /* Initialize nethub_vchan library */
    ret = nethub_vchan_init();
    if (ret != 0) {
        BFLB_LOGE("Failed to initialize nethub_vchan: %d", ret);
        pthread_cond_destroy(&g_vchan_ctx->resp_cond);
        pthread_mutex_destroy(&g_vchan_ctx->resp_mutex);
        pthread_mutex_destroy(&g_vchan_ctx->tx_mutex);
        free(g_vchan_ctx);
        g_vchan_ctx = NULL;
        return -1;
    }

    /* Register AT data callback */
    ret = nethub_vchan_at_register_callback(vchan_data_callback);
    if (ret != 0) {
        BFLB_LOGE("Failed to register AT callback: %d", ret);
        nethub_vchan_deinit();
        pthread_cond_destroy(&g_vchan_ctx->resp_cond);
        pthread_mutex_destroy(&g_vchan_ctx->resp_mutex);
        pthread_mutex_destroy(&g_vchan_ctx->tx_mutex);
        free(g_vchan_ctx);
        g_vchan_ctx = NULL;
        return -1;
    }

    ret = nethub_vchan_register_link_event_callback(vchan_link_event_callback, NULL);
    if (ret != 0) {
        BFLB_LOGE("Failed to register link callback: %d", ret);
        nethub_vchan_deinit();
        pthread_cond_destroy(&g_vchan_ctx->resp_cond);
        pthread_mutex_destroy(&g_vchan_ctx->resp_mutex);
        pthread_mutex_destroy(&g_vchan_ctx->tx_mutex);
        free(g_vchan_ctx);
        g_vchan_ctx = NULL;
        return -1;
    }

    g_vchan_ctx->initialized = true;
    BFLB_LOGI("Virtualchannel initialized successfully");

    return 0;
}

/**
 * @brief Virtualchannel deinit wrapper
 */
static void channel_vchan_deinit(void)
{
    if (!g_vchan_ctx) {
        return;
    }

    BFLB_LOGI("Deinitializing Virtualchannel");

    /* Note: nethub_vchan library doesn't have unregister function,
     * the callback will be cleaned up during deinit */

    nethub_vchan_register_link_event_callback(NULL, NULL);

    /* Deinitialize nethub_vchan library */
    nethub_vchan_deinit();

    /* Destroy mutexes and condition variable */
    pthread_cond_destroy(&g_vchan_ctx->resp_cond);
    pthread_mutex_destroy(&g_vchan_ctx->resp_mutex);
    pthread_mutex_destroy(&g_vchan_ctx->tx_mutex);

    /* Free context */
    free(g_vchan_ctx);
    g_vchan_ctx = NULL;

    BFLB_LOGI("Virtualchannel deinitialized");
}

/**
 * @brief Virtualchannel send wrapper
 */
static int channel_vchan_send(const char *data, int len)
{
    int ret;

    if (!g_vchan_ctx || !g_vchan_ctx->initialized) {
        BFLB_LOGE("Virtualchannel not initialized");
        return -1;
    }

    if (!data || len <= 0) {
        BFLB_LOGE("Invalid parameters");
        return -1;
    }

    /* Lock to prevent concurrent sends */
    pthread_mutex_lock(&g_vchan_ctx->tx_mutex);

    BFLB_LOGD("VCHAN TX: %.*s", len, data);

    /* Send via nethub_vchan AT channel */
    ret = nethub_vchan_at_send(data, len);
    if (ret < 0) {
        BFLB_LOGE("nethub_vchan_at_send failed: %d", ret);
        pthread_mutex_unlock(&g_vchan_ctx->tx_mutex);
        return -1;
    }

    pthread_mutex_unlock(&g_vchan_ctx->tx_mutex);

    return len;
}

/**
 * @brief Virtualchannel send command wrapper (adds \r\n)
 */
static int channel_vchan_send_command(const char *cmd)
{
    char buf[512];
    int len;

    if (!cmd) {
        return -1;
    }

    len = snprintf(buf, sizeof(buf), "%s\r\n", cmd);
    if (len >= (int)sizeof(buf)) {
        BFLB_LOGE("Command too long");
        return -1;
    }

    return channel_vchan_send(buf, len);
}

/**
 * @brief Virtualchannel send command and wait for response
 */
static int channel_vchan_send_command_sync(const char *cmd, int timeout_ms)
{
    int ret;

    if (!g_vchan_ctx || !cmd) {
        return -1;
    }

    if (timeout_ms <= 0) {
        timeout_ms = 5000; /* Default 5 seconds */
    }

    pthread_mutex_lock(&g_vchan_ctx->resp_mutex);
    g_vchan_ctx->cmd_resp = CHANNEL_CMD_RESP_PENDING;
    g_vchan_ctx->last_resp_line[0] = '\0';
    pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);

    /* Send command */
    ret = channel_vchan_send_command(cmd);
    if (ret < 0) {
        return -1;
    }

    /* Wait for response */
    return vchan_wait_for_response(timeout_ms);
}

/**
 * @brief Wait for specific string in response
 */
static int channel_vchan_wait_for(const char *expected, int timeout_ms)
{
    struct timespec ts;
    int ret;

    if (!g_vchan_ctx || !expected) {
        return -1;
    }

    /* Calculate timeout */
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    /* Wait for expected string in last_resp_line */
    pthread_mutex_lock(&g_vchan_ctx->resp_mutex);

    while (g_vchan_ctx->initialized) {
        /* Check if expected string found */
        if (strstr(g_vchan_ctx->last_resp_line, expected) != NULL) {
            pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);
            BFLB_LOGD("Found expected string: %s", expected);
            return 0;
        }

        /* Wait for new data */
        ret = pthread_cond_timedwait(&g_vchan_ctx->resp_cond, &g_vchan_ctx->resp_mutex, &ts);

        if (ret == ETIMEDOUT) {
            pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);
            BFLB_LOGE("Timeout waiting for: %s", expected);
            return -1;
        }
    }

    pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);
    return -1;
}

/**
 * @brief Get last response line
 */
static int channel_vchan_get_last_response(char *buf, size_t len)
{
    if (!g_vchan_ctx || !buf || len == 0) {
        return -1;
    }

    pthread_mutex_lock(&g_vchan_ctx->resp_mutex);
    strncpy(buf, g_vchan_ctx->last_resp_line, len - 1);
    buf[len - 1] = '\0';
    pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);

    return 0;
}

/**
 * @brief Clear last response line
 */
static void channel_vchan_clear_last_response(void)
{
    if (!g_vchan_ctx) {
        return;
    }

    pthread_mutex_lock(&g_vchan_ctx->resp_mutex);
    g_vchan_ctx->last_resp_line[0] = '\0';
    pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);
}

/**
 * @brief Get channel state
 */
static channel_state_t channel_vchan_get_state(void)
{
    nethub_vchan_state_snapshot_t snapshot;

    if (!g_vchan_ctx || !g_vchan_ctx->initialized) {
        return CHANNEL_STATE_DISCONNECTED;
    }

    if (nethub_vchan_get_state_snapshot(&snapshot) != 0) {
        return CHANNEL_STATE_DISCONNECTED;
    }

    return (snapshot.link_state == NETHUB_VCHAN_LINK_UP) ?
           CHANNEL_STATE_CONNECTED :
           CHANNEL_STATE_DISCONNECTED;
}

static void channel_vchan_set_disconnect_callback(channel_disconnect_callback_t callback)
{
    if (g_vchan_ctx) {
        g_vchan_ctx->disconnect_callback = callback;
    }
}

static void channel_vchan_set_reconnect_callback(channel_reconnect_callback_t callback)
{
    if (g_vchan_ctx) {
        g_vchan_ctx->reconnect_callback = callback;
    }
}

/**
 * @brief Suspend receive thread (not applicable for virtualchannel)
 * @note Virtualchannel uses internal thread in nethub_vchan library
 */
static int channel_vchan_suspend_recv_thread(void)
{
    BFLB_LOGW("Suspend recv thread not supported for virtualchannel");
    return 0;
}

/**
 * @brief Resume receive thread (not applicable for virtualchannel)
 */
static int channel_vchan_resume_recv_thread(void)
{
    BFLB_LOGW("Resume recv thread not supported for virtualchannel");
    return 0;
}

/**
 * @brief Get channel file descriptor (not available for virtualchannel)
 */
static int channel_vchan_get_fd(void)
{
    BFLB_LOGE("File descriptor not available for virtualchannel");
    return -1;
}

/* ========== Internal Helper Functions ========== */

/**
 * @brief Wait for command response (OK/ERROR)
 * @param timeout_ms Timeout in milliseconds
 * @return Response status (CHANNEL_CMD_RESP_OK, CHANNEL_CMD_RESP_ERROR, CHANNEL_CMD_RESP_TIMEOUT)
 */
static int vchan_wait_for_response(int timeout_ms)
{
    struct timespec ts;
    int ret;

    if (!g_vchan_ctx) {
        return -1;
    }

    /* Calculate timeout */
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    /* Wait for response */
    pthread_mutex_lock(&g_vchan_ctx->resp_mutex);

    while (g_vchan_ctx->cmd_resp == CHANNEL_CMD_RESP_PENDING) {
        ret = pthread_cond_timedwait(&g_vchan_ctx->resp_cond, &g_vchan_ctx->resp_mutex, &ts);
        if (ret != 0) {
            /* Timeout or other error */
            g_vchan_ctx->cmd_resp = CHANNEL_CMD_RESP_TIMEOUT;
            break;
        }
    }

    channel_cmd_resp_t result = g_vchan_ctx->cmd_resp;
    pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);

    if (result == CHANNEL_CMD_RESP_TIMEOUT) {
        BFLB_LOGE("Command timeout");
        return CHANNEL_CMD_RESP_TIMEOUT;
    }

    if (result == CHANNEL_CMD_RESP_ERROR) {
        BFLB_LOGE("Command failed");
        return CHANNEL_CMD_RESP_ERROR;
    }

    return CHANNEL_CMD_RESP_OK;
}

/**
 * @brief Virtualchannel data receive callback (called by nethub_vchan library)
 * @param data Received data
 * @param len Data length
 */
static void vchan_data_callback(const void *data, size_t len)
{
    const char *str = (const char *)data;
    char parse_buf[256];
    int valid_len;

    if (!g_vchan_ctx || !data || len == 0) {
        return;
    }

    if (len >= sizeof(parse_buf)) {
        len = sizeof(parse_buf) - 1;
    }

    memcpy(parse_buf, data, len);
    valid_len = parse_trim_crlf(parse_buf, len);

    if (valid_len == 0) {
        return;
    }

    BFLB_LOGD("VCHAN RX raw: %.*s", (int)len, str);

    str = parse_buf;
    len = valid_len;

    /* Identify command response (OK/ERROR) and notify waiter */
    if (strcmp(str, "OK") == 0) {
        pthread_mutex_lock(&g_vchan_ctx->resp_mutex);
        g_vchan_ctx->cmd_resp = CHANNEL_CMD_RESP_OK;
        pthread_cond_signal(&g_vchan_ctx->resp_cond);
        pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);
    } else if (strcmp(str, "ERROR") == 0) {
        pthread_mutex_lock(&g_vchan_ctx->resp_mutex);
        g_vchan_ctx->cmd_resp = CHANNEL_CMD_RESP_ERROR;
        pthread_cond_signal(&g_vchan_ctx->resp_cond);
        pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);
    }

    /* Save last response line and notify upper layer */
    if (strcmp(str, "OK") != 0 && strcmp(str, "ERROR") != 0 && str[0] != '\0') {
        pthread_mutex_lock(&g_vchan_ctx->resp_mutex);
        strncpy(g_vchan_ctx->last_resp_line, str, sizeof(g_vchan_ctx->last_resp_line) - 1);
        g_vchan_ctx->last_resp_line[sizeof(g_vchan_ctx->last_resp_line) - 1] = '\0';
        pthread_cond_signal(&g_vchan_ctx->resp_cond);
        pthread_mutex_unlock(&g_vchan_ctx->resp_mutex);
    }

    /* Call upper layer callback*/
    if (g_vchan_ctx->data_callback && str[0] != '\0') {
        g_vchan_ctx->data_callback(str, g_vchan_ctx->user_data);
    }
}

static void vchan_link_event_callback(nethub_vchan_link_state_t link_state, void *user_data)
{
    (void)user_data;

    if (!g_vchan_ctx) {
        return;
    }

    if (link_state == NETHUB_VCHAN_LINK_UP) {
        BFLB_LOGI("Virtualchannel link is up");
        if (g_vchan_ctx->reconnect_callback) {
            g_vchan_ctx->reconnect_callback();
        }
        return;
    }

    BFLB_LOGW("Virtualchannel link is down");
    if (g_vchan_ctx->disconnect_callback) {
        g_vchan_ctx->disconnect_callback();
    }
}

/* Virtualchannel operations structure */
const channel_ops_t channel_vchan_ops = {
    .type = CHANNEL_TYPE_VCHAN,
    .name = "vchan",
    .caps = CHANNEL_CAP_LINK_EVENTS,
    .init = channel_vchan_init,
    .deinit = channel_vchan_deinit,
    .send = channel_vchan_send,
    .send_command = channel_vchan_send_command,
    .send_command_sync = channel_vchan_send_command_sync,
    .wait_for = channel_vchan_wait_for,
    .get_last_response = channel_vchan_get_last_response,
    .clear_last_response = channel_vchan_clear_last_response,
    .get_state = channel_vchan_get_state,
    .set_disconnect_callback = channel_vchan_set_disconnect_callback,
    .set_reconnect_callback = channel_vchan_set_reconnect_callback,
    .suspend_recv_thread = channel_vchan_suspend_recv_thread,
    .resume_recv_thread = channel_vchan_resume_recv_thread,
    .get_fd = channel_vchan_get_fd,
};
