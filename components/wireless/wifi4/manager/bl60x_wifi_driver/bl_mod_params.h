
/**
 ****************************************************************************************
 *
 * @file bl_mod_params.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#ifndef _RWNX_MOD_PARAM_H_
#define _RWNX_MOD_PARAM_H_
#include "lmac_types.h"
#include "bl_defs.h"
#include "cfg80211.h"

struct bl_mod_params {
    bool ht_on;
    bool vht_on;
    int mcs_map;
    int phy_cfg;
    int uapsd_timeout;
    bool sgi;
    bool sgi80;
    bool use_2040;
    int listen_itv;
    bool listen_bcmc;
    int lp_clk_ppm;
    bool ps_on;
    int tx_lft;
    int amsdu_maxnb;
    int uapsd_queues;
};

extern struct bl_mod_params bl_mod_params;

struct bl_hw;
void bl_enable_wapi(struct bl_hw *bl_hw);
void bl_enable_mfp(struct bl_hw *bl_hw);
int bl_handle_dynparams(struct bl_hw *bl_hw);

#endif /* _RWNX_MOD_PARAM_H_ */
