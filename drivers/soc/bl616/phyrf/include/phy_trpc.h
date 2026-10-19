#ifndef _PHY_TRPC_H_
#define _PHY_TRPC_H_

#include "co_int.h"

#define PHY_TRPC_MAX_DBM    (25)
#define PHY_TRPC_NUM_MCS    (10)

enum
{
    PHY_TRPC_11B = 0,
    PHY_TRPC_11G,
    PHY_TRPC_11N,
    PHY_TRPC_11AC,
    PHY_TRPC_11AX,
    PHY_TRPC_MODE_MAX   
};

/*
 * STRUCTURES/TYPES DEFINITION
 ****************************************************************************************
 */
struct trpc_env_tag 
{
    /// RF output power capability
    int8_t power_dbm_max_rf;
    int8_t power_dbm_min_rf;
    /// Maximum power limited by regulations (SRRC/FCC/CE...)
    int8_t power_dbm_lim_reg;
    /// Current channel
    int16_t channel_freq;
    uint8_t channel;
    uint8_t channel_type;
    /// Current temperature
    int8_t temperature;
    int8_t temperature_compensate;
};


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

extern struct trpc_env_tag trpc_env;


/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
void trpc_init();
void trpc_update_power(uint8_t cbw);
void trpc_update_power_11b(int8_t power_rate_table[4]);
void trpc_update_power_11g(int8_t power_rate_table[8]);
void trpc_update_power_11n(int8_t power_rate_table[8]);
void trpc_update_power_11ac(int8_t power_rate_table[10]);
void trpc_update_power_11ax(int8_t power_rate_table[10]);
void trpc_update_vs_channel(uint8_t channel, uint8_t chantype);
void trpc_update_power_limit(int8_t channel, int8_t en, uint8_t limit[12]);

uint8_t trpc_get_default_power_idx(uint8_t formatmod, uint8_t mcs);
uint8_t trpc_get_power_idx(uint8_t formatmod, uint8_t mcs, int8_t pwr_dbm);

#endif
