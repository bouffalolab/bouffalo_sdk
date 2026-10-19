#include <linux/completion.h>
#include "msg_handlers.h"
#include "ctl_port.h"
#include "version.h"

static void handle_ap_disconnected_ind(struct bl_eth_device *dev)
{
    netif_carrier_off(dev->net);
    netif_stop_queue(dev->net);
    notify_daemon_simple_event(dev, CTL_PORT_MSG_DISCONNECT);
}

static void handle_sta_ip_update_ind(struct bl_eth_device *dev, const void *data_ptr, const uint16_t data_len)
{
    printk("Update IP\n");
    set_sta_ip(dev, (rnm_sta_ip_update_ind_msg_t *)data_ptr);
    netif_carrier_on(dev->net);
    netif_wake_queue(dev->net);
    dev->sta_connected = true;
    dev->mode = BL_MODE_STA;
}

static void handle_ping(struct bl_eth_device *dev)
{
#if CARD_DEAD_CHECK
    dev->last_ping_recv_time = jiffies;
#endif
}

static bool transparent_proxy_needed(rnm_base_msg_t *cmd)
{
    bool ret = false;

    if (cmd->flags & (RNM_MSG_FLAG_ACK | RNM_MSG_FLAG_ASYNC)) {
        ret = true;
    }
    return ret;
}

void bl_handle_cmd(struct bl_eth_device *dev, const void *data_ptr, const uint16_t data_len)
{
    rnm_base_msg_t *cmd = (rnm_base_msg_t *)data_ptr;
    const uint8_t *payload = data_ptr + sizeof(rnm_base_msg_t);

    switch (cmd->cmd) {
    case BF1B_CMD_STA_CONNECTED_IND:
        netif_carrier_on(dev->net);
        netif_wake_queue(dev->net);
        dev->sta_connected = true;
        printk("AP connected\n");
        break;
    case BF1B_CMD_STA_DISCONNECTED_IND:
        if (dev->sta_connected == true) {
            dev->sta_connected = false;
            printk("AP disconnected\n");
            handle_ap_disconnected_ind(dev);
        }
        break;
    case BF1B_CMD_STA_IP_UPDATE_IND:
        handle_sta_ip_update_ind(dev, data_ptr, data_len);
        break;
    case BF1B_CMD_PING:
        handle_ping(dev);
        break;

    case BF1B_CMD_GET_DEV_VERSION:
        dev->fw_version = *(uint32_t *)payload; 
        dev->fw_version = le32_to_cpu(dev->fw_version);
        complete(&dev->req_done);
        break;

    default:
        break;
    }
    if (transparent_proxy_needed(cmd)) {
        prepare_transparent_msg_rx(cmd, data_len);
    }
}
