/**
 * @file bflbwifi_tty.c
 * @brief TTY communication layer implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>

#include "../include/bflbwifi_log.h"

#define TTY_RECV_BUF_SIZE  2048
#define TTY_LINE_BUF_SIZE  512

/**
 * @brief Command response status
 */
typedef enum {
    CMD_RESP_PENDING = 0,   /* Waiting for response */
    CMD_RESP_OK,            /* Received OK */
    CMD_RESP_ERROR,         /* Received ERROR */
    CMD_RESP_TIMEOUT,       /* Timeout */
} cmd_response_t;

/**
 * @brief TTY connection status
 */
typedef enum {
    TTY_STATE_DISCONNECTED = 0,  /* Disconnected */
    TTY_STATE_CONNECTING,        /* Connecting */
    TTY_STATE_CONNECTED,         /* Connected */
} tty_state_t;

/**
 * @brief TTY context
 */
typedef struct {
    int fd;                         /* TTY file descriptor */
    pthread_t recv_thread;          /* Receive thread */
    pthread_t reconnect_thread;     /* Reconnect thread */
    bool running;                   /* Thread running flag */
    pthread_mutex_t tx_mutex;       /* Send mutex */

    /* Connection management */
    tty_state_t state;              /* Connection status */
    pthread_mutex_t state_mutex;    /* Status mutex */
    pthread_cond_t state_cond;      /* Status condition variable */
    char dev_path[256];             /* TTY device path */
    int baudrate;                   /* Baud rate */

    /* Command response management */
    cmd_response_t cmd_resp;        /* Current command response status */
    pthread_mutex_t resp_mutex;     /* Response status mutex */
    pthread_cond_t resp_cond;       /* Response condition variable */
    char last_resp_line[256];       /* Last received non-OK/ERROR line */

    /* Receive thread control (for OTA exclusive access) */
    bool recv_thread_suspended;     /* Suspend flag for receive thread */
    pthread_cond_t suspend_cond;    /* Condition variable for suspend/resume */

    /* Callback functions (for notifying upper layer of data arrival) */
    void (*data_callback)(const char *line, void *user_data);
    void (*disconnect_callback)(void);  /* Disconnect callback */
    void (*reconnect_callback)(void);   /* Reconnect success callback */
    void *user_data;
} bflbwifi_tty_t;

/* Global TTY instance */
static bflbwifi_tty_t *g_tty = NULL;

/**
 * @brief Set serial port parameters
 */
static int tty_set_params(int fd, int baudrate)
{
    struct termios options;

    if (tcgetattr(fd, &options) != 0) {
        BFLB_LOGE("tcgetattr failed: %s", strerror(errno));
        return -1;
    }

    /* Set baud rate */
    speed_t speed;
    switch (baudrate) {
        case 9600:   speed = B9600; break;
        case 19200:  speed = B19200; break;
        case 38400:  speed = B38400; break;
        case 57600:  speed = B57600; break;
        case 115200: speed = B115200; break;
        case 230400: speed = B230400; break;
        case 460800: speed = B460800; break;
        case 921600: speed = B921600; break;
        default:
            BFLB_LOGE("Unsupported baudrate: %d", baudrate);
            return -1;
    }

    cfsetispeed(&options, speed);
    cfsetospeed(&options, speed);

    /* 8N1 mode */
    options.c_cflag &= ~PARENB;  /* No parity */
    options.c_cflag &= ~CSTOPB;  /* 1 stop bit */
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;      /* 8 data bits */
    options.c_cflag |= CREAD | CLOCAL;

    /* Disable hardware flow control */
    options.c_cflag &= ~CRTSCTS;

    /* Raw input mode */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* Raw output mode */
    options.c_oflag &= ~OPOST;

    /* Disable software flow control */
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* Set timeout and minimum character count */
    options.c_cc[VTIME] = 1;    /* 0.1 second timeout */
    options.c_cc[VMIN] = 0;     /* Don't block waiting */

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        BFLB_LOGE("tcsetattr failed: %s", strerror(errno));
        return -1;
    }

    /* Flush buffers */
    tcflush(fd, TCIOFLUSH);

    return 0;
}

