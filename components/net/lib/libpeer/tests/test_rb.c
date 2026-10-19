#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "buffer.h"

#define CHECK(condition)                                                    \
  do {                                                                      \
    if (!(condition)) {                                                     \
      fprintf(stderr, "CHECK failed at %s:%d: %s\n", __FILE__, __LINE__,  \
              #condition);                                                  \
      return -1;                                                            \
    }                                                                       \
  } while (0)

static int test_basic_records(void) {
  static const uint8_t first[] = "first";
  static const uint8_t second[] = "second";
  Buffer* rb = buffer_new(64);
  uint8_t* data;
  int size;

  CHECK(rb != NULL);
  CHECK(buffer_get_used(rb) == 0);
  CHECK(buffer_get_capacity(rb) == 60);
  CHECK(buffer_push_tail(rb, first, sizeof(first)) == (int)sizeof(first));
  CHECK(buffer_push_tail(rb, second, sizeof(second)) == (int)sizeof(second));
  CHECK(buffer_get_used(rb) == 24);

  data = buffer_peak_head(rb, &size);
  CHECK(data != NULL && size == (int)sizeof(first));
  CHECK(memcmp(data, first, sizeof(first)) == 0);
  buffer_pop_head(rb);
  CHECK(buffer_get_used(rb) == 12);

  data = buffer_peak_head(rb, &size);
  CHECK(data != NULL && size == (int)sizeof(second));
  CHECK(memcmp(data, second, sizeof(second)) == 0);
  buffer_pop_head(rb);
  CHECK(buffer_peak_head(rb, &size) == NULL);
  CHECK(buffer_get_used(rb) == 0);

  buffer_free(rb);
  return 0;
}

static int test_prefixed_record(void) {
  static const uint8_t payload[] = "jpeg";
  const uint16_t expected_sid = 0x1234;
  uint16_t actual_sid = 0;
  Buffer* rb = buffer_new(64);
  uint8_t* data;
  int size;

  CHECK(rb != NULL);
  CHECK(buffer_push_tail_with_prefix(rb, (const uint8_t*)&expected_sid,
                                     sizeof(expected_sid), payload,
                                     sizeof(payload)) == (int)sizeof(payload));
  CHECK(buffer_get_used(rb) == 12);

  data = buffer_peak_head(rb, &size);
  CHECK(data != NULL);
  CHECK(size == (int)(sizeof(expected_sid) + sizeof(payload)));
  memcpy(&actual_sid, data, sizeof(actual_sid));
  CHECK(actual_sid == expected_sid);
  CHECK(memcmp(data + sizeof(actual_sid), payload, sizeof(payload)) == 0);

  buffer_pop_head(rb);
  CHECK(buffer_peak_head(rb, &size) == NULL);
  buffer_free(rb);
  return 0;
}

static int test_wrap_with_prefix(void) {
  uint8_t first[20];
  uint8_t second[12];
  uint8_t payload[8];
  const uint16_t expected_sid = 7;
  uint16_t actual_sid = 0;
  Buffer* rb = buffer_new(48);
  uint8_t* data;
  int size;

  memset(first, 0x11, sizeof(first));
  memset(second, 0x22, sizeof(second));
  memset(payload, 0x33, sizeof(payload));
  CHECK(rb != NULL);

  CHECK(buffer_push_tail(rb, first, sizeof(first)) == (int)sizeof(first));
  CHECK(buffer_push_tail(rb, second, sizeof(second)) == (int)sizeof(second));
  buffer_pop_head(rb);
  CHECK(buffer_push_tail_with_prefix(rb, (const uint8_t*)&expected_sid,
                                     sizeof(expected_sid), payload,
                                     sizeof(payload)) == (int)sizeof(payload));
  CHECK(buffer_get_used(rb) == 40);

  data = buffer_peak_head(rb, &size);
  CHECK(data != NULL && size == (int)sizeof(second));
  CHECK(memcmp(data, second, sizeof(second)) == 0);
  buffer_pop_head(rb);
  /* The end padding remains reserved until the wrap marker is consumed. */
  CHECK(buffer_get_used(rb) == 24);

  data = buffer_peak_head(rb, &size);
  CHECK(buffer_get_used(rb) == 16);
  CHECK(data != NULL);
  CHECK(size == (int)(sizeof(expected_sid) + sizeof(payload)));
  memcpy(&actual_sid, data, sizeof(actual_sid));
  CHECK(actual_sid == expected_sid);
  CHECK(memcmp(data + sizeof(actual_sid), payload, sizeof(payload)) == 0);
  buffer_pop_head(rb);
  CHECK(buffer_peak_head(rb, &size) == NULL);

  buffer_free(rb);
  return 0;
}

static int test_capacity_boundary(void) {
  uint8_t data[20] = {0};
  Buffer* rb = buffer_new(64);
  int size;

  CHECK(rb != NULL);
  CHECK(buffer_push_tail(rb, data, sizeof(data)) == (int)sizeof(data));
  CHECK(buffer_push_tail(rb, data, sizeof(data)) == (int)sizeof(data));

  /* A third 12-byte record would make tail equal head and must be rejected. */
  CHECK(buffer_push_tail(rb, data, 12) < 0);
  CHECK(buffer_peak_head(rb, &size) != NULL && size == (int)sizeof(data));

  buffer_free(rb);
  return 0;
}

int main(void) {
  CHECK(test_basic_records() == 0);
  CHECK(test_prefixed_record() == 0);
  CHECK(test_wrap_with_prefix() == 0);
  CHECK(test_capacity_boundary() == 0);
  puts("buffer tests passed");
  return 0;
}
