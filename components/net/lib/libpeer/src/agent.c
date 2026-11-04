#include "config.h"
#include <errno.h>
#if !CONFIG_USE_LWIP
#include <fcntl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "agent.h"
#include "base64.h"
#include "ice.h"
#include "ports.h"
#include "socket.h"
#include "stun.h"
#include "utils.h"

#ifndef AGENT_POLL_TIMEOUT
#define AGENT_POLL_TIMEOUT 1
#endif
/* RFC 8489 UDP defaults: RTO 500 ms, seven transmissions, final wait 8 s.
 * Monotonic deadlines are independent of the application's polling rate. */
#define AGENT_CHECK_RTO_MS 500U
#define AGENT_CHECK_TIMEOUT_MS 39500U
#define AGENT_CHECK_TRANSMISSIONS 7
#define AGENT_CHECK_PACING_MS 50U
#ifndef AGENT_STUN_RECV_MAXTIMES
#define AGENT_STUN_RECV_MAXTIMES 1000
#endif

/* RFC 7675: refresh every 4..6 s, expire after 30 s without a response. */
#define AGENT_CONSENT_TIMEOUT_MS 30000U
#define AGENT_CONSENT_MIN_INTERVAL_MS 4000U
#define AGENT_CONSENT_JITTER_MS 2001U

/* A separate socket preserves the TURN allocation's UDP five-tuple and
 * disambiguates local relay vs host routes to the same remote endpoint. */
struct AgentTurn {
  TurnClient client;
  UdpSocket socket;
  uint8_t receive[TURN_CONTROL_SIZE + 1];
};

static int agent_turn_send(void* user, const Address* server,
                            const uint8_t* data, size_t size) {
  Agent* agent = user;
  Address destination = *server;
  return udp_socket_sendto(&agent->turn->socket, &destination, data, (int)size);
}

static int agent_turn_random(void* user, uint8_t* data, size_t size) {
  return utils_random_get_bytes(((Agent*)user)->random, data, size);
}

static int agent_turn_receive(Agent* agent, Address* peer, uint8_t* data,
                               size_t capacity, int timeout_ms) {
  AgentTurn* turn = agent->turn;
  Address source;
  fd_set reads;
  struct timeval timeout = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
  int n;
  if (!turn || turn->socket.fd < 0 || turn->socket.fd >= FD_SETSIZE) return 0;
  turn_poll(&turn->client, ports_get_epoch_time());
  FD_ZERO(&reads); FD_SET(turn->socket.fd, &reads);
  n = select(turn->socket.fd + 1, &reads, NULL, NULL, &timeout);
  if (n <= 0) return n;
  n = udp_socket_recvfrom(&turn->socket, &source, turn->receive, sizeof(turn->receive));
  if (n <= 0 || n > TURN_CONTROL_SIZE) return 0;
  n = turn_input(&turn->client, &source, turn->receive, (size_t)n, peer,
                 data, capacity, ports_get_epoch_time());
  return n > 0 ? n : 0; /* Invalid/control datagrams never escape as peer data. */
}

static void agent_close_turn(Agent* agent) {
  AgentTurn* turn = agent->turn;
  if (!turn) return;
  if (turn->client.state == TURN_READY &&
      turn_release(&turn->client, ports_get_epoch_time()) == 0) {
    /* Synchronous destroy has no future owner tick. Allow a bounded 200 ms
     * for the release ACK, then close locally even if the server is absent. */
    for (unsigned i = 0; i < 20 && turn->client.state == TURN_RELEASING; ++i)
      agent_turn_receive(agent, NULL, NULL, 0, 10);
  }
  udp_socket_close(&turn->socket);
  turn_destroy(&turn->client);
  free(turn);
  agent->turn = NULL;
  agent->received_relay = 0;
}

static int agent_pair_is_relay(const IceCandidatePair* pair) {
  return pair && pair->local && pair->local->type == ICE_CANDIDATE_TYPE_RELAY;
}

static int agent_send_pair(Agent* agent, IceCandidatePair* pair,
                            const uint8_t* data, int size);
static void agent_start_pair(IceCandidatePair* pair, uint32_t now);

static uint64_t agent_pair_priority(Agent* agent, const IceCandidatePair* pair) {
  uint64_t g = agent->mode == AGENT_MODE_CONTROLLING ? pair->local->priority : pair->remote->priority;
  uint64_t d = agent->mode == AGENT_MODE_CONTROLLING ? pair->remote->priority : pair->local->priority;
  return ((g < d ? g : d) << 32) + 2 * (g > d ? g : d) + (g > d);
}

int agent_set_direct_preference(Agent* agent, uint32_t wait_ms) {
  if (!agent || agent->selected_pair || wait_ms > 10000U ||
      utils_random_get_bytes(agent->random, (unsigned char*)&agent->tie_breaker,
                             sizeof(agent->tie_breaker)) != 0) return -1;
  agent->regular_nomination = 1;
  agent->direct_preference_ms = wait_ms;
  for (int i = 0; i < agent->candidate_pairs_num; ++i)
    agent->candidate_pairs[i].priority = agent_pair_priority(agent, &agent->candidate_pairs[i]);
  return 0;
}

static void agent_reset_consent(Agent* agent) {
  for (int i = 0; i < agent->candidate_pairs_num; ++i) {
    agent->candidate_pairs[i].check_pending = 0;
    agent->candidate_pairs[i].triggered = 0;
  }
  agent->check_sent = 0;
  agent->nomination_pair = NULL;
  agent->checks_started = 0;
  memset(agent->stun_transactions, 0, sizeof(agent->stun_transactions));
  agent->stun_transaction_next = 0;
  agent->consent_active = 0;
  agent->consent_probing = 0;
  agent->consent_expired = 0;
  agent->consent_received_ms = 0;
  agent->binding_sent_ms = 0;
  agent->consent_interval_ms = 0;
}

static int agent_consent_poll_expiry(Agent* agent, uint32_t now) {
  if ((agent->consent_active || agent->consent_probing) &&
      (uint32_t)(now - agent->consent_received_ms) >= AGENT_CONSENT_TIMEOUT_MS) {
    agent->consent_expired = 1;
  }
  if (agent->consent_expired) {
    memset(agent->stun_transactions, 0, sizeof(agent->stun_transactions));
    return 1;
  }
  return 0;
}

void agent_clear_candidates(Agent* agent) {
  agent_close_turn(agent);
  agent->local_candidates_count = 0;
  agent->remote_candidates_count = 0;
  agent->candidate_pairs_num = 0;
  agent->selected_pair = NULL;
  agent->nominated_pair = NULL;
  memset(agent->remote_ufrag, 0, sizeof(agent->remote_ufrag));
  memset(agent->remote_upwd, 0, sizeof(agent->remote_upwd));
  agent_reset_consent(agent);
}

