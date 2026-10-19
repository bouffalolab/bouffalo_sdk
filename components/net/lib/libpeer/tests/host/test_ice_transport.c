/* Production PeerConnection + ICE/STUN over real localhost UDP sockets.
 * Only time, interface discovery and DNS are controlled by this test. Crypto,
 * candidate parsing, authentication and packet I/O are not mocked. */
#include "peer_connection.c"
#include "agent.c"
#include "../../../webrtc_peer/src/webrtc_peer.c"
#include <assert.h>

static uint32_t now_ms;
static unsigned disconnected_events;

uint32_t ports_get_epoch_time(void) { return now_ms; }
void ports_sleep_ms(int ms) { (void)ms; }
int ports_get_host_addr(Address* addr, const char* prefix) {
  (void)prefix;
  return addr_from_string("127.0.0.1", addr);
}
int ports_resolve_addr(const char* name, Address* addr) {
  return addr_from_string(name, addr) ? 0 : -1;
}
int mdns_resolve_addr(const char* name, Address* addr) {
  (void)name; (void)addr;
  abort(); /* All test addresses are numeric. */
}

static void receive_stun(Agent* agent) {
  uint8_t packet[CONFIG_MTU];
  uint32_t previous = agent->received_packets;
  for (unsigned i = 0; i < 1000 && agent->received_packets == previous; i++)
    assert(agent_recv(agent, packet, sizeof(packet)) == 0);
  assert(agent->received_packets == previous + 1);
}

static Address loopback_address(Agent* agent) {
  Address addr = agent->udp_sockets[0].bind_addr;
  assert(addr_from_string("127.0.0.1", &addr));
  return addr;
}

static void configure_agent(Agent* agent, const char* name,
                            const char* password, Agent* remote) {
  snprintf(agent->local_ufrag, sizeof(agent->local_ufrag), "%s", name);
  snprintf(agent->local_upwd, sizeof(agent->local_upwd), "%s", password);
  agent->local_candidates_count = agent->remote_candidates_count = 1;
  agent->local_candidates[0].component = 1;
  agent->remote_candidates[0].component = 1;
  agent->local_candidates[0].addr = loopback_address(agent);
  agent->remote_candidates[0].addr = loopback_address(remote);
  agent_update_candidate_pairs(agent);
  assert(agent_select_candidate_pair(agent) == 0);
}

static void start_agents(Agent* a, Agent* b) {
  assert(agent_create(a) == 0 && agent_create(b) == 0);
  configure_agent(a, "local", "local-password", b);
  configure_agent(b, "remote", "remote-password", a);
  strcpy(a->remote_ufrag, b->local_ufrag);
  strcpy(a->remote_upwd, b->local_upwd);
  strcpy(b->remote_ufrag, a->local_ufrag);
  strcpy(b->remote_upwd, a->local_upwd);
  Agent* pair[] = {a, b};
  for (unsigned i = 0; i < 2; i++) {
    Agent* sender = pair[i];
    Agent* receiver = pair[1 - i];
    assert(agent_send_binding_request(sender, 0) == 0);
    receive_stun(receiver);
    receive_stun(sender);
    assert(sender->nominated_pair->state == ICE_CANDIDATE_STATE_SUCCEEDED);
    sender->selected_pair = sender->nominated_pair;
    assert(sender->consent_active && sender->consent_received_ms == now_ms);
  }
}

static StunMessage response_for(Agent* agent, unsigned slot,
                                 const char* password, unsigned error) {
  StunMessage msg = {0};
  stun_msg_create(&msg, (error ? STUN_CLASS_ERROR : STUN_CLASS_RESPONSE) |
                         STUN_METHOD_BINDING);
  memcpy(((StunHeader*)msg.buf)->transaction_id,
         agent->stun_transactions[slot].id, 12);
  if (error) {
    char code[] = {0, 0, error / 100, error % 100};
    assert(stun_msg_write_attr(&msg, STUN_ATTR_TYPE_ERROR_CODE,
                               sizeof(code), code) == 0);
  }
  assert(stun_msg_finish(&msg, STUN_CREDENTIAL_SHORT_TERM, password,
                         strlen(password)) == 0);
  return msg;
}

static void send_response(Agent* from, Agent* to, StunMessage* msg) {
  Address addr = loopback_address(to);
  assert(agent_socket_send(from, &addr, msg->buf, msg->size) == (int)msg->size);
  receive_stun(to);
}

