/**
 * @file nethub_vchan.c
 * @brief BFLB Virtual Channel userspace library
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <termios.h>
#include <linux/netlink.h>

#include "nethub_vchan.h"

/* ==================== Internal Definitions ==================== */

/* Netlink protocol constants */
#define NETHUB_VCHAN_PROTO 31
#define NETHUB_VCHAN_GROUP 1

#define NETHUB_VCHAN_USB_DEFAULT_DEV  "/dev/ttyACM0"
#define NETHUB_VCHAN_USB_WRITE_TIMEOUT_MS 1000
#define NETHUB_VCHAN_USB_SCAN_MAX 16
#define NETHUB_VCHAN_USB_SYSFS_VALUE_LEN 256
#define NETHUB_VCHAN_USB_RECONNECT_INTERVAL_MS 500
#define NETHUB_VCHAN_USB_SEND_RETRIES 2
#define NETHUB_VCHAN_USB_SEND_RETRY_DELAY_MS 100

/*
 * USB ACM framing: magic(A5 5A) + len_le16 + payload + checksum_le32.
 * Invalid magic/len/checksum drops one byte and resyncs on the stream.
 */
#define NETHUB_VCHAN_USB_MAGIC0       0xA5u
#define NETHUB_VCHAN_USB_MAGIC1       0x5Au
#define NETHUB_VCHAN_USB_CHECKSUM     0xA5A55A5Au
#define NETHUB_VCHAN_USB_MAGIC0_OFF   0u
#define NETHUB_VCHAN_USB_MAGIC1_OFF   1u
#define NETHUB_VCHAN_USB_LEN_OFF      2u
#define NETHUB_VCHAN_USB_HDR_LEN      4u
#define NETHUB_VCHAN_USB_CHECKSUM_LEN 4u
#define NETHUB_VCHAN_USB_FRAME_MAX \
    (NETHUB_VCHAN_USB_HDR_LEN + NETHUB_VCHAN_MAX_DATA_LEN + NETHUB_VCHAN_USB_CHECKSUM_LEN)
#define NETHUB_VCHAN_USB_RX_DATA_TYPE NETHUB_VCHAN_DATA_TYPE_AT

enum nethub_vchan_transport {
    NETHUB_VCHAN_TRANSPORT_NETLINK = 0,
    NETHUB_VCHAN_TRANSPORT_USB_ACM,
};

/**
 * @struct nethub_vchan_data_hdr
 * @brief Internal message header for virtual channel payload
 *
 * Layout:
 * +----------+-----------+-------+--------+
 * | data_type| reserved  | len   | data[] |
 * | 1 byte   | 1 byte    | 2 bytes| N bytes|
 * +----------+-----------+-------+--------+
 *
 * @note len field stores the actual user data length to solve Netlink 4-byte alignment issue
 */
#pragma pack(push, 1)
struct nethub_vchan_data_hdr {
    uint8_t data_type; /* Data type (see NETHUB_VCHAN_DATA_TYPE_* defines) */
    uint8_t reserved;  /* Reserved field (initialized to 0) */
    uint16_t len;      /* Actual user data length (exclude header) */
    uint8_t data[];    /* Actual user data (flexible array) */
} __attribute__((packed));
#pragma pack(pop)

#define NETHUB_VCHAN_NETLINK_HDR_LEN ((size_t)sizeof(struct nethub_vchan_data_hdr))

#define NETHUB_VCHAN_CTRL_MAGIC   0x4e485643u /* "NHVC" */
#define NETHUB_VCHAN_CTRL_VERSION 1u

#define NETHUB_VCHAN_CTRL_MSG_QUERY_LINK_STATE 1u
#define NETHUB_VCHAN_CTRL_MSG_LINK_STATE_EVENT 2u

#pragma pack(push, 1)
struct nethub_vchan_ctrl_msg {
    uint32_t magic;
    uint8_t version;
    uint8_t msg_type;
    uint8_t link_state;
    uint8_t host_state;
} __attribute__((packed));
#pragma pack(pop)

/* ==================== Context Structure ==================== */

/* BFLB Virtual Channel context */
struct nethub_vchan_ctx {
    int sock_fd;
    int stopping;
    enum nethub_vchan_transport transport;
    pthread_t recv_thread;                                              /* Receive thread handle (0 if not created) */
    nethub_vchan_recv_callback_t callbacks[NETHUB_VCHAN_DATA_TYPE_MAX]; /* Per-type callbacks */
    pthread_mutex_t state_mutex;
    pthread_cond_t state_cond;
    pthread_mutex_t tx_mutex;
    pthread_mutex_t callback_mutex;
    uint32_t state_generation;
    nethub_vchan_link_state_t link_state;
    nethub_vchan_host_state_t host_state;
    nethub_vchan_link_event_callback_t link_event_callback;
    void *link_event_user_data;
    uint8_t usb_rx_buf[NETHUB_VCHAN_USB_FRAME_MAX];
    size_t usb_rx_len;
};

/* Global singleton handle */
static struct nethub_vchan_ctx *g_nethub_vchan_ctx = NULL;

/* Forward declaration */
static void *nethub_vchan_recv_thread_func(void *arg);
static int nethub_vchan_send_raw(struct nethub_vchan_ctx *ctx, uint8_t data_type, const void *data, size_t len);
static int nethub_vchan_open_transport(struct nethub_vchan_ctx *ctx);
static int nethub_vchan_open_netlink(struct nethub_vchan_ctx *ctx, int quiet);
static int nethub_vchan_open_usb_acm(struct nethub_vchan_ctx *ctx, const char *dev_path, int quiet);
static int nethub_vchan_open_usb_auto(struct nethub_vchan_ctx *ctx, int quiet);
static int nethub_vchan_send_link_query(struct nethub_vchan_ctx *ctx);
static nethub_vchan_host_state_t nethub_vchan_normalize_host_state(uint8_t host_state);
static void nethub_vchan_update_state(struct nethub_vchan_ctx *ctx,
                                      nethub_vchan_link_state_t link_state,
                                      nethub_vchan_host_state_t host_state);
static int nethub_vchan_try_handle_ctrl_msg(struct nethub_vchan_ctx *ctx,
                                            uint8_t data_type,
                                            const void *data,
                                            size_t len);
static int nethub_vchan_data_type_is_valid(uint8_t data_type);
static void nethub_vchan_dispatch_data(struct nethub_vchan_ctx *ctx,
                                       uint8_t data_type,
                                       const void *data,
                                       size_t len);
