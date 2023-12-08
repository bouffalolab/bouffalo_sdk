/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <smart_audio.h>
//#include "drv_amp.h"
#include <csi_core.h>
#include <msp/kernel.h>
#include <xutils/debug.h>

#define TAG "smart_audio"

#define SMTAUDIO_MESSAGE_NUM 5

static uint32_t smtaudio_crtl_id;
static msp_dlist_t smtaudio_ctrl_list_head;
static msp_dlist_t smtaudio_delay_list_head;
static msp_dlist_t smtaudio_resume_list_head;
static audio_evt_t audio_event_callback;

typedef struct smtaudio_type {
    smtaudio_sub_state_t cur_state;
    smtaudio_sub_state_t last_state;
    uint8_t              lpm_flag; // 0: not in lpm state, 1: in lpm state
    msp_mutex_t          smtaudio_mutex;
    msp_queue_t          smtaudio_queue;
    uint8_t *            smtaudio_q_buffer;
    uint8_t              resume_flag;
    uint8_t              run_flag;
} smtaudio_type_t;

static smtaudio_type_t smtaudio_ctx;

#define SMTAUDIO_LOCK()                                                 \
    do {                                                                \
        msp_mutex_lock(&smtaudio_ctx.smtaudio_mutex, MSP_WAIT_FOREVER); \
    } while (0)
#define SMTAUDIO_UNLOCK()                               \
    do {                                                \
        msp_mutex_unlock(&smtaudio_ctx.smtaudio_mutex); \
    } while (0)

static msp_mutex_t smtaudio_list_mutex;
#define SMTAUDIO_LIST_LOCK()                                     \
    do {                                                     \
        if (!msp_mutex_is_valid(&smtaudio_list_mutex))              \
            msp_mutex_new(&smtaudio_list_mutex);                    \
        msp_mutex_lock(&smtaudio_list_mutex, MSP_WAIT_FOREVER);     \
    } while (0)
#define SMTAUDIO_LIST_UNLOCK()                                    \
    do {                                                      \
        msp_mutex_unlock(&smtaudio_list_mutex);                      \
    } while (0)

#define LPM_RETURN()             \
    if (smtaudio_ctx.lpm_flag) { \
        return;                  \
    }
#define LPM_RETURN_RET(ret)      \
    if (smtaudio_ctx.lpm_flag) { \
        return ret;              \
    }

static void smtaudio_delay_list_insert(smtaudio_delay_list_node_t *smt_node);
static void smtaudio_delay_list_rm(smtaudio_delay_list_node_t *smt_node);
static void smtaudio_delay_list_clear(void);
static void smtaudio_resume_list_insert(smtaudio_resume_list_node_t *smt_node);
static void smtaudio_resume_list_rm(smtaudio_resume_list_node_t *smt_node);
static void smtaudio_resume_list_clear(void);
/***** internal APIs *****/
static void smtaudio_delay_list_rm(smtaudio_delay_list_node_t *smt_node)
{
    LOGD(TAG, "delete type:%d from delay list", smt_node->id);
    SMTAUDIO_LIST_LOCK();
    msp_dlist_del(&smt_node->node);
    SMTAUDIO_LIST_UNLOCK();
    msp_free(smt_node->url);
    free(smt_node);
}

static void smtaudio_delay_list_clear(void)
{
    while (!msp_dlist_empty(&smtaudio_delay_list_head)) {
        smtaudio_delay_list_node_t *first_node;
        first_node =
            msp_dlist_entry(smtaudio_delay_list_head.next, smtaudio_delay_list_node_t, node);
        smtaudio_delay_list_rm(first_node);
    }
}