static void test_trickle(void) {
  const PeerConnectionState states[] = {PEER_CONNECTION_CHECKING,
      PEER_CONNECTION_CONNECTED, PEER_CONNECTION_COMPLETED};
  for (unsigned s = 0; s < sizeof(states) / sizeof(states[0]); s++) {
    PeerConnection pc = {0};
    Agent sink = {0};
    assert(agent_create(&pc.agent) == 0 && agent_create(&sink) == 0);
    pc.state = states[s];
    pc.agent.local_candidates_count = 1;
    pc.agent.local_candidates[0].addr = loopback_address(&pc.agent);
    pc.agent.local_candidates[0].component = 1;
    char first[128];
    snprintf(first, sizeof(first), "candidate:first 1 UDP 100 127.0.0.1 %u typ host",
             (unsigned)sink.udp_sockets[0].bind_addr.port);
    assert(peer_connection_add_ice_candidate(&pc, first) == 0);
    assert(agent_select_candidate_pair(&pc.agent) == 0);
    IceCandidatePair* active = pc.agent.nominated_pair;
    active->conncheck = 17;
    if (states[s] != PEER_CONNECTION_CHECKING) {
      active->state = ICE_CANDIDATE_STATE_SUCCEEDED;
      pc.agent.selected_pair = active;
    }
    IceCandidatePair saved = *active;
    assert(peer_connection_add_ice_candidate(&pc,
        "candidate:late 1 UDP 99 192.0.2.2 9000 typ host") == 0);
    assert(pc.state == states[s] && pc.agent.nominated_pair == active);
    assert(!memcmp(active, &saved, sizeof(saved)));
    if (states[s] != PEER_CONNECTION_CHECKING)
      assert(pc.agent.selected_pair == active);
    assert(pc.agent.candidate_pairs_num == 2);
    assert(peer_connection_add_ice_candidate(&pc, first) == 0);
    assert(pc.agent.candidate_pairs_num == 2 && pc.agent.remote_candidates_count == 2);
    assert(peer_connection_add_ice_candidate(&pc, "malformed") == -1);
    assert(pc.agent.candidate_pairs_num == 2);
    /* Model the authenticated check that precedes DTLS/application sends. */
    pc.agent.consent_active = 1;
    pc.agent.consent_received_ms = now_ms;
    /* Real sends must still use the original nominated socket destination. */
    assert(agent_send(&pc.agent, (const uint8_t*)"live", 4) == 4);
    uint8_t packet[16];
    assert(agent_socket_recv_attempts(&sink, NULL, packet, sizeof(packet), 1000) == 4);
    assert(!memcmp(packet, "live", 4));

    /* Fill both bounded candidate tables and verify idempotent updates. */
    for (int i = 1; i < AGENT_MAX_CANDIDATES; i++) {
      pc.agent.local_candidates[i] = pc.agent.local_candidates[0];
      addr_set_port(&pc.agent.local_candidates[i].addr, 10000 + i);
    }
    pc.agent.local_candidates_count = AGENT_MAX_CANDIDATES;
    while (pc.agent.remote_candidates_count < AGENT_MAX_CANDIDATES) {
      char candidate[128];
      snprintf(candidate, sizeof(candidate), "candidate:more 1 UDP 98 192.0.2.3 %u typ host",
               10000U + pc.agent.remote_candidates_count);
      assert(peer_connection_add_ice_candidate(&pc, candidate) == 0);
    }
    assert(pc.agent.candidate_pairs_num == AGENT_MAX_CANDIDATE_PAIRS);
    assert(peer_connection_add_ice_candidate(&pc, first) == 0);
    assert(peer_connection_add_ice_candidate(&pc,
        "candidate:overflow 1 UDP 97 192.0.2.4 9001 typ host") == -1);
    agent_update_candidate_pairs(&pc.agent);
    assert(pc.agent.candidate_pairs_num == AGENT_MAX_CANDIDATE_PAIRS);
    assert(!memcmp(active, &saved, sizeof(saved)));
    pc.agent.consent_active = 1;
    pc.agent.stun_transactions[0].pending = 1;
    strcpy(pc.agent.remote_ufrag, "previous");
    strcpy(pc.agent.remote_upwd, "previous-password");
    agent_set_remote_description(&pc.agent,
        "a=ice-ufrag:new\r\na=ice-pwd:fresh-password\r\n"
        "a=candidate:restart 1 UDP 100 192.0.2.5 9002 typ host\r\n");
    assert(!pc.agent.selected_pair && !pc.agent.nominated_pair);
    assert(!pc.agent.consent_active && !pc.agent.stun_transactions[0].pending);
    assert(pc.agent.candidate_pairs_num == AGENT_MAX_CANDIDATES);
    assert(agent_send(&pc.agent, packet, 4) == -1);
    pc.state = PEER_CONNECTION_CLOSED;
    assert(peer_connection_add_ice_candidate(&pc, first) == -1);
    agent_destroy(&pc.agent);
    agent_destroy(&sink);
  }
  /* Incompatible components and address families must never be paired. */
  Agent agent = {0};
  agent.local_candidates_count = agent.remote_candidates_count = 1;
  agent.local_candidates[0].addr.family = AF_INET;
  agent.local_candidates[0].component = 1;
  agent.remote_candidates[0].addr.family = AF_INET6;
  agent.remote_candidates[0].component = 1;
  agent_update_candidate_pairs(&agent);
  assert(agent.candidate_pairs_num == 0);
  agent.remote_candidates[0].addr.family = AF_INET;
  agent.remote_candidates[0].component = 2;
  agent_update_candidate_pairs(&agent);
  assert(agent.candidate_pairs_num == 0);
  puts("ICE late/duplicate/full/malformed candidates and checklist reset passed");
}

static void without_candidates(const char* sdp, char* output, size_t capacity) {
  size_t used = 0;
  while (*sdp) {
    const char* end = strstr(sdp, "\r\n");
    size_t length = end ? (size_t)(end - sdp) + 2 : strlen(sdp);
    if (strncmp(sdp, "a=candidate:", 12)) {
      assert(used + length < capacity);
      memcpy(output + used, sdp, length);
      used += length;
    }
    sdp += length;
  }
  output[used] = '\0';
}