int agent_create(Agent* agent) {
  int ret;
  if (!agent) return -1;
  agent->udp_sockets[0].fd = -1;
  agent->udp_sockets[1].fd = -1;
  agent->turn = NULL;
  agent->received_relay = 0;
  agent->relay_only = 0;
  agent->regular_nomination = 0;
  agent->direct_preference_ms = 0;
  agent->tie_breaker = 0;
  agent->random = utils_random_create();
  if (!agent->random) return -1;
  if ((ret = udp_socket_open(&agent->udp_sockets[0], AF_INET, 0)) < 0) {
    LOGE("Failed to create UDP socket.");
    utils_random_destroy(agent->random);
    agent->random = NULL;
    return ret;
  }
  LOGI("create IPv4 UDP socket: %d", agent->udp_sockets[0].fd);

#if CONFIG_IPV6
  if ((ret = udp_socket_open(&agent->udp_sockets[1], AF_INET6, 0)) < 0) {
    LOGE("Failed to create IPv6 UDP socket.");
    udp_socket_close(&agent->udp_sockets[0]);
    agent->udp_sockets[0].fd = -1;
    utils_random_destroy(agent->random);
    agent->random = NULL;
    return ret;
  }
  LOGI("create IPv6 UDP socket: %d", agent->udp_sockets[1].fd);
#endif

  agent_clear_candidates(agent);
  return 0;
}

void agent_destroy(Agent* agent) {
  if (!agent) return;
  agent_close_turn(agent);
  utils_random_destroy(agent->random);
  agent->random = NULL;
  if (agent->udp_sockets[0].fd >= 0) {
    udp_socket_close(&agent->udp_sockets[0]);
    agent->udp_sockets[0].fd = -1;
  }

#if CONFIG_IPV6
  if (agent->udp_sockets[1].fd >= 0) {
    udp_socket_close(&agent->udp_sockets[1]);
    agent->udp_sockets[1].fd = -1;
  }
#endif
}

void agent_update_candidate_pairs(Agent* agent) {
  /* Trickle ICE appends candidates. Keep existing pair addresses, check
   * progress and the active nomination stable, including during DTLS. A new
   * remote description explicitly resets the checklist below. */
  for (int i = 0; i < agent->local_candidates_count; i++) {
    for (int j = 0; j < agent->remote_candidates_count; j++) {
      IceCandidatePair* pair;
      int existing;

      if (agent->local_candidates[i].addr.family !=
          agent->remote_candidates[j].addr.family ||
          agent->local_candidates[i].component !=
          agent->remote_candidates[j].component) {
        continue;
      }
      for (existing = 0; existing < agent->candidate_pairs_num; existing++) {
        pair = &agent->candidate_pairs[existing];
        if (pair->local == &agent->local_candidates[i] &&
            pair->remote == &agent->remote_candidates[j])
          break;
      }
      if (existing < agent->candidate_pairs_num) {
        continue;
      }
      if (agent->candidate_pairs_num >= AGENT_MAX_CANDIDATE_PAIRS) {
        LOGW("ICE candidate pair table full");
        return;
      }

      pair = &agent->candidate_pairs[agent->candidate_pairs_num];
      memset(pair, 0, sizeof(*pair));
      pair->local = &agent->local_candidates[i];
      pair->remote = &agent->remote_candidates[j];
      pair->priority = (uint64_t)pair->local->priority + pair->remote->priority;
      if (agent->regular_nomination) pair->priority = agent_pair_priority(agent, pair);
      pair->state = ICE_CANDIDATE_STATE_FROZEN;
      pair->conncheck = 0;
      agent->candidate_pairs_num++;
      if (agent_pair_is_relay(pair) && agent->turn)
        turn_prepare_peer(&agent->turn->client, &pair->remote->addr, ports_get_epoch_time());
    }
  }
}

static int agent_socket_recv(Agent* agent, Address* addr, uint8_t* buf, int len,
                             int timeout_ms) {
  int ret = -1;
  size_t i = 0;
  int maxfd = -1;
  fd_set rfds;
  struct timeval tv;
  int addr_type[] = { AF_INET,
#if CONFIG_IPV6
                      AF_INET6,
#endif
  };

  agent->received_relay = 0;
  if (agent->turn) {
    ret = agent_turn_receive(agent, addr, buf, (size_t)len, 0);
    if (ret > 0) { agent->received_relay = 1; return ret; }
  }
  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms % 1000) * 1000;
  FD_ZERO(&rfds);

  for (i = 0; i < sizeof(addr_type) / sizeof(addr_type[0]); i++) {
    if (agent->udp_sockets[i].fd > maxfd) {
      maxfd = agent->udp_sockets[i].fd;
    }
    if (agent->udp_sockets[i].fd >= 0) {
      FD_SET(agent->udp_sockets[i].fd, &rfds);
    }
  }

  if (agent->turn && agent->turn->socket.fd >= 0 && agent->turn->socket.fd < FD_SETSIZE) {
    FD_SET(agent->turn->socket.fd, &rfds);
    if (agent->turn->socket.fd > maxfd) maxfd = agent->turn->socket.fd;
  }
  ret = select(maxfd + 1, &rfds, NULL, NULL, &tv);
  if (ret < 0) {
    LOGE("select error");
  } else if (ret == 0) {
    // timeout
  } else {
    if (agent->turn && agent->turn->socket.fd >= 0 &&
        agent->turn->socket.fd < FD_SETSIZE && FD_ISSET(agent->turn->socket.fd, &rfds)) {
      ret = agent_turn_receive(agent, addr, buf, (size_t)len, 0);
      agent->received_relay = ret > 0;
      return ret;
    }
    for (i = 0; i < (int)(sizeof(addr_type) / sizeof(addr_type[0])); i++) {
      if (agent->udp_sockets[i].fd >= 0 && FD_ISSET(agent->udp_sockets[i].fd, &rfds)) {
        memset(buf, 0, len);
        ret = udp_socket_recvfrom(&agent->udp_sockets[i], addr, buf, len);
        break;
      }
    }
  }

  return ret;
}

static int agent_socket_recv_attempts(Agent* agent, Address* addr, uint8_t* buf, int len, int maxtimes) {
  int ret = -1;
  int i = 0;
  for (i = 0; i < maxtimes; i++) {
    if ((ret = agent_socket_recv(agent, addr, buf, len,
                                 AGENT_POLL_TIMEOUT)) != 0) {
      break;
    }
  }
  return ret;
}

static int agent_socket_send(Agent* agent, Address* addr, const uint8_t* buf, int len) {
  switch (addr->family) {
    case AF_INET6:
      return udp_socket_sendto(&agent->udp_sockets[1], addr, buf, len);
    case AF_INET:
    default:
      return udp_socket_sendto(&agent->udp_sockets[0], addr, buf, len);
  }
  return -1;
}

