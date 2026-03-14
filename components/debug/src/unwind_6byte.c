/*
 * DWARF Unwind Table Runtime Lookup (6-Byte Format v4.1 with function size)
 *
 * Binary format (version 4.1, no alignment required):
 *
 *   1. Non-standard RA table (at file beginning):
 *      uint16_t num_non_standard_ra_entries;
 *      struct {
 *          uint16_t entry_idx;    // Global entry index
 *          int16_t  ra_offset;     // Non-standard RA offset value
 *      } entries[];
 *      uint16_t terminator;  // 0xFFFF
 *
 *   2. Segment table:
 *      struct {
 *          uint16_t segment_base;
 *          uint16_t start_entry_idx;
 *          uint16_t end_entry_idx;
 *      } segments[];
 *      uint32_t terminator[3];  // 0xFFFFFFFF × 3
 *
 *   3. Entries (6 bytes each, no padding/alignment):
 *      struct {
 *          uint16_t offset_in_segment | flag;  // bits 15-1: offset, bit 0: flag
 *          uint16_t frame_size_words;          // frame_size / 4 (16 bits)
 *          uint16_t func_size;                 // 函数大小（字节）
 *      }
 *
 *   Entry encoding:
 *     - flag=0 (标RA):
 *         offset = offset_in_segment (bit 0 cleared)
 *         frame_size_words = frame_size / 4  (max 65535 = 262KB)
 *         ra_offset = frame_size_words - 1  (standard convention)
 *
 *     - flag=1 (非RA):
 *         offset = offset_in_segment | 1
 *         frame_size_words = frame_size / 4  (max 65535 = 262KB)
 *         ra_offset = binary_search(non_standard_ra_table, entry_idx)
 *
 *   IMPORTANT: Leaf functions (frame_size=0) are NOT in the table
 *   - During backtrace, if PC is not found, it's a leaf function
 *   - For leaf function: PC = [SP], SP unchanged, continue unwind
 *
 *   File layout:
 *     - Entries start immediately after segment table (no padding)
 *     - entries_offset = sizeof(non_standard_ra_table) + sizeof(segment_table)
 */

#ifndef DEBUG
#define DEBUG 0  /* Default: disable debug output */
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Unwind entry structure */
typedef struct {
    uint16_t offset_in_segment;
    uint16_t frame_size_words;
    uint16_t func_size;
    int16_t  ra_offset;
} unwind_entry_t;

/* Non-standard RA entry */
typedef struct {
    uint16_t entry_idx;
    int16_t  ra_offset;
} non_standard_ra_entry_t;

/* Segment descriptor */
typedef struct {
    uint16_t segment_base;
    uint16_t start_entry_idx;
    uint16_t end_entry_idx;
} segment_desc_t;

/* Global state (non-standard RA table and entries data) */
static non_standard_ra_entry_t *g_non_standard_ra_table = NULL;
static uint16_t g_num_non_standard_ra_entries = 0;
static const uint8_t *g_entries_data = NULL;
static const uint8_t *g_segment_table_start = NULL;  /* For parsing segments on demand */

/**
 * Initialize non-standard RA table from binary data
 */
static bool init_non_standard_ra_table(const uint8_t **ptr)
{
    g_num_non_standard_ra_entries = *(uint16_t*)*ptr;
    *ptr += 2;

    if (g_num_non_standard_ra_entries > 0) {
        g_non_standard_ra_table = (non_standard_ra_entry_t*)*ptr;
        *ptr += g_num_non_standard_ra_entries * 4;
    }

    /* Skip terminator */
    uint16_t term = *(uint16_t*)*ptr;
    if (term == 0xFFFF) {
        *ptr += 2;
    } else {
        fprintf(stderr, "Error: Invalid terminator 0x%04x\n", term);
        return false;
    }

    return true;
}

/**
 * Binary search in non-standard RA table
 */
static int16_t lookup_non_standard_ra(uint16_t entry_idx)
{
    int left = 0;
    int right = g_num_non_standard_ra_entries - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        uint16_t mid_idx = g_non_standard_ra_table[mid].entry_idx;

        if (mid_idx == entry_idx) {
            return g_non_standard_ra_table[mid].ra_offset;
        } else if (mid_idx < entry_idx) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;  /* Not found */
}

/**
 * Check for segment table terminator
 */
static inline bool is_segment_terminator(const uint8_t *ptr)
{
    return (*(uint32_t*)ptr == 0xFFFFFFFF &&
            *(uint32_t*)(ptr + 4) == 0xFFFFFFFF &&
            *(uint32_t*)(ptr + 8) == 0xFFFFFFFF);
}

/**
 * Parse segment table
 */
static bool parse_segments(const uint8_t **ptr, segment_desc_t *segments, int *num_segments)
{
    *num_segments = 0;

    while (*num_segments < 64) {
        /* Check for terminator */
        if (is_segment_terminator(*ptr)) {
            *ptr += 12;  /* Skip 3 × 4 bytes */
            break;
        }

        /* Parse segment descriptor */
        segments[*num_segments].segment_base = *(uint16_t*)*ptr;
        segments[*num_segments].start_entry_idx = *(uint16_t*)(*ptr + 2);
        segments[*num_segments].end_entry_idx = *(uint16_t*)(*ptr + 4);

        *ptr += 6;
        (*num_segments)++;
    }

    return *num_segments > 0;
}

