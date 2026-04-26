/**
 * @file mm_track.h
 * @brief Memory allocation leak tracing based on backtrace callsites
 */

#ifndef __MM_TRACK_H__
#define __MM_TRACK_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MM_TRACK_SLOT_BYTES 4U

#ifndef CONFIG_MM_TRACK_CALLSITE_TABLE_SIZE
#define CONFIG_MM_TRACK_CALLSITE_TABLE_SIZE 1024
#endif

#ifndef CONFIG_MM_TRACK_DATA_SIZE
#define CONFIG_MM_TRACK_DATA_SIZE 4096
#endif

#ifndef CONFIG_MM_TRACK_BACKTRACE_DEPTH
#define CONFIG_MM_TRACK_BACKTRACE_DEPTH 5
#endif

#ifndef CONFIG_MM_TRACK_SKIP_FRAMES
#define CONFIG_MM_TRACK_SKIP_FRAMES 2
#endif

int mm_track_start(uint16_t callsite_cap, uint16_t data_cap, uint8_t depth, uint8_t skip_frames);
void mm_track_malloc(void *ptr, size_t block_size);
void mm_track_free(void *ptr, size_t block_size);
void mm_track_free_revoke(void *ptr, size_t block_size, bool is_untracked);
void mm_track_dump(void);
uint32_t mm_track_clear(void);
bool mm_track_is_started(void);
bool mm_track_is_untracked_ptr(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* __MM_TRACK_H__ */