static int agent_create_host_addr(Agent* agent) {
  size_t i, j;
  const char* iface_prefx[] = {CONFIG_IFACE_PREFIX};
  IceCandidate* ice_candidate;
  int addr_type[] = { AF_INET,
#if CONFIG_IPV6
                      AF_INET6,
#endif
  };

  for (i = 0; i < sizeof(addr_type) / sizeof(addr_type[0]); i++) {
    for (j = 0; j < sizeof(iface_prefx) / sizeof(iface_prefx[0]); j++) {
      if (agent->local_candidates_count >= AGENT_MAX_CANDIDATES) {
        LOGW("ICE local candidate table full");
        return -1;
      }
      ice_candidate = agent->local_candidates + agent->local_candidates_count;
      // only copy port and family to addr of ice candidate
      ice_candidate_create(ice_candidate, agent->local_candidates_count, ICE_CANDIDATE_TYPE_HOST,
                           &agent->udp_sockets[i].bind_addr);
      // if resolve host addr, add to local candidate
      if (ports_get_host_addr(&ice_candidate->addr, iface_prefx[j])) {
        agent->local_candidates_count++;
      }
    }
  }

  return 0;
}

static int agent_create_stun_addr(Agent* agent, Address* serv_addr) {
  int ret = -1;
  Address bind_addr;
  StunMessage send_msg;
  StunMessage recv_msg;
  memset(&send_msg, 0, sizeof(send_msg));
  memset(&recv_msg, 0, sizeof(recv_msg));

  stun_msg_create(&send_msg, STUN_CLASS_REQUEST | STUN_METHOD_BINDING);

  ret = agent_socket_send(agent, serv_addr, send_msg.buf, send_msg.size);

  if (ret == -1) {
    LOGE("Failed to send STUN Binding Request.");
    return ret;
  }

  ret = agent_socket_recv_attempts(agent, NULL, recv_msg.buf, sizeof(recv_msg.buf), AGENT_STUN_RECV_MAXTIMES);
  if (ret <= 0) {
    LOGD("Failed to receive STUN Binding Response.");
    return ret;
  }

  recv_msg.size = (size_t)ret;
  if (stun_parse_msg_buf(&recv_msg) != 0)
    return -1;
  memcpy(&bind_addr, &recv_msg.mapped_addr, sizeof(Address));
  if (agent->local_candidates_count >= AGENT_MAX_CANDIDATES) {
    return -1;
  }
  IceCandidate* ice_candidate = agent->local_candidates + agent->local_candidates_count++;
  ice_candidate_create(ice_candidate, agent->local_candidates_count, ICE_CANDIDATE_TYPE_SRFLX, &bind_addr);
  return ret;
}

static int agent_create_turn_addr(Agent* agent, Address* server,
                                   const char* username, const char* credential) {
  AgentTurn* turn;
  const char* stage = "init";
  if (agent->turn || server->family != AF_INET ||
      agent->local_candidates_count >= AGENT_MAX_CANDIDATES) return -1;
  turn = calloc(1, sizeof(*turn));
  if (!turn) return -1;
  turn->socket.fd = -1;
  agent->turn = turn;
  errno = 0;
  if (turn_init(&turn->client, server, username, credential,
                agent_turn_send, agent_turn_random, agent)) goto failed;
  stage = "socket";
  if (udp_socket_open(&turn->socket, AF_INET, 0) < 0 ||
      turn->socket.fd >= FD_SETSIZE) goto failed;
  stage = "nonblock";
  if (fcntl(turn->socket.fd, F_SETFL, O_NONBLOCK) < 0) goto failed;
  stage = "allocate";
  if (turn_start(&turn->client, ports_get_epoch_time())) goto failed;
  /* The public local-SDP API is synchronous. Bound gathering independently
   * of application polling, including platforms with a stalled clock. */
  for (unsigned i = 0; i < 900 && turn->client.state == TURN_ALLOCATING; ++i)
    agent_turn_receive(agent, NULL, NULL, 0, 10);
  if (turn->client.state != TURN_READY) goto failed;
  IceCandidate* candidate = &agent->local_candidates[agent->local_candidates_count++];
  ice_candidate_create(candidate, agent->local_candidates_count,
                        ICE_CANDIDATE_TYPE_RELAY, &turn->client.relayed);
  candidate->raddr = turn->client.mapped;
  LOGI("TURN allocation ready fd=%d", turn->socket.fd);
  return 0;
failed:
  LOGE("TURN allocation failed stage=%s state=%d challenges=%u errno=%d",
       stage, (int)turn->client.state, (unsigned)turn->client.challenges, errno);
  agent_close_turn(agent);
  return -1;
}

static int agent_send_pair(Agent* agent, IceCandidatePair* pair,
                            const uint8_t* data, int size) {
  if (!pair || !pair->local || !pair->remote) return -1;
  if (agent_pair_is_relay(pair)) {
    if (!agent->turn) return -1;
    int ret = turn_send(&agent->turn->client, &pair->remote->addr, data,
                        (size_t)size, ports_get_epoch_time());
    return ret == TURN_AGAIN ? AGENT_SEND_AGAIN : ret;
  }
  return agent_socket_send(agent, &pair->remote->addr, data, size);
}

void agent_gather_candidate(Agent* agent, const char* urls, const char* username, const char* credential) {
  char* pos;
  int port;
  char hostname[64];
  char addr_string[ADDRSTRLEN];
  size_t i;
  int addr_type[1] = {AF_INET};  // ipv6 no need stun
  Address resolved_addr;
  memset(hostname, 0, sizeof(hostname));

  if (urls == NULL) {
    agent_create_host_addr(agent);
    return;
  }

  if (strncmp(urls, "stun:", 5) && strncmp(urls, "turn:", 5)) {
    LOGW("Unsupported ICE URL scheme");
    return;
  }
  pos = strchr(urls + 5, ':');
  if (!pos) { LOGW("ICE URL requires an explicit port"); return; }
  char* suffix;
  long parsed_port = strtol(pos + 1, &suffix, 10);
  if (parsed_port < 1 || parsed_port > 65535 || suffix == pos + 1 ||
      (*suffix && strcmp(suffix, "?transport=udp"))) {
    LOGW("Unsupported ICE URL port/transport");
    return;
  }
  port = (int)parsed_port;

  size_t hostname_len = (size_t)(pos - (urls + 5));
  if (hostname_len == 0 || hostname_len >= sizeof(hostname)) {
    LOGE("STUN/TURN hostname is too long");
    return;
  }
  memcpy(hostname, urls + 5, hostname_len);
  hostname[hostname_len] = '\0';

  for (i = 0; i < sizeof(addr_type) / sizeof(addr_type[0]); i++) {
    if (ports_resolve_addr(hostname, &resolved_addr) == 0) {
      addr_set_port(&resolved_addr, port);
      addr_to_string(&resolved_addr, addr_string, sizeof(addr_string));
      LOGI("Resolved stun/turn server %s:%d", addr_string, port);

      if (strncmp(urls, "stun:", 5) == 0) {
        LOGD("Create stun addr");
        agent_create_stun_addr(agent, &resolved_addr);
      } else if (strncmp(urls, "turn:", 5) == 0) {
        LOGD("Create turn addr");
        agent_create_turn_addr(agent, &resolved_addr, username, credential);
      }
    }
  }
}

