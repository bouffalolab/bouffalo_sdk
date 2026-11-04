#include "media_diagnostics.h"

#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
#include <limits.h>
#include <string.h>

static uint32_t read32(const uint8_t *p) {
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
         ((uint32_t)p[2] << 8) | p[3];
}

/* The Q4 filter uses wide intermediates; public microseconds saturate to
 * UINT32_MAX rather than wrapping for a very long stall or extreme jitter. */
static uint32_t time_us(uint64_t ticks, uint32_t rate) {
  uint64_t value = rate ? ticks * 1000000U / rate : 0;
  return value > UINT32_MAX ? UINT32_MAX : (uint32_t)value;
}

void rtp_diagnostics_init(RtpDiagnostics *s, uint32_t rate, int g711) {
  memset(s, 0, sizeof(*s));
  s->report.clock_rate = rate;
  s->g711 = g711 != 0;
}

static void init_sequence(RtpDiagnostics *s, uint16_t seq) {
  s->base_seq = s->max_seq = seq;
  s->bad_seq = 65537U;
  s->cycles = s->validated_received = 0;
  s->seen_known = s->jitter_known = s->contiguous_known = 0;
  s->seen = s->jitter_q4 = 0;
  s->probation = 0;
}

/* RFC 3550 Appendix A.1: two consecutive packets validate a new source;
 * two packets following a large jump confirm a sequence restart. */
static int update_sequence(RtpDiagnostics *s, uint16_t seq) {
  if (!s->initialized) {
    s->initialized = 1;
    s->probation = 1;
    s->max_seq = seq;
    return 0;
  }
  if (s->probation) {
    if (seq == (uint16_t)(s->max_seq + 1)) {
      init_sequence(s, seq);
      s->validated_received++;
      return 1;
    }
    s->max_seq = seq;
    return 0;
  }
  uint16_t delta = (uint16_t)(seq - s->max_seq);
  if (delta < 3000U) {
    if (seq < s->max_seq) s->cycles += 65536U;
    s->max_seq = seq;
  } else if (delta <= 65536U - 100U) {
    if (seq != s->bad_seq) {
      s->bad_seq = (uint16_t)(seq + 1);
      s->report.sequence_rejects++;
      return 0;
    }
    s->report.sequence_restarts++;
    init_sequence(s, seq);
  }
  s->validated_received++;
  return 1;
}

static void note_order(RtpDiagnostics *s, uint16_t seq) {
  if (!s->seen_known) {
    s->seen_known = 1;
    s->seen_seq = seq;
    s->seen = 1;
    return;
  }
  uint16_t forward = (uint16_t)(seq - s->seen_seq);
  if (forward && forward < 32768U) {
    s->seen = forward < 64U ? (s->seen << forward) | 1U : 1U;
    s->seen_seq = seq;
  } else {
    uint16_t back = (uint16_t)(s->seen_seq - seq);
    if (back >= 64U) {
      s->report.untracked_old_packets++;
    } else if (s->seen & (UINT64_C(1) << back)) {
      s->report.duplicates_recent++;
    } else {
      s->seen |= UINT64_C(1) << back;
      s->report.reordered_packets++;
    }
  }
}

