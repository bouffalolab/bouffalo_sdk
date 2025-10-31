/**
 * *****************************************************************************
 * @file bflb_eflash_loader_interface.h
 * @version 0.1
 * @date 2022-11-23
 * @brief
 * *****************************************************************************
 * @attention
 *
 *  <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *    3. Neither the name of Bouffalo Lab nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * *****************************************************************************
 */
#ifndef __BFLB_EFLASH_LOADER_INTERFACE_H__
#define __BFLB_EFLASH_LOADER_INTERFACE_H__

#include "bflb_eflash_loader.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"

#define BFLB_EFLASH_LOADER_IF_TX_IDLE_TIMEOUT    4 /*ms*/
#if defined(CHIP_BL702L)
#define BFLB_EFLASH_LOADER_HAND_SHAKE_RCV_COUNT  16
#else
#define BFLB_EFLASH_LOADER_HAND_SHAKE_RCV_COUNT  32
#endif
#define BFLB_EFLASH_LOADER_HAND_SHAKE_BYTE       0x55
#if defined(CHIP_BL702L)
#define BFLB_EFLASH_LAODER_HAND_SHAKE_SUSS_COUNT 8
#else
#define BFLB_EFLASH_LAODER_HAND_SHAKE_SUSS_COUNT 16
#endif

typedef enum tag_eflash_loader_if_type_t {
    //BFLB_EFLASH_LOADER_IF_FLASH=0x01,
    BFLB_EFLASH_LOADER_IF_UART = 0x0,
    BFLB_EFLASH_LOADER_IF_JLINK,
    BFLB_EFLASH_LOADER_IF_SDIO,
    BFLB_EFLASH_LOADER_IF_USB,
    BFLB_EFLASH_LOADER_IF_ALL,
} eflash_loader_if_type_t;

typedef int32_t (*boot_if_init_p)(void);
typedef int32_t (*boot_if_handshake_poll_p)(uint32_t timeout);
typedef uint32_t *(*boot_if_read_p)(uint32_t *len, uint32_t maxlen, uint32_t timeout);
typedef int32_t (*boot_if_write_p)(uint32_t *data, uint32_t len);
typedef int32_t (*boot_if_wait_tx_idle_p)(uint32_t timeout);
typedef int32_t (*boot_if_deinit_p)(void);
typedef int32_t (*boot_if_change_rate_p)(uint32_t oldval, uint32_t newval);

typedef struct tag_eflash_loader_if_cfg_t {
    uint8_t if_type;        //interface type
    uint8_t if_type_onfail; //if fail move to this interface
    uint8_t disabled;       //enable this if
    uint8_t rsvd;

    //pentry_t         boot_entry[2];

    uint16_t maxlen;  //max len for one buffer
    uint16_t timeout; //ms

    boot_if_init_p boot_if_init;                     //init function pointer
    boot_if_handshake_poll_p boot_if_handshake_poll; //shake hand poll function pointer
    boot_if_read_p boot_if_recv;                     //read function pointer
    boot_if_write_p boot_if_send;                    //write function pointer
    boot_if_wait_tx_idle_p boot_if_wait_tx_idle;     //wait tx idle function pointer
    boot_if_deinit_p boot_if_deinit;                 //deinit function pointer
    boot_if_change_rate_p boot_if_changerate;        //change rate function pointer

} eflash_loader_if_cfg_t;

eflash_loader_if_cfg_t *bflb_eflash_loader_if_set(eflash_loader_if_type_t type);
int32_t bflb_eflash_loader_if_init();
int32_t bflb_eflash_loader_if_handshake_poll(uint32_t timeout);
uint32_t *bflb_eflash_loader_if_read(uint32_t *read_len);
int32_t bflb_eflash_loader_if_write(uint32_t *data, uint32_t len);
int32_t bflb_eflash_loader_if_wait_tx_idle(uint32_t timeout);
int32_t bflb_eflash_loader_if_deinit();
int32_t bflb_eflash_loader_main(void);

extern uint8_t *g_eflash_loader_readbuf[2];
/*read data buffer from flash or boot interface*/
extern volatile uint32_t g_rx_buf_index;
extern volatile uint32_t g_rx_buf_len;
extern uint32_t g_eflash_loader_cmd_ack_buf[16];

#endif