static void test_pending_candidate_bounds_and_reset(void) {
  PeerConnection pc = {.state = PEER_CONNECTION_NEW};
  assert(agent_create(&pc.agent) == 0);
  pc.agent.local_candidates_count = 1;
  pc.agent.local_candidates[0].component = 1;
  pc.agent.local_candidates[0].addr = loopback_address(&pc.agent);
  for (unsigned i = 0; i < AGENT_MAX_CANDIDATES; ++i) {
    char candidate[128];
    snprintf(candidate, sizeof(candidate), "candidate:early 1 UDP 100 192.0.2.1 %u typ host", 9000 + i);
    assert(peer_connection_add_ice_candidate(&pc, candidate) == 0);
  }
  agent_set_remote_description(&pc.agent,
      "a=ice-ufrag:initial\r\na=ice-pwd:initial-password\r\n"
      "a=candidate:duplicate 1 UDP 100 192.0.2.1 9000 typ host\r\n"
      "a=candidate:overflow 1 UDP 100 192.0.2.2 9000 typ host\r\n");
  assert(pc.agent.remote_candidates_count == AGENT_MAX_CANDIDATES);
  assert(pc.agent.candidate_pairs_num == AGENT_MAX_CANDIDATES);
  agent_set_remote_description(&pc.agent,
      "a=ice-ufrag:replacement\r\na=ice-pwd:replacement-password\r\n"
      "a=candidate:replacement 1 UDP 100 192.0.2.3 9000 typ host\r\n");
  assert(pc.agent.remote_candidates_count == 1 && pc.agent.candidate_pairs_num == 1);
  assert(!strcmp(pc.agent.remote_candidates[0].foundation, "replacement"));
  agent_clear_candidates(&pc.agent);
  assert(!pc.agent.remote_ufrag[0] && !pc.agent.remote_upwd[0]);
  assert(peer_connection_add_ice_candidate(&pc,
      "candidate:fresh 1 UDP 100 192.0.2.4 9000 typ host") == 0);
  agent_set_remote_description(&pc.agent,
      "a=ice-ufrag:fresh\r\na=ice-pwd:fresh-password\r\n");
  assert(pc.agent.remote_candidates_count == 1);
  assert(!strcmp(pc.agent.remote_candidates[0].foundation, "fresh"));
  agent_destroy(&pc.agent);
  puts("ICE pending candidate deduplication, bounded merge and fresh-session reset passed");
}

static void test_trickle_signaling_order(void) {
  /* Exercise both roles through the facade: candidates before SDP, candidates
   * only after candidate-free SDP, and duplicates shared by SDP and trickle. */
  for (unsigned mode = 0; mode < 3; ++mode) {
    now_ms = 1000;
    PeerConfiguration config = {.datachannel = DATA_CHANNEL_BINARY};
    PeerConnection* a = peer_connection_create(&config);
    PeerConnection* b = peer_connection_create(&config);
    assert(a && b && peer_connection_create_offer(a));
    webrtc_peer_t offerer = {.connection = a, .role = WEBRTC_PEER_ROLE_OFFERER};
    webrtc_peer_t answerer = {.connection = b, .role = WEBRTC_PEER_ROLE_ANSWERER};
    char offer[CONFIG_SDP_BUFFER_SIZE], answer[CONFIG_SDP_BUFFER_SIZE];
    char from_a[512], from_b[512];
    ice_candidate_to_description(&a->agent.local_candidates[0], from_a, sizeof(from_a));
    webrtc_peer_message_t candidate_a = {WEBRTC_PEER_MSG_ICE_CANDIDATE, from_a};
    if (mode != 1) {
      assert(webrtc_peer_send_msg(&answerer, &candidate_a) == WEBRTC_PEER_OK);
      assert(b->agent.remote_candidates_count == 1);
    }
    without_candidates(a->sdp, offer, sizeof(offer));
    webrtc_peer_message_t remote_offer = {
        WEBRTC_PEER_MSG_SDP_OFFER, mode == 2 ? a->sdp : offer};
    assert(webrtc_peer_send_msg(&answerer, &remote_offer) == WEBRTC_PEER_OK);
    assert(b->state == PEER_CONNECTION_CHECKING);
    assert(b->agent.remote_candidates_count == (mode == 1 ? 0 : 1));
    ice_candidate_to_description(&b->agent.local_candidates[0], from_b, sizeof(from_b));
    webrtc_peer_message_t candidate_b = {WEBRTC_PEER_MSG_ICE_CANDIDATE, from_b};
    if (mode != 1) {
      assert(webrtc_peer_send_msg(&offerer, &candidate_b) == WEBRTC_PEER_OK);
      assert(a->agent.remote_candidates_count == 1);
    }
    without_candidates(b->sdp, answer, sizeof(answer));
    webrtc_peer_message_t remote_answer = {
        WEBRTC_PEER_MSG_SDP_ANSWER, mode == 2 ? b->sdp : answer};
    assert(webrtc_peer_send_msg(&offerer, &remote_answer) == WEBRTC_PEER_OK);
    assert(a->state == PEER_CONNECTION_CHECKING);
    assert(a->agent.remote_candidates_count == (mode == 1 ? 0 : 1));
    if (mode == 1) {
      for (unsigned i = 0; i < 5; ++i) {
        assert(peer_connection_loop(a) == 0 && peer_connection_loop(b) == 0);
        assert(a->state == PEER_CONNECTION_CHECKING && b->state == PEER_CONNECTION_CHECKING);
        now_ms += 10;
      }
    }
    assert(webrtc_peer_send_msg(&answerer, &candidate_a) == WEBRTC_PEER_OK);
    assert(webrtc_peer_send_msg(&offerer, &candidate_b) == WEBRTC_PEER_OK);
    assert(a->agent.remote_candidates_count == 1 && b->agent.remote_candidates_count == 1);
    assert(a->agent.candidate_pairs_num == 1 && b->agent.candidate_pairs_num == 1);
    for (unsigned i = 0; i < 200; ++i) {
      if (a->state == PEER_CONNECTION_CHECKING) assert(peer_connection_loop(a) == 0);
      if (b->state == PEER_CONNECTION_CHECKING) assert(peer_connection_loop(b) == 0);
      if (a->state == PEER_CONNECTION_CONNECTED && b->state == PEER_CONNECTION_CONNECTED) break;
      now_ms += 10;
    }
    assert(a->state == PEER_CONNECTION_CONNECTED && b->state == PEER_CONNECTION_CONNECTED);
    assert(a->agent.consent_active && b->agent.consent_active);
    peer_connection_destroy(a);
    peer_connection_destroy(b);
  }
  puts("ICE candidate-before-SDP, SDP-before-candidate and duplicate signaling reach authenticated connectivity in both roles");
}