/* resume/delay list插入时 清空对方 list */
static void smtaudio_delay_list_insert(smtaudio_delay_list_node_t *smt_node)
{
    smtaudio_delay_list_node_t *tmp_node = NULL;

    smtaudio_resume_list_clear();

    msp_dlist_for_each_entry(&smtaudio_delay_list_head, tmp_node, smtaudio_delay_list_node_t, node) {
        if ((tmp_node->delay_reason == smt_node->delay_reason) || (tmp_node->id == smt_node->id)) {
            smtaudio_delay_list_rm(tmp_node);
            break;
        }
    }
    LOGD(TAG, "add type:%d into delay list, reason:%d", smt_node->id, smt_node->delay_reason);
    /* sort by prio, small to big*/
    msp_dlist_for_each_entry(&smtaudio_delay_list_head, tmp_node, smtaudio_delay_list_node_t, node) {
        if (tmp_node->prio > smt_node->prio) {
            break;
        }
    }
    SMTAUDIO_LIST_LOCK();
    msp_dlist_add_tail(&smt_node->node, &tmp_node->node);
    SMTAUDIO_LIST_UNLOCK();
}

static void smtaudio_resume_list_rm(smtaudio_resume_list_node_t *smt_node)
{
    LOGD(TAG, "delete type:%d from resume list", smt_node->id);
    SMTAUDIO_LIST_LOCK();
    msp_dlist_del(&smt_node->node);
    SMTAUDIO_LIST_UNLOCK();
    free(smt_node);
}

static void smtaudio_resume_list_clear(void)
{
    while (!msp_dlist_empty(&smtaudio_resume_list_head)) {
        smtaudio_resume_list_node_t *first_node;
        first_node =
            msp_dlist_entry(smtaudio_resume_list_head.next, smtaudio_resume_list_node_t, node);
        smtaudio_resume_list_rm(first_node);
    }
}

static void smtaudio_resume_list_insert(smtaudio_resume_list_node_t *smt_node)
{
    smtaudio_resume_list_node_t *tmp_node = NULL;

    /* 清空 delay list */
    smtaudio_delay_list_clear();

    /* 同一播放源只允许 有一个case在resume list中 */
    msp_dlist_for_each_entry(&smtaudio_resume_list_head, tmp_node, smtaudio_resume_list_node_t, node) {
        if (tmp_node->id == smt_node->id) {
            smtaudio_resume_list_rm(tmp_node);
            break;
        }
    }
    LOGD(TAG, "add type:%d into resume list, reason:%d", smt_node->id, smt_node->interrupt_reason);
    /* sort by prio, small to big*/
    msp_dlist_for_each_entry(&smtaudio_resume_list_head, tmp_node, smtaudio_resume_list_node_t, node) {
        if (tmp_node->prio > smt_node->prio) {
            break;
        }
    }
    SMTAUDIO_LIST_LOCK();
    msp_dlist_add_tail(&smt_node->node, &tmp_node->node);
    SMTAUDIO_LIST_UNLOCK();
}

static void smtaudio_ctrl_list_pri_insert(smtaudio_ops_node_t *smt_node)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    /* sort by prio, small to big*/
    msp_dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->prio > smt_node->prio) {
            break;
        }
    }
    SMTAUDIO_LIST_LOCK();
    msp_dlist_add_tail(&smt_node->node, &tmp_node->node);
    SMTAUDIO_LIST_UNLOCK();
}

static smtaudio_ops_node_t *get_smtaudio_ctrl_ops_by_id(int id)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    msp_dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->id == id) {
            return tmp_node;
        }
    }
    return NULL;
}

static smtaudio_ops_node_t *find_first_playing_audio(void)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    msp_dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
            return tmp_node;
        }
    }
    return NULL;
}

smtaudio_ops_node_t *get_default_audio_ops(void)
{
    return get_smtaudio_ctrl_ops_by_id(DEFAULT_PLAY_TYPE);
}

/* 判断当前 是否允许 播放 resume/delay list 中的 音源 */
static int smtaudio_resume_state(int reason, int type)
{
    if (reason == SMTAUDIO_LOCAL_PLAY) {
        return smtaudio_ctx.resume_flag;
    } else {
        return 1;
    }
}

static void switch_state(smtaudio_sub_state_t old_state, smtaudio_sub_state_t new_state)
{
    if (old_state == new_state) {
        /* don't need to change state */
        return;
    }

    LOGD(TAG, "old state[%d] --> new state[%d]", old_state, new_state);
    smtaudio_ctx.last_state = old_state;
    smtaudio_ctx.cur_state  = new_state;
}

