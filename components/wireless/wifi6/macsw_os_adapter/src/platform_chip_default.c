#include <stddef.h>

#include "wifi_mgmr_coex_internal.h"

const struct wifi_mgmr_coex_rf_ops *platform_coex_rf_ops_get(void)
{
    return NULL;
}

int platform_coex_spdt_debug_apply(enum wifi_mgmr_coex_spdt_debug_mode mode)
{
    (void)mode;
    return WIFI_MGMR_COEX_ERR_NOT_SUPPORTED;
}