/* Function forward declarations */
void bflbwifi_tty_notify_response(cmd_response_t resp);
void bflbwifi_tty_set_disconnect_callback(void (*callback)(void));
void bflbwifi_tty_set_reconnect_callback(void (*callback)(void));

/**
 * @brief Open TTY device
 */
static int tty_open_device(const char *dev, int baudrate)
{
    int fd;

    fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        return -1;
    }

    if (tty_set_params(fd, baudrate) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

/**
 * @brief Close TTY device
 */
static void tty_close_device(void)
{
    if (g_tty && g_tty->fd >= 0) {
        close(g_tty->fd);
        g_tty->fd = -1;
    }
}

/**
 * @brief Set TTY connection status
 */
static void tty_set_state(tty_state_t new_state)
{
    if (!g_tty) {
        return;
    }

    pthread_mutex_lock(&g_tty->state_mutex);
    g_tty->state = new_state;
    pthread_cond_broadcast(&g_tty->state_cond);
    pthread_mutex_unlock(&g_tty->state_mutex);
}

/**
 * @brief Reconnect thread
 */
static void* tty_reconnect_thread(void *arg)
{
    bflbwifi_tty_t *tty = (bflbwifi_tty_t *)arg;
    int fd;

    BFLB_LOGI("Reconnect thread started");

    while (tty->running) {
        /* If already connected, wait for disconnect */
        pthread_mutex_lock(&tty->state_mutex);
        while (tty->state == TTY_STATE_CONNECTED && tty->running) {
            pthread_cond_wait(&tty->state_cond, &tty->state_mutex);
        }
        pthread_mutex_unlock(&tty->state_mutex);

        if (!tty->running) {
            break;
        }

        /* Set connecting status */
        tty_set_state(TTY_STATE_CONNECTING);
        BFLB_LOGI("Attempting to reconnect to %s...", tty->dev_path);

        /* Try to reopen device */
        fd = tty_open_device(tty->dev_path, tty->baudrate);
        if (fd < 0) {
            BFLB_LOGW("Reconnect failed, will retry in 2 seconds");
            sleep(2);
            continue;
        }

        /* Close old device (if already open) */
        if (tty->fd >= 0) {
            close(tty->fd);
        }

        tty->fd = fd;
        tty_set_state(TTY_STATE_CONNECTED);
        BFLB_LOGI("Reconnected to %s", tty->dev_path);

        /* Notify upper layer of successful reconnection */
        if (tty->reconnect_callback) {
            tty->reconnect_callback();
        }

        /* Restart receive thread */
        /* Note: Receive thread is always running, just blocks on read, will continue automatically when device is available */
    }

    BFLB_LOGI("Reconnect thread stopped");
    return NULL;
}

/**
 * @brief Receive thread
 */
static void* tty_recv_thread(void *arg)
{
    bflbwifi_tty_t *tty = (bflbwifi_tty_t *)arg;
    char buf[TTY_RECV_BUF_SIZE];
    char line_buf[TTY_LINE_BUF_SIZE];
    int line_pos = 0;
    fd_set readfds;
    struct timeval tv;
    int ret;

    BFLB_LOGI("TTY recv thread started");

    while (tty->running) {
        /* Check if suspended (for OTA exclusive access) */
        pthread_mutex_lock(&tty->resp_mutex);
        while (tty->recv_thread_suspended && tty->running) {
            pthread_cond_wait(&tty->suspend_cond, &tty->resp_mutex);
        }
        pthread_mutex_unlock(&tty->resp_mutex);

        if (!tty->running) {
            break;
        }

        /* Check if device is connected */
        if (tty->fd < 0) {
            usleep(100000);  /* 100ms */
            continue;
        }

        /* Use select to wait for data */
        FD_ZERO(&readfds);
        FD_SET(tty->fd, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;  /* 100ms timeout */

        ret = select(tty->fd + 1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {
            /* Device disconnect detection */
            if (errno == EBADF || errno == ENODEV) {
                BFLB_LOGW("TTY device disconnected");
                tty_close_device();
                tty_set_state(TTY_STATE_DISCONNECTED);
                if (tty->disconnect_callback) {
                    tty->disconnect_callback();
                }
                continue;
            }
            if (errno == EINTR) {
                continue;
            }
            BFLB_LOGE("select failed: %s", strerror(errno));
            break;
        }

        if (ret == 0) {
            /* Timeout, continue */
            continue;
        }

        /* Read data */
        ret = read(tty->fd, buf, sizeof(buf));
        if (ret <= 0) {
            /* Device disconnect detection */
            if (errno == EBADF || errno == ENODEV || ret == 0) {
                BFLB_LOGW("TTY device read error, marking as disconnected");
                tty_close_device();
                tty_set_state(TTY_STATE_DISCONNECTED);
                if (tty->disconnect_callback) {
                    tty->disconnect_callback();
                }
                continue;
            }
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                BFLB_LOGE("read failed: %s", strerror(errno));
                break;
            }
            continue;
        }

        /* Split data by lines */
        for (int i = 0; i < ret; i++) {
            if (buf[i] == '\n') {
                /* Complete line */
                line_buf[line_pos] = '\0';

                /* Remove \r (if present) */
                if (line_pos > 0 && line_buf[line_pos - 1] == '\r') {
                    line_buf[line_pos - 1] = '\0';
                }

                /* Debug log */
                BFLB_LOGD("TTY RX: %s", line_buf);

                /* Identify command response (OK/ERROR) and notify waiter */
                if (strcmp(line_buf, "OK") == 0) {
                    bflbwifi_tty_notify_response(CMD_RESP_OK);
                } else if (strcmp(line_buf, "ERROR") == 0) {
                    bflbwifi_tty_notify_response(CMD_RESP_ERROR);
                }

                /* Notify upper layer (parser will handle all lines, including OK/ERROR) */
                if (tty->data_callback && line_buf[0] != '\0') {
                    /* Save last response line (for queries like AT+GMR) */
                    if (strcmp(line_buf, "OK") != 0 && strcmp(line_buf, "ERROR") != 0) {
                        pthread_mutex_lock(&tty->resp_mutex);
                        strncpy(tty->last_resp_line, line_buf, sizeof(tty->last_resp_line) - 1);
                        tty->last_resp_line[sizeof(tty->last_resp_line) - 1] = '\0';
                        pthread_mutex_unlock(&tty->resp_mutex);

                        /* Signal waiters that new data arrived */
                        pthread_cond_signal(&tty->resp_cond);
                    }

                    tty->data_callback(line_buf, tty->user_data);
                }

                line_pos = 0;
            } else if (buf[i] == '\r') {
                /* Ignore \r, wait for \n */
                continue;
            } else {
                /* Add to line buffer */
                if (line_pos < TTY_LINE_BUF_SIZE - 1) {
                    line_buf[line_pos++] = buf[i];
                } else {
                    /* Line too long, discard */
                    line_pos = 0;
                }
            }
        }
    }

    BFLB_LOGI("TTY recv thread stopped");
    return NULL;
}

/**
 * @brief Initialize TTY
 */
int bflbwifi_tty_init(const char *dev, int baudrate,
                  void (*callback)(const char *line, void *user_data),
                  void *user_data)
{
    int fd;

    if (!dev || !callback) {
        BFLB_LOGE("Invalid parameters");
        return -1;
    }

    /* Already initialized */
    if (g_tty) {
        BFLB_LOGE("TTY already initialized");
        return -1;
    }

    /* Open TTY device */
    fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        BFLB_LOGE("Failed to open %s: %s", dev, strerror(errno));
        return -1;
    }

    /* Configure serial port parameters */
    if (tty_set_params(fd, baudrate) != 0) {
        close(fd);
        return -1;
    }

    /* Allocate TTY context */
    g_tty = (bflbwifi_tty_t *)calloc(1, sizeof(bflbwifi_tty_t));
    if (!g_tty) {
        BFLB_LOGE("Failed to allocate memory");
        close(fd);
        return -1;
    }

    g_tty->fd = fd;
    g_tty->data_callback = callback;
    g_tty->user_data = user_data;
    g_tty->running = true;
    g_tty->cmd_resp = CMD_RESP_PENDING;
    g_tty->state = TTY_STATE_DISCONNECTED;
    g_tty->disconnect_callback = NULL;
    g_tty->reconnect_callback = NULL;

    /* Save device path and baud rate */
    strncpy(g_tty->dev_path, dev, sizeof(g_tty->dev_path) - 1);
    g_tty->dev_path[sizeof(g_tty->dev_path) - 1] = '\0';
    g_tty->baudrate = baudrate;

    /* Initialize send mutex */
    if (pthread_mutex_init(&g_tty->tx_mutex, NULL) != 0) {
        BFLB_LOGE("Failed to init tx_mutex");
        free(g_tty);
        close(fd);
        return -1;
    }

    /* Initialize state management mutex and condition variable */
    if (pthread_mutex_init(&g_tty->state_mutex, NULL) != 0) {
        BFLB_LOGE("Failed to init state_mutex");
        pthread_mutex_destroy(&g_tty->tx_mutex);
        free(g_tty);
        close(fd);
        return -1;
    }

    if (pthread_cond_init(&g_tty->state_cond, NULL) != 0) {
        BFLB_LOGE("Failed to init state_cond");
        pthread_mutex_destroy(&g_tty->state_mutex);
        pthread_mutex_destroy(&g_tty->tx_mutex);
        free(g_tty);
        close(fd);
        return -1;
    }

    /* Initialize response management mutex and condition variable */
    if (pthread_mutex_init(&g_tty->resp_mutex, NULL) != 0) {
        BFLB_LOGE("Failed to init resp_mutex");
        pthread_cond_destroy(&g_tty->state_cond);
        pthread_mutex_destroy(&g_tty->state_mutex);
        pthread_mutex_destroy(&g_tty->tx_mutex);
        free(g_tty);
        close(fd);
        return -1;
    }

    if (pthread_cond_init(&g_tty->resp_cond, NULL) != 0) {
        BFLB_LOGE("Failed to init resp_cond");
        pthread_mutex_destroy(&g_tty->resp_mutex);
        pthread_cond_destroy(&g_tty->state_cond);
        pthread_mutex_destroy(&g_tty->state_mutex);
        pthread_mutex_destroy(&g_tty->tx_mutex);
        free(g_tty);
        close(fd);
        return -1;
    }

    /* Start receive thread */
    if (pthread_create(&g_tty->recv_thread, NULL, tty_recv_thread, g_tty) != 0) {
        BFLB_LOGE("Failed to create recv thread");
        pthread_cond_destroy(&g_tty->resp_cond);
        pthread_mutex_destroy(&g_tty->resp_mutex);
        pthread_cond_destroy(&g_tty->state_cond);
        pthread_mutex_destroy(&g_tty->state_mutex);
        pthread_mutex_destroy(&g_tty->tx_mutex);
        free(g_tty);
        close(fd);
        g_tty = NULL;
        return -1;
    }

    /* Start reconnect thread */
    if (pthread_create(&g_tty->reconnect_thread, NULL, tty_reconnect_thread, g_tty) != 0) {
        BFLB_LOGE("Failed to create reconnect thread");
        g_tty->running = false;
        pthread_join(g_tty->recv_thread, NULL);
        pthread_cond_destroy(&g_tty->resp_cond);
        pthread_mutex_destroy(&g_tty->resp_mutex);
        pthread_cond_destroy(&g_tty->state_cond);
        pthread_mutex_destroy(&g_tty->state_mutex);
        pthread_mutex_destroy(&g_tty->tx_mutex);
        free(g_tty);
        close(fd);
        g_tty = NULL;
        return -1;
    }

    /* Set to connected status */
    tty_set_state(TTY_STATE_CONNECTED);

    BFLB_LOGI("TTY initialized: %s @ %d baud", dev, baudrate);

    return 0;
}

/**
 * @brief Send data
 */
int bflbwifi_tty_send(const char *data, int len)
{
    int ret;

    if (!g_tty) {
        BFLB_LOGE("TTY not initialized");
        return -1;
    }

    if (!data || len <= 0) {
        BFLB_LOGE("Invalid parameters");
        return -1;
    }

    /* Lock to avoid multiple threads sending at the same time */
    pthread_mutex_lock(&g_tty->tx_mutex);

    /* Debug log */
    BFLB_LOGD("TTY TX: %.*s", len, data);

    /* Send data */
    ret = write(g_tty->fd, data, len);
    if (ret != len) {
        BFLB_LOGE("Write failed: %s", strerror(errno));
        pthread_mutex_unlock(&g_tty->tx_mutex);
        return -1;
    }

    /* Wait for data transmission to complete */
    tcdrain(g_tty->fd);

    pthread_mutex_unlock(&g_tty->tx_mutex);

    return ret;
}

/**
 * @brief Send AT command (automatically add \r\n)
 */
int bflbwifi_tty_send_command(const char *cmd)
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

    return bflbwifi_tty_send(buf, len);
}

