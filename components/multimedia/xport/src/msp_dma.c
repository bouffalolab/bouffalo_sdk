//#include <drv/codec.h>
#include <drv/dma.h>
#include <drv/irq.h>

// #include <bl606p_glb.h>
// #include <bl606p_common.h>
#include <stdint.h>
#include <msp_dma.h>
#include <msp_bl606p_dma.h>
#include <msp_bl606p_glb.h>

//#include "blyoc_sys.h"
// #include "blyoc_dma.h"
// #include "ulog/ulog.h"

typedef struct _dma_callbak_func_type {
    void (*dma_callbak_func)(void *arg);
    uint32_t            argv;   //
} _dma_callbak_func_t;

typedef enum {
    GLB_DMA0_CLK_CH0, /*!< DMA0 clock ID:channel 0 */
    GLB_DMA0_CLK_CH1, /*!< DMA0 clock ID:channel 1 */
    GLB_DMA0_CLK_CH2, /*!< DMA0 clock ID:channel 2 */
    GLB_DMA0_CLK_CH3, /*!< DMA0 clock ID:channel 3 */
    GLB_DMA0_CLK_CH4, /*!< DMA0 clock ID:channel 4 */
    GLB_DMA0_CLK_CH5, /*!< DMA0 clock ID:channel 5 */
    GLB_DMA0_CLK_CH6, /*!< DMA0 clock ID:channel 6 */
    GLB_DMA0_CLK_CH7, /*!< DMA0 clock ID:channel 7 */
    GLB_DMA1_CLK_CH0, /*!< DMA1 clock ID:channel 0 */
    GLB_DMA1_CLK_CH1, /*!< DMA1 clock ID:channel 1 */
    GLB_DMA1_CLK_CH2, /*!< DMA1 clock ID:channel 2 */
    GLB_DMA1_CLK_CH3, /*!< DMA1 clock ID:channel 3 */
} GLB_DMA_CLK_ID_Type;

static  uint32_t blyoc_dmaAddr[DMA_ID_MAX] = { DMA0_BASE, DMA1_BASE, DMA2_BASE };

static _dma_callbak_func_t dmaIntCbfArra[DMA_ID_MAX][DMA_CH_MAX][DMA_INT_ALL];

csi_dma_t blyoc_dma_dev[DMA_ID_MAX];

extern BL_Err_Type GLB_Set_DMA_CLK(uint8_t enable, GLB_DMA_CLK_ID_Type clk);
extern BL_Err_Type GLB_PER_Clock_UnGate(uint64_t ips);

void msp_dma_set_clk(uint8_t dmaId, uint8_t ch)
{
    switch (dmaId) {
        case DMA0_ID:
            if (ch >= DMA_CH_MAX){
                return;
            } else {
                GLB_Set_DMA_CLK(ENABLE, (GLB_DMA_CLK_ID_Type)ch);
                GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);
            }
            break;
        case DMA1_ID:
            if (ch > DMA_CH4){
                return;
            } else {
                GLB_Set_DMA_CLK(ENABLE, (GLB_DMA_CLK_ID_Type)(ch + DMA_CH_MAX));
                GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_1);
            }
            break;
        case DMA2_ID:
            //TODO: GLB_Set_DMA_CLK(ENABLE, GLB_DMA0_CLK_CH0);
            GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_2);
            break;
        default:
            break;
    }

}

void __dma_callback_install(DMA_ID_Type dmaId, DMA_Chan_Type dmaChan, DMA_INT_Type intType,
                                void *cbFun, void *agrv)
{
    /* Check the parameters */
    CHECK_PARAM(IS_DMA_ID_TYPE(dmaId));
    CHECK_PARAM(IS_DMA_CHAN_TYPE(dmaChan));
    CHECK_PARAM(IS_DMA_INT_TYPE(intType));

    dmaIntCbfArra[dmaId][dmaChan][intType].dma_callbak_func = cbFun;
    dmaIntCbfArra[dmaId][dmaChan][intType].argv = (uint32_t)agrv;
}

static void __dma_inthandler(DMA_ID_Type dmaId, void *arg)
{
    uint32_t tmpVal;
    uint32_t intClr;
    uint8_t ch;
    /* Get DMA register */
    uint32_t dma_ch = blyoc_dmaAddr[dmaId];

    for (ch = 0; ch < DMA_CH_MAX; ch++) {
        tmpVal = BL_RD_REG(dma_ch, DMA_INTTCSTATUS);

        if ((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCSTATUS) & (1 << ch)) != 0) {
            /* Clear interrupt */
            tmpVal = BL_RD_REG(dma_ch, DMA_INTTCCLEAR);
            intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR);
            intClr |= (1 << ch);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR, intClr);
            BL_WR_REG(dma_ch, DMA_INTTCCLEAR, tmpVal);

            if (dmaIntCbfArra[dmaId][ch][DMA_INT_TCOMPLETED].dma_callbak_func != NULL) {
                /* Call the callback function */
                dmaIntCbfArra[dmaId][ch][DMA_INT_TCOMPLETED].dma_callbak_func((void *)dmaIntCbfArra[dmaId][ch][DMA_INT_TCOMPLETED].argv);
            }
        }
    }

    for (ch = 0; ch < DMA_CH_MAX; ch++) {
        tmpVal = BL_RD_REG(dma_ch, DMA_INTERRORSTATUS);

        if ((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRORSTATUS) & (1 << ch)) != 0) {
            /*Clear interrupt */
            tmpVal = BL_RD_REG(dma_ch, DMA_INTERRCLR);
            intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR);
            intClr |= (1 << ch);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR, intClr);
            BL_WR_REG(dma_ch, DMA_INTERRCLR, tmpVal);

            if (dmaIntCbfArra[dmaId][ch][DMA_INT_ERR].dma_callbak_func != NULL) {
                /* Call the callback function */
                dmaIntCbfArra[dmaId][ch][DMA_INT_ERR].dma_callbak_func((void *)dmaIntCbfArra[dmaId][ch][DMA_INT_ERR].argv);
            }
        }
    }
}

static void __dma0_irqhandlert(void *arg)
{
    __dma_inthandler(DMA0_ID, arg);
}

static void __dma1_irqhandlert(void *arg)
{
    __dma_inthandler(DMA1_ID, arg);
}


void msp_dma_irq_function(int dmaId, int dmaChan, void *irq_handler, void *dev)
{
    __dma_callback_install(dmaId, dmaChan, DMA_INT_TCOMPLETED,
                               (void *)irq_handler, dev);

    switch (dmaId) {
        case DMA0_ID:
            csi_irq_attach(DMA0_ALL_IRQn, __dma0_irqhandlert, (void *)&blyoc_dma_dev[dmaId]);
            csi_irq_enable(DMA0_ALL_IRQn);
            break;
        case DMA1_ID:
            csi_irq_attach(DMA1_ALL_IRQn, __dma1_irqhandlert, (void *)&blyoc_dma_dev[dmaId]);
            csi_irq_enable(DMA1_ALL_IRQn);
            break;
        default:
            //LOGD("DMA", "DMA ID Is Invaild\r\n");
            break;
    }
}

