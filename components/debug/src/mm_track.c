/**
 * @file mm_track.c
 * @brief Memory allocation leak tracing based on backtrace callsites
 */

#include "mm_track.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "backtrace.h"
#include "mm.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define DBG_TAG "MM_LEAK"
#include "log.h"

extern uintptr_t mm_lock_save(void);
extern void mm_unlock_restore(uintptr_t flags);

typedef struct {
    uint32_t hash;
    uint16_t data_idx;
    uint16_t malloc_count;
    uint16_t free_count;
    uint8_t depth;
    uint8_t used;
} mm_track_entry_t;

typedef struct {
    mm_track_entry_t *entries;
    uint32_t *bt_data;
    uintptr_t *untracked_ptrs;
    uint32_t entry_used;
    uint32_t bt_data_used;
    uint32_t drop_count;
    uint32_t untracked_cap;
    uint32_t untracked_count;
    uint16_t runtime_callsite_cap;
    uint16_t runtime_data_cap;
    uint8_t runtime_depth;
    uint8_t runtime_skip;
} mm_track_ctx_t;

static mm_track_ctx_t *g_mm_track_ctx;

#define MM_LEAK_AUTO_CLEAR_WATERMARK_PCT 90U
#define MM_LEAK_ENTRY_SLOT_INVALID 0xFFFFFFFFU

#define MM_TRACK_UNTRACKED_SLOT_EMPTY   ((uintptr_t)0U)
#define MM_TRACK_UNTRACKED_SLOT_DELETED ((uintptr_t)1U)

static bool mm_track_untracked_probe(uintptr_t *untracked_ptrs, uint32_t untracked_cap, uintptr_t ptr,
                                     uint32_t *match_idx, uint32_t *insert_idx);
static bool mm_track_untracked_insert(uintptr_t *untracked_ptrs, uint32_t untracked_cap, uintptr_t ptr);
static bool mm_track_untracked_contains(uintptr_t *untracked_ptrs, uint32_t untracked_cap, uintptr_t ptr);
static bool mm_track_untracked_remove(uintptr_t *untracked_ptrs, uint32_t untracked_cap, uintptr_t ptr);

static void mm_track_report_capacity_hint_locked(const char *type, uint32_t hash)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;

    LOG_E("Leak trace %s overflow: hash=0x%08lx, dropped=%lu\r\n",
          type,
          (unsigned long)hash,
          (unsigned long)ctx->drop_count);
    LOG_E("Increase limits and retest: mm_track start <callsite_cap> <data_cap> <depth> <skip>\r\n");
    LOG_E("Current limits: callsite=%u data=%u depth=%u skip=%u\r\n",
          ctx->runtime_callsite_cap, ctx->runtime_data_cap, ctx->runtime_depth, ctx->runtime_skip);
}

static uint32_t mm_track_clear_balanced_locked(void)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;
    uint32_t cleared = 0;
    uint32_t write_idx = 0;
    uint32_t old_bt_data_used;

    if (!ctx || !ctx->entries) {
        return 0;
    }

    for (uint32_t i = 0; i < ctx->runtime_callsite_cap; i++) {
        mm_track_entry_t *entry = &ctx->entries[i];

        if (!entry->used) {
            continue;
        }

        if (entry->malloc_count == entry->free_count) {
            memset(entry, 0, sizeof(*entry));
            cleared++;
            continue;
        }

        if (((uint32_t)entry->data_idx + entry->depth) > ctx->runtime_data_cap) {
            memset(entry, 0, sizeof(*entry));
            cleared++;
        }
    }

    old_bt_data_used = ctx->bt_data_used;

    /*
     * Compact backtrace data in source-order, not entry-table order.
     * Entry slots can be reused, so table order may no longer match
     * the original bt_data layout. Compacting by table order can
     * overwrite yet-to-be-copied live backtraces.
     */
    for (uint32_t read_idx = 0; read_idx < old_bt_data_used;) {
        mm_track_entry_t *entry = NULL;

        for (uint32_t i = 0; i < ctx->runtime_callsite_cap; i++) {
            mm_track_entry_t *candidate = &ctx->entries[i];

            if (!candidate->used || candidate->depth == 0U) {
                continue;
            }

            if (candidate->data_idx == read_idx) {
                entry = candidate;
                break;
            }
        }

        if (!entry) {
            read_idx++;
            continue;
        }

        if (entry->data_idx != write_idx) {
            memmove(&ctx->bt_data[write_idx], &ctx->bt_data[entry->data_idx], entry->depth * sizeof(uint32_t));
        }
        entry->data_idx = (uint16_t)write_idx;
        write_idx += entry->depth;
        read_idx += entry->depth;
    }

    for (uint32_t i = 0; i < ctx->runtime_callsite_cap; i++) {
        mm_track_entry_t *entry = &ctx->entries[i];

        if (!entry->used || entry->depth != 0U) {
            continue;
        }

        entry->data_idx = (uint16_t)write_idx;
    }

    if (cleared > ctx->entry_used) {
        ctx->entry_used = 0;
    } else {
        ctx->entry_used -= cleared;
    }

    if (ctx->bt_data && write_idx < ctx->bt_data_used) {
        memset(&ctx->bt_data[write_idx], 0, (ctx->bt_data_used - write_idx) * sizeof(uint32_t));
    }
    ctx->bt_data_used = write_idx;

    return cleared;
}

