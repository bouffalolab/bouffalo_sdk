/**
 * @file iperf_common.c
 * @brief Common Classic iPerf2 wire-format, statistics, and reporting helpers.
 */

#include <string.h>

#include <bflb_irq.h>
#include <bflb_mtimer.h>
#include <lwip/ip4_addr.h>

#define DBG_TAG "IPERF"
#include "log.h"

#include "iperf_internal.h"

/** @brief Permanent zero-filled payload referenced by Raw backend sends. */
const uint8_t g_iperf_raw_payload[BFLB_IPERF_RAW_TCP_BUFFER_LEN];

/**
 * @brief Read the monotonic hardware timer.
 * @return Current monotonic timestamp in microseconds.
 */
uint64_t iperf_now_us(void)
{
    return bflb_mtimer_get_time_us();
}

/* Statistics synchronization: never acquire a mutex, format, or log with IRQs
 * masked. Raw callbacks run in tcpip task context with the core lock held;
 * workers/public readers use the same short IRQ sections, not that lock. */
/**
 * @brief Copy statistics using the same short IRQ guard as all writers.
 * @param[in] iperf Live instance to inspect.
 * @param[out] stats Non-NULL destination for the complete statistics copy.
 * @note Single-CPU protection only; no SMP synchronization. No locking,
 * formatting or printing occurs while interrupts are masked.
 */
void iperf_stats_snapshot(const bflb_iperf_t *iperf, iperf_stats_t *stats)
{
    uintptr_t irq_flags = bflb_irq_save();

    *stats = iperf->stats;
    bflb_irq_restore(irq_flags);
}

/**
 * @brief Encode an unsigned 32-bit integer in network byte order.
 * @param[out] buffer Destination containing at least four writable bytes.
 * @param[in] value Host-order value to encode.
 */
void iperf_put_u32(uint8_t *buffer, uint32_t value)
{
    buffer[0] = (uint8_t)(value >> 24);
    buffer[1] = (uint8_t)(value >> 16);
    buffer[2] = (uint8_t)(value >> 8);
    buffer[3] = (uint8_t)value;
}

/**
 * @brief Decode an unsigned 32-bit integer in network byte order.
 * @param[in] buffer Source containing at least four readable bytes.
 * @return Decoded host-order value.
 */
uint32_t iperf_get_u32(const uint8_t *buffer)
{
    return ((uint32_t)buffer[0] << 24) |
           ((uint32_t)buffer[1] << 16) |
           ((uint32_t)buffer[2] << 8) |
           (uint32_t)buffer[3];
}

/**
 * @brief Encode the modern Classic iPerf2 UDP sequence and timestamp prefix.
 * @param[out] buffer Destination containing at least
 * BFLB_IPERF_UDP_HEADER_SIZE writable bytes.
 * @param[in] id Signed packet identifier; a negative value denotes FIN.
 * @param[in] now_us Packet timestamp in microseconds.
 */
void iperf_write_udp_header(uint8_t *buffer, int64_t id, uint64_t now_us)
{
    uint64_t wire_id = (uint64_t)id;

    iperf_put_u32(buffer, (uint32_t)wire_id);
    iperf_put_u32(buffer + 4, (uint32_t)(now_us / 1000000ULL));
    iperf_put_u32(buffer + 8, (uint32_t)(now_us % 1000000ULL));
    iperf_put_u32(buffer + 12, (uint32_t)(wire_id >> 32));
}

/**
 * @brief Decode the sender timestamp common to both Classic iPerf2 layouts.
 * @param[in] buffer Source containing at least 12 readable bytes.
 * @return Sender timestamp in microseconds.
 */
uint64_t iperf_read_udp_timestamp(const uint8_t *buffer)
{
    return (uint64_t)iperf_get_u32(buffer + 4) * 1000000ULL +
           iperf_get_u32(buffer + 8);
}

/**
 * @brief Encode a normal-mode UDP client setup header with packet ID one.
 * @param[out] buffer Destination containing at least
 * BFLB_IPERF_UDP_CLIENT_HEADER_SIZE writable bytes.
 * @param[in] iperf Client instance supplying normalized test settings.
 * @param[in] now_us Packet timestamp in microseconds.
 * @note Byte amount is saturated to INT32_MAX; duration is encoded as a
 * negative number of centiseconds, as required by Classic iPerf2.
 */
