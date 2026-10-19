#ifndef _BFLB_IPC_H_
#define _BFLB_IPC_H_

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup IPC
  * @{
  */

#define IPC_BITS_MAX   (32)
#define IPC_BITS_ALL   (0xffffffff)
#define IPC_BIT_NUM(n) ((0x01 << n) & IPC_BITS_ALL)

#ifdef __cplusplus
extern "C" {
#endif

void bflb_ipc_init(struct bflb_device_s *dev);
void bflb_ipc_deinit(struct bflb_device_s *dev);

void bflb_ipc_int_mask(struct bflb_device_s *dev, uint32_t ipc_bits);
void bflb_ipc_int_unmask(struct bflb_device_s *dev, uint32_t ipc_bits);

void bflb_ipc_trig(struct bflb_device_s *dev, uint32_t ipc_bits);
void bflb_ipc_clear(struct bflb_device_s *dev, uint32_t ipc_bits);

uint32_t bflb_ipc_get_sta(struct bflb_device_s *dev);
uint32_t bflb_ipc_get_intsta(struct bflb_device_s *dev);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif