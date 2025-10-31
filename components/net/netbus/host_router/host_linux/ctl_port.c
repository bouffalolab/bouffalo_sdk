#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include "common.h"
#include "main.h"
#include "ctl_port.h"
#ifdef BL_INTF_SDIO
#include "sdio_msg_handlers.h"
#include "sdio_msgs.h"
#else
#include "usb_msg_handlers.h"
#include "usb_msgs.h"
#endif

#define BLCTL_PROTO NETLINK_USERSOCK

static struct blctl_dev ctl_dev;

static int nl_msg_out(struct sock *sock, const void *data, size_t len, u32 pid);
static int nl_bcast(struct blctl_dev *cd, const void *data, size_t len);

#define MSG_FILL_HDR(m_msg, m_cmd) \
    memset(&(m_msg), 0, sizeof(m_msg)); \
    memcpy((m_msg).hdr.magic, CTL_PORT_MSG_MAGIC, sizeof((m_msg).hdr.magic)); \
    (m_msg).hdr.type = m_cmd; \
    (m_msg).hdr.length = sizeof(m_msg);

int notify_daemon_simple_event(struct bl_eth_device *dev, enum ctl_port_event event)
{
    struct blctl_dev *cd = &ctl_dev;
    ctl_port_msg_hdr_t m_, *m = &m_;

    memset(m, 0, sizeof(*m));
    memcpy(m->magic, CTL_PORT_MSG_MAGIC, sizeof(m->magic));
    m->type = event;
    m->length = sizeof(*m);

    return nl_bcast(cd, m, sizeof(*m));
}

int set_sta_ip(struct bl_eth_device *dev, rnm_sta_ip_update_ind_msg_t *msg)
{
    struct blctl_dev *cd = &ctl_dev;
    ctl_port_msg_ip_update_t m_, *m = &m_;

    MSG_FILL_HDR(*m, CTL_PORT_MSG_IP_UPDATE);
    // XXX note the elements should be the same
    memcpy(&m->ip_info, &msg->ip4_addr, sizeof(m->ip_info));

    return nl_bcast(cd, m, sizeof(*m));
}

static int nl_bcast(struct blctl_dev *cd, const void *data, size_t len)
{
    int i;

    mutex_lock(&cd->mutex);
    for (i = 0; i < CTL_PORT_SESSIONS; ++i) {
        struct ctl_port_session *session = &cd->sessions[i];
        if (session->used) {
            nl_msg_out(cd->nl_sock, data, len, session->pid);
        }
    }
    mutex_unlock(&cd->mutex);
    return 0;
}

int prepare_transparent_msg_rx(rnm_base_msg_t *msg, size_t len)
{
    struct blctl_dev *cd = &ctl_dev;
    ctl_port_msg_hdr_t *hdr = (ctl_port_msg_hdr_t *)ctl_dev.read_buf;
    uint16_t read_buf_dlen = CTL_PORT_MSG_HDR_LEN + len;
    int i;

    memset(hdr, 0, sizeof(*hdr));
    memcpy(hdr->magic, CTL_PORT_MSG_MAGIC, sizeof(hdr->magic));
    hdr->type = CTL_PORT_MSG_TRANSPARENT_DEVICE2HOST;
    hdr->length = read_buf_dlen;
    memcpy(hdr->payload, msg, len);

    if (msg->flags & RNM_MSG_FLAG_ASYNC) {
        return nl_bcast(cd, hdr, read_buf_dlen);
    }

    mutex_lock(&cd->mutex);
    for (i = 0; i < CTL_PORT_SESSIONS; ++i) {
        struct ctl_port_session *session = &cd->sessions[i];
        if (session->used && session->session_id == msg->session_id) {
            nl_msg_out(cd->nl_sock, hdr, read_buf_dlen, session->pid);
        }
    }
    mutex_unlock(&cd->mutex);
    return 0;
}

static int parse_cmd(const u8 *payload, const u16 len)
{
    const rnm_base_msg_t *cmd;
    struct bl_eth_device *dev;
    bl_wifi_mode_t old_mode = BL_MODE_MAX;
    int xfer_ret = -1;

    cmd = (rnm_base_msg_t *)payload;

    mutex_lock(&gl_dev.mutex);
    dev = gl_dev.eth_dev;
    if (!dev) {
        mutex_unlock(&gl_dev.mutex);
        return -1;
    }

    switch (cmd->cmd) {
        case BF1B_CMD_STA_CONNECT:
            if (!(xfer_ret = bl_mode_xfer(&dev->mode, BL_MODE_STA, &old_mode))) {
                // empty
            }
            bl_change_eth_mac(dev);
            break;
        case BF1B_CMD_STA_DISCONNECT:
            if (!(xfer_ret = bl_mode_xfer(&dev->mode, BL_MODE_NONE, &old_mode))) {
                // empty
            }
            break;
        case BF1B_CMD_AP_START:
            if (!(xfer_ret = bl_mode_xfer(&dev->mode, BL_MODE_AP, &old_mode))) {
                netif_carrier_on(dev->net);
                netif_wake_queue(dev->net);
            }
            bl_change_eth_mac(dev);
            break;
        case BF1B_CMD_AP_STOP:
            if (!(xfer_ret = bl_mode_xfer(&dev->mode, BL_MODE_NONE, &old_mode))) {
                netif_carrier_off(dev->net);
                netif_stop_queue(dev->net);
            }
            bl_change_eth_mac(dev);
            break;
        default:
            break;
    }

    if (!xfer_ret) {
        printk("mode %s->%s\n", bl_mode_to_str(old_mode), bl_mode_to_str(dev->mode));
    }
    mutex_unlock(&gl_dev.mutex);

    return 0;
}

