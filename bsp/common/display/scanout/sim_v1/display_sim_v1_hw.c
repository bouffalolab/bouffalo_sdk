/**
 * @file display_sim_v1_hw.c
 * @brief SIM v1 hardware access for BL616: pins, the SIM engine function table and DMA.
 *
 * One frame is a head node with pre-frame control words, pixel data nodes and
 * a tail node with post-frame control words. The tail interrupt renews the frame.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "scanout/sim_v1/display_sim_v1_hw.h"
#include "display_os.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "hardware/dma_reg.h"

#define SIM_V1_GPIO_FUNC   27
#define SIM_V1_TX_FIFO     0x2005A010u
#define SIM_V1_DMA_REQUEST 0x0000001Cu
#define SIM_V1_PIN_MODE    (SIM_V1_GPIO_FUNC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2)

/** @brief Parameters of the SIM engine, the layout the ROM function expects. */
struct sim_v1_rom_dpi {
    uint16_t width;
    uint16_t height;
    uint16_t hsw;
    uint16_t hbp;
    uint16_t hfp;
    uint16_t vsw;
    uint16_t vbp;
    uint16_t vfp;
    bool data_latch_mode;
    bool de_sig_enable;
    bool de_sig_polarity;
    bool v_sync_sig_polarity;
    bool h_sync_sig_polarity;
    uint32_t *invalid_row_para;
    uint16_t frame_rate;
    uint16_t data_format;
    uint8_t pin_clk;
    uint8_t pin_hs;
    uint8_t pin_data;
};

/* The image supplies a table of relative function addresses; absent tables report -ENOTSUP. */
extern uint32_t __edata_etext_final __attribute__((weak));
#define SIM_V1_ROM_INDEX_DPI_INIT  11
#define SIM_V1_ROM_INDEX_CLOCK_SET 12

static uint32_t sim_v1_rom_function(uint32_t index)
{
    uint32_t base = (uint32_t)&__edata_etext_final;

    return ((uint32_t *)base)[index] + base;
}

static int sim_v1_rom_dpi_init(struct sim_v1_rom_dpi *dpi)
{
    return ((int (*)(struct sim_v1_rom_dpi *))sim_v1_rom_function(SIM_V1_ROM_INDEX_DPI_INIT))(dpi);
}

static void sim_v1_rom_clock_set(uint8_t enable, uint8_t src, uint8_t div)
{
    ((void (*)(uint8_t, uint8_t, uint8_t))sim_v1_rom_function(SIM_V1_ROM_INDEX_CLOCK_SET))(enable, src, div);
}

static void sim_v1_node_init(struct bflb_dma_channel_lli_pool_s *node)
{
    node->control.bits.SBSize = DMA_BURST_INCR4;
    node->control.bits.dst_min_mode = 0;
    node->control.bits.DBSize = DMA_BURST_INCR4;
    node->control.bits.dst_add_mode = 0;
    node->control.bits.SWidth = DMA_DATA_WIDTH_32BIT;
    node->control.bits.DWidth = DMA_DATA_WIDTH_32BIT;
    node->control.bits.fix_cnt = 0;
    node->control.bits.SI = DMA_ADDR_INCREMENT_ENABLE;
    node->control.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
    node->control.bits.I = 0;
    node->dst_addr = SIM_V1_TX_FIFO;
    node->nextlli = 0;
}

static void sim_v1_pin(struct bflb_device_s *gpio, uint8_t pin)
{
    bflb_gpio_init(gpio, pin, SIM_V1_PIN_MODE);
}

static void sim_v1_pin_release(struct bflb_device_s *gpio, uint8_t pin)
{
    bflb_gpio_init(gpio, pin, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
}

static void sim_v1_pins(const display_sim_v1_cfg_t *cfg, void (*fn)(struct bflb_device_s *, uint8_t))
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    uint8_t data_lines = cfg->latch_mode ? 8 : 16;

    fn(gpio, cfg->pin_clk);
    if (cfg->de) {
        fn(gpio, (uint8_t)(cfg->pin_clk + 1));
    }
    if (cfg->latch_mode) {
        fn(gpio, (uint8_t)(cfg->pin_clk + 2));
    }
    fn(gpio, cfg->pin_hsync);
    fn(gpio, (uint8_t)(cfg->pin_hsync + 1));
    for (uint8_t i = 0; i < data_lines; i++) {
        fn(gpio, (uint8_t)(cfg->pin_data + i));
    }
}

static void sim_v1_dma_irq(void *arg)
{
    display_sim_v1_frame_end(arg);
}

int display_sim_v1_hw_init(display_sim_v1_scanout_t *sim)
{
    sim->dma = bflb_device_get_by_name(sim->cfg.dma);
    return sim->dma ? 0 : -ENODEV;
}

