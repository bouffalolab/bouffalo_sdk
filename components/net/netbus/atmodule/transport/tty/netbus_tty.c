#include <stdio.h>
#include <stdint.h>
#include <utils_list.h>
#include <FreeRTOS.h>
#include <task.h>

#include "bflb_clock.h"
#include "bflb_uart.h"
#include <bflb_gpio.h>
#include <bflb_core.h>
#include <bflb_dma.h>
#include "netbus_tty.h"

int netbus_tty_dnld(netbus_tty_ctx_t *pctx, uint8_t *data, uint32_t data_size)
{
    //return data_size;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    int res;

    // FIXME: only support AT
    if (xPortIsInsideInterrupt()) {
        res = xStreamBufferSendFromISR(pctx->rxbuf, data, data_size, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } else {
        res = xStreamBufferSend(pctx->rxbuf, data, data_size, 0);// fixme
    }
    return res;
}

int netbus_tty_init(netbus_tty_ctx_t *pctx, uint32_t txbuf_size, uint32_t rxbuf_size)
{
	// pctx->txbuf   = xStreamBufferCreate(txbuf_size, 1);
	pctx->rxbuf   = xStreamBufferCreate(rxbuf_size, 1);
	pctx->w_mutex = xSemaphoreCreateMutex();

	bflb_tty_dnld_register(netbus_tty_dnld, pctx);
	return 0;
}

int netbus_tty_send(netbus_tty_ctx_t *pctx, const uint8_t *p_data, uint32_t len, uint32_t timeout)
{
    //return len;
#if 0
	int ret = 0;
	int send_len = 0;
	uint32_t remain_len = len;
	struct bflb_device_s *pdev = bflb_device_get_by_name(pctx->config->name);

	if (xSemaphoreTake(pctx->w_mutex, timeout) != pdTRUE) {
		return -1;
	}
	while (remain_len) {
		ret = xStreamBufferSend(pctx->txbuf, p_data + send_len, remain_len, timeout);
		if (ret <= 0) {
			break;
		}
		send_len += ret;
		remain_len -= ret;
		bflb_uart_txint_mask(pdev, false);
	}
	xSemaphoreGive(pctx->w_mutex);
	return send_len;
#else
    if (xSemaphoreTake(pctx->w_mutex, timeout) != pdTRUE) {
		return -1;
	}
    bflb_tty_upld_send(p_data, len);
    xSemaphoreGive(pctx->w_mutex);
#endif
}

int netbus_tty_receive(netbus_tty_ctx_t *pctx, uint8_t *p_buffer, uint32_t buf_len, uint32_t timeout)
{
	return xStreamBufferReceive(pctx->rxbuf, p_buffer, buf_len, timeout);
}

int netbus_tty_deinit(netbus_tty_ctx_t *ctx)
{
	// vStreamBufferDelete(ctx->txbuf);
	vStreamBufferDelete(ctx->rxbuf);
	vSemaphoreDelete(ctx->w_mutex);
	return 0;
}
