#ifndef __OT_RADIO_TRX_EXT_
#define __OT_RADIO_TRX_EXT_

#include <utils_list.h>
#include <openthread/platform/radio.h>
#include <lmac154_frame.h>

#define IE_THREAD_HDR_SIZE 4
#define IE_THREAD_VENDOR_ID VENDOR_IE_THREAD_OUI
#define IE_THREAD_ENH_ACK_PROBING_IE_ID VENDOR_IE_THREAD_ENH_ACK_PROBING

#define MAX_SIZE_PSDU                       128 
#define MAX_BUFFER_SIZE_ACK                 64
#define MAX_BUFFER_SIZE_A_DATA              60
#define MAX_BUFFER_SIZE_M_DATA              100

#define FRAME_OVERHEAD_SIZE                 ((sizeof(otRadio_rxFrame_t) + 3) & 0xfffffffc)
#define MAC_FRAME_SIZE                      (FRAME_OVERHEAD_SIZE + MAX_SIZE_PSDU)
#define MAX_FRAME_SIZE_ACK                  (FRAME_OVERHEAD_SIZE + MAX_BUFFER_SIZE_ACK)

#define VENDOR_IE_HEADER_ID                 0
#define VENDOR_IE_THREAD_OUI                0xeab89b
#define VENDOR_IE_THREAD_ENH_ACK_PROBING    0
#define VENDOR_IE_NEST_OUI                  0x18b430
#define VENDOR_IE_NEST_TIME                 1

#ifndef OT_RADIO_CSL_DEBUG
#define OT_RADIO_CSL_DEBUG                  0
#endif

typedef struct _otRadio_rxFrame_t {
    utils_dlist_t       dlist;
    otRadioFrame        frame;
} otRadio_rxFrame_t;

typedef struct {
    utils_dlist_t                   dlist;
    uint8_t                         ie_buf[11];
    int8_t                          noise_floor;
} ot_link_metric_t;

typedef struct ot_radio {
    otInstance *                    instance;

    struct otRadioFrame *           tx_frame;
    struct otRadioFrame *           rx_ack_frame;
    utils_dlist_t                   rx_frame_list;
    utils_dlist_t                   frame_list;

    union {
        uint32_t                    value;
        uint8_t                     bytes[4];
    }                               frame_cnt;
    struct otMacKeyMaterial         keys[4]; // one more for fast access
    uint8_t                         mac_key_id;
    uint8_t                         reserved[3];

    otRadioIeInfo                   tx_ie_info;
    ot_link_metric_t                link_metrics;

    lmac154_security_t              enh_ack_sec;
    lmac154_txParam_ext_t           tx_param;
} ot_radio_t;

extern ot_radio_t                   ot_radio_ctx;

/****************************************************************************//**
 * @brief  initialize link metrics
 *
 * @param  noisefloor, noise flow
 *
 * @return None
 *
*******************************************************************************/
void ot_link_metrics_init(int8_t noise_floor);

/****************************************************************************//**
 * @brief  generate link metric IE data for enhanced ack
 *
 * @param  rx_info, received packet information
 *
 * @param  rssi, received signal strength indicator
 *
 * @param  lqi, link quality indicator
 *
 * @param  out_data, output buffer for IE data
 *
 * @return length of generated IE data, 0 if no match
 *
*******************************************************************************/
uint32_t ot_link_metrics_gen_enh_ack_data(lmac154_receiveInfo_t * rx_info,
                                          int8_t rssi, uint8_t lqi, uint8_t *out_data);

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

void ot_radioTxDoneCallback (lmac154_tx_status_t tx_status, uint32_t * ack_frame, uint32_t ack_frame_len);
void ot_radioRxDoneCallback(lmac154_rx_status_t status, lmac154_receiveInfo_t *recvInfo, uint32_t *frame);

#endif
