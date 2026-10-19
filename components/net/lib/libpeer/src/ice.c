#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "ice.h"
#include "mdns.h"
#include "ports.h"
#include "socket.h"
#include "utils.h"

static uint8_t ice_candidate_type_preference(IceCandidateType type) {
  switch (type) {
    case ICE_CANDIDATE_TYPE_HOST:
      return 126;
    case ICE_CANDIDATE_TYPE_SRFLX:
      return 100;
    case ICE_CANDIDATE_TYPE_RELAY:
      return 0;
    default:
      return 0;
  }
}

static uint16_t ice_candidate_local_preference(IceCandidate* candidate) {
  return candidate->addr.port;
}

static void ice_candidate_priority(IceCandidate* candidate) {
  // priority = (2^24)*(type preference) + (2^8)*(local preference) + (256 - component ID)
  candidate->priority = (1 << 24) * ice_candidate_type_preference(candidate->type) + (1 << 8) * ice_candidate_local_preference(candidate) + (256 - candidate->component);
}

void ice_candidate_create(IceCandidate* candidate, int foundation, IceCandidateType type, Address* addr) {
  memcpy(&candidate->addr, addr, sizeof(Address));
  candidate->type = type;

  snprintf(candidate->foundation, sizeof(candidate->foundation), "%d", foundation);
  // 1: RTP, 2: RTCP
  candidate->component = 1;

  ice_candidate_priority(candidate);

  snprintf(candidate->transport, sizeof(candidate->transport), "%s", "UDP");
}

void ice_candidate_to_description(IceCandidate* candidate, char* description, int length) {
  char addr_string[ADDRSTRLEN];
  char raddr_str[ADDRSTRLEN];
  char typ_raddr[128];

  memset(typ_raddr, 0, sizeof(typ_raddr));
  addr_to_string(&candidate->raddr, raddr_str, sizeof(raddr_str));

  switch (candidate->type) {
    case ICE_CANDIDATE_TYPE_HOST:
      snprintf(typ_raddr, sizeof(typ_raddr), "host");
      break;
    case ICE_CANDIDATE_TYPE_SRFLX:
      if (candidate->raddr.port != 0 && candidate->raddr.sin.sin_addr.s_addr != 0) {
        snprintf(typ_raddr, sizeof(typ_raddr), "srflx raddr %s rport %d", raddr_str, candidate->raddr.port);
      } else {
        snprintf(typ_raddr, sizeof(typ_raddr), "srflx");
      }
      break;
    case ICE_CANDIDATE_TYPE_RELAY:
      if (candidate->raddr.port != 0 && candidate->raddr.sin.sin_addr.s_addr != 0) {
        snprintf(typ_raddr, sizeof(typ_raddr), "relay raddr %s rport %d", raddr_str, candidate->raddr.port);
      } else {
        snprintf(typ_raddr, sizeof(typ_raddr), "relay");
      }
      break;
    default:
      break;
  }

  addr_to_string(&candidate->addr, addr_string, sizeof(addr_string));
  snprintf(description, length, "a=candidate:%s %d %s %" PRIu32 " %s %d typ %s\r\n",
           candidate->foundation,
           candidate->component,
           candidate->transport,
           candidate->priority,
           addr_string,
           candidate->addr.port,
           typ_raddr);
}

int ice_candidate_from_description(IceCandidate* candidate, const char* description,
                                   const char* end) {
  char line[512];
  const char* candidate_start;
  size_t line_len;
  uint32_t port;
  char type[16];
  char addrstring[ADDRSTRLEN];

  if (!candidate || !description || !end || end < description) {
    return -1;
  }

  line_len = (size_t)(end - description);
  if (line_len == 0 || line_len >= sizeof(line)) {
    LOGE("ICE candidate line is empty or too long");
    return -1;
  }

  memcpy(line, description, line_len);
  line[line_len] = '\0';
  candidate_start = line;

  if (strncmp("a=", candidate_start, 2) == 0) {
    candidate_start += 2;
  }
  if (strncmp("candidate:", candidate_start, strlen("candidate:")) != 0) {
    return -1;
  }
  candidate_start += strlen("candidate:");

  memset(candidate, 0, sizeof(*candidate));

  // a=candidate:448736988 1 udp 2122260223 172.17.0.1 49250 typ host generation 0 network-id 1 network-cost 50
  // a=candidate:udpcandidate 1 udp 120 192.168.1.102 8000 typ host
  if (sscanf(candidate_start, "%32s %d %32s %" SCNu32 " %45s %" SCNu32 " typ %15s",
             candidate->foundation,
             &candidate->component,
             candidate->transport,
             &candidate->priority,
             addrstring,
             &port,
             type) != 7) {
    LOGE("Failed to parse ICE candidate description");
    return -1;
  }

  if (strcmp(candidate->transport, "UDP") != 0 && strcmp(candidate->transport, "udp") != 0) {
    LOGE("Only UDP transport is supported");
    return -1;
  }

  if (strcmp(type, "host") == 0) {
    candidate->type = ICE_CANDIDATE_TYPE_HOST;
  } else if (strcmp(type, "srflx") == 0) {
    candidate->type = ICE_CANDIDATE_TYPE_SRFLX;
  } else if (strcmp(type, "relay") == 0) {
    candidate->type = ICE_CANDIDATE_TYPE_RELAY;
  } else {
    LOGE("Unknown candidate type: %s", type);
    return -1;
  }

  if (port == 0 || port > UINT16_MAX) {
    LOGE("Invalid ICE candidate port: %" PRIu32, port);
    return -1;
  }

  if (strstr(addrstring, ".local") != NULL) {
    if (mdns_resolve_addr(addrstring, &candidate->addr) == 0) {
      LOGW("Failed to resolve mDNS address");
      return -1;
    }
  } else if (addr_from_string(addrstring, &candidate->addr) == 0) {
    return -1;
  }

  addr_set_port(&candidate->addr, (uint16_t)port);

  return 0;
}
