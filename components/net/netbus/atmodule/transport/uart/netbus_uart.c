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
#include "netbus_uart.h"

#if 0
/* ------------------- platform port ------------------- */
#define rsl_malloc(x)          pvPortMalloc(x)
#define rsl_free(x)            vPortFree(x)

/* ------------------- internal type or rodata ------------------- */
struct _netbus_uart_low_priv {
    struct bflb_dma_channel_lli_pool_s *tx_lli;
    struct bflb_dma_channel_lli_pool_s *rx_lli;
};

/* ------------------- internal function ------------------- */
static ATTR_TCM_SECTION void _uart_dma_rx_irq(void *p_arg)
{
    netbus_uart_ctx_t *ctx = (netbus_uart_ctx_t *)p_arg;

    if (ctx->rx_cb) {
        ctx->rx_cb(ctx->rx_arg);
    }
}

static ATTR_TCM_SECTION void _uart_dma_tx_irq(void *p_arg)
{
    netbus_uart_ctx_t *ctx = (netbus_uart_ctx_t *)p_arg;

    if (ctx->tx_cb) {
        ctx->tx_cb(ctx->tx_arg);
    }

    if (ctx->rx_cb) {
        ctx->rx_cb(ctx->rx_arg);
    }
}

static void _uart_hw_init(const netbus_uart_config_t *config)
{
    struct bflb_device_s *pdev;

    struct bflb_uart_config_s cfg = {
        .baudrate = config->speed,
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .flow_ctrl = 0,
        .tx_fifo_threshold = 7,
        .rx_fifo_threshold = 0,
        .bit_order = UART_LSB_FIRST,
    };

    pdev = bflb_device_get_by_name(config->name);
    bflb_uart_init(pdev, &cfg);

    bflb_uart_link_txdma(pdev, true);
    bflb_uart_link_rxdma(pdev, true);
}

static void _uart_dma_init(netbus_uart_ctx_t *ctx)
{
    struct bflb_dma_channel_lli_transfer_s *tx_transfers;
    struct bflb_dma_channel_lli_transfer_s *rx_transfers;

    struct bflb_dma_channel_config_s tx_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = ctx->config->dma_req_tx,
        //.dst_req = DEFAULT_TEST_UART_DMA_TX_REQUEST,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_8BIT,
        .dst_width = DMA_DATA_WIDTH_8BIT,
    };

    struct bflb_dma_channel_config_s rx_config = {
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = ctx->config->dma_req_rx,
        //.src_req = DEFAULT_TEST_UART_DMA_RX_REQUEST,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_8BIT,
        .dst_width = DMA_DATA_WIDTH_8BIT,
    };
    ctx->dma_tx_chan = bflb_device_get_by_name(ctx->config->tx_dmach);
    ctx->dma_rx_chan = bflb_device_get_by_name(ctx->config->rx_dmach);

    bflb_dma_channel_init(ctx->dma_tx_chan, &tx_config);
    bflb_dma_channel_init(ctx->dma_rx_chan, &rx_config);

    bflb_dma_channel_irq_attach(ctx->dma_tx_chan, _uart_dma_tx_irq, (void *)ctx);
    bflb_dma_channel_irq_attach(ctx->dma_rx_chan, _uart_dma_rx_irq, (void *)ctx);

    bflb_irq_enable(((struct bflb_device_s *)(ctx->dma_tx_chan))->irq_num);
    bflb_irq_enable(((struct bflb_device_s *)(ctx->dma_rx_chan))->irq_num);

    tx_transfers = rsl_malloc(sizeof(struct bflb_dma_channel_lli_transfer_s) * ctx->items_tx);
    rx_transfers = rsl_malloc(sizeof(struct bflb_dma_channel_lli_transfer_s) * ctx->items_rx);

    for (int i = 0; i < ctx->items_tx; i++) {
        tx_transfers[i].src_addr = (uint32_t)ctx->node_tx[i].buf;
        tx_transfers[i].dst_addr = (uint32_t)ctx->config->dma_tdr;
        //tx_transfers[i].dst_addr = (uint32_t)DMA_ADDR_UART1_TDR;
        tx_transfers[i].nbytes = ctx->node_tx[i].len;
    }
    for (int i = 0; i < ctx->items_rx; i++) {
        rx_transfers[i].src_addr = (uint32_t)ctx->config->dma_rdr;
        //rx_transfers[i].src_addr = (uint32_t)DMA_ADDR_UART1_RDR;
        rx_transfers[i].dst_addr = (uint32_t)ctx->node_rx[i].buf;
        rx_transfers[i].nbytes = ctx->node_rx[i].len;
    }

    int used_count = bflb_dma_channel_lli_reload(ctx->dma_tx_chan,
                                                 ((struct _netbus_uart_low_priv *)ctx->priv)->tx_lli,
                                                 ctx->items_tx,
                                                 tx_transfers,
                                                 ctx->items_tx);
    bflb_dma_channel_lli_link_head(ctx->dma_tx_chan,
                                   ((struct _netbus_uart_low_priv *)ctx->priv)->tx_lli,
                                   used_count);

    used_count = bflb_dma_channel_lli_reload(ctx->dma_rx_chan,
                                             ((struct _netbus_uart_low_priv *)ctx->priv)->rx_lli,
                                             ctx->items_rx,
                                             rx_transfers,
                                             ctx->items_rx);
    bflb_dma_channel_lli_link_head(ctx->dma_rx_chan,
                                   ((struct _netbus_uart_low_priv *)ctx->priv)->rx_lli,
                                   used_count);

    rsl_free(tx_transfers);
    rsl_free(rx_transfers);
}

