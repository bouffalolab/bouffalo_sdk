#ifndef __BL_HCI_WRAPPER_H__
#define __BL_HCI_WRAPPER_H__

#include "net/buf.h"
#include "bluetooth.h"

#if defined(OPTIMIZE_DATA_EVT_FLOW_FROM_CONTROLLER)
struct rx_msg_struct{
    uint8_t pkt_type;
    uint16_t src_id;
    uint8_t *param;
    uint8_t param_len;
}__packed;
#endif

typedef enum {
  DATA_TYPE_COMMAND = 1,
  DATA_TYPE_ACL     = 2,
  DATA_TYPE_SCO     = 3,
  DATA_TYPE_EVENT   = 4
} serial_data_type_t;

uint8_t bl_onchiphci_interface_init(void);

int bl_onchiphci_send_2_controller(struct net_buf *buf);

#if defined(OPTIMIZE_DATA_EVT_FLOW_FROM_CONTROLLER)
void bl_onchiphci_interface_deinit(void);
#endif
#endif //__BL_CONTROLLER_H__