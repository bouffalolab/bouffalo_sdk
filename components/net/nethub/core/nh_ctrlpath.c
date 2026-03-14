#include "nh_internal.h"

static const nh_ctrlpath_ops_t *nh_ctrlpath_get_ops(void)
{
    nethub_context_t *ctx = nh_ctx_get();

    if (ctx->profile != NULL && ctx->profile->ctrlpath_ops != NULL) {
        return ctx->profile->ctrlpath_ops;
    }

    return nh_profile_get_ctrlpath_ops();
}

int nethub_ctrl_upld_send(uint8_t *data_buff, uint32_t data_size)
{
    const nh_ctrlpath_ops_t *ops = nh_ctrlpath_get_ops();

    if (ops == NULL || ops->upld_send == NULL) {
        return NETHUB_ERR_NOT_SUPPORTED;
    }

    return ops->upld_send(data_buff, data_size);
}

int nethub_ctrl_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg)
{
    const nh_ctrlpath_ops_t *ops = nh_ctrlpath_get_ops();

    if (ops == NULL || ops->dnld_register == NULL) {
        return NETHUB_ERR_NOT_SUPPORTED;
    }

    return ops->dnld_register(dnld_cb, cbpri_arg);
}