static int _uart_dma_start(netbus_uart_ctx_t *ctx)
{
    bflb_dma_channel_start(ctx->dma_rx_chan);
    bflb_dma_channel_start(ctx->dma_tx_chan);

    return 0;
}

static void uart_hw_callback_register(
        netbus_uart_ctx_t *ctx,
        netbus_uart_cb_func_t tx_cb, void *tx_arg,
        netbus_uart_cb_func_t rx_cb, void *rx_arg)
{
    ctx->tx_cb  = tx_cb;
    ctx->tx_arg = tx_arg;
    ctx->rx_cb  = rx_cb;
    ctx->rx_arg = rx_arg;
}


int uart_hw_start(netbus_uart_ctx_t *ctx)
{
    _uart_dma_start(ctx);

    return 0;
}

int uart_hw_clock_init(void)
{
    PERIPHERAL_CLOCK_UART1_ENABLE();
    PERIPHERAL_CLOCK_DMA0_ENABLE();

    //GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_XCLK, 0);

    //FIXME: default use uart1 gpio
    board_uartx_gpio_init();

    return 0;
}

int uart_hw_reset(netbus_uart_ctx_t *ctx)
{
    struct bflb_device_s *dev;

    if (ctx == NULL) {
        return -1;
    }

    uart_hw_clock_init();

    bflb_dma_channel_stop(ctx->dma_tx_chan);
    bflb_dma_channel_stop(ctx->dma_rx_chan);

    dev = bflb_device_get_by_name(ctx->config->name);
    bflb_uart_deinit(dev);

    _uart_hw_init(ctx->config);
    _uart_dma_init(ctx);

    return 0;
}

int uart_hw_deinit(netbus_uart_ctx_t *ctx)
{
    struct _netbus_uart_low_priv *priv;
    struct bflb_device_s *dev;

    if (ctx == NULL) {
        return -1;
    }

    priv = (struct _netbus_uart_low_priv *)ctx->priv;

    dev = bflb_device_get_by_name(ctx->config->name);
    bflb_uart_deinit(dev);

    bflb_dma_channel_stop(ctx->dma_tx_chan);
    bflb_dma_channel_stop(ctx->dma_rx_chan);

    if (ctx && ctx->node_rx) {
        rsl_free(ctx->node_rx);
    }
    if (ctx && ctx->node_tx) {
        rsl_free(ctx->node_tx);
    }
    if (priv && priv->tx_lli) {
        rsl_free(priv->tx_lli);
    }
    if (priv && priv->rx_lli) {
        rsl_free(priv->rx_lli);
    }
    if (priv) {
        rsl_free(priv);
    }
    return 0;
}