void agent_get_local_description(Agent* agent, char* description, int length) {
  memset(description, 0, length);
  memset(agent->local_ufrag, 0, sizeof(agent->local_ufrag));
  memset(agent->local_upwd, 0, sizeof(agent->local_upwd));

  if (utils_random_get_string(agent->random, agent->local_ufrag, 4) != 0 ||
      utils_random_get_string(agent->random, agent->local_upwd, 24) != 0) {
    LOGE("Failed to generate ICE credentials");
    description[0] = '\0';
    return;
  }

  snprintf(description, length, "a=ice-ufrag:%s\r\na=ice-pwd:%s\r\n", agent->local_ufrag, agent->local_upwd);

  for (int i = 0; i < agent->local_candidates_count; i++) {
    ice_candidate_to_description(&agent->local_candidates[i], description + strlen(description), length - strlen(description));
  }

  // remove last \n
  description[strlen(description)] = '\0';
  LOGD("local description:\n%s", description);
}

int agent_send(Agent* agent, const uint8_t* buf, int len) {
  if (!agent || !buf || len <= 0 || !agent->nominated_pair ||
      !agent->nominated_pair->remote)
    return -1;
  if (!agent->consent_active ||
      agent_consent_poll_expiry(agent, ports_get_epoch_time()))
    return -1;
  return agent_send_pair(agent, agent->nominated_pair, buf, len);
}

static int agent_create_binding_response(Agent* agent, StunMessage* msg,
                                         Address* addr) {
  int size = 0;
  char username[584];
  char mapped_address[32];
  uint8_t mask[16];
  uint32_t cookie = htonl(MAGIC_COOKIE);
  StunHeader* header;
  stun_msg_create(msg, STUN_CLASS_RESPONSE | STUN_METHOD_BINDING);
  header = (StunHeader*)msg->buf;
  memcpy(header->transaction_id, agent->transaction_id, sizeof(header->transaction_id));
  snprintf(username, sizeof(username), "%s:%s", agent->local_ufrag, agent->remote_ufrag);
  memcpy(mask, &cookie, sizeof(cookie));
  memcpy(mask + 4, agent->transaction_id, sizeof(agent->transaction_id));
  size = stun_set_mapped_address(mapped_address, mask, addr);
  if (size < 0 ||
      stun_msg_write_attr(msg, STUN_ATTR_TYPE_XOR_MAPPED_ADDRESS,
                          (uint16_t)size, mapped_address) != 0 ||
      (!agent->regular_nomination && stun_msg_write_attr(msg, STUN_ATTR_TYPE_USERNAME,
                          strlen(username), username) != 0) ||
      stun_msg_finish(msg, STUN_CREDENTIAL_SHORT_TERM, agent->local_upwd,
                      strlen(agent->local_upwd)) != 0)
    return -1;
  return 0;
}

static int agent_create_binding_request(Agent* agent, IceCandidatePair* pair,
                                        StunMessage* msg, int consent) {
  uint8_t tie_breaker[8] = {0};
  uint32_t priority;
  unsigned char random[14];
  uint16_t jitter;
  if (!agent || !msg || !pair || !pair->local || !pair->remote) return -1;
  for (unsigned i = 0; i < sizeof(tie_breaker); ++i)
    tie_breaker[i] = (uint8_t)(agent->tie_breaker >> ((7U - i) * 8U));
  priority = htonl(pair->local->priority);
  if (agent->regular_nomination)
    priority = htonl((110U << 24) | (pair->local->priority & 0x00ffffffU));
  // send binding request
  stun_msg_create(msg, STUN_CLASS_REQUEST | STUN_METHOD_BINDING);
  if (!consent && pair->check_pending) {
    memcpy(((StunHeader*)msg->buf)->transaction_id, pair->check_id, 12);
  } else {
    if (utils_random_get_bytes(agent->random, random, sizeof(random)) != 0)
      return -1;
    memcpy(((StunHeader*)msg->buf)->transaction_id, random, 12);
    memcpy(&jitter, random + 12, sizeof(jitter));
    agent->consent_interval_ms = AGENT_CONSENT_MIN_INTERVAL_MS +
                                  jitter % AGENT_CONSENT_JITTER_MS;
  }
  char username[584];
  memset(username, 0, sizeof(username));
  snprintf(username, sizeof(username), "%s:%s", agent->remote_ufrag, agent->local_ufrag);
  if (stun_msg_write_attr(msg, STUN_ATTR_TYPE_USERNAME, strlen(username),
                          username) != 0)
    return -1;
  /* Keep PRIORITY and the ICE role for ICE-lite interoperability. Consent
   * refreshes must not renominate the established pair with USE-CANDIDATE. */
  if (stun_msg_write_attr(msg, STUN_ATTR_TYPE_PRIORITY, 4,
                          (char*)&priority) != 0)
    return -1;
  if (agent->mode == AGENT_MODE_CONTROLLING) {
    if ((!consent && (!agent->regular_nomination || pair->nominating) &&
         stun_msg_write_attr(msg, STUN_ATTR_TYPE_USE_CANDIDATE, 0, NULL) != 0) ||
        stun_msg_write_attr(msg, STUN_ATTR_TYPE_ICE_CONTROLLING, 8,
                            (char*)&tie_breaker) != 0)
      return -1;
  } else {
    if (stun_msg_write_attr(msg, STUN_ATTR_TYPE_ICE_CONTROLLED, 8,
                            (char*)&tie_breaker) != 0)
      return -1;
  }
  return stun_msg_finish(msg, STUN_CREDENTIAL_SHORT_TERM,
                         agent->remote_upwd, strlen(agent->remote_upwd));
}

static int agent_send_binding_request_to(Agent* agent, IceCandidatePair* pair,
                                          int consent) {
  StunMessage msg = {0};
  uint32_t now = ports_get_epoch_time();
  if (!agent || !pair) return -1;

  /* Pace local send/RNG failures too, instead of retrying on every tick. */
  if (consent) {
    agent->binding_sent_ms = now;
    if (!agent->consent_interval_ms) agent->consent_interval_ms = 5000;
  } else {
    agent->check_sent_ms = now;
    agent->check_sent = 1;
    pair->check_sent_ms = now;
    if (pair->conncheck++) pair->check_rto_ms *= 2;
  }
  if (agent_create_binding_request(agent, pair, &msg, consent) != 0 ||
      agent_send_pair(agent, pair, msg.buf, msg.size) != (int)msg.size)
    return -1;

  if (consent) {
    AgentStunTransaction* transaction =
        &agent->stun_transactions[agent->stun_transaction_next];
    memcpy(transaction->id, ((StunHeader*)msg.buf)->transaction_id, sizeof(transaction->id));
    transaction->sent_ms = now;
    transaction->pending = 1;
    agent->stun_transaction_next =
        (agent->stun_transaction_next + 1) % AGENT_STUN_TRANSACTIONS;
  } else {
    memcpy(pair->check_id, ((StunHeader*)msg.buf)->transaction_id, sizeof(pair->check_id));
    pair->check_pending = 1;
  }
  return 0;
}

static int agent_send_binding_request(Agent* agent, int consent) {
  return agent_send_binding_request_to(agent, agent ? agent->nominated_pair : NULL, consent);
}

