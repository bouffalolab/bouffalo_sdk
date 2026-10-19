#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <unistd.h>

#include "address.h"
#include "socket.h"
#include "utils.h"

#define MDNS_GROUP "224.0.0.251"
#define MDNS_PORT 5353
#define DNS_TYPE_A 1
#define DNS_CLASS_IN 1

typedef struct DnsHeader {
  uint16_t id;
  uint16_t flags;
  uint16_t questions;
  uint16_t answer_rrs;
  uint16_t authority_rrs;
  uint16_t additional_rrs;
} DnsHeader;

static uint16_t dns_read_u16(const uint8_t* p) {
  return (uint16_t)((p[0] << 8) | p[1]);
}

static uint32_t dns_read_u32(const uint8_t* p) {
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
         ((uint32_t)p[2] << 8) | p[3];
}

static int dns_skip_name(const uint8_t* buf, size_t size, size_t* offset) {
  size_t pos = *offset;

  while (pos < size) {
    uint8_t length = buf[pos];

    if (length == 0) {
      *offset = pos + 1;
      return 0;
    }
    if ((length & 0xC0) == 0xC0) {
      if (pos + 1 >= size) {
        return -1;
      }
      *offset = pos + 2;
      return 0;
    }
    if ((length & 0xC0) != 0 || length > 63 || pos + 1 + length > size) {
      return -1;
    }
    pos += 1 + length;
  }

  return -1;
}

static int dns_name_matches(const uint8_t* buf, size_t size, size_t offset,
                            const char* hostname, size_t* next_offset) {
  char name[256];
  size_t pos = offset;
  size_t name_len = 0;
  int jumped = 0;
  int hops = 0;

  while (pos < size && hops++ < 32) {
    uint8_t length = buf[pos];

    if (length == 0) {
      if (!jumped && next_offset) {
        *next_offset = pos + 1;
      }
      name[name_len] = '\0';
      return strcasecmp(name, hostname) == 0 ? 0 : -1;
    }
    if ((length & 0xC0) == 0xC0) {
      size_t target;

      if (pos + 1 >= size) {
        return -1;
      }
      target = ((size_t)(length & 0x3F) << 8) | buf[pos + 1];
      if (target >= size) {
        return -1;
      }
      if (!jumped && next_offset) {
        *next_offset = pos + 2;
        jumped = 1;
      }
      pos = target;
      continue;
    }
    if ((length & 0xC0) != 0 || length > 63 || pos + 1 + length > size) {
      return -1;
    }
    if (name_len != 0) {
      if (name_len + 1 >= sizeof(name)) {
        return -1;
      }
      name[name_len++] = '.';
    }
    if (name_len + length >= sizeof(name)) {
      return -1;
    }
    memcpy(name + name_len, buf + pos + 1, length);
    name_len += length;
    pos += 1 + length;
  }

  return -1;
}

static int mdns_parse_answer(const uint8_t* buf, size_t size,
                             const char* hostname, Address* addr) {
  const DnsHeader* header;
  size_t pos = sizeof(DnsHeader);
  uint32_t records;

  if (size < sizeof(DnsHeader)) {
    return -1;
  }

  header = (const DnsHeader*)buf;
  if ((ntohs(header->flags) & 0x8000) == 0) {
    return -1;
  }

  for (uint16_t i = 0; i < ntohs(header->questions); i++) {
    if (dns_skip_name(buf, size, &pos) != 0 || pos + 4 > size) {
      return -1;
    }
    pos += 4;
  }

  records = (uint32_t)ntohs(header->answer_rrs) +
            (uint32_t)ntohs(header->authority_rrs) +
            (uint32_t)ntohs(header->additional_rrs);
  for (uint32_t i = 0; i < records; i++) {
    size_t rdata_offset;
    int name_matches;
    uint16_t type;
    uint16_t rdlength;

    name_matches = dns_name_matches(buf, size, pos, hostname, NULL) == 0;
    if (dns_skip_name(buf, size, &pos) != 0 || pos + 10 > size) {
      return -1;
    }

    type = dns_read_u16(buf + pos);
    (void)dns_read_u16(buf + pos + 2);
    (void)dns_read_u32(buf + pos + 4);
    rdlength = dns_read_u16(buf + pos + 8);
    rdata_offset = pos + 10;
    if (rdata_offset + rdlength > size) {
      return -1;
    }

    if (name_matches && type == DNS_TYPE_A && rdlength == 4) {
      addr_set_family(addr, AF_INET);
      memcpy(&addr->sin.sin_addr, buf + rdata_offset, 4);
      return 0;
    }
    pos = rdata_offset + rdlength;
  }

  return -1;
}