void iperf_write_udp_client_header(uint8_t *buffer,
                                   const bflb_iperf_t *iperf,
                                   uint64_t now_us)
{
    uint32_t flags = BFLB_IPERF_HEADER_SEQNO64B |
                     BFLB_IPERF_HEADER_EXTEND |
                     BFLB_IPERF_HEADER_LEN_BIT |
                     ((BFLB_IPERF_UDP_CLIENT_HEADER_SIZE << 1U) &
                      BFLB_IPERF_HEADER_LEN_MASK);
    uint32_t rate_bps = iperf->config.bandwidth_bps ? iperf->config.bandwidth_bps : BFLB_IPERF_DEFAULT_UDP_RATE_BPS;
    uint32_t amount;
    uint64_t duration_cs;

    /* Upstream normal mode uses packet ID one for its first settings datagram. */
    memset(buffer, 0, BFLB_IPERF_UDP_CLIENT_HEADER_SIZE);
    iperf_write_udp_header(buffer, 1, now_us);
    iperf_put_u32(buffer + 16, flags);
    iperf_put_u32(buffer + 20, 1U);
    iperf_put_u32(buffer + 24, iperf->config.port);
    iperf_put_u32(buffer + 28, iperf->config.buffer_len);
    if (iperf->config.amount_bytes != 0U) {
        amount = (uint32_t)((iperf->config.amount_bytes > INT32_MAX) ? INT32_MAX : iperf->config.amount_bytes);
    } else {
        duration_cs = (uint64_t)iperf->config.duration_s * 100ULL;
        if (duration_cs > (uint64_t)INT32_MAX) {
            duration_cs = (uint64_t)INT32_MAX;
        }
        amount = (uint32_t)(-(int32_t)duration_cs);
    }
    iperf_put_u32(buffer + 36, amount);
    iperf_put_u32(buffer + 64, rate_bps);
}

/**
 * @brief Recognize one of three supported normal-mode UDP setup layouts.
 * @param[in] buffer Datagram prefix with length readable bytes.
 * @param[in] length Available prefix size in bytes.
 * @param[out] reason Optional rejection string output; NULL on success.
 * @return SEQ32, SEQ64 or SEQ64_EXT; INVALID for rejected prefixes.
 * @note SEQ32 requires 36 bytes and a nonnegative signed 32-bit ID. SEQ64
 * layouts require a nonnegative signed 64-bit ID: SEQ64 accepts 20 bytes
 * without base-settings validation; SEQ64_EXT needs 80 bytes. SEQ32 retains
 * flags=0 and threads=1 as format markers; other base fields are not checked.
 * All layouts require usec < 1000000. VERSION1 setup flags are rejected.
 * A SEQ64 claim never falls back to SEQ32. Later IDs can establish
 * a session after initial loss; negative FIN IDs cannot. Receivers start timing
 * on the first accepted datagram but retain the format's loss-accounting base.
 * Delayed old data with a valid prefix may therefore claim a new session.
 * This recognizer does not validate every protocol field or extension.
 */
iperf_udp_setup_t iperf_udp_client_setup_type(const uint8_t *buffer,
                                              uint16_t length, const char **reason)
{
    uint32_t packet_id;
    uint32_t offset_16; /* SEQ64 flags, or SEQ32 numThreads. */
    const char *reject_reason;

    if (reason != NULL) {
        *reason = NULL;
    }

    /* The SEQ64 flag at offset 16 is readable only with a full 20-byte prefix. */
    if (length < BFLB_IPERF_UDP_HEADER_SIZE + sizeof(uint32_t)) {
        reject_reason = "prefix shorter than 20 bytes";
        goto rejected;
    }
    if (iperf_get_u32(buffer + 8) >= 1000000U) {
        reject_reason = "timestamp usec >= 1000000";
        goto rejected;
    }
    packet_id = iperf_get_u32(buffer);

    /* A SEQ64 claim must never fall back to the overlapping SEQ32 layout. */
    offset_16 = iperf_get_u32(buffer + BFLB_IPERF_UDP_HEADER_SIZE);
    if ((offset_16 & BFLB_IPERF_HEADER_SEQNO64B) != 0U) {
        /* Only the high word determines the sign of a 64-bit ID. */
        if ((iperf_get_u32(buffer + 12) & 0x80000000U) != 0U) {
            reject_reason = "negative SEQ64 ID (FIN cannot set up a session)";
            goto rejected;
        }
        if ((offset_16 & BFLB_IPERF_HEADER_VERSION1) != 0U) {
            reject_reason = "SEQ64 VERSION1 mode unsupported";
            goto rejected;
        }
        if ((offset_16 & BFLB_IPERF_HEADER_EXTEND) == 0U) {
            /* 2.0.13 normal mode leaves ASCII payload after flags, not base
             * settings. Preserve the original 20-byte minimum. */
            return IPERF_UDP_SETUP_SEQ64;
        }
        if (length < BFLB_IPERF_UDP_CLIENT_HEADER_SIZE) {
            reject_reason = "SEQ64 EXT prefix shorter than 80 bytes";
            goto rejected;
        }
        return IPERF_UDP_SETUP_SEQ64_EXT;
    }

    /* SEQ32 normal: flags@12=0, threads@16=1 are format markers.
     * Unconsumed port/length/window or bandwidth/amount fields are ignored.
     * In particular, missing SEQ64 flags@16=0
     * cannot masquerade as numThreads=1. Any nonnegative signed 32-bit ID
     * permits setup after initial data loss.
     * Compatibility (-C), dual and tradeoff modes are deliberately excluded. */
    if (length < BFLB_IPERF_UDP_LEGACY_HEADER_SIZE + BFLB_IPERF_UDP_CLIENT_V1_SIZE) {
        reject_reason = "SEQ32 prefix shorter than 36 bytes";
        goto rejected;
    }
    if ((packet_id & 0x80000000U) != 0U) {
        reject_reason = "negative SEQ32 ID (FIN cannot set up a session)";
        goto rejected;
    }
    if ((iperf_get_u32(buffer + 12) & BFLB_IPERF_HEADER_VERSION1) != 0U) {
        reject_reason = "SEQ32 VERSION1 mode unsupported";
        goto rejected;
    }
    if (iperf_get_u32(buffer + 12) != 0U || offset_16 != 1U) {
        reject_reason = "SEQ32 requires normal flags=0 and threads=1";
        goto rejected;
    }
    return IPERF_UDP_SETUP_SEQ32;

rejected:
    if (reason != NULL) {
        *reason = reject_reason;
    }
    return IPERF_UDP_SETUP_INVALID;
}

