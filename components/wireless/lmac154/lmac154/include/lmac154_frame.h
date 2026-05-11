#ifndef __LMAC154_FRAME_H__
#define __LMAC154_FRAME_H__

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************//**
 * @file lmac154_frame.h
 * @brief IEEE 802.15.4 Frame Parsing and Analysis Module
 ******************************************************************************/

/*============================================================================
 * Frame Control Field Definitions
 *============================================================================*/

/* Frame Type */
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK       (7)
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_BEACON     (0)
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_DATA       (1)
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_ACK        (2)
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_CMD        (3)
#define LMAC154_FRAME_CONTROL_FRAME_TYPE_MPP        (5)

/* Security and Pending */
#define LMAC154_FRAME_SECURITY_MASK                 (1 << 3)
#define LMAC154_FRAME_FRAME_PENDING_MASK            (1 << 4)
#define LMAC154_FRAME_ACK_REQUEST_MASK              (1 << 5)
#define LMAC154_FRAME_PANID_COMPRESSION             (1 << 6)
#define LMAC154_FRAME_SEQ_SUPPRESSION_MASK          (1 << 8)
#define LMAC154_FRAME_IE_MASK                       (1 << 9)

/* Addressing Fields */
#define LMAC154_FRAME_ADDR_DEST_NONE                (0 << 10)
#define LMAC154_FRAME_ADDR_DEST_SHORT               (2 << 10)
#define LMAC154_FRAME_ADDR_DEST_EXT                 (3 << 10)
#define LMAC154_FRAME_ADDR_DEST_MASK                (3 << 10)

#define LMAC154_FRAME_VERSION_MASK                  (3 << 12)
#define LMAC154_FRAME_VERSION_2003                  (0 << 12)
#define LMAC154_FRAME_VERSION_2006                  (1 << 12)
#define LMAC154_FRAME_VERSION_2015                  (2 << 12)

#define LMAC154_FRAME_ADDR_SRC_NONE                 (0 << 14)
#define LMAC154_FRAME_ADDR_SRC_SHORT                (2 << 14)
#define LMAC154_FRAME_ADDR_SRC_EXT                  (3 << 14)
#define LMAC154_FRAME_ADDR_SRC_MASK                 (3 << 14)

/* MPP (Multi-Purpose Protocol) Frame Specific */
#define LMAC154_FRAME_MPP_LONG_FRAME_BIT            (1 << 3)
#define LMAC154_FRAME_MPP_SECURITY_BIT              (1 << 9)
#define LMAC154_FRAME_MPP_ACK_REQ                   (1 << 14)

/* Auxiliary Security Header */
#define LMAC154_FRAME_AUX_KEY_ID_MODE_MASK          (3 << 3)
#define LMAC154_FRAME_AUX_KEY_ID_MODE_0             (0 << 3)
#define LMAC154_FRAME_AUX_KEY_ID_MODE_1             (1 << 3)
#define LMAC154_FRAME_AUX_KEY_ID_MODE_2             (2 << 3)
#define LMAC154_FRAME_AUX_KEY_ID_MODE_3             (3 << 3)

/*============================================================================
 * Frame Check Macros
 *============================================================================*/

/* Frame Type Check */
#define LMAC154_FRAME_IS_BEACON(x)                  (((x) & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_BEACON)
#define LMAC154_FRAME_IS_DATA(x)                    (((x) & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_DATA)
#define LMAC154_FRAME_IS_ACK(x)                     (((x) & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_ACK)
#define LMAC154_FRAME_IS_CMD(x)                     (((x) & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_CMD)
#define LMAC154_FRAME_IS_MPP(x)                     (((x) & LMAC154_FRAME_CONTROL_FRAME_TYPE_MASK) == LMAC154_FRAME_CONTROL_FRAME_TYPE_MPP)