static int mdns_build_query(const char* hostname, uint8_t* buf, size_t size) {
  DnsHeader* header;
  size_t offset = sizeof(DnsHeader);
  const char* label = hostname;

  if (size < sizeof(DnsHeader) + sizeof(uint16_t) * 2) {
    return -1;
  }

  memset(buf, 0, size);
  header = (DnsHeader*)buf;
  header->questions = htons(1);

  while (*label) {
    const char* dot = strchr(label, '.');
    size_t label_len = dot ? (size_t)(dot - label) : strlen(label);

    if (label_len == 0 || label_len > 63 || offset + label_len + 1 >= size) {
      return -1;
    }
    buf[offset++] = (uint8_t)label_len;
    memcpy(buf + offset, label, label_len);
    offset += label_len;
    label = dot ? dot + 1 : label + label_len;
  }

  if (offset + 5 > size) {
    return -1;
  }
  buf[offset++] = 0;
  buf[offset++] = 0;
  buf[offset++] = DNS_TYPE_A;
  buf[offset++] = 0;
  buf[offset++] = DNS_CLASS_IN;
  return (int)offset;
}

int mdns_resolve_addr(const char* hostname, Address* addr) {
  Address mcast_addr = {0};
  UdpSocket udp_socket;
  uint8_t buf[1500];
  char addr_string[ADDRSTRLEN];
  int query_size;

  if (udp_socket_open(&udp_socket, AF_INET, MDNS_PORT) < 0) {
    LOGE("Failed to create mDNS socket");
    return -1;
  }

  addr_from_string(MDNS_GROUP, &mcast_addr);
  addr_set_port(&mcast_addr, MDNS_PORT);
  if (udp_socket_add_multicast_group(&udp_socket, &mcast_addr) < 0) {
    LOGE("Failed to join mDNS group");
    udp_socket_close(&udp_socket);
    return -1;
  }

  query_size = mdns_build_query(hostname, buf, sizeof(buf));
  if (query_size < 0) {
    LOGE("Invalid mDNS hostname: %s", hostname);
    udp_socket_close(&udp_socket);
    return -1;
  }

  for (int send_retry = 0; send_retry < 3; send_retry++) {
    if (udp_socket_sendto(&udp_socket, &mcast_addr, buf, query_size) < 0) {
      continue;
    }

    for (int recv_retry = 0; recv_retry < 3; recv_retry++) {
      fd_set rfds;
      struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
      int ret;

      FD_ZERO(&rfds);
      FD_SET(udp_socket.fd, &rfds);
      ret = select(udp_socket.fd + 1, &rfds, NULL, NULL, &tv);
      if (ret < 0) {
        LOGE("mDNS select error");
        break;
      }
      if (ret == 0 || !FD_ISSET(udp_socket.fd, &rfds)) {
        continue;
      }

      ret = udp_socket_recvfrom(&udp_socket, NULL, buf, sizeof(buf));
      if (ret > 0 && mdns_parse_answer(buf, (size_t)ret, hostname, addr) == 0) {
        addr_to_string(addr, addr_string, sizeof(addr_string));
        LOGI("Resolved mDNS %s -> %s", hostname, addr_string);
        udp_socket_close(&udp_socket);
        return 1;
      }
    }
  }

  LOGW("Failed to resolve mDNS hostname: %s", hostname);
  udp_socket_close(&udp_socket);
  return 0;
}