int agent_consent_check(Agent* agent) {
  uint32_t now = ports_get_epoch_time();
  if (!agent || !agent->nominated_pair || !agent->selected_pair ||
      !agent->nominated_pair->local || !agent->nominated_pair->remote)
    return AGENT_CONSENT_INVALID;
  if (agent_pair_is_relay(agent->selected_pair) &&
      (!agent->turn || agent->turn->client.state != TURN_READY)) {
    agent->consent_expired = 1;
    return AGENT_CONSENT_LOST;
  }
  if (!agent->consent_active && !agent->consent_probing) {
    agent->consent_probing = 1;
    agent->consent_received_ms = now;
    agent->consent_interval_ms = 5000;
    agent->binding_sent_ms = now - agent->consent_interval_ms;
  }
  if (agent_consent_poll_expiry(agent, now)) return AGENT_CONSENT_LOST;
  if ((uint32_t)(now - agent->binding_sent_ms) >= agent->consent_interval_ms)
    agent_send_binding_request(agent, 1);
  return agent->consent_active ? AGENT_CONSENT_VALID : AGENT_CONSENT_PENDING;
}

static void agent_activate_pair(Agent* agent, IceCandidatePair* selected) {
  uint32_t now = ports_get_epoch_time();
  agent->selected_pair = agent->nominated_pair = selected;
  agent->nomination_pair = NULL;
  selected->state = ICE_CANDIDATE_STATE_SUCCEEDED;
  agent->binding_sent_ms = agent->consent_received_ms = now;
  agent->consent_active = 1;
  agent->consent_probing = 0;
  for (int i = 0; i < agent->candidate_pairs_num; ++i) {
    IceCandidatePair* pair = &agent->candidate_pairs[i];
    pair->check_pending = 0;
    pair->triggered = 0;
    if (pair != selected && pair->state == ICE_CANDIDATE_STATE_INPROGRESS)
      pair->state = ICE_CANDIDATE_STATE_FROZEN;
  }
  if (agent->regular_nomination) {
    char address[ADDRSTRLEN];
    addr_to_string(&selected->remote->addr, address, sizeof(address));
    LOGI("ICE nominated local_type=%d remote_type=%d remote=%s:%u",
         selected->local->type, selected->remote->type, address, (unsigned)selected->remote->addr.port);
  }
}

/* Only attributes preceding MESSAGE-INTEGRITY are authenticated. */
static const uint8_t* agent_stun_attribute(const StunMessage* msg, uint16_t wanted,
                                           uint16_t* length) {
  for (size_t at = sizeof(StunHeader); at + 4 <= msg->size;) {
    uint16_t type, size;
    memcpy(&type, msg->buf + at, 2); memcpy(&size, msg->buf + at + 2, 2);
    type = ntohs(type); size = ntohs(size);
    if ((size_t)size > msg->size - at - 4 || type == STUN_ATTR_TYPE_MESSAGE_INTEGRITY) return NULL;
    if (type == wanted) { *length = size; return msg->buf + at + 4; }
    at += 4 + ((size + 3U) & ~3U);
  }
  return NULL;
}

static void agent_change_role(Agent* agent, AgentMode mode) {
  agent->mode = mode;
  agent->nomination_pair = NULL;
  for (int i = 0; i < agent->candidate_pairs_num; ++i) {
    IceCandidatePair* pair = &agent->candidate_pairs[i];
    pair->priority = agent_pair_priority(agent, pair);
    pair->nominating = pair->remote_nominated = pair->triggered = 0;
    if (pair->state == ICE_CANDIDATE_STATE_INPROGRESS) {
      pair->check_pending = 0;
      pair->state = pair->validated ? ICE_CANDIDATE_STATE_SUCCEEDED : ICE_CANDIDATE_STATE_FROZEN;
    }
  }
}

static int agent_check_role(Agent* agent, StunMessage* incoming, StunMessage* error) {
  uint16_t length = 0;
  uint16_t same_role = agent->mode == AGENT_MODE_CONTROLLING ?
      STUN_ATTR_TYPE_ICE_CONTROLLING : STUN_ATTR_TYPE_ICE_CONTROLLED;
  const uint8_t* value = agent_stun_attribute(incoming, same_role, &length);
  if (!value) return 0;
  if (length != 8) return -1;
  uint64_t remote = 0;
  for (unsigned i = 0; i < 8; ++i) remote = (remote << 8) | value[i];
  if ((agent->mode == AGENT_MODE_CONTROLLING && remote > agent->tie_breaker) ||
      (agent->mode == AGENT_MODE_CONTROLLED && remote <= agent->tie_breaker)) {
    agent_change_role(agent, agent->mode == AGENT_MODE_CONTROLLING ? AGENT_MODE_CONTROLLED : AGENT_MODE_CONTROLLING);
    return 0;
  }
  char code[4] = {0, 0, 4, 87};
  stun_msg_create(error, STUN_CLASS_ERROR | STUN_METHOD_BINDING);
  memcpy(((StunHeader*)error->buf)->transaction_id, agent->transaction_id, sizeof(agent->transaction_id));
  if (stun_msg_write_attr(error, STUN_ATTR_TYPE_ERROR_CODE, sizeof(code), code) != 0 ||
      stun_msg_finish(error, STUN_CREDENTIAL_SHORT_TERM, agent->local_upwd, strlen(agent->local_upwd)) != 0) return -1;
  return 1;
}

static IceCandidatePair* agent_learn_remote(Agent* agent, StunMessage* msg, const Address* address) {
  uint16_t length = 0;
  const uint8_t* value = agent_stun_attribute(msg, STUN_ATTR_TYPE_PRIORITY, &length);
  if (!value || length != 4) return NULL;
  IceCandidate* local = NULL;
  for (int i = 0; i < agent->local_candidates_count; ++i) {
    IceCandidate* c = &agent->local_candidates[i];
    if (c->addr.family == address->family && c->component == 1 &&
        (c->type == ICE_CANDIDATE_TYPE_RELAY) == agent->received_relay &&
        (!local || c->priority > local->priority)) local = c;
  }
  if (!local) return NULL;
  int index;
  for (index = 0; index < agent->remote_candidates_count; ++index)
    if (agent->remote_candidates[index].component == 1 &&
        addr_equal(&agent->remote_candidates[index].addr, address)) break;
  if (index == agent->remote_candidates_count) {
    if (index == AGENT_MAX_CANDIDATES) return NULL;
    IceCandidate* candidate = &agent->remote_candidates[agent->remote_candidates_count++];
    memset(candidate, 0, sizeof(*candidate));
    memcpy(&candidate->priority, value, 4); candidate->priority = ntohl(candidate->priority);
    candidate->addr = *address; candidate->component = 1;
    candidate->type = ICE_CANDIDATE_TYPE_PRFLX;
    strcpy(candidate->transport, "UDP");
    snprintf(candidate->foundation, sizeof(candidate->foundation), "prflx%d", index);
    agent_update_candidate_pairs(agent);
  }
  for (int i = 0; i < agent->candidate_pairs_num; ++i) {
    IceCandidatePair* pair = &agent->candidate_pairs[i];
    if (pair->local == local && pair->remote == &agent->remote_candidates[index]) return pair;
  }
  return NULL;
}

