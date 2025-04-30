/**
 ****************************************************************************************
 *
 * @file phy.h
 *
 * @brief Header file for nX Radio physical layer driver.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */
#ifndef _PHY_H_
#define _PHY_H_

/**
 ****************************************************************************************
 * @defgroup PHY PHY
 * @ingroup PLATFORM_DRIVERS
 * @brief Declaration of the PHY Drivers
 * @{
 ****************************************************************************************
 */
#ifndef PHYRF_LOG_EN
#define PHYRF_LOG_EN 0
#endif

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// default inclusion directives
#include <stdint.h>
#include <stdbool.h>

//for __INLINE
#include "compiler.h"

/*
 * DEFINES
 ****************************************************************************************
 */



#ifndef NX_DEBUG_DUMP
#define NX_DEBUG_DUMP 0
#endif


/// Invalid value for some octet parameters reserved for the future
#define PHY_UNUSED           0xFF

/// Maximum number of words in the configuration buffer
#define PHY_CFG_BUF_SIZE     16

/// 5G lower bound freq
#define PHY_FREQ_5G 5000

/// @name PHY INFO related definitions.
/// @{

/// Macro retrieving the band of the phy channel info
/// @param[in] __x phy channel info 1 value.
#define PHY_INFO_BAND(__x) ((__x.info1) & 0x000000FF)

/// Macro retrieving the bandwidth of the phy channel info
/// @param[in] __x phy channel info 1 value.
#define PHY_INFO_BW(__x) (((__x.info1) & 0x0000FF00) >> 8)

/// Macro retrieving the channel of the phy channel info
/// @param[in] __x phy channel info 1 value.
#define PHY_INFO_CHAN(__x) (((__x.info1) & 0xFFFF0000) >> 16)

/// Macro retrieving the center frequency 1 of the phy channel info
/// @param[in] __x phy channel info 2 value.
#define PHY_INFO_CENT_FREQ1(__x) ((__x.info2) & 0x0000FFFF)

/// Macro retrieving the center frequency 2 of the phy channel info
/// @param[in] __x phy channel info 2 value.
#define PHY_INFO_CENT_FREQ2(__x) (((__x.info2) & 0xFFFF0000) >> 16)

/// @}

/// RF index definitions
enum
{
    /// Primary radar detection chain (i.e for the operating channel)
    PHY_PRIM,
    /// Secondary radar detection chain
    PHY_SEC,
};

/// Channel Band
enum phy_mac_chan_band
{
    /// 2.4GHz Band
    E_PHY_BAND_2G4,
    /// 5GHz band
    E_PHY_BAND_5G,
    /// Number of bands
    E_PHY_BAND_MAX,
};

/// Operating Channel Bandwidth
enum phy_mac_chan_bandwidth
{
    /// 20MHz BW
    E_PHY_CHNL_BW_20,
    /// 40MHz BW
    E_PHY_CHNL_BW_40,
    /// 80MHz BW
    E_PHY_CHNL_BW_80,
    /// 160MHz BW
    E_PHY_CHNL_BW_160,
    /// 80+80MHz BW
    E_PHY_CHNL_BW_80P80,
    /// Reserved BW
    E_PHY_CHNL_BW_OTHER,
};

/// Operating Channel
struct phy_mac_chan_op
{
    /// Band (@ref mac_chan_band)
    uint8_t band;
    /// Channel type (@ref mac_chan_bandwidth)
    uint8_t type;
    /// Frequency for Primary 20MHz channel (in MHz)
    uint16_t prim20_freq;
    /// Frequency center of the contiguous channel or center of Primary 80+80 (in MHz)
    uint16_t center1_freq;
    /// Frequency center of the non-contiguous secondary 80+80 (in MHz)
    uint16_t center2_freq;
    /// Max transmit power allowed on this channel (dBm)
    int8_t tx_power;
    /// Additional information (@ref mac_chan_flags)
    uint8_t flags;
};