static void on_state(PeerConnectionState state, void* context) {
  (void)context;
  assert(state == PEER_CONNECTION_DISCONNECTED);
  disconnected_events++;
}

static void test_consent(void) {
  Agent a = {0}, b = {0}, other = {0};
  now_ms = 100;
  start_agents(&a, &b);
  assert(agent_create(&other) == 0);
  uint32_t success = a.consent_received_ms;
  assert(agent_consent_check(&a) == 0);
  unsigned slot = a.stun_transaction_next;
  now_ms += a.consent_interval_ms;
  assert(agent_consent_check(&a) == 0);
  assert(a.stun_transactions[slot].pending);
  StunMessage valid = response_for(&a, slot, b.local_upwd, 0);
  StunMessage wrong_key = response_for(&a, slot, "wrong", 0);
  StunMessage wrong_id = {0};
  stun_msg_create(&wrong_id, STUN_CLASS_RESPONSE | STUN_METHOD_BINDING);
  memcpy(((StunHeader*)wrong_id.buf)->transaction_id,
         ((StunHeader*)valid.buf)->transaction_id, 12);
  ((StunHeader*)wrong_id.buf)->transaction_id[0] ^= 1;
  assert(stun_msg_finish(&wrong_id, STUN_CREDENTIAL_SHORT_TERM, b.local_upwd,
                         strlen(b.local_upwd)) == 0);
  send_response(&other, &a, &valid);
  send_response(&b, &a, &wrong_key);
  send_response(&b, &a, &wrong_id);
  assert(a.consent_received_ms == success && a.stun_transactions[slot].pending);
  /* Consume the real request at b, but accept our authenticated response first. */
  receive_stun(&b);
  send_response(&b, &a, &valid);
  assert(a.consent_received_ms == now_ms && !a.stun_transactions[slot].pending);
  receive_stun(&a); /* The duplicate real response cannot refresh consent. */
  success = a.consent_received_ms;
  now_ms += 50;
  send_response(&b, &a, &valid);
  assert(a.consent_received_ms == success);

  /* Two outstanding requests: a delayed response need not be the newest one. */
  unsigned older = a.stun_transaction_next;
  now_ms += 6000;
  assert(agent_consent_check(&a) == 0);
  StunMessage delayed = response_for(&a, older, b.local_upwd, 0);
  unsigned newer = a.stun_transaction_next;
  now_ms += 6000;
  assert(agent_consent_check(&a) == 0);
  StunMessage newest = response_for(&a, newer, b.local_upwd, 0);
  send_response(&b, &a, &newest);
  now_ms++;
  send_response(&b, &a, &delayed);
  assert(a.consent_received_ms == now_ms);

  /* Incoming authenticated requests and arbitrary data do not prove that
   * our outbound packets are received. They must not extend our consent. */
  success = a.consent_received_ms;
  assert(agent_send_binding_request(&b, 1) == 0);
  receive_stun(&a);
  assert(a.consent_received_ms == success);
  now_ms += 6000;
  unsigned lost_slot = a.stun_transaction_next;
  assert(agent_consent_check(&a) == 0);
  StunMessage late = response_for(&a, lost_slot, b.local_upwd, 0);
  now_ms = success + AGENT_CONSENT_TIMEOUT_MS;
  assert(agent_send(&a, (const uint8_t*)"stop", 4) == -1);
  assert(agent_consent_check(&a) == -1 && a.consent_expired);
  send_response(&b, &a, &late);
  assert(a.consent_received_ms == success && a.consent_expired);
  for (unsigned i = 0; i < AGENT_STUN_TRANSACTIONS; i++)
    assert(!a.stun_transactions[i].pending);

  PeerConnection pc = {0};
  pc.agent = a;
  pc.state = PEER_CONNECTION_COMPLETED;
  pc.oniceconnectionstatechange = on_state;
  pc.sctp.connected = 1;
  pc.audio_rb = buffer_new(64);
  pc.video_rb = buffer_new(64);
  pc.data_rb = buffer_new(64);
  assert(pc.audio_rb && pc.video_rb && pc.data_rb);
  assert(buffer_push_tail(pc.audio_rb, (const uint8_t*)"audio", 5) == 5);
  assert(buffer_push_tail(pc.video_rb, (const uint8_t*)"video", 5) == 5);
  assert(buffer_push_tail(pc.data_rb, (const uint8_t*)"data", 4) == 4);
  assert(peer_connection_loop(&pc) == 0);
  assert(pc.state == PEER_CONNECTION_DISCONNECTED && disconnected_events == 1);
  assert(!sctp_is_connected(&pc.sctp));
  assert(!buffer_get_used(pc.audio_rb) && !buffer_get_used(pc.video_rb) &&
         !buffer_get_used(pc.data_rb));
  assert(peer_connection_loop(&pc) == 0 && disconnected_events == 1);
  assert(peer_connection_datachannel_send_sid(&pc, "no", 2, 0) == -1);
  buffer_free(pc.audio_rb); buffer_free(pc.video_rb); buffer_free(pc.data_rb);
  agent_destroy(&a); agent_destroy(&b); agent_destroy(&other);
  puts("ICE consent authentication, source, transaction, replay, expiry and owner cleanup passed");
}