void agent_process_stun_request(Agent* agent, StunMessage* stun_msg, Address* addr) {
  StunMessage msg;
  StunHeader* header;
  if (agent->relay_only && !agent->received_relay) return;
  switch (stun_msg->stunmethod) {
    case STUN_METHOD_BINDING:
      if (stun_msg_is_valid(stun_msg->buf, stun_msg->size, agent->local_upwd) == 0) {
        if (++agent->valid_stun_requests == 1) {
          LOGI("ICE first valid STUN request on fd=%d",
               agent->udp_sockets[0].fd);
        }
        header = (StunHeader*)stun_msg->buf;
        memcpy(agent->transaction_id, header->transaction_id, sizeof(header->transaction_id));
        int conflict = agent->regular_nomination && !agent->selected_pair ? agent_check_role(agent, stun_msg, &msg) : 0;
        if (conflict < 0) return;
        IceCandidatePair* incoming_pair = NULL;
        if (agent->regular_nomination && !agent->selected_pair && !conflict) {
          incoming_pair = agent_learn_remote(agent, stun_msg, addr);
          if (incoming_pair) {
            uint16_t length = 0;
            if (agent->mode == AGENT_MODE_CONTROLLED &&
                agent_stun_attribute(stun_msg, STUN_ATTR_TYPE_USE_CANDIDATE, &length) && length == 0)
              incoming_pair->remote_nominated = 1;
            /* Queue once, including cancellation of an ordinary in-flight
             * check. Do not repeatedly cancel a triggered transaction. */
            if (!incoming_pair->validated && !incoming_pair->triggered) {
              agent_start_pair(incoming_pair, ports_get_epoch_time());
              incoming_pair->triggered = 1;
            }
          }
        }
        if (conflict || agent_create_binding_response(agent, &msg, addr) == 0) {
          int sent = agent->received_relay && agent->turn ?
              turn_send(&agent->turn->client, addr, msg.buf, msg.size, ports_get_epoch_time()) :
              agent_socket_send(agent, addr, msg.buf, msg.size);
          if (sent != (int)msg.size) {
            if (++agent->failed_stun_responses == 1 ||
                (agent->failed_stun_responses % 64U) == 0)
              LOGW("ICE STUN response send failed (%u failures)",
                   agent->failed_stun_responses);
          }
          if (sent == (int)msg.size && incoming_pair && incoming_pair->validated &&
              incoming_pair->remote_nominated && !agent->selected_pair)
            agent_activate_pair(agent, incoming_pair);
        }
      } else if (++agent->invalid_stun_requests == 1) {
        LOGW("ICE first invalid STUN request on fd=%d",
             agent->udp_sockets[0].fd);
      }
      break;
    default:
      break;
  }
}

static void agent_process_stun_response(Agent* agent, StunMessage* stun_msg,
                                        Address* addr) {
  uint32_t now = ports_get_epoch_time();
  StunHeader* header = (StunHeader*)stun_msg->buf;
  IceCandidatePair* check_pair = NULL;
  AgentStunTransaction* consent = NULL;
  if (stun_msg->stunmethod != STUN_METHOD_BINDING) return;
  if (agent_consent_poll_expiry(agent, now)) goto invalid;

  /* Match a live transaction/source before doing HMAC work. A response on a
   * different outstanding pair may win; no unauthenticated pair switch. */
  if (!agent->selected_pair) {
    for (int i = 0; i < agent->candidate_pairs_num; ++i) {
      IceCandidatePair* pair = &agent->candidate_pairs[i];
      if (pair->state == ICE_CANDIDATE_STATE_INPROGRESS && pair->check_pending &&
          (uint32_t)(now - pair->check_started_ms) < AGENT_CHECK_TIMEOUT_MS &&
          agent_pair_is_relay(pair) == agent->received_relay &&
          addr_equal(addr, &pair->remote->addr) &&
          !memcmp(pair->check_id, header->transaction_id, sizeof(pair->check_id))) {
        check_pair = pair;
        break;
      }
    }
  }
  if (!check_pair && agent->nominated_pair &&
      agent_pair_is_relay(agent->nominated_pair) == agent->received_relay &&
      addr_equal(addr, &agent->nominated_pair->remote->addr)) {
    for (unsigned i = 0; i < AGENT_STUN_TRANSACTIONS; ++i) {
      AgentStunTransaction* transaction = &agent->stun_transactions[i];
      if (transaction->pending &&
          (uint32_t)(now - transaction->sent_ms) < AGENT_CONSENT_TIMEOUT_MS &&
          !memcmp(transaction->id, header->transaction_id, sizeof(transaction->id))) {
        consent = transaction;
        break;
      }
    }
  }
  if ((!check_pair && !consent) ||
      stun_msg_is_valid(stun_msg->buf, stun_msg->size, agent->remote_upwd) != 0)
    goto invalid;
  if (check_pair) check_pair->check_pending = check_pair->triggered = 0;
  if (consent) consent->pending = 0;
  if (stun_msg->stunclass == STUN_CLASS_ERROR) {
    if (check_pair && agent->regular_nomination && stun_msg->error_code == 487) {
      /* RFC 8445 7.2.5.1: only an authenticated 487 changes this session's
       * tie-breaker. Fail the checklist if its owner RNG cannot renew it. */
      uint64_t tie = agent->tie_breaker;
      int renewed = 0;
      for (unsigned i = 0; i < 2; ++i) {
        if (utils_random_get_bytes(agent->random, (unsigned char*)&tie, sizeof(tie)) != 0) break;
        if (tie != agent->tie_breaker) { renewed = 1; break; }
      }
      if (!renewed) {
        agent->nomination_pair = NULL;
        for (int i = 0; i < agent->candidate_pairs_num; ++i) {
          IceCandidatePair* pair = &agent->candidate_pairs[i];
          pair->state = ICE_CANDIDATE_STATE_FAILED;
          pair->check_pending = pair->triggered = pair->nominating = 0;
        }
        LOGE("ICE role conflict: unable to renew tie-breaker");
        return;
      }
      agent->tie_breaker = tie;
      agent_change_role(agent, agent->mode == AGENT_MODE_CONTROLLING ? AGENT_MODE_CONTROLLED : AGENT_MODE_CONTROLLING);
      agent_start_pair(check_pair, now);
      check_pair->triggered = 1;
      return;
    }
    if (check_pair) {
      check_pair->state = ICE_CANDIDATE_STATE_FAILED;
      if (agent->regular_nomination && agent->nomination_pair == check_pair) {
        agent->nomination_pair = NULL;
        check_pair->nominating = check_pair->validated = 0;
      }
    }
    if (consent && stun_msg->error_code == 403) {
      agent->consent_expired = 1;
      agent_consent_poll_expiry(agent, now);
    }
    return;
  }
  if (++agent->valid_stun_responses == 1)
    LOGI("ICE first valid STUN response on fd=%d", agent->udp_sockets[0].fd);
  if (check_pair) {
    check_pair->state = ICE_CANDIDATE_STATE_SUCCEEDED;
    check_pair->validated = 1;
    if (!agent->regular_nomination || check_pair->nominating ||
        (agent->mode == AGENT_MODE_CONTROLLED && check_pair->remote_nominated))
      agent_activate_pair(agent, check_pair);
  }
  agent->consent_active = 1;
  agent->consent_probing = 0;
  agent->consent_received_ms = now;
  return;
invalid:
  if (++agent->invalid_stun_responses == 1)
    LOGW("ICE first unmatched or invalid STUN response on fd=%d", agent->udp_sockets[0].fd);
}

