#ifndef AGENT_H_
#define AGENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "base64.h"
#include "ice.h"
#include "socket.h"
#include "stun.h"
#include "utils.h"

#ifndef AGENT_MAX_DESCRIPTION
#define AGENT_MAX_DESCRIPTION 40960
#endif

#ifndef AGENT_MAX_CANDIDATES
#define AGENT_MAX_CANDIDATES 10
#endif

#ifndef AGENT_MAX_CANDIDATE_PAIRS
#define AGENT_MAX_CANDIDATE_PAIRS 100
#endif

/* Eight slots cover the 30 s consent lifetime at the minimum 4 s interval. */
#define AGENT_STUN_TRANSACTIONS 8

typedef struct AgentStunTransaction {
  uint32_t id[3];
  uint32_t sent_ms;
  int pending;
} AgentStunTransaction;

typedef enum AgentState {

  AGENT_STATE_GATHERING_ENDED = 0,
  AGENT_STATE_GATHERING_STARTED,
  AGENT_STATE_GATHERING_COMPLETED,

} AgentState;

typedef enum AgentMode {

  AGENT_MODE_CONTROLLED = 0,
  AGENT_MODE_CONTROLLING

} AgentMode;

typedef struct Agent Agent;

struct Agent {
  char remote_ufrag[ICE_UFRAG_LENGTH + 1];
  char remote_upwd[ICE_UPWD_LENGTH + 1];

  char local_ufrag[ICE_UFRAG_LENGTH + 1];
  char local_upwd[ICE_UPWD_LENGTH + 1];

  IceCandidate local_candidates[AGENT_MAX_CANDIDATES];
  IceCandidate remote_candidates[AGENT_MAX_CANDIDATES];

  int local_candidates_count;
  int remote_candidates_count;

  UdpSocket udp_sockets[2];

  Address host_addr;
  int b_host_addr;
  UtilsRandom* random;
  /* Connectivity state is bounded by candidate_pairs, never consent slots. */
  uint32_t check_sent_ms;
  int check_sent;
  AgentStunTransaction stun_transactions[AGENT_STUN_TRANSACTIONS];
  unsigned stun_transaction_next;
  uint32_t consent_received_ms;
  uint32_t binding_sent_ms;
  uint32_t consent_interval_ms;
  int consent_active;
  int consent_probing;
  int consent_expired;
  uint32_t valid_stun_requests;
  uint32_t invalid_stun_requests;
  uint32_t valid_stun_responses;
  uint32_t received_packets;
  uint32_t stun_parse_failures;
  uint32_t nonstun_packets;
  uint32_t invalid_stun_responses;
  uint32_t failed_stun_responses;
  AgentState state;

  AgentMode mode;

  IceCandidatePair candidate_pairs[AGENT_MAX_CANDIDATE_PAIRS];
  IceCandidatePair* selected_pair;
  IceCandidatePair* nominated_pair;

  int candidate_pairs_num;
  int use_candidate;
  uint32_t transaction_id[3];
};

void agent_gather_candidate(Agent* agent, const char* urls, const char* username, const char* credential);

void agent_get_local_description(Agent* agent, char* description, int length);

int agent_send(Agent* agent, const uint8_t* buf, int len);

int agent_recv(Agent* agent, uint8_t* buf, int len);

/* Poll an established connection without sleeping while its owner holds
 * the peer mutex. Handshake and gathering retain agent_recv's timeout. */
int agent_recv_nonblocking(Agent* agent, uint8_t* buf, int len);

void agent_set_remote_description(Agent* agent, char* description);

int agent_select_candidate_pair(Agent* agent);

int agent_connectivity_check(Agent* agent);

void agent_clear_candidates(Agent* agent);

int agent_create(Agent* agent);

void agent_destroy(Agent* agent);

void agent_update_candidate_pairs(Agent* agent);

/* Nonblocking owner-loop tick. Pending is distinct from lost; only VALID
 * permits application traffic. A missing selected path is an invariant error.
 * A new ICE session is required after loss; late replies cannot revive it. */
enum {
  AGENT_CONSENT_INVALID = -2,
  AGENT_CONSENT_LOST = -1,
  AGENT_CONSENT_VALID = 0,
  AGENT_CONSENT_PENDING = 1
};
int agent_consent_check(Agent* agent);

#endif  // AGENT_H_