/**
 * @brief Notify that command response was received (called by receive thread)
 * @param resp Response type (CMD_RESP_OK or CMD_RESP_ERROR)
 * @note This function is thread-safe and can be called in receive thread
 */
void bflbwifi_tty_notify_response(cmd_response_t resp)
{
    if (!g_tty) {
        return;
    }

    /* Quickly update response status (short critical section) */
    pthread_mutex_lock(&g_tty->resp_mutex);
    g_tty->cmd_resp = resp;
    pthread_cond_signal(&g_tty->resp_cond);
    pthread_mutex_unlock(&g_tty->resp_mutex);

    BFLB_LOGD("Command response: %s", (resp == CMD_RESP_OK) ? "OK" : "ERROR");
}

/**
 * @brief Send command and synchronously wait for response
 * @param cmd AT command (without \r\n)
 * @param timeout_ms Timeout in milliseconds
 * @return >=0 for success (response status), <0 for error
 *
 * @note This function will block until OK/ERROR is received or timeout
 * @note Timeout returns CMD_RESP_TIMEOUT, failure returns -1
 */
int bflbwifi_tty_send_command_sync(const char *cmd, int timeout_ms)
{
    int ret;
    struct timespec ts;
    cmd_response_t result;

    if (!g_tty || !cmd) {
        return -1;
    }

    if (timeout_ms <= 0) {
        timeout_ms = 5000;  /* Default 5 seconds */
    }

    /* 1. Send command (protected by tx_mutex) */
    ret = bflbwifi_tty_send_command(cmd);
    if (ret < 0) {
        return -1;
    }

    /* 2. Wait for response (protected by resp_mutex) */
    /* Calculate timeout */
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    /* Wait for response */
    pthread_mutex_lock(&g_tty->resp_mutex);
    g_tty->cmd_resp = CMD_RESP_PENDING;

    while (g_tty->cmd_resp == CMD_RESP_PENDING) {
        ret = pthread_cond_timedwait(&g_tty->resp_cond, &g_tty->resp_mutex, &ts);
        if (ret != 0) {
            /* Timeout or other error */
            g_tty->cmd_resp = CMD_RESP_TIMEOUT;
            break;
        }
    }

    result = g_tty->cmd_resp;
    pthread_mutex_unlock(&g_tty->resp_mutex);

    if (result == CMD_RESP_TIMEOUT) {
        BFLB_LOGE("Command timeout: %s", cmd);
        return CMD_RESP_TIMEOUT;
    }

    if (result == CMD_RESP_ERROR) {
        BFLB_LOGE("Command failed: %s", cmd);
        return CMD_RESP_ERROR;
    }

    BFLB_LOGD("Command OK: %s", cmd);
    return CMD_RESP_OK;
}

