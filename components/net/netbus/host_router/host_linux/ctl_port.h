#ifndef CTL_PORT_H_MFQCNVLT
#define CTL_PORT_H_MFQCNVLT

#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/mutex.h>

#include "config.h"
#ifdef BL_INTF_SDIO
#include "bl_sdio_eth.h"
#else
#include "bl_usb_eth.h"
#endif
#include "rnm_msg.h"

#include "common.h"

#define CTL_PORT_SESSIONS 2

struct ctl_port_session {
    bool used;
    u32 pid;
    u16 session_id;
    u32 flags;
};

struct blctl_dev {
    struct mutex mutex;
    struct sock *nl_sock;
    struct ctl_port_session sessions[CTL_PORT_SESSIONS];
    u16 last_session_id;

    uint8_t read_buf[CTL_PORT_MSG_LEN_MAX];
};

int bl_register_ctl_port(void);
void bl_release_ctl_port(void);

int notify_daemon_simple_event(struct bl_eth_device *dev, enum ctl_port_event event);
int set_sta_ip(struct bl_eth_device *dev, rnm_sta_ip_update_ind_msg_t *msg);
int notify_process(void);
int prepare_transparent_msg_rx(rnm_base_msg_t *msg, size_t len);

#endif /* end of include guard: CTL_PORT_H_MFQCNVLT */