int display_sim_v1_hw_open(display_sim_v1_scanout_t *sim, const display_video_mode_t *mode, uint16_t frame_rate)
{
    struct sim_v1_rom_dpi dpi = {
        .width = mode->width,
        .height = mode->height,
        .hsw = mode->hsw,
        .hbp = mode->hbp,
        .hfp = mode->hfp,
        .vsw = mode->vsw,
        .vbp = mode->vbp,
        .vfp = mode->vfp,
        .data_latch_mode = sim->cfg.latch_mode,
        .de_sig_enable = true,
        .de_sig_polarity = !mode->de_invert,
        .v_sync_sig_polarity = mode->vsync_invert,
        .h_sync_sig_polarity = mode->hsync_invert,
        .invalid_row_para = sim->invalid_rows,
        .frame_rate = frame_rate,
        .data_format = 16,
        .pin_clk = sim->cfg.pin_clk,
        .pin_hs = sim->cfg.pin_hsync,
        .pin_data = sim->cfg.pin_data,
    };
    struct bflb_dma_channel_config_s tx = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = SIM_V1_DMA_REQUEST,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    if (!&__edata_etext_final) {
        return -ENOTSUP;
    }
    sim_v1_pins(&sim->cfg, sim_v1_pin);
    sim_v1_rom_clock_set(1, 0, 0);
    if (sim_v1_rom_dpi_init(&dpi)) {
        sim_v1_pins(&sim->cfg, sim_v1_pin_release);
        return -ENOTSUP;
    }

    sim_v1_node_init(&sim->head);
    sim->head.src_addr = (uint32_t)(uintptr_t)sim->invalid_rows;
    sim->head.control.bits.TransferSize = (mode->vsw + mode->vbp) * 2u + 1u;
    sim_v1_node_init(&sim->tail);
    sim->tail.src_addr = (uint32_t)(uintptr_t)&sim->invalid_rows[(mode->vsw + mode->vbp + 1u) * 2u];
    sim->tail.control.bits.TransferSize = mode->vfp * 2u;
    sim->tail.control.bits.I = 1;
    bflb_l1c_dcache_clean_range(sim->invalid_rows, sizeof(sim->invalid_rows));
    bflb_l1c_dcache_clean_range(&sim->tail, sizeof(sim->tail));

    bflb_dma_channel_init(sim->dma, &tx);
    bflb_dma_channel_irq_attach(sim->dma, sim_v1_dma_irq, sim);
    return 0;
}

void display_sim_v1_hw_build_list(display_sim_v1_scanout_t *sim, struct bflb_dma_channel_lli_pool_s *nodes,
                                  uint16_t count, const void *frame, uint32_t words)
{
    for (uint16_t i = 0; i < count; i++) {
        uint32_t done = (uint32_t)i * 4032u;

        sim_v1_node_init(&nodes[i]);
        nodes[i].src_addr = (uint32_t)(uintptr_t)frame + done * 4u;
        nodes[i].control.bits.TransferSize = (words - done) > 4032u ? 4032u : (words - done);
        nodes[i].nextlli = (uint32_t)(uintptr_t)(i + 1 < count ? &nodes[i + 1] : &sim->tail);
    }
    bflb_l1c_dcache_clean_range(nodes, sizeof(nodes[0]) * count);
}

void display_sim_v1_hw_run(display_sim_v1_scanout_t *sim, struct bflb_dma_channel_lli_pool_s *nodes)
{
    uint32_t base = sim->dma->reg_base;

    sim->head.nextlli = (uint32_t)(uintptr_t)nodes;
    bflb_dma_channel_stop(sim->dma);
    putreg32(sim->head.src_addr, base + DMA_CxSRCADDR_OFFSET);
    putreg32(sim->head.dst_addr, base + DMA_CxDSTADDR_OFFSET);
    putreg32(sim->head.nextlli, base + DMA_CxLLI_OFFSET);
    putreg32(sim->head.control.WORD, base + DMA_CxCONTROL_OFFSET);
    bflb_dma_channel_start(sim->dma);
}

void display_sim_v1_hw_close(display_sim_v1_scanout_t *sim)
{
    uintptr_t flags = display_os_irq_save();

    /* LHAL detaches the handler before masking: exclude the shared DMA ISR until this channel is quiet. */
    bflb_dma_channel_stop(sim->dma);
    bflb_dma_channel_tcint_mask(sim->dma, true);
    bflb_dma_channel_tcint_clear(sim->dma);
    bflb_dma_channel_irq_detach(sim->dma);
    display_os_irq_restore(flags);
    sim_v1_pins(&sim->cfg, sim_v1_pin_release);
}