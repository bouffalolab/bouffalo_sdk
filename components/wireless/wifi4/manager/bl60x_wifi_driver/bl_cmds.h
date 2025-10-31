#ifndef __RWNX_CMDS_H__
#define __RWNX_CMDS_H__

#include "list.h"

#include "lmac_types.h"
#include "lmac_msg.h"
#include "ipc_shared.h"
#include "bflb_os_private.h"

#define BIT(n) (0x1U << (n))

#define RWNX_CMD_FLAG_NONBLOCK      BIT(0)
#define RWNX_CMD_FLAG_REQ_CFM       BIT(1)
#define RWNX_CMD_FLAG_WAIT_PUSH     BIT(2)
#define RWNX_CMD_FLAG_WAIT_ACK      BIT(3)
#define RWNX_CMD_FLAG_WAIT_CFM      BIT(4)
#define RWNX_CMD_FLAG_DONE          BIT(5)

/**
 ****************************************************************************************
 *
 * @file bl_cmds.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#define RWNX_CMD_WAIT_COMPLETE(flags) \
        (!(flags & (RWNX_CMD_FLAG_WAIT_ACK | RWNX_CMD_FLAG_WAIT_CFM)))

//TODO fix the follow ugly declare
struct bl_hw;
struct bl_cmd;
typedef int (*msg_cb_fct)(struct bl_hw *bl_hw, struct bl_cmd *cmd, struct ipc_e2a_msg *msg);

#define RWNX_CMD_MAX_QUEUED         8
struct bl_cmd {
    struct list_head list;
    ke_msg_id_t id;
    ke_msg_id_t reqid;
    struct lmac_msg *a2e_msg;
    char            *e2a_msg;
    u32 tkn;
    u16 flags;

    BL_EventGroup_t complete;
    u32 result;
};

enum bl_cmd_mgr_state {
     RWNX_CMD_MGR_STATE_DEINIT,
     RWNX_CMD_MGR_STATE_INITED,
     RWNX_CMD_MGR_STATE_CRASHED,
};

struct bl_cmd_mgr {
    enum bl_cmd_mgr_state state;
    u32 next_tkn;
    u32 queue_sz;
    u32 max_queue_sz;

    struct list_head cmds;
    BL_Mutex_t lock;

    int  (*queue)(struct bl_cmd_mgr *, struct bl_cmd *);
    int  (*llind)(struct bl_cmd_mgr *, struct bl_cmd *);
    int  (*msgind)(struct bl_cmd_mgr *, struct ipc_e2a_msg *, msg_cb_fct);
    void (*print)(struct bl_cmd_mgr *);
    void (*drain)(struct bl_cmd_mgr *);
};

void bl_cmd_mgr_init(struct bl_cmd_mgr *cmd_mgr);

#endif
