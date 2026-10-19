#include <stddef.h>
#include <stdint.h>

#include "wifi_mgmr_coex_internal.h"

#if defined(CONFIG_RF) && defined(CONFIG_FHOST)
#include <errno.h>
#include "board_rf.h"
#endif

#if defined(CONFIG_RF) && defined(CONFIG_FHOST) && \
    defined(BOARD_RF_COEX_OPS_SUPPORTED)
static int platform_coex_rf_result(int ret)
{
    switch (ret) {
    case 0: return WIFI_MGMR_COEX_OK;
    case -EINVAL: return WIFI_MGMR_COEX_ERR_INVALID_ARGUMENT;
    case -ENOTSUP: return WIFI_MGMR_COEX_ERR_NOT_SUPPORTED;
    case -EAGAIN:
    case -ENODEV: return WIFI_MGMR_COEX_ERR_NOT_READY;
    case -EBUSY: return WIFI_MGMR_COEX_ERR_BUSY;
    default: return WIFI_MGMR_COEX_ERR_APPLY_FAILED;
    }
}

static int platform_coex_board_modes_get(
    const struct wifi_mgmr_coex_board_config *board, uint32_t *modes)
{
    uint32_t board_modes;
    int ret;

    if (board == NULL || modes == NULL ||
        board->topology < WIFI_MGMR_COEX_BOARD_COMBO_SHARED_PATH ||
        board->topology > WIFI_MGMR_COEX_BOARD_STANDALONE_SINGLE_ANT_SPDT ||
        (board->topology == WIFI_MGMR_COEX_BOARD_STANDALONE_SINGLE_ANT_SPDT ?
         board->spdt_gpio < 0 : board->spdt_gpio != -1)) {
        return WIFI_MGMR_COEX_ERR_INVALID_ARGUMENT;
    }
    ret = board_rf_modes_get(board->topology != WIFI_MGMR_COEX_BOARD_COMBO_SHARED_PATH,
                            board->spdt_gpio, &board_modes);
    if (ret != 0) {
        return platform_coex_rf_result(ret);
    }
    *modes = 0;
    if (board_modes & BOARD_RF_MODE_COMBO_BIT) {
        *modes |= 1u << WIFI_MGMR_COEX_RF_COMBO;
    }
    if (board_modes & BOARD_RF_MODE_STANDALONE_BIT) {
        *modes |= 1u << WIFI_MGMR_COEX_RF_STANDALONE;
    }
    return WIFI_MGMR_COEX_OK;
}

static int platform_coex_rf_prepare(
    const struct wifi_mgmr_coex_board_config *board,
    enum wifi_mgmr_coex_rf_mode mode)
{
    uint32_t modes;
    int ret;

    if (mode != WIFI_MGMR_COEX_RF_COMBO && mode != WIFI_MGMR_COEX_RF_STANDALONE) {
        return WIFI_MGMR_COEX_ERR_INVALID_ARGUMENT;
    }
    ret = platform_coex_board_modes_get(board, &modes);
    if (ret != WIFI_MGMR_COEX_OK) {
        return ret;
    }
    if (!(modes & (1u << mode))) {
        return WIFI_MGMR_COEX_ERR_NOT_SUPPORTED;
    }
    switch (board->topology) {
    case WIFI_MGMR_COEX_BOARD_COMBO_SHARED_PATH:
        ret = board_rf_combo_init();
        break;
    case WIFI_MGMR_COEX_BOARD_STANDALONE_DUAL_ANT:
        ret = board_rf_dual_ant_init();
        break;
    case WIFI_MGMR_COEX_BOARD_STANDALONE_SINGLE_ANT_SPDT:
        ret = board_rf_single_ant_spdt_init(board->spdt_gpio);
        break;
    default:
        return WIFI_MGMR_COEX_ERR_INVALID_ARGUMENT;
    }
    return platform_coex_rf_result(ret);
}

static int platform_coex_rf_status_get(void)
{
    return platform_coex_rf_result(board_rf_init_status_get());
}

static int platform_coex_spdt_apply(uint8_t mode)
{
    switch (mode) {
    case COEXM_SPDT_FIXED_BT:
        return board_rf_spdt_mode_apply(BOARD_RF_SPDT_FIXED_BT);
    case COEXM_SPDT_DYNAMIC_PTA:
        return board_rf_spdt_mode_apply(BOARD_RF_SPDT_DYNAMIC_PTA);
    default:
        return -EINVAL;
    }
}

static int platform_coex_spdt_verify(uint8_t mode)
{
    switch (mode) {
    case COEXM_SPDT_FIXED_BT:
        return board_rf_spdt_mode_verify(BOARD_RF_SPDT_FIXED_BT);
    case COEXM_SPDT_DYNAMIC_PTA:
        return board_rf_spdt_mode_verify(BOARD_RF_SPDT_DYNAMIC_PTA);
    default:
        return -EINVAL;
    }
}

static const struct wifi_mgmr_coex_rf_ops platform_coex_rf_ops = {
    .board_modes_get = platform_coex_board_modes_get,
    .prepare = platform_coex_rf_prepare,
    .status_get = platform_coex_rf_status_get,
    .spdt = {
        .spdt_apply = platform_coex_spdt_apply,
        .spdt_verify = platform_coex_spdt_verify,
    },
};
#endif

const struct wifi_mgmr_coex_rf_ops *platform_coex_rf_ops_get(void)
{
#if defined(CONFIG_RF) && defined(CONFIG_FHOST) && \
    defined(BOARD_RF_COEX_OPS_SUPPORTED)
    return &platform_coex_rf_ops;
#else
    return NULL;
#endif
}

int platform_coex_spdt_debug_apply(enum wifi_mgmr_coex_spdt_debug_mode mode)
{
#if defined(CONFIG_RF) && defined(CONFIG_FHOST) && \
    defined(BOARD_RF_COEX_OPS_SUPPORTED) && defined(CONFIG_WIFI_COEX_DEBUG_CLI)
    switch (mode) {
    case WIFI_MGMR_COEX_SPDT_DEBUG_2G:
        return platform_coex_rf_result(board_rf_spdt_mode_apply(BOARD_RF_SPDT_FIXED_2G));
    case WIFI_MGMR_COEX_SPDT_DEBUG_BT:
        return platform_coex_rf_result(board_rf_spdt_mode_apply(BOARD_RF_SPDT_FIXED_BT));
#ifdef CONFIG_COEX_SPDT_EXPERIMENT
    case WIFI_MGMR_COEX_SPDT_DEBUG_PTA:
        /* Lab-only handover; caller already checked MACSW readiness/ownership. */
        if ((*(volatile uint32_t *)0x24920028u & (1u << 31)) == 0) {
            return WIFI_MGMR_COEX_ERR_NOT_READY;
        }
        return platform_coex_rf_result(board_rf_spdt_mode_apply(BOARD_RF_SPDT_DYNAMIC_PTA));
#endif
    default:
        break;
    }
#else
    (void)mode;
#endif
    return WIFI_MGMR_COEX_ERR_NOT_SUPPORTED;
}
