/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __MSP_DMA__
#define __MSP_DMA__

void msp_dma_set_clk(uint8_t dmaId, uint8_t ch);

void msp_dma_irq_function(int dmaId, int dmaChan, void *irq_handler, void *dev);

#endif /* __MSP_DMA__ */

