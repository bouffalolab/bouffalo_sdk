#ifndef __OT_UTILS_EXT_
#define __OT_UTILS_EXT_

#include <ot_frame.h>

#define VENDOR_IE_HEADER_ID                 0
#define VENDOR_IE_THREAD_OUI                0xeab89b
#define VENDOR_IE_THREAD_ENH_ACK_PROBING    0
#define VENDOR_IE_NEST_OUI                  0x18b430
#define VENDOR_IE_NEST_TIME                 1

#define OT_PDS_10_SYMBOL_US             (LMAC154_US_PER_SYMBOL * 10)
#define OT_PDS_CSL_STATE_IDLE           0
#define OT_PDS_CSL_STATE_SAMPLE         1
#define OT_PDS_CSL_STATE_SLEEP          2

#define LMAC154_ALAMR_PDS_TIMER_ID          (0)

/****************************************************************************//**
 * @brief  Initalize utilize module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_utils_init(void);

/****************************************************************************//**
 * @brief  Get the version number
 *
 * @param  None
 *
 * @return The library version number
 *
*******************************************************************************/
uint32_t ot_utils_getVersionNumber(void);

/****************************************************************************//**
 * @brief  Get the version number string
 *
 * @param  None
 *
 * @return The library version number string
 *
*******************************************************************************/
char * ot_utils_getVersionString(void);

/****************************************************************************//**
 * @brief  initialize link metrics
 *
 * @param  noisefloor, noise flow
 *
 * @return None
 *
*******************************************************************************/
void otLinkMetrics_init(int8_t noisefloor);

/****************************************************************************//**
 * @brief  configre link metric probing using enhanced ack method
 *
 * @param  aShortAddress, short address
 * 
 * @param  aExtAddress, extended address
 *
 * @param  aLinkMetrics, link metrics options
 * 
 * @return otError
 *
*******************************************************************************/
otError otLinkMetrics_configEnhAckProbing(otShortAddress      aShortAddress,
                                            const otExtAddress *aExtAddress,
                                            otLinkMetrics       aLinkMetrics);

/****************************************************************************//**
 * @brief  generate link metric IE data for enhanced ack
 *
 * @param  addrType, address type
 * 
 * @param  aExtAddress, extended address
 *
 * @param  aLinkMetrics, link metrics options
 * 
 * @return otError
 *
*******************************************************************************/
uint32_t otLinkMetrics_genEnhAckData(otInternel_addrType_t addrType, uint8_t *pAddr, 
    int8_t rssi, int8_t lqi, uint8_t * pData);

/****************************************************************************//**
 * @brief  Enable Radio
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_radioEnable(void);

/****************************************************************************//**
 * @brief  Enable/Disable RX Promiscuous Mode
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_setRxPromiscuousMode(bool isEnable, bool isEnhancedMode);

/****************************************************************************//**
 * @brief  frame send
 *
 * @param  aFrame, a frame to send
 *
 * @return None
 *
*******************************************************************************/
int ot_radioSend(otRadioFrame *aFrame);

#ifdef BL616
/****************************************************************************//**
 * @brief  Init WiFi/M154 co-exist module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
int ot_coexist_event_init(void);
#endif
#endif
