/**
 * @file iperf_common.h
 * @brief Private Classic iPerf2 protocol, statistics, and reporting helpers.
 * @note Depends only on standard types and the public opaque handle. Instance
 * management belongs to iperf_internal.h; implementations are in iperf_common.c.
 *
 * @section iperf_udp_wire_layout Classic iPerf2 UDP wire layout
 * All offsets below are byte offsets within the UDP payload, excluding the
 * Ethernet/IP/UDP headers. Numeric fields use network byte order (big-endian).
 * Packet length includes the iPerf prefix, settings (if present), and test data;
 * a 12/16-byte prefix does NOT mean the whole datagram is only 12/16 bytes.
 *
 * @verbatim
 * Sequence/timestamp prefix:
 * Offset  Size  SEQ32 (2.0.5)              SEQ64 / SEQ64_EXT
 *   0       4   Signed 32-bit packet ID    Low 32 bits of packet ID
 *   4       4   Timestamp seconds         Timestamp seconds
 *   8       4   Timestamp microseconds    Timestamp microseconds
 *  12       4   [Client settings begin]   High 32 bits of packet ID
 *  16           --                       [Client settings begin]
 *
 * Normal-mode client layouts supported by this component:
 * SEQ32     : [12-byte prefix][24-byte base settings][test data ...]
 * SEQ64     : [16-byte prefix][4-byte flags][remaining payload ...]
 * SEQ64_EXT : [16-byte prefix][24-byte base][40-byte extension][data ...]
 *
 * Base client settings (offsets relative to the start of settings):
 *  +0       4   flags
 *  +4       4   numThreads
 *  +8       4   port
 * +12       4   buffer length (zero may select the peer's default)
 * +16       4   window/bandwidth field (meaning depends on mode/version)
 * +20       4   amount: positive byte limit or negative duration in 0.01 s
 * @endverbatim
 *
 * - SEQ32: normal-mode flags at offset 12 are zero. Data IDs start at zero.
 *   Recognition needs a complete 36-byte setup prefix and any nonnegative
 *   signed 32-bit ID; threads at offset 16 must be one as a format marker.
 * - SEQ64: flags at offset 16 contain SEQNO64B (0x08000000), without EXTEND.
 *   In 2.0.13 normal mode, bytes after flags can be ASCII test data, NOT valid
 *   base settings. Recognition needs only 20 bytes and any nonnegative signed
 *   64-bit ID. This receiver skips ID 0 and uses one as its data sequence base.
 * - SEQ64_EXT: flags contain SEQNO64B and EXTEND (0x40000000). Recognition
 *   requires any nonnegative signed 64-bit ID and a complete 80-byte setup
 *   prefix, without base-settings validation. This component's TX also
 *   sets LEN_BIT (0x00010000) and encodes the 80-byte length in LEN_MASK.
 *   Its target rate is written at absolute offset 64. Other extensions are
 *   version-dependent and are not fully interpreted by this component.
 *
 * All layouts require timestamp usec < 1000000 and reject VERSION1 setup
 * flags. Unconsumed base fields (port, buffer length, window/bandwidth, amount)
 * are not checked. Each server worker warns only on its first setup rejection,
 * including the reason; subsequent rejections are silently discarded.
 *
 * The first accepted datagram starts receiver timing and selects the peer and
 * layout, even if IDs 0/1 were lost. Initial loss still uses the protocol base
 * (zero for SEQ32, one for SEQ64 layouts), not the first received ID. Negative
 * FIN IDs cannot establish a session. Delayed old data with a valid prefix can
 * claim a new session; this recognizer cannot distinguish it from a new test.
 *
 * @subsection iperf_udp_fin FIN and server AckFIN
 * A negative signed packet ID denotes FIN, not ordinary test data. The first
 * FIN fixes the next-ID boundary and freezes receive statistics; expected
 * datagrams = abs(FIN ID) - first_id (zero for SEQ32, one for SEQ64 layouts).
 * Repeated FINs trigger another reply without recounting the transfer.
 *
 * @verbatim
 * AckFIN: [same 12/16-byte prefix][40-byte base server report][zero padding]
 * This component sends 112 bytes for SEQ32, 128 bytes for SEQ64/SEQ64_EXT.
 * The trailing 60/72 bytes are compatibility padding for 2.0.9/2.0.13,
 * not extended statistics support. Base layout and VERSION1 flags stay unchanged.
 * Client reception requires only the 52/56-byte base prefix, not the padding.
 * The prefix echoes the FIN ID. Settings are replaced by the server report.
 *
 * Server report offsets relative to its start (12 or 16 in the UDP payload):
 *  +0       4   flags: VERSION1 (0x80000000) for the emitted base report
 *  +4       4   Total bytes, high 32 bits
 *  +8       4   Total bytes, low 32 bits
 * +12       4   Transfer duration, seconds
 * +16       4   Transfer duration, microseconds remainder
 * +20       4   Lost datagrams
 * +24       4   Out-of-order datagrams
 * +28       4   Expected datagrams
 * +32       4   Jitter, seconds
 * +36       4   Jitter, microseconds remainder
 * @endverbatim
 * @note This describes the supported normal-mode subset, not every iPerf2
 * extension or compatibility (-C) mode. Never cast payloads to native structs:
 * use the byte-oriented helpers for alignment, byte order and layout handling.
 */

