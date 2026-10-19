#include <xav_port.h>
#include "avutil/av_config.h"
#include "avutil/common.h"
#include "avutil/av_typedef.h"
#include "stream/stream.h"
#include "avformat/avformat.h"
#include "avformat/avformat_utils.h"
#include "avformat/avparser_all.h"
#include "avcodec/avcodec.h"
#include "output/output.h"
#include "avfilter/avfilter.h"
#include "avfilter/avfilter_all.h"
#include <stdarg.h>
#include "version_xav.h"
#include <xutils/debug.h>
#include "app_player_mixer.h"

#define TAG                    "player_mixer"

static player_mixer_mgr_t g_mp_mgr = {0};

static void mixer_manager(void* arg);

static player_mixer_entry_t* find_free_slot(void) {
    for (int i = 0; i < MAX_MP_PLAYERS; i++) {
        if (!g_mp_mgr.entries[i].in_use && !g_mp_mgr.entries[i].active) {
            return &g_mp_mgr.entries[i];
        }
    }
    return NULL;
}

static player_mixer_entry_t* find_by_id(uint8_t id) {
    for (int i = 0; i < MAX_MP_PLAYERS; i++) {
        if (g_mp_mgr.entries[i].in_use &&
            g_mp_mgr.entries[i].id == id) {
            return &g_mp_mgr.entries[i];
        }
    }
    return NULL;
}

static void mp_player_event_callback(void *user_data, uint8_t type,const void *data, uint32_t len) {
    xplayer_t *player = (xplayer_t *)user_data;
    player_mixer_entry_t *entry = NULL;
    xplayer_t *to_free = NULL;

    for (int i = 0; i < MAX_MP_PLAYERS; i++) {
        if (g_mp_mgr.entries[i].in_use &&
            g_mp_mgr.entries[i].player == player) {
            entry = &g_mp_mgr.entries[i];
            break;
        }
    }

    if (entry != NULL) {
        msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
        printf("[MP] Player %d event: %d\n", entry->id, type);
        switch (type) {
            case XPLAYER_EVENT_START:
                entry->active = 1;
                printf("[MP] Player %d started\n", entry->id);
                break;
            case XPLAYER_EVENT_FINISH:
                entry->active = 0;
                entry->in_use = 0;
                to_free = entry->player;
                entry->player = NULL;
                printf("[MP] Player %d finished\n", entry->id);
                break;
            case XPLAYER_EVENT_ERROR:
                entry->in_use = 0;
                entry->active = 0;
                to_free = entry->player;
                entry->player = NULL;
                printf("[MP] Player %d error\n", entry->id);
                break;
            case XPLAYER_EVENT_UNDER_RUN:
                printf("[MP] Player %d underrun\n", entry->id);
                break;
        }
        msp_mutex_unlock(&entry->lock);
    }

    if (to_free) {
        xplayer_free(to_free);
    }
}

/**
 * @brief  init player mixer manager
 * @return 0/-1
 */
int8_t player_mixer_init(void) {
    if (g_mp_mgr.initialized) {
        printf("[MP] Manager already initialized\n");
        return -1;
    }

    memset(&g_mp_mgr, 0, sizeof(g_mp_mgr));

    for (int i = 0; i < MAX_MP_PLAYERS; i++) {
        g_mp_mgr.entries[i].id = i;
        if (msp_mutex_new(&g_mp_mgr.entries[i].lock) < 0) {
            printf("[MP] Failed to create mutex for entry %d\n", i);
            return -1;
        }
    }

    g_mp_mgr.mixer_enabled = config_av_mixer_enable();
    if (!g_mp_mgr.mixer_enabled) {
        printf("[MP] ERROR: AO mixer not enabled, enabling now\n");
        for (int i = 0; i < MAX_MP_PLAYERS; i++) {
            msp_mutex_free(&g_mp_mgr.entries[i].lock);
        }
        return -1;
    }

    g_mp_mgr.mixer_message_queue = xQueueCreate(MIXER_MSG_QUEUE_SIZE, sizeof(player_mixer_msg_t));
    if (!g_mp_mgr.mixer_message_queue) {
        printf("[MP] Failed to create mixer message queue\n");
        return -1;
    }

    xplayer_module_init(NULL);
    g_mp_mgr.initialized = 1;

    xTaskCreate(mixer_manager, "mixer_manager", 512, NULL, MIXER_MANAGER_TASK_PRIO, &g_mp_mgr.mixer_manager_task);

    return 0;
}