static void nethub_vchan_usb_frame_build(uint8_t *frame, const void *data, size_t len);
static void nethub_vchan_usb_stream_feed(struct nethub_vchan_ctx *ctx, const uint8_t *data, size_t len);
static int nethub_vchan_wait_fd_writable(int fd, int timeout_ms);
static int nethub_vchan_write_all(int fd, const uint8_t *data, size_t len);
static int nethub_vchan_start_recv_thread(struct nethub_vchan_ctx *ctx);
static void nethub_vchan_stop_recv_thread(struct nethub_vchan_ctx *ctx);
static int nethub_vchan_wait_initial_link(struct nethub_vchan_ctx *ctx, int timeout_ms);
static int nethub_vchan_current_link_is_up(struct nethub_vchan_ctx *ctx);
static int nethub_vchan_auto_transport_enabled(void);
static int nethub_vchan_usb_candidate_score(int index);
static int nethub_vchan_usb_try_reconnect_locked(struct nethub_vchan_ctx *ctx);
static void nethub_vchan_usb_mark_down(struct nethub_vchan_ctx *ctx, int expected_fd);
static void nethub_vchan_sleep_ms(int timeout_ms);

static int nethub_vchan_data_type_is_valid(uint8_t data_type)
{
    return data_type > 0 && data_type < NETHUB_VCHAN_DATA_TYPE_MAX;
}

static uint16_t nethub_vchan_get_le16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t nethub_vchan_get_le32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void nethub_vchan_put_le16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)(value & 0xffu);
    p[1] = (uint8_t)((value >> 8) & 0xffu);
}

static void nethub_vchan_put_le32(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)(value & 0xffu);
    p[1] = (uint8_t)((value >> 8) & 0xffu);
    p[2] = (uint8_t)((value >> 16) & 0xffu);
    p[3] = (uint8_t)((value >> 24) & 0xffu);
}

static void nethub_vchan_usb_frame_build(uint8_t *frame, const void *data, size_t len)
{
    frame[NETHUB_VCHAN_USB_MAGIC0_OFF] = NETHUB_VCHAN_USB_MAGIC0;
    frame[NETHUB_VCHAN_USB_MAGIC1_OFF] = NETHUB_VCHAN_USB_MAGIC1;
    nethub_vchan_put_le16(&frame[NETHUB_VCHAN_USB_LEN_OFF], (uint16_t)len);
    memcpy(&frame[NETHUB_VCHAN_USB_HDR_LEN], data, len);
    nethub_vchan_put_le32(&frame[NETHUB_VCHAN_USB_HDR_LEN + len], NETHUB_VCHAN_USB_CHECKSUM);
}

static void nethub_vchan_dispatch_data(struct nethub_vchan_ctx *ctx,
                                       uint8_t data_type,
                                       const void *data,
                                       size_t len)
{
    nethub_vchan_recv_callback_t callback = NULL;

    if (nethub_vchan_try_handle_ctrl_msg(ctx, data_type, data, len)) {
        return;
    }

    if (!nethub_vchan_data_type_is_valid(data_type)) {
        return;
    }

    pthread_mutex_lock(&ctx->callback_mutex);
    callback = ctx->callbacks[data_type];
    pthread_mutex_unlock(&ctx->callback_mutex);

    if (callback) {
        callback(data, len);
    }
}

static void nethub_vchan_usb_stream_drop(struct nethub_vchan_ctx *ctx, size_t len)
{
    if (len >= ctx->usb_rx_len) {
        ctx->usb_rx_len = 0;
        return;
    }

    memmove(ctx->usb_rx_buf, ctx->usb_rx_buf + len, ctx->usb_rx_len - len);
    ctx->usb_rx_len -= len;
}

static void nethub_vchan_usb_stream_process(struct nethub_vchan_ctx *ctx)
{
    while (ctx->usb_rx_len >= 2) {
        uint16_t payload_len;
        size_t frame_len;

        if (ctx->usb_rx_buf[NETHUB_VCHAN_USB_MAGIC0_OFF] != NETHUB_VCHAN_USB_MAGIC0 ||
            ctx->usb_rx_buf[NETHUB_VCHAN_USB_MAGIC1_OFF] != NETHUB_VCHAN_USB_MAGIC1) {
            nethub_vchan_usb_stream_drop(ctx, 1);
            continue;
        }

        if (ctx->usb_rx_len < NETHUB_VCHAN_USB_HDR_LEN) {
            return;
        }

        payload_len = nethub_vchan_get_le16(&ctx->usb_rx_buf[NETHUB_VCHAN_USB_LEN_OFF]);
        if (payload_len == 0 ||
            payload_len > NETHUB_VCHAN_MAX_DATA_LEN) {
            nethub_vchan_usb_stream_drop(ctx, 1);
            continue;
        }

        frame_len = NETHUB_VCHAN_USB_HDR_LEN + payload_len + NETHUB_VCHAN_USB_CHECKSUM_LEN;
        if (ctx->usb_rx_len < frame_len) {
            return;
        }

        if (nethub_vchan_get_le32(&ctx->usb_rx_buf[NETHUB_VCHAN_USB_HDR_LEN + payload_len]) !=
            NETHUB_VCHAN_USB_CHECKSUM) {
            nethub_vchan_usb_stream_drop(ctx, 1);
            continue;
        }

        nethub_vchan_dispatch_data(ctx,
                                   NETHUB_VCHAN_USB_RX_DATA_TYPE,
                                   &ctx->usb_rx_buf[NETHUB_VCHAN_USB_HDR_LEN],
                                   payload_len);
        nethub_vchan_usb_stream_drop(ctx, frame_len);
    }
}

static void nethub_vchan_usb_stream_feed(struct nethub_vchan_ctx *ctx, const uint8_t *data, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++) {
        if (ctx->usb_rx_len >= sizeof(ctx->usb_rx_buf)) {
            nethub_vchan_usb_stream_drop(ctx, 1);
        }
        ctx->usb_rx_buf[ctx->usb_rx_len++] = data[i];
        nethub_vchan_usb_stream_process(ctx);
    }
}

static int nethub_vchan_wait_fd_writable(int fd, int timeout_ms)
{
    int ret;

    do {
        fd_set writefds;
        struct timeval tv;

        FD_ZERO(&writefds);
        FD_SET(fd, &writefds);

        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        ret = select(fd + 1, NULL, &writefds, NULL, &tv);
    } while (ret < 0 && errno == EINTR);

    return ret;
}