/**
 * @brief Get the sequence/timestamp prefix length for a selected layout.
 * @param[in] format Receiver layout.
 * @return 12 for SEQ32, 16 for either SEQ64 layout, or zero if invalid.
 */
uint16_t iperf_udp_header_size(iperf_udp_setup_t format)
{
    switch (format) {
        case IPERF_UDP_SETUP_SEQ32:
            return BFLB_IPERF_UDP_LEGACY_HEADER_SIZE;
        case IPERF_UDP_SETUP_SEQ64:
        case IPERF_UDP_SETUP_SEQ64_EXT:
            return BFLB_IPERF_UDP_HEADER_SIZE;
        default:
            return 0U;
    }
}

/**
 * @brief Get the server AckFIN TX length including compatibility padding.
 * @param[in] format Receiver layout.
 * @return 112 for SEQ32, 128 for either SEQ64 layout, or zero if invalid.
 */
uint16_t iperf_udp_server_report_size(iperf_udp_setup_t format)
{
    switch (format) {
        case IPERF_UDP_SETUP_SEQ32:
            return BFLB_IPERF_UDP_LEGACY_ACK_TX_SIZE;
        case IPERF_UDP_SETUP_SEQ64:
        case IPERF_UDP_SETUP_SEQ64_EXT:
            return BFLB_IPERF_UDP_ACK_TX_SIZE;
        default:
            return 0U;
    }
}

/**
 * @brief Decode a signed sequence ID after checking the selected prefix fits.
 * @param[in] format Previously selected session layout.
 * @param[in] buffer Prefix with length readable bytes.
 * @param[in] length Available prefix size in bytes.
 * @param[out] id Non-NULL output; unchanged on failure.
 * @return true on decoding, false for an invalid format or short prefix.
 * @note Negative IDs denote FIN; this does not validate settings or timestamps.
 */
bool iperf_udp_decode_id(iperf_udp_setup_t format, const uint8_t *buffer,
                         uint16_t length, int64_t *id)
{
    uint16_t header_size = iperf_udp_header_size(format);

    if (header_size == 0U || length < header_size) {
        return false;
    }
    if (format == IPERF_UDP_SETUP_SEQ32) {
        uint32_t wire_id = iperf_get_u32(buffer);

        *id = (wire_id & 0x80000000U) != 0U ?
                  (int64_t)wire_id - 0x100000000LL :
                  (int64_t)wire_id;
    } else {
        uint64_t wire_id = (uint64_t)iperf_get_u32(buffer) |
                           ((uint64_t)iperf_get_u32(buffer + 12) << 32);

        *id = (int64_t)wire_id;
    }
    return true;
}

/**
 * @brief Locate a length-checked version-1 base server report.
 * @param[in] buffer Received prefix with length readable bytes.
 * @param[in] length Available prefix size in bytes.
 * @return 16 for a modern report, 12 for legacy, or zero if not recognized.
 * @note Requires 56/52 bytes respectively and prefers modern. UINT32_MAX at
 * legacy flags is rejected to avoid treating a negative 64-bit ID's high word
 * as flags in a truncated modern report. Other report fields are not validated.
 */
static uint16_t iperf_udp_client_report_offset(const uint8_t *buffer, uint16_t length)
{
    if (length >= BFLB_IPERF_UDP_ACK_SIZE &&
        (iperf_get_u32(buffer + BFLB_IPERF_UDP_HEADER_SIZE) &
         BFLB_IPERF_HEADER_VERSION1) != 0U) {
        return BFLB_IPERF_UDP_HEADER_SIZE;
    }
    if (length >= BFLB_IPERF_UDP_LEGACY_ACK_SIZE &&
        iperf_get_u32(buffer + BFLB_IPERF_UDP_LEGACY_HEADER_SIZE) != UINT32_MAX &&
        (iperf_get_u32(buffer + BFLB_IPERF_UDP_LEGACY_HEADER_SIZE) &
         BFLB_IPERF_HEADER_VERSION1) != 0U) {
        return BFLB_IPERF_UDP_LEGACY_HEADER_SIZE;
    }
    return 0U;
}