/* Security Check */
#define LMAC154_FRAME_IS_NORMAL_SECURITY(x)         ((x & LMAC154_FRAME_SECURITY_MASK) == LMAC154_FRAME_SECURITY_MASK)
#define LMAC154_FRAME_IS_MPP_LONG_FRAME(x)          ((x & LMAC154_FRAME_MPP_LONG_FRAME_BIT) == LMAC154_FRAME_MPP_LONG_FRAME_BIT)
#define LMAC154_FRAME_IS_MPP_SECURITY(x)            (LMAC154_FRAME_IS_MPP_LONG_FRAME(x) && ((x) & LMAC154_FRAME_MPP_SECURITY_BIT) == LMAC154_FRAME_MPP_SECURITY_BIT)
#define LMAC154_FRAME_IS_SECURITY_ENABLED(x)        (LMAC154_FRAME_IS_MPP(x) ? LMAC154_FRAME_IS_MPP_SECURITY(x) : LMAC154_FRAME_IS_NORMAL_SECURITY(x))

/* ACK and Pending Check */
#define LMAC154_FRAME_IS_ACK_REQ(x)                 (((x) & LMAC154_FRAME_ACK_REQUEST_MASK) == LMAC154_FRAME_ACK_REQUEST_MASK)
#define LMAC154_FRAME_IS_FRAME_PENDED(x)            ((x) & LMAC154_FRAME_FRAME_PENDING_MASK)
#define LMAC154_FRAME_IS_PANID_COMPRESSED(x)        (((x) & LMAC154_FRAME_PANID_COMPRESSION) == LMAC154_FRAME_PANID_COMPRESSION)
#define LMAC154_FRAME_IS_SEQ_SUPPRESS(x)            (((x) & LMAC154_FRAME_SEQ_SUPPRESSION_MASK) == LMAC154_FRAME_SEQ_SUPPRESSION_MASK)

/* Enhanced ACK Check */
#define LMAC154_FRAME_IS_MPP_ACK_REQ(x)             (LMAC154_FRAME_IS_MPP(x) && LMAC154_FRAME_IS_MPP_LONG_FRAME(x) && ((x) & LMAC154_FRAME_MPP_ACK_REQ) == LMAC154_FRAME_MPP_ACK_REQ)
#define LMAC154_FRAME_IS_ENH_ACK_REQ(x)             (LMAC154_FRAME_IS_MPP_ACK_REQ(x) || (LMAC154_FRAME_IS_ACK_REQ(x) && LMAC154_FRAME_IS_FRAME_2015(x)))
#define LMAC154_FRAME_IS_IMM_ACK_REQ(x)             (LMAC154_FRAME_IS_ACK_REQ(x) && !LMAC154_FRAME_IS_FRAME_2015(x))

/* Frame Version Check */
#define LMAC154_FRAME_IS_FRAME_2015(x)              (((x) & LMAC154_FRAME_VERSION_MASK) == LMAC154_FRAME_VERSION_2015)
#define LMAC154_FRAME_IS_IE_EXIST(x)                (((x) & LMAC154_FRAME_IE_MASK) && LMAC154_FRAME_IS_FRAME_2015(x))

/* ACK Type Check */
#define LMAC154_FRAME_IS_IMM_ACK(x)                 (LMAC154_FRAME_IS_ACK(x) && !LMAC154_FRAME_IS_FRAME_2015(x))
#define LMAC154_FRAME_IS_ENH_ACK(x)                 (LMAC154_FRAME_IS_ACK(x) && LMAC154_FRAME_IS_FRAME_2015(x))

/* Address Type Extraction */
#define LMAC154_FRAME_GET_DST_ADDR_TYPE(x)          ((x) & LMAC154_FRAME_ADDR_DEST_MASK)
#define LMAC154_FRAME_GET_SRC_ADDR_TYPE(x)          ((x) & LMAC154_FRAME_ADDR_SRC_MASK)

typedef struct {
    uint8_t sec_lvl:3;
    uint8_t key_id_mode:2;
    uint8_t frame_counter_sup:1;
    uint8_t asn_in_nonce:1;
    uint8_t reserved:1;
    uint8_t frame_cnt[4];
    uint8_t key_id[8];
} __packed lmac154_aux_hdr_t;

typedef struct {
    uint16_t length:7;
    uint16_t element_id:8;
    uint16_t type:1;
} __packed lmac154_ie_hdr_t;

typedef union {
    struct {
        lmac154_ie_hdr_t hdr;
        uint16_t phase;
        uint16_t period;
    } __packed bf;
    uint8_t bytes[6];
} __packed lmac154_ie_csl_t;