/**
 * @brief  deinit player mixer manager
 * @return 0/-1
 */
int8_t player_mixer_deinit(void) {
    if (!g_mp_mgr.initialized) {
        printf("[MP] Multiplayer not initialized\n");
        return -1;
    }

    player_mixer_stop_all();

    for (int i = 0; i < MAX_MP_PLAYERS; i++) {
        msp_mutex_free(&g_mp_mgr.entries[i].lock);
    }

    g_mp_mgr.initialized = 0;

    printf("[MP] Manager deinitialized\n");
    return 0;
}

/**
 * @brief  start a new player by url
 * @param  [in] url
 * @param  [in] volume (0~100)
 * @return player id (0..MAX_MP_PLAYERS-1) / -1 on error
 */
int8_t player_mixer_start(char *url, uint8_t volume) {
    if (!g_mp_mgr.initialized) {
        printf("[MP] Manager not initialized\n");
        return -1;
    }

    if (!url || strlen(url) == 0) {
        printf("[MP] Invalid URL\n");
        return -1;
    }

    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;

    player_mixer_entry_t *entry = find_free_slot();
    if (!entry) {
        printf("[MP] No free slot available (max: %d)\n", MAX_MP_PLAYERS);
        return -1;
    }

    xplayer_t *player = xplayer_new(NULL);
    if (!player) {
        printf("[MP] Failed to create xplayer\n");
        return -1;
    }

    xplayer_cnf_t xconf;
    memset(&xconf, 0, sizeof(xconf));
    xplayer_get_config(player, &xconf);
    xconf.resample_rate       = 48000;
    xconf.web_cache_size      = 8 * 1024;
    xconf.web_start_threshold = 10;
    xconf.snd_period_ms       = 10;
    xconf.snd_period_num      = 5;
    xconf.db_min              = VOL_SCALE_DB_MIN;
    xconf.db_max              = VOL_SCALE_DB_MAX;
    xconf.eq_segments         = 0;
    xconf.aef_conf            = NULL;
    xconf.aef_conf_size       = 0;
    xplayer_set_config(player, &xconf);

    xplayer_set_callback(player, mp_player_event_callback, (void*)player);

    msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
    entry->player = player;
    strncpy(entry->url, url, MP_URL_MAX_LEN - 1);
    entry->url[MP_URL_MAX_LEN - 1] = '\0';
    entry->volume = volume;
    entry->in_use = 1;
    entry->active = 0;
    int player_id = entry->id;
    msp_mutex_unlock(&entry->lock);

    xplayer_set_vol(player, player_mixer_vol_100_to_255(volume));
    xplayer_set_url(player, url);
    int ret = xplayer_play(player);
    if (ret != 0) {
        printf("[MP] Failed to start playing: %d\n", ret);
        msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
        entry->in_use = 0;
        entry->player = NULL;
        msp_mutex_unlock(&entry->lock);
        xplayer_free(player);
        return -1;
    }
    printf("[MP] Started player %d: %s (vol=%d)\n", player_id, url, volume);
    return player_id;
}

/**
 * @brief  stop a player by id
 * @param  [in] id
 * @return 0/-1
 */
static int8_t __player_mixer_stop(uint8_t id) {
    if (!g_mp_mgr.initialized) {
        printf("[MP] Manager not initialized\n");
        return -1;
    }

    player_mixer_entry_t *entry = find_by_id(id);
    if (!entry) {
        printf("[MP] Player %d not found\n", id);
        return -1;
    }

    xplayer_t *player = NULL;
    msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
    player = entry->player;
    entry->in_use = 0;
    entry->active = 0;
    entry->player = NULL;
    msp_mutex_unlock(&entry->lock);

    if (player) {
        xplayer_stop(player);
        xplayer_free(player);
    }

    printf("[MP] Stop requested for player %d\n", id);
    return 0;
}

/**
 * @brief  pause a player by id
 * @param  [in] id
 * @return 0/-1
 */
