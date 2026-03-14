/**
 * @file channel.c
 * @brief Runtime channel backend dispatcher
 */

#include "channel.h"
#include "../include/bflbwifi_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const channel_ops_t channel_tty_ops;
extern const channel_ops_t channel_vchan_ops;

static const channel_ops_t *channel_backend_ops(const channel_backend_t *backend)
{
    if (!backend || !backend->ops || !channel_type_is_valid(backend->type)) {
        return NULL;
    }

    return backend->ops;
}

/**
 * @brief Get channel operations structure by runtime type
 */
const channel_ops_t *channel_get_ops(channel_type_t type)
{
    switch (type) {
        case CHANNEL_TYPE_TTY:
            return &channel_tty_ops;
        case CHANNEL_TYPE_VCHAN:
            return &channel_vchan_ops;
        default:
            BFLB_LOGE("Unsupported channel type: %u", (unsigned int)type);
            return NULL;
    }
}

/**
 * @brief Validate channel type
 */
bool channel_type_is_valid(channel_type_t type)
{
    return type == CHANNEL_TYPE_TTY || type == CHANNEL_TYPE_VCHAN;
}

/**
 * @brief Get channel type name
 */
const char *channel_get_type_name(channel_type_t type)
{
    switch (type) {
        case CHANNEL_TYPE_TTY:
            return "tty";
        case CHANNEL_TYPE_VCHAN:
            return "vchan";
        default:
            return "unknown";
    }
}

void channel_backend_reset(channel_backend_t *backend)
{
    if (!backend) {
        return;
    }

    backend->type = CHANNEL_TYPE_NONE;
    backend->ops = NULL;
}

bool channel_backend_is_valid(const channel_backend_t *backend)
{
    return channel_backend_ops(backend) != NULL;
}

channel_type_t channel_backend_type(const channel_backend_t *backend)
{
    return channel_backend_is_valid(backend) ? backend->type : CHANNEL_TYPE_NONE;
}

const char *channel_backend_name(const channel_backend_t *backend)
{
    return channel_get_type_name(channel_backend_type(backend));
}

bool channel_backend_has_capability(const channel_backend_t *backend, uint32_t cap)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    return channel_has_capability(ops, cap);
}

bool channel_backend_supports_ota(const channel_backend_t *backend)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops) {
        return false;
    }

    return channel_has_capability(ops, CHANNEL_CAP_RX_THREAD_CTL) &&
           channel_has_capability(ops, CHANNEL_CAP_RAW_FD) &&
           ops->suspend_recv_thread != NULL &&
           ops->resume_recv_thread != NULL &&
           ops->get_fd != NULL;
}

int channel_backend_init(channel_backend_t *backend,
                         channel_type_t type,
                         const char *dev,
                         int baudrate,
                         channel_data_callback_t callback,
                         void *user_data)
{
    const channel_ops_t *ops;
    int ret;

    if (!backend) {
        return -1;
    }

    ops = channel_get_ops(type);
    if (!ops) {
        return -1;
    }

    channel_backend_reset(backend);
    backend->type = type;
    backend->ops = ops;

    ret = ops->init(dev, baudrate, callback, user_data);
    if (ret != 0) {
        channel_backend_reset(backend);
    }

    return ret;
}

void channel_backend_deinit(channel_backend_t *backend)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (ops && ops->deinit) {
        ops->deinit();
    }

    channel_backend_reset(backend);
}

channel_state_t channel_backend_get_state(const channel_backend_t *backend)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops || !ops->get_state) {
        return CHANNEL_STATE_DISCONNECTED;
    }

    return ops->get_state();
}

void channel_backend_set_disconnect_callback(const channel_backend_t *backend,
                                             channel_disconnect_callback_t callback)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (ops && ops->set_disconnect_callback) {
        ops->set_disconnect_callback(callback);
    }
}

void channel_backend_set_reconnect_callback(const channel_backend_t *backend,
                                            channel_reconnect_callback_t callback)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (ops && ops->set_reconnect_callback) {
        ops->set_reconnect_callback(callback);
    }
}

int channel_backend_send(const channel_backend_t *backend, const char *data, int len)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops || !ops->send) {
        return -1;
    }

    return ops->send(data, len);
}

int channel_backend_send_command(const channel_backend_t *backend, const char *cmd)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops || !ops->send_command) {
        return -1;
    }

    return ops->send_command(cmd);
}

int channel_backend_send_command_sync(const channel_backend_t *backend, const char *cmd, int timeout_ms)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops || !ops->send_command_sync) {
        return -1;
    }

    return ops->send_command_sync(cmd, timeout_ms);
}

int channel_backend_wait_for(const channel_backend_t *backend, const char *expected, int timeout_ms)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops || !ops->wait_for) {
        return -1;
    }

    return ops->wait_for(expected, timeout_ms);
}

int channel_backend_get_last_response(const channel_backend_t *backend, char *buf, size_t len)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops || !ops->get_last_response) {
        return -1;
    }

    return ops->get_last_response(buf, len);
}

void channel_backend_clear_last_response(const channel_backend_t *backend)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (ops && ops->clear_last_response) {
        ops->clear_last_response();
    }
}

int channel_backend_suspend_recv_thread(const channel_backend_t *backend)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops || !ops->suspend_recv_thread) {
        return -1;
    }

    return ops->suspend_recv_thread();
}

int channel_backend_resume_recv_thread(const channel_backend_t *backend)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops || !ops->resume_recv_thread) {
        return -1;
    }

    return ops->resume_recv_thread();
}

int channel_backend_get_fd(const channel_backend_t *backend)
{
    const channel_ops_t *ops = channel_backend_ops(backend);

    if (!ops || !ops->get_fd) {
        return -1;
    }

    return ops->get_fd();
}