/**
 * @brief Clean up TTY
 */
void bflbwifi_tty_deinit(void)
{
    if (!g_tty) {
        return;
    }

    BFLB_LOGI("Deinitializing TTY...");

    /* Stop receive thread and reconnect thread */
    g_tty->running = false;

    /* Wake up reconnect thread */
    pthread_cond_broadcast(&g_tty->state_cond);

    pthread_join(g_tty->recv_thread, NULL);
    pthread_join(g_tty->reconnect_thread, NULL);

    /* Close TTY */
    if (g_tty->fd >= 0) {
        close(g_tty->fd);
    }

    /* Destroy locks and condition variables */
    pthread_cond_destroy(&g_tty->resp_cond);
    pthread_mutex_destroy(&g_tty->resp_mutex);
    pthread_cond_destroy(&g_tty->state_cond);
    pthread_mutex_destroy(&g_tty->state_mutex);
    pthread_mutex_destroy(&g_tty->tx_mutex);

    /* Free memory */
    free(g_tty);
    g_tty = NULL;

    BFLB_LOGI("TTY deinitialized");
}

/**
 * @brief Set disconnect callback
 */
void bflbwifi_tty_set_disconnect_callback(void (*callback)(void))
{
    if (g_tty) {
        g_tty->disconnect_callback = callback;
    }
}

