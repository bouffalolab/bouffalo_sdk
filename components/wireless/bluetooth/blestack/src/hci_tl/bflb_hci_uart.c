#include "bflb_hci_uart.h"
#include "bl_port.h"
#include "errno.h"
#include "log.h"
#include "types.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "bflb_dma.h"
#include "bflb_uart.h"
#include "bt_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#define PRI_HCI_PROCESS (configMAX_PRIORITIES - 1)
#define STKSIZE_HCI_PROCESS 1024

static uint8_t uartInitialized = 0;
static struct k_thread uart_read_thread;
static struct mdev_t uart_dev;

struct bflb_device_s *uartx;

const static bflb_hci_trans_callbacks_t *hciTranCb;

uint64_t baud_rate = 115200;
UART_Context uart_ctx;

StreamBufferHandle_t xStreamBufferRX = NULL;

u8_t get_uart_rxstate(void) { return uart_ctx.rxState; }

static void UartReadThread(void *args) {
  int bytesRead = 0;
  uint16_t len;

  do {
    if (uart_ctx.rxState != UART_GET_BUFFER) {
      bytesRead = xStreamBufferReceive(xStreamBufferRX, uart_ctx.ptr, uart_ctx.maxReadLen, portMAX_DELAY);
      BT_DBG("uart_ctx.maxReadLen = %d, bytesRead = %d\r\n", uart_ctx.maxReadLen, bytesRead);
      uart_ctx.ptr += bytesRead;
      if (bytesRead != uart_ctx.maxReadLen) {
        uart_ctx.maxReadLen = (uint16_t)(uart_ctx.maxReadLen) - bytesRead;
        continue;
      }
    }

    switch (uart_ctx.rxState) {
    case UART_GET_TYPE:
      /* Get the type of buffer */
      if (uart_ctx.buffType == IND_EVENT) {
        uart_ctx.headerLen = 2;
      } else if (uart_ctx.buffType == IND_ACL_DATA) {
        uart_ctx.headerLen = 4;
      } else {
        BT_ERR("Invalid buffType(%d)", uart_ctx.buffType);
        uart_ctx.buffType = 0;
        uart_ctx.ptr = &(uart_ctx.buffType);
        uart_ctx.maxReadLen = 1;
        uart_ctx.rxState = UART_GET_TYPE;
        break;
      }

      /* Set up for the next state */
      uart_ctx.ptr = &uart_ctx.header[0];
      uart_ctx.maxReadLen = uart_ctx.headerLen;
      uart_ctx.rxState = UART_GET_HEADER;
      break;

    case UART_GET_HEADER:
      /*Get buffer and store header info */
      uart_ctx.rxState = UART_GET_BUFFER;
    case UART_GET_BUFFER:
      /* Calculate buffer length based on type */
      switch (uart_ctx.buffType) {
      case IND_ACL_DATA:
        len =
            (uint16_t)(LEtoHost16(&(uart_ctx.header)[2])) + uart_ctx.headerLen;
        uart_ctx.rxBuf =
            (struct net_buf *)hciTranCb->alloc_buf(IND_ACL_DATA, 0, len);

        if (uart_ctx.rxBuf) {
          uart_ctx.maxReadLen = LEtoHost16(&(uart_ctx.header)[2]);
          uart_ctx.rxBuf->len = len;
          uart_ctx.ptr = (uint8_t *)(uart_ctx.rxBuf->data);
        } else {
          BT_ERR("Fail to allocate rx buffer for rx acl data\r\n");
          uart_ctx.buffType = 0;
          uart_ctx.ptr = &(uart_ctx.buffType);
          uart_ctx.maxReadLen = 1;
          uart_ctx.rxState = UART_GET_TYPE;
        }
        break;
      case IND_EVENT:
        len = (uint16_t)(uart_ctx.header[1] + uart_ctx.headerLen);
        uart_ctx.rxBuf = (struct net_buf *)(hciTranCb->alloc_buf(
            IND_EVENT, uart_ctx.header[0], len));
        if (uart_ctx.rxBuf) {
          uart_ctx.maxReadLen = uart_ctx.header[1];
          uart_ctx.rxBuf->len = len;
          uart_ctx.ptr = (uint8_t *)(uart_ctx.rxBuf->data);
        } else {
          BT_ERR("%s,Fail to allocate rx buffer for event\r\n", __func__);
          uart_ctx.buffType = 0;
          uart_ctx.ptr = &(uart_ctx.buffType);
          uart_ctx.maxReadLen = 1;
          uart_ctx.rxState = UART_GET_TYPE;
          break;
        }
        break;

      default: {
        BT_ERR("wrong type (%d)\r\n", uart_ctx.buffType);
        uart_ctx.buffType = 0;
        uart_ctx.ptr = &(uart_ctx.buffType);
        uart_ctx.maxReadLen = 1;
        uart_ctx.rxState = UART_GET_TYPE;
        break;
      }
      }

      memcpy(uart_ctx.ptr, &uart_ctx.header, uart_ctx.headerLen);

      uart_ctx.ptr += uart_ctx.headerLen;
      uart_ctx.rxState = UART_GET_DATA;
      bytesRead = 1;
      break;

    case UART_GET_DATA:
      if (uart_ctx.buffType == IND_EVENT) {
        hciTranCb->rx_handler(uart_ctx.rxBuf);
      } else if (uart_ctx.buffType == IND_ACL_DATA) {
        hciTranCb->rx_handler(uart_ctx.rxBuf);
      }
      /* Reset for next read*/
      uart_ctx.buffType = 0;
      uart_ctx.ptr = &(uart_ctx.buffType);
      uart_ctx.maxReadLen = 1;
      uart_ctx.rxState = UART_GET_TYPE;
      break;
    }
  } while (bytesRead);
}