#define LMAC154_LIFS                    40
#define LMAC154_SIFS                    12
#define LMAC154_AIFS                    LMAC154_SIFS
#define LMAC154_SIFS_MAX_FRAME_SIZE     18
#define LMAC154_TURNAROUND              12
#define LMAC154_PKT_MAX_LEN             127
#define LMAC154_PREAMBLE_LEN            8
#define LMAC154_SHR_LEN                 10
#define LMAC154_US_PER_SYMBOL           16
#define LMAC154_US_PER_SYMBOL_BITS      4

#define LMAC154_CSMA_CA_MIN_BE          3
#define LMAC154_CSMA_CA_MAX_BE          5
#define LMAC154_CSMA_CA_MAX_BACKOFF     4

/* Key ID Mode definitions */
#define LMAC154_KEY_ID_MODE_0                 0
#define LMAC154_KEY_ID_MODE_1                 1
#define LMAC154_KEY_ID_MODE_2                 2
#define LMAC154_KEY_ID_MODE_3                 3
#define LMAC154_KEY_ID_MODE_4                 4

/* Key ID lengths for different modes */
#define LMAC154_KEY_ID_LEN_MODE_0             0
#define LMAC154_KEY_ID_LEN_MODE_1             1
#define LMAC154_KEY_ID_LEN_MODE_2             5
#define LMAC154_KEY_ID_LEN_MODE_3             8
#define LMAC154_KEY_ID_LEN_MODE_4_MIN         0
#define LMAC154_KEY_ID_LEN_MODE_4_MAX         9

/* IE (Information Element) Definitions */
#define LMAC154_IE_TERmination2_ID              0x7f
#define LMAC154_IE_CSL_ID                       0x1a

/* Thread Vendor IE Definitions */
#define LMAC154_IE_VENDOR_ID                    0x00
#define LMAC154_IE_THREAD_VENDOR_ID             0x00
#define LMAC154_IE_THREAD_ENH_ACK_PROBING_IE_ID 0x00
#define LMAC154_IE_THREAD_HDR_SIZE              4

/**
 * @brief  32-bit optimized MAC Header parser.
 * @note   Assumes 'pr' (buffer start) is 32-bit aligned. 
 * Uses single-word access to minimize bus cycles and branch mispredictions.
 * * @param pr           Pointer to the start of the MAC frame (FCF).
 * @param a_dest_panid Output pointer to Destination PAN ID.
 * @param a_dest_sa    Output pointer to Destination Short Address.
 * @param a_dest_xa    Output pointer to Destination Extended Address.
 * @param a_src_sa     Output pointer to Source Short Address.
 * @param a_src_xa     Output pointer to Source Extended Address.
 */
static inline uint32_t lmac154_parse_mhr(uint8_t *pr, uint8_t **a_dest_panid, uint8_t **a_dest_sa,
                                 uint8_t **a_dest_xa, uint8_t **a_src_sa, uint8_t **a_src_xa)
{
    /* 1. Single 32-bit load to fetch FCF, Seq, and the start of Dest PAN ID.
     * This is significantly faster than multiple 8-bit loads on 32-bit architectures. */
    uint32_t head = *(const uint32_t *)pr;
    uint32_t fcf  = head & 0xFFFF;
    
    /* Initial offset: FCF(2) + Sequence Number(1) = 3 */
    uint32_t offset = 3;

    /* Extract Address Modes and PAN ID Compression bit via bit-shifting */
    uint32_t dst_mode  = (fcf >> 10) & 0x03; 
    uint32_t src_mode  = (fcf >> 14) & 0x03; 
    uint32_t pan_comp  = (fcf >> 6)  & 0x01;

    /* 2. Parse Destination Address Field (PAN ID + Address) */
    if (dst_mode) {
        /* Destination PAN ID always follows Sequence Number at offset 3 */
        *a_dest_panid = pr + 3;
        
        /* Map Mode to length: Mode 2 (Short) -> 2 bytes, Mode 3 (Ext) -> 8 bytes.
         * Using a ternary here allows compilers to use CMOV (conditional move) 
         * instructions to avoid pipeline stalls. */
        uint32_t dst_addr_len = (dst_mode == 3) ? 8 : 2;
        
        /* Assign address pointers based on mode. Dest Addr starts at offset 5. */
        if (dst_mode == 2) {
            *a_dest_sa = pr + 5;
        } else {
            *a_dest_xa = pr + 5;
        }

        /* Update offset: 3 (FCF+Seq) + 2 (PAN) + AddrLength */
        offset = 5 + dst_addr_len; 
    }

    /* 3. Source PAN ID Compression Logic (Standard 2003/2006/2015 common subset).
     * If compression bit is 0, a 2-byte Source PAN ID exists before the Source Addr. */
    if (!pan_comp) {
        offset += 2;
    }

    /* 4. Parse Source Address Field */
    if (src_mode == 2) {
        /* Source Short Address (2 bytes) */
        *a_src_sa = pr + offset;
    } else if (src_mode == 3) {
        /* Source Extended Address (8 bytes) */
        *a_src_xa = pr + offset;
    }

    return offset;
}

