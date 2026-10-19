/*
 *  Copyright (C) Bouffalo Lab 2016-2023
 *  SPDX-License-Identifier: Apache-2.0
 */
#include "blctl.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include "../common.h"
#include "../rnm_msg.h"
#include "utils.h"
#include "blctl_log.h"

#define BLCTL_PROTO NETLINK_USERSOCK

#define NL_RCV_MAX_PAYLOAD 2048

#define WAIT_SLOTS 4

typedef struct {
    size_t idx;
    sem_t sem;
    uint16_t cmd_id;
    uint16_t seq;
    void *resp;
} wait_env_t;

typedef struct {
    bool daemon;
    int sock;
    struct sockaddr_nl kaddr;

    struct nlmsghdr *rcv_nlh;
    uint16_t sid;
    pthread_t read_thread_id;
    bool read_thread_should_exit;

    pthread_mutex_t wait_slots_lock;
    wait_env_t *wait_slots[WAIT_SLOTS];
    uint16_t tx_last_seq;

    blctl_wildcard_msg_handler_t msg_handler;
    void *msg_handler_arg;
} blctl_env_t;

const char *blctl_auth_to_str(uint8_t auth)
{
    const char *table[RNM_WIFI_AUTH_MAX] = {
        [RNM_WIFI_AUTH_UNKNOWN]           = "UNKNOWN",
        [RNM_WIFI_AUTH_OPEN]              = "OPEN",
        [RNM_WIFI_AUTH_WEP]               = "WEP",
        [RNM_WIFI_AUTH_WPA_PSK]           = "WPA-PSK",
        [RNM_WIFI_AUTH_WPA2_PSK]          = "WPA2-PSK",
        [RNM_WIFI_AUTH_WPA_WPA2_PSK]      = "WPA2-PSK/WPA-PSK",
        [RNM_WIFI_AUTH_WPA_ENTERPRISE]    = "WPA-ENT",
        [RNM_WIFI_AUTH_WPA3_SAE]          = "WPA3-SAE",
        [RNM_WIFI_AUTH_WPA2_PSK_WPA3_SAE] = "WPA2-PSK/WPA3-SAE",
    };
    if (auth < RNM_WIFI_AUTH_MAX)
        return table[auth];
    else
        return table[RNM_WIFI_AUTH_UNKNOWN];
}

const char *blctl_cipher_to_str(uint8_t cipher)
{
    const char *table[RNM_WIFI_CIPHER_MAX] = {
        [RNM_WIFI_CIPHER_UNKNOWN]  = "UNKNOWN",
        [RNM_WIFI_CIPHER_NONE]     = "NONE",
        [RNM_WIFI_CIPHER_WEP]      = "WEP",
        [RNM_WIFI_CIPHER_AES]      = "AES",
        [RNM_WIFI_CIPHER_TKIP]     = "TKIP",
        [RNM_WIFI_CIPHER_TKIP_AES] = "TKIP/AES",
    };
    if (cipher < RNM_WIFI_CIPHER_MAX)
        return table[cipher];
    else
        return table[RNM_WIFI_CIPHER_UNKNOWN];
}

static int nl_msg_out(blctl_env_t *env, const void *data, size_t len)
{
    int ret = 0;
    struct nlmsghdr *nlh = NULL;
    struct msghdr msg;
    struct iovec iov;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(len));
    if (!nlh) {
        bl_loge("no mem for netlink header\n");
        return ENOMEM;
    }

    nlh->nlmsg_len = NLMSG_SPACE(len);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    memcpy(NLMSG_DATA(nlh), data, len);

    memset(&iov, 0, sizeof(iov));
    iov.iov_base = nlh;
    iov.iov_len = nlh->nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = &env->kaddr;
    msg.msg_namelen = sizeof(env->kaddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (sendmsg(env->sock, &msg, 0) < 0) {
        bl_loge("sendmsg failed, %d, %s\n", errno, strerror(errno));
        ret = errno;
    }
    free(nlh);

    return ret;
}