static void smtaudio_change_state(smtaudio_state_t new_state, smtaudio_player_type_t type)
{
    smtaudio_ops_node_t *tmp_node = NULL;
    smtaudio_ops_node_t *smtaudio_node = NULL;
    smtaudio_ops_node_t *first_playing_audio_ops = NULL;
    smtaudio_sub_state_t new_sub_state;

    LOGD(TAG, "Enter %s: current state:%d type:%d new_state:%d", __FUNCTION__,
         smtaudio_ctx.cur_state, type, new_state);
    new_sub_state = type * 3 + new_state;

    if (smtaudio_ctx.cur_state == new_sub_state)
        return;
    smtaudio_node = get_smtaudio_ctrl_ops_by_id(type);
    if (smtaudio_node) {
        if (new_state == SMTAUDIO_STATE_PLAYING) {
            if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
                smtaudio_node->pause();
                return;
            }

            msp_dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
                if ((tmp_node->status == SMTAUDIO_STATE_PLAYING) && (tmp_node->id != type)) {
                    first_playing_audio_ops = tmp_node;
                    break;
                }
            }

            if (first_playing_audio_ops) {
                LOGD(TAG, "first playig audio type:%d", first_playing_audio_ops->id);
                if (first_playing_audio_ops->prio < smtaudio_node->prio) {
                    smtaudio_node->pause();
                    smtaudio_delay_list_node_t *node =
                        (smtaudio_delay_list_node_t *)malloc(sizeof(smtaudio_delay_list_node_t));
                    node->id           = smtaudio_node->id;
                    node->prio         = smtaudio_node->prio;
                    node->seek_time    = 0;
                    node->url          = NULL;
                    node->delay_reason = first_playing_audio_ops->id;
                    node->valid = 1;
                    smtaudio_delay_list_insert(node);
                    return;
                } else if (first_playing_audio_ops->prio > smtaudio_node->prio) {
                    first_playing_audio_ops->pause();
                    smtaudio_resume_list_node_t *node =
                        (smtaudio_resume_list_node_t *)malloc(sizeof(smtaudio_resume_list_node_t));
                    node->id               = first_playing_audio_ops->id;
                    node->prio             = first_playing_audio_ops->prio;
                    node->interrupt_reason = type;
                    node->valid = 1;
                    smtaudio_resume_list_insert(node);
                } else {
                    //同一优先级打断 不resume
                    first_playing_audio_ops->pause();
                }
            }
            switch_state(smtaudio_ctx.cur_state, new_sub_state);
        } else {
            /* 播放源切换后, pause stop 无效*/
            if (smtaudio_get_play_type() == type) {
                switch_state(smtaudio_ctx.cur_state, new_sub_state);
            }
        }
    }
}

/* 所有播放源产生事件(start、pause、stop)时, 需要调用的回调函数 */
static void smtaudio_event_callback(int type, smtaudio_player_evtid_t evt_id)
{
    int            ret;
    audio_result_t result;

    result.type   = type;
    result.evt_id = evt_id;

    if(audio_event_callback) {
        audio_event_callback(type, evt_id);
    }

    ret = msp_queue_send(&smtaudio_ctx.smtaudio_queue, &result, sizeof(audio_result_t));
    if (ret < 0) {
        LOGE(TAG, "queue send failed");
    }
}

