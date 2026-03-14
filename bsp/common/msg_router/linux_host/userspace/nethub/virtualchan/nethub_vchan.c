/**
 * @file nethub_vchan.c
 * @brief BFLB Virtual Channel Netlink Userspace Library
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <linux/netlink.h>

#include "nethub_vchan.h"

/* ==================== Internal Definitions ==================== */

/* Netlink protocol constants */
#define NETHUB_VCHAN_PROTO 31
#define NETHUB_VCHAN_GROUP 1

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
    pthread_t recv_thread;                                              /* Receive thread handle (0 if not created) */
    nethub_vchan_recv_callback_t callbacks[NETHUB_VCHAN_DATA_TYPE_MAX]; /* Per-type callbacks */
    pthread_mutex_t state_mutex;
    pthread_cond_t state_cond;
    uint32_t state_generation;
    nethub_vchan_link_state_t link_state;
    nethub_vchan_host_state_t host_state;
    nethub_vchan_link_event_callback_t link_event_callback;
    void *link_event_user_data;
};

/* Global singleton handle */
static struct nethub_vchan_ctx *g_nethub_vchan_ctx = NULL;

/* Forward declaration */
static void *nethub_vchan_recv_thread_func(void *arg);
static int nethub_vchan_send_raw(struct nethub_vchan_ctx *ctx, uint8_t data_type, const void *data, size_t len);
static int nethub_vchan_send_link_query(struct nethub_vchan_ctx *ctx);
static nethub_vchan_host_state_t nethub_vchan_normalize_host_state(uint8_t host_state);
static void nethub_vchan_update_state(struct nethub_vchan_ctx *ctx,
                                      nethub_vchan_link_state_t link_state,
                                      nethub_vchan_host_state_t host_state);
static int nethub_vchan_try_handle_ctrl_msg(struct nethub_vchan_ctx *ctx,
                                            uint8_t data_type,
                                            const void *data,
                                            size_t len);

/**
 * @brief Initialize BFLB Virtual Channel (singleton)
 * @retval 0 Success
 * @retval <0 Error code
 */
