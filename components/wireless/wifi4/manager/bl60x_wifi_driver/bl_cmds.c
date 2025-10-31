#include <stdio.h>
#include <string.h>

#include <bflb_os_private.h>
#include "bl_cmds.h"
#include "bl_utils.h"
#include "bl_strs.h"

#undef bflb_os_log_printf
#define bflb_os_log_printf(...) \
    do {                      \
    } while (0)

static void cmd_dump(const struct bl_cmd *cmd)
{
    bflb_os_log_debug("tkn[%d]  flags:%04x  result:%3d  cmd:%4d-%-24s - reqcfm(%4d-%-s)\n",
           cmd->tkn, cmd->flags, cmd->result, cmd->id, RWNX_ID2STR(cmd->id),
           cmd->reqid, cmd->reqid != (lmac_msg_id_t)-1 ? RWNX_ID2STR(cmd->reqid) : "none");
}

static void cmd_complete(struct bl_cmd_mgr *cmd_mgr, struct bl_cmd *cmd)
{
    bflb_os_log_debug("[CMDS] CMD complete: %p\r\n", cmd);

    cmd_mgr->queue_sz--;
    list_del(&cmd->list);
    cmd->flags |= RWNX_CMD_FLAG_DONE;
    if (cmd->flags & RWNX_CMD_FLAG_NONBLOCK) {
        bflb_os_log_debug("[CMDS] NONBLOCK CMD, free now %p\r\n", cmd);
        bflb_os_free(cmd);
    } else {
        if (RWNX_CMD_WAIT_COMPLETE(cmd->flags)) {
            bflb_os_log_debug("[CMDS] BLOCK EVENT is ready, complete now %p\r\n", cmd);
            cmd->result = 0;
            bflb_os_event_group_send(cmd->complete, 0x1);
        }
    }
}

static int cmd_mgr_queue(struct bl_cmd_mgr *cmd_mgr, struct bl_cmd *cmd)
{
    struct bl_hw *bl_hw = container_of(cmd_mgr, struct bl_hw, cmd_mgr);
    struct bl_cmd *last;
    bool defer_push = false;
    uint32_t e;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    bflb_os_mutex_lock(cmd_mgr->lock);

    if (cmd_mgr->state == RWNX_CMD_MGR_STATE_CRASHED) {
        bflb_os_log_debug("cmd queue crashed\r\n");
        cmd->result = EPIPE;
        RWNX_DBG(RWNX_FN_LEAVE_STR);
        bflb_os_mutex_unlock(cmd_mgr->lock);
        return -EPIPE;
    }

    if (!list_empty(&cmd_mgr->cmds)) {
        if (cmd_mgr->queue_sz == cmd_mgr->max_queue_sz) {
            bflb_os_log_debug("Too many cmds (%d) already queued\r\n",
                   cmd_mgr->max_queue_sz);
            cmd->result = ENOMEM;
            RWNX_DBG(RWNX_FN_LEAVE_STR);
            bflb_os_mutex_unlock(cmd_mgr->lock);
            return -ENOMEM;
        }
        last = list_entry(cmd_mgr->cmds.prev, struct bl_cmd, list);
        //FIXME comment out the following code block, since it is buggy
#if 1
        if (last->flags & (RWNX_CMD_FLAG_WAIT_ACK | RWNX_CMD_FLAG_WAIT_PUSH)) {
#if 0 // queue even NONBLOCK command.
            if (cmd->flags & RWNX_CMD_FLAG_NONBLOCK) {
                printk(KERN_CRIT"cmd queue busy\n");
                cmd->result = -EBUSY;
                spin_unlock_bh(&cmd_mgr->lock);
                RWNX_DBG(RWNX_FN_LEAVE_STR);
                return -EBUSY;
            }
#endif
            cmd->flags |= RWNX_CMD_FLAG_WAIT_PUSH;
            defer_push = true;
        }
#endif
    }

    cmd->flags |= RWNX_CMD_FLAG_WAIT_ACK;
    if (cmd->flags & RWNX_CMD_FLAG_REQ_CFM)
        cmd->flags |= RWNX_CMD_FLAG_WAIT_CFM;

    cmd->tkn    = cmd_mgr->next_tkn++;
    cmd->result = EINTR;

    if (!(cmd->flags & RWNX_CMD_FLAG_NONBLOCK)) {
        cmd->complete = bflb_os_event_group_create();
    }


    list_add_tail(&cmd->list, &cmd_mgr->cmds);
    cmd_mgr->queue_sz++;
    bflb_os_mutex_unlock(cmd_mgr->lock);

    if (!defer_push) {
        bflb_os_log_debug("pushing CMD, param_len is %d...\r\n", cmd->a2e_msg->param_len);
        ipc_host_msg_push(bl_hw->ipc_env, cmd, sizeof(struct lmac_msg) + cmd->a2e_msg->param_len);
        bflb_os_free(cmd->a2e_msg);
    }

    if (!(cmd->flags & RWNX_CMD_FLAG_NONBLOCK)) {
        bflb_os_log_debug("Waiting ACK...\r\n");
        e = bflb_os_event_group_wait(cmd->complete,
                                             (1 << 0),
                                             BL_OS_TRUE,
                                             BL_OS_FALSE,
                                             BL_OS_WAITING_FOREVER);
        if (e & (1 << 0)) {
            bflb_os_log_debug("cmd OK\r\n");
        } else {
            bflb_os_log_debug("cmd timed-out\r\n");
            cmd_dump(cmd);
            bflb_os_mutex_lock(cmd_mgr->lock);
            cmd_mgr->state = RWNX_CMD_MGR_STATE_CRASHED;
            if (!(cmd->flags & RWNX_CMD_FLAG_DONE)) {
                cmd->result = ETIMEDOUT;
                cmd_complete(cmd_mgr, cmd);
            }
            bflb_os_mutex_unlock(cmd_mgr->lock);
        }
        bflb_os_event_group_delete(cmd->complete);//detach after block read
    } else {
        bflb_os_log_debug("No need to wait for CMD\r\n");
        cmd->result = 0;
    }
    RWNX_DBG(RWNX_FN_LEAVE_STR);
    return 0;
}

