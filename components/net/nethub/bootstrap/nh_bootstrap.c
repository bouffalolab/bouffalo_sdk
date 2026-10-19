#include "nethub.h"
#include "nh_filter.h"
#include "nh_hub.h"
#include "nh_profile.h"

#define DBG_TAG "NETHUB_BOOTSTRAP"
#include "log.h"

static int nh_bootstrap_register_endpoints(void)
{
    size_t endpoint_count;
    size_t i;
    int ret;

    endpoint_count = nh_profile_get_endpoint_count();
    if (endpoint_count == 0) {
        return NETHUB_ERR_NOT_FOUND;
    }

    for (i = 0; i < endpoint_count; i++) {
        const struct nhif_ops *ops = nh_profile_get_endpoint_ops(i);

        if (ops == NULL) {
            nhif_unregister_all();
            return NETHUB_ERR_INVALID_PARAM;
        }

        ret = nhif_register(ops, NULL);
        if (ret != NETHUB_OK) {
            nhif_unregister_all();
            return ret;
        }
    }

    return NETHUB_OK;
}

int nethub_bootstrap(void)
{
    int ret;

    ret = nh_bootstrap_register_endpoints();
    if (ret != NETHUB_OK) {
        LOG_E("register endpoints failed: %d\r\n", ret);
        return ret;
    }

    ret = nethub_init(NULL);
    if (ret != NETHUB_OK) {
        nhif_unregister_all();
        LOG_E("nethub_init failed: %d\r\n", ret);
        return ret;
    }

    ret = nethub_start();
    if (ret != NETHUB_OK) {
        nethub_deinit();
        LOG_E("nethub_start failed: %d\r\n", ret);
        return ret;
    }

    if (nh_filter_custom_wifi_rx_is_active()) {
        LOG_W("custom wifi rx filter is active, builtin policy is bypassed\r\n");
    }

    LOG_I("nethub started with profile=%s\r\n", nethub_profile_name());
    return NETHUB_OK;
}

void nethub_shutdown(void)
{
    nethub_stop();
    nethub_deinit();
}