static inline void mm_track_count_inc_u16(uint16_t *count)
{
    if (*count != 0xFFFFU) {
        (*count)++;
    }
}

static inline uint32_t mm_track_hash_ptr(uintptr_t ptr)
{
    uint32_t x = (uint32_t)(ptr >> 2);

    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

static bool mm_track_untracked_probe(uintptr_t *untracked_ptrs, uint32_t untracked_cap, uintptr_t ptr,
                                     uint32_t *match_idx, uint32_t *insert_idx)
{
    uint32_t first_deleted = 0;
    bool has_deleted = false;

    if (!untracked_ptrs || untracked_cap == 0U || ptr <= MM_TRACK_UNTRACKED_SLOT_DELETED) {
        return false;
    }

    if (match_idx) {
        *match_idx = 0;
    }
    if (insert_idx) {
        *insert_idx = 0;
    }

    for (uint32_t step = 0; step < untracked_cap; step++) {
        uint32_t idx = (mm_track_hash_ptr(ptr) + step) & (untracked_cap - 1U);
        uintptr_t slot = untracked_ptrs[idx];

        if (slot == ptr) {
            if (match_idx) {
                *match_idx = idx;
            }
            return true;
        }

        if (slot == MM_TRACK_UNTRACKED_SLOT_DELETED) {
            if (!has_deleted) {
                first_deleted = idx;
                has_deleted = true;
            }
            continue;
        }

        if (slot == MM_TRACK_UNTRACKED_SLOT_EMPTY) {
            if (insert_idx) {
                *insert_idx = has_deleted ? first_deleted : idx;
            }
            return false;
        }
    }

    if (insert_idx && has_deleted) {
        *insert_idx = first_deleted;
    }

    return false;
}

static bool mm_track_untracked_insert(uintptr_t *untracked_ptrs, uint32_t untracked_cap, uintptr_t ptr)
{
    uint32_t match_idx;
    uint32_t insert_idx;

    if (mm_track_untracked_probe(untracked_ptrs, untracked_cap, ptr, &match_idx, &insert_idx)) {
        return true;
    }

    if (!untracked_ptrs || untracked_cap == 0U || ptr <= MM_TRACK_UNTRACKED_SLOT_DELETED) {
        return false;
    }

    untracked_ptrs[insert_idx] = ptr;
    return true;
}

static bool mm_track_untracked_contains(uintptr_t *untracked_ptrs, uint32_t untracked_cap, uintptr_t ptr)
{
    return mm_track_untracked_probe(untracked_ptrs, untracked_cap, ptr, NULL, NULL);
}

static bool mm_track_untracked_remove(uintptr_t *untracked_ptrs, uint32_t untracked_cap, uintptr_t ptr)
{
    uint32_t match_idx;

    if (mm_track_untracked_probe(untracked_ptrs, untracked_cap, ptr, &match_idx, NULL)) {
        if (untracked_ptrs) {
            untracked_ptrs[match_idx] = MM_TRACK_UNTRACKED_SLOT_DELETED;
        }
        return true;
    }

    return false;
}

static uint32_t mm_track_maybe_auto_clear_locked(void)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;
    bool near_entry = false;
    bool near_bt_data = false;

    if (ctx->runtime_callsite_cap > 0U) {
        near_entry = (ctx->entry_used * 100U) >= (ctx->runtime_callsite_cap * MM_LEAK_AUTO_CLEAR_WATERMARK_PCT);
    }
    if (ctx->runtime_data_cap > 0U) {
        near_bt_data = (ctx->bt_data_used * 100U) >= (ctx->runtime_data_cap * MM_LEAK_AUTO_CLEAR_WATERMARK_PCT);
    }

    if (near_entry || near_bt_data) {
        uint32_t cleared = mm_track_clear_balanced_locked();
        if (cleared > 0U) {
            LOG_W("Auto clear balanced entries: %lu\r\n", (unsigned long)cleared);
        }
        return cleared;
    }

    return 0;
}

