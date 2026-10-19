#include <stdio.h>
#include <stdint.h>
#include <utils_list.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ramsync.h"
#include "spisync_port.h"

#include "bl616_glb.h"
#include "bflb_clock.h"
#include <bflb_gpio.h>
#include <bflb_core.h>
#include <bflb_dma.h>
#include <bflb_spi.h>
#include <hardware/spi_reg.h>

/* ------------------- platform port ------------------- */
#define rsl_malloc(x)          malloc_aligned_with_padding(x, 32)
#define rsl_free(x)            free_aligned_with_padding(x)

/* ------------------- internal type or rodata ------------------- */
struct _ramsync_low_priv {
    struct bflb_dma_channel_lli_pool_s *tx_lli;
    struct bflb_dma_channel_lli_pool_s *rx_lli;
};

/* ------------------- internal function ------------------- */
static ATTR_TCM_SECTION void _spi_dma_rx_irq(void *p_arg)
{
    lramsync_ctx_t *ctx = (lramsync_ctx_t *)p_arg;

    if (ctx->rx_cb) {
        ctx->rx_cb(ctx->rx_arg);
    }
}

static ATTR_TCM_SECTION void _spi_dma_tx_irq(void *p_arg)
{
    lramsync_ctx_t *ctx = (lramsync_ctx_t *)p_arg;

    if (ctx->tx_cb) {
        ctx->tx_cb(ctx->tx_arg);
    }

    if (ctx->rx_cb) {
        ctx->rx_cb(ctx->rx_arg);
    }
}

static void _spi_hw_init(const spisync_hw_t *config)
{
    struct bflb_device_s *spi_dev;
    struct bflb_spi_config_s spi_cfg = {
        .freq = config->spi_speed,
        .role = SPI_ROLE_SLAVE,
        .mode = config->spi_mode,
        .data_width = SPI_DATA_WIDTH_32BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 1,
        .rx_fifo_threshold = 1,
    };

    spi_dev = bflb_device_get_by_name(config->spi_name);
    bflb_spi_init(spi_dev, &spi_cfg);
    if (config->spi_3pin_mode) {
        uint32_t regval = getreg32(spi_dev->reg_base + SPI_CONFIG_OFFSET);
        putreg32(regval | SPI_CR_SPI_S_3PIN_MODE, spi_dev->reg_base + SPI_CONFIG_OFFSET);
    }
    bflb_spi_link_txdma(spi_dev, true);
    bflb_spi_link_rxdma(spi_dev, true);
}