/**
 * Initialize unwind table from binary data
 * Only parses non-standard RA table and entries data location.
 * Segments are parsed on each backtrace call to save RAM.
 */
bool unwind_table_init(const uint8_t *data)
{
    static bool initialized = false;
    static const uint8_t *cached_data = NULL;

    /* Already initialized with same data - skip */
    if (initialized && cached_data == data) {
        return true;
    }

    const uint8_t *ptr = data;

#if DEBUG
    printf("Initializing unwind table...\n");
#endif
    if (!init_non_standard_ra_table(&ptr)) return false;

    /* Save segment table start position for later parsing */
    g_segment_table_start = ptr;

    /* Find entries data start (skip segment table) */
    /* We need to parse segment table to find where entries start */
    /* But we don't store segments, just find the end marker */
    while (!is_segment_terminator(ptr)) {
        /* Skip segment descriptors until terminator */
        ptr += 6;
    }
    ptr += 12;  /* Skip terminator (3 × 4 bytes) */

    /* Entries start immediately after segment table (no padding) */
    size_t header_size = ptr - data;
    g_entries_data = data + header_size;

#if DEBUG
    printf("  Header size: %zu bytes\n", header_size);
    printf("  Entries start at offset: %zu (no padding)\n\n", header_size);
#endif

    initialized = true;
    cached_data = data;
    return true;
}

/**
 * Find unwind entry for a given PC address
 *
 * Since CFI table only contains function start addresses (optimized),
 * we need to find the function entry that contains the PC address.
 * This is done by finding the greatest entry offset <= PC offset.
 *
 * IMPORTANT: Leaf functions (frame_size=0) are NOT in the table
 * - Returns false if entry not found (caller should treat as leaf function)
 */
bool find_unwind_entry(uint32_t pc, const segment_desc_t *segments, int num_segments, unwind_entry_t *entry)
{
    if (g_entries_data == NULL) {
        fprintf(stderr, "Error: Unwind table not initialized\n");
        return false;
    }

    /* 1. Extract segment ID and offset from PC */
    uint16_t pc_segment_id = pc >> 16;
    uint16_t pc_offset = pc & 0xFFFF;

    /* 2. Find matching segment */
    const segment_desc_t *seg = NULL;
    for (int i = 0; i < num_segments; i++) {
        if (segments[i].segment_base == pc_segment_id) {
            seg = &segments[i];
            break;
        }
    }

    if (seg == NULL) {
        return false;  /* No matching segment */
    }

    /* 3. Binary search to find function entry containing PC */
    uint16_t left = seg->start_entry_idx;
    uint16_t right = seg->end_entry_idx;
    uint16_t best_match = 0xFFFF;  /* Invalid index */
    uint16_t best_offset = 0;

    while (left <= right) {
        uint16_t mid = left + (right - left) / 2;

        /* Read 6-byte entry at index mid */
        const uint8_t *entry_ptr = g_entries_data + mid * 6;
        uint16_t offset_field = *(uint16_t*)entry_ptr;
        uint16_t frame_size_words = *(uint16_t*)(entry_ptr + 2);
        uint16_t func_size = *(uint16_t*)(entry_ptr + 4);

        /* Extract offset (clear bit 0) */
        uint16_t mid_offset = offset_field & 0xFFFE;

        if (mid_offset == pc_offset) {
            /* Exact match - this is the function start */
            best_match = mid;
            best_offset = mid_offset;
            break;
        } else if (mid_offset < pc_offset) {
            /* This function starts before PC, could be the containing function */
            best_match = mid;
            best_offset = mid_offset;
            left = mid + 1;  /* Continue searching for closer match */
        } else {
            /* mid_offset > pc_offset, search left half */
            if (mid == 0) break;
            right = mid - 1;
        }
    }

    /* Check if we found a valid function entry */
    if (best_match == 0xFFFF) {
        /* Entry not found - this is a leaf function (not in table) */
        return false;
    }

    /* Read the best matching entry (6 bytes) */
    const uint8_t *entry_ptr = g_entries_data + best_match * 6;
    uint16_t offset_field = *(uint16_t*)entry_ptr;
    uint16_t frame_size_words = *(uint16_t*)(entry_ptr + 2);
    uint16_t func_size = *(uint16_t*)(entry_ptr + 4);

    entry->offset_in_segment = best_offset;
    entry->frame_size_words = frame_size_words;
    entry->func_size = func_size;

    /* Extract RA offset */
    if (offset_field & 1) {
        /* Non-standard RA: binary search in table */
        int16_t ra_offset = lookup_non_standard_ra(best_match);
        if (ra_offset < 0) {
            fprintf(stderr, "Error: Non-standard RA not found for entry %u\n", best_match);
            return false;
        }
        entry->ra_offset = ra_offset;
    } else {
        /* Standard RA: use standard convention */
        entry->ra_offset = frame_size_words - 1;
    }

    return true;
}