void rtp_diagnostics_receive(RtpDiagnostics *s, uint32_t ssrc, uint16_t seq,
                             uint32_t stamp, size_t bytes, uint32_t now) {
  if (!s->report.clock_rate) return;
  if (s->ssrc_known && s->report.ssrc != ssrc)
    rtp_diagnostics_init(s, s->report.clock_rate, s->g711);
  s->ssrc_known = 1;
  s->report.ssrc = ssrc;
  s->report.received_packets++;
  if (s->arrival_known) {
    uint32_t gap = now - s->last_arrival_ms;
    if (gap > s->report.max_arrival_gap_ms) s->report.max_arrival_gap_ms = gap;
  }
  s->arrival_known = 1;
  s->last_arrival_ms = now;
  if (!update_sequence(s, seq)) return;
  note_order(s, seq);
  /* RFC 3550 A.8, integer Q4 filter. Clock differences are wrapping; no
   * wall-clock synchronization is needed. All accepted arrivals contribute. */
  if (s->jitter_known) {
    uint32_t dt = stamp - s->last_timestamp;
    int64_t stamp_delta = dt <= INT32_MAX ? (int64_t)dt : (int64_t)dt - INT64_C(4294967296);
    int64_t d = (int64_t)((uint64_t)(uint32_t)(now - s->last_jitter_ms) *
                         s->report.clock_rate / 1000U) - stamp_delta;
    uint64_t magnitude = (uint64_t)(d < 0 ? -d : d);
    uint64_t filtered = (s->jitter_q4 + 8U) >> 4;
    if (magnitude >= filtered) s->jitter_q4 += magnitude - filtered;
    else s->jitter_q4 -= filtered - magnitude;
  }
  s->jitter_known = 1;
  s->last_jitter_ms = now;
  s->last_timestamp = stamp;
  if (s->g711 && s->contiguous_known && seq == (uint16_t)(s->contiguous_seq + 1)) {
    uint32_t delta = stamp - s->contiguous_timestamp;
    if (delta > s->contiguous_payload_samples && delta < UINT32_C(0x80000000)) {
      s->report.timestamp_gaps++;
      s->report.timestamp_gap_samples += delta - s->contiguous_payload_samples;
    }
  }
  if (!s->contiguous_known ||
      ((uint16_t)(seq - s->contiguous_seq) > 0 &&
       (uint16_t)(seq - s->contiguous_seq) < 32768U)) {
    s->contiguous_known = 1;
    s->contiguous_seq = seq;
    s->contiguous_timestamp = stamp;
    s->contiguous_payload_samples = (uint32_t)bytes;
  }
}

void rtp_diagnostics_get(const RtpDiagnostics *s, RtpDiagnosticReport *r) {
  *r = s->report;
  if (s->initialized && !s->probation) {
    r->expected_packets = s->cycles + s->max_seq - s->base_seq + 1U;
    int64_t lost = (int64_t)r->expected_packets - s->validated_received;
    r->packets_lost = lost > INT32_MAX ? INT32_MAX :
                      lost < INT32_MIN ? INT32_MIN : (int32_t)lost;
  }
  r->jitter_us = time_us(s->jitter_q4 >> 4, r->clock_rate);
}

void rtp_diagnostics_rtcp(RtpDiagnostics *s, const uint8_t *p, size_t subpacket_size,
                          uint32_t remote_ssrc, uint32_t local_ssrc) {
  size_t size = subpacket_size;
  if (!p || size < 4 || p[0] >> 6 != 2 || !s->report.clock_rate) return;
  size_t wire_size = 4U * ((((size_t)p[2] << 8) | p[3]) + 1U);
  if (wire_size != size) return;
  if (p[0] & 0x20) {
    size_t padding = p[size - 1];
    if (!padding || padding > size - 4) return;
    size -= padding;
  }
  size_t offset = p[1] == 200 ? 28U : 8U;
  if ((p[1] != 200 && p[1] != 201) || size < offset) return;
  unsigned count = p[0] & 31U;
  if (count > (size - offset) / 24U) return;
  if (p[1] == 200 && read32(p + 4) == remote_ssrc) {
    s->report.sender_reports++;
    s->report.sender_packets = read32(p + 20);
    s->report.sender_octets = read32(p + 24);
  }
  for (unsigned i = 0; i < count; ++i, offset += 24U) {
    const uint8_t *block = p + offset;
    if (read32(block) != local_ssrc) continue;
    uint32_t lost = read32(block + 4) & UINT32_C(0x00ffffff);
    s->report.remote_reports++;
    s->report.remote_packets_lost = lost & UINT32_C(0x00800000) ?
        (int32_t)lost - INT32_C(16777216) : (int32_t)lost;
    s->report.remote_fraction_lost_256 = block[4];
    s->report.remote_jitter_us = time_us(read32(block + 12), s->report.clock_rate);
  }
}
#endif