static int nethub_vchan_write_all(int fd, const uint8_t *data, size_t len)
{
    size_t written = 0;

    while (written < len) {
        ssize_t ret = write(fd, data + written, len - written);

        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                ret = nethub_vchan_wait_fd_writable(fd, NETHUB_VCHAN_USB_WRITE_TIMEOUT_MS);
                if (ret > 0) {
                    continue;
                }
                if (ret == 0) {
                    fprintf(stderr, "write timeout\n");
                } else {
                    perror("select");
                }
                return -1;
            }
            perror("write");
            return -1;
        }

        if (ret == 0) {
            return -1;
        }

        written += (size_t)ret;
    }

    return 0;
}

static void nethub_vchan_sleep_ms(int timeout_ms)
{
    struct timespec req;

    if (timeout_ms <= 0) {
        return;
    }

    req.tv_sec = timeout_ms / 1000;
    req.tv_nsec = (long)(timeout_ms % 1000) * 1000000L;
    while (nanosleep(&req, &req) < 0 && errno == EINTR) {
    }
}

static int nethub_vchan_auto_transport_enabled(void)
{
    const char *transport = getenv("NETHUB_VCHAN_TRANSPORT");

    return transport == NULL || transport[0] == '\0';
}

static int nethub_vchan_start_recv_thread(struct nethub_vchan_ctx *ctx)
{
    int ret;

    if (!ctx) {
        return -1;
    }

    pthread_mutex_lock(&ctx->tx_mutex);
    ctx->stopping = 0;
    pthread_mutex_unlock(&ctx->tx_mutex);

    ret = pthread_create(&ctx->recv_thread, NULL, nethub_vchan_recv_thread_func, ctx);
    if (ret != 0) {
        fprintf(stderr, "Failed to create recv thread: %d\n", ret);
        ctx->recv_thread = 0;
        return -1;
    }

    return 0;
}

static void nethub_vchan_stop_recv_thread(struct nethub_vchan_ctx *ctx)
{
    int fd_to_close = -1;

    if (!ctx) {
        return;
    }

    pthread_mutex_lock(&ctx->tx_mutex);
    ctx->stopping = 1;
    if (ctx->sock_fd >= 0) {
        fd_to_close = ctx->sock_fd;
        ctx->sock_fd = -1;
    }
    pthread_mutex_unlock(&ctx->tx_mutex);

    if (ctx->recv_thread != 0) {
        pthread_join(ctx->recv_thread, NULL);
        ctx->recv_thread = 0;
    }

    if (fd_to_close >= 0) {
        close(fd_to_close);
    }
}

static int nethub_vchan_wait_initial_link(struct nethub_vchan_ctx *ctx, int timeout_ms)
{
    struct timespec ts;
    uint32_t generation;
    int ret = 0;

    if (!ctx || ctx->transport != NETHUB_VCHAN_TRANSPORT_NETLINK) {
        return -1;
    }

    if (nethub_vchan_send_link_query(ctx) < 0) {
        return -1;
    }

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    pthread_mutex_lock(&ctx->state_mutex);
    generation = ctx->state_generation;
    while (ctx->state_generation == generation) {
        ret = pthread_cond_timedwait(&ctx->state_cond, &ctx->state_mutex, &ts);
        if (ret == ETIMEDOUT) {
            break;
        }
    }
    pthread_mutex_unlock(&ctx->state_mutex);
    return ret == ETIMEDOUT ? -1 : 0;
}

static int nethub_vchan_current_link_is_up(struct nethub_vchan_ctx *ctx)
{
    int is_up;

    if (!ctx) {
        return 0;
    }

    pthread_mutex_lock(&ctx->state_mutex);
    is_up = (ctx->link_state == NETHUB_VCHAN_LINK_UP);
    pthread_mutex_unlock(&ctx->state_mutex);
    return is_up;
}

static void nethub_vchan_usb_mark_down(struct nethub_vchan_ctx *ctx, int expected_fd)
{
    int fd_to_close = -1;
    int link_down = 0;

    if (!ctx) {
        return;
    }

    pthread_mutex_lock(&ctx->tx_mutex);
    if (ctx->sock_fd >= 0 && (expected_fd < 0 || ctx->sock_fd == expected_fd)) {
        fd_to_close = ctx->sock_fd;
        ctx->sock_fd = -1;
        ctx->usb_rx_len = 0;
        link_down = 1;
    }
    pthread_mutex_unlock(&ctx->tx_mutex);

    if (fd_to_close >= 0) {
        close(fd_to_close);
    }

    if (link_down) {
        nethub_vchan_update_state(ctx, NETHUB_VCHAN_LINK_DOWN, NETHUB_VCHAN_HOST_STATE_UNKNOWN);
    }
}

static char nethub_vchan_ascii_lower(char ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return (char)(ch - 'A' + 'a');
    }

    return ch;
}

static int nethub_vchan_strcasestr_found(const char *text, const char *needle)
{
    size_t needle_len;

    if (!text || !needle) {
        return 0;
    }

    needle_len = strlen(needle);
    if (needle_len == 0) {
        return 1;
    }

    for (; *text; text++) {
        size_t i;

        for (i = 0; i < needle_len; i++) {
            if (text[i] == '\0' ||
                nethub_vchan_ascii_lower(text[i]) != nethub_vchan_ascii_lower(needle[i])) {
                break;
            }
        }

        if (i == needle_len) {
            return 1;
        }
    }

    return 0;
}

static int nethub_vchan_read_text_file(const char *path, char *buf, size_t buf_len)
{
    int fd;
    ssize_t len;

    if (!path || !buf || buf_len == 0) {
        return -1;
    }

    fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        return -1;
    }

    len = read(fd, buf, buf_len - 1u);
    close(fd);
    if (len <= 0) {
        return -1;
    }

    buf[len] = '\0';
    return 0;
}

static int nethub_vchan_usb_score_text(const char *text)
{
    int score = 0;

    if (nethub_vchan_strcasestr_found(text, "nethub")) {
        score += 100;
    }
    if (nethub_vchan_strcasestr_found(text, "bouffalo")) {
        score += 80;
    }
    if (nethub_vchan_strcasestr_found(text, "bflb")) {
        score += 60;
    }
    if (nethub_vchan_strcasestr_found(text, "ecm")) {
        score += 20;
    }
    if (nethub_vchan_strcasestr_found(text, "acm")) {
        score += 10;
    }

    return score;
}