static void smtaudio_event_task(void *arg)
{
    size_t         len;
    audio_result_t       audio_result;
    smtaudio_ops_node_t *smtaudio_node;
    smtaudio_resume_list_node_t *first_resume_node;
    smtaudio_delay_list_node_t *first_delay_node;

    while (smtaudio_ctx.run_flag) {
        msp_queue_recv(&smtaudio_ctx.smtaudio_queue, MSP_WAIT_FOREVER, &audio_result, &len);
        LOGD(TAG, "smtaudio_event_callback type:%d evt_id:%d\n", audio_result.type,
             audio_result.evt_id);
        smtaudio_node = get_smtaudio_ctrl_ops_by_id(audio_result.type);
        switch (audio_result.evt_id) {
        case SMTAUDIO_PLAYER_EVENT_START:
        case SMTAUDIO_PLAYER_EVENT_RESUME:
            smtaudio_node->status = SMTAUDIO_STATE_PLAYING;
            smtaudio_change_state(SMTAUDIO_STATE_PLAYING, audio_result.type);
            break;
        case SMTAUDIO_PLAYER_EVENT_ERROR:
        case SMTAUDIO_PLAYER_EVENT_STOP:
            smtaudio_node->status = SMTAUDIO_STATE_STOP;
            smtaudio_change_state(SMTAUDIO_STATE_STOP, audio_result.type);
            break;
        case SMTAUDIO_PLAYER_EVENT_PAUSE:
            smtaudio_node->status = SMTAUDIO_STATE_PAUSE;
            smtaudio_change_state(SMTAUDIO_STATE_PAUSE, audio_result.type);
            break;
        default:
            break;
        }
        if ((audio_result.evt_id == SMTAUDIO_PLAYER_EVENT_STOP) ||
            (audio_result.evt_id == SMTAUDIO_PLAYER_EVENT_PAUSE)) {
            if (!msp_dlist_empty(&smtaudio_resume_list_head)) {
                msp_dlist_for_each_entry(&smtaudio_resume_list_head, first_resume_node, smtaudio_resume_list_node_t, node) {
                    if((first_resume_node->interrupt_reason == audio_result.type) && (first_resume_node->valid)) {
                        if (smtaudio_resume_state(first_resume_node->interrupt_reason,
                                                  audio_result.type)) {
                            smtaudio_node = get_smtaudio_ctrl_ops_by_id(first_resume_node->id);
                            if (smtaudio_node) {
                                LOGD(TAG, "resume type:%d from resume list", smtaudio_node->id);
                                smtaudio_node->resume();
                            }
                            smtaudio_resume_list_rm(first_resume_node);
                            break;
                        } else {
                            LOGD(TAG, "not in play state");
                        }
                    }
                }
            } else if (!msp_dlist_empty(&smtaudio_delay_list_head)) {
                msp_dlist_for_each_entry(&smtaudio_delay_list_head, first_delay_node, smtaudio_delay_list_node_t, node) {
                    if ((first_delay_node->delay_reason == audio_result.type) && (first_delay_node->valid)){
                        if (smtaudio_resume_state(first_delay_node->delay_reason, audio_result.type)) {
                            smtaudio_node = get_smtaudio_ctrl_ops_by_id(first_delay_node->id);
                            if (smtaudio_node) {
                                LOGD(TAG, "start type:%d from delay list", smtaudio_node->id);
                                smtaudio_node->start(first_delay_node->url, first_delay_node->seek_time, 0);
                            }
                            smtaudio_delay_list_rm(first_delay_node);
                            break;
                        } else {
                            LOGD(TAG, "not in play state");
                        }
                    }
                }
            }
        }
    }

    msp_task_exit(0);
}



/****************************************/
/*          external APIs               */
/****************************************/
int8_t smtaudio_init(audio_evt_t audio_evt_cb)
{
    static int init_flag = 0;
    if(init_flag) {
        LOGE(TAG, "smtaudio already init");
        return -1;
    }
    LOGD(TAG, "Enter %s", __FUNCTION__);
    msp_task_t smt_task_hdl;

    memset(&smtaudio_ctx, 0, sizeof(smtaudio_type_t));
    msp_mutex_new(&smtaudio_ctx.smtaudio_mutex);
    smtaudio_ctx.smtaudio_q_buffer = (uint8_t *)malloc(sizeof(audio_result_t) * SMTAUDIO_MESSAGE_NUM);
    if (smtaudio_ctx.smtaudio_q_buffer == NULL) {
        LOGE(TAG, "smtaudio_ctx.smtaudio_q_buffer malloc fail");
        return -EINVAL;
    }
    msp_queue_new(&smtaudio_ctx.smtaudio_queue, smtaudio_ctx.smtaudio_q_buffer,
                  SMTAUDIO_MESSAGE_NUM * sizeof(audio_result_t), sizeof(audio_result_t));

    SMTAUDIO_LOCK();
    smtaudio_ctx.cur_state = smtaudio_ctx.last_state = SMTAUDIO_SUBSTATE_LOCAL_STOP;
    smtaudio_ctx.run_flag                            = 1;
    msp_task_new_ext(&smt_task_hdl, "smtaudio_event_task", smtaudio_event_task, NULL, CONFIG_SMART_AUDIO_STACK_SIZE,
                     MSP_DEFAULT_APP_PRI);
    SMTAUDIO_LIST_LOCK();
    msp_dlist_init(&smtaudio_ctrl_list_head);
    msp_dlist_init(&smtaudio_delay_list_head);
    msp_dlist_init(&smtaudio_resume_list_head);
    if(audio_evt_cb) {
        audio_event_callback = audio_evt_cb;
    }
    SMTAUDIO_LIST_UNLOCK();

    SMTAUDIO_UNLOCK();  
    init_flag = 1;       
    return 0;
}