/**
 * Unwind one stack frame
 *
 * IMPORTANT: Leaf functions (frame_size=0) are NOT in the CFI table
 * - If find_unwind_entry returns false, PC is in a leaf function
 * - For leaf function: RA is at [SP], SP unchanged, continue unwind
 */
bool unwind_frame(uint32_t pc, uint32_t sp,
                  const segment_desc_t *segments, int num_segments,
                  uint32_t *new_pc, uint32_t *new_sp)
{
    unwind_entry_t entry;

    if (!find_unwind_entry(pc, segments, num_segments, &entry)) {
        /* Entry not found - leaf function (not in table) */
        /* For leaf function: RA is at [SP], SP unchanged */
#ifdef STACK_READ
        uint32_t ra = STACK_READ(sp);
#else
        uint32_t ra = *(uint32_t*)sp;
#endif
        *new_sp = sp;  /* SP unchanged */
        *new_pc = ra;
#if DEBUG
        printf("  [LEAF] PC=0x%08x -> RA=0x%08x (not in table, RA at SP)\n", pc, ra);
#endif
        return true;
    }

    /* Normal function with stack frame */
    uint32_t ra_addr = sp + (entry.ra_offset * 4);
    if (ra_addr < sp || ra_addr > sp + entry.frame_size_words * 4) {
        fprintf(stderr, "Invalid RA address 0x%08x\n", ra_addr);
        return false;
    }
#ifdef STACK_READ
    uint32_t ra = STACK_READ(ra_addr);
#else
    uint32_t ra = *(uint32_t*)ra_addr;
#endif
    *new_sp = sp + (entry.frame_size_words * 4);
    *new_pc = ra;
#if DEBUG
    printf("  [OK] PC=0x%08x -> RA=0x%08x (frame=%dw, ra_off=%d)\n",
           pc, ra, entry.frame_size_words, entry.ra_offset);
#endif
    return true;
}

/**
 * Perform backtrace
 */
int backtrace(uint32_t pc, uint32_t sp,
              uint32_t *addrs, int max_frames,
              const uint8_t *cfi_table_base)
{
    int count = 0;
    segment_desc_t segments[64];
    int num_segments = 0;
    const uint8_t *ptr;

    /* Initialize unwind table */
    if (cfi_table_base != NULL) {
        unwind_table_init(cfi_table_base);
    }

    /* Parse segments from CFI table (on stack, not stored) */
    /* Use saved segment table start position */
    if (g_segment_table_start == NULL) {
        return 0;  /* Not initialized */
    }
    ptr = g_segment_table_start;

    /* Now parse segments */
    if (!parse_segments(&ptr, segments, &num_segments)) {
        return 0;
    }

#if DEBUG
    printf("\n=== Backtrace ===\n");
    printf("Initial: PC=0x%08x, SP=0x%08x, Segments: %d\n\n", pc, sp, num_segments);
#endif

    while (count < max_frames && pc != 0 && pc != 0xFFFFFFFF && pc != 0xA5A5A5A5) {
        addrs[count++] = pc;

        uint32_t new_pc, new_sp;
        if (!unwind_frame(pc, sp, segments, num_segments, &new_pc, &new_sp)) {
            break;
        }

        pc = new_pc;
        sp = new_sp;
    }

#if DEBUG
    printf("\n=== Total: %d frames ===\n\n", count);
#endif
    return count;
}

/**
 * Print backtrace results
 */
void print_backtrace(const uint32_t *addrs, int count)
{
    printf("Backtrace (%d frames):\n", count);
    printf("  #     Address        Segment\n");
    printf("  ---   ------------   --------\n");

    for (int i = 0; i < count; i++) {
        uint32_t addr = addrs[i];
        uint16_t seg = addr >> 16;
        printf("  [%d]  0x%08x       0x%04x\n", i, addr, seg);
    }
    printf("\n");
}

/*
 * Example usage:
 *
 *   extern const uint8_t __dwarf_unwind_table_start__;
 *
 *   int main() {
 *       // Initialize unwind table
 *       if (!unwind_table_init(&__dwarf_unwind_table_start__)) {
 *           fprintf(stderr, "Failed to initialize unwind table\n");
 *           return 1;
 *       }
 *
 *       // Simulate stack
 *       uint32_t fake_stack[256];
 *       fake_stack[0] = 0xa0001234;  // RA for frame 0
 *       fake_stack[3] = 0xa0005678;  // RA for frame 1
 *       // ... more stack frames
 *
 *       // Perform backtrace
 *       uint32_t pc = 0xa000abcd;  // Current PC
 *       uint32_t sp = (uint32_t)&fake_stack[0];
 *       uint32_t addrs[32];
 *
 *       int count = backtrace(pc, sp, addrs, 32);
 *       print_backtrace(addrs, count);
 *
 *       return 0;
 *   }
 */
