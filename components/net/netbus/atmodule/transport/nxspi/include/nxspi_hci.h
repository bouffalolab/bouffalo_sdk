#ifndef __NXSPI_HCI_H__
#define __NXSPI_HCI_H__

void spihci_init(void);
int spi_hci_write(uint8_t *buf, uint32_t len);

#endif