/**
 * lmac154_parse_mhr_2015 - Strict MHR parser based on 802.15.4-2015 Table 7-2
 * @pr: Pointer to frame start (MUST be 32-bit aligned)
 * ... (output pointers)
 *
 * Returns the total length of the MHR in bytes.
 */
static inline uint32_t lmac154_parse_mhr_2015(uint8_t *pr,
                       uint8_t **a_dest_panid, uint8_t **a_dest_sa,
                       uint8_t **a_dest_xa, uint8_t **a_src_panid,
                       uint8_t **a_src_sa, uint8_t **a_src_xa)
{
    uint32_t head = *(const uint32_t *)pr;
    uint16_t fcf = (uint16_t)(head & 0xffff);
    uint32_t dst_mode = (fcf >> 10) & 0x03;
    uint32_t src_mode = (fcf >> 14) & 0x03;
    uint32_t pan_comp = (fcf >> 6) & 0x01;
    uint32_t offset;

    /* 1. Sequence Number Suppression (Version 2 feature) */
    offset = (fcf & (1 << 8)) ? 2 : 3;

    bool has_dst_pan = false;
    bool has_src_pan = false;

    /* 2. Strict Table 7-2 Logic Implementation */
    if (dst_mode == 0 && src_mode == 0) {
        has_dst_pan = (pan_comp == 1);
    } else if (dst_mode > 0 && src_mode == 0) {
        has_dst_pan = (pan_comp == 0);
    } else if (dst_mode == 0 && src_mode > 0) {
        has_src_pan = (pan_comp == 0);
    } else if (dst_mode == 3 && src_mode == 3) {
        /* Extended <-> Extended: Row 7 & 8 */
        has_dst_pan = (pan_comp == 0);
        has_src_pan = false;
    } else {
        /* Mixed or Short <-> Short: Row 9-14 */
        if (pan_comp == 0) {
            has_dst_pan = true;
            has_src_pan = true;
        } else {
            has_dst_pan = true;
            has_src_pan = false;
        }
    }

    /* 3. Extract Destination Fields */
    if (has_dst_pan) {
        *a_dest_panid = pr + offset;
        offset += 2;
    }

    if (dst_mode == 2) {
        *a_dest_sa = pr + offset;
        offset += 2;
    } else if (dst_mode == 3) {
        *a_dest_xa = pr + offset;
        offset += 8;
    }

    /* 4. Extract Source Fields */
    if (has_src_pan) {
        *a_src_panid = pr + offset;
        offset += 2;
    }

    if (src_mode == 2) {
        *a_src_sa = pr + offset;
        offset += 2;
    } else if (src_mode == 3) {
        *a_src_xa = pr + offset;
        offset += 8;
    }

    return offset;
}

/**
 * lmac154_get_mhr_len - MHR length for 802.15.4-2003/2006
 * @pkt: Pointer to frame start
 */