static int agent_recv_with_timeout(Agent* agent, uint8_t* buf, int len,
                                   int timeout_ms) {
  int ret = -1;
  StunMessage stun_msg = {0};
  Address addr;

  if (!agent || !buf || len <= 0)
    return -1;

  ret = agent_socket_recv(agent, &addr, buf, len, timeout_ms);
  if (ret <= 0)
    return ret;

  agent->received_packets++;
  if (agent->received_packets <= 3) {
    char source[ADDRSTRLEN];
    addr_to_string(&addr, source, sizeof(source));
    LOGI("ICE RX packet %u fd=%d len=%d from=%s:%u first=%02x%02x",
         agent->received_packets, agent->udp_sockets[0].fd, ret, source,
         (unsigned)addr.port, (unsigned)buf[0],
         (unsigned)(ret > 1 ? buf[1] : 0));
  }

  if (stun_probe(buf, (size_t)ret) == 0) {
    if ((size_t)ret > sizeof(stun_msg.buf)) {
      LOGW("Discarding oversized STUN message (%d bytes)", ret);
      return 0;
    }
    memcpy(stun_msg.buf, buf, ret);
    stun_msg.size = (size_t)ret;
    if (stun_parse_msg_buf(&stun_msg) != 0) {
      if (++agent->stun_parse_failures == 1)
        LOGW("ICE first STUN parse failure len=%d fd=%d", ret,
             agent->udp_sockets[0].fd);
      return 0;
    }
    switch (stun_msg.stunclass) {
      case STUN_CLASS_REQUEST:
        agent_process_stun_request(agent, &stun_msg, &addr);
        break;
      case STUN_CLASS_RESPONSE:
      case STUN_CLASS_ERROR:
        agent_process_stun_response(agent, &stun_msg, &addr);
        break;
      default:
        break;
    }
    ret = 0;
  } else if (!agent->selected_pair ||
             agent_pair_is_relay(agent->selected_pair) != agent->received_relay ||
             !addr_equal(&addr, &agent->selected_pair->remote->addr)) {
    return 0;
  } else if (++agent->nonstun_packets == 1) {
    LOGI("ICE first non-STUN packet len=%d fd=%d", ret,
         agent->udp_sockets[0].fd);
  }
  return ret;
}

int agent_recv(Agent* agent, uint8_t* buf, int len) {
  return agent_recv_with_timeout(agent, buf, len, AGENT_POLL_TIMEOUT);
}

int agent_recv_nonblocking(Agent* agent, uint8_t* buf, int len) {
  return agent_recv_with_timeout(agent, buf, len, 0);
}

void agent_set_remote_description(Agent* agent, char* description) {
  /*
  a=ice-ufrag:Iexb
  a=ice-pwd:IexbSoY7JulyMbjKwISsG9
  a=candidate:1 1 UDP 1 36.231.28.50 38143 typ srflx
  */
  int i;

  if (!description) {
    return;
  }

  LOGD("Set remote description:\n%s", description);

  char* line_start = description;
  char* line_end = NULL;

  /* Candidates may be trickled before the initial remote SDP. Merge those
   * into its checklist; an existing description instead starts a new set.
   * agent_clear_candidates() also clears credentials for a fresh session. */
  if (agent->remote_ufrag[0] || agent->remote_upwd[0])
    agent->remote_candidates_count = 0;
  memset(agent->remote_ufrag, 0, sizeof(agent->remote_ufrag));
  memset(agent->remote_upwd, 0, sizeof(agent->remote_upwd));
  agent->candidate_pairs_num = 0;
  agent->selected_pair = NULL;
  agent->nominated_pair = NULL;
  agent_reset_consent(agent);

  while (*line_start) {
    /* Find next line ending - support both \r\n and \n */
    char* next = strchr(line_start, '\n');
    line_end = next ? next : line_start + strlen(line_start);
    if (line_end > line_start && line_end[-1] == '\r') --line_end;

    if (strncmp(line_start, "a=ice-ufrag:", strlen("a=ice-ufrag:")) == 0) {
      size_t value_len = (size_t)(line_end - line_start - strlen("a=ice-ufrag:"));
      if (value_len >= sizeof(agent->remote_ufrag)) {
        value_len = sizeof(agent->remote_ufrag) - 1;
      }
      memcpy(agent->remote_ufrag, line_start + strlen("a=ice-ufrag:"), value_len);
      agent->remote_ufrag[value_len] = '\0';

    } else if (strncmp(line_start, "a=ice-pwd:", strlen("a=ice-pwd:")) == 0) {
      size_t value_len = (size_t)(line_end - line_start - strlen("a=ice-pwd:"));
      if (value_len >= sizeof(agent->remote_upwd)) {
        value_len = sizeof(agent->remote_upwd) - 1;
      }
      memcpy(agent->remote_upwd, line_start + strlen("a=ice-pwd:"), value_len);
      agent->remote_upwd[value_len] = '\0';

    } else if (strncmp(line_start, "a=candidate:", strlen("a=candidate:")) == 0) {
      IceCandidate candidate;

      if (ice_candidate_from_description(&candidate, line_start, line_end) == 0) {
        for (i = 0; i < agent->remote_candidates_count; i++) {
          if (agent->remote_candidates[i].component == candidate.component &&
              addr_equal(&agent->remote_candidates[i].addr, &candidate.addr)) {
            break;
          }
        }
        if (i == agent->remote_candidates_count &&
            agent->remote_candidates_count < AGENT_MAX_CANDIDATES) {
          agent->remote_candidates[agent->remote_candidates_count++] = candidate;
        }
      }
    }

    line_start = next ? next + 1 : line_start + strlen(line_start);
  }

  LOGD("remote ufrag: %s", agent->remote_ufrag);
  LOGD("remote upwd: %s", agent->remote_upwd);
  LOGI("parsed %d remote candidates", agent->remote_candidates_count);

  agent_update_candidate_pairs(agent);
  LOGD("candidate pairs num: %d", agent->candidate_pairs_num);
}

static void agent_start_pair(IceCandidatePair* pair, uint32_t now) {
  pair->conncheck = 0;
  pair->check_pending = 0;
  pair->triggered = 0;
  pair->check_started_ms = now;
  pair->check_rto_ms = AGENT_CHECK_RTO_MS;
  pair->state = ICE_CANDIDATE_STATE_INPROGRESS;
}