/* ICE-lite services may require the same PRIORITY/role attributes as an
 * initial connectivity check, even after the selected pair is established. */
static void test_consent_ice_lite_attributes(void) {
  for (AgentMode mode = AGENT_MODE_CONTROLLED; mode <= AGENT_MODE_CONTROLLING; mode++) {
    Agent a = {0}, b = {0};
    now_ms = 100;
    start_agents(&a, &b);
    a.mode = mode;
    a.nominated_pair->local->priority = 1234;
    IceCandidatePair* selected = a.selected_pair;
    now_ms += 6000;
    assert(agent_consent_check(&a) == AGENT_CONSENT_VALID);
    StunMessage request = {0};
    Address source;
    int len = agent_socket_recv_attempts(&b, &source, request.buf, sizeof(request.buf), 1000);
    assert(len > 0);
    request.size = (size_t)len;
    assert(stun_parse_msg_buf(&request) == 0);
    assert(stun_msg_is_valid(request.buf, request.size, b.local_upwd) == 0);
    unsigned priorities = 0, roles = 0;
    for (size_t pos = sizeof(StunHeader); pos < request.size;) {
      StunAttribute* attr = (StunAttribute*)(request.buf + pos);
      uint16_t type = ntohs(attr->type), size = ntohs(attr->length);
      assert(type != STUN_ATTR_TYPE_USE_CANDIDATE);
      if (type == STUN_ATTR_TYPE_PRIORITY) {
        uint32_t priority;
        assert(size == sizeof(priority));
        memcpy(&priority, attr->value, sizeof(priority));
        assert(ntohl(priority) == 1234);
        priorities++;
      }
      if (type == STUN_ATTR_TYPE_ICE_CONTROLLED || type == STUN_ATTR_TYPE_ICE_CONTROLLING) {
        assert(type == (mode == AGENT_MODE_CONTROLLED ? STUN_ATTR_TYPE_ICE_CONTROLLED :
                                                        STUN_ATTR_TYPE_ICE_CONTROLLING));
        assert(size == sizeof(uint64_t));
        roles++;
      }
      pos += sizeof(*attr) + ((size + 3U) & ~3U);
    }
    assert(priorities == 1 && roles == 1);
    agent_process_stun_request(&b, &request, &source);
    receive_stun(&a);
    assert(a.consent_received_ms == now_ms && a.selected_pair == selected);
    agent_destroy(&a); agent_destroy(&b);
  }
  puts("ICE-lite consent includes PRIORITY/role without renominating, in both roles");
}

