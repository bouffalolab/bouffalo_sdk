#ifndef __BFLB_HCI_UART_COMMON_H
#define __BFLB_HCI_UART_COMMON_H

#define HCI_UART_NAME          "uart1"
#define HCI_UART_BAUDRATE      115200
#define HCI_UART_TXD_PIN       23
#define HCI_UART_RXD_PIN       24
#define HCI_UART_CTS_PIN       25
#define HCI_UART_RTS_PIN       26

#define HCI_UART_DMA_RX_BUF_SIZE  2048
#define HCI_UART_DMA_RX_CH        "dma0_ch2"
#define HCI_UART_DMA_TX_CH        "dma0_ch3"

#define HCI_UART_STREAM_BUF_SIZE  2048
#define HCI_UART_FLOW_CTRL_ENABLE 0

#define PRI_HCI_PROCESS  (configMAX_PRIORITIES - 1)
#define STKSIZE_HCI_PROCESS 2048

#endif /* __BFLB_HCI_UART_COMMON_H */