static int agent_pair_is_direct(const IceCandidatePair* pair) {
  return pair->local->type != ICE_CANDIDATE_TYPE_RELAY &&
         pair->remote->type != ICE_CANDIDATE_TYPE_RELAY;
}

static void agent_regular_nominate(Agent* agent, uint32_t now) {
  if (agent->mode != AGENT_MODE_CONTROLLING || agent->selected_pair || agent->nomination_pair) return;
  IceCandidatePair* best = NULL;
  for (int i = 0; i < agent->candidate_pairs_num; ++i) {
    IceCandidatePair* pair = &agent->candidate_pairs[i];
    if (pair->state != ICE_CANDIDATE_STATE_SUCCEEDED || !pair->validated) continue;
    if (!best || agent_pair_is_direct(pair) > agent_pair_is_direct(best) ||
        (agent_pair_is_direct(pair) == agent_pair_is_direct(best) && pair->priority > best->priority)) best = pair;
  }
  if (!best) return;
  if ((uint32_t)(now - agent->checks_started_ms) < agent->direct_preference_ms) {
    if (!agent_pair_is_direct(best)) return;
    for (int i = 0; i < agent->candidate_pairs_num; ++i) {
      IceCandidatePair* pair = &agent->candidate_pairs[i];
      if (agent_pair_is_direct(pair) && pair->priority > best->priority &&
          pair->state != ICE_CANDIDATE_STATE_SUCCEEDED && pair->state != ICE_CANDIDATE_STATE_FAILED) return;
    }
  }
  best->nominating = 1;
  agent->nomination_pair = best;
  agent_start_pair(best, now); /* New transaction: only this check carries USE-CANDIDATE. */
}

static int agent_regular_check(Agent* agent) {
  uint32_t now = ports_get_epoch_time();
  uint8_t buf[1400];
  if (!agent->checks_started) { agent->checks_started = 1; agent->checks_started_ms = now; }
  agent_regular_nominate(agent, now);
  if (!agent->check_sent || (uint32_t)(now - agent->check_sent_ms) >= AGENT_CHECK_PACING_MS) {
    IceCandidatePair* next = NULL;
    for (int i = 0; i < agent->candidate_pairs_num; ++i) {
      IceCandidatePair* pair = &agent->candidate_pairs[i];
      int fresh = pair->state == ICE_CANDIDATE_STATE_FROZEN ||
          (pair->state == ICE_CANDIDATE_STATE_INPROGRESS && !pair->conncheck);
      if (fresh && (!next || (pair->triggered == 1) > (next->triggered == 1) ||
          ((pair->triggered == 1) == (next->triggered == 1) &&
           (pair->nominating > next->nominating ||
            (pair->nominating == next->nominating && pair->priority > next->priority))))) next = pair;
    }
    if (!next) {
      for (int i = 0; i < agent->candidate_pairs_num; ++i) {
        IceCandidatePair* pair = &agent->candidate_pairs[i];
        if (pair->state == ICE_CANDIDATE_STATE_INPROGRESS && pair->conncheck < AGENT_CHECK_TRANSMISSIONS &&
            (uint32_t)(now - pair->check_sent_ms) >= pair->check_rto_ms &&
            (!next || pair->nominating > next->nominating ||
             (pair->nominating == next->nominating && pair->priority > next->priority))) next = pair;
      }
    }
    if (next) {
      if (next->state == ICE_CANDIDATE_STATE_FROZEN) agent_start_pair(next, now);
      if (next->triggered == 1) next->triggered = 2;
      agent_send_binding_request_to(agent, next, 0);
    }
  }
  agent_recv(agent, buf, sizeof(buf));
  agent_regular_nominate(agent, ports_get_epoch_time());
  return agent->selected_pair ? 0 : -1;
}

int agent_connectivity_check(Agent* agent) {
  uint8_t buf[1400];
  if (!agent) return -1;
  if (agent->selected_pair) return 0;
  if (agent->regular_nomination) return agent_regular_check(agent);
  uint32_t now = ports_get_epoch_time();
  if (!agent->check_sent ||
      (uint32_t)(now - agent->check_sent_ms) >= AGENT_CHECK_PACING_MS) {
    IceCandidatePair* next = NULL;
    /* Give every compatible pair an initial check before waiting through an
     * unreachable pair's retransmission lifetime. Tables and send rate remain
     * bounded; transactions are not evicted to admit later candidates. */
    for (int i = 0; i < agent->candidate_pairs_num; ++i) {
      IceCandidatePair* pair = &agent->candidate_pairs[i];
      if (pair->state == ICE_CANDIDATE_STATE_FROZEN) {
        agent_start_pair(pair, now);
        next = pair;
        break;
      }
      if (pair->state == ICE_CANDIDATE_STATE_INPROGRESS && !pair->conncheck) {
        next = pair;
        break;
      }
    }
    if (!next) {
      for (int i = 0; i < agent->candidate_pairs_num; ++i) {
        IceCandidatePair* pair = &agent->candidate_pairs[i];
        if (pair->state == ICE_CANDIDATE_STATE_INPROGRESS &&
            pair->conncheck < AGENT_CHECK_TRANSMISSIONS &&
            (uint32_t)(now - pair->check_started_ms) < AGENT_CHECK_TIMEOUT_MS &&
            (uint32_t)(now - pair->check_sent_ms) >= pair->check_rto_ms) {
          next = pair;
          break;
        }
      }
    }
    if (next) agent_send_binding_request_to(agent, next, 0);
  }
  agent_recv(agent, buf, sizeof(buf));
  return agent->selected_pair ? 0 : -1;
}

int agent_select_candidate_pair(Agent* agent) {
  if (!agent) return -1;
  if (agent->selected_pair) return 0;
  uint32_t now = ports_get_epoch_time();
  IceCandidatePair* active = NULL;
  IceCandidatePair* frozen = NULL;
  for (int i = 0; i < agent->candidate_pairs_num; ++i) {
    IceCandidatePair* pair = &agent->candidate_pairs[i];
    if (pair->state == ICE_CANDIDATE_STATE_INPROGRESS &&
        (uint32_t)(now - pair->check_started_ms) >= AGENT_CHECK_TIMEOUT_MS) {
      pair->state = ICE_CANDIDATE_STATE_FAILED;
      pair->check_pending = 0;
      pair->triggered = 0;
      if (agent->regular_nomination && pair == agent->nomination_pair) {
        agent->nomination_pair = NULL;
        pair->nominating = pair->validated = 0;
      }
    }
    if (!active && pair->state == ICE_CANDIDATE_STATE_INPROGRESS) active = pair;
    if (!frozen && pair->state == ICE_CANDIDATE_STATE_FROZEN) frozen = pair;
    if (agent->regular_nomination && !active && pair->state == ICE_CANDIDATE_STATE_SUCCEEDED) active = pair;
  }
  if (!active && frozen) {
    agent_start_pair(frozen, now);
    active = frozen;
  }
  if (!active) return -1;
  if (!agent->regular_nomination) agent->nominated_pair = active;
  return 0;
}
