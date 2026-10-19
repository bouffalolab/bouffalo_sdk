#include <sdiowifi_securec_wrap.h>
#include <rnm_client.h>

#include <assert.h>
#include <string.h>
#include <rnm_utils.h>

static inline void inc_stat_scratch_ctr(rnmc_t *rnm, int id)
{
    ++rnm->statistic.scratch_counter[id];
}

static inline void add_stat_scratch_ctr(rnmc_t *rnm, int id, uint32_t add)
{
    rnm->statistic.scratch_counter[id] += add;
}

static void reset_wait_slot(struct wait_slot *slot)
{
    slot->used = false;
    slot->session_id = 0;
    slot->resp = NULL;
}

static void rnmc_msg_process(rnmc_t *rnm, rnm_msg_t *msg)
{
    bool free = true;
    rnm_base_msg_t *msg_to_send;
    size_t msg_len;
    struct cmd_rpc *rpc;
    struct wait_slot *slot = NULL;

    if (!(msg->cmd.cmd < BF1B_CMD_MAX)) {
        rnm_err("Unknown cmd %u!\r\n", msg->cmd.cmd);
        inc_stat_scratch_ctr(rnm, RNMC_STAT_ERR_CMD_ID_NOT_IN_RANGE);
        goto free;
    }

    if (msg->data) {
        msg_to_send = (rnm_base_msg_t *)msg->data;
        msg_len = msg->data_len;
    } else {
        msg_to_send = &msg->cmd;
        msg_len = sizeof(*msg_to_send);
    }

    if (msg->cmd.flags & RNM_MSG_FLAG_TRANSPARENT) {
        msg_to_send->msg_id = msg->cmd.msg_id;
        msg_to_send->session_id = msg->cmd.session_id;
        // msgs with flag transparent set are commands to sent to server
        msg_to_send->flags &= ~RNM_MSG_FLAG_TRANSPARENT;
        rnm->output_fn(rnm->output_fn_env, RNM_OUT_COMMAND, msg_to_send, msg_len);

        rnm->statistic.cmds[msg_to_send->cmd].tx_cnt++;
        goto free;
    }

    rnm_debug("received cmd %u, session_id %u, msg_id %u\r\n", msg->cmd.cmd, msg->cmd.session_id, msg->cmd.msg_id);
    rnm->statistic.cmds[msg->cmd.cmd].rx_cnt++;

    if (msg->cmd.flags & RNM_MSG_FLAG_ACK) {
        rpc = &rnm->cmd_rpc;

        sdiowifi_mutex_lock(rpc->mutex);
        for (int i = 0; i < RNMC_CMD_RPC_WAIT_SLOT_CNT; ++i) {
            if (rpc->wait_slots[i].used && rpc->wait_slots[i].session_id == msg->cmd.session_id) {
                slot = &rpc->wait_slots[i];
                break;
            }
        }
        if (slot) {
            free = false;
            MEMCPY_SAFE(slot->resp,  sizeof(*msg),  msg,  sizeof(*msg));
            sdiowifi_semaphore_signal(slot->sem);
            reset_wait_slot(slot);
        } else {
            rnm_err("MSG[ACK] msg_id %u no matching slot\r\n", msg->cmd.msg_id);
            inc_stat_scratch_ctr(rnm, RNMC_STAT_ERR_RPC_NO_MATCHING_SLOT);
        }
        sdiowifi_mutex_unlock(rpc->mutex);
    }

    if (msg->cmd.flags & RNM_MSG_FLAG_ASYNC) {
        // callback should not care the ASYNC flag
        if (rnm->async_event_cb) {
            rnm->async_event_cb(rnm->async_event_cb_arg, msg_to_send);
        }
    }

free:
    if (free && msg->data) {
        msg->free_fn(msg->data);
    }
}

static void rnmc_mgmr_task(void *pvParameters)
{
    rnmc_t *env = (rnmc_t *)pvParameters;
    rnm_msg_t msg;

    while (1) {
        if (0 == sdiowifi_queue_read(env->cmd_msg_queue, &msg, SDIOWIFI_WAIT_FOREVER)) {
            rnmc_msg_process(env, &msg);
        } else {
            rnm_err("read msg queue failed\r\n");
        }
    }
}