static int8_t __player_mixer_pause(uint8_t id) {
    if (!g_mp_mgr.initialized) {
        printf("[MP] Manager not initialized\n");
        return -1;
    }

    player_mixer_entry_t *entry = find_by_id(id);
    if (!entry) {
        printf("[MP] Player %d not found\n", id);
        return -1;
    }
    xplayer_t *player = NULL;

    msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
    player = entry->player;
    entry->active = 0;
    msp_mutex_unlock(&entry->lock);

    if (player) {
        xplayer_pause(player);
    }

    printf("[MP] Paused player %d\n", id);
    return 0;
}

/**
 * @brief  resume a player by id
 * @param  [in] id
 * @return 0/-1
 */
static int8_t __player_mixer_resume(uint8_t id) {
    if (!g_mp_mgr.initialized) {
        printf("[MP] Manager not initialized\n");
        return -1;
    }

    player_mixer_entry_t *entry = find_by_id(id);
    if (!entry) {
        printf("[MP] Player %d not found\n", id);
        return -1;
    }

    xplayer_t *player = NULL;

    msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
    player = entry->player;
    entry->active = 1;
    msp_mutex_unlock(&entry->lock);

    if (player) {
        xplayer_resume(player);
    }

    printf("[MP] Resumed player %d\n", id);
    return 0;
}

/**
 * @brief  set volume of a player by id
 * @param  [in] id
 * @param  [in] volume (0~100)
 * @return 0/-1
 */
static int8_t __player_mixer_vol(uint8_t id, uint32_t volume) {
    if (!g_mp_mgr.initialized) {
        printf("[MP] Manager not initialized\n");
        return -1;
    }

    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;

    player_mixer_entry_t *entry = find_by_id(id);
    if (!entry) {
        printf("[MP] Player %d not found\n", id);
        return -1;
    }

    xplayer_t *player = NULL;

    msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
    player = entry->player;

    int ret = xplayer_set_vol(player, player_mixer_vol_100_to_255(volume));
    if (ret == 0) {
        entry->volume = volume;
        printf("[MP] Set player %d volume to %d\n", id, volume);
    } else {
        printf("[MP] Failed to set volume for player %d\n", id);
    }
    msp_mutex_unlock(&entry->lock);
    return ret;
}

/**
 * @brief  set play speed of a player
 * @param  [in] id
 * @param  [in] speed
 * @return 0/-1
 */
static int8_t __player_mixer_set_speed(uint8_t id, float speed) {
    if (!g_mp_mgr.initialized) {
        return -1;
    }
    player_mixer_entry_t *entry = find_by_id(id);
    if (!entry) {
        return -1;
    }
    xplayer_t *player = NULL;
    msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
    player = entry->player;
    msp_mutex_unlock(&entry->lock);
    if (!player) return -1;
    return xplayer_set_speed(player, speed);
}

/**
 * @brief  stop all players
 * @return number of players requested to stop
 */
static int8_t __player_mixer_stop_all(void) {
    if (!g_mp_mgr.initialized) {
        return 0;
    }

    printf("[MP] Stopping all players...\n");

    xplayer_t *players[MAX_MP_PLAYERS] = {0};
    int count = 0;

    for (int i = 0; i < MAX_MP_PLAYERS; i++) {
        if (g_mp_mgr.entries[i].in_use) {
            msp_mutex_lock(&g_mp_mgr.entries[i].lock, MSP_WAIT_FOREVER);
            g_mp_mgr.entries[i].in_use = 0;
            g_mp_mgr.entries[i].active = 0;
            players[count++] = g_mp_mgr.entries[i].player;
            g_mp_mgr.entries[i].player = NULL;
            msp_mutex_unlock(&g_mp_mgr.entries[i].lock);
        }
    }

    for (int i = 0; i < count; i++) {
        if (players[i]) {
            xplayer_stop(players[i]);
            xplayer_free(players[i]);
        }
    }
    printf("[MP] Stop requested for %d players\n", count);
    return count;
}

/**
 * @brief  pause all players
 * @return number of players requested to pause
 */