static inline uint32_t lmac154_get_mhr_len(const uint8_t *pkt)
{
    uint16_t fcf = pkt[0] | (pkt[1] << 8);
    uint32_t dst_mode = (fcf >> 10) & 0x03;
    uint32_t src_mode = (fcf >> 14) & 0x03;
    uint32_t pan_comp = (fcf >> 6) & 0x01;
    uint32_t pos = 3; /* FCF(2) + Seq(1), Legacy always has Seq */

    /* Destination Field */
    if (dst_mode) {
        pos += 2; /* Destination PAN ID always present if DstMode > 0 */
        pos += (dst_mode == 3 ? 8 : 2);
    }

    /* Source Field */
    if (src_mode) {
        /* In Legacy: Source PAN is omitted ONLY IF:
         * Both addresses are present AND PanComp is set to 1.
         */
        if (!dst_mode || !pan_comp) {
            pos += 2; /* Source PAN ID present */
        }
        pos += (src_mode == 3 ? 8 : 2);
    }

    return pos;
}

/**
 * lmac154_get_mhr_len_2015 - MHR length for 802.15.4-2015 (Strict Table 7-2)
 * @pkt: Pointer to frame start
 */
static inline uint32_t lmac154_get_mhr_len_2015(const uint8_t *pkt)
{
    uint16_t fcf = pkt[0] | (pkt[1] << 8);
    uint32_t dst_mode = (fcf >> 10) & 0x03;
    uint32_t src_mode = (fcf >> 14) & 0x03;
    uint32_t pan_comp = (fcf >> 6) & 0x01;
    
    /* Sequence Number Suppression: Bit 8 */
    uint32_t pos = (fcf & (1 << 8)) ? 2 : 3;

    bool has_dst_pan = false;
    bool has_src_pan = false;

    /* Strict Mapping of Table 7-2 */
    if (dst_mode == 0 && src_mode == 0) {
        has_dst_pan = (pan_comp == 1); /* Row 1 & 2 */
    } else if (dst_mode > 0 && src_mode == 0) {
        has_dst_pan = (pan_comp == 0); /* Row 3 & 4 */
    } else if (dst_mode == 0 && src_mode > 0) {
        has_src_pan = (pan_comp == 0); /* Row 5 & 6 */
    } else if (dst_mode == 3 && src_mode == 3) {
        has_dst_pan = (pan_comp == 0); /* Row 7 & 8: Extended-to-Extended */
    } else {
        /* Mixed or Short-to-Short: Rows 9-14 */
        if (pan_comp == 0) {
            has_dst_pan = true;  /* Row 9, 10, 11 */
            has_src_pan = true;
        } else {
            has_dst_pan = true;  /* Row 12, 13, 14 */
            has_src_pan = false;
        }
    }

    if (has_dst_pan) pos += 2;
    if (dst_mode > 0) pos += (dst_mode == 3 ? 8 : 2);
    if (has_src_pan) pos += 2;
    if (src_mode > 0) pos += (src_mode == 3 ? 8 : 2);

    return pos;
}

/**
 * @brief  Calculates the Auxiliary Security Header length.
 * @param  pkt: Pointer to the start of the frame.
 * @param  mhr_len: The offset where Aux Header starts (output of lmac154_get_mhr_len).
 * @return Length of the Security Header (5, 6, 10, or 14 bytes).
 */
static inline uint32_t lmac154_get_aux_sec_len(uint8_t *pkt, uint32_t mhr_len)
{
    uint16_t fcf = pkt[0] | (pkt[1] << 8);

    // If Security Enabled bit (Bit 3) is not set, length is 0
    if (!(fcf & 0x0008)) {
        return 0;
    }

    // Read Security Control byte at the calculated offset
    uint8_t sec_ctrl = pkt[mhr_len];
    uint32_t key_id_mode = (sec_ctrl >> 3) & 0x03;
    
    // Fast lookup using a packed 32-bit constant (Register-based LUT)
    // Modes: 0->0, 1->1, 2->5, 3->9 bytes extra for KeyID
    uint32_t key_table = 0x09050100;
    uint32_t extra = (key_table >> (key_id_mode << 3)) & 0xFF;
    
    // Base Security Header is 5 bytes: Security Control(1) + Frame Counter(4)
    return 5 + extra;
}