/**
 * @brief Check report length and the supported version-1 flag placement.
 * @param[in] buffer Received report prefix.
 * @param[in] length Number of readable bytes.
 * @return true if a base report is recognized, false otherwise.
 * @note Does not verify peer identity, FIN correspondence or counter accuracy.
 */
bool iperf_udp_client_report_valid(const uint8_t *buffer, uint16_t length)
{
    return iperf_udp_client_report_offset(buffer, length) != 0U;
}

/**
 * @brief Initialize receiver-side UDP sequence and jitter tracking.
 * @param[out] tracker Tracker to reset.
 * @param[in] format Validated session layout.
 * @post Counters are zero and the expected ID uses the format's sequence base.
 * @pre format is a supported layout; tracker is worker-owned.
 * @note first_id is zero for SEQ32 and one for both SEQ64 layouts.
 */
void iperf_udp_server_rx_init(iperf_udp_rx_t *tracker, iperf_udp_setup_t format)
{
    memset(tracker, 0, sizeof(*tracker));
    tracker->format = format;
    tracker->first_id = format == IPERF_UDP_SETUP_SEQ32 ? 0U : 1U;
    tracker->next_id = tracker->first_id;
    tracker->last_transit_us = INT64_MIN;
}

/**
 * @brief Account one received UDP data datagram.
 * @param[in,out] iperf Instance whose transfer statistics are updated.
 * @param[in,out] tracker Receiver sequence and jitter tracker.
 * @param[in] id Nonnegative UDP packet identifier.
 * @param[in] sent_us Sender timestamp in microseconds.
 * @param[in] received_us Local datagram arrival time in microseconds.
 * @param[in] length Received UDP payload length in bytes.
 * @note Jitter uses Classic iPerf2's RFC 3550-style 1/16 smoothing. Sequence
 * gaps are treated as loss candidates and late packets cancel prior gaps.
 * @warning No packet history is retained: duplicates also count as out of order
 * and may reduce estimated loss. This is not exact deduplication.
 */
void iperf_udp_server_rx_account(bflb_iperf_t *iperf, iperf_udp_rx_t *tracker,
                                 int64_t id, uint64_t sent_us, uint64_t received_us,
                                 uint16_t length)
{
    iperf_stats_t *stats = &iperf->stats;
    int64_t transit_us = (int64_t)(received_us - sent_us);
    int64_t delta_us;
    int64_t jitter_delta;
    uint64_t sequence = (uint64_t)id;
    uint64_t gap;
    bool out_of_order = false;
    uintptr_t irq_flags;

    if (tracker->finished || id < tracker->first_id) {
        return;
    }

    /* Apply the RFC 3550-style 1/16 smoothing used by Classic iPerf2 jitter. */
    if (tracker->last_transit_us != INT64_MIN) {
        delta_us = transit_us - tracker->last_transit_us;
        if (delta_us < 0) {
            delta_us = -delta_us;
        }
        jitter_delta = delta_us - (int64_t)(tracker->jitter_q4 >> 4);
        tracker->jitter_q4 = (uint32_t)((int64_t)tracker->jitter_q4 + jitter_delta);
    }
    tracker->last_transit_us = transit_us;

    /* Sequence gaps add loss candidates; late packets cancel gaps as reordering. */
    if (sequence >= tracker->next_id) {
        gap = sequence - tracker->next_id;
        tracker->gap_count += (uint32_t)LWIP_MIN(gap, (uint64_t)UINT32_MAX - tracker->gap_count);
        tracker->next_id = sequence + 1U;
    } else {
        out_of_order = true;
    }

    /* The tracker is worker-local; publish the complete sample atomically. */
    irq_flags = bflb_irq_save();
    stats->bytes += length;
    stats->datagrams++;
    stats->jitter_us = tracker->jitter_q4 >> 4;
    if (out_of_order) {
        stats->out_of_order++;
    }
    stats->lost = (tracker->gap_count > stats->out_of_order) ?
                      (tracker->gap_count - stats->out_of_order) :
                      0U;
    bflb_irq_restore(irq_flags);
}

/**
 * @brief Finalize receiver loss accounting when a UDP FIN arrives.
 * @param[in,out] iperf Instance whose end time and loss statistics are updated.
 * @param[in,out] tracker Receiver sequence tracker.
 * @param[in] fin_id Negative sequence boundary; only the first FIN is accounted.
 * @note Unsigned subtraction handles INT64_MIN without signed-negation UB.
 * FIN encodes minus the next data ID: expected count is abs(fin_id) - first_id,
 * saturated to UINT32_MAX. The first FIN freezes accounting; later FINs do not.
 */