static int8_t __player_mixer_pause_all(void) {
    if (!g_mp_mgr.initialized) {
        return 0;
    }

    printf("[MP] Pausing all players...\n");

    xplayer_t *players[MAX_MP_PLAYERS] = {0};
    int count = 0;

    for (int i = 0; i < MAX_MP_PLAYERS; i++) {
        if (g_mp_mgr.entries[i].in_use) {
            msp_mutex_lock(&g_mp_mgr.entries[i].lock, MSP_WAIT_FOREVER);
            players[count++] = g_mp_mgr.entries[i].player;
            g_mp_mgr.entries[i].active = 0;
            msp_mutex_unlock(&g_mp_mgr.entries[i].lock);
        }
    }

    for (int i = 0; i < count; i++) {
        if (players[i]) {
            xplayer_pause(players[i]);
        }
    }

    printf("[MP] Paused %d players\n", count);
    return count;
}

/**
 * @brief  resume all players
 * @return number of players requested to resume
 */
static int8_t __player_mixer_resume_all(void) {
    if (!g_mp_mgr.initialized) {
        return 0;
    }

    printf("[MP] Resuming all players...\n");

    xplayer_t *players[MAX_MP_PLAYERS] = {0};
    int count = 0;

    for (int i = 0; i < MAX_MP_PLAYERS; i++) {
        if (g_mp_mgr.entries[i].in_use) {
            msp_mutex_lock(&g_mp_mgr.entries[i].lock, MSP_WAIT_FOREVER);
            players[count++] = g_mp_mgr.entries[i].player;
            g_mp_mgr.entries[i].active = 1;
            msp_mutex_unlock(&g_mp_mgr.entries[i].lock);
        }
    }

    for (int i = 0; i < count; i++) {
        if (players[i]) {
            xplayer_resume(players[i]);
        }
    }

    printf("[MP] Resumed %d players\n", count);
    return count;
}

/**
 * @brief  stop a player by id
 * @param  [in] id
 * @return 1/0
 */
 int8_t player_mixer_stop(uint8_t id) {
    player_mixer_msg_t msg = {
        .target_id = id,
        .type = MSG_STOP,
    };
    return xQueueSend(g_mp_mgr.mixer_message_queue, &msg, MSP_WAIT_FOREVER);
}

/**
 * @brief  pause a player by id
 * @param  [in] id
 * @return 1/0
 */
int8_t player_mixer_pause(uint8_t id) {
    player_mixer_msg_t msg = {
        .target_id = id,
        .type = MSG_PAUSE,
    };
    return xQueueSend(g_mp_mgr.mixer_message_queue, &msg, MSP_WAIT_FOREVER);
}

/**
 * @brief  resume a player by id
 * @param  [in] id
 * @return 1/0
 */
int8_t player_mixer_resume(uint8_t id) {
    player_mixer_msg_t msg = {
        .target_id = id,
        .type = MSG_RESUME,
    };
    return xQueueSend(g_mp_mgr.mixer_message_queue, &msg, MSP_WAIT_FOREVER);
}

/**
 * @brief  set volume of a player by id
 * @param  [in] id
 * @param  [in] volume (0~100)
 * @return 1/0
 */
int8_t player_mixer_vol(uint8_t id, uint32_t volume) {
    player_mixer_msg_t msg = {
        .target_id = id,
        .type = MSG_VOL_SET,
        .params.volume = volume
    };
    return xQueueSend(g_mp_mgr.mixer_message_queue, &msg, MSP_WAIT_FOREVER);
}
/**
 * @brief  set play speed of a player
 * @param  [in] id
 * @param  [in] speed
 * @return 1/0
 */
int8_t player_mixer_set_speed(uint8_t id, float speed) {
    player_mixer_msg_t msg = {
        .target_id = id,
        .type = MSG_SPEED_SET,
        .params.speed = speed,
    };
    return xQueueSend(g_mp_mgr.mixer_message_queue, &msg, MSP_WAIT_FOREVER);
}

/**
 * @brief  stop all players
 * @return 1/0
 */
 int8_t player_mixer_stop_all(void) {
    player_mixer_msg_t msg = {
        .target_id = 0,
        .type = MSG_STOP_ALL,
    };
    return xQueueSend(g_mp_mgr.mixer_message_queue, &msg, MSP_WAIT_FOREVER);
}

/**
 * @brief  pause all players
 * @return 1/0
 */
int8_t player_mixer_pause_all(void) {
    player_mixer_msg_t msg = {
        .target_id = 0,
        .type = MSG_PAUSE_ALL,
    };
    return xQueueSend(g_mp_mgr.mixer_message_queue, &msg, MSP_WAIT_FOREVER);
}