/**
 * @brief Set reconnect callback
 */
void bflbwifi_tty_set_reconnect_callback(void (*callback)(void))
{
    if (g_tty) {
        g_tty->reconnect_callback = callback;
    }
}

/**
 * @brief Get last response line
 * @param buf Output buffer
 * @param len Buffer size
 * @return 0 for success, -1 for failure
 */
int bflbwifi_tty_get_last_response(char *buf, size_t len)
{
    if (!g_tty || !buf || len == 0) {
        return -1;
    }

    pthread_mutex_lock(&g_tty->resp_mutex);
    strncpy(buf, g_tty->last_resp_line, len - 1);
    buf[len - 1] = '\0';
    pthread_mutex_unlock(&g_tty->resp_mutex);

    return 0;
}

/**
 * @brief Write raw binary data to TTY (thread-safe)
 * @param data Data buffer
 * @param len Data length
 * @return Number of bytes written, -1 on error
 *
 * @note This function holds tx_mutex during write
 * @note This does NOT send AT command format, just raw bytes
 */
int bflbwifi_tty_write_raw(const void *data, int len)
{
    int ret;

    if (!g_tty || !data || len <= 0) {
        return -1;
    }

    /* Lock to prevent concurrent writes */
    pthread_mutex_lock(&g_tty->tx_mutex);

    /* Direct write to TTY */
    ret = write(g_tty->fd, data, len);
    if (ret < 0) {
        BFLB_LOGE("Failed to write raw data: %s", strerror(errno));
    }

    pthread_mutex_unlock(&g_tty->tx_mutex);

    return ret;
}