static int nl_msg_out(struct sock *sock, const void *data, size_t len, u32 id)
{
    int ret;
    struct sk_buff *skb;
    struct nlmsghdr *nlh;

    if (!(skb = nlmsg_new(len, GFP_KERNEL))) {
        return -1;
    }

    nlh = nlmsg_put(skb, 0, 0, NLMSG_DONE, len, 0);
    NETLINK_CB(skb).dst_group = 0;
    memcpy(nlmsg_data(nlh), data, len);

    ret = nlmsg_unicast(sock, skb, id);
    if (ret) {
        pr_err("error sending nl msg, ret %d\n", ret);
    }

    return ret;
}

void ctl_port_new_session(u32 pid)
{
    struct blctl_dev *cd = &ctl_dev;
    int i;
    int slot_idx;
    ctl_port_msg_conn_rsp_t rsp;

    MSG_FILL_HDR(rsp, CTL_PORT_MSG_CONNECT_RSP);
    rsp.success = 1;

    mutex_lock(&cd->mutex);
    for (i = 0; i < CTL_PORT_SESSIONS; ++i) {
        if (!cd->sessions[i].used) {
            break;
        }
    }
    if (i == CTL_PORT_SESSIONS) {
        mutex_unlock(&cd->mutex);
        printk("new_session: no more slot");
        rsp.success = 0;
        nl_msg_out(cd->nl_sock, &rsp, sizeof(rsp), pid);
        return;
    }
    slot_idx = i;

    while (1) {
        ++cd->last_session_id;
        for (i = 0; i < CTL_PORT_SESSIONS; ++i)
            if (cd->sessions[i].used &&
                    cd->sessions[i].session_id == cd->last_session_id)
                continue;
        break;
    }

    rsp.session_id = cd->last_session_id;
    if (nl_msg_out(cd->nl_sock, &rsp, sizeof(rsp), pid) == 0) {
        struct ctl_port_session *session = &cd->sessions[slot_idx];
        session->used = true;
        session->pid = pid;
        session->session_id = cd->last_session_id;
        session->flags = 0;
    }
    mutex_unlock(&cd->mutex);
}

void ctl_port_del_session(u32 pid)
{
    struct blctl_dev *cd = &ctl_dev;
    int i;
    ctl_port_msg_conn_rsp_t rsp;

    MSG_FILL_HDR(rsp, CTL_PORT_MSG_DISCONNECT_RSP);
    rsp.success = 1;

    mutex_lock(&cd->mutex);
    for (i = 0; i < CTL_PORT_SESSIONS; ++i) {
        struct ctl_port_session *session = &cd->sessions[i];
        if (session->used && session->pid == pid) {
            nl_msg_out(cd->nl_sock, &rsp, sizeof(rsp), pid);
            session->used = false;
            mutex_unlock(&cd->mutex);
            return;
        }
    }

    if (i == CTL_PORT_SESSIONS) {
        rsp.success = 0;
        nl_msg_out(cd->nl_sock, &rsp, sizeof(rsp), pid);
    }
}

void ctl_port_del_all_session(u32 pid)
{
    struct blctl_dev *cd = &ctl_dev;
    int i;
    ctl_port_msg_conn_rsp_t rsp;

    MSG_FILL_HDR(rsp, CTL_PORT_MSG_DISCONNECT_ALL_RSP);
    rsp.success = 1;

    mutex_lock(&cd->mutex);
    for (i = 0; i < CTL_PORT_SESSIONS; ++i) {
        struct ctl_port_session *session = &cd->sessions[i];
        session->used = false;
    }
    mutex_unlock(&cd->mutex);
    nl_msg_out(cd->nl_sock, &rsp, sizeof(rsp), pid);
}

static void netlink_recv(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    u32 pid;
    const void *msg;
    size_t msg_len;
    const ctl_port_msg_hdr_t *cmsg;

    nlh = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid;
    msg = nlmsg_data(nlh);
    msg_len = nlmsg_len(nlh);

    cmsg = msg;
    if (!(msg_len >= CTL_PORT_MSG_HDR_LEN &&
          !memcmp(cmsg->magic, CTL_PORT_MSG_MAGIC, sizeof(cmsg->magic)))) {
        pr_warn("not our nl msg\n");
        return;
    }

    switch (cmsg->type) {
    case CTL_PORT_MSG_CONNECT_REQ:
        ctl_port_new_session(pid);
        break;
    case CTL_PORT_MSG_DISCONNECT_REQ:
        ctl_port_del_session(pid);
        break;
    case CTL_PORT_MSG_DISCONNECT_ALL_REQ:
        ctl_port_del_all_session(pid);
        break;
    case CTL_PORT_MSG_TRANSPARENT_HOST2DEVICE:
        push_host2device_msg(cmsg->payload, cmsg->length - sizeof(*cmsg));
        parse_cmd(cmsg->payload, cmsg->length - sizeof(*cmsg));
        break;
    default:
        break;
    }
}

int bl_register_ctl_port()
{
    struct netlink_kernel_cfg cfg = {
        .input = netlink_recv,
    };

    mutex_init(&ctl_dev.mutex);

    ctl_dev.nl_sock = netlink_kernel_create(&init_net, BLCTL_PROTO, &cfg);
    if (!ctl_dev.nl_sock) {
        pr_err("error creating nl sock\n");
        return -1;
    }

    return 0;
}

void bl_release_ctl_port()
{
    if (ctl_dev.nl_sock) {
        netlink_kernel_release(ctl_dev.nl_sock);
        ctl_dev.nl_sock = NULL;
    }
}
