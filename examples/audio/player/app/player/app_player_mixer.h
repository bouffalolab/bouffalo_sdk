#ifndef __APP_PLAYER_MIXER_H__
#define __APP_PLAYER_MIXER_H__

#include "avutil/common.h"
#include "stream/stream.h"
#include "stream/stream_all.h"
#include "avformat/avformat_all.h"
#include "avformat/avformat.h"
#include "avcodec/avcodec_all.h"
#include "output/ao.h"
#include "output/output_all.h"
#include "aef/eq_all.h"
#include "aef/aef_all.h"
#include "atempo/atempo_all.h"
#include "swresample/resample_all.h"
#include "xplayer/xplayer.h"
#include "xplayer/xplayer_typedef.h"

/* Configuration switch for controlling player mixer support. */
#define CONFIG_PLAYER_MIXER_ENABLE           (1)

#define MAX_MP_PLAYERS          4
#define MP_URL_MAX_LEN          256
#define MP_TASK_NAME_LEN        32
#define MIXER_MANAGER_TASK_PRIO 25
#define MIXER_MSG_QUEUE_SIZE    10

typedef struct pm_entry {
    int             id;
    xplayer_t       *player;
    msp_mutex_t     lock;
    char            url[MP_URL_MAX_LEN];
    int             volume;
    int             in_use;
    int             active;
} player_mixer_entry_t;

typedef struct pm_mgr {
    int                     initialized;
    int                     mixer_enabled;
    TaskHandle_t            mixer_manager_task;
    QueueHandle_t           mixer_message_queue;
    player_mixer_entry_t    entries[MAX_MP_PLAYERS];
} player_mixer_mgr_t;

typedef struct pm_info {
    int             id;
    char            url[MP_URL_MAX_LEN];
    int             volume;
    int             active;
} player_mixer_info_t;

typedef enum {
    MSG_PAUSE,
    MSG_PAUSE_ALL,
    MSG_RESUME,
    MSG_RESUME_ALL,
    MSG_STOP,
    MSG_STOP_ALL,
    MSG_VOL_SET,
    MSG_SPEED_SET
}player_mixer_msg_type_t;

typedef struct {
    player_mixer_msg_type_t    type;
    int                        target_id;
    union {
        uint32_t volume;
        float speed;
    } params;
}player_mixer_msg_t;
/**
 * @brief  init player mixer manager
 * @return 0/-1
 */
int8_t player_mixer_init(void);

/**
 * @brief  deinit player mixer manager
 * @return 0/-1
 */
int8_t player_mixer_deinit(void);

/**
 * @brief  start a new player by url
 * @param  [in] url
 * @param  [in] volume (0~100)
 * @return player id (0..MAX_MP_PLAYERS-1) / -1 on error
 */
int8_t player_mixer_start(char *url, uint8_t volume);

/**
 * @brief  stop a player by id
 * @param  [in] id
 * @return 0/-1
 */
int8_t player_mixer_stop(uint8_t id);

/**
 * @brief  pause a player by id
 * @param  [in] id
 * @return 0/-1
 */
int8_t player_mixer_pause(uint8_t id);

/**
 * @brief  resume a player by id
 * @param  [in] id
 * @return 0/-1
 */
int8_t player_mixer_resume(uint8_t id);

/**
 * @brief  set play speed of a player
 * @param  [in] id
 * @param  [in] speed
 * @return 1/0
 */
int8_t player_mixer_set_speed(uint8_t id, float speed);

/**
 * @brief  set volume of a player by id
 * @param  [in] id
 * @param  [in] volume (0~100)
 * @return 1/0
 */
int8_t player_mixer_vol(uint8_t id, uint32_t volume);

/**
 * @brief  stop all players
 * @return number of players requested to stop
 */
int8_t player_mixer_stop_all(void);

/**
 * @brief  pause all players
 * @return number of players requested to pause
 */
int8_t player_mixer_pause_all(void);

/**
 * @brief  resume all players
 * @return number of players requested to resume
 */
int8_t player_mixer_resume_all(void);

/**
 * @brief  print players info
 * @return 0/-1
 */
int8_t player_mixer_info();

/**
 * @brief  get play speed of a player
 * @param  [in] id
 * @param  [out] speed
 * @return 0/-1
 */
int8_t player_mixer_get_speed(uint8_t id, float *speed);

/**
 * @brief  get current play time of a player
 * @param  [in] id
 * @param  [out] ptime
 * @return 0/-1
 */
int8_t player_mixer_get_curttime(uint8_t id, xplay_time_t *ptime);

static inline int player_mixer_vol_100_to_255(uint8_t vol_100) {
    return (vol_100 * 255) / 100;
}
#endif