#ifndef IPERF_COMMON_H
#define IPERF_COMMON_H

#include <stdbool.h>
#include <stdint.h>

#include "bflb_iperf.h"

/** @name Protocol constants and shared payload
 * @{
 */

/** @brief Size of the modern UDP sequence and timestamp prefix, in bytes. */
#define BFLB_IPERF_UDP_HEADER_SIZE        16U
/** @brief Size of the Classic iPerf2 version-1 client header, in bytes. */
#define BFLB_IPERF_UDP_CLIENT_V1_SIZE     24U
/** @brief Size of the extended client settings area, in bytes. */
#define BFLB_IPERF_UDP_CLIENT_EXT_SIZE    40U
/** @brief Total size of a modern normal-mode UDP setup header, in bytes. */
#define BFLB_IPERF_UDP_CLIENT_HEADER_SIZE (BFLB_IPERF_UDP_HEADER_SIZE + BFLB_IPERF_UDP_CLIENT_V1_SIZE + BFLB_IPERF_UDP_CLIENT_EXT_SIZE)
/** @brief Size of the base server AckFIN report, in bytes. */
#define BFLB_IPERF_SERVER_HEADER_SIZE     40U
/** @brief Modern AckFIN base receive minimum/prefix capacity (56), not TX size. */
#define BFLB_IPERF_UDP_ACK_SIZE           (BFLB_IPERF_UDP_HEADER_SIZE + BFLB_IPERF_SERVER_HEADER_SIZE)
/** @brief Size of the legacy 32-bit UDP sequence prefix, in bytes. */
#define BFLB_IPERF_UDP_LEGACY_HEADER_SIZE 12U
/** @brief Legacy AckFIN base receive minimum (52), not padded TX size. */
#define BFLB_IPERF_UDP_LEGACY_ACK_SIZE    (BFLB_IPERF_UDP_LEGACY_HEADER_SIZE + BFLB_IPERF_SERVER_HEADER_SIZE)
/** @brief SEQ32 AckFIN TX size with zero compatibility padding for 2.0.9. */
#define BFLB_IPERF_UDP_LEGACY_ACK_TX_SIZE 112U
/** @brief Both SEQ64 AckFIN TX sizes with zero compatibility padding for 2.0.13. */
#define BFLB_IPERF_UDP_ACK_TX_SIZE        128U
/** @brief Classic iPerf2 flag indicating a version-1 control header. */
#define BFLB_IPERF_HEADER_VERSION1        0x80000000UL
/** @brief Classic iPerf2 flag indicating an extended client header. */
#define BFLB_IPERF_HEADER_EXTEND          0x40000000UL
/** @brief Classic iPerf2 flag indicating 64-bit UDP sequence numbers. */
#define BFLB_IPERF_HEADER_SEQNO64B        0x08000000UL
/** @brief Classic iPerf2 flag indicating an encoded header length. */
#define BFLB_IPERF_HEADER_LEN_BIT         0x00010000UL
/** @brief Mask containing the encoded client header length. */
#define BFLB_IPERF_HEADER_LEN_MASK        0x000001FEUL

/** @brief Maximum TCP buffer size for the lwIP Raw backend. */
#define BFLB_IPERF_RAW_TCP_BUFFER_LEN     4096U

/** @brief Permanent immutable payload referenced by zero-copy Raw sends. */
extern const uint8_t g_iperf_raw_payload[BFLB_IPERF_RAW_TCP_BUFFER_LEN];

/** @} */

/** @name Statistics and UDP receiver types
 * @{
 */

