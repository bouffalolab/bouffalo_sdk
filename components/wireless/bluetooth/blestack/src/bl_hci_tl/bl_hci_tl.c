#include "bl_hci_tl.h"
#include "bl_hci_uart.h"
#include "errno.h"
#include "hci_driver.h"
#include "bt_log.h"
#include "hci_uart_lp.h"

struct mdev_t *hci_dev;
void bl_hci_recv(struct net_buf *rx_buf);
struct net_buf *bl_hci_alloc_buf(uint8_t rx_type, uint8_t sub_evt,
                                 uint16_t len);

bl_hci_trans_callbacks_t hci_callbacks = {
    bl_hci_recv,
    bl_hci_alloc_buf,
};

int bl_hci_init(char *port) {
  hci_dev = bl_hci_uart_init(&hci_callbacks);
  if (!hci_dev)
    return -ENODEV;

  if (hci_dev->open()) {
    BT_ERR("Fail to open");
    return -1;
  }
  return 0;
}

struct net_buf *bl_hci_alloc_buf(uint8_t rx_type, uint8_t sub_evt,
                                 uint16_t len) {
  struct net_buf *rx_buf = NULL;

  // printf("hci alloc buf, rx_type:%d, len:%d\n", rx_type, len);
  if (rx_type == IND_ACL_DATA) {
    rx_buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_NO_WAIT);
    if (rx_buf)
      bt_buf_set_type(rx_buf, BT_BUF_ACL_IN);
  } else if (rx_type == IND_EVENT) {
    if (sub_evt == BT_HCI_EVT_CMD_COMPLETE || sub_evt == BT_HCI_EVT_CMD_STATUS)
      rx_buf = bt_buf_get_cmd_complete(K_NO_WAIT);
    else
      rx_buf = bt_buf_get_rx(BT_BUF_EVT, K_NO_WAIT);

    if (rx_buf)
      bt_buf_set_type(rx_buf, BT_BUF_EVT);
  }

  if (rx_buf)
    rx_buf->len = len;

  return rx_buf;
}

int bl_hci_send(struct net_buf *buf) {
  uint8_t buf_type;
  int ret = 0;

  if (!buf || !buf->len || !buf->data)
    return -EINVAL;

  uint32_t len = buf->len + 1;
  uint8_t data[len];
  buf_type = bt_buf_get_type(buf);

  switch (buf_type) {
  case BT_BUF_CMD: {
    data[0] = HCI_TYPE_CMD_PKT;
    memcpy(data + 1, buf->data, buf->len);
  } break;
  case BT_BUF_ACL_OUT: {
    data[0] = HCI_TYPE_ACL_PKT;
    memcpy(data + 1, buf->data, buf->len);
  } break;

  default:
    return -EINVAL;
  }

  if (hci_dev) {
    ret = hci_dev->write(data, len);
    if (ret != len) {
      BT_ERR("Fail to write with ret (%d)", ret);
    } else {
      ret = 0;
    }
    return ret;
  } else
    return -ENODEV;
}

void bl_hci_recv(struct net_buf *rx_buf) {
  uint8_t rx_type;

  // printf("%s\n", __func__);

  if (!rx_buf)
    return;

  // printf("%s, rx_buf:0x%x\n", __func__, rx_buf);
  // printf("%s, rx_buf userdata:0x%x\n", __func__, rx_buf->user_data);
  rx_type = bt_buf_get_type(rx_buf);
  // printf("%s, rx_type:%d\n", __func__, rx_type);
  if (rx_type == BT_BUF_EVT && bt_hci_evt_is_prio(rx_buf->data[0])) {
    bt_recv_prio(rx_buf);
  } else {
    hci_driver_enque_recvq(rx_buf);
  }
}

void bl_hci_reset(void) {
    hci_uart_reset();
}