void iperf_udp_server_rx_finish(bflb_iperf_t *iperf,
                                iperf_udp_rx_t *tracker,
                                int64_t fin_id)
{
    iperf_stats_t *stats = &iperf->stats;
    uint64_t next_id;
    uint64_t expected_datagrams;
    uint64_t gap;
    uint64_t now_us;
    uint32_t datagrams;
    uintptr_t irq_flags;

    if (tracker->finished || fin_id >= 0) {
        return;
    }
    tracker->finished = true;
    next_id = 0ULL - (uint64_t)fin_id;
    /* The FIN identifier carries the sequence number after the last data packet. */
    if (next_id > tracker->next_id) {
        gap = next_id - tracker->next_id;
        tracker->gap_count += (uint32_t)LWIP_MIN(gap, (uint64_t)UINT32_MAX - tracker->gap_count);
        tracker->next_id = next_id;
    }

    /* 2.0.5 sends data 0..N-1, FIN=-N; SEQ64 data starts at one. */
    expected_datagrams = next_id - tracker->first_id;
    datagrams = (uint32_t)LWIP_MIN(expected_datagrams, (uint64_t)UINT32_MAX);
    now_us = iperf_now_us();
    irq_flags = bflb_irq_save();
    if (stats->start_us != 0U && stats->end_us == 0U) {
        stats->end_us = now_us;
    }
    stats->lost = (tracker->gap_count > stats->out_of_order) ?
                      (tracker->gap_count - stats->out_of_order) :
                      0U;
    stats->datagrams = datagrams;
    bflb_irq_restore(irq_flags);
}

/**
 * @brief Encode a server AckFIN with the receiver's selected prefix layout.
 * @param[out] buffer Destination containing at least
 * iperf_udp_server_report_size(format) writable bytes.
 * @param[in] iperf Instance supplying finalized server statistics.
 * @param[in] format Validated receiver layout (112-byte or 128-byte report).
 * @param[in] fin_id Negative FIN packet identifier echoed to the client.
 * @return 112 for SEQ32, 128 for either SEQ64 layout, or zero for invalid format.
 * @note Base layout and VERSION1 flags are unchanged. Trailing zero bytes are
 * 2.0.9/2.0.13 compatibility padding, not extended statistics support.
 * @pre Statistics describe a started receiver; buffer has the selected capacity.
 */
uint16_t iperf_write_udp_server_report(uint8_t *buffer,
                                       const bflb_iperf_t *iperf,
                                       iperf_udp_setup_t format, int64_t fin_id)
{
    iperf_stats_t snapshot;
    const iperf_stats_t *stats = &snapshot;
    uint64_t end_us;
    uint64_t duration_us;
    uint16_t header_size = iperf_udp_header_size(format);
    uint16_t report_size = iperf_udp_server_report_size(format);
    uint64_t now_us;

    if (report_size == 0U) {
        return 0U;
    }
    iperf_stats_snapshot(iperf, &snapshot);
    now_us = iperf_now_us();
    end_us = stats->end_us == 0U ? now_us : stats->end_us;
    duration_us = end_us - stats->start_us;
    /* Clear the full TX report, including padding beyond the base statistics. */
    memset(buffer, 0, report_size);
    if (format == IPERF_UDP_SETUP_SEQ32) {
        iperf_put_u32(buffer, (uint32_t)fin_id);
        iperf_put_u32(buffer + 4, (uint32_t)(now_us / 1000000ULL));
        iperf_put_u32(buffer + 8, (uint32_t)(now_us % 1000000ULL));
    } else {
        iperf_write_udp_header(buffer, fin_id, now_us);
    }
    buffer += header_size;
    iperf_put_u32(buffer, BFLB_IPERF_HEADER_VERSION1);
    iperf_put_u32(buffer + 4, (uint32_t)(stats->bytes >> 32));
    iperf_put_u32(buffer + 8, (uint32_t)stats->bytes);
    iperf_put_u32(buffer + 12, (uint32_t)(duration_us / 1000000ULL));
    iperf_put_u32(buffer + 16, (uint32_t)(duration_us % 1000000ULL));
    iperf_put_u32(buffer + 20, stats->lost);
    iperf_put_u32(buffer + 24, stats->out_of_order);
    iperf_put_u32(buffer + 28, stats->datagrams);
    iperf_put_u32(buffer + 32, stats->jitter_us / 1000000U);
    iperf_put_u32(buffer + 36, stats->jitter_us % 1000000U);
    return report_size;
}

/**
 * @brief Import statistics from a modern or legacy UDP AckFIN report.
 * @param[in,out] iperf Client instance whose UDP result is updated.
 * @param[in] buffer Received report prefix.
 * @param[in] length Number of readable bytes in buffer.
 * @note Unsupported or truncated reports are ignored without modifying the
 * imported server statistics.
 */
