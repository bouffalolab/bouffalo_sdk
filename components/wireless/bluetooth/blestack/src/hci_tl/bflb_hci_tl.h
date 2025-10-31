#ifndef __BLFB_HCI_WRAPPER_H__
#define __BLFB_HCI_WRAPPER_H__

#include "buf.h"

#define HCI_TYPE_CMD_PKT 0x01
#define HCI_TYPE_ACL_PKT 0x02

int bflb_hci_init(char *port);
int bflb_hci_send(struct net_buf *buf);
int bflb_hci_reset(void);

#endif