int nethub_vchan_init(void)
{
    struct nethub_vchan_ctx *ctx;
    struct sockaddr_nl addr;
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
    ctx->recv_thread = 0; /* Initialize thread handle to 0 (not created) */
    ctx->link_state = NETHUB_VCHAN_LINK_DOWN;
    ctx->host_state = NETHUB_VCHAN_HOST_STATE_UNKNOWN;

    /* Create Netlink socket */
    ctx->sock_fd = socket(AF_NETLINK, SOCK_RAW, NETHUB_VCHAN_PROTO);
    if (ctx->sock_fd < 0) {
        perror("socket");
        free(ctx);
        return -1;
    }

    /* Bind socket */
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = NETHUB_VCHAN_GROUP; /* Join multicast group to receive broadcasts */
    addr.nl_pid = getpid();              /* Use process PID as port ID */

    if (bind(ctx->sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(ctx->sock_fd);
        free(ctx);
        return -1;
    }

    /* Callbacks are already NULL due to calloc */

    if (pthread_mutex_init(&ctx->state_mutex, NULL) != 0) {
        fprintf(stderr, "Failed to create state mutex\n");
        close(ctx->sock_fd);
        free(ctx);
        return -1;
    }

    if (pthread_cond_init(&ctx->state_cond, NULL) != 0) {
        fprintf(stderr, "Failed to create state condition variable\n");
        pthread_mutex_destroy(&ctx->state_mutex);
        close(ctx->sock_fd);
        free(ctx);
        return -1;
    }

    /* Auto-start receive thread */
    ret = pthread_create(&ctx->recv_thread, NULL, nethub_vchan_recv_thread_func, ctx);
    if (ret != 0) {
        fprintf(stderr, "Failed to create recv thread: %d\n", ret);
        pthread_cond_destroy(&ctx->state_cond);
        pthread_mutex_destroy(&ctx->state_mutex);
        close(ctx->sock_fd);
        free(ctx);
        return -1;
    }

    /* Set global handle only after everything succeeds */
    g_nethub_vchan_ctx = ctx;

    /* Query current kernel-side link state so late-joining userspace can build
     * an accurate initial view instead of assuming "connected". */
    if (nethub_vchan_send_link_query(ctx) >= 0) {
        struct timespec ts;
        uint32_t generation;

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += 300 * 1000000;
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

    /* Stop receive thread if running */
    if (ctx->recv_thread != 0) {
        /* Close socket to trigger thread exit */
        if (ctx->sock_fd >= 0) {
            close(ctx->sock_fd);
            ctx->sock_fd = -1;
        }

        /* Wait for thread to exit */
        pthread_join(ctx->recv_thread, NULL);
        ctx->recv_thread = 0;
    } else {
        /* No thread running, just close socket */
        if (ctx->sock_fd >= 0) {
            close(ctx->sock_fd);
        }
    }

    pthread_cond_destroy(&ctx->state_cond);
    pthread_mutex_destroy(&ctx->state_mutex);
    free(ctx);
    g_nethub_vchan_ctx = NULL; /* Clear global handle */
    return 0;
}

/**
 * @brief Send data to device with type
 */
static int nethub_vchan_send_raw(struct nethub_vchan_ctx *ctx, uint8_t data_type, const void *data, size_t len)
{
    struct sockaddr_nl dest_addr;
    struct nlmsghdr *nlh;
    char buffer[NETHUB_VCHAN_DATA_HDR_LEN + NETHUB_VCHAN_MAX_DATA_LEN];
    int ret;
    struct nethub_vchan_data_hdr *data_hdr;

    if (!ctx) {
        fprintf(stderr, "Netlink not initialized\n");
        return -1;
    }

    if (!data || len == 0 || len > NETHUB_VCHAN_MAX_DATA_LEN) {
        fprintf(stderr, "Invalid send parameters\n");
        return -1;
    }

    /* Build Netlink message */
    memset(buffer, 0, sizeof(buffer));
    nlh = (struct nlmsghdr *)buffer;
    nlh->nlmsg_len = NLMSG_LENGTH(NETHUB_VCHAN_DATA_HDR_LEN + len);
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

    ret = sendto(ctx->sock_fd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
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
        fprintf(stderr, "Netlink not initialized\n");
        return -1;
    }

    if (data_type >= NETHUB_VCHAN_DATA_TYPE_MAX) {
        fprintf(stderr, "Invalid data_type: %d (max: %d)\n", data_type, NETHUB_VCHAN_DATA_TYPE_MAX);
        return -2;
    }

    ctx->callbacks[data_type] = callback;
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

    if (data_type >= NETHUB_VCHAN_DATA_TYPE_MAX) {
        fprintf(stderr, "Invalid data_type: %d (max: %d)\n", data_type, NETHUB_VCHAN_DATA_TYPE_MAX);
        return NETHUB_VCHAN_ERROR_PARAM;
    }

    ctx->callbacks[data_type] = NULL;
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
 * @return 0 Success, negative on failure
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
 * @brief Netlink receive thread with graceful shutdown support
 * @param[in] arg Netlink context (struct nethub_vchan_ctx *)
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

    while (ctx->sock_fd >= 0) {
        /* Use select() with timeout to allow checking socket validity */
        fd_set readfds;
        struct timeval tv;
        int fd;
        int ret;

        fd = ctx->sock_fd; /* Get fresh fd in each iteration */
        if (fd < 0) {
            break; /* Socket was closed */
        }

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        /* 100ms timeout to periodically check ctx->sock_fd */
        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        ret = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        /* Check if socket was closed (via nethub_vchan_deinit) */
        if (ctx->sock_fd < 0) {
            break;
        }

        if (ret > 0 && FD_ISSET(fd, &readfds)) {
            char buffer[2048];
            struct nlmsghdr *nlh;
            struct nethub_vchan_data_hdr *data_hdr;
            struct sockaddr_nl src_addr;
            socklen_t addr_len = sizeof(src_addr);
            ssize_t len;

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
                if (nethub_vchan_data_len >= NETHUB_VCHAN_DATA_HDR_LEN) {
                    data_hdr = (struct nethub_vchan_data_hdr *)nethub_vchan_data;

                    /* Check if data_type field looks valid (must be < MAX and len must match) */
                    if (data_hdr->data_type < NETHUB_VCHAN_DATA_TYPE_MAX &&
                        data_hdr->len <= NETHUB_VCHAN_MAX_DATA_LEN &&
                        (size_t)data_hdr->len + NETHUB_VCHAN_DATA_HDR_LEN <= nethub_vchan_data_len) {
                        /* Valid header format, extract data */
                        size_t user_data_len = data_hdr->len;
                        void *user_data = data_hdr->data;

                        if (nethub_vchan_try_handle_ctrl_msg(ctx, data_hdr->data_type, user_data, user_data_len)) {
                            continue;
                        }

                        /* Invoke corresponding callback based on data_type */
                        if (ctx->callbacks[data_hdr->data_type]) {
                            ctx->callbacks[data_hdr->data_type](user_data, user_data_len);
                        }
                        continue;
                    }
                }

                /* Fallback: treat as raw data (kernel-to-userspace format without header) */
                /* Kernel sends raw data, default to AT type for AT commands */
                if (nethub_vchan_data_len > 0 && nethub_vchan_data_len <= NETHUB_VCHAN_MAX_DATA_LEN) {
                    if (ctx->callbacks[NETHUB_VCHAN_DATA_TYPE_AT]) {
                        ctx->callbacks[NETHUB_VCHAN_DATA_TYPE_AT](nethub_vchan_data, nethub_vchan_data_len);
                    }
                } else if (nethub_vchan_data_len > 0) {
                    fprintf(stderr, "Error: Raw data too large (len=%zu, max=%d)\n", nethub_vchan_data_len,
                            NETHUB_VCHAN_MAX_DATA_LEN);
                }
            }
        }
    }

    return NULL;
}