static int nethub_vchan_usb_score_sysfs_path(const char *path)
{
    char value[NETHUB_VCHAN_USB_SYSFS_VALUE_LEN];

    if (nethub_vchan_read_text_file(path, value, sizeof(value)) != 0) {
        return 0;
    }

    return nethub_vchan_usb_score_text(value);
}

static int nethub_vchan_usb_candidate_score(int index)
{
    char path[128];
    int score = 0;

    snprintf(path, sizeof(path), "/sys/class/tty/ttyACM%d/device/interface", index);
    score += nethub_vchan_usb_score_sysfs_path(path);
    snprintf(path, sizeof(path), "/sys/class/tty/ttyACM%d/device/../product", index);
    score += nethub_vchan_usb_score_sysfs_path(path);
    snprintf(path, sizeof(path), "/sys/class/tty/ttyACM%d/device/../manufacturer", index);
    score += nethub_vchan_usb_score_sysfs_path(path);
    snprintf(path, sizeof(path), "/sys/class/tty/ttyACM%d/device/../../product", index);
    score += nethub_vchan_usb_score_sysfs_path(path);
    snprintf(path, sizeof(path), "/sys/class/tty/ttyACM%d/device/../../manufacturer", index);
    score += nethub_vchan_usb_score_sysfs_path(path);

    return score;
}

static int nethub_vchan_usb_try_reconnect_locked(struct nethub_vchan_ctx *ctx)
{
    int ret;

    if (!ctx || ctx->stopping) {
        return -1;
    }

    if (ctx->sock_fd >= 0) {
        return 0;
    }

    ret = nethub_vchan_open_usb_auto(ctx, 1);
    if (ret == 0) {
        ctx->usb_rx_len = 0;
        return 1;
    }

    return -1;
}

/**
 * @brief Initialize BFLB Virtual Channel (singleton)
 * @retval 0 Success
 * @retval <0 Error code
 */
int nethub_vchan_init(void)
{
    struct nethub_vchan_ctx *ctx;
    int ret;

    /* Check if already initialized */
    if (g_nethub_vchan_ctx != NULL) {
        fprintf(stderr, "NETHUB_VCHAN already initialized\n");
        return -1;
    }

    /* Allocate context */
    ctx = (struct nethub_vchan_ctx *)calloc(1, sizeof(struct nethub_vchan_ctx));
    if (!ctx) {
        perror("calloc");
        return -1;
    }
    ctx->sock_fd = -1;
    ctx->recv_thread = 0; /* Initialize thread handle to 0 (not created) */
    ctx->link_state = NETHUB_VCHAN_LINK_DOWN;
    ctx->host_state = NETHUB_VCHAN_HOST_STATE_UNKNOWN;

    /* Callbacks are already NULL due to calloc */

    if (pthread_mutex_init(&ctx->state_mutex, NULL) != 0) {
        fprintf(stderr, "Failed to create state mutex\n");
        free(ctx);
        return -1;
    }

    if (pthread_cond_init(&ctx->state_cond, NULL) != 0) {
        fprintf(stderr, "Failed to create state condition variable\n");
        pthread_mutex_destroy(&ctx->state_mutex);
        free(ctx);
        return -1;
    }

    if (pthread_mutex_init(&ctx->tx_mutex, NULL) != 0) {
        fprintf(stderr, "Failed to create tx mutex\n");
        pthread_cond_destroy(&ctx->state_cond);
        pthread_mutex_destroy(&ctx->state_mutex);
        free(ctx);
        return -1;
    }

    if (pthread_mutex_init(&ctx->callback_mutex, NULL) != 0) {
        fprintf(stderr, "Failed to create callback mutex\n");
        pthread_mutex_destroy(&ctx->tx_mutex);
        pthread_cond_destroy(&ctx->state_cond);
        pthread_mutex_destroy(&ctx->state_mutex);
        free(ctx);
        return -1;
    }

    ret = nethub_vchan_open_transport(ctx);
    if (ret != 0) {
        pthread_mutex_destroy(&ctx->callback_mutex);
        pthread_mutex_destroy(&ctx->tx_mutex);
        pthread_cond_destroy(&ctx->state_cond);
        pthread_mutex_destroy(&ctx->state_mutex);
        free(ctx);
        return -1;
    }

    /* Auto-start receive thread */
    ret = nethub_vchan_start_recv_thread(ctx);
    if (ret != 0) {
        pthread_mutex_destroy(&ctx->callback_mutex);
        pthread_mutex_destroy(&ctx->tx_mutex);
        pthread_cond_destroy(&ctx->state_cond);
        pthread_mutex_destroy(&ctx->state_mutex);
        if (ctx->sock_fd >= 0) {
            close(ctx->sock_fd);
        }
        free(ctx);
        return -1;
    }

    /* Set global handle only after everything succeeds */
    g_nethub_vchan_ctx = ctx;

    /* Query current kernel-side link state so late-joining userspace can build
     * an accurate initial view instead of assuming "connected". */
    if (ctx->transport == NETHUB_VCHAN_TRANSPORT_NETLINK) {
        (void)nethub_vchan_wait_initial_link(ctx, 300);
        if (nethub_vchan_auto_transport_enabled() && !nethub_vchan_current_link_is_up(ctx)) {
            g_nethub_vchan_ctx = NULL;
            nethub_vchan_stop_recv_thread(ctx);
            ctx->link_state = NETHUB_VCHAN_LINK_DOWN;
            ctx->host_state = NETHUB_VCHAN_HOST_STATE_UNKNOWN;
            ctx->state_generation = 0;

            if (nethub_vchan_open_usb_auto(ctx, 1) != 0 ||
                nethub_vchan_start_recv_thread(ctx) != 0) {
                pthread_mutex_destroy(&ctx->callback_mutex);
                pthread_mutex_destroy(&ctx->tx_mutex);
                pthread_cond_destroy(&ctx->state_cond);
                pthread_mutex_destroy(&ctx->state_mutex);
                if (ctx->sock_fd >= 0) {
                    close(ctx->sock_fd);
                }
                free(ctx);
                return -1;
            }
            g_nethub_vchan_ctx = ctx;
            nethub_vchan_update_state(ctx, NETHUB_VCHAN_LINK_UP, NETHUB_VCHAN_HOST_STATE_DEVICE_RUN);
        }
    } else if (ctx->transport == NETHUB_VCHAN_TRANSPORT_USB_ACM) {
        nethub_vchan_update_state(ctx, NETHUB_VCHAN_LINK_UP, NETHUB_VCHAN_HOST_STATE_DEVICE_RUN);
    }

    return 0;
}

