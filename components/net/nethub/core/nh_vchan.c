#include "nh_internal.h"

static bool nethub_vchan_type_is_valid(nethub_vchan_type_t type)
{
    return (type > NETHUB_VCHAN_TYPE_INVALID) && (type < NETHUB_VCHAN_TYPE_MAX);
}

int nethub_vchan_send(nethub_vchan_type_t type, const void *data, uint16_t len)
{
    nethub_context_t *ctx = nh_ctx_get();
    const nh_vchan_ops_t *ops = NULL;

    if (ctx->profile != NULL && ctx->profile->vchan_ops != NULL) {
        ops = ctx->profile->vchan_ops;
    } else {
        ops = nh_profile_get_vchan_ops();
    }

    if (ops == NULL || ops->send == NULL) {
        return NETHUB_ERR_NOT_SUPPORTED;
    }

    return ops->send(type, data, len);
}

int nethub_vchan_recv_register(nethub_vchan_type_t type,
                               nethub_vchan_recv_cb_t recv_cb,
                               void *arg)
{
    bool found = false;
    int first_error = NETHUB_OK;
    size_t i;

    if (!nethub_vchan_type_is_valid(type)) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    for (i = 0; i < nh_profile_get_host_count(); i++) {
        const nh_profile_t *profile = nh_profile_get_host(i);
        const nh_vchan_ops_t *ops = profile ? profile->vchan_ops : NULL;
        int ret;

        if (ops == NULL || ops->recv_register == NULL) {
            continue;
        }

        found = true;
        ret = ops->recv_register(type, recv_cb, arg);
        if (ret != NETHUB_OK && first_error == NETHUB_OK) {
            first_error = ret;
        }
    }

    if (!found) {
        return NETHUB_ERR_NOT_SUPPORTED;
    }

    return first_error;
}

int nethub_vchan_user_send(const void *data, uint16_t len)
{
    return nethub_vchan_send(NETHUB_VCHAN_TYPE_USER, data, len);
}

int nethub_vchan_user_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg)
{
    return nethub_vchan_recv_register(NETHUB_VCHAN_TYPE_USER, recv_cb, cb_arg);
}

int nethub_vchan_at_send(const void *data, uint16_t len)
{
    return nethub_vchan_send(NETHUB_VCHAN_TYPE_AT, data, len);
}

int nethub_vchan_at_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg)
{
    return nethub_vchan_recv_register(NETHUB_VCHAN_TYPE_AT, recv_cb, cb_arg);
}
