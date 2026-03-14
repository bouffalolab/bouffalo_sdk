/**
 * @file unwind_6byte.h
 * @brief DWARF Unwind Table Runtime Lookup (6-Byte Format v4.1 with function size)
 *
 * This module provides low-level unwind table parsing and frame unwinding
 * using the compact 6-byte entry format.
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
 *   IMPORTANT: Leaf functions (frame_size=0) are NOT in the table
 *   - During backtrace, if PC is not found, it's a leaf function
 *   - For leaf function: PC = [SP], SP unchanged, continue unwind
 */

#ifndef __UNWIND_6BYTE_H__
#define __UNWIND_6BYTE_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * Data Structures
 *============================================================================*/

/**
 * @brief Segment descriptor
 */
typedef struct {
    uint16_t segment_base;       /**< Segment base address */
    uint16_t start_entry_idx;    /**< Start entry index */
    uint16_t end_entry_idx;      /**< End entry index */
} segment_desc_t;

/**
 * @brief Unwind entry structure
 */
typedef struct {
    uint16_t offset_in_segment;  /**< Offset within segment */
    uint16_t frame_size_words;   /**< Frame size in words (frame_size / 4) */
    uint16_t func_size;          /**< Function size in bytes */
    int16_t  ra_offset;          /**< RA offset from SP (in words) */
} unwind_entry_t;

/*============================================================================
 * Core API
 *============================================================================*/

/**
 * @brief Initialize unwind table from binary data
 *
 * Parses the binary unwind table format including:
 * - Non-standard RA table
 * - Segment table
 * - Entries data pointer
 *
 * @param data  Pointer to binary unwind table data
 * @return true on success, false on failure
 */
bool unwind_table_init(const uint8_t *data);

/**
 * @brief Find unwind entry for a given PC address
 *
 * Since CFI table only contains function start addresses (optimized),
 * we need to find the function entry that contains the PC address.
 * This is done by finding the greatest entry offset <= PC offset.
 *
 * IMPORTANT: Leaf functions (frame_size=0) are NOT in the table
 * - Returns false if entry not found (caller should treat as leaf function)
 *
 * @param pc            Program counter address
 * @param segments      Segment descriptor array
 * @param num_segments  Number of segments
 * @param entry         [out] Unwind entry structure
 * @return true if entry found, false if leaf function (not in table)
 */
bool find_unwind_entry(uint32_t pc, const segment_desc_t *segments, int num_segments, unwind_entry_t *entry);

/**
 * @brief Unwind one stack frame
 *
 * Given current PC and SP, computes the previous frame's PC and SP.
 *
 * IMPORTANT: Leaf functions (frame_size=0) are NOT in the CFI table
 * - If find_unwind_entry returns false, PC is in a leaf function
 * - For leaf function: RA is at [SP], SP unchanged, continue unwind
 *
 * @param pc            Current program counter
 * @param sp            Current stack pointer
 * @param segments      Segment descriptor array
 * @param num_segments  Number of segments
 * @param new_pc        [out] Previous frame's program counter
 * @param new_sp        [out] Previous frame's stack pointer
 * @return true if unwind succeeded, false on error
 */
bool unwind_frame(uint32_t pc, uint32_t sp,
                  const segment_desc_t *segments, int num_segments,
                  uint32_t *new_pc, uint32_t *new_sp);

/**
 * @brief Perform backtrace
 *
 * Unwinds the stack starting from the given PC and SP addresses,
 * collecting return addresses into the provided array.
 *
 * @param pc              Current program counter
 * @param sp              Current stack pointer
 * @param addrs           [out] Array to store backtrace addresses
 * @param max_frames      Maximum number of frames to capture
 * @param cfi_table_base  Pointer to CFI table data (for initialization)
 * @return Number of frames captured
 */
int backtrace(uint32_t pc, uint32_t sp,
              uint32_t *addrs, int max_frames,
              const uint8_t *cfi_table_base);

/**
 * @brief Print backtrace results
 *
 * Formats and prints the backtrace addresses.
 *
 * @param addrs  Backtrace address array
 * @param count  Number of addresses in the array
 */
void print_backtrace(const uint32_t *addrs, int count);

#ifdef __cplusplus
}
#endif

#endif /* __UNWIND_6BYTE_H__ */
