#include "bl702_common.h"
#include "bl702_glb.h"
#include "bl702_emac.h"
#include "bflb_platform.h"
#include "emac_reg.h"

#include "emac_bd.h"

#define TAG "EMAC_BD: "

#define ETH_RXBUFNB 5
#define ETH_TXBUFNB 3

EMAC_Handle_Type ethHandle;
static EMAC_Handle_Type *thiz = NULL;

uint8_t ethRxBuff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE] __attribute__ ((align(4)))={0}; /* Ethernet Receive Buffers */
uint8_t ethTxBuff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE] __attribute__ ((align(4))); /* Ethernet Transmit Buffers */

BL_Err_Type EMAC_BD_Init(void)
{
    BL_Err_Type err = SUCCESS;
    thiz = &ethHandle;

    /* init the BDs in emac with buffer address */
    err = EMAC_DMADescListInit(thiz, (uint8_t *)ethTxBuff, ETH_TXBUFNB, (uint8_t *)ethRxBuff, ETH_RXBUFNB);

    return err;
}

BL_Err_Type EMAC_BD_TX_enqueue(uint32_t flags, uint32_t len, const uint8_t *data_in)
{
    BL_Err_Type err = SUCCESS;
    EMAC_BD_Desc_Type *DMADesc;

    DMADesc = &thiz->bd[thiz->txIndexCPU];

    if ((uint32_t)(-1) == flags) {
        flags = EMAC_TX_COMMON_FLAGS;
    }

    if(DMADesc->C_S_L & EMAC_BD_FIELD_MSK(TX_RD)){
        /* no free BD, lost sync with DMA TX? */
        err = NORESC;
        //MSG_ERR(TAG"%s:%d\n", __func__, __LINE__);
    }else{
        //DMADesc->TXBuffer = (uint32_t)data_in;
        ARCH_MemCpy_Fast((void *)DMADesc->Buffer, data_in, len);

        DMADesc->C_S_L = flags | (len << BD_TX_LEN_POS);
        /* move to next TX BD */
        if ((++thiz->txIndexCPU) > thiz->txBuffLimit) {
            /* the last BD */
            DMADesc->C_S_L |= EMAC_BD_FIELD_MSK(TX_WR);
            /* wrap back */
            thiz->txIndexCPU = 0;
        }
    }

    return err;
}

/* this func will be called in ISR */
BL_Err_Type EMAC_BD_TX_dequeue(uint32_t index)
{
    BL_Err_Type err = SUCCESS;
    EMAC_BD_Desc_Type *DMADesc;

    thiz->txIndexEMAC = index;
    DMADesc = &thiz->bd[thiz->txIndexEMAC];
    /* release this tx BD to SW (HW will do this) */
    DMADesc->C_S_L &= EMAC_BD_FIELD_UMSK(TX_RD);

    return err;
}

void EMAC_BD_TX_onErr(uint32_t index)
{
    /* handle error */
    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_UR)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_RTRY)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_RL)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_LC)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_DF)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(TX_CS)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    thiz->bd[index].C_S_L &= ~0xff;
}

BL_Err_Type EMAC_BD_RX_enqueue(uint32_t index)
{
    BL_Err_Type err = SUCCESS;

    thiz->rxIndexEMAC = index;

    return err;
}

BL_Err_Type EMAC_BD_RX_dequeue(uint32_t flags, uint32_t *len, uint8_t *data_out)
{
    BL_Err_Type err = SUCCESS;
    EMAC_BD_Desc_Type *DMADesc;

    DMADesc = &thiz->bd[thiz->rxIndexCPU];

    if(DMADesc->C_S_L & EMAC_BD_FIELD_MSK(RX_E)){
        /* current RX BD is empty */
        err = NORESC;
        *len = 0;
        //MSG_ERR(TAG"%s:%d\n", __func__, __LINE__);
    }else{
        *len = (thiz->bd[thiz->rxIndexCPU].C_S_L & EMAC_BD_FIELD_MSK(RX_LEN)) >> BD_RX_LEN_POS;
        if (data_out) {
            ARCH_MemCpy_Fast(data_out, (const void *)DMADesc->Buffer, *len);
        }
        /* RX BD can be used for another receive */
        DMADesc->C_S_L |= EMAC_BD_FIELD_MSK(RX_E);
        /* move to next RX BD */
        if ((++thiz->rxIndexCPU) > thiz->rxBuffLimit) {
            /* the last BD */
            DMADesc->C_S_L |= EMAC_BD_FIELD_MSK(RX_WR);
            /* wrap back */
            thiz->rxIndexCPU = thiz->txBuffLimit + 1;
        }
    }

    return err;
}

void EMAC_BD_RX_onErr(uint32_t index)
{
    /* handle error */
    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_OR)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_RE)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_DN)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_TL)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_CRC)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    if (thiz->bd[index].C_S_L & EMAC_BD_FIELD_MSK(RX_LC)) {
        MSG("%s:%d\r\n", __func__, __LINE__);
    }

    thiz->bd[index].C_S_L &= ~0xff;
    /* RX BD is ready for RX */
    thiz->bd[index].C_S_L |= EMAC_BD_FIELD_MSK(RX_E);
}

uint32_t EMAC_BD_Get_CurActive(EMAC_BD_TYPE_e bdt)
{
    uint32_t bd = 0;

    bd = BL_RD_REG(EMAC_BASE, EMAC_TX_BD_NUM);

    if (bdt == EMAC_BD_TYPE_TX) {
        bd &= EMAC_TXBDPTR_MSK;
        bd >>= EMAC_TXBDPTR_POS;
    }

    if (bdt == EMAC_BD_TYPE_RX) {
        bd &= EMAC_RXBDPTR_MSK;
        bd >>= EMAC_RXBDPTR_POS;
    }

    return bd;
}
