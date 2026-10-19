#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "address.h"
#include "stun.h"

static void set_message_length(uint8_t* packet, uint16_t length) {
  packet[2] = (uint8_t)(length >> 8);
  packet[3] = (uint8_t)length;
}

static int test_integrity_and_fingerprint(void) {
  static const char password[] = "test-password";
  StunMessage message = {0};
  StunMessage parsed = {0};

  stun_msg_create(&message, STUN_CLASS_REQUEST | STUN_METHOD_BINDING);
  if (stun_msg_write_attr(&message, STUN_ATTR_TYPE_USERNAME, 4, "a:b") != 0 ||
      stun_msg_finish(&message, STUN_CREDENTIAL_SHORT_TERM, password,
                      strlen(password)) != 0 ||
      stun_probe(message.buf, message.size) != 0 ||
      stun_msg_is_valid(message.buf, message.size, (char*)password) != 0)
    return 0;

  memcpy(parsed.buf, message.buf, message.size);
  parsed.size = message.size;
  if (stun_parse_msg_buf(&parsed) != 0 || strcmp(parsed.username, "a:b") != 0)
    return 0;

  message.buf[message.size - 1] ^= 1;
  return stun_msg_is_valid(message.buf, message.size, (char*)password) != 0;
}

static int test_malformed_messages(void) {
  StunMessage message = {0};
  uint8_t oversized[240] = {0};

  stun_msg_create(&message, STUN_CLASS_REQUEST | STUN_METHOD_BINDING);
  set_message_length(message.buf, 4);
  if (stun_probe(message.buf, sizeof(StunHeader)) == 0)
    return 0;

  memset(&message, 0, sizeof(message));
  stun_msg_create(&message, STUN_CLASS_REQUEST | STUN_METHOD_BINDING);
  set_message_length(message.buf, 4);
  message.buf[20] = 0x00;
  message.buf[21] = STUN_ATTR_TYPE_USERNAME;
  message.buf[22] = 0x00;
  message.buf[23] = 100;
  message.size = 24;
  if (stun_parse_msg_buf(&message) == 0)
    return 0;

  memset(&message, 0, sizeof(message));
  stun_msg_create(&message, STUN_CLASS_REQUEST | STUN_METHOD_BINDING);
  if (stun_msg_write_attr(&message, STUN_ATTR_TYPE_SOFTWARE,
                          sizeof(oversized), (char*)oversized) == 0)
    return 0;

  memset(&message, 0, sizeof(message));
  stun_msg_create(&message, STUN_CLASS_REQUEST | STUN_METHOD_BINDING);
  memset(oversized, 'x', 128);
  if (stun_msg_write_attr(&message, STUN_ATTR_TYPE_SOFTWARE, 128,
                          (char*)oversized) != 0)
    return 0;
  message.buf[20] = 0x00;
  message.buf[21] = STUN_ATTR_TYPE_USERNAME;
  return stun_parse_msg_buf(&message) != 0;
}

static int test_xor_mapped_address(void) {
  Address input = {0};
  Address output = {0};
  uint8_t mask[16] = {0};
  uint8_t value[20] = {0};
  uint32_t cookie = htonl(MAGIC_COOKIE);

  if (addr_from_string("192.168.18.181", &input) != 1)
    return 0;
  addr_set_port(&input, 45678);
  memcpy(mask, &cookie, sizeof(cookie));
  if (stun_set_mapped_address((char*)value, mask, &input) != 8)
    return 0;
  stun_get_mapped_address((char*)value, mask, &output);
  return addr_equal(&input, &output);
}

static int test_optional_attributes(void) {
  StunMessage message = {0};
  static const char software[] = "libpeer-test";
  uint8_t response_origin[8] = {0, STUN_FAMILY_IPV4, 0, 80, 127, 0, 0, 1};

  stun_msg_create(&message, STUN_CLASS_RESPONSE | STUN_METHOD_BINDING);
  if (stun_msg_write_attr(&message, STUN_ATTR_TYPE_SOFTWARE,
                          sizeof(software) - 1, (char*)software) != 0 ||
      stun_msg_write_attr(&message, STUN_ATTR_TYPE_RESPONSE_ORIGIN,
                          sizeof(response_origin), (char*)response_origin) != 0 ||
      stun_parse_msg_buf(&message) != 0)
    return 0;

  memset(&message, 0, sizeof(message));
  stun_msg_create(&message, STUN_CLASS_RESPONSE | STUN_METHOD_BINDING);
  if (stun_msg_write_attr(&message, STUN_ATTR_TYPE_RESPONSE_ORIGIN, 4,
                          (char*)response_origin) != 0)
    return 0;
  return stun_parse_msg_buf(&message) != 0;
}

int main(void) {
  int integrity = test_integrity_and_fingerprint();
  int malformed = test_malformed_messages();
  int xor_address = test_xor_mapped_address();
  int optional_attributes = test_optional_attributes();
  int pass = integrity && malformed && xor_address && optional_attributes;

  printf("  integrity=%d malformed=%d xor_address=%d optional_attributes=%d\n",
         integrity, malformed, xor_address, optional_attributes);
  printf("STUN bounds/integrity: %s\n", pass ? "PASS" : "FAIL");
  return pass ? 0 : 1;
}
