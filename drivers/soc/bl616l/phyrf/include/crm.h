/**
 ****************************************************************************************
 *
 * @file crm.h
 *
 * @brief Clock and Reset Module declarations.
 *
 * Copyright (C) RivieraWaves 2018-2020
 *
 ****************************************************************************************
 */

#ifndef _CRM_H_
#define _CRM_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_bool.h"
// #include "rwnx_config.h"


/**
 *****************************************************************************************
 * @defgroup CRM CRM
 * @ingroup PLATFORM_DRIVERS
 * @brief Clock and Reset module.
 * @{
 *****************************************************************************************
 */
#if NX_PLF_VER == 20
/**
 * @name Aliases used to map CRM accesses to old MDM clock force register accesses
 * @{
 ****************************************************************************************
 */
#define crm_radartimclkforce_setf   mdm_radartimclkforce_setf
#define crm_rcclkforce_setf         mdm_rcclkforce_setf
#define crm_feclkforce_setf         mdm_feclkforce_setf
/** @} */
#elif NX_PLF_VER >= 30
#include "reg_crm.h"
#endif

/**
 ****************************************************************************************
 * @brief Initial clock settings (e.g. MPIF and MAC core clock value depending on the
 * PHY capabilities)
 ****************************************************************************************
 */
void crm_init(void);

/**
 ****************************************************************************************
 * @brief Return the frequency of the MAC core clock in case it would have been adjusted
 * depending on the PHY properties.
 *
 * @return The MAC core clock frequency in MHz
 ****************************************************************************************
 */
uint8_t crm_get_mac_freq(void);

/**
 ****************************************************************************************
 * @brief Return the frequency of the CPU clock
 *
 * @return The CPU clock frequency in Hz
 ****************************************************************************************
 */
uint32_t crm_get_cpu_freq(void);

/**
 ****************************************************************************************
 * @brief Indicate whether the RF front-end is configured at 160MHz or not.
 *
 * @return true if configured at 160MHz, false otherwise
 ****************************************************************************************
 */
bool crm_fe_160m(void);

/**
 ****************************************************************************************
 * @brief Set the different clocks depending on the chosen channel bandwidth.
 *
 * @param[in] chan_type  Channel bandwidth
 ****************************************************************************************
 */
void crm_clk_set(uint8_t chan_type);

/**
 ****************************************************************************************
 * @brief Reset the MODEM and the AGC
 ****************************************************************************************
 */
void crm_mdm_reset(void);

/// @} CRM

#endif // _CRM_H_