static int nl_msg_recv(blctl_env_t *env, size_t timeout_ms)
{
    int ret = 0;
    struct nlmsghdr *nlh = env->rcv_nlh;
    struct msghdr msg;
    struct iovec iov;
    struct timeval tv;

    nlh->nlmsg_len = NLMSG_SPACE(NL_RCV_MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    memset(&iov, 0, sizeof(iov));
    iov.iov_base = nlh;
    iov.iov_len = nlh->nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = timeout_ms % 1000 * 1000;
    if (setsockopt(env->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv)) {
        bl_loge("setsockopt failed, %d, %s\n", errno, strerror(errno));
        ret = errno;
        goto exit;
    }
    ret = recvmsg(env->sock, &msg, 0);
    if (ret < 0) {
        if (errno != EAGAIN)
            bl_loge("recvmsg failed, %d, %s\n", errno, strerror(errno));

        ret = errno;
    } else {
        ret = 0;
    }

exit:
    return ret;
}

static int ctl_msg_send(blctl_env_t *env, uint16_t type, const void *payload, size_t payload_len)
{
    int ret = 0;
    long msg_buf[CTL_PORT_MSG_LEN_MAX / sizeof(long)];
    ctl_port_msg_hdr_t *req = (ctl_port_msg_hdr_t *)msg_buf;
    size_t msg_len = sizeof(*req) + payload_len;

    memset(req, 0, sizeof(*req));
    memcpy(&req->magic, CTL_PORT_MSG_MAGIC, sizeof(req->magic));
    req->type = type;
    req->length = msg_len;
    if (payload)
        memcpy(req->payload, payload, payload_len);

    if ((ret = nl_msg_out(env, req, msg_len))) {
        goto out;
    }

out:
    return ret;
}

int blctl_msg_recv(blctl_handle_t handle, void **rsp, size_t timeout_ms)
{
    int ret = 0;
    ctl_port_msg_hdr_t *resp;
    blctl_env_t *env = (blctl_env_t *)handle;

    if ((ret = nl_msg_recv(env, timeout_ms))) {
        ret = 1;
        goto out;
    }
    resp = NLMSG_DATA(env->rcv_nlh);

    if (!memcmp(resp->magic, CTL_PORT_MSG_MAGIC, sizeof(resp->magic))) {
        if (rsp)
            *rsp = resp;
    } else {
        bl_loge("recved wrong resp msg magic, "
                "%02x %02x %02x %02x %02x %02x %02x %02x\n",
                resp->magic[0], resp->magic[1], resp->magic[2], resp->magic[3],
                resp->magic[4], resp->magic[5], resp->magic[6], resp->magic[7]);
        ret = 1;
    }
out:
    return ret;
}

/**
 * @return -1 if other type message is received, 0 on success, else on error
 */
static int ctl_msg_recv_type(blctl_env_t *env, uint16_t rsp_type, void **rsp, size_t timeout_ms)
{
    int ret = 0;
    ctl_port_msg_hdr_t *resp = NULL;

    if ((ret = blctl_msg_recv((blctl_handle_t)env, (void *)&resp, timeout_ms)))
        goto out;
    if (resp->type != rsp_type) {
        ret = -1;
    }
    *rsp = resp;
out:
    return ret;
}

static int ctl_msg_send_expect_recv(blctl_env_t *env, uint16_t type, const void *payload, size_t payload_len, uint16_t rsp_type, void **rsp, size_t timeout_ms)
{
    int ret;

    if ((ret = ctl_msg_send(env, type, payload, payload_len)))
        goto out;
    if ((ret = ctl_msg_recv_type(env, rsp_type, rsp, timeout_ms)))
        goto out;

out:
    return ret;
}

static int create_session(blctl_env_t *env)
{
    int ret;
    ctl_port_msg_conn_rsp_t *rsp;

    if ((ret = ctl_msg_send_expect_recv(env, CTL_PORT_MSG_CONNECT_REQ, NULL, 0,
                    CTL_PORT_MSG_CONNECT_RSP, (void *)&rsp, 100)) == 0 &&
            rsp->success) {
        env->sid = rsp->session_id;
        return 0;
    } else {
        return 1;
    }
}

static int delete_session(blctl_env_t *env)
{
    int ret;
    ctl_port_msg_conn_rsp_t *rsp;

    if ((ret = ctl_msg_send_expect_recv(env, CTL_PORT_MSG_DISCONNECT_REQ, NULL, 0,
                    CTL_PORT_MSG_DISCONNECT_RSP, (void *)&rsp, 100)) == 0 &&
            rsp->success) {
        return 0;
    } else {
        return 1;
    }
}

static wait_env_t *alloc_wait_env(blctl_env_t *env, rnm_base_msg_t *msg)
{
    wait_env_t *ret = NULL;
    size_t i;

    pthread_mutex_lock(&env->wait_slots_lock);
    for (i = 0; i < WAIT_SLOTS; ++i) {
        if (env->wait_slots[i] == NULL)
            break;
    }

    if (i >= WAIT_SLOTS) {
        bl_logw("wait slots are empty now, here are active slots, please check:\n");
        for (i = 0; i < WAIT_SLOTS; i++)
            bl_logw("wait slot[%d]: cmd_id %d\n", i, env->wait_slots[i]->cmd_id);
        return ret;
    }

    ret = calloc(1, sizeof(wait_env_t));
    if (ret) {
        ret->idx = i;
        sem_init(&ret->sem, 0, 0);
        ret->cmd_id = msg->cmd;
        msg->msg_id = ++env->tx_last_seq;
        ret->seq = msg->msg_id;

        env->wait_slots[i] = ret;
    }
    pthread_mutex_unlock(&env->wait_slots_lock);

    return ret;
}

static void free_wait_env(blctl_env_t *env, wait_env_t *wait)
{
    pthread_mutex_lock(&env->wait_slots_lock);
    sem_destroy(&wait->sem);
    free(wait->resp);
    env->wait_slots[wait->idx] = NULL;
    pthread_mutex_unlock(&env->wait_slots_lock);
    free(wait);
}

int blctl_rnm_msg_expect(blctl_handle_t handle, void *msg, size_t msg_len, void **response, size_t timeout_ms)
{
    blctl_env_t *env = handle;
    int ret = -1;
    ctl_port_msg_hdr_t *rsp;
    rnm_base_msg_t *xmsg = msg;
    wait_env_t *wait;
    rnm_base_msg_t *copy;
    size_t copy_len;

    xmsg->session_id = env->sid;
    wait = alloc_wait_env(env, msg);
    if (!wait) {
        bl_loge("no more wait slot\n");
        goto out;
    }
    if ((ret = ctl_msg_send(env, CTL_PORT_MSG_TRANSPARENT_HOST2DEVICE, xmsg, msg_len))) {
        free_wait_env(env, wait);
        goto out;
    }

    if ((ret = sem_timedwait_ms(&wait->sem, timeout_ms))) {
        free_wait_env(env, wait);
        goto out;
    }
    rsp = wait->resp;
    copy_len = rsp->length - CTL_PORT_MSG_HDR_LEN;
    if (!(copy = malloc(copy_len))) {
        free_wait_env(env, wait);
        goto out;
    }
    memcpy(copy, rsp->payload, copy_len);
    free_wait_env(env, wait);
    *response = copy;

    ret = 0;

out:
    return ret;
}

int blctl_rnm_msg_expect_simple(blctl_handle_t handle, void *msg, size_t msg_len, uint16_t *status, size_t timeout_ms)
{
    int ret;
    rnm_base_msg_t *rsp;

    if ((ret = blctl_rnm_msg_expect(handle, msg, msg_len, (void *)&rsp, timeout_ms)))
        return ret;

    if (status)
        *status = rsp->status;
    free(rsp);
    return 0;
}

/* this api will deprecation, use blctl_rnm_user_send_extension replace */
int blctl_rnm_user_ext_send(blctl_handle_t handle, const void *msg, size_t msg_len)
{
    blctl_env_t *env = handle;
    char msg_buf[1514];
    rnm_user_ext_msg_t *rm = (rnm_user_ext_msg_t *)msg_buf;

    if (msg_len > sizeof(msg_buf) - sizeof(*rm))
        return -1;

    memset(&rm->hdr, 0, sizeof(rm->hdr));
    rm->hdr.cmd = BF1B_CMD_USER_EXT;
    rm->hdr.session_id = env->sid;
    memcpy(rm->payload, msg, msg_len);

    return ctl_msg_send(env, CTL_PORT_MSG_TRANSPARENT_HOST2DEVICE, rm, sizeof(*rm) + msg_len);
}
/* end */

int blctl_rnm_send_resp(blctl_handle_t handle, uint16_t cmdid, uint16_t msgid,
        const void *msg, size_t msglen)
{
    rnm_base_msg_t *hdr;
    blctl_env_t *env = handle;
    uint32_t total_len = msglen + sizeof(*hdr);
    char msgbuf[CTL_PORT_MSG_LEN_MAX - CTL_PORT_MSG_HDR_LEN];

    if (!handle || !msg || !msglen)
        return -1;
    if (total_len > sizeof(msgbuf))
        return -1;

    hdr = (rnm_base_msg_t *)msgbuf;
    memset(hdr, 0, sizeof(*hdr));
    /* this is an ack msg */
    hdr->flags = RNM_MSG_FLAG_ACK;
    /* this is the request this msg acks */
    hdr->cmd = cmdid;
    hdr->msg_id = msgid;
    hdr->session_id = env->sid;
    memcpy(&msgbuf[sizeof(*hdr)], msg, msglen);
    return ctl_msg_send(env, CTL_PORT_MSG_TRANSPARENT_HOST2DEVICE, msgbuf,
            total_len);
}

int blctl_rnm_user_send_extension(blctl_handle_t handle, const void *msg, size_t msg_len, uint16_t cmd)
{
    blctl_env_t *env = handle;
    char msg_buf[1514];
    rnm_user_ext_msg_t *rm = (rnm_user_ext_msg_t *)msg_buf;

    if (msg_len > sizeof(msg_buf) - sizeof(*rm))
        return -1;

    memset(&rm->hdr, 0, sizeof(rm->hdr));
    rm->hdr.cmd = cmd;
    rm->hdr.session_id = env->sid;
    memcpy(rm->payload, msg, msg_len);

    return ctl_msg_send(env, CTL_PORT_MSG_TRANSPARENT_HOST2DEVICE, rm, sizeof(*rm) + msg_len);
}

static void *read_thread(void *parm)
{
    int ret;
    blctl_env_t *env = parm;
    void *rsp;
    size_t i;

    while (!env->read_thread_should_exit) {
        ctl_port_msg_hdr_t *cm, *copy = NULL;

        rsp = NULL;
        ret = ctl_msg_recv_type(env, CTL_PORT_MSG_TRANSPARENT_DEVICE2HOST, &rsp, 100);

        if (ret <= 0) {
            cm = rsp;
            copy = malloc(cm->length);
            if (!copy)
                continue;
            memcpy(copy, cm, cm->length);
        }

        if (ret == 0) {
            rnm_base_msg_t *rm = (rnm_base_msg_t *)copy->payload;

            if (rm->flags & RNM_MSG_FLAG_ACK) {
                pthread_mutex_lock(&env->wait_slots_lock);
                for (i = 0; i < WAIT_SLOTS; ++i) {
                    if (!env->wait_slots[i])
                        continue;
                    wait_env_t *wait = env->wait_slots[i];
                    if (rm->cmd == wait->cmd_id && rm->msg_id_replying == wait->seq) {
                        wait->resp = copy;
                        copy = NULL;
                        sem_post(&wait->sem);
                        break;
                    }
                }
                pthread_mutex_unlock(&env->wait_slots_lock);
            }
            if (copy) {
                goto wildcard;
            }
        } else if (ret < 0) {
wildcard:
            if (env->msg_handler) {
                env->msg_handler(copy, env->msg_handler_arg);
                copy = NULL;
            }
        }

        free(copy);
    }

    return NULL;
}

static int blctl_send_hello(blctl_env_t *env)
{
    rnm_base_msg_t msg;

    memset(&msg, 0, sizeof(msg));
    msg.cmd = BF1B_CMD_HELLO;
    return ctl_msg_send(env, CTL_PORT_MSG_TRANSPARENT_HOST2DEVICE, &msg, sizeof(msg));
}

int blctl_init2(blctl_handle_t *handle, bool daemon)
{
    int ret = 0;
    blctl_env_t *env = NULL;
    int sock = -1;
    struct sockaddr_nl src_addr;
    struct nlmsghdr *rcv_nlh = NULL;

    if (!(env = calloc(1, sizeof(blctl_env_t)))) {
        bl_loge("no mem for blctl env\n");
        ret = ENOMEM;
        goto error;
    }

    if (!(rcv_nlh = malloc(NLMSG_SPACE(NL_RCV_MAX_PAYLOAD)))) {
        bl_loge("no mem for blctl env recv buffer\n");
        ret = ENOMEM;
        goto error;
    }

    sock = socket(PF_NETLINK, SOCK_RAW, BLCTL_PROTO);
    if (sock < 0) {
        bl_loge("failed to create netlink socket, %d, %s\n",
                errno, strerror(errno));
        ret = errno;
        goto error;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    if (bind(sock, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        bl_loge("failed to bind netlink socket, %d, %s\n",
                errno, strerror(errno));
        ret = errno;
        goto error;
    }

    env->kaddr.nl_family = AF_NETLINK;
    env->kaddr.nl_pid = 0;
    env->kaddr.nl_groups = 0;

    env->sock = sock;
    env->rcv_nlh = rcv_nlh;
    env->daemon = daemon;

    if ((ret = create_session(env))) {
        bl_loge("failed to create new session, %d\n", ret);
        goto error;
    }

    if (!daemon) {
        pthread_mutex_init(&env->wait_slots_lock, NULL);
        pthread_create(&env->read_thread_id, NULL, read_thread, env);
    }

    *handle = env;
    if (daemon && blctl_send_hello(env)) {
        bl_loge("blctl failed to send hello\r\n");
    }
    return 0;

error:
    if (sock >= 0) {
        close(sock);
        env->sock = -1;
    }
    free(rcv_nlh);
    env->rcv_nlh = NULL;
    free(env);
    return ret;
}

int blctl_register_wildcard_msg_handler(blctl_handle_t *handle, blctl_wildcard_msg_handler_t callback, void *callback_arg)
{
    blctl_env_t *env = (blctl_env_t *)handle;

    if (!env)
        return -1;

    env->msg_handler = callback;
    env->msg_handler_arg = callback_arg;

    return 0;
}

int blctl_init(blctl_handle_t *handle)
{
    return blctl_init2(handle, false);
}

void blctl_deinit(blctl_handle_t handle)
{
    blctl_env_t *env = (blctl_env_t *)handle;

    if (!env) {
        return;
    }

    if (!env->daemon) {
        env->read_thread_should_exit = true;
        pthread_join(env->read_thread_id, NULL);
    }

    if (env->sock >= 0) {
        delete_session(env);
        close(env->sock);
    }

    free(env->rcv_nlh);
    free(env);
}
