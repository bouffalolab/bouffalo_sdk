/**
 * @file netlink_ctrl.c
 * @brief Netlink Userspace Control Library
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include "netlink_ctrl.h"
#include "netlink_protocol.h"

/* Netlink context */
struct netlink_ctx {
    int sock_fd;
    netlink_recv_callback_t callbacks[NETLINK_DATA_TYPE_MAX];  /* Per-type callbacks */
};

/**
 * @brief Initialize Netlink connection
 */
int netlink_init(netlink_handle_t *handle)
{
    struct netlink_ctx *ctx;
    struct sockaddr_nl addr;

    if (!handle) {
        fprintf(stderr, "Invalid parameter\n");
        return -1;
    }

    /* Allocate context */
    ctx = (struct netlink_ctx *)calloc(1, sizeof(struct netlink_ctx));
    if (!ctx) {
        perror("calloc");
        return -1;
    }

    /* Create Netlink socket */
    ctx->sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_PROTO);
    if (ctx->sock_fd < 0) {
        perror("socket");
        free(ctx);
        return -1;
    }

    /* Bind socket */
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = NETLINK_GROUP;  /* Join multicast group to receive broadcasts */
    addr.nl_pid = getpid();  /* Use process PID as port ID */

    if (bind(ctx->sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(ctx->sock_fd);
        free(ctx);
        return -1;
    }

    /* Callbacks are already NULL due to calloc */
    *handle = ctx;
    return 0;
}

/**
 * @brief Deinitialize Netlink connection
 */
int netlink_deinit(netlink_handle_t handle)
{
    struct netlink_ctx *ctx = (struct netlink_ctx *)handle;

    if (!ctx) {
        return -1;
    }

    if (ctx->sock_fd >= 0) {
        close(ctx->sock_fd);
    }

    free(ctx);
    return 0;
}

/**
 * @brief Send data to device with type
 */
int netlink_send(netlink_handle_t handle, uint8_t data_type, const void *data, size_t len)
{
    struct netlink_ctx *ctx = (struct netlink_ctx *)handle;
    struct sockaddr_nl dest_addr;
    struct nlmsghdr *nlh;
    char buffer[MAX_MSGLEN];
    int ret;
    struct netlink_data_hdr *data_hdr;

    if (!ctx || !data || len == 0 || len > MAX_MSGLEN - NETLINK_DATA_HDR_LEN) {
        fprintf(stderr, "Invalid send parameters\n");
        return -1;
    }

    /* Build Netlink message */
    memset(buffer, 0, sizeof(buffer));
    nlh = (struct nlmsghdr *)buffer;
    nlh->nlmsg_len = NLMSG_LENGTH(NETLINK_DATA_HDR_LEN + len);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    /* Build data header */
    data_hdr = (struct netlink_data_hdr *)NLMSG_DATA(nlh);
    data_hdr->data_type = data_type;
    memset(data_hdr->reserved, 0, sizeof(data_hdr->reserved));

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
int netlink_register_callback(netlink_handle_t handle, uint8_t data_type, netlink_recv_callback_t callback)
{
    struct netlink_ctx *ctx = (struct netlink_ctx *)handle;

    if (!ctx) {
        return -1;
    }

    if (data_type >= NETLINK_DATA_TYPE_MAX) {
        fprintf(stderr, "Invalid data_type: %d (max: %d)\n", data_type, NETLINK_DATA_TYPE_MAX);
        return -2;
    }

    ctx->callbacks[data_type] = callback;
    return 0;
}

/**
 * @brief Netlink receive thread (called by application pthread)
 * @param[in] handle Netlink handle
 * @note This function should be called in a separate thread
 */
void netlink_recv_thread(netlink_handle_t handle)
{
    struct netlink_ctx *ctx = (struct netlink_ctx *)handle;
    struct nlmsghdr *nlh;
    struct netlink_data_hdr *data_hdr;
    char buffer[MAX_MSGLEN];
    int len;

    if (!ctx) {
        return;
    }

    while (1) {
        /* Receive message */
        len = recv(ctx->sock_fd, buffer, sizeof(buffer), 0);
        if (len < 0) {
            if (errno == EINTR) {
                continue;  /* Interrupted, continue */
            }
            perror("recv");
            break;
        }

        if (len < NLMSG_HDRLEN) {
            fprintf(stderr, "Invalid message length\n");
            continue;
        }

        nlh = (struct nlmsghdr *)buffer;

        /* Extract Netlink payload */
        void *netlink_data = NLMSG_DATA(nlh);
        size_t netlink_data_len = nlh->nlmsg_len - NLMSG_HDRLEN;

        /* Parse data header */
        if (netlink_data_len < NETLINK_DATA_HDR_LEN) {
            fprintf(stderr, "Data too short for header\n");
            continue;
        }

        data_hdr = (struct netlink_data_hdr *)netlink_data;
        void *user_data = data_hdr->data;
        size_t user_data_len = netlink_data_len - NETLINK_DATA_HDR_LEN;

        /* Call callback function (invoke corresponding callback based on data_type) */
        if (data_hdr->data_type < NETLINK_DATA_TYPE_MAX) {
            if (ctx->callbacks[data_hdr->data_type] && user_data_len > 0) {
                ctx->callbacks[data_hdr->data_type](user_data, user_data_len);
            }
        }
    }
}

/**
 * @brief Get socket file descriptor (for select/poll)
 */
int netlink_get_fd(netlink_handle_t handle)
{
    struct netlink_ctx *ctx = (struct netlink_ctx *)handle;

    if (!ctx) {
        return -1;
    }

    return ctx->sock_fd;
}
