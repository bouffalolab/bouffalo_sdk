#ifndef __RWNX_UTILS_H__
#define __RWNX_UTILS_H__
#include "bl_defs.h"
#ifdef CONFIG_RWNX_DBG

/**
 ****************************************************************************************
 *
 * @file bl_utils.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#define RWNX_DBG os_printf
#else
#define RWNX_DBG(a...) do {} while (0)
#endif

#define RWNX_FN_ENTRY_STR ">>> %s()\r\n", __func__
#define RWNX_FN_LEAVE_STR "<<< %s()\r\n", __func__

#define RWNX_RXBUFF_PATTERN     (0xCAFEFADE)
#define RWNX_RXBUFF_VALID_IDX(idx) ((idx) < RWNX_RXBUFF_MAX)
/* Used to ensure that hostid set to fw is never 0 */
#define RWNX_RXBUFF_IDX_TO_HOSTID(idx) ((idx) + 1)
#define RWNX_RXBUFF_HOSTID_TO_IDX(hostid) ((hostid) - 1)
#define RWNX_RXBUFF_DMA_ADDR_GET(skbuff)                \
    skbuff->payload

/*
 * Structure used to store information regarding E2A msg buffers in the driver
 */
struct bl_e2amsg_elem {
    struct ipc_e2a_msg *msgbuf_ptr;
    u32 dma_addr;//XXX only for 32-bit addr
};

/*
 * Structure used to store information regarding Debug msg buffers in the driver
 */
struct bl_dbg_elem {
    struct ipc_dbg_msg *dbgbuf_ptr;
    u32 dma_addr;
};

/*
 * Structure used to store information regarding E2A radar events in the driver
 */
struct bl_e2aradar_elem {
    struct radar_pulse_array_desc *radarbuf_ptr;
    u32 dma_addr;
};

int bl_ipc_init(struct bl_hw *bl_hw, struct ipc_shared_env_tag *ipc_shared_mem);
uint32_t* bl_utils_pbuf_alloc(void);
void bl_utils_pbuf_free(uint32_t *p);
int bl_utils_idx_lookup(struct bl_hw *bl_hw, uint8_t *mac);
#endif
