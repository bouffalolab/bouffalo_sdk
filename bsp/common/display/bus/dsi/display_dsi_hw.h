/**
 * @file display_dsi_hw.h
 * @brief Hardware access of the DSI host backend: the only part that touches the clocks and the controller.
 *
 * The host logic (display_dsi.c) is written against these functions, so that it can be tested on the host
 * with a fake. The implementation for BL618DG is display_dsi_bl618dg.c.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_DSI_HW_H
#define DISPLAY_DSI_HW_H

#include "bus/dsi/display_dsi.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Largest payload of one LP packet: the command byte and its parameters. */
#define DISPLAY_DSI_LP_PAYLOAD_MAX 64u

/**
 * @brief Look up the controller (host->dsi). Nothing is started.
 *
 * @param [in,out] host DSI host
 * @retval 0 success
 * @retval -ENODEV controller not found
 */
int display_dsi_hw_init(display_dsi_host_t *host);

/**
 * @brief Bring the link up: DSI PLL, escape and display clocks, controller and D-PHY, for a validated video link.
 *
 * @param [in] host  DSI host
 * @param [in] video video link, DISPLAY_VLINK_DSI
 * @param [in] vc    virtual channel of the video, the one of the command endpoint
 * @retval 0 success
 * @retval -ENOTSUP the clock or lane rate cannot be produced
 */
int display_dsi_hw_open(display_dsi_host_t *host, const display_video_mode_t *video, uint8_t vc);

/**
 * @brief Take the link down: controller and PLL off.
 *
 * @param [in] host DSI host
 */
void display_dsi_hw_close(display_dsi_host_t *host);

/**
 * @brief Send one LP packet.
 *
 * @param [in] host       DSI host
 * @param [in] type       DSI data type of the packet
 * @param [in] is_long    long packet, otherwise short (payload of 1 or 2 bytes)
 * @param [in] vc         virtual channel
 * @param [in] payload    payload: the command byte and its parameters
 * @param [in] len        payload length
 * @retval 0 success
 * @retval <0 the controller reported an error
 */
int display_dsi_hw_send(display_dsi_host_t *host, uint8_t type, bool is_long, uint8_t vc, const uint8_t *payload,
                        uint16_t len);

/**
 * @brief Set the line buffer for the video link and switch the D-PHY to HS.
 *
 * @param [in] host  DSI host
 * @param [in] video video link the controller was opened with
 */
void display_dsi_hw_hs_start(display_dsi_host_t *host, const display_video_mode_t *video);

/**
 * @brief Switch the D-PHY back to LP.
 *
 * @param [in] host DSI host
 * @retval 0 success
 * @retval <0 the controller did not leave HS
 */
int display_dsi_hw_hs_stop(display_dsi_host_t *host);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_DSI_HW_H */
