#include <sdiowifi_securec_wrap.h>
#include <rnm_server.h>

#include <assert.h>
#include <string.h>
//#include <bl_wifi.h>
#include <wifi_mgmr_ext.h>
#include <platform_al.h>

static inline void inc_stat_scratch_ctr(rnms_t *rnm, int id)
{
    ++rnm->statistic.scratch_counter[id];
}

static void rnms_mgmr_task(void *pvParameters)
{
    rnms_t *env = (rnms_t *)pvParameters;
    rnm_msg_t msg;

    while (1) {
        if (0 == sdiowifi_queue_read(env->cmd_msg_queue, &msg, SDIOWIFI_WAIT_FOREVER, 0)) {
            rnms_msg_process(env, &msg);
        } else {
            rnm_err("read msg queue failed\r\n");
        }
    }
}

static void rnms_event_task(void *pvParameters)
{
    rnms_t *rnm = (rnms_t *)pvParameters;
    uint32_t code;
        
    while (1) {

        if (0 == sdiowifi_queue_read(rnm->rnm_event_queue, &code, SDIOWIFI_WAIT_FOREVER, 0)) {
            if (code == CODE_WIFI_ON_CONNECTED) {
                rnms_send_simple_async_msg(rnm, BF1B_CMD_STA_CONNECTED_IND);
            } else if (code == CODE_WIFI_ON_DISCONNECT) {
                rnm->have_valid_sta_ip = false;
                rnms_send_simple_async_msg(rnm, BF1B_CMD_STA_DISCONNECTED_IND);
            } else if (code == CODE_WIFI_ON_AP_STARTED) {
                rnms_send_simple_async_msg(rnm, BF1B_CMD_AP_STARTED_IND);
            } else if (code == CODE_WIFI_ON_AP_STOPPED) {
                rnms_send_simple_async_msg(rnm, BF1B_CMD_AP_STOPPED_IND);
            } else if (code == CODE_WIFI_ON_GOT_IP) {
                rnm->have_valid_sta_ip = true;
#ifdef CFG_DUAL_ETH
        	 rnms_notify_sdioeth_ip_addr(rnm);
#else
        	 rnms_notify_sta_ip_addr(rnm);
#endif
            }
        }
    }
    
}

int rnm_cmd_input(rnms_t *rnm, const rnm_msg_t *msg, int timeout_ms)
{
    if (!(rnm && msg)) {
        return -1;
    }

    sdiowifi_tick_t timeout;
    if (timeout_ms == -1) {
        timeout = SDIOWIFI_WAIT_FOREVER;
    } else {
        timeout = timeout_ms;
    }
    if (0 == sdiowifi_queue_write(rnm->cmd_msg_queue, (void *)msg, timeout, 0)) {
        return 0;
    } else {
        return -1;
    }
}

int rnms_cmd_input_raw(rnms_t *rnm, const void *data, uint16_t data_len)
{
    rnm_msg_t msg;
    void *payload;

    assert(rnm && data && data_len);
    assert(data_len >= sizeof(rnm_base_msg_t));

    MEMSET_SAFE(&msg,  sizeof(msg),  0,  sizeof(msg));
    MEMCPY_SAFE(&msg.cmd,  sizeof(rnm_base_msg_t),  data,  sizeof(rnm_base_msg_t));
    if (data_len > sizeof(rnm_base_msg_t)) {
        payload = rnm_malloc(data_len);
        if (!payload) {
            inc_stat_scratch_ctr(rnm, RNMS_STAT_ERR_INPUT_NOMEM);
            return -1;
        }
        MEMCPY_SAFE(payload,  data_len,  data,  data_len);
        msg.free_fn = rnm_free;
        msg.data = payload;
        msg.data_len = data_len;
    }
    return rnm_cmd_input(rnm, &msg, -1);
}

static rnms_t *private_data = NULL;

void rnm_event_handler(uint32_t code)
{
    if (private_data == NULL) {
        return;
    }
    rnms_t *rnm = (rnms_t *)private_data;
    int ret = sdiowifi_queue_write(rnm->rnm_event_queue, &code, 0, 0);
    assert(0 == ret);
}

rnms_t *rnm_server_init(void *output_fn_env, rnm_output_t output_fn, void *ready_cb_arg, rnm_ready_cb_t ready_cb)
{
    rnms_t *env;

    // hack to check limit at compile time
    if (!(RNMS_STAT_MAX <= RNM_STATISTIC_SCRATCH_COUNTER_NUM)) {
        void RNMS_No_enough_scratch_counter_for_stat_Consider_increase_counters(void);
        RNMS_No_enough_scratch_counter_for_stat_Consider_increase_counters();
    }

    if ((env = rnm_malloc(sizeof(rnms_t))) == NULL) {
        return NULL;
    }

    MEMSET_SAFE(env,  sizeof(rnms_t),  0,  sizeof(rnms_t));

    env->output_fn_env = output_fn_env;
    env->output_fn = output_fn;

    env->ready_cb_arg = ready_cb_arg;
    env->ready_cb = ready_cb;

    sdiowifi_queue_create(RNMS_CMD_MSG_SIZE, RNMS_CMD_MSG_QUEUE_CAP, &env->cmd_msg_queue);
    sdiowifi_task_create(rnms_mgmr_task, "rnms", 0, RNMS_TASK_STACK_DEPTH, env, RNMS_TASK_PRIORITY, &env->cmd_task);
    
    sdiowifi_queue_create(RNME_CMD_MSG_SIZE, RNME_CMD_MSG_QUEUE_CAP, &env->rnm_event_queue);
    sdiowifi_task_create(rnms_event_task, "rnm event", 0, RNME_TASK_STACK_SIZE, env, RNME_TASK_PRIORITY, &env->rnm_event);
    //aos_register_event_filter(EV_WIFI, wifi_event_cb, env);
    private_data = env;

    return env;
}