int8_t smtaudio_vol_up(int16_t vol)
{
    smtaudio_ops_node_t *node_adjust_ops;
    smtaudio_resume_list_node_t *node_adjust;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }
    SMTAUDIO_LOCK();
    node_adjust_ops = find_first_playing_audio();
    if (node_adjust_ops && node_adjust_ops->vol_up) {
        node_adjust_ops->vol_up(vol);
        SMTAUDIO_UNLOCK();
        return 0;
    }
    //判断resume list是否存在播放对象
    if(!msp_dlist_empty(&smtaudio_resume_list_head)) {
        node_adjust = msp_dlist_entry(smtaudio_resume_list_head.next, smtaudio_resume_list_node_t, node);
        if((node_adjust->interrupt_reason != INTERRUPT_REASON_BY_USER) && (node_adjust->valid == 1)){
            node_adjust_ops = get_smtaudio_ctrl_ops_by_id(node_adjust->id);
            if (node_adjust_ops && node_adjust_ops->vol_up) {
                node_adjust_ops->vol_up(vol);
                LOGD(TAG, "smart audio adjust volume from resume list. type:%d", node_adjust->id);
                SMTAUDIO_UNLOCK();
                return 0;
            }
        }
    }
    /*如果没有 audio 播放 则调整 默认音量*/
    node_adjust_ops = get_default_audio_ops();
    if (node_adjust_ops) {
        node_adjust_ops->vol_up(vol);
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_vol_down(int16_t vol)
{
    smtaudio_ops_node_t *node_adjust_ops;
    smtaudio_resume_list_node_t *node_adjust;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }
    SMTAUDIO_LOCK();
    node_adjust_ops = find_first_playing_audio();
    if (node_adjust_ops && node_adjust_ops->vol_down) {
        node_adjust_ops->vol_down(vol);
        SMTAUDIO_UNLOCK();
        return 0;
    }
    //判断resume list是否存在播放对象
    if(!msp_dlist_empty(&smtaudio_resume_list_head)) {
        node_adjust = msp_dlist_entry(smtaudio_resume_list_head.next, smtaudio_resume_list_node_t, node);
         if((node_adjust->interrupt_reason != INTERRUPT_REASON_BY_USER) && (node_adjust->valid == 1)){
            node_adjust_ops = get_smtaudio_ctrl_ops_by_id(node_adjust->id);
            if (node_adjust_ops && node_adjust_ops->vol_down) {
                node_adjust_ops->vol_down(vol);
                LOGD(TAG, "smart audio adjust volume from resume list. type:%d", node_adjust->id);
                SMTAUDIO_UNLOCK();
                return 0;
            }
        }
    }
    /*如果没有 audio 播放 则调整 默认音量*/
    node_adjust_ops = get_default_audio_ops();
    if (node_adjust_ops) {
        node_adjust_ops->vol_down(vol);
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_vol_set(int16_t set_vol)
{
    smtaudio_ops_node_t *node_adjust_ops;
    smtaudio_resume_list_node_t *node_adjust;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }
    SMTAUDIO_LOCK();
    node_adjust_ops = find_first_playing_audio();
    if (node_adjust_ops && node_adjust_ops->vol_set) {
        node_adjust_ops->vol_set(set_vol);
        SMTAUDIO_UNLOCK();
        return 0;
    }
    //判断resume list是否存在播放对象
    if(!msp_dlist_empty(&smtaudio_resume_list_head)) {
        node_adjust = msp_dlist_entry(smtaudio_resume_list_head.next, smtaudio_resume_list_node_t, node);
         if((node_adjust->interrupt_reason != INTERRUPT_REASON_BY_USER) && (node_adjust->valid == 1)){
            node_adjust_ops = get_smtaudio_ctrl_ops_by_id(node_adjust->id);
            if (node_adjust_ops && node_adjust_ops->vol_set) {
                node_adjust_ops->vol_set(set_vol);
                LOGD(TAG, "smart audio adjust volume from resume list. type:%d", node_adjust->id);
                SMTAUDIO_UNLOCK();
                return 0;
            }
        }
    }
    /*如果没有 audio 播放 则调整 默认音量*/
    node_adjust_ops = get_default_audio_ops();
    if (node_adjust_ops) {
        node_adjust_ops->vol_set(set_vol);
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_vol_get(void)
{
    int vol = 0;
    smtaudio_ops_node_t *audio_default_ops;
    smtaudio_ops_node_t *first_playing_audio_ops;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    SMTAUDIO_LOCK();
    first_playing_audio_ops = find_first_playing_audio();
    if (first_playing_audio_ops && first_playing_audio_ops->vol_get) {
        vol = first_playing_audio_ops->vol_get();
        SMTAUDIO_UNLOCK();
        return vol;
    }

    audio_default_ops = get_default_audio_ops();
    if (audio_default_ops) {
        vol = audio_default_ops->vol_get();
    }
    SMTAUDIO_UNLOCK();
    return vol;
}

int8_t smtaudio_start(int type, char *url, uint64_t seek_time, uint8_t resume)
{
    int8_t               ret = 0;
    smtaudio_ops_node_t *smtaudio_node;
    smtaudio_ops_node_t *first_playing_audio_ops;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.(%s)", url);
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state:%d type:%d resume_flag:%d", __FUNCTION__, smtaudio_ctx.cur_state, type, resume);

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }

    if ((type < 0) || (type >= SMTAUDIO_PLAY_TYPE_NUM)) {
        LOGD(TAG, "invalid play type");
        return -1;
    }

    SMTAUDIO_LOCK();
    smtaudio_node = get_smtaudio_ctrl_ops_by_id(type);
    if (smtaudio_node) {
        first_playing_audio_ops = find_first_playing_audio();
        if (first_playing_audio_ops) {
            if (first_playing_audio_ops->prio < smtaudio_node->prio) {
                smtaudio_delay_list_node_t *node =
                    (smtaudio_delay_list_node_t *)malloc(sizeof(smtaudio_delay_list_node_t));
                node->id        = smtaudio_node->id;
                node->prio      = smtaudio_node->prio;
                node->seek_time = seek_time;
                node->url       = strdup(url);
                if (!node->url) {
                    SMTAUDIO_UNLOCK();
                    return 0;
                }
                node->delay_reason     = first_playing_audio_ops->id;
                node->valid = 1;
                smtaudio_delay_list_insert(node);
                SMTAUDIO_UNLOCK();
                return 0;
            } else if (first_playing_audio_ops->prio > smtaudio_node->prio) {
                first_playing_audio_ops->pause();
                smtaudio_resume_list_node_t *node =
                    (smtaudio_resume_list_node_t *)malloc(sizeof(smtaudio_resume_list_node_t));
                node->id               = first_playing_audio_ops->id;
                node->prio             = first_playing_audio_ops->prio;
                node->interrupt_reason = type;
                node->valid = 1;
                smtaudio_resume_list_insert(node);
            } else {
                first_playing_audio_ops->pause();
            }
        }
        ret = smtaudio_node->start(url, seek_time, resume);
        smtaudio_ctx.resume_flag = resume;
    } else {
        LOGD(TAG, "%s ops not found", __func__);
    }
    SMTAUDIO_UNLOCK();
    return ret;
}

int8_t smtaudio_pause()
{
    smtaudio_ops_node_t *tmp_node = NULL;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return 0;
    }
    SMTAUDIO_LOCK();
    smtaudio_clear_ready_list();
    msp_dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
            tmp_node->pause();
            smtaudio_resume_list_node_t *node =
                        (smtaudio_resume_list_node_t *)malloc(sizeof(smtaudio_resume_list_node_t));
            node->id               = tmp_node->id;
            node->prio             = tmp_node->prio;
            node->interrupt_reason = INTERRUPT_REASON_BY_USER;
            node->valid = 1;
            smtaudio_resume_list_insert(node);
        }
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_stop(int type)
{
    smtaudio_ops_node_t *tmp_node = NULL;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    SMTAUDIO_LOCK();
    if(SMTAUDIO_TYPE_ALL == type) {
        msp_dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
            if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
                tmp_node->stop();
            }
        }
    } else if((type >=0) && (type <SMTAUDIO_PLAY_TYPE_NUM)) {
        tmp_node = get_smtaudio_ctrl_ops_by_id(type);
        if (tmp_node) {
            if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
                tmp_node->stop();
            }
        }
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_mute()
{
    smtaudio_ops_node_t *tmp_node = NULL;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    SMTAUDIO_LOCK();
    if (smtaudio_ctx.cur_state != SMTAUDIO_SUBSTATE_MUTE) {
        // amplifier_onoff(0);
        switch_state(smtaudio_ctx.cur_state, SMTAUDIO_SUBSTATE_MUTE);
    } else {
        // amplifier_onoff(1);
        msp_dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
            if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
                switch_state(SMTAUDIO_SUBSTATE_MUTE, tmp_node->id * 3 + SMTAUDIO_STATE_PLAYING);
                SMTAUDIO_UNLOCK();
                return 0;
            }
        }
        switch_state(SMTAUDIO_SUBSTATE_MUTE, SMTAUDIO_SUBSTATE_LOCAL_STOP);
    }
    SMTAUDIO_UNLOCK();
    return 0;
}