rnmc_t *rnm_client_init(void *output_fn_env, rnm_output_t output_fn, void *async_event_cb_arg, rnm_async_event_cb_t async_event_cb)
{
    rnmc_t *env;

    // hack to check limit at compile time
    if (!(RNMC_STAT_MAX <= RNM_STATISTIC_SCRATCH_COUNTER_NUM)) {
        void RNMC_No_enough_scratch_counter_for_stat_Consider_increase_counters(void);
        RNMC_No_enough_scratch_counter_for_stat_Consider_increase_counters();
    }

    if (!output_fn) {
        return NULL;
    }
    if ((env = rnm_malloc(sizeof(rnmc_t))) == NULL) {
        return NULL;
    }
    MEMSET_SAFE(env,  sizeof(*env),  0,  sizeof(*env));

    // cmd_rpc
    sdiowifi_mutex_create(&env->cmd_rpc.mutex);
    for (int i = 0; i < RNMC_CMD_RPC_WAIT_SLOT_CNT; ++i) {
        struct wait_slot *slot = &env->cmd_rpc.wait_slots[i];
        sdiowifi_semaphore_create(&slot->sem);
        reset_wait_slot(slot);
    }

    env->output_fn_env = output_fn_env;
    env->output_fn = output_fn;
    env->async_event_cb_arg = async_event_cb_arg;
    env->async_event_cb = async_event_cb;

    sdiowifi_queue_create(RNMC_CMD_MSG_SIZE, RNMC_CMD_MSG_QUEUE_CAP, &env->cmd_msg_queue);
    sdiowifi_task_create(rnmc_mgmr_task, "rnmc", 0, RNMC_TASK_STACK_DEPTH, env, RNMC_TASK_PRIORITY, &env->cmd_task);

    return env;
}

int rnmc_sniffer_callback_register(rnmc_t *rnm, void *sniffer_cb_arg, rnmc_sniffer_cb_t sniffer_cb)
{
    if (!rnm) {
        return -1;
    }
    rnm->sniffer_cb_arg = sniffer_cb_arg;
    rnm->sniffer_cb = sniffer_cb;
    return 0;
}

int rnmc_cmd_input_raw(rnmc_t *rnm, void *data, uint16_t data_len)
{
    rnm_msg_t msg;
    void *payload;

    assert(rnm && data && data_len);
    assert(data_len >= sizeof(rnm_base_msg_t));

    rnm_debug("%s: len %u\r\n", __func__, data_len);
    rnm_debugbuf(data, data_len);

    MEMSET_SAFE(&msg,  sizeof(msg),  0,  sizeof(msg));
    MEMCPY_SAFE(&msg.cmd,  sizeof(rnm_base_msg_t),  data,  sizeof(rnm_base_msg_t));
    if (data_len > sizeof(rnm_base_msg_t)) {
        payload = rnm_malloc(data_len);
        if (!payload) {
            inc_stat_scratch_ctr(rnm, RNMC_STAT_ERR_INPUT_NOMEM);
            return -1;
        }
        MEMCPY_SAFE(payload,  data_len,  data,  data_len);
        msg.free_fn = rnm_free;
        msg.data = payload;
        msg.data_len = data_len;
    }
    if (0 == sdiowifi_queue_write(rnm->cmd_msg_queue, &msg, SDIOWIFI_WAIT_FOREVER, 0)) {
        return 0;
    } else {
        return -1;
    }
}

static inline uint16_t acquire_session_id(struct cmd_rpc *rpc)
{
    ++rpc->last_session_id;
    if (rpc->last_session_id == 0) {
        ++rpc->last_session_id;
    }
    return rpc->last_session_id;
}

