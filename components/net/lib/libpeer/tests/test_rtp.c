#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "rtp.h"

typedef struct TestOutput {
  uint8_t data[64];
  size_t size;
  int calls;
} TestOutput;

static void on_packet(uint8_t* packet, size_t size, void* user_data) {
  TestOutput* output = user_data;

  output->calls++;
  output->size = size;
  if (size <= sizeof(output->data))
    memcpy(output->data, packet, size);
}

static size_t make_rtp(uint8_t* packet, uint16_t sequence, uint32_t timestamp,
                       const uint8_t* payload, size_t payload_size) {
  memset(packet, 0, 12 + payload_size);
  packet[0] = 0x80;
  packet[1] = PT_H264;
  packet[2] = (uint8_t)(sequence >> 8);
  packet[3] = (uint8_t)sequence;
  packet[4] = (uint8_t)(timestamp >> 24);
  packet[5] = (uint8_t)(timestamp >> 16);
  packet[6] = (uint8_t)(timestamp >> 8);
  packet[7] = (uint8_t)timestamp;
  memcpy(packet + 12, payload, payload_size);
  return 12 + payload_size;
}

static int output_equals(const TestOutput* output, const uint8_t* expected,
                         size_t expected_size, int calls) {
  return output->calls == calls && output->size == expected_size &&
         memcmp(output->data, expected, expected_size) == 0;
}

static int test_single_nalu(void) {
  static const uint8_t payload[] = {0x65, 0x11, 0x22};
  static const uint8_t expected[] = {0x00, 0x00, 0x00, 0x01,
                                     0x65, 0x11, 0x22};
  uint8_t packet[32];
  TestOutput output = {0};
  RtpDecoder decoder;
  int ok;

  if (rtp_decoder_init(&decoder, CODEC_H264, on_packet, &output) != 0)
    return 0;
  ok = rtp_decoder_decode(&decoder, packet,
                          make_rtp(packet, 1, 9000, payload,
                                   sizeof(payload))) > 0 &&
       output_equals(&output, expected, sizeof(expected), 1);
  rtp_decoder_deinit(&decoder);
  return ok;
}

static int test_interleaved_fu_a(void) {
  static const uint8_t start_a[] = {0x7c, 0x85, 0x11};
  static const uint8_t end_a[] = {0x7c, 0x45, 0x12};
  static const uint8_t start_b[] = {0x7c, 0x81, 0x21};
  static const uint8_t end_b[] = {0x7c, 0x41, 0x22};
  static const uint8_t expected_a[] = {0x00, 0x00, 0x00, 0x01,
                                       0x65, 0x11, 0x12};
  static const uint8_t expected_b[] = {0x00, 0x00, 0x00, 0x01,
                                       0x61, 0x21, 0x22};
  uint8_t packet[32];
  TestOutput output_a = {0}, output_b = {0};
  RtpDecoder decoder_a, decoder_b;
  int ok = 0;

  if (rtp_decoder_init(&decoder_a, CODEC_H264, on_packet, &output_a) != 0)
    return 0;
  if (rtp_decoder_init(&decoder_b, CODEC_H264, on_packet, &output_b) != 0) {
    rtp_decoder_deinit(&decoder_a);
    return 0;
  }

  if (rtp_decoder_decode(&decoder_a, packet,
                         make_rtp(packet, 10, 9000, start_a,
                                  sizeof(start_a))) < 0 ||
      rtp_decoder_decode(&decoder_b, packet,
                         make_rtp(packet, 20, 12000, start_b,
                                  sizeof(start_b))) < 0 ||
      rtp_decoder_decode(&decoder_a, packet,
                         make_rtp(packet, 11, 9000, end_a,
                                  sizeof(end_a))) < 0 ||
      rtp_decoder_decode(&decoder_b, packet,
                         make_rtp(packet, 21, 12000, end_b,
                                  sizeof(end_b))) < 0)
    goto done;

  ok = output_equals(&output_a, expected_a, sizeof(expected_a), 1) &&
       output_equals(&output_b, expected_b, sizeof(expected_b), 1);
done:
  rtp_decoder_deinit(&decoder_a);
  rtp_decoder_deinit(&decoder_b);
  return ok;
}

static int test_fu_a_loss(void) {
  static const uint8_t start[] = {0x7c, 0x85, 0x11};
  static const uint8_t end[] = {0x7c, 0x45, 0x12};
  uint8_t packet[32];
  TestOutput output = {0};
  RtpDecoder decoder;
  int ok;

  if (rtp_decoder_init(&decoder, CODEC_H264, on_packet, &output) != 0)
    return 0;
  ok = rtp_decoder_decode(&decoder, packet,
                          make_rtp(packet, 30, 9000, start,
                                   sizeof(start))) > 0 &&
       rtp_decoder_decode(&decoder, packet,
                          make_rtp(packet, 32, 9000, end,
                                   sizeof(end))) < 0 &&
       rtp_decoder_decode(&decoder, packet,
                          make_rtp(packet, 33, 9000, end,
                                   sizeof(end))) < 0 &&
       output.calls == 0;
  rtp_decoder_deinit(&decoder);
  return ok;
}

static int test_stap_a(void) {
  static const uint8_t payload[] = {
      0x78, 0x00, 0x02, 0x67, 0x11, 0x00, 0x03, 0x68, 0x22, 0x33};
  static const uint8_t expected[] = {
      0x00, 0x00, 0x00, 0x01, 0x68, 0x22, 0x33};
  uint8_t packet[32];
  TestOutput output = {0};
  RtpDecoder decoder;
  int ok;

  if (rtp_decoder_init(&decoder, CODEC_H264, on_packet, &output) != 0)
    return 0;
  ok = rtp_decoder_decode(&decoder, packet,
                          make_rtp(packet, 1, 9000, payload,
                                   sizeof(payload))) > 0 &&
       output_equals(&output, expected, sizeof(expected), 2);
  rtp_decoder_deinit(&decoder);
  return ok;
}

int main(void) {
  int pass = test_single_nalu() && test_interleaved_fu_a() &&
             test_fu_a_loss() && test_stap_a();

  printf("RTP depacketizer: %s\n", pass ? "PASS" : "FAIL");
  return pass ? 0 : 1;
}