/**
 * @brief  resume all players
 * @return 1/0
 */
int8_t player_mixer_resume_all(void) {
    player_mixer_msg_t msg = {
        .target_id = 0,
        .type = MSG_RESUME_ALL,
    };
    return xQueueSend(g_mp_mgr.mixer_message_queue, &msg, MSP_WAIT_FOREVER);
}

/**
 * @brief  get play speed of a player
 * @param  [in] id
 * @param  [out] speed
 * @return 0/-1
 */
int8_t player_mixer_get_speed(uint8_t id, float *speed) {
    if (!g_mp_mgr.initialized || !speed) {
        return -1;
    }
    player_mixer_entry_t *entry = find_by_id(id);
    if (!entry) {
        return -1;
    }
    xplayer_t *player = NULL;
    msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
    player = entry->player;
    msp_mutex_unlock(&entry->lock);
    if (!player) return -1;
    return xplayer_get_speed(player, speed);
}

/**
 * @brief  get current play time of a player
 * @param  [in] id
 * @param  [out] ptime
 * @return 0/-1
 */
int8_t player_mixer_get_curttime(uint8_t id, xplay_time_t *ptime) {
    if (!g_mp_mgr.initialized || !ptime) {
        return -1;
    }
    player_mixer_entry_t *entry = find_by_id(id);
    if (!entry) {
        return -1;
    }
    xplayer_t *player = NULL;
    msp_mutex_lock(&entry->lock, MSP_WAIT_FOREVER);
    player = entry->player;
    msp_mutex_unlock(&entry->lock);
    if (!player) return -1;
    return xplayer_get_time(player, ptime);
}

/**
 * @brief  print players info
 * @return 0/-1
 */
int8_t player_mixer_info() {
    if (!g_mp_mgr.initialized) {
        return -1;
    }

    for (int i = 0; i < MAX_MP_PLAYERS; i++) {
        if (g_mp_mgr.entries[i].in_use) {
            msp_mutex_lock(&g_mp_mgr.entries[i].lock, MSP_WAIT_FOREVER);
            printf("[MP] Player %d: %s (vol=%d), active=%d\n", g_mp_mgr.entries[i].id,g_mp_mgr.entries[i].url,g_mp_mgr.entries[i].volume,g_mp_mgr.entries[i].active);
            msp_mutex_unlock(&g_mp_mgr.entries[i].lock);
        }
    }
    return 0;
}

static void mixer_manager(void *arg) {
    while (1) {
        player_mixer_msg_t msg;
        if (xQueueReceive(g_mp_mgr.mixer_message_queue, &msg, MSP_WAIT_FOREVER) == pdPASS) {
            printf("[MP] Received message: %d\n", msg.type);
            switch (msg.type) {
                case MSG_STOP:
                    printf("[MP] Stop player %d\n", msg.target_id);
                    __player_mixer_stop(msg.target_id);
                    break;
                case MSG_PAUSE:
                    printf("[MP] Pause player %d\n", msg.target_id);
                    __player_mixer_pause(msg.target_id);
                    break;
                case MSG_RESUME:
                    printf("[MP] Resume player %d\n", msg.target_id);
                    __player_mixer_resume(msg.target_id);
                    break;
                case MSG_VOL_SET:
                    printf("[MP] Set volume of player %d to %d\n", msg.target_id, msg.params.volume);
                    __player_mixer_vol(msg.target_id, msg.params.volume);
                    break;
                case MSG_SPEED_SET:
                    printf("[MP] Set speed of player %d to %f\n", msg.target_id, msg.params.speed);
                    __player_mixer_set_speed(msg.target_id, msg.params.speed);
                    break;
                case MSG_STOP_ALL:
                    printf("[MP] Stop all players\n");
                    __player_mixer_stop_all();
                    break;
                case MSG_PAUSE_ALL:
                    printf("[MP] Pause all players\n");
                    __player_mixer_pause_all();
                    break;
                case MSG_RESUME_ALL:
                    printf("[MP] Resume all players\n");
                    __player_mixer_resume_all();
                    break;
                default:
                    printf("[MP] Unknown message type: %d\n", msg.type);
                    break;
            }
        }
    }
}