static void mm_track_release_ctx(mm_track_ctx_t *ctx)
{
    if (!ctx) {
        return;
    }

    if (ctx->entries) {
        free(ctx->entries);
    }
    if (ctx->bt_data) {
        free(ctx->bt_data);
    }
    if (ctx->untracked_ptrs) {
        free(ctx->untracked_ptrs);
    }
    free(ctx);
}

static inline uintptr_t get_sp(void)
{
    uintptr_t sp;
    __asm__ volatile("mv %0, sp" : "=r"(sp));
    return sp;
}

static inline uintptr_t get_pc(void)
{
    uintptr_t pc;
    __asm__ volatile("auipc %0, 0" : "=r"(pc));
    return pc;
}

static inline uint32_t mm_track_rotl32(uint32_t x, uint8_t r)
{
    return (x << r) | (x >> (32U - r));
}

static uint32_t mm_track_hash_finalize32(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

static uint32_t mm_track_hash_compute_bt(const uint32_t *addrs, uint32_t depth)
{
    uint32_t hash = 0x9747b28cU ^ (depth * 0x9e3779b9U);

    for (uint32_t i = 0; i < depth; i++) {
        uint32_t k = (addrs[i] & 0xFFFFU) | (i << 16);
        k ^= k >> 16;
        k *= 0x85ebca6bU;
        k ^= k >> 13;
        k *= 0xc2b2ae35U;
        k ^= k >> 16;
        hash ^= k;
        hash = mm_track_rotl32(hash, 13);
        hash = hash * 5U + 0xe6546b64U;
    }

    return mm_track_hash_finalize32(hash ^ depth);
}

static uint32_t mm_track_capture_backtrace(uint32_t *addrs, uint32_t max_depth)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;
    uint32_t raw[CONFIG_MM_TRACK_BACKTRACE_DEPTH + CONFIG_MM_TRACK_SKIP_FRAMES];
    uint32_t sp = (uint32_t)get_sp();
    uint32_t pc = (uint32_t)get_pc();
    int raw_count;
    uint32_t out_count;

    raw_count = backtrace_unwind(pc, sp, raw, (int)(CONFIG_MM_TRACK_BACKTRACE_DEPTH + CONFIG_MM_TRACK_SKIP_FRAMES));
    if (raw_count <= 0 || raw_count <= ctx->runtime_skip) {
        return 0;
    }

    out_count = (uint32_t)(raw_count - ctx->runtime_skip);
    if (out_count > max_depth) {
        out_count = max_depth;
    }

    memcpy(addrs, &raw[ctx->runtime_skip], out_count * sizeof(uint32_t));
    return out_count;
}

static void mm_track_write_tail_index(void *ptr, size_t user_size, uint32_t entry_idx)
{
    memcpy((uint8_t *)ptr + user_size, &entry_idx, sizeof(entry_idx));
}

static uint32_t mm_track_read_tail_index(void *ptr, size_t user_size)
{
    uint32_t entry_idx = MM_LEAK_ENTRY_SLOT_INVALID;

    memcpy(&entry_idx, (const uint8_t *)ptr + user_size, sizeof(entry_idx));
    return entry_idx;
}