// forward declaration
struct dbg_debug_info_tag;

/// Structure containing the information about the PHY channel that was used for this RX
struct phy_channel_info
{
    /// PHY channel information 1
    uint32_t info1;
    /// PHY channel information 2
    uint32_t info2;
};

/// Structure containing information about radar pulse detected
struct phy_radar_pulse
{
    /// In our PHY a radar pulse is only one 32-bit word
    uint32_t pulse;
};

/// Structure containing the parameters of the PHY configuration
struct phy_cfg_tag
{
    /// Buffer containing the parameters specific for the PHY used
    uint32_t parameters[PHY_CFG_BUF_SIZE];
};


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Modem and Radio initialization function.
 * This function is called at platform reset and initializes modem, then radio.
 * @param[in] config     Pointer to the PHY parameters (used radio paths, other cfg)
 ****************************************************************************************
 */
void phy_init(const struct phy_cfg_tag *config);

/**
 ****************************************************************************************
 * @brief Resets some parts of the PHY.
 * This function is called upon a system reset during the recovery procedure.
 ****************************************************************************************
 */
void phy_reset(void);


/**
 ****************************************************************************************
 * @brief PHY version reading function.
 * This function returns the PHY version on two 32-bit words. The format of these two
 * words is PHY dependent.
 *
 * @param[out] version_1  Pointer to the 1st version word
 * @param[out] version_2  Pointer to the 2nd version word
 ****************************************************************************************
 */
void phy_get_version(uint32_t *version_1, uint32_t *version_2);

/**
 ****************************************************************************************
 * @brief Set channel function.
 * This function is called when the MAC stack needs to change the channel on which the
 * radio is locked.
 * The function is blocking until the PLL has locked on the new channel frequency.
 *
 * @param[in] chan    Channel configuration
 * @param[in] index   Index of the RF for which the channel has to be set (@ref PHY_PRIM
 *                    operating (primary RF)), 1: secondary RF (@ref PHY_SEC used for
 *                    additional radar detection). This parameter is reserved if no
 *                    secondary RF is available in the system
 ****************************************************************************************
 */
void phy_set_channel(const void *chan, uint8_t index);

/**
 ****************************************************************************************
 * @brief Get channel function.
 * This function is called when the MAC stack needs to get the information on the channel
 * on which the radio is currently locked.
 *
 * @param[out] info        Pointer on the phy_channel_info structure used to return the
 *                         information
 * @param[in] index        Index of the RF for which the channel has to be retrieved
 *                         (@ref PHY_PRIM or @ref PHY_SEC)
 *
 ****************************************************************************************
 */
void phy_get_channel(struct phy_channel_info *info, uint8_t index);

/**
 ****************************************************************************************
 * @brief PHY stop function.
 * This function is called when the MAC stack needs to stop the PHY.
 * It puts back the MAC HW to the IDLE state
 ****************************************************************************************
 */
void phy_stop(void);

/**
 ****************************************************************************************
 * @brief Get maximum duration to set a new channel.
 * This function is used when scheduling multiple channel context.
 * @return Maximum duration, in ms, to configure a new channel.
 ****************************************************************************************
 */
uint32_t phy_get_channel_switch_dur(void);

/**
 ****************************************************************************************
 * @brief Compute the channel number from its center frequency and its band
 * @param[in] band  RF band (must be @ref E_PHY_BAND_2G4 or @ref E_PHY_BAND_5G)
 * @param[in] freq  Center frequency of the channel
 *
 * @return The channel number, 0 if not found
 ****************************************************************************************
 */
__INLINE int phy_freq_to_channel(uint8_t band, uint16_t freq)
{
    if ((band == E_PHY_BAND_2G4) && (freq >= 2412) && (freq <= 2484))
    {
        if (freq == 2484)
            return 14;
        else
            return (freq - 2407) / 5;
    }
    else if ((band == E_PHY_BAND_5G) && (freq >= 5005) && (freq <= 5885))
    {
        return (freq - PHY_FREQ_5G) / 5;
    }

    return 0;
}