int8_t smtaudio_resume()
{
    int                  ret = -1;
    smtaudio_ops_node_t *smtaudio_node;
    smtaudio_resume_list_node_t *tmp_resume_list_node = NULL;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    LPM_RETURN_RET(-1);
    LOGD(TAG, "Enter %s: current state [%d]", __FUNCTION__, smtaudio_ctx.cur_state);
    SMTAUDIO_LOCK();
    msp_dlist_for_each_entry(&smtaudio_resume_list_head, tmp_resume_list_node, smtaudio_resume_list_node_t, node) {
        if((tmp_resume_list_node) && (tmp_resume_list_node->valid)) {
            smtaudio_node = get_smtaudio_ctrl_ops_by_id(tmp_resume_list_node->id);
            if ((smtaudio_node) && (smtaudio_node->status != SMTAUDIO_STATE_PLAYING)) {
                ret = smtaudio_node->resume();
                smtaudio_resume_list_rm(tmp_resume_list_node);
                break;
            }
        }
    }
    SMTAUDIO_UNLOCK();
    return ret;
}

void smtaudio_clear_ready_list(void)
{
    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return;
    }

    smtaudio_delay_list_clear();
    smtaudio_resume_list_clear();
}

void smtaudio_enable_ready_list(int enable)
{
    smtaudio_delay_list_node_t *tmp_delay_list_node = NULL;
    smtaudio_resume_list_node_t *tmp_resume_list_node = NULL;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return;
    }

    msp_dlist_for_each_entry(&smtaudio_delay_list_head, tmp_delay_list_node, smtaudio_delay_list_node_t, node) {
        if(tmp_delay_list_node) {
            tmp_delay_list_node->valid = enable;
            LOGD(TAG, "set type:%d in delay list, delay reason:%d set valid:%d", tmp_delay_list_node->id, tmp_delay_list_node->delay_reason, enable);
        }
    }

    msp_dlist_for_each_entry(&smtaudio_resume_list_head, tmp_resume_list_node, smtaudio_resume_list_node_t, node) {
        if(tmp_resume_list_node) {
            tmp_resume_list_node->valid = enable;
            LOGD(TAG, "set type:%d in resume list, interrupt reason:%d valid:%d", tmp_resume_list_node->id, tmp_resume_list_node->interrupt_reason, enable);
        }
    }
}

