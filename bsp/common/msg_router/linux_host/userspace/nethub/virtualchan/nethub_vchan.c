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
#include <linux/netlink.h>

#include "nethub_vchan.h"

/* ==================== Internal Definitions ==================== */

/* Netlink protocol constants */
#define NETHUB_VCHAN_PROTO          31
#define NETHUB_VCHAN_GROUP          1

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
    uint8_t  data_type;     /* Data type (see NETHUB_VCHAN_DATA_TYPE_* defines) */
    uint8_t  reserved;      /* Reserved field (initialized to 0) */
    uint16_t len;           /* Actual user data length (exclude header) */
    uint8_t  data[];        /* Actual user data (flexible array) */
} __attribute__((packed));
#pragma pack(pop)

/* ==================== Context Structure ==================== */

/* BFLB Virtual Channel context */
struct nethub_vchan_ctx {
    int sock_fd;
    pthread_t recv_thread;         /* Receive thread handle (0 if not created) */
    nethub_vchan_recv_callback_t callbacks[NETHUB_VCHAN_DATA_TYPE_MAX];  /* Per-type callbacks */
};

/* Global singleton handle */
static struct nethub_vchan_ctx *g_nethub_vchan_ctx = NULL;

/* Forward declaration */
static void *nethub_vchan_recv_thread_func(void *arg);

/**
 * @brief Initialize BFLB Virtual Channel (singleton)
 * @retval 0 Success
 * @retval <0 Error code
 */
int nethub_vchan_init(void)
{
    struct nethub_vchan_ctx *ctx;
    struct sockaddr_nl addr;

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
    ctx->recv_thread = 0;  /* Initialize thread handle to 0 (not created) */

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
    addr.nl_groups = NETHUB_VCHAN_GROUP;  /* Join multicast group to receive broadcasts */
    addr.nl_pid = getpid();  /* Use process PID as port ID */

    if (bind(ctx->sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(ctx->sock_fd);
        free(ctx);
        return -1;
    }

    /* Callbacks are already NULL due to calloc */

    /* Auto-start receive thread */
    int ret = pthread_create(&ctx->recv_thread, NULL, nethub_vchan_recv_thread_func, ctx);
    if (ret != 0) {
        fprintf(stderr, "Failed to create recv thread: %d\n", ret);
        close(ctx->sock_fd);
        free(ctx);
        return -1;
    }

    /* Set global handle only after everything succeeds */
    g_nethub_vchan_ctx = ctx;
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

    free(ctx);
    g_nethub_vchan_ctx = NULL;  /* Clear global handle */
    return 0;
}

/**
 * @brief Send data to device with type
 */
int nethub_vchan_send(uint8_t data_type, const void *data, size_t len)
{
    struct nethub_vchan_ctx *ctx = g_nethub_vchan_ctx;
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
    data_hdr->len = (uint16_t)len;  /* Store actual data length */

    /* Copy user data */
    memcpy(data_hdr->data, data, len);

    /* Send to kernel */
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;  /* kernel */
    dest_addr.nl_groups = 0;

    ret = sendto(ctx->sock_fd, nlh, nlh->nlmsg_len, 0,
                 (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret < 0) {
        perror("sendto");
        return -1;
    }

    return len;
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

        fd = ctx->sock_fd;  /* Get fresh fd in each iteration */
        if (fd < 0) {
            break;  /* Socket was closed */
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

            len = recvfrom(fd, buffer, sizeof(buffer), 0,
                           (struct sockaddr *)&src_addr, &addr_len);
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
                fprintf(stderr, "Warning: Received netlink message from non-kernel source (pid=%u)\n",
                        src_addr.nl_pid);
                continue;  /* Drop message from non-kernel source */
            }

            if ((size_t)len >= NLMSG_HDRLEN) {
                nlh = (struct nlmsghdr *)buffer;

                /* Validate nlmsg_len to prevent integer underflow (32/64-bit safe) */
                if (nlh->nlmsg_len < NLMSG_HDRLEN || nlh->nlmsg_len > (size_t)len) {
                    fprintf(stderr, "Error: Invalid netlink message length (len=%u, hdr=%u)\n",
                            (unsigned int)len, nlh->nlmsg_len);
                    continue;
                }

                void *nethub_vchan_data = NLMSG_DATA(nlh);
                size_t nethub_vchan_data_len = nlh->nlmsg_len - NLMSG_HDRLEN;

                /* Parse data_type header */
                if (nethub_vchan_data_len >= NETHUB_VCHAN_DATA_HDR_LEN) {
                    data_hdr = (struct nethub_vchan_data_hdr *)nethub_vchan_data;

                    /* Use len field from header to get actual data length (solves alignment padding issue) */
                    size_t user_data_len = data_hdr->len;

                    /* Validate len field to prevent buffer overflow */
                    if (user_data_len > NETHUB_VCHAN_MAX_DATA_LEN) {
                        fprintf(stderr, "Error: Invalid data length in header (len=%zu, max=%d)\n",
                                user_data_len, NETHUB_VCHAN_MAX_DATA_LEN);
                        continue;
                    }

                    void *user_data = data_hdr->data;

                    /* Invoke corresponding callback based on data_type */
                    if (data_hdr->data_type < NETHUB_VCHAN_DATA_TYPE_MAX) {
                        if (ctx->callbacks[data_hdr->data_type] && user_data_len > 0) {
                            ctx->callbacks[data_hdr->data_type](user_data, user_data_len);
                        }
                    }
                }
            }
        }
    }

    return NULL;
}