static void cmd_mgr_print(struct bl_cmd_mgr *cmd_mgr)
{
    struct bl_cmd *cur;

    bflb_os_mutex_lock(cmd_mgr->lock);
    RWNX_DBG("q_sz/max: %2d / %2d - next tkn: %d\r\n",
             cmd_mgr->queue_sz, cmd_mgr->max_queue_sz,
             cmd_mgr->next_tkn);
    list_for_each_entry(cur, &cmd_mgr->cmds, list) {
        cmd_dump(cur);
    }

    bflb_os_mutex_unlock(cmd_mgr->lock);
}

static void cmd_mgr_drain(struct bl_cmd_mgr *cmd_mgr)
{
    struct bl_cmd *cur, *nxt;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    bflb_os_mutex_lock(cmd_mgr->lock);
    list_for_each_entry_safe(cur, nxt, &cmd_mgr->cmds, list) {
        list_del(&cur->list);
        cmd_mgr->queue_sz--;
        if (!(cur->flags & RWNX_CMD_FLAG_NONBLOCK)) {
            bflb_os_event_group_send(cur->complete, 0x1);
        }
    }
    bflb_os_mutex_unlock(cmd_mgr->lock);
}

static int cmd_mgr_llind(struct bl_cmd_mgr *cmd_mgr, struct bl_cmd *cmd)
{
    struct bl_cmd *cur, *acked = NULL, *next = NULL;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    bflb_os_mutex_lock(cmd_mgr->lock);
    list_for_each_entry(cur, &cmd_mgr->cmds, list) {
        bflb_os_log_debug("Search cmds list cmd %p vs cur %p...\r\n", cmd, cur);
        if (!acked) {
            bflb_os_log_debug("Finding acked...\r\n");
            if (cur->tkn == cmd->tkn) {
                bflb_os_log_debug("Found acked\r\n");
                if (WARN_ON_ONCE(cur != cmd)) {
                    cmd_dump(cmd);
                }
                acked = cur;
                continue;
            }
        }
        if (cur->flags & RWNX_CMD_FLAG_WAIT_PUSH) {
                bflb_os_log_debug("Found WAIT_PUSH\r\n");
                next = cur;
                break;
        }
    }
    if (!acked) {
        bflb_os_log_debug("Error: acked cmd not found\r\n");
    } else {
        cmd->flags &= ~RWNX_CMD_FLAG_WAIT_ACK;
        if (RWNX_CMD_WAIT_COMPLETE(cmd->flags)) {
            bflb_os_log_debug("[BUG] should cmd complete allowed here?\r\n");//FIXME attention
            cmd_complete(cmd_mgr, cmd);// XXX potential buggy
        } else {
            bflb_os_log_debug("[IPC] Wait Until ACKED for cmd %p, flags %08X\r\n", cmd, cmd->flags);
        }
    }
    if (next) {
        struct bl_hw *bl_hw = container_of(cmd_mgr, struct bl_hw, cmd_mgr);
        next->flags &= ~RWNX_CMD_FLAG_WAIT_PUSH;
        bflb_os_log_debug("Pushing new CMD in llind...\r\n");
        ipc_host_msg_push(bl_hw->ipc_env, next,
                          sizeof(struct lmac_msg) + next->a2e_msg->param_len);
        bflb_os_free(next->a2e_msg);
    }
    bflb_os_mutex_unlock(cmd_mgr->lock);

    return 0;
}