/**
 * @brief Wait for specific string in response (with timeout)
 * @param expected Expected string (e.g., "SEND OK")
 * @param timeout_ms Timeout in milliseconds
 * @return 0 if found, -1 on timeout/error
 *
 * @note This function uses a temporary buffer to scan for expected string
 * @note Thread-safe: Uses resp_mutex but releases it during sleep
 */
int bflbwifi_tty_wait_for(const char *expected, int timeout_ms)
{
    struct timespec ts;
    struct timeval tv;
    int ret;

    if (!g_tty || !expected) {
        return -1;
    }

    /* Calculate timeout */
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + timeout_ms / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    /* Wait for expected string in last_resp_line */
    pthread_mutex_lock(&g_tty->resp_mutex);

    while (g_tty->running) {
        /* Check if expected string found */
        if (strstr(g_tty->last_resp_line, expected) != NULL) {
            pthread_mutex_unlock(&g_tty->resp_mutex);
            BFLB_LOGD("Found expected string: %s", expected);
            return 0;
        }

        /* Wait for new data (100ms poll) */
        ret = pthread_cond_timedwait(&g_tty->resp_cond, &g_tty->resp_mutex, &ts);

        if (ret == ETIMEDOUT) {
            pthread_mutex_unlock(&g_tty->resp_mutex);
            BFLB_LOGE("Timeout waiting for: %s", expected);
            return -1;
        }
    }

    pthread_mutex_unlock(&g_tty->resp_mutex);
    return -1;
}

/**
 * @brief Clear last response line
 * @note This is useful to avoid matching old data
 */
void bflbwifi_tty_clear_last_response(void)
{
    if (!g_tty) {
        return;
    }

    pthread_mutex_lock(&g_tty->resp_mutex);
    g_tty->last_resp_line[0] = '\0';
    pthread_mutex_unlock(&g_tty->resp_mutex);
}

/**
 * @brief Suspend receive thread (for OTA exclusive access)
 * @return 0 for success, -1 for failure
 *
 * @note This function blocks until receive thread is confirmed suspended
 * @note OTA operations can then safely use g_tty->fd directly
 */
