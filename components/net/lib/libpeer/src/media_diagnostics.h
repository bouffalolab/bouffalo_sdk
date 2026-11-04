#ifndef LIBPEER_MEDIA_DIAGNOSTICS_H
#define LIBPEER_MEDIA_DIAGNOSTICS_H

#include "config.h"

#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
#include <stddef.h>
#include <stdint.h>

/* Passive, post-SRTP observations. Loss/expected cover the current validated
 * sequence epoch; received includes all observed packets, including probation.
 * Duplicate detection is bounded to 64 sequence numbers. No packet is held,
 * dropped, reordered or retransmitted by this instrumentation. */
typedef struct {
  uint32_t ssrc, clock_rate, received_packets, expected_packets;
  int32_t packets_lost;
  uint32_t duplicates_recent, reordered_packets, untracked_old_packets;
  uint32_t sequence_rejects, sequence_restarts;
  /* Microsecond jitter conversions saturate at UINT32_MAX. */
  uint32_t jitter_us, max_arrival_gap_ms;
  /* G.711 mono only: timestamp gaps between consecutive sequence numbers.
   * These can reflect DTX/source discontinuities, not RTP packet loss. */
  uint32_t timestamp_gaps, timestamp_gap_samples;
  uint32_t sender_reports, sender_packets, sender_octets;
  /* Reports received about OUR outgoing SSRC, a separate direction. */
  uint32_t remote_reports;
  int32_t remote_packets_lost;
  uint32_t remote_fraction_lost_256, remote_jitter_us;
} RtpDiagnosticReport;

typedef struct {
  RtpDiagnosticReport report;
  uint64_t seen, jitter_q4;
  uint32_t cycles, base_seq, bad_seq, validated_received;
  uint32_t last_arrival_ms, last_jitter_ms, last_timestamp;
  /* G.711 mono has one sample per payload byte in the last forward packet. */
  uint32_t contiguous_timestamp, contiguous_payload_samples;
  uint16_t max_seq, seen_seq, contiguous_seq;
  uint8_t initialized, probation, ssrc_known, arrival_known, jitter_known;
  uint8_t seen_known, contiguous_known, g711;
} RtpDiagnostics;

void rtp_diagnostics_init(RtpDiagnostics *state, uint32_t clock_rate, int g711);
void rtp_diagnostics_receive(RtpDiagnostics *state, uint32_t ssrc,
                             uint16_t seq, uint32_t timestamp,
                             size_t payload_size, uint32_t now_ms);
void rtp_diagnostics_get(const RtpDiagnostics *state, RtpDiagnosticReport *report);
/* Exactly one validated RTCP subpacket; the observer rechecks its own bounds. */
void rtp_diagnostics_rtcp(RtpDiagnostics *state, const uint8_t *packet,
                          size_t subpacket_size, uint32_t remote_ssrc, uint32_t local_ssrc);
#endif
#endif