/**
 * @brief Mutable statistics produced by one backend instance.
 * @note Timestamps use the monotonic microsecond clock returned by
 * iperf_now_us(); zero denotes a test boundary that has not been recorded.
 * All published fields (including report baselines) are read/written under
 * short bflb_irq_save() sections. Do not acquire locks or log inside them.
 * This protects tasks on one CPU, not instances shared across SMP CPUs.
 */
typedef struct {
    uint64_t start_us;          /**< Test start time, or zero before traffic begins. */
    uint64_t end_us;            /**< Test end time, or zero while the test is active. */
    uint64_t bytes;             /**< Accounted payload bytes. */
    uint64_t last_report_us;    /**< Timestamp of the previous interval report. */
    uint64_t last_report_bytes; /**< Byte count at the previous interval report. */
    uint32_t datagrams;         /**< UDP datagrams received, or expected after FIN. */
    uint32_t lost;              /**< Estimated lost UDP datagrams. */
    uint32_t out_of_order;      /**< UDP datagrams received out of order. */
    uint32_t jitter_us;         /**< Smoothed UDP inter-arrival jitter in microseconds. */
    bool udp_report_received;   /**< A valid UDP server AckFIN report was imported. */
} iperf_stats_t;

/** @brief Supported normal-mode layouts (prefix width and settings extension). */
typedef enum {
    IPERF_UDP_SETUP_INVALID = 0, /**< Unsupported or insufficient setup prefix. */
    IPERF_UDP_SETUP_SEQ32,       /**< iPerf2 2.0.5: 12-byte prefix, ID zero is data. */
    IPERF_UDP_SETUP_SEQ64,       /**< iPerf2 2.0.13: 16-byte prefix, no extension. */
    IPERF_UDP_SETUP_SEQ64_EXT,   /**< iPerf2 2.2.1: 16-byte prefix with extension. */
} iperf_udp_setup_t;

/**
 * @brief Receiver-side UDP sequence and jitter tracker.
 * @note Initialize with iperf_udp_server_rx_init() before accounting data packets.
 * Owned exclusively by the UDP worker; not shared with callbacks/readers.
 */
typedef struct {
    iperf_udp_setup_t format; /**< Layout recognized from the accepted setup prefix. */
    uint8_t first_id;         /**< Zero for SEQ32, one for SEQ64 layouts. */
    bool finished;            /**< Freeze statistics after the first FIN. */
    uint64_t next_id;         /**< Next expected nonnegative UDP sequence number. */
    uint32_t gap_count;       /**< Accumulated sequence-gap candidates. */
    int64_t last_transit_us;  /**< Previous transit time, or INT64_MIN without a sample. */
    uint32_t jitter_q4;       /**< Jitter accumulator in Q4 microseconds. */
} iperf_udp_rx_t;

/** @} */

/** @name Clock and statistics snapshots
 * @{
 */

/**
 * @brief Read the monotonic hardware timer.
 * @return Current timestamp in microseconds.
 */
uint64_t iperf_now_us(void);

/**
 * @brief Copy all statistics under the same single-CPU IRQ guard as writers.
 * @param[in] iperf Live instance to inspect.
 * @param[out] stats Non-NULL statistics destination.
 * @note No mutex, formatting or logging inside the short IRQ section.
 */
void iperf_stats_snapshot(const bflb_iperf_t *iperf, iperf_stats_t *stats);

/** @} */

/** @name Wire encoding and UDP tracking
 * @{
 */

/**
 * @brief Write one unsigned 32-bit value in network byte order.
 * @param[out] buffer Destination containing at least four bytes.
 * @param[in] value Value to encode.
 */
void iperf_put_u32(uint8_t *buffer, uint32_t value);

/**
 * @brief Read one unsigned 32-bit value in network byte order.
 * @param[in] buffer Source containing at least four bytes.
 * @return Decoded host-order value.
 */
uint32_t iperf_get_u32(const uint8_t *buffer);

/**
 * @brief Write the modern UDP sequence and timestamp prefix.
 * @param[out] buffer Destination containing BFLB_IPERF_UDP_HEADER_SIZE bytes.
 * @param[in] id Signed packet ID; a negative value denotes FIN.
 * @param[in] now_us Packet timestamp in microseconds.
 */
void iperf_write_udp_header(uint8_t *buffer, int64_t id, uint64_t now_us);

/**
 * @brief Read the sender timestamp shared by both UDP prefix layouts.
 * @param[in] buffer Source containing at least 12 bytes.
 * @return Sender timestamp in microseconds.
 */
uint64_t iperf_read_udp_timestamp(const uint8_t *buffer);