rnmc_ret_t rnmc_cmd_send(rnmc_t *rnm, void *cmd, uint16_t cmd_len, rnm_msg_t *resp, int ms_to_wait)
{
    rnm_msg_t msg;
    void *msg_copy = NULL;
    struct cmd_rpc *rpc;
    struct wait_slot *slot = NULL;

    if (!(rnm && cmd && cmd_len >= sizeof(rnm_base_msg_t))) {
        return RNMC_INVALID_INPUT;
    }

    MEMSET_SAFE(&msg,  sizeof(msg),  0,  sizeof(msg));
    MEMCPY_SAFE(&msg.cmd,  sizeof(rnm_base_msg_t),  cmd,  sizeof(rnm_base_msg_t));
    if (cmd_len > sizeof(rnm_base_msg_t)) {
        msg_copy = rnm_malloc(cmd_len);
        msg.data = msg_copy;
        if (!msg.data) {
            inc_stat_scratch_ctr(rnm, RNMC_STAT_ERR_INPUT_NOMEM);
            return RNMC_NOMEM;
        }
        msg.free_fn = rnm_free;
        MEMCPY_SAFE(msg.data,  cmd_len,  cmd,  cmd_len);
        msg.data_len = cmd_len;
    }
    msg.cmd.flags |= RNM_MSG_FLAG_TRANSPARENT;

    rpc = &rnm->cmd_rpc;
    sdiowifi_mutex_lock(rpc->mutex);
    for (int i = 0; i < RNMC_CMD_RPC_WAIT_SLOT_CNT; ++i) {
        if (!rpc->wait_slots[i].used) {
            slot = &rpc->wait_slots[i];
            break;
        }
    }
    if (slot) {
        slot->used = true;
        slot->session_id = acquire_session_id(rpc);
        slot->resp = resp;
        msg.cmd.session_id = slot->session_id;
        msg.cmd.msg_id = ++rpc->last_msg_id;
        rnm_debug("sending cmd with session_id %u, msg_id %u\r\n", msg.cmd.session_id, msg.cmd.msg_id);
    } else {
        rnm_warning("CMD no free slot! cmd %u\r\n", msg.cmd.cmd);
        sdiowifi_mutex_unlock(rpc->mutex);
        rnm_free(msg_copy);
        inc_stat_scratch_ctr(rnm, RNMC_STAT_ERR_RPC_NO_FREE_SLOT);
        return RNMC_NO_FREE_SLOT;
    }
    sdiowifi_queue_write(rnm->cmd_msg_queue, &msg, SDIOWIFI_WAIT_FOREVER);
    sdiowifi_mutex_unlock(rpc->mutex);

    if (sdiowifi_semaphore_wait(slot->sem, ms_to_wait) != 0) {
        rnm_warning("CMD timeout! cmd %u, session_id %u, msg_id %u\r\n", msg.cmd.cmd, msg.cmd.session_id, msg.cmd.msg_id);
        sdiowifi_mutex_lock(rpc->mutex);
        reset_wait_slot(slot);
        sdiowifi_mutex_unlock(rpc->mutex);
        inc_stat_scratch_ctr(rnm, RNMC_STAT_ERR_CMD_TIMEOUT);
        return RNMC_TIMEOUT;
    }

    return RNMC_OK;
}

rnmc_ret_t rnmc_cmd_send_simple(rnmc_t *rnm, void *cmd, uint16_t cmd_len, int ms_to_wait)
{
    rnm_msg_t resp;
    rnmc_ret_t ret;

    ret = rnmc_cmd_send(rnm, cmd, cmd_len, &resp, ms_to_wait);
    if (ret == RNMC_OK) {
        if (resp.cmd.status != STATUS_OK) {
            ret = resp.cmd.status;
        }
        if (resp.data) {
            resp.free_fn(resp.data);
        }
    }

    return ret;
}

rnm_wifi_mode_t rnmc_get_mode(rnmc_t *rnm)
{
    if (!rnm) {
        return RNMC_WIFI_MODE_NONE;
    }
    return rnm->mode;
}

int rnmc_sniffer_packet_input(rnmc_t *rnm, const void *data, size_t data_len)
{
    if (!rnm) {
        return -1;
    }
    inc_stat_scratch_ctr(rnm, RNMC_STAT_SNIFFER_PKT_CNT);
    add_stat_scratch_ctr(rnm, RNMC_STAT_SNIFFER_PKT_BYTES, data_len);
    if (rnm->sniffer_cb) {
        rnm->sniffer_cb(rnm->sniffer_cb_arg, data, data_len);
    }
    return 0;
}

#define MD(id, desc) RNM_MAKE_STAT_SCR_CTR_DESC(id, desc)

static rnm_stat_scr_ctr_desc_t rnmc_stat_scr_ctr_desc[RNMC_STAT_MAX] = {
    MD(RNMC_STAT_ERR_CMD_ID_NOT_IN_RANGE, "invalid CMD"),
    MD(RNMC_STAT_ERR_INPUT_NOMEM, "malloc fail when RX msg"),
    MD(RNMC_STAT_ERR_OUTPUT_NOMEM, "malloc fail when TX msg"),
    MD(RNMC_STAT_ERR_CMD_TIMEOUT, "CMD timeout"),
    MD(RNMC_STAT_ERR_RPC_NO_FREE_SLOT, "RPC no free slot"),
    MD(RNMC_STAT_ERR_RPC_NO_MATCHING_SLOT, "RPC no matching slot"),
    MD(RNMC_STAT_SNIFFER_PKT_CNT, "sniffer packets"),
    MD(RNMC_STAT_SNIFFER_PKT_BYTES, "sniffer bytes"),
};

int rnmc_statistic_dump(rnmc_t *rnm)
{
    return rnm_statistic_dump(&rnm->statistic, rnmc_stat_scr_ctr_desc, RNMC_STAT_MAX);
}