static void mm_track_clear_tail_index(void *ptr, size_t user_size)
{
    uint32_t invalid = MM_LEAK_ENTRY_SLOT_INVALID;

    memcpy((uint8_t *)ptr + user_size, &invalid, sizeof(invalid));
}

struct mm_track_untracked_capture_ctx {
    uintptr_t *untracked_ptrs;
    uint32_t untracked_cap;
    uint32_t count;
    uint32_t overflow;
};

static void mm_track_count_allocated_block(struct mm_heap *heap, void *ptr, size_t size, void *user)
{
    uint32_t *count = (uint32_t *)user;

    (void)heap;
    (void)ptr;
    (void)size;

    if (count) {
        (*count)++;
    }
}

static void mm_track_capture_untracked_block(struct mm_heap *heap, void *ptr, size_t size, void *user)
{
    struct mm_track_untracked_capture_ctx *ctx = (struct mm_track_untracked_capture_ctx *)user;

    (void)heap;
    (void)size;

    if (!ctx || !ptr) {
        return;
    }

    if (mm_track_untracked_insert(ctx->untracked_ptrs, ctx->untracked_cap, (uintptr_t)ptr)) {
        ctx->count++;
    } else {
        ctx->overflow++;
    }
}

static bool mm_track_entry_bt_equal(const mm_track_entry_t *entry, const uint32_t *addrs, uint32_t depth)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;

    if (!entry || !entry->used) {
        return false;
    }

    if (entry->depth != depth) {
        return false;
    }

    if (!ctx || (uint32_t)entry->data_idx + depth > ctx->runtime_data_cap) {
        return false;
    }

    return memcmp(&ctx->bt_data[entry->data_idx], addrs, depth * sizeof(uint32_t)) == 0;
}

static int mm_track_find_or_create_entry_locked(uint32_t hash, const uint32_t *addrs, uint32_t depth)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;
    int free_slot = -1;

    for (uint32_t i = 0; i < ctx->runtime_callsite_cap; i++) {
        mm_track_entry_t *entry = &ctx->entries[i];

        if (!entry->used) {
            if (free_slot < 0) {
                free_slot = (int)i;
            }
            continue;
        }

        if (entry->hash != hash) {
            continue;
        }

        if (mm_track_entry_bt_equal(entry, addrs, depth)) {
            return (int)i;
        }
    }

    if (free_slot < 0 || (ctx->bt_data_used + depth) > ctx->runtime_data_cap) {
        ctx->drop_count++;
        if (free_slot < 0) {
            mm_track_report_capacity_hint_locked("entry_table", hash);
        } else {
            mm_track_report_capacity_hint_locked("data_table", hash);
        }
        return -1;
    }

    {
        mm_track_entry_t *new_entry = &ctx->entries[free_slot];

        new_entry->hash = hash;
        new_entry->data_idx = (uint16_t)ctx->bt_data_used;
        new_entry->malloc_count = 0U;
        new_entry->free_count = 0U;
        new_entry->depth = (uint8_t)depth;
        new_entry->used = 1U;

        if (depth > 0U) {
            memcpy(&ctx->bt_data[ctx->bt_data_used], addrs, depth * sizeof(uint32_t));
        }

        ctx->bt_data_used += depth;
        ctx->entry_used++;
    }

    return free_slot;
}

static void mm_track_print_bt_line_by_idx(uint16_t bt_idx)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;
    mm_track_entry_t *entry;

    if (!ctx || bt_idx >= ctx->runtime_callsite_cap) {
        printf("[invalid bt_idx=%u]\r\n", (unsigned int)bt_idx);
        return;
    }

    entry = &ctx->entries[bt_idx];
    if (!entry->used || ((uint32_t)entry->data_idx + entry->depth > ctx->runtime_data_cap)) {
        printf("[invalid bt entry]\r\n");
        return;
    }

    for (uint32_t i = 0; i < entry->depth; i++) {
        uint32_t addr = ctx->bt_data[entry->data_idx + i];
        if (i > 0U) {
            printf(" ");
        }
        printf("0x%08lx", (unsigned long)addr);
    }
    printf("\r\n");
}