/**
 ****************************************************************************************
 * @brief Compute the channel frequency from its number and band
 * @param[in] band    RF band (must be @ref E_PHY_BAND_2G4 or @ref E_PHY_BAND_5G)
 * @param[in] channel Channel number
 *
 * @return The channel frequency, 0 if not found
 ****************************************************************************************
 */
__INLINE uint16_t phy_channel_to_freq(uint8_t band, int channel)
{
    if ((band == E_PHY_BAND_2G4) && (channel >= 1) && (channel <= 14))
    {
        if (channel == 14)
            return 2484;
        else
            return 2407 + channel * 5;
    }
    else if ((band == E_PHY_BAND_5G) && (channel >= 1) && (channel <= 177))
    {
        return PHY_FREQ_5G + channel * 5;
    }

    return 0;
}

/**
 ****************************************************************************************
 * @brief Modem interrupt service routine.
 * This function is called when a Modem interrupt is asserted to the interrupt controller
 ****************************************************************************************
 */
void phy_mdm_isr(void);

/**
 ****************************************************************************************
 * @brief RC interrupt service routine.
 * This function is called when a RC interrupt is asserted to the interrupt controller
 ****************************************************************************************
 */
void phy_rc_isr(void);

/**
 ****************************************************************************************
 * @brief Indicates if the PHY has radar pulses available or not.
 *
 * @param[in] rd_idx Index of the radar detection chain that needs to be checked
 * (@ref PHY_PRIM or @ref PHY_SEC)
 *
 * @return true if pulse(s) is(are) available, false otherwise
 *
 ****************************************************************************************
 */
bool phy_has_radar_pulse(int rd_idx);

/**
 ****************************************************************************************
 * @brief Read a radar pulse from the FIFO, and writes it to the buffer passed as parameter
 *
 * @param[in] rd_idx Index of the radar detection chain that needs to be read
 * (@ref PHY_PRIM or @ref PHY_SEC)
 * @param[out] pulse Pointer to the pulse structure where to write to
 *
 * @return true if a pulse has been written, false otherwise
 *
 ****************************************************************************************
 */
bool phy_get_radar_pulse(int rd_idx, struct phy_radar_pulse *pulse);

/**
 ****************************************************************************************
 * @brief Return the VHT capability of the PHY
 *
 * @return true if the VHT is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_vht_supported(void);

/**
 ****************************************************************************************
 * @brief Return the HE capability of the PHY
 *
 * @return true if the HE is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_he_supported(void);

/**
 ****************************************************************************************
 * @brief Return the unsupported HT frame handling capability of the PHY
 *
 * @return true if the UF is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_uf_supported(void);

/**
 ****************************************************************************************
 * @brief Configure the unsupported HT frame handling capability of the PHY
 *
 * @param[in] enable Enable/Disable unsupported HT frame handling feature
 ****************************************************************************************
 */
void phy_uf_enable(bool enable);

/**
 ****************************************************************************************
 * @brief Return the LDPC TX capability of the PHY
 *
 * @return true if the LDPC TX is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_ldpc_tx_supported(void);

/**
 ****************************************************************************************
 * @brief Return the LDPC RX capability of the PHY
 *
 * @return true if the LDPC RX is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_ldpc_rx_supported(void);

/**
 ****************************************************************************************
 * @brief Return the Beamformee capability of the PHY
 *
 * @return true if Beamformee is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_bfmee_supported(void);

/**
 ****************************************************************************************
 * @brief Return the Beamformer capability of the PHY
 *
 * @return true if Beamformer is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_bfmer_supported(void);

/**
 ****************************************************************************************
 * @brief Return the MU-MIMO RX capability of the PHY
 *
 * @return true if MU-MIMO RX is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_mu_mimo_rx_supported(void);

/**
 ****************************************************************************************
 * @brief Return the MU-MIMO TX capability of the PHY
 *
 * @return true if MU-MIMO TX is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_mu_mimo_tx_supported(void);

/**
 ****************************************************************************************
 * @brief Set the MU-MIMO RX group information as received in the VHT Group ID action frame
 *
 * @param[in] membership_addr  Address of the membership status in the received frame
 * @param[in] userpos_addr     Address of the user position array in the received frame
 ****************************************************************************************
 */
