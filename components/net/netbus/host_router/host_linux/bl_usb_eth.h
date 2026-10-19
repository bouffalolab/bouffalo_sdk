#ifndef BL_USB_ETH_H_LCMZHRS5
#define BL_USB_ETH_H_LCMZHRS5

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/usb.h>
#include "config.h"
#include "usb_msgs.h"
#include "wifi.h"

#define BL_ETH_TX_TIMEOUT       (30 * HZ)

#define BL_NDEV_FLOW_CTRL_STOP      32
#define BL_NDEV_FLOW_CTRL_RESTART   16

#define BL_TXQ_NDEV_FLOW_CTRL 1

#define BL_TX_DATA_URB 40
#define BL_RX_DATA_URB 40
#define BL_USB_TIMEOUT 100

#define RX_DATA_URB_MAX       32
#define RX_DATA_URB_THRESHOLD (RX_DATA_URB_MAX / 2)
#define TX_DATA_URB_THRESHOLD 32

#define CMD_BUF_LEN 2048
#define NEEDED_HEADROOM_LEN (sizeof(struct urb_context) + sizeof(usb_data_t) + sizeof(int *))

#define ALIGN_LOW(align, addr) ((uintptr_t)(addr) & ~((uintptr_t)(align) - 1))

enum bl_usb_ep {
    BL_USB_EP_IN = 1,
    BL_USB_EP_OUT = 2,
};

enum bl_urb_type {
    BL_URB_CMD = 1,
    BL_URB_PKT = 2,
};

typedef struct urb_context {
    struct sk_buff *skb;
    struct bl_eth_device *dev;
    int data_type;
    unsigned pkt_len;
} urb_ctx_t;

struct bl_eth_device {
    bl_wifi_mode_t mode;
    struct net_device *net;
    struct rtnl_link_stats64 stats;
    u8 sta_mac[ETH_ALEN];
    u8 ap_mac[ETH_ALEN];
    u8 status;

    struct usb_device *udev;
    struct usb_interface *intf;
    u8 rx_ep;
    u8 tx_ep;
    u16 tx_mps;

    struct usb_anchor tx_submitted;
    atomic_t tx_data_urb_pending;
    struct usb_anchor rx_submitted;
    atomic_t rx_data_urb_pending;

    void *cmd_buf;

    u8 bl_processing;
    u8 more_task_flag;
    u8 tx_more_task_flag;

    spinlock_t main_proc_lock;

    u32 tx_credit;

    struct sk_buff_head sk_list;
    struct sk_buff_head rx_sk_list;

    struct work_struct main_work;
    struct workqueue_struct *workqueue;

    bool sta_connected;
};

int bl_write_data_sync(struct bl_eth_device *dev, u8 *data, u32 len, u32 timeout);
int bl_read_data_sync(struct bl_eth_device *dev, u8 *data, u32 len, int *actual_length, u32 timeout);

#endif /* end of include guard: BL_USB_ETH_H_LCMZHRS5 */
