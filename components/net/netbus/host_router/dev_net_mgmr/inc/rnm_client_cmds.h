#pragma once

#include <rnm_client.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Reset server. Stop STA, AP, Sniffer.
 *
 * @param rnm  RNM client.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_reset(rnmc_t *rnm);

/**
 * @brief  Instruct server to connect to AP.
 *
 * @detail  If connected or disconnected, client should receive corresponding indication.
 *
 * @param rnm       RNM client.
 * @param ssid      AP SSID.
 * @param password  AP password.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_sta_connect(rnmc_t *rnm, const uint8_t *ssid, uint16_t ssid_len, const uint8_t *password);

/**
 * @brief  Instruct server to disconnect from AP.
 *
 * @detail  If disconnected, client should receive indication.
 *
 * @param rnm  RNM client.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_sta_disconnect(rnmc_t *rnm);

/**
 * @brief  Get MAC address of server Wi-Fi.
 *
 * @param rnm  RNM client.
 * @param mac  MAC address to return.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_get_mac_addr(rnmc_t *rnm, uint8_t *mac);

/**
 * @brief  Instruct server to scan.
 *
 * @param rnm          RNM client.
 * @param scan_result  Scan results to return.
 *
 * @note  The returned scan result should be freed by calling rnm_free().
 *
 * @return
 *     - RNMC_OK              Scan succeeds and result is returned.
 *     - STATUS_INVALID_MODE  Wi-Fi is not in correct state. Wi-Fi must be in
 *                            idle state to scan. That is to say, STA or AP
 *                            should be disabled.
 *     - other                See rnmc_ret_t.
 */
rnmc_ret_t rnmc_scan(rnmc_t *rnm, rnm_scan_ind_msg_t **scan_result);

/**
 * @brief  Get STA status.
 *
 * @param rnm     RNM client.
 * @param status  STA status to return.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_sta_get_link_status(rnmc_t *rnm, struct bf1b_wifi_ap_record *status);

/**
 * @brief  Instruct server to start AP.
 *
 * @detail  If AP is started, client should receive indication.
 *
 * @param rnm       RNM client.
 * @param channel   Channel of AP(1-14).
 * @param ssid_len  Length of SSID.
 * @param ssid      SSID.
 * @param password  Optional password. Pass NULL for AP without password, or non-NULL for WPA2.
 *                  The password should be 8 to 63 ASCII chars.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_ap_start(rnmc_t *rnm, uint16_t channel, uint16_t ssid_len, const uint8_t *ssid, const char *password);

/**
 * @brief  Instruct server to stop AP.
 *
 * @detail  If AP is stopped, client should receive indication.
 * @param rnm  RNM client.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_ap_stop(rnmc_t *rnm);

/**
 * @brief  Get STA list of AP.
 *
 * @param rnm     RNM client.
 * @param result  STA list to return.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_ap_get_sta_list(rnmc_t *rnm, rnm_ap_sta_list_ind_msg_t **result);

/**
 * @brief  Instruct server to enter sniffer mode.
 *
 * @param rnm  RNM client.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_sniffer_start(rnmc_t *rnm);

/**
 * @brief  Instruct server to stop sniffer mode.
 *
 * @param rnm  RNM client.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_sniffer_stop(rnmc_t *rnm);

/**
 * @brief  Set channel for sniffer mode.
 *
 * @param rnm      RNM client.
 * @param channel  Channel to set(1-14).
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_sniffer_set_channel(rnmc_t *rnm, uint16_t channel);

/**
 * @brief  Get channel of sniffer mode.
 *
 * @param rnm      RNM client.
 * @param channel  Channel to return.
 *
 * @return  See rnmc_ret_t.
 */
rnmc_ret_t rnmc_sniffer_get_channel(rnmc_t *rnm, uint16_t *channel);

#ifdef __cplusplus
}
#endif