void smtaudio_check_ready_list(void)
{
    smtaudio_delay_list_node_t *tmp_delay_list_node = NULL;
    smtaudio_resume_list_node_t *tmp_resume_list_node = NULL;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return;
    }

    msp_dlist_for_each_entry(&smtaudio_delay_list_head, tmp_delay_list_node, smtaudio_delay_list_node_t, node) {
        if(tmp_delay_list_node) {
            LOGD(TAG, "type:%d in delay list, delay reason:%d valid:%d", tmp_delay_list_node->id, tmp_delay_list_node->delay_reason, tmp_delay_list_node->valid);
        }
    }

    msp_dlist_for_each_entry(&smtaudio_resume_list_head, tmp_resume_list_node, smtaudio_resume_list_node_t, node) {
        if(tmp_resume_list_node) {
            LOGD(TAG, "type:%d in resume list, interrupt reason:%d valid:%d", tmp_resume_list_node->id, tmp_resume_list_node->interrupt_reason, tmp_resume_list_node->valid);
        }
    }
}

smtaudio_state_t smtaudio_get_state(void)
{
    smtaudio_ops_node_t *first_playing_audio_ops;

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return SMTAUDIO_STATE_NOINIT;
    }

    if (smtaudio_ctx.cur_state == SMTAUDIO_SUBSTATE_MUTE) {
        return SMTAUDIO_STATE_MUTE;
    }
    first_playing_audio_ops = find_first_playing_audio();
    if (first_playing_audio_ops) {
        return SMTAUDIO_STATE_PLAYING;
    }
    return SMTAUDIO_STATE_STOP;
}

