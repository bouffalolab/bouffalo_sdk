#include "bflb_hci_tl.h"
#include "bflb_hci_transport.h"
#include "hci_h4.h"
#include "hci_driver.h"
#include "bt_log.h"
#include "errno.h"

static bflb_hci_trans_callbacks_t hci_callbacks;
static void bflb_hci_recv(struct net_buf *rx_buf);
static struct net_buf *bflb_hci_alloc_buf(uint8_t rx_type, uint8_t sub_evt, uint16_t len);

/* Forward declarations for transport registration functions */
extern int bflb_hci_uart_dma_register(const bflb_hci_trans_callbacks_t *cb);
extern int bflb_hci_uart_int_register(const bflb_hci_trans_callbacks_t *cb);

int bflb_hci_init(char *port)
{
    const struct bflb_hci_transport *transport;
    int ret;

    (void)port;

    hci_callbacks.rx_handler = bflb_hci_recv;
    hci_callbacks.alloc_buf = bflb_hci_alloc_buf;

#if defined(CONFIG_BT_HCI_UART_INT)
    ret = bflb_hci_uart_int_register(&hci_callbacks);
#else
    ret = bflb_hci_uart_dma_register(&hci_callbacks);
#endif

    if (ret) {
        BT_ERR("Transport registration failed: %d", ret);
        return ret;
    }

    transport = bflb_hci_transport_get();
    if (!transport)
        return -ENODEV;

    if (transport->open()) {
        BT_ERR("Fail to open transport");
        return -1;
    }

    return 0;
}

static struct net_buf *bflb_hci_alloc_buf(uint8_t rx_type, uint8_t sub_evt, uint16_t len)
{
    struct net_buf *rx_buf = NULL;

    if (rx_type == H4_IND_ACL_DATA) {
        rx_buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_NO_WAIT);
        if (rx_buf)
            bt_buf_set_type(rx_buf, BT_BUF_ACL_IN);
    } else if (rx_type == H4_IND_EVENT) {
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

int bflb_hci_send(struct net_buf *buf)
{
    const struct bflb_hci_transport *transport = bflb_hci_transport_get();
    uint8_t buf_type;
    int ret;

    if (!buf || !buf->len || !buf->data)
        return -EINVAL;

    uint32_t len = buf->len + 1;
    uint8_t data[len];
    buf_type = bt_buf_get_type(buf);

    switch (buf_type) {
    case BT_BUF_CMD:
        data[0] = H4_IND_COMMAND;
        memcpy(data + 1, buf->data, buf->len);
        break;
    case BT_BUF_ACL_OUT:
        data[0] = H4_IND_ACL_DATA;
        memcpy(data + 1, buf->data, buf->len);
        break;
    default:
        return -EINVAL;
    }

    if (transport) {
        ret = transport->write(data, len);
        if (ret != (int)len) {
            BT_ERR("Fail to write with ret (%d)", ret);
            return -EIO;
        }
        return 0;
    }

    return -ENODEV;
}

static void bflb_hci_recv(struct net_buf *rx_buf)
{
    uint8_t rx_type;

    if (!rx_buf)
        return;

    rx_type = bt_buf_get_type(rx_buf);
    if (rx_type == BT_BUF_EVT && bt_hci_evt_is_prio(rx_buf->data[0])) {
        bt_recv_prio(rx_buf);
    } else {
        hci_driver_enque_recvq(rx_buf);
    }
}

__attribute__((weak)) int bflb_hci_reset(void)
{
    return 0;
}