int uart_hw_init(
        const netbus_uart_config_t *config,
        netbus_uart_ctx_t *ctx,
        node_mem_t *node_tx, uint32_t items_tx,
        netbus_uart_cb_func_t tx_cb, void *tx_arg,
        node_mem_t *node_rx, uint32_t items_rx,
        netbus_uart_cb_func_t rx_cb, void *rx_arg,
        netbus_uart_cb_func_t reset_signal_cb, void *reset_signal_arg)
{
    struct _netbus_uart_low_priv *priv = NULL;

    if (ctx == NULL) {
        goto rsl_init_err;
    }

    uart_hw_clock_init();

    memset(ctx, 0, sizeof(netbus_uart_ctx_t));

    ctx->config      = config;
    ctx->dma_tx_chan = NULL;
    ctx->dma_rx_chan = NULL;
    ctx->reset_signal_cb = reset_signal_cb;
    ctx->reset_signal_arg = reset_signal_arg;

    ctx->node_tx = rsl_malloc(sizeof(node_mem_t) * items_tx);
    if (ctx->node_tx == NULL) {
        goto rsl_init_err;
    }
    ctx->node_rx = rsl_malloc(sizeof(node_mem_t) * items_rx);
    if (ctx->node_rx == NULL) {
        goto rsl_init_err;
    }
    memcpy(ctx->node_tx, node_tx, sizeof(node_mem_t) * items_tx);
    memcpy(ctx->node_rx, node_rx, sizeof(node_mem_t) * items_rx);
    ctx->items_tx    = items_tx;
    ctx->items_rx    = items_rx;

    uart_hw_callback_register(ctx, tx_cb, tx_arg, rx_cb, rx_arg);

    ctx->priv = rsl_malloc(sizeof(struct _netbus_uart_low_priv));
    if (ctx->priv == NULL) {
        goto rsl_init_err;
    }
    priv = (struct _netbus_uart_low_priv *)ctx->priv;

    /* tx lli config */
    priv->tx_lli = rsl_malloc(sizeof(struct bflb_dma_channel_lli_pool_s) * ctx->items_tx);
    if (priv->tx_lli == NULL) {
        goto rsl_init_err;
    }

    /* rx lli config */
    priv->rx_lli = rsl_malloc(sizeof(struct bflb_dma_channel_lli_pool_s) * ctx->items_rx);
    if (priv->rx_lli == NULL) {
        goto rsl_init_err;
    }

    _uart_hw_init(ctx->config);
    _uart_dma_init(ctx);
    _uart_dma_start(ctx);

    return 0;
rsl_init_err:
    if (ctx && ctx->node_rx) {
        rsl_free(ctx->node_rx);
    }
    if (ctx && ctx->node_tx) {
        rsl_free(ctx->node_tx);
    }
    if (priv && priv->tx_lli) {
        rsl_free(priv->tx_lli);
    }
    if (priv && priv->rx_lli) {
        rsl_free(priv->rx_lli);
    }
    if (priv) {
        rsl_free(priv);
    }
    return -1;
}

void *malloc_aligned_with_padding(int size, int align)
{
    size_t aligned_size = (size + (align - 1)) & ~(align - 1);
    uintptr_t addr = (uintptr_t)pvPortMalloc(aligned_size + align);
    if(addr == NULL) {
        return NULL;
    }
    uintptr_t aligned_addr = (addr + align) & ~(align - 1);

    // save pad size
    *(char *)(aligned_addr - 1) = aligned_addr - addr;

    return (void *)aligned_addr;
}

void free_aligned_with_padding(void *ptr)
{
    if(ptr == NULL) {
        return;
    }
    uintptr_t aligned_addr = (uintptr_t)ptr;
    uintptr_t addr = aligned_addr - *(char *)(aligned_addr - 1);

    vPortFree((void *)addr);
}

void *malloc_aligned_with_padding_nocache(int size, int align_bytes)
{
    return ((uint32_t)(malloc_aligned_with_padding(size, align_bytes))) & 0xBFFFFFFF;
}

void free_aligned_with_padding_nocache(void *ptr)
{
    free_aligned_with_padding(((uint32_t)(ptr)) | 0x60000000);
}
#endif


#define TXSLOT_COUNT                2
#define TXSLOT_SIZE                 1024
#define RXSLOT_COUNT                2
#define RXSLOT_SIZE                 1024
#define UART_TASK_PRI               (55)
#define UART_STACK_SIZE             (1024)
//#define NETBUS_TX_BUFFER_SIZE (2*1024)
//#define NETBUS_RX_BUFFER_SIZE (2*1024)

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
	int i, ret;

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

    pctx->config  = hwcfg;
	pctx->txbuf   = xStreamBufferCreate(txbuf_size, 1);
	pctx->rxbuf   = xStreamBufferCreate(rxbuf_size, 1);
	pctx->w_mutex = xSemaphoreCreateMutex();
    bflb_uart_init(pdev, &cfg);

    bflb_uart_txint_mask(pdev, true);
    bflb_uart_rxint_mask(pdev, false);
    bflb_irq_attach(pdev->irq_num, uart_isr, pctx);
    bflb_irq_enable(pdev->irq_num);

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
	return xStreamBufferReceive(pctx->rxbuf, p_buffer, buf_len, timeout);
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