smtaudio_player_type_t smtaudio_get_play_type(void)
{
    int ret = -1;

    switch (smtaudio_ctx.cur_state) {
    case SMTAUDIO_SUBSTATE_ONLINE_PLAYING:
    case SMTAUDIO_SUBSTATE_ONLINE_PAUSE:
        ret = SMTAUDIO_ONLINE_MUSIC;
        break;
    case SMTAUDIO_SUBSTATE_BT_A2DP_PLAYING:
    case SMTAUDIO_SUBSTATE_BT_A2DP_PAUSE:
        ret = SMTAUDIO_BT_A2DP;
        break;
    case SMTAUDIO_SUBSTATE_BT_HFP_PLAYING:
        ret = SMTAUDIO_BT_HFP;
        break;
    case SMTAUDIO_SUBSTATE_LOCAL_PLAYING:
    case SMTAUDIO_SUBSTATE_LOCAL_PAUSE:
        ret = SMTAUDIO_LOCAL_PLAY;
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}

void smtaudio_substate_get(int *cur_state, int *last_state)
{
    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return;
    }

    *cur_state  = smtaudio_ctx.cur_state;
    *last_state = smtaudio_ctx.last_state;
}

int8_t smtaudio_lpm(uint8_t state)
{
    uint32_t irq_flag = 0;

    irq_flag = csi_irq_save();
    msp_kernel_sched_suspend();
    smtaudio_ctx.lpm_flag = state;
    msp_kernel_sched_resume();
    csi_irq_restore(irq_flag);

    return 0;
}

int smtaudio_enter_lpm_check(void)
{
    smtaudio_ops_node_t *tmp_node = NULL;
    msp_dlist_for_each_entry(&smtaudio_ctrl_list_head, tmp_node, smtaudio_ops_node_t, node) {
        if (tmp_node->status == SMTAUDIO_STATE_PLAYING) {
            return 0;
        }
    }
    return 1;
}

int8_t smtaudio_ops_register(smtaudio_ops_node_t *ops)
{
    msp_assert(ops);

    if(!msp_mutex_is_valid(&smtaudio_ctx.smtaudio_mutex)) {
        LOGE(TAG, "smtaudio is not initialized.");
        return -1;
    }

    if((smtaudio_crtl_id & (1 << ops->id)) != 0) {
        LOGE(TAG, "id:%d has been register", ops->id);
        return -1;
    }

    smtaudio_crtl_id |= (1 << ops->id);
    smtaudio_ctrl_list_pri_insert(ops);
    ops->callback = smtaudio_event_callback;

    return 0;
}