void iperf_udp_client_read_report(bflb_iperf_t *iperf,
                                  const uint8_t *buffer,
                                  uint16_t length)
{
    iperf_stats_t *stats = &iperf->stats;
    uint16_t offset = iperf_udp_client_report_offset(buffer, length);
    uint32_t lost, out_of_order, datagrams, jitter_us;
    uintptr_t irq_flags;

    if (offset == 0U) {
        return;
    }
    lost = iperf_get_u32(buffer + offset + 20U);
    out_of_order = iperf_get_u32(buffer + offset + 24U);
    datagrams = iperf_get_u32(buffer + offset + 28U);
    jitter_us = iperf_get_u32(buffer + offset + 32U) * 1000000U +
                iperf_get_u32(buffer + offset + 36U);
    irq_flags = bflb_irq_save();
    stats->lost = lost;
    stats->out_of_order = out_of_order;
    stats->datagrams = datagrams;
    stats->jitter_us = jitter_us;
    stats->udp_report_received = true;
    bflb_irq_restore(irq_flags);
}

/**
 * @brief Print the PC iPerf-style client connection preamble.
 * @param[in] iperf Client instance supplying protocol, destination, datagram,
 * bandwidth, and TCP window settings.
 */
void iperf_log_client_preamble(const bflb_iperf_t *iperf)
{
    ip4_addr_t remote_ip4 = { .addr = iperf->config.remote_ip4 };

    LOG_RI("\r\n");
    LOG_RI("------------------------------------------------------------\r\n");
    LOG_I("Client connecting to %s, %s port %u\r\n",
          ip4addr_ntoa(&remote_ip4),
          iperf->config.proto == BFLB_IPERF_PROTO_UDP ? "UDP" : "TCP",
          iperf->config.port);

    if (iperf->config.proto == BFLB_IPERF_PROTO_UDP) {
        uint32_t bandwidth_bps = iperf->config.bandwidth_bps;
        uint32_t bandwidth_mbps_x10;

        if (bandwidth_bps == 0U) {
            bandwidth_bps = BFLB_IPERF_DEFAULT_UDP_RATE_BPS;
        }
        bandwidth_mbps_x10 = (uint32_t)(((uint64_t)bandwidth_bps + 50000ULL) /
                                        100000ULL);
        LOG_I("Sending %u byte datagrams\r\n", iperf->config.buffer_len);
        LOG_I("UDP target bandwidth: %u.%01u Mbits/sec\r\n",
              bandwidth_mbps_x10 / 10U,
              bandwidth_mbps_x10 % 10U);
    } else {
        uint32_t window_kbytes_x10 = ((uint32_t)TCP_WND * 10U + 512U) / 1024U;

        LOG_I("TCP window size: %u.%01u KByte (configured)\r\n",
              window_kbytes_x10 / 10U,
              window_kbytes_x10 % 10U);
    }
    LOG_RI("------------------------------------------------------------\r\n");
}

/**
 * @brief Print the PC iPerf-style server listening preamble.
 * @param[in] iperf Server instance supplying protocol, listen port, datagram,
 * and TCP window settings.
 */
void iperf_log_server_preamble(const bflb_iperf_t *iperf)
{
    LOG_RI("\r\n");
    LOG_RI("------------------------------------------------------------\r\n");
    LOG_I("Server listening on %s port %u\r\n",
          iperf->config.proto == BFLB_IPERF_PROTO_UDP ? "UDP" : "TCP",
          iperf->config.port);

    if (iperf->config.proto == BFLB_IPERF_PROTO_UDP) {
        LOG_I("Receiving up to %u byte datagrams\r\n",
              iperf->config.buffer_len);
    } else {
        uint32_t window_kbytes_x10 = ((uint32_t)TCP_WND * 10U + 512U) / 1024U;

        LOG_I("TCP window size: %u.%01u KByte (configured)\r\n",
              window_kbytes_x10 / 10U,
              window_kbytes_x10 % 10U);
    }
    LOG_RI("------------------------------------------------------------\r\n");
}

/**
 * @brief Print an established endpoint tuple and report column heading.
 * @param[in] iperf Active instance supplying the reporting context.
 * @param[in] local_ip4 Local IPv4 address in network byte order; zero is
 * rendered as an unspecified wildcard.
 * @param[in] local_port Local port in host byte order.
 * @param[in] remote_ip4 Peer IPv4 address in network byte order.
 * @param[in] remote_port Peer port in host byte order.
 */
void iperf_log_connection(const bflb_iperf_t *iperf,
                          uint32_t local_ip4,
                          uint16_t local_port,
                          uint32_t remote_ip4,
                          uint16_t remote_port)
{
    char local_text[IP4ADDR_STRLEN_MAX];
    char remote_text[IP4ADDR_STRLEN_MAX];
    ip4_addr_t local = { .addr = local_ip4 };
    ip4_addr_t remote = { .addr = remote_ip4 };

    if (local_ip4 == 0U) {
        strcpy(local_text, "*");
    } else {
        ip4addr_ntoa_r(&local, local_text, sizeof(local_text));
    }
    ip4addr_ntoa_r(&remote, remote_text, sizeof(remote_text));
    LOG_I("[  1] local %s port %u connected with %s port %u\r\n",
          local_text, local_port, remote_text, remote_port);
    LOG_I("[ ID] Interval       Transfer     Bandwidth\r\n");
}