static int cmd_mgr_msgind(struct bl_cmd_mgr *cmd_mgr, struct ipc_e2a_msg *msg, msg_cb_fct cb)
{
    struct bl_hw *bl_hw = container_of(cmd_mgr, struct bl_hw, cmd_mgr);
    struct bl_cmd *cmd;
    bool found = false;

    bflb_os_mutex_lock(cmd_mgr->lock);
    list_for_each_entry(cmd, &cmd_mgr->cmds, list) {
        if (cmd->reqid == msg->id &&
            (cmd->flags & RWNX_CMD_FLAG_WAIT_CFM)) {
            bflb_os_log_debug("[IPC] Found cb %p for cmd %p , msg id %08X\r\n", cb, cmd, msg->id);
            if (!cb || (cb && !cb(bl_hw, cmd, msg))) {
                bflb_os_log_debug("[IPC] MSG is handed by static handler, cb %p\r\n", cb);
                found = true;
                cmd->flags &= ~RWNX_CMD_FLAG_WAIT_CFM;

                if (cmd->e2a_msg && msg->param_len) {
                    bflb_os_log_debug("[IPC] copy back RSP to cmd %p, e2a_msg %p, len %d\r\n",
                            cmd, cmd->e2a_msg, msg->param_len);
                    memcpy(cmd->e2a_msg, &msg->param, msg->param_len);
                }

                if (RWNX_CMD_WAIT_COMPLETE(cmd->flags)) {
                    cmd_complete(cmd_mgr, cmd);
                }

                break;
            } else {
                bflb_os_log_debug("[IPC] MSG is NOT handled by static handler\r\n");
            }
        }
    }
    bflb_os_mutex_unlock(cmd_mgr->lock);

    if (!found && cb)
        cb(bl_hw, NULL, msg);

    return 0;
}

void bl_cmd_mgr_init(struct bl_cmd_mgr *cmd_mgr)
{
    INIT_LIST_HEAD(&cmd_mgr->cmds);
    cmd_mgr->lock = bflb_os_mutex_create();
    ASSERT_ERR(NULL != cmd_mgr->lock);

    cmd_mgr->max_queue_sz = RWNX_CMD_MAX_QUEUED;
    cmd_mgr->queue  = &cmd_mgr_queue;
    cmd_mgr->print  = &cmd_mgr_print;
    cmd_mgr->drain  = &cmd_mgr_drain;
    cmd_mgr->llind  = &cmd_mgr_llind;
    cmd_mgr->msgind = &cmd_mgr_msgind;
}