static void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uartx);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (intstatus & UART_INTSTS_RX_FIFO) {
        BT_DBG("rx fifo\r\n");
        while (bflb_uart_rxavailable(uartx) && !xStreamBufferIsFull(xStreamBufferRX)) {
          uint8_t data = bflb_uart_getchar(uartx);
          xStreamBufferSendFromISR(xStreamBufferRX, &data, 1, &xHigherPriorityTaskWoken);
        }
    }
    if (intstatus & UART_INTSTS_RTO) {
        bflb_uart_int_clear(uartx, UART_INTCLR_RTO);
        BT_DBG("rto\r\n");
        while (bflb_uart_rxavailable(uartx) && !xStreamBufferIsFull(xStreamBufferRX)) {
          uint8_t data = bflb_uart_getchar(uartx);
          xStreamBufferSendFromISR(xStreamBufferRX, &data, 1, &xHigherPriorityTaskWoken);
        }
    }
    if (intstatus & UART_INTSTS_TX_END) {
        bflb_uart_int_clear(uartx, UART_INTCLR_TX_END);
        BT_DBG("tx end\r\n");
    }
    if (intstatus & UART_INTSTS_RX_END) {
        bflb_uart_int_clear(uartx, UART_INTCLR_RX_END);
        BT_DBG("rx end\r\n");
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static int32_t hci_uart_open(void) {
  if (uartInitialized == true) {
    return 0;
  }
  uartInitialized = true;

  xStreamBufferRX = xStreamBufferCreate(512, 1);
  if (xStreamBufferRX == NULL) {
    BT_DBG("create stream buffer failed\r\n");
    return -1;
  }

  uart_ctx.maxReadLen = 1;
  uart_ctx.buffType = 0;
  uart_ctx.ptr = &uart_ctx.buffType;
  uart_ctx.rxState = UART_GET_TYPE;

  uartx = bflb_device_get_by_name("uart1");
  struct bflb_uart_config_s cfg;

  cfg.baudrate = 115200;
  cfg.data_bits = UART_DATA_BITS_8;
  cfg.stop_bits = UART_STOP_BITS_1;
  cfg.parity = UART_PARITY_NONE;
  cfg.flow_ctrl = 0;
  cfg.tx_fifo_threshold = 7;
  cfg.rx_fifo_threshold = 7;
  cfg.bit_order = UART_LSB_FIRST;
  bflb_uart_init(uartx, &cfg);

  bflb_uart_rxint_mask(uartx, false);
  bflb_irq_attach(uartx->irq_num, uart_isr, NULL);
  bflb_irq_enable(uartx->irq_num);

  /* Create thread for read */
  if (k_thread_create(&uart_read_thread, "uart_read_thread",
                      STKSIZE_HCI_PROCESS, UartReadThread, PRI_HCI_PROCESS)) {
    BT_ERR("Fail to Create UartReadThread\r\n");
  } else
    BT_DBG("Create UartReadThread successfully\r\n");

  return 0;
}

static int32_t hci_uart_close(void) {
  
  int ret = 0;

  if (!uartInitialized) {
    BT_ERR("ERROR!!!UART is not open");
    return -1;
  }

  vStreamBufferDelete(xStreamBufferRX);

  k_thread_delete(&uart_read_thread);

  uartInitialized = false;

  return ret;
}

static int32_t hci_uart_write(uint8_t *buf, uint32_t nbytes) {

  if (!uartInitialized) {
    BT_ERR("ERROR!!!uart is not open");
    return -1;
  }

  if (!buf) {
    BT_ERR("ERROR!!!buf is NULL");
    return -1;
  }

  if (bflb_uart_put_block(uartx, buf, nbytes)) {
    return -1;
  } else {
    return nbytes;
  }

}

struct mdev_t *bflb_hci_uart_init(const bflb_hci_trans_callbacks_t *cb) {
  uart_dev.name = "uart";
  uart_dev.open = hci_uart_open;
  uart_dev.close = hci_uart_close;
  uart_dev.read = NULL;
  uart_dev.write = hci_uart_write;

  hciTranCb = cb;
  BT_DBG("%s, hciTranCb:0x%p, hciTranCb->alloc_buf:0x%p\r\n", __func__, hciTranCb,
         hciTranCb->alloc_buf);
  return &uart_dev;
}
