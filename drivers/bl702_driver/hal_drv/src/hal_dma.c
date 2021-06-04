/**
 * @file hal_dma.c
 * @brief 
 * 
 * Copyright (c) 2021 Bouffalolab team
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 * 
 */
#include "hal_dma.h"
#include "drv_mmheap.h"
#include "bl702_dma.h"

static dma_control_data_t dma_ctrl_cfg;

static void DMA0_IRQ(void);

static dma_device_t dmax_device[DMA_MAX_INDEX] =
{
#ifdef BSP_USING_DMA0_CH0
    DMA0_CH0_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH1
    DMA0_CH1_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH2
    DMA0_CH2_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH3
    DMA0_CH3_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH4
    DMA0_CH4_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH5
    DMA0_CH5_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH6
    DMA0_CH6_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH7
    DMA0_CH7_CONFIG,
#endif
};
/**
 * @brief 
 * 
 * @param dev 
 * @param oflag 
 * @return int 
 */
int dma_open(struct device *dev, uint16_t oflag)
{
    dma_device_t *dma_device = (dma_device_t *)dev;

    DMA_LLI_Cfg_Type lliCfg = {0};

    /* Disable all interrupt */
    DMA_IntMask(dma_device->ch, DMA_INT_ALL, MASK);
    /* Enable uart interrupt*/
    NVIC_DisableIRQ(DMA_ALL_IRQn);

    DMA_Disable();

    DMA_Channel_Disable(dma_device->ch);

    lliCfg.dir = dma_device->direction;
    lliCfg.srcPeriph = dma_device->src_req;
    lliCfg.dstPeriph = dma_device->dst_req;

    DMA_LLI_Init(dma_device->ch, &lliCfg);

    dma_ctrl_cfg.bits.fix_cnt = 0;
    dma_ctrl_cfg.bits.dst_min_mode = 0;
    dma_ctrl_cfg.bits.dst_add_mode = 0;

    DMA_Enable();

    Interrupt_Handler_Register(DMA_ALL_IRQn, DMA0_IRQ);

    return 0;
}
/**
 * @brief 
 * 
 * @param dev 
 * @param cmd 
 * @param args 
 * @return int 
 */
int dma_control(struct device *dev, int cmd, void *args)
{
    dma_device_t *dma_device = (dma_device_t *)dev;

    switch (cmd)
    {
        case DEVICE_CTRL_SET_INT /* constant-expression */:
            /* Dma interrupt configuration */
            DMA_IntMask(dma_device->ch, DMA_INT_TCOMPLETED, UNMASK);
            DMA_IntMask(dma_device->ch, DMA_INT_ERR, UNMASK);

            /* Enable uart interrupt*/
            NVIC_EnableIRQ(DMA_ALL_IRQn);
            break;
        case DEVICE_CTRL_CLR_INT /* constant-expression */:
            /* Dma interrupt configuration */
            DMA_IntMask(dma_device->ch, DMA_INT_TCOMPLETED, MASK);
            DMA_IntMask(dma_device->ch, DMA_INT_ERR, MASK);

            /* Enable uart interrupt*/
            NVIC_DisableIRQ(DMA_ALL_IRQn);
            break;
        case DEVICE_CTRL_GET_INT /* constant-expression */:
            /* code */
            break;
        case DEVICE_CTRL_CONFIG /* constant-expression */:
        {
            dma_ctrl_param_t *cfg = (dma_ctrl_param_t *)args;
            DMA_LLI_Cfg_Type lliCfg = {0};

            lliCfg.dir = cfg->direction;
            lliCfg.srcPeriph = cfg->src_req;
            lliCfg.dstPeriph = cfg->dst_req;

            DMA_LLI_Init(dma_device->ch, &lliCfg);

            break;
        }
        case DMA_CHANNEL_UPDATE:
            DMA_LLI_Update(dma_device->ch, (uint32_t)args);
            break;
        case DMA_CHANNEL_GET_STATUS /* constant-expression */:
            return DMA_Channel_Is_Busy(dma_device->ch);
        case DMA_CHANNEL_START /* constant-expression */:
            DMA_Channel_Enable(dma_device->ch);
            break;
        case DMA_CHANNEL_STOP /* constant-expression */:
            DMA_Channel_Disable(dma_device->ch);
            break;
        default:
            break;
    }

    return 0;
}
/**
 * @brief 
 * 
 * @param dev 
 * @return int 
 */
int dma_close(struct device *dev)
{
    DMA_Disable();
    return 0;
}

int dma_register(enum dma_index_type index, const char *name, uint16_t flag)
{
    struct device *dev;

    if (DMA_MAX_INDEX == 0)
        return -DEVICE_EINVAL;

    dev = &(dmax_device[index].parent);

    dev->open = dma_open;
    dev->close = dma_close;
    dev->control = dma_control;
    // dev->write = dma_write;
    // dev->read = dma_read;

    dev->status = DEVICE_UNREGISTER;
    dev->type = DEVICE_CLASS_DMA;
    dev->handle = NULL;

    return device_register(dev, name, flag);
}