static void _spi_dma_init(lramsync_ctx_t *ctx)
{
    struct bflb_dma_channel_lli_transfer_s *tx_transfers;
    struct bflb_dma_channel_lli_transfer_s *rx_transfers;

    struct bflb_dma_channel_config_s tx_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = ctx->config->spi_dma_req_tx,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    struct bflb_dma_channel_config_s rx_config = {
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = ctx->config->spi_dma_req_rx,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };
    ctx->dma_tx_chan = bflb_device_get_by_name(ctx->config->spi_tx_dmach);
    ctx->dma_rx_chan = bflb_device_get_by_name(ctx->config->spi_rx_dmach);

    bflb_dma_channel_init(ctx->dma_tx_chan, &tx_config);
    bflb_dma_channel_init(ctx->dma_rx_chan, &rx_config);

    bflb_dma_channel_irq_attach(ctx->dma_tx_chan, _spi_dma_tx_irq, (void *)ctx);
    bflb_irq_enable(((struct bflb_device_s *)(ctx->dma_tx_chan))->irq_num);
    //bflb_dma_channel_irq_attach(ctx->dma_rx_chan, _spi_dma_rx_irq, (void *)ctx);

#if 1
    tx_transfers = rsl_malloc(sizeof(struct bflb_dma_channel_lli_transfer_s) * ctx->items_tx);
    rx_transfers = rsl_malloc(sizeof(struct bflb_dma_channel_lli_transfer_s) * ctx->items_rx);
    for (int i = 0; i < ctx->items_tx; i++) {
        tx_transfers[i].src_addr = (uint32_t)ctx->node_tx[i].buf;
        tx_transfers[i].dst_addr = (uint32_t)ctx->config->spi_dma_tdr;
        tx_transfers[i].nbytes = ctx->node_tx[i].len;
    }
    for (int i = 0; i < ctx->items_rx; i++) {
        rx_transfers[i].src_addr = (uint32_t)ctx->config->spi_dma_rdr;
        rx_transfers[i].dst_addr = (uint32_t)ctx->node_rx[i].buf;
        rx_transfers[i].nbytes = ctx->node_rx[i].len;
    }

    int used_count = bflb_dma_channel_lli_reload(ctx->dma_tx_chan,
                                                 ((struct _ramsync_low_priv *)ctx->priv)->tx_lli,
                                                 ctx->items_tx,
                                                 tx_transfers,
                                                 ctx->items_tx);
    bflb_dma_channel_lli_link_head(ctx->dma_tx_chan,
                                   ((struct _ramsync_low_priv *)ctx->priv)->tx_lli,
                                   used_count);

    used_count = bflb_dma_channel_lli_reload(ctx->dma_rx_chan,
                                             ((struct _ramsync_low_priv *)ctx->priv)->rx_lli,
                                             ctx->items_rx,
                                             rx_transfers,
                                             ctx->items_rx);
#else
    tx_transfers = rsl_malloc(sizeof(struct bflb_dma_channel_lli_transfer_s) * ctx->items_tx / 2);
    rx_transfers = rsl_malloc(sizeof(struct bflb_dma_channel_lli_transfer_s) * ctx->items_rx / 2);
    for (int i = 0; i < ctx->items_tx / 2; i++) {
        tx_transfers[i].src_addr = (uint32_t)ctx->node_tx[2*i].buf;
        tx_transfers[i].dst_addr = (uint32_t)ctx->config->spi_dma_tdr;
        tx_transfers[i].nbytes = ctx->node_tx[i].len + ctx->node_tx[i+1].len;
    }
    for (int i = 0; i < ctx->items_rx / 2; i++) {
        rx_transfers[i].src_addr = (uint32_t)ctx->config->spi_dma_rdr;
        rx_transfers[i].dst_addr = (uint32_t)ctx->node_rx[2*i].buf;
        rx_transfers[i].nbytes = ctx->node_rx[i].len + ctx->node_rx[i+1].len;
    }

    int used_count = bflb_dma_channel_lli_reload(ctx->dma_tx_chan,
                                                 ((struct _ramsync_low_priv *)ctx->priv)->tx_lli,
                                                 ctx->items_tx,
                                                 tx_transfers,
                                                 ctx->items_tx / 2);
    bflb_dma_channel_lli_link_head(ctx->dma_tx_chan,
                                   ((struct _ramsync_low_priv *)ctx->priv)->tx_lli,
                                   used_count);

    used_count = bflb_dma_channel_lli_reload(ctx->dma_rx_chan,
                                             ((struct _ramsync_low_priv *)ctx->priv)->rx_lli,
                                             ctx->items_rx,
                                             rx_transfers,
                                             ctx->items_rx / 2);
#endif
    bflb_dma_channel_lli_link_head(ctx->dma_rx_chan,
                                   ((struct _ramsync_low_priv *)ctx->priv)->rx_lli,
                                   used_count);

    rsl_free(tx_transfers);
    rsl_free(rx_transfers);
}

static int _spi_dma_start(lramsync_ctx_t *ctx)
{
    bflb_dma_channel_start(ctx->dma_rx_chan);
    bflb_dma_channel_start(ctx->dma_tx_chan);

    return 0;
}

static void lramsync_callback_register(
        lramsync_ctx_t *ctx,
        lramsync_cb_func_t tx_cb, void *tx_arg,
        lramsync_cb_func_t rx_cb, void *rx_arg)
{
    ctx->tx_cb  = tx_cb;
    ctx->tx_arg = tx_arg;
    ctx->rx_cb  = rx_cb;
    ctx->rx_arg = rx_arg;
}


int lramsync_start(lramsync_ctx_t *ctx)
{
    _spi_dma_start(ctx);

    return 0;
}

int lramsync_clock_init(void)
{
    PERIPHERAL_CLOCK_SPI0_ENABLE();
    PERIPHERAL_CLOCK_DMA0_ENABLE();

    GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_MCU_MUXPLL_160M, 0);
    GLB_Set_DBI_CLK(ENABLE, GLB_SPI_CLK_MCU_MUXPLL_160M, 0);

    GLB_Swap_MCU_SPI_0_MOSI_With_MISO(0);
}

int lramsync_reset(lramsync_ctx_t *ctx)
{
    struct bflb_device_s *spi_dev;

    if (ctx == NULL) {
        return -1;
    }

    spisync_log("lramsync_reset\r\n");
    lramsync_clock_init();

    bflb_dma_channel_stop(ctx->dma_tx_chan);
    bflb_dma_channel_stop(ctx->dma_rx_chan);

    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SPI);

    spi_dev = bflb_device_get_by_name(ctx->config->spi_name);
    bflb_spi_deinit(spi_dev);

    _spi_hw_init(ctx->config);
    _spi_dma_init(ctx);

    return 0;
}