/**
 * @brief Write a normal-mode UDP client setup header with packet ID one.
 * @param[out] buffer Destination containing BFLB_IPERF_UDP_CLIENT_HEADER_SIZE bytes.
 * @param[in] iperf Instance supplying normalized client settings.
 * @param[in] now_us Packet timestamp in microseconds.
 */
void iperf_write_udp_client_header(uint8_t *buffer,
                                   const bflb_iperf_t *iperf,
                                   uint64_t now_us);

/**
 * @brief Identify a supported normal-mode UDP client setup header.
 * @param[in] buffer Received datagram prefix.
 * @param[in] length Number of available bytes.
 * @param[out] reason Optional output for a static rejection string; set to
 * NULL on success. Pass NULL if no diagnostic is needed.
 * @return Detected setup format, or IPERF_UDP_SETUP_INVALID.
 * @note Recognizes SEQ32 (36-byte minimum), SEQ64 (20-byte minimum, no base
 * settings check), and SEQ64_EXT (80-byte minimum). Accepts any nonnegative
 * signed ID of the selected width, never FIN. Requires usec < 1000000 and
 * rejects VERSION1 setup flags. SEQ32 flags=0/threads=1 remain format markers;
 * unconsumed base fields are ignored. A SEQ64 claim cannot fall back
 * to SEQ32. Not full protocol validation; see the session limits above.
 */
iperf_udp_setup_t iperf_udp_client_setup_type(const uint8_t *buffer,
                                              uint16_t length, const char **reason);

/**
 * @brief Get the selected sequence/timestamp prefix length.
 * @param[in] format Receiver layout.
 * @return 12 for SEQ32, 16 for SEQ64 layouts, zero for invalid format.
 */
uint16_t iperf_udp_header_size(iperf_udp_setup_t format);

/**
 * @brief Get the selected server AckFIN TX length including zero padding.
 * @param[in] format Receiver layout.
 * @return 112 for SEQ32, 128 for SEQ64 layouts, zero for invalid format.
 */
uint16_t iperf_udp_server_report_size(iperf_udp_setup_t format);

/**
 * @brief Decode an ID only after checking the selected prefix fits in length.
 * @param[in] format Selected session layout.
 * @param[in] buffer Prefix with length readable bytes.
 * @param[in] length Available byte count.
 * @param[out] id Non-NULL decoded ID destination; unchanged on failure.
 * @return true on success, false for invalid layout or insufficient bytes.
 */
bool iperf_udp_decode_id(iperf_udp_setup_t format, const uint8_t *buffer,
                         uint16_t length, int64_t *id);

/**
 * @brief Validate a modern or legacy UDP AckFIN report.
 * @param[in] buffer Received report prefix.
 * @param[in] length Number of available bytes.
 * @return true when a supported version-1 report header is present.
 * @note Checks length/flag placement, not FIN correspondence or all fields.
 * Requires only 52 bytes for legacy or 56 for modern; TX padding is not required.
 */
bool iperf_udp_client_report_valid(const uint8_t *buffer, uint16_t length);

/**
 * @brief Initialize receiver-side UDP sequence and jitter tracking.
 * @param[out] tracker Tracker to reset.
 * @param[in] format Validated setup format; fixed for the peer's session.
 * @post first_id is zero for SEQ32 and one for either SEQ64 layout.
 */
void iperf_udp_server_rx_init(iperf_udp_rx_t *tracker, iperf_udp_setup_t format);

/**
 * @brief Account one received UDP data datagram.
 * @param[in,out] iperf Instance whose statistics are updated.
 * @param[in,out] tracker Receiver sequence and jitter tracker.
 * @param[in] id Nonnegative UDP packet ID.
 * @param[in] sent_us Sender timestamp in microseconds.
 * @param[in] received_us Local datagram arrival time in microseconds.
 * @param[in] length Received datagram length in bytes.
 * @note Loss is estimated from gaps minus out-of-order arrivals; no exact
 * deduplication is performed. The worker exclusively owns tracker.
 */
void iperf_udp_server_rx_account(bflb_iperf_t *iperf, iperf_udp_rx_t *tracker,
                                 int64_t id, uint64_t sent_us, uint64_t received_us,
                                 uint16_t length);

/**
 * @brief Finalize receiver loss accounting when a UDP FIN arrives.
 * @param[in,out] iperf Instance whose statistics are finalized.
 * @param[in,out] tracker Receiver sequence tracker.
 * @param[in] fin_id Negative FIN ID; repeated calls leave statistics unchanged.
 * @note Expected datagrams are abs(fin_id) - first_id, saturated to UINT32_MAX.
 */
