#ifndef __HCI_UART_LP_H__
#define __HCI_UART_LP_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define HCI_UART_OK 0
#define HCI_UART_INIT_FAULT -1
#define HCI_UART_TX_QUEUE_FULL -2
#define HCI_UART_TX_FAULT -3
#define HCI_UART_WAKEUP_FAULT -4
#define HCI_UART_WAKEUP_TIMEOUT -5

int hci_uart_init(void);
int hci_uart_write(uint8_t *data, uint32_t len);
uint32_t hci_uart_get_rx_count(void);
uint32_t hci_uart_read(uint8_t *data, uint32_t len);
int hci_uart_is_busy(void);
void hci_uart_reset(void);

void hci_uart_ack_received_callback(void); // inside task context
void hci_uart_rx_done_callback(uint8_t *data,
                               uint32_t len); // inside interrupt context

#endif
