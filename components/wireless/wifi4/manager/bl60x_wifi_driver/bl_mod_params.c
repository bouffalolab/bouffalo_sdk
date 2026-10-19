
/**
 ****************************************************************************************
 *
 * @file bl_mod_params.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include "bl_defs.h"

#define COMMON_PARAM(name, default_softmac, default_fullmac)    \
    .name = default_fullmac,
#define SOFTMAC_PARAM(name, default)
#define FULLMAC_PARAM(name, default) .name = default,

struct bl_mod_params bl_mod_params = {
    /* common parameters */
    COMMON_PARAM(ht_on, true, true)
    COMMON_PARAM(vht_on, false, false)
    COMMON_PARAM(mcs_map, IEEE80211_VHT_MCS_SUPPORT_0_7, IEEE80211_VHT_MCS_SUPPORT_0_7)
    COMMON_PARAM(phy_cfg, 2, 2)
    COMMON_PARAM(uapsd_timeout, 3000, 3000)
    COMMON_PARAM(sgi, false, false)
    COMMON_PARAM(sgi80, false, false)
    COMMON_PARAM(listen_itv, 1, 1)
    COMMON_PARAM(listen_bcmc, true, true)
    COMMON_PARAM(lp_clk_ppm, 20, 20)
    COMMON_PARAM(ps_on, false, false)
    COMMON_PARAM(tx_lft, RWNX_TX_LIFETIME_MS, RWNX_TX_LIFETIME_MS)
    COMMON_PARAM(amsdu_maxnb, NX_TX_PAYLOAD_MAX, NX_TX_PAYLOAD_MAX)
    // By default, only enable UAPSD for Voice queue (see IEEE80211_DEFAULT_UAPSD_QUEUE comment)
    COMMON_PARAM(uapsd_queues, 0, 0)
};

int bl_handle_dynparams(struct bl_hw *bl_hw)
{
    const int nss = 1;

    if (bl_hw->mod_params->phy_cfg < 0 || bl_hw->mod_params->phy_cfg > 5)
        bl_hw->mod_params->phy_cfg = 2;

    if (bl_hw->mod_params->mcs_map < 0 || bl_hw->mod_params->mcs_map > 2)
        bl_hw->mod_params->mcs_map = 0;

    /* HT capabilities */
    bl_hw->ht_cap.cap |= 1 << IEEE80211_HT_CAP_RX_STBC_SHIFT;
    bl_hw->ht_cap.mcs.rx_highest = cpu_to_le16(65 * nss);
    //Fixed leo disable MCS5/6/7
    bl_hw->ht_cap.mcs.rx_mask[0] = 0xFF;

    if (bl_hw->mod_params->sgi) {
        bl_hw->ht_cap.cap |= IEEE80211_HT_CAP_SGI_20;
        bl_hw->ht_cap.mcs.rx_highest = cpu_to_le16(72 * nss);
    }
    bl_hw->ht_cap.cap |= IEEE80211_HT_CAP_SM_PS;
    if (!bl_hw->mod_params->ht_on)
        bl_hw->ht_cap.ht_supported = false;

    return 0;
}