/**
 * @brief Record the test start time and initialize interval-report baselines.
 * @param[in,out] iperf Instance whose statistics are initialized.
 * @note This operation is idempotent; an existing start timestamp is retained.
 */
void iperf_test_begin(bflb_iperf_t *iperf)
{
    iperf_stats_t *stats = &iperf->stats;
    uint64_t now_us = iperf_now_us();
    uintptr_t irq_flags;

    irq_flags = bflb_irq_save();
    if (stats->start_us != 0U) {
        bflb_irq_restore(irq_flags);
        return;
    }
    stats->start_us = now_us;
    stats->last_report_us = stats->start_us;
    stats->last_report_bytes = 0U;
    bflb_irq_restore(irq_flags);
}

/**
 * @brief Record the test end time once.
 * @param[in,out] iperf Instance whose statistics are finalized.
 * @note No end time is recorded before the test starts, and an existing end
 * timestamp is retained.
 */
void iperf_test_end(bflb_iperf_t *iperf)
{
    uint64_t now_us = iperf_now_us();
    uintptr_t irq_flags = bflb_irq_save();

    if (iperf->stats.start_us != 0U && iperf->stats.end_us == 0U) {
        /* A task-context begin may have run between the clock read and save. */
        iperf->stats.end_us = LWIP_MAX(now_us, iperf->stats.start_us);
    }
    bflb_irq_restore(irq_flags);
}

/**
 * @brief Account one successful transfer without timing, formatting or logging.
 * @param[in,out] iperf Live instance whose counters are updated.
 * @param[in] length Payload bytes to add; UDP also increments datagrams once.
 * @note Safe from a TCP Raw task-context callback holding the core lock.
 */
void iperf_account_transfer(bflb_iperf_t *iperf, uint32_t length)
{
    uintptr_t irq_flags = bflb_irq_save();

    iperf->stats.bytes += length;
    if (iperf->config.proto == BFLB_IPERF_PROTO_UDP) {
        iperf->stats.datagrams++;
    }
    bflb_irq_restore(irq_flags);
}

/* Worker-only reporting: capture statistics first, calculate/print unlocked. */

/**
 * @brief Emit an interval report when the configured deadline has elapsed.
 * @param[in,out] iperf Active instance whose report baselines are maintained.
 * @param[in] now_us Current monotonic time in microseconds.
 * @return true if a due report was formatted and passed to the logger; false
 * when disabled, not started, or not yet due. Logger filtering is not checked.
 * @note No report is emitted when interval reporting is disabled or the test
 * has not started. Called only by the instance worker, outside the core lock.
 */
bool iperf_test_report_periodic(bflb_iperf_t *iperf, uint64_t now_us)
{
    iperf_stats_t snapshot;
    const iperf_stats_t *stats = &snapshot;
    uintptr_t irq_flags;
    uint64_t report_interval_us = (uint64_t)iperf->config.interval_s * 1000000ULL;
    uint64_t interval_us;
    uint64_t interval_bytes;
    uint64_t interval_start_ds;
    uint64_t interval_end_ds;
    uint64_t bps;
    uint64_t mbytes_x100;
    uint32_t mbps_x10;

    if (report_interval_us == 0U) {
        return false;
    }

    irq_flags = bflb_irq_save();
    if (iperf->stats.end_us != 0U) {
        now_us = iperf->stats.end_us;
    }
    if (iperf->stats.start_us == 0U || now_us <= iperf->stats.last_report_us ||
        now_us - iperf->stats.last_report_us < report_interval_us) {
        bflb_irq_restore(irq_flags);
        return false;
    }
    snapshot = iperf->stats;
    /* Advance from exactly the captured bytes, never from post-LOG traffic. */
    iperf->stats.last_report_us = now_us;
    iperf->stats.last_report_bytes = snapshot.bytes;
    bflb_irq_restore(irq_flags);

    /* Interval bandwidth uses deltas so previous traffic is not counted again. */
    interval_us = now_us - stats->last_report_us;
    interval_bytes = stats->bytes - stats->last_report_bytes;
    bps = interval_bytes * 8ULL * 1000000ULL / interval_us;
    mbytes_x100 = (interval_bytes * 100ULL + (512ULL * 1024ULL)) / (1024ULL * 1024ULL);
    mbps_x10 = (uint32_t)((bps + 50000ULL) / 100000ULL);
    interval_start_ds = (stats->last_report_us - stats->start_us + 50000ULL) / 100000ULL;
    interval_end_ds = (now_us - stats->start_us + 50000ULL) / 100000ULL;

    LOG_I("[%u] %2llu.%01llu-%2llu.%01llu sec  %llu.%02llu MByte  %3u.%01u Mbits/sec\r\n",
          0U,
          (unsigned long long)(interval_start_ds / 10ULL),
          (unsigned long long)(interval_start_ds % 10ULL),
          (unsigned long long)(interval_end_ds / 10ULL),
          (unsigned long long)(interval_end_ds % 10ULL),
          (unsigned long long)(mbytes_x100 / 100ULL),
          (unsigned long long)(mbytes_x100 % 100ULL),
          mbps_x10 / 10U,
          mbps_x10 % 10U);
    return true;
}