int mm_track_start(uint16_t callsite_cap, uint16_t data_cap, uint8_t depth, uint8_t skip_frames)
{
    uintptr_t flags;
    mm_track_ctx_t *new_ctx;
    uint32_t untracked_count = 0;
    uint32_t untracked_cap = 16;
    struct mm_track_untracked_capture_ctx untracked_ctx = { 0 };

    if (callsite_cap == 0 || data_cap == 0 ||
        depth == 0 || depth > CONFIG_MM_TRACK_BACKTRACE_DEPTH ||
        skip_frames > CONFIG_MM_TRACK_SKIP_FRAMES) {
        return -EINVAL;
    }

    flags = mm_lock_save();
    if (g_mm_track_ctx != NULL) {
        mm_unlock_restore(flags);
        LOG_W("mm_track is already started; reboot is required to restart\r\n");
        return -EALREADY;
    }
    mm_unlock_restore(flags);

    new_ctx = (mm_track_ctx_t *)calloc(1, sizeof(*new_ctx));
    if (!new_ctx) {
        return -ENOMEM;
    }

    new_ctx->entries = (mm_track_entry_t *)malloc(sizeof(mm_track_entry_t) * callsite_cap);
    new_ctx->bt_data = (uint32_t *)malloc(sizeof(uint32_t) * data_cap);
    (void)mm_walk_allocated_blocks(mm_track_count_allocated_block, &untracked_count);
    while (untracked_cap < (untracked_count * 4U + 16U)) {
        untracked_cap <<= 1;
    }
    new_ctx->untracked_ptrs = (uintptr_t *)malloc(sizeof(uintptr_t) * untracked_cap);
    if (!new_ctx->entries || !new_ctx->bt_data || !new_ctx->untracked_ptrs) {
        mm_track_release_ctx(new_ctx);
        LOG_E("mm_track_start alloc failed: callsite=%u data=%u\r\n",
              callsite_cap, data_cap);
        return -ENOMEM;
    }

    memset(new_ctx->entries, 0, sizeof(mm_track_entry_t) * callsite_cap);
    memset(new_ctx->bt_data, 0, sizeof(uint32_t) * data_cap);
    memset(new_ctx->untracked_ptrs, 0, sizeof(uintptr_t) * untracked_cap);
    untracked_ctx.untracked_ptrs = new_ctx->untracked_ptrs;
    untracked_ctx.untracked_cap = untracked_cap;
    (void)mm_walk_allocated_blocks(mm_track_capture_untracked_block, &untracked_ctx);

    new_ctx->untracked_cap = untracked_cap;
    new_ctx->untracked_count = untracked_ctx.count;
    new_ctx->runtime_callsite_cap = callsite_cap;
    new_ctx->runtime_data_cap = data_cap;
    new_ctx->runtime_depth = depth;
    new_ctx->runtime_skip = skip_frames;

    flags = mm_lock_save();
    if (g_mm_track_ctx != NULL) {
        mm_unlock_restore(flags);
        mm_track_release_ctx(new_ctx);
        LOG_W("mm_track is already started; reboot is required to restart\r\n");
        return -EALREADY;
    }
    g_mm_track_ctx = new_ctx;
    mm_unlock_restore(flags);

    if (untracked_ctx.overflow > 0U) {
        LOG_W("Untracked allocation snapshot truncated: overflow=%lu\r\n", (unsigned long)untracked_ctx.overflow);
    }

    return 0;
}

void mm_track_malloc(void *ptr, size_t block_size)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;
    uint32_t addrs[CONFIG_MM_TRACK_BACKTRACE_DEPTH];
    uint32_t depth;
    uint32_t hash;
    uintptr_t flags;
    size_t user_size;
    int entry_slot;
    uint32_t entry_idx = MM_LEAK_ENTRY_SLOT_INVALID;

    user_size = block_size - MM_TRACK_SLOT_BYTES;

    depth = mm_track_capture_backtrace(addrs, ctx->runtime_depth);
    hash = mm_track_hash_compute_bt(addrs, depth);

    flags = mm_lock_save();

    ctx = g_mm_track_ctx;
    (void)mm_track_maybe_auto_clear_locked();
    entry_slot = mm_track_find_or_create_entry_locked(hash, addrs, depth);
    if (entry_slot < 0) {
        (void)mm_track_maybe_auto_clear_locked();
        entry_slot = mm_track_find_or_create_entry_locked(hash, addrs, depth);
    }
    if (entry_slot >= 0) {
        mm_track_count_inc_u16(&ctx->entries[entry_slot].malloc_count);
        entry_idx = (uint32_t)entry_slot;
    }

    mm_unlock_restore(flags);
    mm_track_write_tail_index(ptr, user_size, entry_idx);
}

