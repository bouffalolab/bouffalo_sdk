#include "nh_internal.h"

static const struct nhif_ops *nh_host_link_get_endpoint_ops(void)
{
    nethub_context_t *ctx = nh_ctx_get();

    if (ctx->profile != NULL && ctx->profile->host_endpoint_ops != NULL) {
        return ctx->profile->host_endpoint_ops;
    }

    return nh_profile_get_endpoint_ops(0);
}

bool nethub_host_link_is_idle(void)
{
    const struct nhif_ops *ops = nh_host_link_get_endpoint_ops();

    if (ops == NULL || ops->is_idle == NULL) {
        return true;
    }

    return ops->is_idle();
}

int nethub_host_link_lowpower_prepare(void)
{
    const struct nhif_ops *ops = nh_host_link_get_endpoint_ops();

    if (ops == NULL || ops->lowpower_prepare == NULL) {
        return NETHUB_OK;
    }

    return ops->lowpower_prepare();
}

int nethub_host_link_lowpower_resume(void)
{
    const struct nhif_ops *ops = nh_host_link_get_endpoint_ops();

    if (ops == NULL || ops->lowpower_resume == NULL) {
        return NETHUB_OK;
    }

    return ops->lowpower_resume();
}
