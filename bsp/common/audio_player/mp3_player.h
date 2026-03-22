#ifndef MP3_PLAYER_H
#define MP3_PLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef CONFIG_MP3_PLAYER_TEST_MP3
#define MP3_PLAYER_TEST_MP3 0
#else
#define MP3_PLAYER_TEST_MP3 CONFIG_MP3_PLAYER_TEST_MP3
#endif

#ifndef CONFIG_MP3_PLAYER_TASK_STACK_DEPTH
#define MP3_PLAYER_TASK_STACK_DEPTH 376U
#else
#define MP3_PLAYER_TASK_STACK_DEPTH CONFIG_MP3_PLAYER_TASK_STACK_DEPTH
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int (*init)(void *user_data);
    int (*read)(void *user_data, uint8_t *buf, uint32_t len);
    void (*close)(void *user_data);
} audio_mp3_ops_t;

typedef struct {
    audio_mp3_ops_t ops;
    void *user_data;
} audio_mp3_task_cfg_t;

int mp3_player_start(audio_mp3_task_cfg_t *cfg);
int mp3_player_pause(void);
int mp3_player_resume(void);
int mp3_player_stop(void);
bool mp3_player_is_running(void);
void mp3_player_dump_stack(void);

#ifdef __cplusplus
}
#endif

#endif /* MP3_PLAYER_H */