void mm_track_free(void *ptr, size_t block_size)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;
    uint32_t entry_idx;
    uintptr_t flags;
    bool removed;
    size_t user_size;
    mm_track_entry_t *entry;

    removed = mm_track_untracked_remove(ctx->untracked_ptrs, ctx->untracked_cap, (uintptr_t)ptr);
    if (removed) {
        if (ctx->untracked_count > 0U) {
            ctx->untracked_count--;
        }
        return;
    }

    user_size = block_size - MM_TRACK_SLOT_BYTES;
    entry_idx = mm_track_read_tail_index(ptr, user_size);
    mm_track_clear_tail_index(ptr, user_size);
    flags = mm_lock_save();

    ctx = g_mm_track_ctx;
    if (entry_idx != MM_LEAK_ENTRY_SLOT_INVALID && entry_idx < ctx->runtime_callsite_cap) {
        entry = &ctx->entries[entry_idx];
    } else {
        entry = NULL;
    }

    if (entry && entry->used) {
        mm_track_count_inc_u16(&entry->free_count);
    } else {
        LOG_W("Free without matching tracked entry: idx=0x%08lx\r\n",
              (unsigned long)entry_idx);
    }

    mm_unlock_restore(flags);
}

void mm_track_free_revoke(void *ptr, size_t block_size, bool is_untracked)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;

    if (is_untracked) {
        if (!mm_track_untracked_contains(ctx->untracked_ptrs, ctx->untracked_cap, (uintptr_t)ptr) &&
            mm_track_untracked_insert(ctx->untracked_ptrs, ctx->untracked_cap, (uintptr_t)ptr)) {
            ctx->untracked_count++;
        }
    } else {
        mm_track_malloc(ptr, block_size);
    }
}

void mm_track_dump(void)
{
    mm_track_ctx_t *ctx;
    uintptr_t flags = mm_lock_save();
    uint32_t total_used_blocks = 0;
    ctx = g_mm_track_ctx;
    if (!ctx || !ctx->entries || !ctx->bt_data) {
        printf("MM Leak Trace Report\r\n");
        printf("state=stopped (buffers not allocated)\r\n");
        mm_unlock_restore(flags);
        return;
    }

    printf("MM Leak Trace Report\r\n");
    printf("state=%s callsite_cap=%u/%u data_cap=%u/%u depth=%u skip=%u\r\n",
           "started",
           ctx->runtime_callsite_cap, CONFIG_MM_TRACK_CALLSITE_TABLE_SIZE,
           ctx->runtime_data_cap, CONFIG_MM_TRACK_DATA_SIZE,
           ctx->runtime_depth, ctx->runtime_skip);
    printf("hash         malloc    free    used\r\n");

    for (uint32_t i = 0; i < ctx->runtime_callsite_cap; i++) {
        mm_track_entry_t *entry = &ctx->entries[i];
        uint32_t used;

        if (!entry->used) {
            continue;
        }

        if (entry->malloc_count == 0U && entry->free_count == 0U) {
            continue;
        }

        used = (entry->malloc_count > entry->free_count) ?
                   (uint32_t)(entry->malloc_count - entry->free_count) :
                   0U;

        if (used > 0U) {
            total_used_blocks += used;
        }

        printf("0x%08lx %8lu %8lu %7lu\r\n",
               (unsigned long)entry->hash,
               (unsigned long)entry->malloc_count,
               (unsigned long)entry->free_count,
               (unsigned long)used);
    }

    printf("callsites=%lu bt_entries=%lu data_used=%lu/%u dropped=%lu\r\n",
           (unsigned long)ctx->entry_used,
           (unsigned long)ctx->entry_used,
           (unsigned long)ctx->bt_data_used,
           ctx->runtime_data_cap,
           (unsigned long)ctx->drop_count);
    printf("used blocks=%lu\r\n", (unsigned long)total_used_blocks);
    if (ctx->drop_count > 0U) {
        printf("hint: overflow happened, increase limits then retest: mm_track start <callsite_cap> <data_cap> <depth> <skip>\r\n");
    }

    mm_unlock_restore(flags);
}