int bflbwifi_tty_suspend_recv_thread(void)
{
    if (!g_tty) {
        return -1;
    }

    BFLB_LOGI("Suspending receive thread for OTA...");

    pthread_mutex_lock(&g_tty->resp_mutex);
    g_tty->recv_thread_suspended = true;

    /* Wait a bit to ensure receive thread is in select() and will see the flag */
    pthread_mutex_unlock(&g_tty->resp_mutex);
    usleep(200000);  /* 200ms */

    BFLB_LOGI("Receive thread suspended");
    return 0;
}

/**
 * @brief Resume receive thread (after OTA completes)
 * @return 0 for success, -1 for failure
 */
int bflbwifi_tty_resume_recv_thread(void)
{
    if (!g_tty) {
        return -1;
    }

    BFLB_LOGI("Resuming receive thread...");

    pthread_mutex_lock(&g_tty->resp_mutex);
    g_tty->recv_thread_suspended = false;
    pthread_cond_signal(&g_tty->suspend_cond);
    pthread_mutex_unlock(&g_tty->resp_mutex);

    BFLB_LOGI("Receive thread resumed");
    return 0;
}

/**
 * @brief Get TTY file descriptor (for OTA direct access)
 * @return TTY fd, or -1 if not initialized
 */
int bflbwifi_tty_get_fd(void)
{
    if (!g_tty) {
        return -1;
    }
    return g_tty->fd;
}

/**
 * @brief Synchronous wait for specific string on TTY (bypasses receive thread)
 * @param expected Expected string to wait for
 * @param timeout_ms Timeout in milliseconds
 * @return 0 for success (found), -1 for timeout/error
 *
 * @note This function directly reads from TTY fd, bypassing the receive thread
 * @note This is useful for binary data transfer (e.g., OTA) where line-based parsing fails
 * @note This function mimics the behavior of easyat.c's wait_response()
 */
int bflbwifi_tty_sync_wait(const char *expected, int timeout_ms)
{
    char buffer[2048];
    int buffer_pos = 0;
    time_t start_time;
    fd_set read_fds;
    struct timeval tv;
    int ret;

    if (!g_tty || !expected) {
        return -1;
    }

    if (g_tty->fd < 0) {
        BFLB_LOGE("TTY not connected");
        return -1;
    }

    start_time = time(NULL);
    buffer_pos = 0;

    while (time(NULL) - start_time < timeout_ms / 1000) {
        /* Use select to wait for data */
        FD_ZERO(&read_fds);
        FD_SET(g_tty->fd, &read_fds);

        tv.tv_sec = 1;  /* Wait for 1 second each iteration */
        tv.tv_usec = 0;

        ret = select(g_tty->fd + 1, &read_fds, NULL, NULL, &tv);

        if (ret < 0) {
            if (errno == EINTR) {
                continue;  /* Interrupted by signal, continue */
            }
            BFLB_LOGE("select failed: %s", strerror(errno));
            return -1;
        } else if (ret == 0) {
            /* Timeout, continue loop */
            continue;
        }

        if (FD_ISSET(g_tty->fd, &read_fds)) {
            /* Read data */
            int bytes_read = read(g_tty->fd, buffer + buffer_pos,
                                  sizeof(buffer) - buffer_pos - 1);
            if (bytes_read > 0) {
                buffer_pos += bytes_read;
                buffer[buffer_pos] = '\0';

                /* Display received data (for debugging) */
                BFLB_LOGD("TTY RX raw: %.*s", bytes_read,
                         buffer + buffer_pos - bytes_read);

                /* Check if target string received */
                if (strstr(buffer, expected) != NULL) {
                    BFLB_LOGD("Found expected string: %s", expected);
                    return 0;  /* Successfully found target string */
                }

                /* Prevent buffer overflow - shift data if buffer half full */
                if (buffer_pos > (int)sizeof(buffer) / 2) {
                    memmove(buffer, buffer + buffer_pos / 2, buffer_pos / 2);
                    buffer_pos /= 2;
                }
            } else if (bytes_read < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;  /* Non-blocking read, try again */
                }
                BFLB_LOGE("read failed: %s", strerror(errno));
                return -1;
            } else {
                /* bytes_read == 0, connection closed */
                BFLB_LOGE("TTY connection closed");
                return -1;
            }
        }
    }

    BFLB_LOGE("Timeout waiting for: %s", expected);
    return -1;  /* Timeout, target string not found */
}