static BL_Err_Type dma_scan_unregister_device(uint8_t * allocate_index){
    struct device *dev;
    dlist_t *node;
    uint8_t dma_index=0;
    uint32_t dma_handle[DMA_MAX_INDEX];

    for(dma_index = 0;dma_index<DMA_MAX_INDEX;dma_index++){
    	dma_handle[dma_index] = 0xff;
    }

    /* get registered dma handle list*/
    dlist_for_each(node, device_get_list_header())
    {
        dev = dlist_entry(node, struct device, list);

        if(dev->type == DEVICE_CLASS_DMA){
            dma_handle[(((uint32_t)dev - (uint32_t)dmax_device) / sizeof(dma_device_t)) % DMA_MAX_INDEX] = SET;
        }
    }

    for(dma_index = 0;dma_index<DMA_MAX_INDEX;dma_index++){
    	if(dma_handle[dma_index]==0xff){
    		* allocate_index =  dma_index;
    		return SUCCESS;
    	}
    }

    return ERROR;
}

int dma_allocate_register(const char *name, uint16_t flag)
{
    struct device *dev;
    uint8_t index;

    if (DMA_MAX_INDEX == 0)
        return -DEVICE_EINVAL;

    if(dma_scan_unregister_device(&index) == ERROR){
        return -DEVICE_ENOSPACE;
    }

    dev = &(dmax_device[index].parent);

    dev->open = dma_open;
    dev->close = dma_close;
    dev->control = dma_control;
    // dev->write = dma_write;
    // dev->read = dma_read;

    dev->status = DEVICE_UNREGISTER;
    dev->type = DEVICE_CLASS_DMA;
    dev->handle = NULL;

    return device_register(dev, name, flag);
}

/**
 * @brief 
 * 
 * @param dev 
 * @param src_addr 
 * @param dst_addr 
 * @param transfer_size 
 * @return int 
 */