uint32_t mm_track_clear(void)
{
    uintptr_t flags = mm_lock_save();
    uint32_t cleared;

    cleared = mm_track_clear_balanced_locked();

    mm_unlock_restore(flags);
    return cleared;
}

bool mm_track_is_started(void)
{
    return g_mm_track_ctx != NULL;
}

bool mm_track_is_untracked_ptr(void *ptr)
{
    mm_track_ctx_t *ctx = g_mm_track_ctx;

    return ctx ? mm_track_untracked_contains(ctx->untracked_ptrs, ctx->untracked_cap, (uintptr_t)ptr) : false;
}

#ifdef CONFIG_SHELL
static void mm_track_print_start_alloc_bytes(void)
{
    mm_track_ctx_t *ctx;
    uintptr_t flags = mm_lock_save();
    size_t ctx_bytes;
    size_t entry_bytes;
    size_t bt_data_bytes;
    size_t untracked_bytes;
    size_t total_bytes;

    ctx = g_mm_track_ctx;
    if (!ctx || !ctx->entries || !ctx->bt_data || !ctx->untracked_ptrs) {
        mm_unlock_restore(flags);
        return;
    }

    ctx_bytes = sizeof(*ctx);
    entry_bytes = (size_t)ctx->runtime_callsite_cap * sizeof(mm_track_entry_t);
    bt_data_bytes = (size_t)ctx->runtime_data_cap * sizeof(uint32_t);
    untracked_bytes = (size_t)ctx->untracked_cap * sizeof(uintptr_t);
    total_bytes = ctx_bytes + entry_bytes + bt_data_bytes + untracked_bytes;
    mm_unlock_restore(flags);

    printf("mm leak trace alloc bytes:\r\n");
    printf("  ctx: req_bytes=%lu\r\n", (unsigned long)ctx_bytes);
    printf("  entry: cap=%u item_size=%u req_bytes=%lu\r\n",
           ctx->runtime_callsite_cap,
           (unsigned int)sizeof(mm_track_entry_t),
           (unsigned long)entry_bytes);
    printf("  bt_data: cap=%u item_size=%u req_bytes=%lu\r\n",
           ctx->runtime_data_cap,
           (unsigned int)sizeof(uint32_t),
           (unsigned long)bt_data_bytes);
    printf("  untracked: cap=%lu item_size=%u req_bytes=%lu\r\n",
           (unsigned long)ctx->untracked_cap,
           (unsigned int)sizeof(uintptr_t),
           (unsigned long)untracked_bytes);
    printf("  total_req_bytes=%lu\r\n", (unsigned long)total_bytes);
}

static void mm_track_print_help(void)
{
    printf("Usage: mm_track [subcmd]\r\n");
    printf("  help\r\n");
    printf("  dump\r\n");
    printf("  clear\r\n");
    printf("  start [callsite_cap] [data_cap] [depth] [skip]\r\n");
    printf("  bt <hash>\r\n");
}

