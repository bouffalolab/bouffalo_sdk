#ifndef __BFLB_HCI_UART_H
#define __BFLB_HCI_UART_H

#include <stdint.h>
#include "buf.h"

/**************************************************************************************************************

  MACRO's Declerations

 ****************************************************************************************************************/
#define BT_STATUS_HW_READY_IND                0xF1
#define BT_STATUS_HW_FAILED_IND               0xF2
#define BT_STATUS_SW_READY_IND                0xF3
#define BT_STATUS_HCI_EVENT_RECEIVED_IND      0xF4
#define BT_STATUS_HCI_ACL_RECEIVED_IND        0xF5

/* Receive States */
typedef enum
{
    UART_GET_TYPE      = 0x01,
    UART_GET_SEQ       = 0x02,
    UART_GET_HEADER    = 0x03,
    UART_GET_BUFFER    = 0x04,
    UART_GET_DATA      = 0x05,
}uart_recv_state_t;

/* Packet types or indicators */
#define IND_COMMAND         (uint8_t)0x01
#define IND_ACL_DATA        (uint8_t)0x02
#define IND_SCO_DATA        (uint8_t)0x03
#define IND_EVENT           (uint8_t)0x04
#define IND_ERROR_MSG       (uint8_t)0x05
#define IND_NEGOTIATION     (uint8_t)0x06

/* Little Endian to Host integer format conversion macros */
#define LEtoHost16(ptr)  (uint16_t)( ((uint16_t) *((uint8_t*)(ptr)+1) << 8) | \
                                ((uint16_t) *((uint8_t*)(ptr))) )

#define LEtoHost32(ptr)  (uint32_t)( ((uint32_t) *((uint8_t*)(ptr)+3) << 24) | \
                                ((uint32_t) *((uint8_t*)(ptr)+2) << 16) | \
                                ((uint32_t) *((uint8_t*)(ptr)+1) << 8)  | \
                                ((uint32_t) *((uint8_t*)(ptr))) )

#define BT_STATUS_SUCCESS  0
#define BT_STATUS_NO_RESOURCES  12

struct mdev_t
{
    char *name;
    int32_t(*open) (void);
    int32_t(*close) (void);
    int32_t(*read) (uint8_t * buf, uint32_t nbytes, uint32_t flags);
    int32_t(*write) (uint8_t * buf, uint32_t nbytes);
};

typedef struct _UART_Context {
  /* RX State variables */
  uint16_t         maxReadLen;
  uint16_t         totalLen;
  uint8_t          buffType;
  uint8_t          *ptr;
  uint8_t          header[4];
  uint8_t          headerLen;
  struct net_buf *rxBuf;
  uint8_t          rxState;

  /* TX State variables */
  uint8_t        *txPacket;

}UART_Context;

typedef void (*bflb_hci_rx_ptr)(struct net_buf* rx_buf);
typedef struct net_buf* (*bflb_hci_alloc_buf_ptr)(uint8_t rx_type, uint8_t sub_evt, uint16_t len);

typedef struct _bflb_hci_trans_callbacks_t
{
  bflb_hci_rx_ptr            rx_handler;
  bflb_hci_alloc_buf_ptr     alloc_buf;
}bflb_hci_trans_callbacks_t;

/**************************************************************************************************************

  Structure Definitions

 ****************************************************************************************************************/
struct mdev_t* bflb_hci_uart_init(const bflb_hci_trans_callbacks_t *cb);

#endif
