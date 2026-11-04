#ifndef LIBPEER_TURN_H_
#define LIBPEER_TURN_H_

#include <stddef.h>
#include <stdint.h>
#include "address.h"

/* Internal, single-owner RFC 5766 UDP/IPv4 transport. ASCII credentials only.
 * No socket ownership, sleeping, TCP/TLS TURN or SASLprep.
 * The owner supplies I/O, CSPRNG and wrapping monotonic milliseconds.
 * Call poll regularly even without traffic. Credentials are deep copied.
 * All callbacks must be nonblocking and must not re-enter this client. */
#define TURN_MAX_PEERS 10
#define TURN_CONTROL_SIZE 1536
#define TURN_USERNAME_SIZE 512
#define TURN_PASSWORD_SIZE 256
#define TURN_REALM_SIZE 128
#define TURN_NONCE_SIZE 512
#define TURN_AGAIN (-2)
#define TURN_ERROR (-1)

typedef enum {
  TURN_NEW, TURN_ALLOCATING, TURN_READY, TURN_RELEASING, TURN_CLOSED, TURN_FAILED
} TurnState;

typedef int (*TurnSend)(void* user, const Address* server, const uint8_t* data, size_t size);
typedef int (*TurnRandom)(void* user, uint8_t* data, size_t size);

typedef struct {
  Address address;
  uint32_t permission_at, channel_at;
  uint16_t channel;
  uint8_t used, permission, bound;
} TurnPeer;

typedef struct {
  TurnState state;
  Address server, relayed, mapped;
  TurnSend send_datagram;
  TurnRandom random;
  void* user;
  char username[TURN_USERNAME_SIZE], password[TURN_PASSWORD_SIZE];
  uint8_t realm[TURN_REALM_SIZE], nonce[TURN_NONCE_SIZE], key[16];
  size_t realm_size, nonce_size;
  uint8_t authenticated, operation, pending_peer, challenges, attempts;
  uint8_t transaction[12], request[TURN_CONTROL_SIZE];
  size_t request_size;
  uint32_t started_at, sent_at, allocated_at, lifetime_ms;
  TurnPeer peers[TURN_MAX_PEERS];
} TurnClient;

/* init is only valid for fresh/destroyed storage; returns 0 or TURN_ERROR. */
int turn_init(TurnClient* client, const Address* server, const char* username,
              const char* password, TurnSend send, TurnRandom random, void* user);
int turn_start(TurnClient* client, uint32_t now);
void turn_poll(TurnClient* client, uint32_t now);
/* prepare is idempotent; TURN_AGAIN means permission/channel is still pending. */
int turn_prepare_peer(TurnClient* client, const Address* peer, uint32_t now);
/* send accepts one whole datagram (1..CONFIG_MTU); no internal data queue.
 * Returns payload size, TURN_AGAIN (nothing accepted), or TURN_ERROR. */
int turn_send(TurnClient* client, const Address* peer, const uint8_t* data,
              size_t size, uint32_t now);
/* Returns payload size, 0 for consumed control, TURN_ERROR for rejected input.
 * The output may alias input. Data Indications and ChannelData require a live
 * permission for the source IP (RFC 5766 permissions are not port-specific);
 * ChannelData also requires a live channel. Output peer is written
 * only on success. Control packets never become application data. */
int turn_input(TurnClient* client, const Address* source, const uint8_t* packet,
               size_t size, Address* peer, uint8_t* data, size_t capacity, uint32_t now);
/* Supersedes a pending request with authenticated Refresh(0). poll/input must
 * continue until CLOSED/FAILED (at most 8s). destroy wipes local secrets; it
 * cannot acknowledge server release without this owner-driven close exchange. */
int turn_release(TurnClient* client, uint32_t now);
void turn_destroy(TurnClient* client);

#endif