int rnms_heartbeat_register_failure_cb(rnms_t *rnm, rnm_heartbeat_failure_cb_t cb, void *cb_arg)
{
    if (!rnm) {
        return -1;
    }

    rnm->heartbeat_failure_cb = cb;
    rnm->heartbeat_failure_cb_arg = cb_arg;

    return 0;
}

static void heartbeat_timer_cb(sdiowifi_timer_handle_t timer, void *arg)
{
    rnms_t *rnm = (rnms_t *)arg;
    rnms_send_simple_async_msg_timeout(rnm, BF1B_CMD_PING, 0);
}

int rnms_heartbeat_start(rnms_t *rnm, unsigned interval_ms, uint8_t threshold)
{
    rnms_heartbeat_stop(rnm);

    rnm->heartbeat_threshold = threshold;
    rnm->heartbeat_failures = 0;
    rnm->heartbeat_timer = sdiowifi_timer_create(heartbeat_timer_cb, rnm, interval_ms, 1, 0);
    sdiowifi_timer_start(rnm->heartbeat_timer);
    return 0;
}

int rnms_heartbeat_stop(rnms_t *rnm)
{
    if (rnm->heartbeat_timer) {
        sdiowifi_timer_stop(rnm->heartbeat_timer);
        sdiowifi_timer_free(rnm->heartbeat_timer);
        rnm->heartbeat_timer = NULL;
        return 0;
    }
    return -1;
}

int rnms_send_transparent_msg(rnms_t *rnm, rnm_msg_t *msg, int timeout_ms)
{
    if (!(rnm && msg)) {
        return -1;
    }
    msg->cmd.flags |= RNM_MSG_FLAG_TRANSPARENT;
    return rnm_cmd_input(rnm, msg, timeout_ms);
}

int rnms_send_simple_async_msg_timeout(rnms_t *rnm, bf1b_cmd_t cmd, int timeout_ms)
{
    rnm_msg_t msg;

    MEMSET_SAFE(&msg,  sizeof(msg),  0,  sizeof(msg));
    msg.cmd.cmd = cmd;
    msg.cmd.flags |= RNM_MSG_FLAG_ASYNC;
    return rnms_send_transparent_msg(rnm, &msg, timeout_ms);
}

int rnms_send_simple_async_msg(rnms_t *rnm, bf1b_cmd_t cmd)
{
    return rnms_send_simple_async_msg_timeout(rnm, cmd, -1);
}

int rnms_msg_output(rnms_t *rnm, const void *data, uint16_t data_length)
{
    rnm_base_msg_t *msg = (rnm_base_msg_t *)data;

    if (!(msg->cmd < BF1B_CMD_MAX)) {
        rnm_err("Unknown cmd %u!\r\n", msg->cmd);
        return -1;
    }
    rnm->statistic.cmds[msg->cmd].tx_cnt++;
    return rnm->output_fn(rnm->output_fn_env, RNM_OUT_COMMAND, data, data_length);
}

bool rnms_msg_is_get_mac(const void *msg)
{
    const rnm_base_msg_t *m = msg;
    if (m->cmd == BF1B_CMD_GET_MAC_ADDR) {
        return true;
    } else {
        return false;
    }
}

void rnms_msg_fill_common(rnms_t *rnm, void *resp, const rnm_msg_t *msg)
{
    rnm_base_msg_t *hdr = resp;
    const rnm_base_msg_t *m = &msg->cmd;

    hdr->cmd = m->cmd;
    hdr->flags = RNM_MSG_FLAG_ACK;
    hdr->status = STATUS_OK;
    hdr->session_id = m->session_id;
    hdr->msg_id = ++rnm->last_msg_id;
    hdr->msg_id_replying = m->msg_id;
}

#define MD(id, desc) RNM_MAKE_STAT_SCR_CTR_DESC(id, desc)

static rnm_stat_scr_ctr_desc_t rnms_stat_scr_ctr_desc[RNMS_STAT_MAX] = {
    MD(RNMS_STAT_ERR_INVALID_CMD, "invalid CMD"),
    MD(RNMS_STAT_ERR_INPUT_NOMEM, "malloc fail when RX msg"),
    MD(RNMS_STAT_ERR_OUTPUT_NOMEM, "malloc fail when TX msg"),
};

int rnms_statistic_dump(rnms_t *rnm)
{
    return rnm_statistic_dump(&rnm->statistic, rnms_stat_scr_ctr_desc, RNMS_STAT_MAX);
}