/**
 * @brief Deinitialize BFLB Virtual Channel (singleton)
 * @retval 0 Success
 * @retval <0 Error code
 */
int nethub_vchan_deinit(void)
{
    struct nethub_vchan_ctx *ctx = g_nethub_vchan_ctx;

    if (!ctx) {
        return -1;
    }

    nethub_vchan_stop_recv_thread(ctx);

    pthread_mutex_destroy(&ctx->callback_mutex);
    pthread_mutex_destroy(&ctx->tx_mutex);
    pthread_cond_destroy(&ctx->state_cond);
    pthread_mutex_destroy(&ctx->state_mutex);
    free(ctx);
    g_nethub_vchan_ctx = NULL; /* Clear global handle */
    return 0;
}

static int nethub_vchan_open_netlink(struct nethub_vchan_ctx *ctx, int quiet)
{
    struct sockaddr_nl addr;
    int fd;

    fd = socket(AF_NETLINK, SOCK_RAW, NETHUB_VCHAN_PROTO);
    if (fd < 0) {
        if (!quiet) {
            perror("socket");
        }
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = NETHUB_VCHAN_GROUP; /* Join multicast group to receive broadcasts */
    addr.nl_pid = getpid();              /* Use process PID as port ID */

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        if (!quiet) {
            perror("bind");
        }
        close(fd);
        return -1;
    }

    ctx->sock_fd = fd;
    ctx->transport = NETHUB_VCHAN_TRANSPORT_NETLINK;
    return 0;
}

static int nethub_vchan_open_usb_acm(struct nethub_vchan_ctx *ctx, const char *dev_path, int quiet)
{
    struct termios tio;
    int fd;

    if (dev_path == NULL || dev_path[0] == '\0') {
        dev_path = NETHUB_VCHAN_USB_DEFAULT_DEV;
    }

    fd = open(dev_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        if (!quiet) {
            perror("open usb acm");
        }
        return -1;
    }

    if (tcgetattr(fd, &tio) != 0) {
        if (!quiet) {
            perror("tcgetattr");
        }
        close(fd);
        return -1;
    }

    cfmakeraw(&tio);
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSANOW, &tio) != 0) {
        if (!quiet) {
            perror("tcsetattr");
        }
        close(fd);
        return -1;
    }
    tcflush(fd, TCIOFLUSH);

    ctx->sock_fd = fd;
    ctx->transport = NETHUB_VCHAN_TRANSPORT_USB_ACM;
    ctx->usb_rx_len = 0;
    return 0;
}

static int nethub_vchan_open_usb_auto(struct nethub_vchan_ctx *ctx, int quiet)
{
    const char *dev_path = getenv("NETHUB_VCHAN_DEV");
    char path[32];
    int opened[NETHUB_VCHAN_USB_SCAN_MAX] = {0};
    int best_index = -1;
    int best_score = 0;
    int i;

    if (dev_path != NULL && dev_path[0] != '\0') {
        return nethub_vchan_open_usb_acm(ctx, dev_path, quiet);
    }

    for (i = 0; i < NETHUB_VCHAN_USB_SCAN_MAX; i++) {
        int score;

        snprintf(path, sizeof(path), "/dev/ttyACM%d", i);
        if (access(path, F_OK) != 0) {
            continue;
        }

        score = nethub_vchan_usb_candidate_score(i);
        if (score > best_score) {
            best_score = score;
            best_index = i;
        }
    }

    if (best_index >= 0) {
        snprintf(path, sizeof(path), "/dev/ttyACM%d", best_index);
        opened[best_index] = 1;
        if (nethub_vchan_open_usb_acm(ctx, path, quiet) == 0) {
            return 0;
        }
    }

    for (i = 0; i < NETHUB_VCHAN_USB_SCAN_MAX; i++) {
        if (opened[i]) {
            continue;
        }
        snprintf(path, sizeof(path), "/dev/ttyACM%d", i);
        if (access(path, F_OK) != 0) {
            continue;
        }
        if (nethub_vchan_open_usb_acm(ctx, path, quiet) == 0) {
            return 0;
        }
    }

    return -1;
}

static int nethub_vchan_open_transport(struct nethub_vchan_ctx *ctx)
{
    const char *transport = getenv("NETHUB_VCHAN_TRANSPORT");
    const char *dev_path = getenv("NETHUB_VCHAN_DEV");

    if (transport != NULL && transport[0] != '\0') {
        if (strcmp(transport, "usb") == 0) {
            if (dev_path != NULL && dev_path[0] != '\0') {
                return nethub_vchan_open_usb_acm(ctx, dev_path, 0);
            }
            return nethub_vchan_open_usb_auto(ctx, 0);
        }

        if (strcmp(transport, "netlink") == 0) {
            return nethub_vchan_open_netlink(ctx, 0);
        }

        fprintf(stderr, "Unsupported NETHUB_VCHAN_TRANSPORT=%s (supported: netlink, usb)\n", transport);
        return -1;
    }

    if (nethub_vchan_open_netlink(ctx, 1) == 0) {
        return 0;
    }

    return nethub_vchan_open_usb_auto(ctx, 0);
}

/**
 * @brief Send data to device with type
 */
