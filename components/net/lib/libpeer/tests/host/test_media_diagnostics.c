#include "media_diagnostics.h"
#include "rtp.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint32_t now;
static unsigned delivered;
uint32_t ports_get_epoch_time(void) { return now; }
static void payload(uint8_t *p, size_t size, void *ctx) {
  (void)ctx;
  assert(size == 160 && p[0] == 0xd5);
  delivered++;
}
static void put32(uint8_t *p, uint32_t value) {
  p[0] = value >> 24; p[1] = value >> 16; p[2] = value >> 8; p[3] = value;
}
static void receive(RtpDiagnostics *s, uint16_t seq, uint32_t stamp, uint32_t ms) {
  rtp_diagnostics_receive(s, 42, seq, stamp, 160, ms);
}
static void sequences(void) {
  RtpDiagnostics s; RtpDiagnosticReport r;
  rtp_diagnostics_init(&s, 8000, 1);
  receive(&s, 100, 0, 0); receive(&s, 101, 160, 20);
  receive(&s, 103, 480, 60);
  rtp_diagnostics_get(&s, &r);
  assert(r.received_packets == 3 && r.expected_packets == 3 && r.packets_lost == 1);
  receive(&s, 102, 320, 61);
  rtp_diagnostics_get(&s, &r);
  assert(r.packets_lost == 0 && r.reordered_packets == 1);
  receive(&s, 103, 480, 62);
  rtp_diagnostics_get(&s, &r);
  assert(r.packets_lost == -1 && r.duplicates_recent == 1);
  assert(r.timestamp_gaps == 0); /* Late/duplicate packets are not source gaps. */
  receive(&s, 10000, 1000, 80);
  receive(&s, 10001, 1160, 100);
  rtp_diagnostics_get(&s, &r);
  assert(r.sequence_rejects == 1 && r.sequence_restarts == 1);
  assert(r.expected_packets == 1 && r.packets_lost == 0);

  rtp_diagnostics_init(&s, 8000, 1);
  receive(&s, 65534, UINT32_MAX - 319, UINT32_MAX - 39);
  receive(&s, 65535, UINT32_MAX - 159, UINT32_MAX - 19);
  receive(&s, 0, 0, 0); receive(&s, 1, 160, 20);
  rtp_diagnostics_get(&s, &r);
  assert(r.expected_packets == 3 && r.packets_lost == 0);
  assert(r.jitter_us == 0 && r.max_arrival_gap_ms == 20);
  receive(&s, 65535, UINT32_MAX - 159, 21);
  rtp_diagnostics_get(&s, &r);
  assert(r.duplicates_recent == 1 && r.expected_packets == 3);

  rtp_diagnostics_receive(&s, 77, 10, 0, 160, 100);
  rtp_diagnostics_get(&s, &r);
  assert(r.ssrc == 77 && r.received_packets == 1 && r.expected_packets == 0);
}
static void timing(void) {
  RtpDiagnostics s; RtpDiagnosticReport r;
  rtp_diagnostics_init(&s, 8000, 1);
  receive(&s, 1, 0, 0); receive(&s, 2, 160, 20); receive(&s, 3, 320, 60);
  rtp_diagnostics_get(&s, &r);
  assert(r.jitter_us == 1250 && r.max_arrival_gap_ms == 40 && !r.packets_lost);
  receive(&s, 4, 640, 100);
  rtp_diagnostics_get(&s, &r);
  assert(r.timestamp_gaps == 1 && r.timestamp_gap_samples == 160);
  assert(r.packets_lost == 0); /* Source timestamp gap, no missing RTP packet. */
  rtp_diagnostics_init(&s, 48000, 0);
  receive(&s, 1, 0, 0); receive(&s, 2, 960, 20); receive(&s, 3, 2880, 60);
  rtp_diagnostics_get(&s, &r);
  assert(!r.timestamp_gaps); /* Opus payload byte count is not sample count. */
}
static void rtcp(void) {
  RtpDiagnostics s; RtpDiagnosticReport r;
  rtp_diagnostics_init(&s, 8000, 1);
  uint8_t sr[28] = {0x80, 200, 0, 6};
  put32(sr + 4, 42); put32(sr + 20, 500); put32(sr + 24, 80000);
  rtp_diagnostics_rtcp(&s, sr, sizeof(sr), 42, 7);
  uint8_t rr[32] = {0x81, 201, 0, 7};
  put32(rr + 4, 42); put32(rr + 8, 7);
  put32(rr + 12, 0x20ffffff); put32(rr + 20, 80);
  rtp_diagnostics_rtcp(&s, rr, sizeof(rr), 42, 7);
  rtp_diagnostics_get(&s, &r);
  assert(r.sender_reports == 1 && r.sender_packets == 500 && r.sender_octets == 80000);
  assert(r.remote_reports == 1 && r.remote_packets_lost == -1);
  assert(r.remote_fraction_lost_256 == 32 && r.remote_jitter_us == 10000);
  assert(r.received_packets == 0 && r.packets_lost == 0); /* Directions separate. */
  for (size_t len = 0; len < sizeof(rr); ++len)
    rtp_diagnostics_rtcp(&s, rr, len, 42, 7);
  rr[0] = 0x82; /* Two reports advertised, only one present. */
  rtp_diagnostics_rtcp(&s, rr, sizeof(rr), 42, 7);
  rr[0] = 0xa1; rr[31] = 32; /* Padding consumes the body. */
  rtp_diagnostics_rtcp(&s, rr, sizeof(rr), 42, 7);
  rr[0] = 0x81; rr[31] = 0;
  rtp_diagnostics_rtcp(&s, rr, sizeof(rr), 42, 99);
  rtp_diagnostics_get(&s, &r);
  assert(r.remote_reports == 1);
  /* A legal 32-bit RTCP jitter value can exceed the public microsecond range. */
  put32(rr + 20, UINT32_MAX);
  rtp_diagnostics_rtcp(&s, rr, sizeof(rr), 42, 7);
  rtp_diagnostics_get(&s, &r);
  assert(r.remote_reports == 2 && r.remote_jitter_us == UINT32_MAX);
}
static void decoder(void) {
  RtpDecoder d; RtpDiagnosticReport r;
  assert(rtp_decoder_init(&d, CODEC_PCMA, payload, NULL) == 0);
  uint8_t packet[172] = {0x80, 8};
  put32(packet + 8, 42); memset(packet + 12, 0xd5, 160);
  for (unsigned i = 0; i < 4; ++i) {
    packet[3] = (uint8_t)i; put32(packet + 4, 160 * i); now = 20 * i;
    assert(rtp_decoder_decode(&d, packet, sizeof(packet)) == sizeof(packet));
  }
  packet[0] = 0xb0; /* Invalid extension/padding must not alter statistics. */
  assert(rtp_decoder_decode(&d, packet, sizeof(packet)) == -1);
  rtp_diagnostics_get(&d.diagnostics, &r);
  assert(delivered == 4 && r.received_packets == 4 && r.expected_packets == 3);
  assert(r.packets_lost == 0 && r.jitter_us == 0);
  rtp_decoder_deinit(&d);
}
int main(void) {
  sequences(); timing(); rtcp(); decoder();
  puts("Media diagnostics: loss/recovery, duplicates, wrap/restart, jitter, source gaps, bounded RTCP and unchanged PCM delivery passed");
}