/**
 * lmac154_parse_aux_hdr - Safely map and return the next position
 * @pkt: Pointer to frame start (32-bit aligned)
 * @mhr_len: Offset to security header
 * @p_aux_hdr: Output pointer
 *
 * Return: Next position pointer.
 */
static inline uint32_t lmac154_parse_aux_hdr(uint8_t *pkt, uint32_t mhr_len,
                        lmac154_aux_hdr_t **p_aux_hdr)
{
    uint32_t len;
    uint8_t *pos = pkt + mhr_len;

    len = lmac154_get_aux_sec_len(pkt, mhr_len);
    if (len == 0) {
        if (p_aux_hdr)
            *p_aux_hdr = NULL;
        return 0;
    }

    if (p_aux_hdr)
        *p_aux_hdr = (lmac154_aux_hdr_t *)pos;

    return len;
}

/**
 * lmac154_traverse_ie - IE section traversal
 * @ie_start: Pointer to the start of IE section
 * @ie_len: Maximum length to search (buffer limit)
 * @target_id: Target IE ID to locate; @found_ie is set on first match
 * @found_ie: Output pointer to the matched IE header (NULL if not found)
 *
 * Return: Total bytes from ie_start to the end of the Termination IE.
 * Always traverses the full IE section regardless of target match.
 * Returns 0 if a structural error (truncation) is detected.
 */
static inline uint32_t lmac154_traverse_ie(uint8_t *ie_start, uint32_t ie_len,
                       uint8_t target_id, lmac154_ie_hdr_t **found_ie)
{
    uint8_t *cur = ie_start;
    uint8_t *limit = ie_start + ie_len;

    if (found_ie)
        *found_ie = NULL;

    /* Pre-check: Minimal Header IE is 2 bytes */
    while (cur + 2 <= limit) {
        /* RISC-V Safe: Byte-by-byte 16-bit header fetch
         * Bits: [0:6] Length, [7:14] ID, [15] Type
         */
        uint32_t hdr = (uint32_t)(cur[0] | (cur[1] << 8));
        uint32_t len = hdr & 0x7F;
        uint32_t id  = (hdr >> 7) & 0xFF;
        uint32_t total_sz = len + 2;

        /* 1. Fast Termination Check: 0x7E/0x7F both match (id & 0xFE) == 0x7E */
        if ((id & 0xFE) == 0x7E) {
            cur += 2;
            break;
        }

        /* 2. Boundary Validation */
        if (cur + total_sz > limit)
            return 0;

        /* 3. Target Matching - record first occurrence, continue traversal */
        if (id == target_id) {
            if (found_ie && !*found_ie)
                *found_ie = (lmac154_ie_hdr_t *)cur;
        }

        /* 4. Advance pointer */
        cur += total_sz;
    }

    /* Return total length of all traversed IEs (including termination) */
    return (uint32_t)(cur - ie_start);
}

/**
 * @brief  Generate CSL IE and return the computed phase value.
 * @note   CslPhase = ceil((sample_time - tx_sfd_sym) % period / 10),
 *         in 10-symbol units per IEEE 802.15.4-2015.
 * @param  csl_period     CSL period in 10-symbol units.
 * @param  csl_sample_time  CSL sample time in symbol count.
 * @param  csl_ie         Output CSL IE buffer.
 * @param  tx_sfd_sym     Estimated TX SFD time in symbol count.
 * @return CSL phase value written to the IE.
 */
static inline uint16_t lmac154_gen_csl_ie(uint16_t csl_period, uint32_t csl_sample_time,
                                           lmac154_ie_csl_t *csl_ie, uint32_t tx_sfd_sym)
{
    uint32_t period_sym = (uint32_t)csl_period * 10;
    uint32_t delta_sym  = (csl_sample_time - tx_sfd_sym) % period_sym;
    uint16_t csl_phase  = (uint16_t)((delta_sym + 9) / 10);

    csl_ie->bf.hdr.length     = 4;
    csl_ie->bf.hdr.element_id = LMAC154_IE_CSL_ID;
    csl_ie->bf.hdr.type       = 0;
    csl_ie->bf.phase          = csl_phase;
    csl_ie->bf.period         = csl_period;

    return csl_phase;
}

#endif /* __LMAC154_FRAME_H__ */
