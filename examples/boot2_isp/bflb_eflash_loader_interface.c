/**
 * *****************************************************************************
 * @file bflb_eflash_loader_interface.c
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
#include "bflb_eflash_loader.h"
#include "partition.h"
#include "blsp_common.h"
#include "xz_config.h"
#include "blsp_port.h"
#include "bflb_port_boot2.h"

uint8_t *g_eflash_loader_readbuf[2];
volatile uint32_t g_rx_buf_index = 0;
volatile uint32_t g_rx_buf_len = 0;
uint32_t g_eflash_loader_cmd_ack_buf[16];

static eflash_loader_if_cfg_t eflash_loader_if_cfg;

eflash_loader_if_cfg_t *bflb_eflash_loader_if_set(eflash_loader_if_type_t type)
{
    switch (type) {
        case BFLB_EFLASH_LOADER_IF_UART:
            eflash_loader_if_cfg.if_type = (uint8_t)BFLB_EFLASH_LOADER_IF_UART;
            eflash_loader_if_cfg.if_type_onfail = (uint8_t)BFLB_EFLASH_LOADER_IF_JLINK;
            eflash_loader_if_cfg.disabled = 0;
            eflash_loader_if_cfg.maxlen = BFLB_EFLASH_LOADER_READBUF_SIZE;
            eflash_loader_if_cfg.timeout = BFLB_EFLASH_LOADER_IF_UART_RX_TIMEOUT;
            eflash_loader_if_cfg.boot_if_init = bflb_eflash_loader_uart_init;
            eflash_loader_if_cfg.boot_if_handshake_poll = bflb_eflash_loader_uart_handshake_poll;
            eflash_loader_if_cfg.boot_if_recv = bflb_eflash_loader_uart_recv;
            eflash_loader_if_cfg.boot_if_send = bflb_eflash_loader_uart_send;
            eflash_loader_if_cfg.boot_if_wait_tx_idle = bflb_eflash_loader_usart_wait_tx_idle;
            eflash_loader_if_cfg.boot_if_deinit = bflb_eflash_loader_uart_deinit;
            //eflash_loader_if_cfg.boot_if_changerate=bflb_eflash_loader_uart_change_rate;

            return &eflash_loader_if_cfg;
#if BLSP_BOOT2_SUPPORT_USB_IAP
        case BFLB_EFLASH_LOADER_IF_USB:
            eflash_loader_if_cfg.if_type = (uint8_t)BFLB_EFLASH_LOADER_IF_USB;
            eflash_loader_if_cfg.if_type_onfail = (uint8_t)BFLB_EFLASH_LOADER_IF_UART;
            eflash_loader_if_cfg.disabled = 0;
            eflash_loader_if_cfg.maxlen = BFLB_EFLASH_LOADER_READBUF_SIZE;
            eflash_loader_if_cfg.timeout = BFLB_EFLASH_LOADER_IF_USB_RX_TIMEOUT;
            eflash_loader_if_cfg.boot_if_init = bflb_eflash_loader_usb_init;
            eflash_loader_if_cfg.boot_if_handshake_poll = bflb_eflash_loader_usb_handshake_poll;
            eflash_loader_if_cfg.boot_if_recv = bflb_eflash_loader_uart_recv;
            eflash_loader_if_cfg.boot_if_send = bflb_eflash_loader_usb_send;
            eflash_loader_if_cfg.boot_if_wait_tx_idle = bflb_eflash_loader_usb_wait_tx_idle;
            eflash_loader_if_cfg.boot_if_deinit = bflb_eflash_loader_usb_deinit;
            //eflash_loader_if_cfg.boot_if_changerate=bflb_eflash_loader_uart_change_rate;

            return &eflash_loader_if_cfg;
#endif
        default:
            break;
    }

    return NULL;
}

int32_t bflb_eflash_loader_if_init()
{
    return eflash_loader_if_cfg.boot_if_init();
}

int32_t bflb_eflash_loader_if_handshake_poll(uint32_t timeout)
{
    return eflash_loader_if_cfg.boot_if_handshake_poll(timeout);
}

ATTR_TCM_SECTION uint32_t *bflb_eflash_loader_if_read(uint32_t *read_len)
{
    return eflash_loader_if_cfg.boot_if_recv(read_len, eflash_loader_if_cfg.maxlen, eflash_loader_if_cfg.timeout);
}

ATTR_TCM_SECTION int32_t bflb_eflash_loader_if_write(uint32_t *data, uint32_t len)
{
    return eflash_loader_if_cfg.boot_if_send(data, len);
}

int32_t bflb_eflash_loader_if_wait_tx_idle(uint32_t timeout)
{
    return eflash_loader_if_cfg.boot_if_wait_tx_idle(timeout);
}

int32_t bflb_eflash_loader_if_deinit()
{
    return eflash_loader_if_cfg.boot_if_deinit();
}

int32_t ATTR_TCM_SECTION bflb_eflash_loader_main()
{
#if defined(CHIP_BL602) || defined(CHIP_BL702)
    int32_t ret;
    uint32_t total_len;
    uint32_t i, tmp, cmd_len;
    uint8_t *recv_buf = NULL;
    uint8_t err_cnt = 0;
    uint8_t to_cnt = 0;

    //LOG_F("bflb_eflash_loader_main\r\n");
    //pt_table_dump();
    //ret = pt_table_get_iap_para(&p_iap_param);
    //if(0 != ret){
    //LOG_F("no valid partition table\r\n");
    //return -1;
    //}

    while (1) {
        to_cnt = 0;
        total_len = 0;

        do {
            total_len = 0;
            recv_buf = (uint8_t *)bflb_eflash_loader_if_read(&total_len);

            if (total_len <= 0) {
                to_cnt++;
            }
        } while (to_cnt < 2 && total_len <= 0);

        if (to_cnt >= 2 || total_len <= 0) {
            //LOG_F("rcv err break\r\n");
            break;
        }

        //LOG_F("Recv\r\n");
        //eflash_loader_dump_data(recv_buf,total_len);
        cmd_len = recv_buf[2] + (recv_buf[3] << 8);
        //LOG_F("cmd_len %d\r\n", cmd_len);

        /* Check checksum*/
        if (recv_buf[1] != 0) {
            tmp = 0;

            for (i = 2; i < cmd_len + 4; i++) {
                tmp += recv_buf[i];
            }

            if ((tmp & 0xff) != recv_buf[1]) {
                /* FL+Error code(2bytes) */
                //LOG_F("Checksum error %02x\r\n", tmp & 0xff);
                g_eflash_loader_cmd_ack_buf[0] = BFLB_EFLASH_LOADER_CMD_NACK | ((BFLB_EFLASH_LOADER_CMD_CRC_ERROR << 16) & 0xffff0000);
                bflb_eflash_loader_if_write(g_eflash_loader_cmd_ack_buf, 4);
                continue;
            }
        }
        if (recv_buf[0] == BFLB_EFLASH_LOADER_CMD_ISP_MODE) {
            eflash_loader_if_cfg.timeout = 8000;
        }
        ret = bflb_eflash_loader_cmd_process(recv_buf[0], recv_buf + 4, cmd_len);

        if (ret != BFLB_EFLASH_LOADER_SUCCESS) {
            //LOG_F(" CMD Pro Ret %d\r\n", ret);

            err_cnt++;

            if (err_cnt > 2) {
                break;
            }
        }
    }

    /* read data finished,deinit and go on*/
    bflb_eflash_loader_if_deinit();
#endif
    return 0;
}