static int nethub_vchan_send_raw(struct nethub_vchan_ctx *ctx, uint8_t data_type, const void *data, size_t len)
{
    struct sockaddr_nl dest_addr;
    struct nlmsghdr *nlh;
    uint8_t usb_frame[NETHUB_VCHAN_USB_FRAME_MAX];
    char buffer[NLMSG_SPACE(NETHUB_VCHAN_NETLINK_HDR_LEN + NETHUB_VCHAN_MAX_DATA_LEN)];
    int ret;
    struct nethub_vchan_data_hdr *data_hdr;

    if (!ctx) {
        fprintf(stderr, "NETHUB_VCHAN not initialized\n");
        return -1;
    }

    if (!data || len == 0 || len > NETHUB_VCHAN_MAX_DATA_LEN ||
        !nethub_vchan_data_type_is_valid(data_type)) {
        fprintf(stderr, "Invalid send parameters\n");
        return -1;
    }

    if (ctx->transport == NETHUB_VCHAN_TRANSPORT_USB_ACM) {
        int attempt;

        nethub_vchan_usb_frame_build(usb_frame, data, len);
        for (attempt = 0; attempt < NETHUB_VCHAN_USB_SEND_RETRIES; attempt++) {
            int fd;
            int reconnected = 0;
            int fd_to_close = -1;

            pthread_mutex_lock(&ctx->tx_mutex);
            if (ctx->sock_fd < 0) {
                ret = nethub_vchan_usb_try_reconnect_locked(ctx);
                if (ret < 0) {
                    pthread_mutex_unlock(&ctx->tx_mutex);
                    if (attempt + 1 < NETHUB_VCHAN_USB_SEND_RETRIES) {
                        nethub_vchan_sleep_ms(NETHUB_VCHAN_USB_SEND_RETRY_DELAY_MS);
                        continue;
                    }
                    return -1;
                }
                reconnected = ret > 0;
            }
            fd = ctx->sock_fd;
            if (fd < 0) {
                pthread_mutex_unlock(&ctx->tx_mutex);
                return -1;
            }
            ret = nethub_vchan_write_all(fd,
                                         usb_frame,
                                         NETHUB_VCHAN_USB_HDR_LEN + len + NETHUB_VCHAN_USB_CHECKSUM_LEN);
            if (ret < 0 && ctx->sock_fd == fd) {
                fd_to_close = ctx->sock_fd;
                ctx->sock_fd = -1;
                ctx->usb_rx_len = 0;
            }
            pthread_mutex_unlock(&ctx->tx_mutex);

            if (fd_to_close >= 0) {
                close(fd_to_close);
                nethub_vchan_update_state(ctx, NETHUB_VCHAN_LINK_DOWN, NETHUB_VCHAN_HOST_STATE_UNKNOWN);
            } else if (reconnected) {
                nethub_vchan_update_state(ctx, NETHUB_VCHAN_LINK_UP, NETHUB_VCHAN_HOST_STATE_DEVICE_RUN);
            }

            if (ret == 0) {
                return (int)len;
            }

            if (attempt + 1 < NETHUB_VCHAN_USB_SEND_RETRIES) {
                nethub_vchan_sleep_ms(NETHUB_VCHAN_USB_SEND_RETRY_DELAY_MS);
            }
        }

        return -1;
    }

    /* Build Netlink message */
    memset(buffer, 0, sizeof(buffer));
    nlh = (struct nlmsghdr *)buffer;
    nlh->nlmsg_len = NLMSG_LENGTH(NETHUB_VCHAN_NETLINK_HDR_LEN + len);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    /* Build data header */
    data_hdr = (struct nethub_vchan_data_hdr *)NLMSG_DATA(nlh);
    data_hdr->data_type = data_type;
    data_hdr->reserved = 0;
    data_hdr->len = (uint16_t)len; /* Store actual data length */

    /* Copy user data */
    memcpy(data_hdr->data, data, len);

    /* Send to kernel */
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* kernel */
    dest_addr.nl_groups = 0;

    pthread_mutex_lock(&ctx->tx_mutex);
    if (ctx->sock_fd < 0) {
        pthread_mutex_unlock(&ctx->tx_mutex);
        return -1;
    }
    ret = sendto(ctx->sock_fd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    pthread_mutex_unlock(&ctx->tx_mutex);
    if (ret < 0) {
        perror("sendto");
        return -1;
    }

    return len;
}

int nethub_vchan_send(uint8_t data_type, const void *data, size_t len)
{
    return nethub_vchan_send_raw(g_nethub_vchan_ctx, data_type, data, len);
}

/**
 * @brief Register receive callback function for specific data type
 */
int nethub_vchan_register_callback(uint8_t data_type, nethub_vchan_recv_callback_t callback)
{
    struct nethub_vchan_ctx *ctx = g_nethub_vchan_ctx;

    if (!ctx) {
        fprintf(stderr, "NETHUB_VCHAN not initialized\n");
        return -1;
    }

    if (!nethub_vchan_data_type_is_valid(data_type)) {
        fprintf(stderr, "Invalid data_type: %d (max: %d)\n", data_type, NETHUB_VCHAN_DATA_TYPE_MAX);
        return NETHUB_VCHAN_ERROR_PARAM;
    }

    pthread_mutex_lock(&ctx->callback_mutex);
    ctx->callbacks[data_type] = callback;
    pthread_mutex_unlock(&ctx->callback_mutex);
    return 0;
}

/**
 * @brief Unregister receive callback function for specific data type
 * @param data_type Data type (NETHUB_VCHAN_DATA_TYPE_*)
 * @return 0 Success, negative on failure
 */
int nethub_vchan_unregister_callback(uint8_t data_type)
{
    struct nethub_vchan_ctx *ctx = g_nethub_vchan_ctx;

    if (!ctx) {
        fprintf(stderr, "NETHUB_VCHAN not initialized\n");
        return NETHUB_VCHAN_ERROR_NOT_INIT;
    }

    if (!nethub_vchan_data_type_is_valid(data_type)) {
        fprintf(stderr, "Invalid data_type: %d (max: %d)\n", data_type, NETHUB_VCHAN_DATA_TYPE_MAX);
        return NETHUB_VCHAN_ERROR_PARAM;
    }

    pthread_mutex_lock(&ctx->callback_mutex);
    ctx->callbacks[data_type] = NULL;
    pthread_mutex_unlock(&ctx->callback_mutex);
    return NETHUB_VCHAN_OK;
}

int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot)
{
    struct nethub_vchan_ctx *ctx = g_nethub_vchan_ctx;

    if (!ctx) {
        return NETHUB_VCHAN_ERROR_NOT_INIT;
    }

    if (!snapshot) {
        return NETHUB_VCHAN_ERROR_PARAM;
    }

    pthread_mutex_lock(&ctx->state_mutex);
    snapshot->link_state = ctx->link_state;
    snapshot->host_state = ctx->host_state;
    pthread_mutex_unlock(&ctx->state_mutex);

    return NETHUB_VCHAN_OK;
}

const char *nethub_vchan_link_state_name(nethub_vchan_link_state_t link_state)
{
    switch (link_state) {
        case NETHUB_VCHAN_LINK_UP:
            return "UP";
        case NETHUB_VCHAN_LINK_DOWN:
            return "DOWN";
        default:
            return "UNKNOWN";
    }
}