static void test_consent_wrap_and_loss(void) {
  Agent a = {0}, b = {0};
  now_ms = UINT32_MAX - 1000;
  start_agents(&a, &b);
  uint32_t previous_id[3] = {0};
  for (unsigned tick = 0; tick < 200; tick++) {
    assert(a.consent_interval_ms >= 4000 && a.consent_interval_ms <= 6000);
    now_ms += a.consent_interval_ms;
    unsigned slot = a.stun_transaction_next;
    assert(agent_consent_check(&a) == 0);
    assert(memcmp(previous_id, a.stun_transactions[slot].id, sizeof(previous_id)));
    memcpy(previous_id, a.stun_transactions[slot].id, sizeof(previous_id));
    if (tick % 3 != 0) {
      receive_stun(&b);
      receive_stun(&a);
      assert(a.consent_received_ms == now_ms);
    } else {
      uint8_t discarded[CONFIG_MTU];
      assert(agent_socket_recv_attempts(&b, NULL, discarded, sizeof(discarded), 1000) > 0);
    }
  }
  unsigned slot = a.stun_transaction_next;
  now_ms += a.consent_interval_ms;
  assert(agent_consent_check(&a) == 0);
  StunMessage error = response_for(&a, slot, b.local_upwd, 401);
  /* An attacker can recompute FINGERPRINT but cannot authenticate a 403
   * appended after MESSAGE-INTEGRITY. Keep the original signed 401. */
  error.size -= sizeof(StunAttribute) + sizeof(uint32_t);
  char forbidden[] = {0, 0, 4, 3};
  assert(stun_msg_write_attr(&error, STUN_ATTR_TYPE_ERROR_CODE,
                             sizeof(forbidden), forbidden) == 0);
  uint32_t fingerprint = 0;
  assert(stun_msg_write_attr(&error, STUN_ATTR_TYPE_FINGERPRINT,
                             sizeof(fingerprint), (char*)&fingerprint) == 0);
  size_t offset = error.size - sizeof(StunAttribute) - sizeof(uint32_t);
  stun_calculate_fingerprint((char*)error.buf, offset, &fingerprint);
  memcpy(error.buf + offset + sizeof(StunAttribute), &fingerprint, sizeof(fingerprint));
  assert(stun_msg_is_valid(error.buf, error.size, b.local_upwd) == 0);
  send_response(&b, &a, &error);
  assert(!a.consent_expired);
  now_ms += a.consent_interval_ms;
  slot = a.stun_transaction_next;
  assert(agent_consent_check(&a) == 0);
  StunMessage revoked = response_for(&a, slot, b.local_upwd, 403);
  send_response(&b, &a, &revoked);
  assert(a.consent_expired && agent_consent_check(&a) == -1);
  agent_clear_candidates(&a);
  assert(!a.consent_active && !a.consent_expired && !a.nominated_pair);
  agent_destroy(&a); agent_destroy(&b);
  /* Fresh sessions must obtain new consent normally. */
  memset(&a, 0, sizeof(a)); memset(&b, 0, sizeof(b));
  start_agents(&a, &b);
  assert(agent_consent_check(&a) == 0);
  agent_destroy(&a); agent_destroy(&b);
  puts("ICE consent timer wrap, 1/3 lost probes, randomized IDs, 403 and new session passed");
}

static void test_connectivity_transaction_lifetime(void) {
  Agent a = {0}, b = {0};
  now_ms = 100;
  assert(agent_create(&a) == 0 && agent_create(&b) == 0);
  configure_agent(&a, "local", "local-password", &b);
  configure_agent(&b, "remote", "remote-password", &a);
  strcpy(a.remote_ufrag, b.local_ufrag);
  strcpy(a.remote_upwd, b.local_upwd);
  strcpy(b.remote_ufrag, a.local_ufrag);
  strcpy(b.remote_upwd, a.local_upwd);
  assert(agent_connectivity_check(&a) == -1);
  uint8_t first[CONFIG_MTU], retry[CONFIG_MTU];
  int length = agent_socket_recv_attempts(&b, NULL, first, sizeof(first), 1000);
  assert(length > 0 && a.candidate_pairs[0].check_pending);
  /* Poll frequency must not consume the transaction's time/retry budget. */
  for (unsigned i = 0; i < 2000; ++i) assert(agent_select_candidate_pair(&a) == 0);
  now_ms += 500;
  assert(agent_connectivity_check(&a) == -1);
  assert(agent_socket_recv_attempts(&b, NULL, retry, sizeof(retry), 1000) == length);
  assert(!memcmp(first, retry, length));
  assert(a.nominated_pair->conncheck == 2 && !a.stun_transaction_next);
  StunMessage delayed = {0};
  stun_msg_create(&delayed, STUN_CLASS_RESPONSE | STUN_METHOD_BINDING);
  memcpy(((StunHeader*)delayed.buf)->transaction_id,
         ((StunHeader*)first)->transaction_id, 12);
  assert(stun_msg_finish(&delayed, STUN_CREDENTIAL_SHORT_TERM, b.local_upwd,
                         strlen(b.local_upwd)) == 0);
  /* A valid response delayed 20 s still matches the independent check. */
  now_ms += 20000;
  assert(agent_select_candidate_pair(&a) == 0);
  send_response(&b, &a, &delayed);
  assert(a.nominated_pair->state == ICE_CANDIDATE_STATE_SUCCEEDED);
  assert(a.consent_active && !a.candidate_pairs[0].check_pending);
  uint32_t success = a.consent_received_ms;
  now_ms++;
  send_response(&b, &a, &delayed);
  assert(a.consent_received_ms == success);
  agent_destroy(&a); agent_destroy(&b);

  memset(&a, 0, sizeof(a)); memset(&b, 0, sizeof(b));
  assert(agent_create(&a) == 0 && agent_create(&b) == 0);
  configure_agent(&a, "local", "local-password", &b);
  assert(agent_create_binding_request(NULL, NULL, &delayed, 0) == -1);
  Agent empty = {0};
  assert(agent_create_binding_request(&empty, NULL, &delayed, 0) == -1);
  now_ms += AGENT_CHECK_TIMEOUT_MS;
  assert(agent_select_candidate_pair(&a) == -1);
  assert(a.candidate_pairs[0].state == ICE_CANDIDATE_STATE_FAILED);
  agent_destroy(&a); agent_destroy(&b);
  puts("ICE independent transactions, identical retries, 20 s delayed reply and monotonic deadline passed");
}

