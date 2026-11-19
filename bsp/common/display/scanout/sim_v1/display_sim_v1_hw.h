/**
 * @file display_sim_v1_hw.h
 * @brief Hardware access of the SIM v1 scanout: pins, SIM engine, DMA nodes and channel.
 *
 * The scanout logic (display_sim_v1_scanout.c: ping-pong lists, flip, frame-end interrupt) is written against these
 * functions, so that it can be tested on the host with a fake. The implementation for BL616 is
 * display_sim_v1_hw.c.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_SIM_V1_HW_H
#define DISPLAY_SIM_V1_HW_H

#include "scanout/sim_v1/display_sim_v1_scanout.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Bytes of one DMA node (32-bit words, the transfer size field limits it to 4032 words). */
#define DISPLAY_SIM_V1_NODE_BYTES (4032u * 4u)

/**
 * @brief Look up the DMA channel (sim->dma).
 *
 * @param [in,out] sim SIM v1 scanout
 * @retval 0 success
 * @retval -ENODEV channel not found
 */
int display_sim_v1_hw_init(display_sim_v1_scanout_t *sim);

/**
 * @brief Mux the pins, set up the SIM engine for the timing and the frame rate, and set up the DMA channel.
 *        The head and tail nodes (the non-active lines) are built from the control words of the engine.
 *
 * @param [in,out] sim        SIM v1 scanout
 * @param [in]     mode       video timing
 * @param [in]     frame_rate frames per second for the engine
 * @retval 0 success
 * @retval -ENOTSUP the engine cannot produce this timing
 */
int display_sim_v1_hw_open(display_sim_v1_scanout_t *sim, const display_video_mode_t *mode, uint16_t frame_rate);

/**
 * @brief Build the nodes of one list for a frame: the chain of data nodes ending at the tail node.
 *
 * @param [in,out] sim   SIM v1 scanout
 * @param [in]     nodes first node of the list
 * @param [in]     count number of nodes of the list
 * @param [in]     frame frame the list reads; the cache of the nodes is cleaned
 * @param [in]     words size of the frame in 32-bit words
 */
void display_sim_v1_hw_build_list(display_sim_v1_scanout_t *sim, struct bflb_dma_channel_lli_pool_s *nodes,
                                  uint16_t count, const void *frame, uint32_t words);

/**
 * @brief Point the head node at a list, and (re)start the channel with it: stop, load the head node into the
 *        channel registers, start.
 *
 * @param [in,out] sim   SIM v1 scanout
 * @param [in]     nodes first node of the list to send after the head node
 */
void display_sim_v1_hw_run(display_sim_v1_scanout_t *sim, struct bflb_dma_channel_lli_pool_s *nodes);

/**
 * @brief Stop the channel and release the pins and the interrupt.
 *
 * @param [in,out] sim SIM v1 scanout
 */
void display_sim_v1_hw_close(display_sim_v1_scanout_t *sim);

/**
 * @brief Frame-end interrupt of the channel; implemented by the scanout logic, called by the hardware layer.
 *
 * @param [in] sim SIM v1 scanout
 */
void display_sim_v1_frame_end(display_sim_v1_scanout_t *sim);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_SIM_V1_HW_H */