const char *nethub_vchan_host_state_name(nethub_vchan_host_state_t host_state)
{
    switch (host_state) {
        case NETHUB_VCHAN_HOST_STATE_ERROR:
            return "ERROR";
        case NETHUB_VCHAN_HOST_STATE_RESET:
            return "RESET";
        case NETHUB_VCHAN_HOST_STATE_HOST_READY:
            return "HOST_READY";
        case NETHUB_VCHAN_HOST_STATE_DEVICE_RUN:
            return "DEVICE_RUN";
        case NETHUB_VCHAN_HOST_STATE_UNKNOWN:
            return "UNKNOWN";
        default:
            return "INVALID";
    }
}

int nethub_vchan_register_link_event_callback(nethub_vchan_link_event_callback_t callback, void *user_data)
{
    struct nethub_vchan_ctx *ctx = g_nethub_vchan_ctx;

    if (!ctx) {
        return NETHUB_VCHAN_ERROR_NOT_INIT;
    }

    pthread_mutex_lock(&ctx->state_mutex);
    ctx->link_event_callback = callback;
    ctx->link_event_user_data = user_data;
    pthread_mutex_unlock(&ctx->state_mutex);

    return NETHUB_VCHAN_OK;
}

static int nethub_vchan_send_link_query(struct nethub_vchan_ctx *ctx)
{
    struct nethub_vchan_ctrl_msg msg;

    if (!ctx) {
        return NETHUB_VCHAN_ERROR_NOT_INIT;
    }

    msg.magic = NETHUB_VCHAN_CTRL_MAGIC;
    msg.version = NETHUB_VCHAN_CTRL_VERSION;
    msg.msg_type = NETHUB_VCHAN_CTRL_MSG_QUERY_LINK_STATE;
    msg.link_state = NETHUB_VCHAN_LINK_DOWN;
    msg.host_state = 0u;

    return nethub_vchan_send_raw(ctx, NETHUB_VCHAN_DATA_TYPE_SYSTEM, &msg, sizeof(msg));
}

static nethub_vchan_host_state_t nethub_vchan_normalize_host_state(uint8_t host_state)
{
    switch (host_state) {
        case NETHUB_VCHAN_HOST_STATE_ERROR:
        case NETHUB_VCHAN_HOST_STATE_RESET:
        case NETHUB_VCHAN_HOST_STATE_HOST_READY:
        case NETHUB_VCHAN_HOST_STATE_DEVICE_RUN:
            return (nethub_vchan_host_state_t)host_state;
        default:
            return NETHUB_VCHAN_HOST_STATE_UNKNOWN;
    }
}

static void nethub_vchan_update_state(struct nethub_vchan_ctx *ctx,
                                      nethub_vchan_link_state_t link_state,
                                      nethub_vchan_host_state_t host_state)
{
    nethub_vchan_link_event_callback_t callback = NULL;
    void *user_data = NULL;
    int link_changed = 0;

    pthread_mutex_lock(&ctx->state_mutex);
    if (ctx->link_state != link_state) {
        ctx->link_state = link_state;
        callback = ctx->link_event_callback;
        user_data = ctx->link_event_user_data;
        link_changed = 1;
    }
    ctx->host_state = host_state;
    ctx->state_generation++;
    pthread_cond_broadcast(&ctx->state_cond);
    pthread_mutex_unlock(&ctx->state_mutex);

    if (link_changed && callback) {
        callback(link_state, user_data);
    }
}

static int nethub_vchan_try_handle_ctrl_msg(struct nethub_vchan_ctx *ctx,
                                            uint8_t data_type,
                                            const void *data,
                                            size_t len)
{
    const struct nethub_vchan_ctrl_msg *msg = (const struct nethub_vchan_ctrl_msg *)data;

    if (data == NULL || len < sizeof(*msg) || data_type != NETHUB_VCHAN_DATA_TYPE_SYSTEM) {
        return 0;
    }

    if (msg->magic != NETHUB_VCHAN_CTRL_MAGIC || msg->version != NETHUB_VCHAN_CTRL_VERSION) {
        return 0;
    }

    if (msg->msg_type != NETHUB_VCHAN_CTRL_MSG_LINK_STATE_EVENT) {
        return 0;
    }

    nethub_vchan_update_state(ctx,
                              msg->link_state == NETHUB_VCHAN_LINK_UP ?
                              NETHUB_VCHAN_LINK_UP :
                              NETHUB_VCHAN_LINK_DOWN,
                              nethub_vchan_normalize_host_state(msg->host_state));
    return 1;
}

/* ==================== Convenience APIs for USER ==================== */

/**
 * @brief Send USER data (convenience wrapper)
 * @param[in] data Data buffer
 * @param[in] len Data length
 * @retval >=0 Sent bytes
 * @retval <0 Error code
 */
int nethub_vchan_user_send(const void *data, size_t len)
{
    return nethub_vchan_send(NETHUB_VCHAN_DATA_TYPE_USER, data, len);
}

/**
 * @brief Register USER data receive callback (convenience wrapper)
 * @param[in] callback Receive callback function
 * @retval 0 Success
 * @retval <0 Error code
 */
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback)
{
    return nethub_vchan_register_callback(NETHUB_VCHAN_DATA_TYPE_USER, callback);
}

/* ==================== Convenience APIs for AT ==================== */

/**
 * @brief Send AT data (convenience wrapper)
 * @param data Data buffer
 * @param len Data length
 * @return Sent bytes for success, negative for failure
 */
int nethub_vchan_at_send(const void *data, size_t len)
{
    return nethub_vchan_send(NETHUB_VCHAN_DATA_TYPE_AT, data, len);
}

/**
 * @brief Register AT data receive callback (convenience wrapper)
 * @param callback Receive callback function
 * @return 0 Success, negative on failure
 */
int nethub_vchan_at_register_callback(nethub_vchan_recv_callback_t callback)
{
    return nethub_vchan_register_callback(NETHUB_VCHAN_DATA_TYPE_AT, callback);
}

/* ==================== Internal Functions ==================== */

/**
 * @brief Receive thread with graceful shutdown support
 * @param[in] arg Virtual channel context (struct nethub_vchan_ctx *)
 * @note Internal function called by pthread_create
 * @note Non-blocking implementation with select() timeout for graceful shutdown
 */