int lramsync_deinit(lramsync_ctx_t *ctx)
{
    struct _ramsync_low_priv *priv;
    struct bflb_device_s *spi_dev;

    if (ctx == NULL) {
        return -1;
    }

    priv = (struct _ramsync_low_priv *)ctx->priv;

    spi_dev = bflb_device_get_by_name(ctx->config->spi_name);
    bflb_spi_deinit(spi_dev);

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

int lramsync_init(
        const spisync_hw_t *config,
        lramsync_ctx_t *ctx,
        node_mem_t *node_tx, uint32_t items_tx,
        lramsync_cb_func_t tx_cb, void *tx_arg,
        node_mem_t *node_rx, uint32_t items_rx,
        lramsync_cb_func_t rx_cb, void *rx_arg,
        lramsync_cb_func_t reset_signal_cb, void *reset_signal_arg)
{
    struct _ramsync_low_priv *priv = NULL;

    if (ctx == NULL) {
        goto rsl_init_err;
    }

    lramsync_clock_init();

    memset(ctx, 0, sizeof(lramsync_ctx_t));

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

    lramsync_callback_register(ctx, tx_cb, tx_arg, rx_cb, rx_arg);

    ctx->priv = rsl_malloc(sizeof(struct _ramsync_low_priv));
    if (ctx->priv == NULL) {
        goto rsl_init_err;
    }
    priv = (struct _ramsync_low_priv *)ctx->priv;

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

    _spi_hw_init(ctx->config);
    _spi_dma_init(ctx);
    _spi_dma_start(ctx);

    spisync_log("lramsync_init slave\r\n");
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

int lramsync_dump(lramsync_ctx_t *ctx)
{
    struct _ramsync_low_priv *priv = NULL;
    int i;
    struct bflb_dma_channel_lli_pool_s *node;

    priv = (struct _ramsync_low_priv *)ctx->priv;
    if ((NULL == ctx) || (NULL == ctx->priv)) {
        return -1;
    }

    // printf node config
    for(i = 0; i < ctx->items_tx; i++) {
        node = (struct bflb_dma_channel_lli_pool_s *)priv->tx_lli;
        printf("tx[%d] src:0x%08X, dst:0x%08X, next:0x%08X, ctl:0x%08X\r\n",
                i, node[i].src_addr, node[i].dst_addr, node[i].dst_addr, node[i].control.WORD);
    }
    for(i = 0; i < ctx->items_rx; i++) {
        node = (struct bflb_dma_channel_lli_pool_s *)priv->rx_lli;
        printf("rx[%d] src:0x%08X, dst:0x%08X, next:0x%08X, ctl:0x%08X\r\n",
                i, node[i].src_addr, node[i].dst_addr, node[i].dst_addr, node[i].control.WORD);
    }

    // printf reg value
    printf("dma-ch0 tx src:0x%08X, dst:0x%08X, tfsize:0x%08X-%d\r\n",
            *(uint32_t *)0x2000C100, *(uint32_t *)0x2000C104, *(uint32_t *)0x2000C10C, ((*(uint32_t *)0x2000C10C)&4095));
    printf("dma-ch1 rx src:0x%08X, dst:0x%08X, tfsize:0x%08X-%d\r\n",
            *(uint32_t *)0x2000C200, *(uint32_t *)0x2000C204, *(uint32_t *)0x2000C20C, ((*(uint32_t *)0x2000C20C)&4095));
    // Memory can store up to 4+32Bytes of data in shift registers and FIFO, spirxfifo 32Bytes, spitxfifo 32Bytes
    printf("spi cfg0-0x2000A280:0x%08X, cfg1-0x2000A284:0x%08X fifo_available_Bytes rx:%d-tx:%d\r\n",
            *(uint32_t *)0x2000A280,
            *(uint32_t *)0x2000A284, (*(uint32_t *)0x2000A284)&0x3F00,  (*(uint32_t *)0x2000A284)&0x3F);

    return 0;
}

int lramsync_get_info(lramsync_ctx_t *ctx, uint32_t *start_addr, uint32_t *curr_addr)
{
    struct _ramsync_low_priv *priv = NULL;
    struct bflb_dma_channel_lli_pool_s *node;

    priv = (struct _ramsync_low_priv *)ctx->priv;
    node = (struct bflb_dma_channel_lli_pool_s *)priv->tx_lli;

    if (start_addr) {
        *start_addr = node[0].src_addr;
    }

    if (curr_addr) {
        *curr_addr = (*(volatile uint32_t *)0x2000C100);
    }

    return 0;
}