static int cmd_mm_track(int argc, char **argv)
{
    if (argc < 2) {
        int ret;

        if (mm_track_is_started()) {
            mm_track_dump();
            return 0;
        }

        ret = mm_track_start(CONFIG_MM_TRACK_CALLSITE_TABLE_SIZE,
                             CONFIG_MM_TRACK_DATA_SIZE,
                             CONFIG_MM_TRACK_BACKTRACE_DEPTH,
                             CONFIG_MM_TRACK_SKIP_FRAMES);
        if (ret == 0) {
            printf("mm leak trace started: callsite=%u data=%u depth=%u skip=%u\r\n",
                   CONFIG_MM_TRACK_CALLSITE_TABLE_SIZE,
                   CONFIG_MM_TRACK_DATA_SIZE,
                   CONFIG_MM_TRACK_BACKTRACE_DEPTH,
                   CONFIG_MM_TRACK_SKIP_FRAMES);
            mm_track_print_start_alloc_bytes();
            return 0;
        }

        if (ret == -EALREADY) {
            printf("mm track already started; reboot is required to restart\r\n");
            return -1;
        }
        printf("start failed, check range:\r\n");
        printf("  callsite: 1..%u data: 1..%u depth: 1..%u skip: 0..%u\r\n",
               CONFIG_MM_TRACK_CALLSITE_TABLE_SIZE,
               CONFIG_MM_TRACK_DATA_SIZE,
               CONFIG_MM_TRACK_BACKTRACE_DEPTH,
               CONFIG_MM_TRACK_SKIP_FRAMES);
        return -1;
    }

    if (strcmp(argv[1], "dump") == 0) {
        mm_track_dump();
        return 0;
    }

    if (strcmp(argv[1], "help") == 0) {
        mm_track_print_help();
        return 0;
    }

    if (strcmp(argv[1], "clear") == 0) {
        uint32_t cleared = mm_track_clear();
        printf("mm leak trace clear done, cleared balanced entries: %u\r\n", (unsigned int)cleared);
        return 0;
    }

    if (strcmp(argv[1], "start") == 0) {
        uint16_t callsite_cap = CONFIG_MM_TRACK_CALLSITE_TABLE_SIZE;
        uint16_t data_cap = CONFIG_MM_TRACK_DATA_SIZE;
        uint8_t depth = CONFIG_MM_TRACK_BACKTRACE_DEPTH;
        uint8_t skip = CONFIG_MM_TRACK_SKIP_FRAMES;
        int ret;

        if (argc >= 3) {
            callsite_cap = (uint16_t)strtoul(argv[2], NULL, 0);
        }
        if (argc >= 4) {
            data_cap = (uint16_t)strtoul(argv[3], NULL, 0);
        }
        if (argc >= 5) {
            depth = (uint8_t)strtoul(argv[4], NULL, 0);
        }
        if (argc >= 6) {
            skip = (uint8_t)strtoul(argv[5], NULL, 0);
        }

        ret = mm_track_start(callsite_cap, data_cap, depth, skip);
        if (ret == 0) {
            printf("mm leak trace started: callsite=%u data=%u depth=%u skip=%u\r\n",
                   callsite_cap, data_cap, depth, skip);
            mm_track_print_start_alloc_bytes();
            return 0;
        }
        if (ret == -EALREADY) {
            printf("mm track already started; reboot is required to restart\r\n");
            return -1;
        }
        printf("start failed, check range:\r\n");
        printf("  callsite: 1..%u data: 1..%u depth: 1..%u skip: 0..%u\r\n",
               CONFIG_MM_TRACK_CALLSITE_TABLE_SIZE,
               CONFIG_MM_TRACK_DATA_SIZE,
               CONFIG_MM_TRACK_BACKTRACE_DEPTH,
               CONFIG_MM_TRACK_SKIP_FRAMES);
        return -1;
    }

    if (strcmp(argv[1], "bt") == 0) {
        mm_track_ctx_t *ctx;
        uintptr_t flags;
        uint32_t hash;
        uint32_t matched = 0;

        if (argc < 3) {
            printf("Usage: mm_track bt <hash>\r\n");
            return -1;
        }

        hash = (uint32_t)strtoul(argv[2], NULL, 0);
        flags = mm_lock_save();
        ctx = g_mm_track_ctx;
        if (!ctx || !ctx->entries || !ctx->bt_data) {
            mm_unlock_restore(flags);
            printf("mm track is stopped\r\n");
            return -1;
        }
        for (uint32_t i = 0; i < ctx->runtime_callsite_cap; i++) {
            mm_track_entry_t *entry = &ctx->entries[i];

            if (!entry->used || entry->hash != hash) {
                continue;
            }

            mm_track_print_bt_line_by_idx((uint16_t)i);
            matched++;
        }
        mm_unlock_restore(flags);

        if (matched == 0U) {
            printf("No callsite found for hash=0x%08lx\r\n", (unsigned long)hash);
            return -1;
        }

        printf("matched bt entries: %u\r\n", (unsigned int)matched);
        return 0;
    }

    printf("unknown subcmd: %s\r\n", argv[1]);
    mm_track_print_help();
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mm_track, mm_track, mm leak trace commands);
#endif