static void *nethub_vchan_recv_thread_func(void *arg)
{
    struct nethub_vchan_ctx *ctx = (struct nethub_vchan_ctx *)arg;

    if (!ctx) {
        fprintf(stderr, "Error: Invalid handle in recv thread\n");
        return NULL;
    }

    for (;;) {
        /* Use select() with timeout to allow checking socket validity */
        fd_set readfds;
        struct timeval tv;
        int fd;
        enum nethub_vchan_transport transport;
        int ret;
        int select_ret;
        int same_fd;
        int stopping;
        int reconnected = 0;

        pthread_mutex_lock(&ctx->tx_mutex);
        stopping = ctx->stopping;
        transport = ctx->transport;
        fd = ctx->sock_fd; /* Get fresh fd in each iteration */
        if (!stopping && transport == NETHUB_VCHAN_TRANSPORT_USB_ACM && fd < 0) {
            ret = nethub_vchan_usb_try_reconnect_locked(ctx);
            if (ret >= 0) {
                fd = ctx->sock_fd;
                reconnected = ret > 0;
            }
        }
        pthread_mutex_unlock(&ctx->tx_mutex);

        if (stopping) {
            break;
        }

        if (reconnected) {
            nethub_vchan_update_state(ctx, NETHUB_VCHAN_LINK_UP, NETHUB_VCHAN_HOST_STATE_DEVICE_RUN);
        }

        if (fd < 0) {
            if (transport == NETHUB_VCHAN_TRANSPORT_USB_ACM) {
                nethub_vchan_sleep_ms(NETHUB_VCHAN_USB_RECONNECT_INTERVAL_MS);
                continue;
            }
            break; /* Socket was closed */
        }

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        /* 100ms timeout to periodically check ctx->sock_fd */
        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        select_ret = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (select_ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (transport == NETHUB_VCHAN_TRANSPORT_USB_ACM) {
                nethub_vchan_usb_mark_down(ctx, fd);
                nethub_vchan_sleep_ms(NETHUB_VCHAN_USB_RECONNECT_INTERVAL_MS);
                continue;
            }
            break;
        }

        /* Check if socket was closed (via nethub_vchan_deinit) */
        pthread_mutex_lock(&ctx->tx_mutex);
        stopping = ctx->stopping;
        same_fd = (ctx->sock_fd == fd);
        pthread_mutex_unlock(&ctx->tx_mutex);
        if (stopping) {
            break;
        }
        if (!same_fd) {
            continue;
        }

        if (select_ret > 0 && FD_ISSET(fd, &readfds)) {
            uint8_t buffer[2048];
            struct nlmsghdr *nlh;
            struct nethub_vchan_data_hdr *data_hdr;
            struct sockaddr_nl src_addr;
            socklen_t addr_len = sizeof(src_addr);
            ssize_t len;

            if (ctx->transport == NETHUB_VCHAN_TRANSPORT_USB_ACM) {
                len = read(fd, buffer, sizeof(buffer));
                if (len < 0) {
                    if (errno == EINTR) {
                        continue;
                    }
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue;
                    }
                    nethub_vchan_usb_mark_down(ctx, fd);
                    nethub_vchan_sleep_ms(NETHUB_VCHAN_USB_RECONNECT_INTERVAL_MS);
                    continue;
                }
                if (len == 0) {
                    nethub_vchan_usb_mark_down(ctx, fd);
                    nethub_vchan_sleep_ms(NETHUB_VCHAN_USB_RECONNECT_INTERVAL_MS);
                    continue;
                }

                nethub_vchan_usb_stream_feed(ctx, buffer, (size_t)len);
                continue;
            }

            len = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&src_addr, &addr_len);
            if (len < 0) {
                if (errno == EINTR) {
                    continue;
                }
                break;
            }

            if (len == 0) {
                /* Connection closed */
                break;
            }

            /* Verify message is from kernel (nl_pid == 0) */
            if (src_addr.nl_family != AF_NETLINK || src_addr.nl_pid != 0) {
                fprintf(stderr, "Warning: Received netlink message from non-kernel source (pid=%u)\n", src_addr.nl_pid);
                continue; /* Drop message from non-kernel source */
            }

            if ((size_t)len >= NLMSG_HDRLEN) {
                nlh = (struct nlmsghdr *)buffer;

                /* Validate nlmsg_len to prevent integer underflow (32/64-bit safe) */
                if (nlh->nlmsg_len < NLMSG_HDRLEN || nlh->nlmsg_len > (size_t)len) {
                    fprintf(stderr, "Error: Invalid netlink message length (len=%u, hdr=%u)\n", (unsigned int)len,
                            nlh->nlmsg_len);
                    continue;
                }

                void *nethub_vchan_data = NLMSG_DATA(nlh);
                size_t nethub_vchan_data_len = nlh->nlmsg_len - NLMSG_HDRLEN;

                /* Try to parse with data_type header first (userspace-to-userspace format) */
                if (nethub_vchan_data_len >= NETHUB_VCHAN_NETLINK_HDR_LEN) {
                    data_hdr = (struct nethub_vchan_data_hdr *)nethub_vchan_data;

                    /* Check if data_type field looks valid (must be < MAX and len must match) */
                    if (nethub_vchan_data_type_is_valid(data_hdr->data_type) &&
                        data_hdr->len <= NETHUB_VCHAN_MAX_DATA_LEN &&
                        (size_t)data_hdr->len + NETHUB_VCHAN_NETLINK_HDR_LEN <= nethub_vchan_data_len) {
                        /* Valid header format, extract data */
                        size_t user_data_len = data_hdr->len;
                        void *user_data = data_hdr->data;

                        nethub_vchan_dispatch_data(ctx, data_hdr->data_type, user_data, user_data_len);
                        continue;
                    }
                }

                /* Fallback: treat as raw data (kernel-to-userspace format without header) */
                /* Kernel sends raw data, default to AT type for AT commands */
                if (nethub_vchan_data_len > 0 && nethub_vchan_data_len <= NETHUB_VCHAN_MAX_DATA_LEN) {
                    nethub_vchan_dispatch_data(ctx, NETHUB_VCHAN_DATA_TYPE_AT, nethub_vchan_data, nethub_vchan_data_len);
                } else if (nethub_vchan_data_len > 0) {
                    fprintf(stderr, "Error: Raw data too large (len=%zu, max=%d)\n", nethub_vchan_data_len,
                            NETHUB_VCHAN_MAX_DATA_LEN);
                }
            }
        }
    }

    return NULL;
}