int dma_reload(struct device *dev, uint32_t src_addr, uint32_t dst_addr, uint32_t transfer_size)
{
    uint32_t malloc_count;
    uint32_t remain_len;
    uint32_t actual_transfer_len = 0;
    uint32_t actual_transfer_offset = 0;

    dma_device_t *dma_device = (dma_device_t *)dev;

    DMA_Channel_Disable(dma_device->ch);

    if (dma_device->direction == DMA_MEMORY_TO_MEMORY)
    {
        dma_ctrl_cfg.bits.SI = 1;
        dma_ctrl_cfg.bits.DI = 1;
    }
    else if (dma_device->direction == DMA_MEMORY_TO_PERIPH)
    {
        dma_ctrl_cfg.bits.SI = 1;
        dma_ctrl_cfg.bits.DI = 0;
    }
    else if (dma_device->direction == DMA_PERIPH_TO_MEMORY)
    {
        dma_ctrl_cfg.bits.SI = 0;
        dma_ctrl_cfg.bits.DI = 1;
    }
    else if (dma_device->direction == DMA_PERIPH_TO_PERIPH)
    {
        dma_ctrl_cfg.bits.SI = 0;
        dma_ctrl_cfg.bits.DI = 0;
    }

    dma_ctrl_cfg.bits.SBSize = dma_device->src_burst_size;
    dma_ctrl_cfg.bits.DBSize = dma_device->dst_burst_size;
    dma_ctrl_cfg.bits.SWidth = dma_device->src_width;
    dma_ctrl_cfg.bits.DWidth = dma_device->dst_width;

    if (dma_device->src_width == DMA_TRANSFER_WIDTH_8BIT)
    {
        actual_transfer_offset = 4095;
        actual_transfer_len = transfer_size;
    }
    else if (dma_device->src_width == DMA_TRANSFER_WIDTH_16BIT)
    {
        if (transfer_size % 2)
            return -1;

        actual_transfer_offset = (4095 * 2);
        actual_transfer_len = transfer_size/2;
    }
    else if (dma_device->src_width == DMA_TRANSFER_WIDTH_32BIT)
    {
        if (transfer_size % 4)
            return -1;

        actual_transfer_offset = (4095 * 4);
        actual_transfer_len = transfer_size/4;
    }

    malloc_count = actual_transfer_len / 4095;
    remain_len = actual_transfer_len % 4095;

    if (remain_len)
    {
        malloc_count++;
    }

    if(dma_device->lli_cfg)
    {
        mmheap_free(dma_device->lli_cfg);
        dma_device->lli_cfg = (dma_lli_ctrl_t *)mmheap_alloc(sizeof(dma_lli_ctrl_t) * malloc_count);
    }
    else
    {
         dma_device->lli_cfg = (dma_lli_ctrl_t *)mmheap_alloc(sizeof(dma_lli_ctrl_t) * malloc_count);
    }

    if (dma_device->lli_cfg)
    {
        /*transfer_size will be 4095 or 4095*2 or 4095*4 in different transfer width*/
        if ((!remain_len) && (malloc_count == 1))
        {
            dma_device->lli_cfg[0].src_addr = src_addr;
            dma_device->lli_cfg[0].dst_addr = dst_addr;
            dma_device->lli_cfg[0].nextlli = 0;
            dma_ctrl_cfg.bits.TransferSize = remain_len;
            dma_ctrl_cfg.bits.I = 1;
            memcpy(&dma_device->lli_cfg[0].cfg, &dma_ctrl_cfg, sizeof(dma_control_data_t));
        }
        /*transfer_size will be 4095*n or 4095*2*n or 4095*4*n,(n>1) in different transfer width*/
        else if ((!remain_len) && (malloc_count > 1))
        {
            for (uint32_t i = 0; i < malloc_count; i++)
            {
                dma_device->lli_cfg[i].src_addr = src_addr;
                dma_device->lli_cfg[i].dst_addr = dst_addr;
                dma_device->lli_cfg[i].nextlli = 0;

                dma_ctrl_cfg.bits.TransferSize = 4095;
                dma_ctrl_cfg.bits.I = 0;
                if (dma_ctrl_cfg.bits.SI)
                    src_addr += actual_transfer_offset;
                if (dma_ctrl_cfg.bits.DI)
                    dst_addr += actual_transfer_offset;

                if (i == malloc_count - 1)
                {
                    dma_ctrl_cfg.bits.I = 1;

                    if (dma_device->transfer_mode == DMA_LLI_CYCLE_MODE)
                        dma_device->lli_cfg[i].nextlli = (uint32_t)&dma_device->lli_cfg[0];
                }
                if (i)
                {
                    dma_device->lli_cfg[i - 1].nextlli = (uint32_t)&dma_device->lli_cfg[i];
                }
                memcpy(&dma_device->lli_cfg[i].cfg, &dma_ctrl_cfg, sizeof(dma_control_data_t));
            }
        }
        else
        {
            for (uint32_t i = 0; i < malloc_count; i++)
            {
                dma_device->lli_cfg[i].src_addr = src_addr;
                dma_device->lli_cfg[i].dst_addr = dst_addr;
                dma_device->lli_cfg[i].nextlli = 0;

                dma_ctrl_cfg.bits.TransferSize = 4095;
                dma_ctrl_cfg.bits.I = 0;
                if (dma_ctrl_cfg.bits.SI)
                    src_addr += actual_transfer_offset;
                if (dma_ctrl_cfg.bits.DI)
                    dst_addr += actual_transfer_offset;

                if (i == malloc_count - 1)
                {
                    dma_ctrl_cfg.bits.TransferSize = remain_len;
                    dma_ctrl_cfg.bits.I = 1;

                    if (dma_device->transfer_mode == DMA_LLI_CYCLE_MODE)
                        dma_device->lli_cfg[i].nextlli = (uint32_t)&dma_device->lli_cfg[0];

                }
                if (i)
                {
                    dma_device->lli_cfg[i - 1].nextlli = (uint32_t)&dma_device->lli_cfg[i];
                }
                memcpy(&dma_device->lli_cfg[i].cfg, &dma_ctrl_cfg, sizeof(dma_control_data_t));
            }
        }
        DMA_LLI_Update(dma_device->ch, (uint32_t)dma_device->lli_cfg);
    }
    else
    {
        return -2;
    }

    return 0;
}
/**
 * @brief 
 * 
 * @param handle 
 */
void dma_isr(dma_device_t *handle)
{
    uint32_t tmpVal;
    uint32_t intClr;

    /* Get DMA register */
    if (handle->id == 0)
    {
        uint32_t DMAChs = DMA_BASE;
        for (uint8_t i = 0; i < DMA_MAX_INDEX; i++)
        {
            tmpVal = BL_RD_REG(DMAChs, DMA_INTTCSTATUS);
            if ((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCSTATUS) & (1 << handle[i].ch)) != 0)
            {
                /* Clear interrupt */
                tmpVal = BL_RD_REG(DMAChs, DMA_INTTCCLEAR);
                intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR);
                intClr |= (1 << handle[i].ch);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR, intClr);
                BL_WR_REG(DMAChs, DMA_INTTCCLEAR, tmpVal);

                if (handle[i].parent.callback)
                    handle[i].parent.callback(&handle[i].parent, NULL, 0, DMA_INT_TCOMPLETED);
            }
        }

        for (uint8_t i = 0; i < DMA_MAX_INDEX; i++)
        {
            tmpVal = BL_RD_REG(DMAChs, DMA_INTERRORSTATUS);
            if ((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRORSTATUS) & (1 << handle[i].ch)) != 0)
            {
                /*Clear interrupt */
                tmpVal = BL_RD_REG(DMAChs, DMA_INTERRCLR);
                intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR);
                intClr |= (1 << handle[i].ch);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR, intClr);
                BL_WR_REG(DMAChs, DMA_INTERRCLR, tmpVal);

                if (handle[i].parent.callback)
                    handle[i].parent.callback(&handle->parent, NULL, 0, DMA_INT_ERR);
            }
        }
    }
    else
    {
    }
}
/**
 * @brief 
 * 
 */
void DMA0_IRQ(void)
{
    dma_isr(&dmax_device[0]);
}
