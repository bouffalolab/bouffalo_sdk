#ifndef _EMAC_BD_H_
#define _EMAC_BD_H_

#define EMAC_TX_COMMON_FLAGS   (EMAC_BD_FIELD_MSK(TX_RD)  | \
                                EMAC_BD_FIELD_MSK(TX_IRQ) | \
                                EMAC_BD_FIELD_MSK(TX_PAD) | \
                                EMAC_BD_FIELD_MSK(TX_CRC) )

#define EMAC_RX_COMMON_FLAGS   (ETH_MAX_PACKET_SIZE << 16) | \
                                EMAC_BD_FIELD_MSK(RX_IRQ) )

typedef enum _BD_TYPE_ {
    EMAC_BD_TYPE_INVLAID,
    EMAC_BD_TYPE_TX,
    EMAC_BD_TYPE_RX,
    EMAC_BD_TYPE_NONE,
    EMAC_BD_TYPE_MAX = 0x7FFFFFFF
} EMAC_BD_TYPE_e;

BL_Err_Type EMAC_BD_Init(void);
BL_Err_Type EMAC_BD_TX_enqueue(uint32_t flags, uint32_t len, const uint8_t *data_in);
BL_Err_Type EMAC_BD_TX_dequeue(uint32_t index);
void EMAC_BD_TX_onErr(uint32_t index);
BL_Err_Type EMAC_BD_RX_enqueue(uint32_t index);
BL_Err_Type EMAC_BD_RX_dequeue(uint32_t flags, uint32_t *len, uint8_t *data_out);
void EMAC_BD_RX_onErr(uint32_t index);
uint32_t EMAC_BD_Get_CurActive(EMAC_BD_TYPE_e bdt);

#endif
