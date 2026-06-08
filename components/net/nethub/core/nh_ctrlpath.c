#include "nh_internal.h"

#if defined(CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE) && CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE
static nethub_ctrl_rx_cb_t g_ctrlpath_dnld_cb;
static void *g_ctrlpath_dnld_arg;

static int nethub_ctrlpath_vchan_recv(void *arg, uint8_t *data_buff, uint16_t data_size)
{
    NETHUB_UNUSED(arg);

    if (g_ctrlpath_dnld_cb == NULL) {
        return NETHUB_OK;
    }

    return g_ctrlpath_dnld_cb(g_ctrlpath_dnld_arg, data_buff, (uint32_t)data_size);
}
#endif

int nethub_ctrl_upld_send(uint8_t *data_buff, uint32_t data_size)
{
#if defined(CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE) && CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE
    if (data_size > NETHUB_VCHAN_MAX_DATA_LEN) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    return nethub_vchan_at_send(data_buff, (uint16_t)data_size);
#else
    NETHUB_UNUSED(data_buff);
    NETHUB_UNUSED(data_size);
    return NETHUB_ERR_NOT_SUPPORTED;
#endif
}

int nethub_ctrl_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg)
{
#if defined(CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE) && CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE
    nethub_ctrl_rx_cb_t old_cb = g_ctrlpath_dnld_cb;
    void *old_arg = g_ctrlpath_dnld_arg;
    int ret;

    g_ctrlpath_dnld_cb = dnld_cb;
    g_ctrlpath_dnld_arg = cbpri_arg;

    ret = nethub_vchan_at_recv_register(dnld_cb ? nethub_ctrlpath_vchan_recv : NULL, NULL);
    if (ret != NETHUB_OK) {
        g_ctrlpath_dnld_cb = old_cb;
        g_ctrlpath_dnld_arg = old_arg;
    }

    return ret;
#else
    NETHUB_UNUSED(dnld_cb);
    NETHUB_UNUSED(cbpri_arg);
    return NETHUB_ERR_NOT_SUPPORTED;
#endif
}
