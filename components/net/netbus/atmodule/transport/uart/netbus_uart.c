#include <stdio.h>
#include <stdint.h>
#include <utils_list.h>
#include <FreeRTOS.h>
#include <task.h>
#include <string.h>

#include "bflb_clock.h"
#include "bflb_uart.h"
#include <bflb_gpio.h>
#include <bflb_core.h>
#include <bflb_dma.h>
#include "netbus_uart.h"

#define TXSLOT_COUNT                2
#define TXSLOT_SIZE                 1024
#define RXSLOT_COUNT                2
#define RXSLOT_SIZE                 1024
#define UART_TASK_PRI               (55)
#define UART_STACK_SIZE             (1024)
//#define NETBUS_TX_BUFFER_SIZE (2*1024)
//#define NETBUS_RX_BUFFER_SIZE (2*1024)

static netbus_uart_ctx_t *g_netbus_uart_ctx;

ATTR_TCM_SECTION static inline void __uart_rx_cb(netbus_uart_ctx_t *pctx, struct bflb_device_s *pdev)
{
	int rxcnt = 0;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    while (bflb_uart_rxavailable(pdev)) {
        rxcnt = bflb_uart_get(pdev, pctx->buffer, sizeof(pctx->buffer));
	    xStreamBufferSendFromISR(pctx->rxbuf, pctx->buffer, rxcnt, &xHigherPriorityTaskWoken);
    }
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

ATTR_TCM_SECTION static inline void __uart_tx_cb(netbus_uart_ctx_t *pctx, struct bflb_device_s *pdev)
{
	uint8_t ch;
	int ret;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	while (bflb_uart_txready(pdev)) {
		ret = xStreamBufferReceiveFromISR(pctx->txbuf, &ch, 1, &xHigherPriorityTaskWoken);
	    if (ret > 0) {
	    	bflb_uart_putchar(pdev, ch);
	    } else {
	    	bflb_uart_txint_mask(pdev, true);
	    	break;
	    }
	}

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

ATTR_TCM_SECTION void uart_isr(int irq, void *arg)
{
	netbus_uart_ctx_t *pctx = (netbus_uart_ctx_t *)arg;
	struct bflb_device_s *pdev = bflb_device_get_by_name(pctx->config->name);
    uint32_t intstatus = bflb_uart_get_intstatus(pdev);
    if (intstatus & UART_INTSTS_RX_FIFO) {
        __uart_rx_cb(pctx, pdev);
    }
    if (intstatus & UART_INTSTS_RTO) {
        bflb_uart_int_clear(pdev, UART_INTCLR_RTO);
        __uart_rx_cb(pctx, pdev);
    }
    if (intstatus & UART_INTSTS_TX_FIFO) {
    	__uart_tx_cb(pctx, pdev);
    }
}

//static void netbus_uart_task(void *arg)
//{
//	netbus_uart_ctx_t *pctx = (netbus_uart_ctx_t *)arg;
//}

int netbus_uart_init(netbus_uart_ctx_t *pctx, const netbus_uart_config_t *hwcfg, uint32_t txbuf_size, uint32_t rxbuf_size)
{
    int i __attribute__((unused)), ret __attribute__((unused));

    struct bflb_device_s *pdev = bflb_device_get_by_name(hwcfg->name);
    struct bflb_uart_config_s cfg = {
        .baudrate = hwcfg->speed,
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .flow_ctrl = 0,
        .tx_fifo_threshold = 7,
        .rx_fifo_threshold = 7,
        .bit_order = UART_LSB_FIRST,
    };

    if (hwcfg->databits == 5)
        cfg.data_bits = UART_DATA_BITS_5;
    else if (hwcfg->databits == 6)
        cfg.data_bits = UART_DATA_BITS_6;
    else if (hwcfg->databits == 7)
        cfg.data_bits = UART_DATA_BITS_7;
    else if (hwcfg->databits == 8)
        cfg.data_bits = UART_DATA_BITS_8;
    else
        return -1;

    if (hwcfg->stopbits == 1)
        cfg.stop_bits = UART_STOP_BITS_1;
    else if (hwcfg->stopbits == 2)
        cfg.stop_bits = UART_STOP_BITS_1_5;
    else  if (hwcfg->stopbits == 3)
        cfg.stop_bits = UART_STOP_BITS_2;
    else
        return -1;

    if (hwcfg->parity == 0)
        cfg.parity = UART_PARITY_NONE;
    else if (hwcfg->parity == 1)
        cfg.parity = UART_PARITY_ODD;
    else  if (hwcfg->parity == 2)
        cfg.parity = UART_PARITY_EVEN;
    else  if (hwcfg->parity == 3)
        cfg.parity = UART_PARITY_MARK;
    else  if (hwcfg->parity == 4)
        cfg.parity = UART_PARITY_SPACE;
    else
        return -1;

    pctx->runtime_config = *hwcfg;
    pctx->config = &pctx->runtime_config;
	pctx->txbuf   = xStreamBufferCreate(txbuf_size, 1);
	pctx->rxbuf   = xStreamBufferCreate(rxbuf_size, 1);
	pctx->w_mutex = xSemaphoreCreateMutex();
    bflb_uart_init(pdev, &cfg);

    bflb_uart_txint_mask(pdev, true);
    bflb_uart_rxint_mask(pdev, false);
    bflb_irq_attach(pdev->irq_num, uart_isr, pctx);
    bflb_irq_enable(pdev->irq_num);
    g_netbus_uart_ctx = pctx;
//    ret = xTaskCreate(netbus_uart_task,
//                (char*)"netbus_uart",
//				UART_STACK_SIZE/sizeof(StackType_t), pctx,
//				UART_TASK_PRI,
//                &pctx->task);
//    if (ret != pdPASS) {
//    	printf("failed to create spisync task, %ld\r\n", ret);
//	}
	return 0;
}

static int netbus_uart_build_runtime_config( struct bflb_device_s *pdev, const netbus_uart_config_t *next, struct bflb_uart_config_s *cfg)
{
    uint64_t divider;

    if (!pdev || !next || !cfg ||
        next->speed < 80 || next->speed > 5000000 ||
        next->databits < 5 || next->databits > 8 ||
        next->stopbits < 1 || next->stopbits > 3 ||
        next->parity > 2 || next->flow_control != 0) {
        return -1;
    }

    divider = ((uint64_t)bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_UART, pdev->idx) * 10 / next->speed + 5) / 10;

    if (divider == 0 || divider >= 0xffff) {
        return -1;
    }

    memset(cfg, 0, sizeof(*cfg));
    cfg->baudrate = next->speed;
    cfg->data_bits = (uint8_t)(next->databits - 5);
    cfg->stop_bits = next->stopbits;
    cfg->parity = next->parity;
    cfg->flow_ctrl = UART_FLOWCTRL_NONE;
    cfg->tx_fifo_threshold = 7;
    cfg->rx_fifo_threshold = 7;
    cfg->bit_order = UART_LSB_FIRST;

    return 0;
}

int netbus_uart_config_validate(const netbus_uart_config_t *config)
{
    struct bflb_uart_config_s hw_config;

    if (!config || !config->name) {
        return -1;
    }

    return netbus_uart_build_runtime_config(bflb_device_get_by_name(config->name), config, &hw_config);
}

int netbus_uart_get_config(netbus_uart_ctx_t *pctx, netbus_uart_config_t *config)
{
    if (!pctx || !pctx->config || !pctx->w_mutex || !config ||
        xSemaphoreTake(pctx->w_mutex, pdMS_TO_TICKS(2000)) != pdTRUE) {
        return -1;
    }

    *config = *pctx->config;

    xSemaphoreGive(pctx->w_mutex);
    return 0;
}

int netbus_uart_reconfigure(netbus_uart_ctx_t *pctx, const netbus_uart_config_t *next, const uint8_t *reply, uint32_t reply_len)
{
    struct bflb_uart_config_s cfg;
    struct bflb_device_s *pdev;
    TickType_t started;
    int result = -1;

#if defined(NETBUS_UART_DMA) && NETBUS_UART_DMA
    /* DMA requires a separate transfer-stop and restart sequence. */
    return -1;
#endif

    if (!pctx || !pctx->config || !pctx->config->name ||
        !pctx->txbuf || !pctx->w_mutex ||
        !next || !next->name || (reply_len && !reply) ||
        strcmp(next->name, pctx->config->name) != 0) {
        return -1;
    }

    pdev = bflb_device_get_by_name(pctx->config->name);

    if (netbus_uart_build_runtime_config(pdev, next, &cfg) != 0) {
        return -1;
    }

    if (xSemaphoreTake(pctx->w_mutex, pdMS_TO_TICKS(2000)) != pdTRUE) {
        return -1;
    }

    /* Block other writers while the TX ISR drains the existing queue. */
    started = xTaskGetTickCount();

    while (xStreamBufferBytesAvailable(pctx->txbuf) != 0) {
        bflb_uart_txint_mask(pdev, false);

        if ((TickType_t)(xTaskGetTickCount() - started) >=
            pdMS_TO_TICKS(2000)) {
            goto done;
        }

        vTaskDelay(1);
    }

    if (bflb_uart_wait_tx_done(pdev) != 0) {
        goto done;
    }

    bflb_uart_txint_mask(pdev, true);

    if (reply_len) {
        result = -2;

        for (uint32_t i = 0; i < reply_len; i++) {
            if (bflb_uart_putchar(pdev, reply[i]) != 0) {
                goto done;
            }
        }

        if (bflb_uart_wait_tx_done(pdev) != 0) {
            goto done;
        }
    }

    /* The reply has left the wire using the old settings. */
    bflb_irq_disable(pdev->irq_num);

    bflb_uart_init(pdev, &cfg);

    pctx->runtime_config = *next;
    pctx->config = &pctx->runtime_config;

    bflb_uart_txint_mask(pdev, true);
    bflb_uart_rxint_mask(pdev, false);
    bflb_irq_enable(pdev->irq_num);

    result = 0;

done:
    xSemaphoreGive(pctx->w_mutex);
    return result;
}

int netbus_uart_recovery(void)
{
    netbus_uart_ctx_t *pctx = g_netbus_uart_ctx;
    struct bflb_device_s *pdev = NULL;
    if(pctx == NULL || pctx->config == NULL) {
        return -1;
    }

    const netbus_uart_config_t *hwcfg = pctx->config;
    pdev = bflb_device_get_by_name(pctx->config->name);
    if(pdev == NULL) {
        return -1;
    }

    struct bflb_uart_config_s cfg = {
        .baudrate = hwcfg->speed,
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .flow_ctrl = 0,
        .tx_fifo_threshold = 7,
        .rx_fifo_threshold = 7,
        .bit_order = UART_LSB_FIRST,
    };

    if (hwcfg->databits == 5)
        cfg.data_bits = UART_DATA_BITS_5;
    else if (hwcfg->databits == 6)
        cfg.data_bits = UART_DATA_BITS_6;
    else if (hwcfg->databits == 7)
        cfg.data_bits = UART_DATA_BITS_7;
    else if (hwcfg->databits == 8)
        cfg.data_bits = UART_DATA_BITS_8;
    else
        return -1;

    if (hwcfg->stopbits == 1)
        cfg.stop_bits = UART_STOP_BITS_1;
    else if (hwcfg->stopbits == 2)
        cfg.stop_bits = UART_STOP_BITS_1_5;
    else  if (hwcfg->stopbits == 3)
        cfg.stop_bits = UART_STOP_BITS_2;
    else
        return -1;

    if (hwcfg->parity == 0)
        cfg.parity = UART_PARITY_NONE;
    else if (hwcfg->parity == 1)
        cfg.parity = UART_PARITY_ODD;
    else  if (hwcfg->parity == 2)
        cfg.parity = UART_PARITY_EVEN;
    else  if (hwcfg->parity == 3)
        cfg.parity = UART_PARITY_MARK;
    else  if (hwcfg->parity == 4)
        cfg.parity = UART_PARITY_SPACE;
    else
        return -1;

    PERIPHERAL_CLOCK_UART1_ENABLE();
    bflb_uart_init(pdev, &cfg);
    bflb_uart_feature_control(pdev, UART_CMD_CLR_RX_FIFO, 0);
    bflb_uart_feature_control(pdev, UART_CMD_CLR_TX_FIFO, 0);
    bflb_uart_txint_mask(pdev, true);
    bflb_uart_rxint_mask(pdev, false);
    bflb_irq_attach(pdev->irq_num, uart_isr, pctx);
    bflb_irq_enable(pdev->irq_num);

    return 0;
}

int netbus_uart_send(netbus_uart_ctx_t *pctx, const uint8_t *p_data, uint32_t len, uint32_t timeout)
{
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
}

int netbus_uart_receive(netbus_uart_ctx_t *pctx, uint8_t *p_buffer, uint32_t buf_len, uint32_t timeout)
{
	pctx->task = xTaskGetCurrentTaskHandle();
	return xStreamBufferReceive(pctx->rxbuf, p_buffer, buf_len, timeout);
}

int netbus_uart_wakeup(netbus_uart_ctx_t *pctx)
{
	if (pctx == NULL || pctx->task == NULL) {
		return -1;
	}

	return xTaskAbortDelay(pctx->task) == pdPASS ? 0 : -1;
}

int netbus_uart_deinit(netbus_uart_ctx_t *ctx)
{
    struct bflb_device_s *pdev = bflb_device_get_by_name(ctx->config->name);

    bflb_uart_txint_mask(pdev, true);
    bflb_uart_rxint_mask(pdev, true);
    bflb_irq_detach(pdev->irq_num);
    bflb_irq_disable(pdev->irq_num);
    bflb_uart_deinit(pdev);
    vStreamBufferDelete(ctx->txbuf);
    vStreamBufferDelete(ctx->rxbuf);
    vSemaphoreDelete(ctx->w_mutex);
    return 0;
}
