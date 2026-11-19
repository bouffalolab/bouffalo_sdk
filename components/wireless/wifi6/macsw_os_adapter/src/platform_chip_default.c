#include <stddef.h>

#include "wifi6_chip.h"
#include "wifi_mgmr_coex_internal.h"

#if WIFI6_RESTORE_EM_CONFIG
#include "bl616_sys.h"
#endif

/* Default RF bring-up implemented by the platform adapter. */
extern int platform_rf_param_init(void);

int platform_chip_rf_init(void)
{
#if WIFI6_RESTORE_EM_CONFIG
    /* Restore the configured Wi-Fi/BT memory split after low-power setup. */
    if (bl_sys_em_config() != 0) {
        return -1;
    }
#endif
    return platform_rf_param_init();
}

const struct wifi_mgmr_coex_rf_ops *platform_coex_rf_ops_get(void)
{
    return NULL;
}

int platform_coex_spdt_debug_apply(enum wifi_mgmr_coex_spdt_debug_mode mode)
{
    (void)mode;
    return WIFI_MGMR_COEX_ERR_NOT_SUPPORTED;
}
