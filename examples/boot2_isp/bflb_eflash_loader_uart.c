/**
  ******************************************************************************
  * @file    blsp_eflash_loader_uart.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
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
#include "bflb_eflash_loader.h"
#include "blsp_port.h"
#include "blsp_common.h"
#include "partition.h"
#include "bflb_uart.h"
#include "bflb_port_boot2.h"

static uint32_t g_detected_baudrate;
struct bflb_device_s *uartx;
static void bflb_eflash_loader_usart_if_deinit();
#define UART_FIFO_LEN 32
void bflb_dump_data(uint8_t *buf, uint32_t size)
{
    for (int i = 0; i < size; i++) {
        if (i % 16 == 0)
            LOG_F("\r\n");
        LOG_F("%02x ", buf[i]);
    }
    LOG_F("\r\n");
}
void ATTR_TCM_SECTION uart0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
}

static void bflb_eflash_loader_usart_if_init(uint32_t bdrate)
{
    struct bflb_uart_config_s cfg;
    hal_boot2_uart_gpio_init();
    uartx = bflb_device_get_by_name("uart0");

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    bflb_uart_init(uartx, &cfg);
}

void bflb_eflash_loader_usart_if_enable_int(void)
{
    bflb_uart_rxint_mask(uartx,false);
}

void bflb_eflash_loader_usart_if_send(uint8_t *data, uint32_t len)
{
    uartx = bflb_device_get_by_name("uart0");
    bflb_uart_put(uartx, data, len);
}

int32_t bflb_eflash_loader_usart_if_wait_tx_idle(uint32_t timeout)
{
    /*UART now can't judge tx idle now*/
    bflb_mtimer_delay_ms(timeout);

    return 0;
}

static uint32_t *bflb_eflash_loader_usart_if_receive(uint32_t *recv_len, uint16_t maxlen, uint16_t timeout)
{
    return NULL;
}

static void bflb_eflash_loader_usart_if_deinit()
{
    uartx = bflb_device_get_by_name("uart0");
    bflb_uart_deinit(uartx);
}

int32_t bflb_eflash_loader_uart_init()
{
    bflb_eflash_loader_usart_if_deinit();
    bflb_eflash_loader_usart_if_init(0);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_eflash_loader_uart_handshake_poll(uint32_t timeout)
{
    uint8_t buf[UART_FIFO_LEN] = { 0 };
    uint32_t i;
    uint32_t handshake_count = 0;
    uint32_t rcv_buf_len = 0;
    //rcv_buf_len = UART_ReceiveData(g_uart_if_id,buf,128);
    uartx = bflb_device_get_by_name("uart0");
    rcv_buf_len = bflb_uart_get(uartx, buf, UART_FIFO_LEN);

    //while(1)
    {
        if (rcv_buf_len >= BFLB_EFLASH_LOADER_HAND_SHAKE_RCV_COUNT) {
            for (i = 0; i < BFLB_EFLASH_LOADER_HAND_SHAKE_RCV_COUNT; i++) {
                if (buf[i] == BFLB_EFLASH_LOADER_HAND_SHAKE_BYTE) {
                    handshake_count++;

                    if (handshake_count > BFLB_EFLASH_LAODER_HAND_SHAKE_SUSS_COUNT) {
                        break;
                    }
                }
            }
        }
    }

    if (handshake_count < BFLB_EFLASH_LAODER_HAND_SHAKE_SUSS_COUNT) {
        return -1;
    }

#if defined(CHIP_BL606P) || defined(CHIP_BL808) || defined(CHIP_BL616) || defined(CHIP_WB03)
    /*receive shake hanad signal*/
    bflb_eflash_loader_usart_if_send((uint8_t *)"Boot2 ISP Ready", strlen("Boot2 ISP Ready"));
    bflb_mtimer_delay_ms(2);
    hal_reboot_config(HAL_REBOOT_FROM_INTERFACE);
    GLB_SW_System_Reset();
    while (1)
        ;
#endif

#if defined(CHIP_BL602) || defined(CHIP_BL702)
#define BFLB_EFLASH_LAODER_COMSUME_55_TIMEOUT 20

    uint64_t nowtime;
    /*receive shake hanad signal*/
    bflb_eflash_loader_usart_if_send((uint8_t *)"Boot2 ISP Shakehand Suss", strlen("Boot2 ISP Shakehand Suss"));

    /* consume the remaining bytes when shake hand(0x55) if needed */
    nowtime = bflb_mtimer_get_time_ms();
    do {
        
        rcv_buf_len = bflb_uart_get(uartx,buf,UART_FIFO_LEN);
        if (rcv_buf_len > 0) {
            nowtime = bflb_mtimer_get_time_ms();
        }

    } while (bflb_mtimer_get_time_ms() - nowtime < BFLB_EFLASH_LAODER_COMSUME_55_TIMEOUT);
    bflb_eflash_loader_usart_if_send((uint8_t *)"Boot2 ISP Ready", strlen("Boot2 ISP Ready"));

    /*init rx info */
    g_rx_buf_index = 0;
    g_rx_buf_len = 0;


    simple_malloc_init(g_malloc_buf, sizeof(g_malloc_buf));
    g_eflash_loader_readbuf[0] = vmalloc(BFLB_EFLASH_LOADER_READBUF_SIZE);
    g_eflash_loader_readbuf[1] = vmalloc(BFLB_EFLASH_LOADER_READBUF_SIZE);
    arch_memset(g_eflash_loader_readbuf[0], 0, BFLB_EFLASH_LOADER_READBUF_SIZE);
    arch_memset(g_eflash_loader_readbuf[1], 0, BFLB_EFLASH_LOADER_READBUF_SIZE);

    bflb_eflash_loader_usart_if_enable_int();

#endif

    return 0;
}

uint32_t *bflb_eflash_loader_uart_recv(uint32_t *recv_len, uint32_t maxlen, uint32_t timeout)
{
    return bflb_eflash_loader_usart_if_receive(recv_len, maxlen, timeout);
}

int32_t bflb_eflash_loader_uart_send(uint32_t *data, uint32_t len)
{
    bflb_eflash_loader_usart_if_send((uint8_t *)data, len);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_eflash_loader_usart_wait_tx_idle(uint32_t timeout)
{
    return bflb_eflash_loader_usart_if_wait_tx_idle(timeout);
}

int32_t bflb_eflash_loader_uart_change_rate(uint32_t oldval, uint32_t newval)
{
    uint32_t b = (uint32_t)((g_detected_baudrate * 1.0 * newval) / oldval);

    //LOG_F("BDR:");
    //LOG_F(oldval);
    //LOG_F(newval);
    //LOG_F(g_detected_baudrate);
    //LOG_F(b);

    bflb_eflash_loader_usart_if_wait_tx_idle(BFLB_EFLASH_LOADER_IF_TX_IDLE_TIMEOUT);

    bflb_eflash_loader_usart_if_init(b);
    bflb_eflash_loader_usart_if_send((uint8_t *)"OK", 2);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_eflash_loader_uart_deinit()
{
    /* delete uart deinit, when uart tx(gpio16) set input function, uart send 0xFF to uart tx fifo
    bflb_eflash_loader_deinit_uart_gpio(g_abr_gpio_sel);

    bflb_eflash_loader_usart_if_deinit();
    */

    return BFLB_EFLASH_LOADER_SUCCESS;
}