static void test_parallel_candidate_checks(void) {
  Agent a = {0}, b = {0}, ghost = {0};
  UdpSocket holes[AGENT_MAX_CANDIDATES - 1];
  now_ms = UINT32_MAX - 100U; /* Scheduling must also survive clock wrap. */
  assert(agent_create(&a) == 0 && agent_create(&b) == 0);
  for (unsigned i = 0; i < AGENT_MAX_CANDIDATES - 1; ++i)
    assert(udp_socket_open(&holes[i], AF_INET, 0) == 0);
  ghost.udp_sockets[0] = holes[0];
  configure_agent(&a, "local", "local-password", &ghost);
  configure_agent(&b, "remote", "remote-password", &a);
  strcpy(a.remote_ufrag, b.local_ufrag); strcpy(a.remote_upwd, b.local_upwd);
  strcpy(b.remote_ufrag, a.local_ufrag); strcpy(b.remote_upwd, a.local_upwd);
  for (unsigned i = 1; i < AGENT_MAX_CANDIDATES; ++i) {
    a.remote_candidates[i] = a.remote_candidates[0];
    a.remote_candidates[i].addr = i + 1 == AGENT_MAX_CANDIDATES ?
        loopback_address(&b) : holes[i].bind_addr;
    assert(addr_from_string("127.0.0.1", &a.remote_candidates[i].addr));
  }
  a.remote_candidates_count = AGENT_MAX_CANDIDATES;
  agent_update_candidate_pairs(&a);
  uint32_t began = now_ms;
  assert(agent_connectivity_check(&a) == -1);
  uint32_t first_id[3];
  memcpy(first_id, a.candidate_pairs[0].check_id, sizeof(first_id));
  now_ms = began + AGENT_CHECK_PACING_MS - 1;
  assert(agent_connectivity_check(&a) == -1);
  assert(a.candidate_pairs[1].state == ICE_CANDIDATE_STATE_FROZEN);
  for (unsigned i = 1; i < AGENT_MAX_CANDIDATES; ++i) {
    now_ms = began + AGENT_CHECK_PACING_MS * i;
    assert(agent_select_candidate_pair(&a) == 0);
    assert(agent_connectivity_check(&a) == -1);
    assert(a.candidate_pairs[i].conncheck == 1);
  }
  assert(!a.selected_pair);
  receive_stun(&b); receive_stun(&a);
  IceCandidatePair* winner = &a.candidate_pairs[AGENT_MAX_CANDIDATES - 1];
  assert(a.selected_pair == winner && a.nominated_pair == winner);
  assert((uint32_t)(now_ms - began) == AGENT_CHECK_PACING_MS * (AGENT_MAX_CANDIDATES - 1));
  assert(agent_connectivity_check(&a) == 0);
  uint32_t success = a.consent_received_ms;
  StunMessage late = {0};
  stun_msg_create(&late, STUN_CLASS_RESPONSE | STUN_METHOD_BINDING);
  memcpy(((StunHeader*)late.buf)->transaction_id, first_id, sizeof(first_id));
  assert(stun_msg_finish(&late, STUN_CREDENTIAL_SHORT_TERM, b.local_upwd,
                         strlen(b.local_upwd)) == 0);
  now_ms++;
  Address destination = loopback_address(&a);
  assert(udp_socket_sendto(&holes[0], &destination, late.buf, late.size) == (int)late.size);
  receive_stun(&a);
  assert(a.selected_pair == winner && a.consent_received_ms == success);
  for (unsigned i = 0; i < AGENT_MAX_CANDIDATES - 1; ++i) udp_socket_close(&holes[i]);
  agent_destroy(&a); agent_destroy(&b);
  puts("ICE nine silent candidates cannot block candidate ten: authenticated selection in 450 ms across timer wrap");
}