void iperf_udp_server_rx_finish(bflb_iperf_t *iperf, iperf_udp_rx_t *tracker,
                                int64_t fin_id);

/**
 * @brief Encode a format-specific UDP server AckFIN report.
 * @param[out] buffer Destination containing iperf_udp_server_report_size(format) bytes.
 * @param[in] iperf Instance supplying finalized server statistics.
 * @param[in] format Layout selected for this receiver.
 * @param[in] fin_id Negative FIN packet ID echoed to the client.
 * @return Actual datagram length (112 or 128), or zero for an invalid format.
 * @note Base report layout/flags are unchanged; trailing bytes are zero-filled
 * for 2.0.9/2.0.13 compatibility, not extended statistics support.
 */
uint16_t iperf_write_udp_server_report(uint8_t *buffer, const bflb_iperf_t *iperf,
                                       iperf_udp_setup_t format, int64_t fin_id);

/**
 * @brief Import statistics from a modern or legacy UDP AckFIN report.
 * @param[in,out] iperf Client instance whose UDP result is updated.
 * @param[in] buffer Received report prefix.
 * @param[in] length Number of available bytes.
 */
void iperf_udp_client_read_report(bflb_iperf_t *iperf, const uint8_t *buffer,
                                  uint16_t length);

/** @} */

/** @name Connection logging
 * @{
 */

/**
 * @brief Print the PC iPerf-style client connection preamble.
 * @param[in] iperf Client instance supplying protocol and destination settings.
 */
void iperf_log_client_preamble(const bflb_iperf_t *iperf);

/**
 * @brief Print the PC iPerf-style server listening preamble.
 * @param[in] iperf Server instance supplying protocol and listen settings.
 */
void iperf_log_server_preamble(const bflb_iperf_t *iperf);

/**
 * @brief Print the established endpoint tuple and report column heading.
 * @param[in] iperf Active instance supplying the protocol.
 * @param[in] local_ip4 Local IPv4 address in network byte order.
 * @param[in] local_port Local port in host byte order.
 * @param[in] remote_ip4 Peer IPv4 address in network byte order.
 * @param[in] remote_port Peer port in host byte order.
 */
void iperf_log_connection(const bflb_iperf_t *iperf,
                          uint32_t local_ip4,
                          uint16_t local_port,
                          uint32_t remote_ip4,
                          uint16_t remote_port);

/** @} */

/** @name Traffic statistics and reports
 * @{
 */

/**
 * @brief Record the test start time once.
 * @param[in,out] iperf Instance whose statistics are initialized.
 */
void iperf_test_begin(bflb_iperf_t *iperf);

/**
 * @brief Record the test end time once.
 * @param[in,out] iperf Instance whose statistics are finalized.
 */
void iperf_test_end(bflb_iperf_t *iperf);

/**
 * @brief Account one successful transfer without formatting or logging.
 * @param[in,out] iperf Active instance.
 * @param[in] length Number of payload bytes to add.
 * @note UDP transfers also increment the datagram count; TCP transfers only
 * update the byte count.
 */
void iperf_account_transfer(bflb_iperf_t *iperf, uint32_t length);

/**
 * @brief Snapshot and print a due interval, then retain its exact baselines.
 * @param[in,out] iperf Active instance owned by the calling worker.
 * @param[in] now_us Current monotonic time in microseconds.
 * @return true when a due report is passed to the logger, false otherwise.
 * @note The return value does not indicate whether logger filtering hides it.
 * @note Worker only, outside the TCP/IP core lock; callbacks only account.
 */
bool iperf_test_report_periodic(bflb_iperf_t *iperf, uint64_t now_us);

/**
 * @brief Finalize timing and print the summary report.
 * @param[in,out] iperf Completed instance.
 * @note Worker only, after callbacks are detached and all accounting is done.
 */
void iperf_test_report_finish(bflb_iperf_t *iperf);

/**
 * @brief Copy internal statistics into a public result snapshot.
 * @param[in] iperf Instance to inspect.
 * @param[out] result Destination result structure.
 * @pre Caller holds the instance lifecycle mutex for state/error stability.
 * @note This helper does not acquire that mutex. Statistics are copied under
 * their independent short single-CPU IRQ guard, not the TCP/IP core lock.
 */
void iperf_result_snapshot(const bflb_iperf_t *iperf,
                           bflb_iperf_result_t *result);

/** @} */

#endif /* IPERF_COMMON_H */