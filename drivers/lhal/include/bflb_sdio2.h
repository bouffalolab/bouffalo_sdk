/**
  ******************************************************************************
  * @file    bflb_sdio2.h
  * @version V1.0
  * @date    2022-09-27
  * @brief   This file is the low hardware abstraction layer file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2022 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#ifndef _BFLB_SDIO2_H
#define _BFLB_SDIO2_H

#include "bflb_core.h"

/* SDIO2 buffer size */
#define SDIO2_DEFAULT_SIZE_MAX        (1024 * 2)

/* No modification */
#define SDIO2_SIZE_CONSULT_MULTIPLE   (256)
#define SDIO2_BYTE_MOD_SIZE_MAX       (512)
#define SDIO2_MAX_PORT_NUM            16

/* error callback resaon */
#define SDIO2_ERROR_CRC               (1)
#define SDIO2_ERROR_ABORT             (2)

/* card to host interrupt event */
#define SDIO2_HOST_INT_EVENT_DNLD_RDY (1 << 0)
#define SDIO2_HOST_INT_EVENT_UPLD_RDY (1 << 1)
#define SDIO2_HOST_INT_EVENT_CIS_RDY  (1 << 2)
#define SDIO2_HOST_INT_EVENT_IO_RDY   (1 << 3)

#if (SDIO2_MAX_PORT_NUM & (SDIO2_MAX_PORT_NUM - 1) != 0)
#error "sdio2 port num error, must be 2^n"
#else
#define SDIO2_MAX_PORT_MASK (SDIO2_MAX_PORT_NUM - 1)
#endif

/* trans desc */
typedef struct
{
    uint16_t buff_len;
    uint16_t data_len;
    void *buff;
    void *user_arg;
} bflb_sdio2_trans_desc_t;

/* trans callback */
typedef void (*bflb_sdio2_trans_irq_callback_t)(void *arg, bflb_sdio2_trans_desc_t *trans_desc);
/* error callback */
typedef void (*bflb_sdio2_error_irq_callback_t)(void *arg, uint32_t err_code);
#if defined(BL616)
/* reset callback */
typedef void (*bflb_sdio2_reset_irq_callback_t)(void *arg,
                                                bflb_sdio2_trans_desc_t *upld_rest_desc, uint32_t upld_rest_desc_num,
                                                bflb_sdio2_trans_desc_t *dnld_rest_desc, uint32_t dnld_rest_desc_num);
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* initialization */
int bflb_sdio2_init(struct bflb_device_s *dev, uint32_t dnld_size_max);
int bflb_sdio2_check_host_ready(struct bflb_device_s *dev);
int bflb_sdio2_get_block_size(struct bflb_device_s *dev);

/* get trans max size */
int bflb_sdio2_get_upld_max_size(struct bflb_device_s *dev);
int bflb_sdio2_get_dnld_max_size(struct bflb_device_s *dev);

/* attach dnld/upld buff */
int bflb_sdio2_dnld_port_push(struct bflb_device_s *dev, bflb_sdio2_trans_desc_t *trans_desc);
int bflb_sdio2_upld_port_push(struct bflb_device_s *dev, bflb_sdio2_trans_desc_t *trans_desc);

/* get dnld/upld info */
int bflb_sdio2_dnld_get_waiting(struct bflb_device_s *dev);
int bflb_sdio2_dnld_get_available(struct bflb_device_s *dev);
int bflb_sdio2_upld_get_waiting(struct bflb_device_s *dev);
int bflb_sdio2_upld_get_available(struct bflb_device_s *dev);

/* isr callback attach */
int bflb_sdio2_dnld_irq_attach(struct bflb_device_s *dev, bflb_sdio2_trans_irq_callback_t dnld_irq_callback, void *arg);
int bflb_sdio2_upld_irq_attach(struct bflb_device_s *dev, bflb_sdio2_trans_irq_callback_t upld_irq_callback, void *arg);
int bflb_sdio2_error_irq_attach(struct bflb_device_s *dev, bflb_sdio2_error_irq_callback_t error_irq_callback, void *arg);
#if defined(BL616)
int bflb_sdio2_reset_irq_attach(struct bflb_device_s *dev, bflb_sdio2_reset_irq_callback_t reset_irq_callback, void *arg);
#endif

/* trigger host interrupt event */
int bflb_sdio2_trig_host_int(struct bflb_device_s *dev, uint32_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BFLB_SDIO3_H */