static void test_pending_consent_and_remote_replacement(void) {
  Agent a = {0}, b = {0};
  now_ms = 100;
  start_agents(&a, &b);
  agent_reset_consent(&a);
  assert(agent_consent_check(&a) == AGENT_CONSENT_PENDING);
  assert(agent_send(&a, (const uint8_t*)"stop", 4) == -1);
  PeerConnection pc = {0};
  pc.agent = a;
  pc.state = PEER_CONNECTION_COMPLETED;
  /* COMPLETED owns an initialized DTLS context, even while consent waits.
   * The receive loop must be allowed to inspect its pending records. */
  assert(dtls_srtp_init(&pc.dtls_srtp, DTLS_SRTP_ROLE_SERVER, &pc) == 0);
  pc.dtls_initialized = 1;
  pc.sctp.connected = 1;
  assert(peer_connection_loop(&pc) == 0);
  assert(pc.state == PEER_CONNECTION_COMPLETED && pc.sctp.connected);
  receive_stun(&b);
  receive_stun(&pc.agent);
  assert(agent_consent_check(&pc.agent) == AGENT_CONSENT_VALID);

  const PeerConnectionState states[] = {PEER_CONNECTION_CHECKING,
      PEER_CONNECTION_CONNECTED, PEER_CONNECTION_COMPLETED,
      PEER_CONNECTION_FAILED, PEER_CONNECTION_DISCONNECTED};
  webrtc_peer_t facade = {0};
  facade.connection = &pc;
  facade.role = WEBRTC_PEER_ROLE_ANSWERER;
  webrtc_peer_message_t offer = {WEBRTC_PEER_MSG_SDP_OFFER, "invalid-new-sdp"};
  for (unsigned i = 0; i < sizeof(states) / sizeof(states[0]); ++i) {
    pc.state = states[i];
    Agent before = pc.agent;
    peer_connection_set_remote_description_with_type(&pc, offer.payload, SDP_TYPE_OFFER);
    assert(pc.state == states[i] && !memcmp(&before, &pc.agent, sizeof(before)));
    assert(webrtc_peer_send_msg(&facade, &offer) == WEBRTC_PEER_ERR_INVALID_STATE);
    assert(pc.state == states[i] && !memcmp(&before, &pc.agent, sizeof(before)));
    PeerConnection transport_before = pc;
    assert(peer_connection_create_offer(&pc) == NULL);
    assert(peer_connection_create_answer(&pc) == NULL);
    assert(!memcmp(&transport_before, &pc, sizeof(pc)));
  }
  assert(!facade.stats.signaling_rx_messages);
  pc.state = PEER_CONNECTION_COMPLETED;
  now_ms += AGENT_CONSENT_TIMEOUT_MS;
  assert(agent_consent_check(&pc.agent) == AGENT_CONSENT_LOST);
  assert(agent_consent_check(&a) == AGENT_CONSENT_LOST); /* Never activated. */
  dtls_srtp_deinit(&pc.dtls_srtp);
  agent_destroy(&a); agent_destroy(&b);
  PeerConnection idle = {0};
  idle.oniceconnectionstatechange = on_state;
  unsigned previous_events = disconnected_events;
  assert(peer_connection_check_consent(&idle) == 0);
  assert(peer_connection_loop(&idle) == 0);
  assert(idle.state == PEER_CONNECTION_CLOSED &&
         disconnected_events == previous_events);
  /* COMPLETED with no path is a different case: preserve its failure guard. */
  PeerConnection invalid = {0};
  invalid.state = PEER_CONNECTION_COMPLETED;
  invalid.sctp.connected = 1;
  assert(peer_connection_check_consent(&invalid) < 0);
  assert(invalid.state == PEER_CONNECTION_FAILED && !invalid.sctp.connected);
  puts("Pending consent blocks DATA without tearing down SCTP; unsupported SDP preserves live state");
}

static void test_stats_abi(void) {
  _Static_assert(sizeof(webrtc_peer_stats_t) == 72, "published stats ABI changed");
  _Static_assert(offsetof(webrtc_peer_stats_t, sctp_peer_supports_forward_tsn) == 68,
                 "published stats layout changed");
  webrtc_peer_t facade = {0};
  PeerConnection pc = {0};
  facade.connection = &pc;
  pc.sctp.data_retransmissions = 42;
  pc.sctp.data_retry_exhaustions = 7;
  struct { webrtc_peer_stats_t base; uint32_t canary; } legacy = {0};
  legacy.canary = 0x12345678;
  assert(webrtc_peer_get_stats(&facade, &legacy.base) == WEBRTC_PEER_OK);
  assert(legacy.canary == 0x12345678);
  assert(webrtc_peer_get_stats_ex(&facade, (webrtc_peer_stats_ex_t*)&legacy.base,
                                 sizeof(legacy.base)) == WEBRTC_PEER_OK);
  assert(legacy.canary == 0x12345678);
  struct { webrtc_peer_stats_ex_t full; uint32_t future; } extended = {0};
  extended.future = 0x12345678;
  assert(webrtc_peer_get_stats_ex(&facade, &extended.full,
                                 sizeof(extended)) == WEBRTC_PEER_OK);
  assert(extended.full.data_retransmissions == 42 &&
         extended.full.data_retry_exhaustions == 7 && extended.future == 0x12345678);
  assert(webrtc_peer_get_stats_ex(&facade, &extended.full, 1) == WEBRTC_PEER_ERR_INVALID_ARG);
  puts("Published stats ABI, sized prefix/extension and buffer canaries passed");
}

static void test_error_attributes(void) {
  const unsigned char codes[][4] = {{0, 0, 2, 99}, {0, 0, 7, 0}, {0, 0, 4, 100}};
  for (unsigned i = 0; i < sizeof(codes) / sizeof(codes[0]); i++) {
    StunMessage msg = {0};
    stun_msg_create(&msg, STUN_CLASS_ERROR | STUN_METHOD_BINDING);
    assert(stun_msg_write_attr(&msg, STUN_ATTR_TYPE_ERROR_CODE, 4, (char*)codes[i]) == 0);
    assert(stun_parse_msg_buf(&msg) < 0);
  }
  StunMessage msg = {0};
  stun_msg_create(&msg, STUN_CLASS_ERROR | STUN_METHOD_BINDING);
  assert(stun_msg_write_attr(&msg, STUN_ATTR_TYPE_ERROR_CODE, 3, "bad") == 0);
  assert(stun_parse_msg_buf(&msg) < 0);
  puts("STUN malformed and unauthenticated trailing error attributes rejected");
}

int main(void) {
  test_trickle_signaling_order();
  test_pending_candidate_bounds_and_reset();
  test_connectivity_transaction_lifetime();
  test_parallel_candidate_checks();
  test_pending_consent_and_remote_replacement();
  test_stats_abi();
  test_trickle();
  test_consent();
  test_consent_ice_lite_attributes();
  test_consent_wrap_and_loss();
  test_error_attributes();
  return 0;
}