void phy_set_group_id_info(uint32_t membership_addr, uint32_t userpos_addr);

/**
 ****************************************************************************************
 * @brief Set the AID to the PHY, in order to allow correct operation of OFDMA-RX
 *
 * @param[in] aid  Association ID assigned by the AP
 ****************************************************************************************
 */
void phy_set_aid(uint16_t aid);

/**
 ****************************************************************************************
 * @brief Return the supported BW of the PHY
 *
 * @return 0: BW20MHZ, 1: BW40MHZ, 2: BW80MHZ
 ****************************************************************************************
 */
uint8_t phy_get_bw(void);

/**
 ****************************************************************************************
 * @brief Return the NSS of the PHY
 *
 * @return Number of Spatial Streams supported by the PHY (start from 0, 0 is for 1 SS)
 ****************************************************************************************
 */
uint8_t phy_get_nss(void);

/**
 ****************************************************************************************
 * @brief Return the NTX of the PHY
 *
 * @return Number of antenna supported for TX minus 1 (i.e. 0 means 1 antenna)
 ****************************************************************************************
 */
uint8_t phy_get_ntx(void);

/**
 ****************************************************************************************
 * @brief Return the NRX of the PHY
 *
 * @return Number of antenna supported for RX minus 1 (i.e. 0 means 1 antenna)
 ****************************************************************************************
 */
uint8_t phy_get_nrx(void);

/**
 ****************************************************************************************
 * @brief Return the size of the BFMER memory in kBytes
 *
 * @return The size of the BFMER memory in kilobytes
 ****************************************************************************************
 */
uint8_t phy_get_bfr_mem_size(void);

/**
 ****************************************************************************************
 * @brief Get radio code to set a specific TX power
 * Return the idx to use (set in policy table), to configure a specific TX power. If the TX
 * power requested is not possible, it should return idx to get the maximum TX power that
 * do not exceed the request. In this case, power parameter should be updated to the
 * actual TX power set.
 *
 * @param[in,out] power TX power (in dBm) to configure for the radio, (updated with
 *                      the actual power set if request is not possible)
 * @param[out]    idx   Idx to use in policy table to configure a tx power of \<gain\>
 ****************************************************************************************
 */
void phy_get_rf_gain_idx(int8_t *power, uint8_t *idx);

/**
 ****************************************************************************************
 * @brief Get radio's transmission power capabilities
 *
 * @param[out] max Maximum TX power for the radio (in dBm)
 * @param[out] min Minimum TX power for the radio (in dBm)
 ****************************************************************************************
 */
void phy_get_rf_gain_capab(int8_t *max, int8_t *min);

/**
 ****************************************************************************************
 * @brief Get the antenna set to be put in the policy table.
 * This function returns the antenna set based on the current antenna configuration.
 *
 * @return The antenna set value
 ****************************************************************************************
 */
uint8_t phy_get_antenna_set(void);

#if NX_DEBUG_DUMP
/**
 ****************************************************************************************
 * @brief Read the state of all PHY diagnostic ports and put it in the debug dump info
 * structure
 *
 * @param[out] dbg_info Pointer to the debug dump information structure
 *
 ****************************************************************************************
 */
void phy_get_diag_state(struct dbg_debug_info_tag *dbg_info);
#endif

/**
 ****************************************************************************************
 * @brief Switch paths connections
 *
 * @return Value of PATHMUX register
 ****************************************************************************************
 */
uint8_t phy_switch_antenna_paths(void);

void phy_update_power_table(void);

/// @}


void phy_mdm_reset();
extern int8_t rxgain[];

#endif //_PHY_H_