/**
 * @brief Finalize timing and print the aggregate transfer report.
 * @param[in,out] iperf Completed instance whose final statistics are reported.
 * @note UDP servers print locally measured receive statistics. UDP clients
 * print jitter, loss, and out-of-order counts only after importing a valid
 * AckFIN report. An instance that never started produces no report.
 * @pre Worker context outside the TCP/IP core lock, after accounting has ended.
 */
void iperf_test_report_finish(bflb_iperf_t *iperf)
{
    iperf_stats_t snapshot;
    const iperf_stats_t *stats = &snapshot;
    uint64_t duration_us;
    uint64_t duration_ds;
    uint64_t bps;
    uint64_t mbytes_x100;
    uint32_t mbps_x10;

    iperf_test_end(iperf);
    iperf_stats_snapshot(iperf, &snapshot);
    iperf_test_report_periodic(iperf, stats->end_us);
    if (stats->start_us == 0U) {
        return;
    }
    duration_us = stats->end_us - stats->start_us;
    duration_ds = (duration_us + 50000ULL) / 100000ULL;
    bps = duration_us == 0U ? 0U : (stats->bytes * 8ULL * 1000000ULL / duration_us);
    mbytes_x100 = (stats->bytes * 100ULL + (512ULL * 1024ULL)) / (1024ULL * 1024ULL);
    mbps_x10 = (uint32_t)((bps + 50000ULL) / 100000ULL);

    LOG_I("[SUM] 0.0-%llu.%01llu sec  %llu.%02llu MByte  %u.%01u Mbits/sec\r\n",
          (unsigned long long)(duration_ds / 10ULL),
          (unsigned long long)(duration_ds % 10ULL),
          (unsigned long long)(mbytes_x100 / 100ULL),
          (unsigned long long)(mbytes_x100 % 100ULL),
          mbps_x10 / 10U,
          mbps_x10 % 10U);

    if (iperf->config.proto == BFLB_IPERF_PROTO_UDP &&
        (iperf->config.role == BFLB_IPERF_ROLE_SERVER ||
         stats->udp_report_received)) {
        uint32_t loss_percent_x10 = 0;
        if (stats->datagrams) {
            loss_percent_x10 = (uint32_t)(((uint64_t)stats->lost * 1000ULL + stats->datagrams / 2U) / stats->datagrams);
        }
        LOG_I("    jitter %u.%03u ms  lost %u/%u (%u.%01u%%)  out-of-order %u\r\n",
              stats->jitter_us / 1000U,
              stats->jitter_us % 1000U,
              stats->lost,
              stats->datagrams,
              loss_percent_x10 / 10U,
              loss_percent_x10 % 10U,
              stats->out_of_order);
    } else if (iperf->config.proto == BFLB_IPERF_PROTO_UDP &&
               iperf->config.role == BFLB_IPERF_ROLE_CLIENT) {
        LOG_W("failed to receive a valid UDP server report\r\n");
    }

    LOG_RI("------------------------------------------------------------\r\n");
}

/**
 * @brief Copy internal statistics into a public result snapshot.
 * @param[in] iperf Instance to inspect.
 * @param[out] result Destination result structure.
 * @pre Hold the lifecycle mutex for stable state/error; statistics use their
 * own short single-CPU IRQ guard, not the TCP/IP core lock.
 * @note The statistics are copied inside an interrupt critical section to
 * prevent torn 64-bit reads on 32-bit targets. A running test uses the current
 * monotonic timestamp when calculating duration and bandwidth.
 */
void iperf_result_snapshot(const bflb_iperf_t *iperf,
                           bflb_iperf_result_t *result)
{
    iperf_stats_t stats;
    uint64_t start_us;
    uint64_t end_us;

    memset(result, 0, sizeof(*result));
    /* Callers hold the lifecycle mutex; statistics have their own IRQ guard. */
    iperf_stats_snapshot(iperf, &stats);
    result->state = iperf->state;
    result->error = iperf->error;
    result->bytes = stats.bytes;
    result->datagrams = stats.datagrams;
    result->lost = stats.lost;
    result->out_of_order = stats.out_of_order;
    result->jitter_us = stats.jitter_us;
    start_us = stats.start_us;
    end_us = stats.end_us;

    if (end_us == 0U && start_us != 0U) {
        end_us = iperf_now_us();
    }
    result->duration_us = start_us == 0U ? 0U : end_us - start_us;
    result->bits_per_second = result->duration_us == 0U ?
                                  0U :
                                  (result->bytes * 8ULL * 1000000ULL / result->duration_us);
}
