#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stun.h"
#include "utils.h"

uint32_t CRC32_TABLE[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535,
    0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd,
    0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d,
    0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
    0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac,
    0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab,
    0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
    0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb,
    0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea,
    0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce,
    0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
    0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409,
    0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739,
    0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268,
    0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0,
    0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8,
    0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
    0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703,
    0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7,
    0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae,
    0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6,
    0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d,
    0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5,
    0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
    0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

void stun_msg_create(StunMessage* msg, uint16_t type) {
  StunHeader* header = (StunHeader*)msg->buf;
  header->type = htons(type);
  header->length = 0;
  header->magic_cookie = htonl(MAGIC_COOKIE);
  header->transaction_id[0] = htonl(0x12345678);
  header->transaction_id[1] = htonl(0x90abcdef);
  header->transaction_id[2] = htonl(0x12345678);
  msg->size = sizeof(StunHeader);
}

int stun_set_mapped_address(char* value, uint8_t* mask, Address* addr) {
  const uint8_t* address;
  size_t address_size;
  uint16_t encoded_port;
  uint16_t port_mask;
  char addr_string[ADDRSTRLEN];

  if (!value || !mask || !addr)
    return -1;

  value[0] = 0;

  switch (addr->family) {
    case AF_INET6:
      value[1] = STUN_FAMILY_IPV6;
      address = (const uint8_t*)&addr->sin6.sin6_addr;
      address_size = 16;
      break;
    case AF_INET:
      value[1] = STUN_FAMILY_IPV4;
      address = (const uint8_t*)&addr->sin.sin_addr;
      address_size = 4;
      break;
    default:
      return -1;
  }

  encoded_port = htons(addr->port);
  memcpy(&port_mask, mask, sizeof(port_mask));
  encoded_port ^= port_mask;
  memcpy(value + 2, &encoded_port, sizeof(encoded_port));
  for (size_t i = 0; i < address_size; i++)
    value[4 + i] = (char)(address[i] ^ mask[i]);

  addr_to_string(addr, addr_string, sizeof(addr_string));

  LOGD("XOR Mapped Address Family: %d", value[1]);
  LOGD("XOR Mapped Address Port: %d", addr->port);
  LOGD("XOR Mapped Address IP: %s", addr_string);
  return (int)(4 + address_size);
}

void stun_get_mapped_address(char* value, uint8_t* mask, Address* addr) {
  uint8_t* address;
  size_t address_size;
  uint16_t encoded_port;
  uint16_t port_mask;
  char addr_string[ADDRSTRLEN];
  uint8_t family = value[1];

  switch (family) {
    case STUN_FAMILY_IPV6:
      addr_set_family(addr, AF_INET6);
      address = (uint8_t*)&addr->sin6.sin6_addr;
      address_size = 16;
      break;
    case STUN_FAMILY_IPV4:
      addr_set_family(addr, AF_INET);
      address = (uint8_t*)&addr->sin.sin_addr;
      address_size = 4;
      break;
    default:
      return;
  }

  for (size_t i = 0; i < address_size; i++)
    address[i] = (uint8_t)value[4 + i] ^ mask[i];
  memcpy(&encoded_port, value + 2, sizeof(encoded_port));
  memcpy(&port_mask, mask, sizeof(port_mask));
  encoded_port ^= port_mask;
  addr_set_port(addr, ntohs(encoded_port));

  addr_to_string(addr, addr_string, sizeof(addr_string));

  LOGD("XOR Mapped Address Family: %d", family);
  LOGD("XOR Mapped Address Port: %d", addr->port);
  LOGD("XOR Mapped Address IP: %s", addr_string);
}

int stun_parse_msg_buf(StunMessage* msg) {
  StunHeader* header;
  size_t length;
  size_t pos = sizeof(StunHeader);
  uint8_t mask[16];
  uint32_t cookie;
  int integrity_seen = 0;

  if (!msg || stun_probe(msg->buf, msg->size) != 0)
    return -1;

  header = (StunHeader*)msg->buf;
  cookie = htonl(MAGIC_COOKIE);
  length = sizeof(StunHeader) + ntohs(header->length);
  memset(msg->message_integrity, 0, sizeof(msg->message_integrity));
  memset(msg->username, 0, sizeof(msg->username));
  memset(msg->realm, 0, sizeof(msg->realm));
  memset(msg->nonce, 0, sizeof(msg->nonce));
  msg->fingerprint = 0;
  msg->error_code = 0;

  msg->stunclass = ntohs(header->type);
  if ((msg->stunclass & STUN_CLASS_ERROR) == STUN_CLASS_ERROR) {
    msg->stunclass = STUN_CLASS_ERROR;
  } else if ((msg->stunclass & STUN_CLASS_INDICATION) == STUN_CLASS_INDICATION) {
    msg->stunclass = STUN_CLASS_INDICATION;
  } else if ((msg->stunclass & STUN_CLASS_RESPONSE) == STUN_CLASS_RESPONSE) {
    msg->stunclass = STUN_CLASS_RESPONSE;
  } else if ((msg->stunclass & STUN_CLASS_REQUEST) == STUN_CLASS_REQUEST) {
    msg->stunclass = STUN_CLASS_REQUEST;
  }

  msg->stunmethod = ntohs(header->type) & 0x0FFF;
  if ((msg->stunmethod & STUN_METHOD_ALLOCATE) == STUN_METHOD_ALLOCATE) {
    msg->stunmethod = STUN_METHOD_ALLOCATE;
  } else if ((msg->stunmethod & STUN_METHOD_BINDING) == STUN_METHOD_BINDING) {
    msg->stunmethod = STUN_METHOD_BINDING;
  }

  while (pos < length) {
    StunAttribute* attr = (StunAttribute*)(msg->buf + pos);
    size_t attr_length;
    size_t padded_length;

    if (length - pos < sizeof(StunAttribute))
      return -1;
    attr_length = ntohs(attr->length);
    padded_length = (attr_length + 3) & ~(size_t)3;
    if (padded_length > length - pos - sizeof(StunAttribute))
      return -1;

    memset(mask, 0, sizeof(mask));
    // LOGD("Attribute Type: 0x%04x", ntohs(attr->type));
    // LOGD("Attribute Length: %d", ntohs(attr->length));

    switch (ntohs(attr->type)) {
      case STUN_ATTR_TYPE_MAPPED_ADDRESS:
        if (attr_length != 8 && attr_length != 20)
          return -1;
        stun_get_mapped_address(attr->value, mask, &msg->mapped_addr);
        break;
      case STUN_ATTR_TYPE_USERNAME:
        if (attr_length >= sizeof(msg->username))
          return -1;
        memcpy(msg->username, attr->value, attr_length);
        // LOGD("length = %d, Username %s", ntohs(attr->length), msg->username);
        break;
      case STUN_ATTR_TYPE_MESSAGE_INTEGRITY:
        if (attr_length != sizeof(msg->message_integrity))
          return -1;
        memcpy(msg->message_integrity, attr->value, attr_length);
        integrity_seen = 1;
        break;
      case STUN_ATTR_TYPE_ERROR_CODE:
        /* Attributes after MESSAGE-INTEGRITY are not authenticated. In
         * particular, an appended 403 must not revoke a live connection. */
        if (integrity_seen)
          break;
        if (attr_length < 4 || (uint8_t)attr->value[2] < 3 ||
            (uint8_t)attr->value[2] > 6 || (uint8_t)attr->value[3] > 99)
          return -1;
        msg->error_code = (uint8_t)attr->value[2] * 100 +
                          (uint8_t)attr->value[3];
        break;
      case STUN_ATTR_TYPE_LIFETIME:
        if (attr_length != 4)
          return -1;
        break;
      case STUN_ATTR_TYPE_REALM:
        if (attr_length >= sizeof(msg->realm))
          return -1;
        memcpy(msg->realm, attr->value, attr_length);
        LOGD("Realm %s", msg->realm);
        break;
      case STUN_ATTR_TYPE_NONCE:
        if (attr_length >= sizeof(msg->nonce))
          return -1;
        memcpy(msg->nonce, attr->value, attr_length);
        LOGD("Nonce %s", msg->nonce);
        break;
      case STUN_ATTR_TYPE_XOR_RELAYED_ADDRESS:
        if (attr_length != 8 && attr_length != 20)
          return -1;
        memcpy(mask, &cookie, sizeof(cookie));
        memcpy(mask + 4, header->transaction_id, sizeof(header->transaction_id));
        LOGD("XOR Relayed Address");
        stun_get_mapped_address(attr->value, mask, &msg->relayed_addr);
        break;
      case STUN_ATTR_TYPE_XOR_MAPPED_ADDRESS:
        if (attr_length != 8 && attr_length != 20)
          return -1;
        memcpy(mask, &cookie, sizeof(cookie));
        memcpy(mask + 4, header->transaction_id, sizeof(header->transaction_id));
        stun_get_mapped_address(attr->value, mask, &msg->mapped_addr);
        break;
      case STUN_ATTR_TYPE_PRIORITY:
        if (attr_length != 4)
          return -1;
        break;
      case STUN_ATTR_TYPE_USE_CANDIDATE:
        if (attr_length != 0)
          return -1;
        // LOGD("Use Candidate");
        break;
      case STUN_ATTR_TYPE_FINGERPRINT:
        if (attr_length != sizeof(msg->fingerprint))
          return -1;
        memcpy(&msg->fingerprint, attr->value, attr_length);
        // LOGD("Fingerprint: 0x%.4x", msg->fingerprint);
        break;
      case STUN_ATTR_TYPE_ICE_CONTROLLED:
      case STUN_ATTR_TYPE_ICE_CONTROLLING:
        if (attr_length != 8)
          return -1;
        break;
      case STUN_ATTR_TYPE_SOFTWARE:
        break;
      case STUN_ATTR_TYPE_RESPONSE_ORIGIN:
      case STUN_ATTR_TYPE_OTHER_ADDRESS:
        if (attr_length != 8 && attr_length != 20)
          return -1;
        break;
      case STUN_ATTR_TYPE_NETWORK_COST:
        // Do nothing
        break;
      default:
        if (ntohs(attr->type) < 0x8000) {
          LOGW("Ignoring unsupported comprehension-required STUN attribute: 0x%04x",
               ntohs(attr->type));
        } else {
          LOGD("Ignoring unknown comprehension-optional STUN attribute: 0x%04x",
               ntohs(attr->type));
        }
        break;
    }

    pos += padded_length + sizeof(StunAttribute);
  }
  return pos == length ? 0 : -1;
}

void stun_calculate_fingerprint(char* buf, size_t len, uint32_t* fingerprint) {
  uint32_t c = 0xFFFFFFFF;
  size_t i = 0;

  for (i = 0; i < len; ++i) {
    c = CRC32_TABLE[(c ^ buf[i]) & 0xFF] ^ (c >> 8);
  }

  *fingerprint = htonl((c ^ 0xFFFFFFFF) ^ STUN_FINGERPRINT_XOR);
}

int stun_msg_write_attr(StunMessage* msg, StunAttrType type, uint16_t length, char* value) {
  StunHeader* header;
  StunAttribute* stun_attr;
  size_t padded_length;
  size_t attr_size;

  if (!msg || msg->size < sizeof(StunHeader) ||
      msg->size > sizeof(msg->buf) || (length > 0 && !value))
    return -1;
  if ((type == STUN_ATTR_TYPE_USERNAME && length >= sizeof(msg->username)) ||
      (type == STUN_ATTR_TYPE_REALM && length >= sizeof(msg->realm)) ||
      (type == STUN_ATTR_TYPE_NONCE && length >= sizeof(msg->nonce)))
    return -1;

  padded_length = ((size_t)length + 3) & ~(size_t)3;
  attr_size = sizeof(StunAttribute) + padded_length;
  if (attr_size > sizeof(msg->buf) - msg->size ||
      msg->size + attr_size - sizeof(StunHeader) > UINT16_MAX)
    return -1;

  header = (StunHeader*)msg->buf;
  stun_attr = (StunAttribute*)(msg->buf + msg->size);
  stun_attr->type = htons(type);
  stun_attr->length = htons(length);
  if (length > 0)
    memcpy(stun_attr->value, value, length);
  if (padded_length > length)
    memset(stun_attr->value + length, 0, padded_length - length);

  msg->size += attr_size;
  header->length = htons((uint16_t)(msg->size - sizeof(StunHeader)));

  switch (type) {
    case STUN_ATTR_TYPE_REALM:
      memcpy(msg->realm, value, length);
      msg->realm[length] = '\0';
      break;
    case STUN_ATTR_TYPE_NONCE:
      memcpy(msg->nonce, value, length);
      msg->nonce[length] = '\0';
      break;
    case STUN_ATTR_TYPE_USERNAME:
      memcpy(msg->username, value, length);
      msg->username[length] = '\0';
      break;
    default:
      break;
  }

  return 0;
}

int stun_msg_finish(StunMessage* msg, StunCredential credential, const char* password, size_t password_len) {
  StunHeader* header;
  StunAttribute* stun_attr;
  uint16_t header_length;
  uint32_t fingerprint;
  char key[256];
  char hash_key[17];
  int key_length;

  if (!msg || !password || msg->size < sizeof(StunHeader) ||
      msg->size > sizeof(msg->buf) ||
      sizeof(msg->buf) - msg->size <
          2 * sizeof(StunAttribute) + 20 + sizeof(fingerprint))
    return -1;

  header = (StunHeader*)msg->buf;
  header_length = ntohs(header->length);
  memset(key, 0, sizeof(key));
  memset(hash_key, 0, sizeof(hash_key));

  switch (credential) {
    case STUN_CREDENTIAL_LONG_TERM:
      key_length = snprintf(key, sizeof(key), "%s:%s:%s", msg->username,
                            msg->realm, password);
      if (key_length < 0 || (size_t)key_length >= sizeof(key))
        return -1;
      LOGD("key: %s", key);
      utils_get_md5(key, strlen(key), (unsigned char*)hash_key);
      password = hash_key;
      password_len = 16;
      break;
    default:
      break;
  }

  stun_attr = (StunAttribute*)(msg->buf + msg->size);
  header->length = htons(header_length + 24); /* HMAC-SHA1 */
  stun_attr->type = htons(STUN_ATTR_TYPE_MESSAGE_INTEGRITY);
  stun_attr->length = htons(20);
  utils_get_hmac_sha1((char*)msg->buf, msg->size, password, password_len, (unsigned char*)stun_attr->value);
  msg->size += sizeof(StunAttribute) + 20;
  // FINGERPRINT

  stun_attr = (StunAttribute*)(msg->buf + msg->size);
  header->length = htons(header_length + 24 /* HMAC-SHA1 */ + 8 /* FINGERPRINT */);
  stun_attr->type = htons(STUN_ATTR_TYPE_FINGERPRINT);
  stun_attr->length = htons(4);
  stun_calculate_fingerprint((char*)msg->buf, msg->size, &fingerprint);
  memcpy(stun_attr->value, &fingerprint, sizeof(fingerprint));
  msg->size += sizeof(StunAttribute) + 4;
  return 0;
}

int stun_probe(uint8_t* buf, size_t size) {
  uint16_t type;
  uint16_t message_length;
  uint32_t magic_cookie;

  if (!buf || size < sizeof(StunHeader))
    return -1;

  memcpy(&type, buf, sizeof(type));
  memcpy(&message_length, buf + 2, sizeof(message_length));
  memcpy(&magic_cookie, buf + 4, sizeof(magic_cookie));
  type = ntohs(type);
  message_length = ntohs(message_length);
  if ((type & 0xc000) != 0 || (message_length & 0x0003) != 0 ||
      magic_cookie != htonl(MAGIC_COOKIE) ||
      (size_t)message_length + sizeof(StunHeader) != size)
    return -1;

  return 0;
}

#if 0
StunMsgType stun_is_stun_msg(uint8_t *buf, size_t size) {

  if (size < sizeof(StunHeader)) {
    //LOGE("STUN message is too short.");
    return STUN_MSG_TYPE_INVLID;
  }

  StunHeader *header = (StunHeader *)buf;
  if (header->magic_cookie != htonl(MAGIC_COOKIE)) {
    //LOGE("STUN magic cookie does not match.");
    return STUN_MSG_TYPE_INVLID;
  }

  if (ntohs(header->type) == STUN_BINDING_REQUEST) {

    return STUN_MSG_TYPE_BINDING_REQUEST;

  } else if (ntohs(header->type) == STUN_BINDING_RESPONSE) {

    return STUN_MSG_TYPE_BINDING_RESPONSE;

  } else if (ntohs(header->type) == STUN_BINDING_ERROR_RESPONSE) {

    return STUN_MSG_TYPE_BINDING_ERROR_RESPONSE;
  } else {

    return STUN_MSG_TYPE_INVLID;
  }

  return 0;
}
#endif
int stun_msg_is_valid(uint8_t* buf, size_t size, char* password) {
  StunMessage msg = {0};
  StunHeader* header;
  StunAttribute* message_integrity_attr = NULL;
  size_t message_integrity_offset = 0;
  size_t fingerprint_offset = 0;
  size_t pos = sizeof(StunHeader);
  uint32_t fingerprint = 0;
  uint32_t received_fingerprint = 0;
  unsigned char message_integrity[20];

  if (!buf || !password || size > sizeof(msg.buf) ||
      stun_probe(buf, size) != 0)
    return -1;

  memcpy(msg.buf, buf, size);
  msg.size = size;
  if (stun_parse_msg_buf(&msg) != 0)
    return -1;
  header = (StunHeader*)msg.buf;

  while (pos < size) {
    StunAttribute* attr = (StunAttribute*)(msg.buf + pos);
    size_t attr_length = ntohs(attr->length);
    size_t padded_length = (attr_length + 3) & ~(size_t)3;
    uint16_t attr_type = ntohs(attr->type);

    if (attr_type == STUN_ATTR_TYPE_MESSAGE_INTEGRITY) {
      message_integrity_attr = attr;
      message_integrity_offset = pos;
    } else if (attr_type == STUN_ATTR_TYPE_FINGERPRINT) {
      fingerprint_offset = pos;
      memcpy(&received_fingerprint, attr->value,
             sizeof(received_fingerprint));
    }
    pos += sizeof(StunAttribute) + padded_length;
  }

  if (!message_integrity_attr || !fingerprint_offset ||
      message_integrity_offset >= fingerprint_offset ||
      fingerprint_offset + sizeof(StunAttribute) + sizeof(uint32_t) != size)
    return -1;

  stun_calculate_fingerprint((char*)msg.buf, fingerprint_offset, &fingerprint);
  if (fingerprint != received_fingerprint)
    return -1;

  header->length =
      htons((uint16_t)(message_integrity_offset - sizeof(StunHeader) +
                       sizeof(StunAttribute) + sizeof(message_integrity)));
  utils_get_hmac_sha1((char*)msg.buf, message_integrity_offset, password,
                      strlen(password), message_integrity);
  if (memcmp(message_integrity, message_integrity_attr->value,
             sizeof(message_integrity)) != 0)
    return -1;

  return 0;
}
