#include "turn.h"

#include <string.h>
#include <mbedtls/md.h>
#include <mbedtls/platform_util.h>

#define COOKIE 0x2112a442u
#define ALLOCATE 3
#define REFRESH 4
#define PERMISSION 8
#define CHANNEL_BIND 9
#define USERNAME 6
#define INTEGRITY 8
#define ERROR_CODE 9
#define CHANNEL_NUMBER 0x0c
#define LIFETIME 0x0d
#define XOR_PEER 0x12
#define DATA 0x13
#define REALM 0x14
#define NONCE 0x15
#define XOR_RELAYED 0x16
#define REQUESTED_TRANSPORT 0x19
#define XOR_MAPPED 0x20
#define FINGERPRINT 0x8028
#define PERMISSION_MS 300000u
#define CHANNEL_MS 600000u
#define REFRESH_PEER_MS 120000u
#define REQUEST_TIMEOUT_MS 8000u

/* Attribute views point into the validated input and are never retained. */
typedef struct { const uint8_t* value; size_t size; } Attribute;
typedef struct {
  Attribute realm, nonce, error, lifetime, relayed, mapped, peer, data;
  size_t integrity;
} Response;

static uint16_t get16(const uint8_t* p) { return (uint16_t)((p[0] << 8) | p[1]); }
static uint32_t get32(const uint8_t* p) { return ((uint32_t)get16(p) << 16) | get16(p + 2); }
static void put16(uint8_t* p, uint16_t v) { p[0] = (uint8_t)(v >> 8); p[1] = (uint8_t)v; }
static void put32(uint8_t* p, uint32_t v) { put16(p, (uint16_t)(v >> 16)); put16(p + 2, (uint16_t)v); }
static size_t padded(size_t n) { return (n + 3) & ~(size_t)3; }
static int valid_address(const Address* a) { return a && a->family == AF_INET && a->port != 0; }

static void wipe_credentials(TurnClient* c) {
  mbedtls_platform_zeroize(c->password, sizeof(c->password));
  mbedtls_platform_zeroize(c->username, sizeof(c->username));
  mbedtls_platform_zeroize(c->realm, sizeof(c->realm));
  mbedtls_platform_zeroize(c->nonce, sizeof(c->nonce));
  mbedtls_platform_zeroize(c->key, sizeof(c->key));
  mbedtls_platform_zeroize(c->request, sizeof(c->request));
  c->realm_size = c->nonce_size = c->request_size = 0;
  c->authenticated = c->operation = 0;
}

static int fail(TurnClient* c) {
  wipe_credentials(c);
  c->state = TURN_FAILED;
  return TURN_ERROR;
}

void turn_destroy(TurnClient* c) {
  if (c) mbedtls_platform_zeroize(c, sizeof(*c));
}

int turn_init(TurnClient* c, const Address* server, const char* username,
              const char* password, TurnSend send, TurnRandom random, void* user) {
  size_t un, pn;
  if (!c) return TURN_ERROR;
  memset(c, 0, sizeof(*c));
  if (!valid_address(server) || !username || !password || !send || !random) return fail(c);
  for (un = 0; un < sizeof(c->username) && username[un]; ++un) {}
  for (pn = 0; pn < sizeof(c->password) && password[pn]; ++pn) {}
  if (!un || !pn || un == sizeof(c->username) || pn == sizeof(c->password)) return fail(c);
  /* Internationalized credentials need SASLprep, which this bounded embedded
   * profile does not implement. Do not silently derive an incorrect key. */
  for (size_t i = 0; i < un; ++i)
    if ((uint8_t)username[i] < 0x20 || (uint8_t)username[i] > 0x7e) return fail(c);
  for (size_t i = 0; i < pn; ++i)
    if ((uint8_t)password[i] < 0x20 || (uint8_t)password[i] > 0x7e) return fail(c);
  c->server = *server;
  memcpy(c->username, username, un + 1);
  memcpy(c->password, password, pn + 1);
  c->send_datagram = send;
  c->random = random;
  c->user = user;
  return 0;
}

static int add_attribute(TurnClient* c, uint16_t type, const void* value, size_t size) {
  size_t n = c->request_size;
  if (size > UINT16_MAX || n + 4 + padded(size) > sizeof(c->request)) return TURN_ERROR;
  put16(c->request + n, type);
  put16(c->request + n + 2, (uint16_t)size);
  memcpy(c->request + n + 4, value, size);
  memset(c->request + n + 4 + size, 0, padded(size) - size);
  c->request_size += 4 + padded(size);
  return 0;
}

static void encode_address(const Address* address, uint8_t value[8]) {
  value[0] = 0;
  value[1] = 1;
  put16(value + 2, address->port ^ (COOKIE >> 16));
  put32(value + 4, ntohl(address->sin.sin_addr.s_addr) ^ COOKIE);
}

static int decode_address(Attribute a, Address* address) {
  if (a.size != 8 || a.value[0] || a.value[1] != 1) return TURN_ERROR;
  memset(address, 0, sizeof(*address));
  addr_set_family(address, AF_INET);
  address->sin.sin_family = AF_INET;
  address->sin.sin_addr.s_addr = htonl(get32(a.value + 4) ^ COOKIE);
  addr_set_port(address, get16(a.value + 2) ^ (COOKIE >> 16));
  return address->port ? 0 : TURN_ERROR;
}

static int digest_key(TurnClient* c) {
  uint8_t joined[TURN_USERNAME_SIZE + TURN_REALM_SIZE + TURN_PASSWORD_SIZE + 2];
  size_t un = strlen(c->username), pn = strlen(c->password), n = 0;
  int ret;
  const mbedtls_md_info_t* md = mbedtls_md_info_from_type(MBEDTLS_MD_MD5);
  memcpy(joined, c->username, un); n += un;
  joined[n++] = ':';
  memcpy(joined + n, c->realm, c->realm_size); n += c->realm_size;
  joined[n++] = ':';
  memcpy(joined + n, c->password, pn); n += pn;
  ret = md ? mbedtls_md(md, joined, n, c->key) : -1;
  mbedtls_platform_zeroize(joined, sizeof(joined));
  return ret;
}

static int hmac(const uint8_t key[16], const uint8_t* packet, size_t size, uint8_t out[20]) {
  const mbedtls_md_info_t* md = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
  return md ? mbedtls_md_hmac(md, key, 16, packet, size, out) : -1;
}

static int begin_request(TurnClient* c, uint8_t operation, uint8_t peer, uint32_t now) {
  uint8_t value[8], signature[20];
  int bad = 0;
  memset(c->request, 0, sizeof(c->request));
  c->operation = operation;
  c->pending_peer = peer;
  c->request_size = 20;
  c->attempts = 1;
  c->sent_at = now;
  if (c->random(c->user, c->transaction, sizeof(c->transaction)) != 0) return fail(c);
  put16(c->request, operation);
  put32(c->request + 4, COOKIE);
  memcpy(c->request + 8, c->transaction, 12);
  if (operation == ALLOCATE) {
    value[0] = 17; value[1] = value[2] = value[3] = 0;
    bad |= add_attribute(c, REQUESTED_TRANSPORT, value, 4);
  } else if (operation == REFRESH) {
    put32(value, c->state == TURN_RELEASING ? 0 : 600);
    bad |= add_attribute(c, LIFETIME, value, 4);
  } else {
    if (peer >= TURN_MAX_PEERS || !c->peers[peer].used) return fail(c);
    encode_address(&c->peers[peer].address, value);
    bad |= add_attribute(c, XOR_PEER, value, 8);
    if (operation == CHANNEL_BIND) {
      put16(value, c->peers[peer].channel); value[2] = value[3] = 0;
      bad |= add_attribute(c, CHANNEL_NUMBER, value, 4);
    }
  }
  if (c->authenticated) {
    bad |= add_attribute(c, USERNAME, c->username, strlen(c->username));
    bad |= add_attribute(c, REALM, c->realm, c->realm_size);
    bad |= add_attribute(c, NONCE, c->nonce, c->nonce_size);
    if (bad || c->request_size + 24 > sizeof(c->request)) return fail(c);
    put16(c->request + 2, (uint16_t)(c->request_size + 24 - 20));
    if (hmac(c->key, c->request, c->request_size, signature) != 0) return fail(c);
    bad |= add_attribute(c, INTEGRITY, signature, sizeof(signature));
    mbedtls_platform_zeroize(signature, sizeof(signature));
  }
  if (bad) return fail(c);
  put16(c->request + 2, (uint16_t)(c->request_size - 20));
  /* UDP backpressure is retried using this exact transaction and packet. */
  c->send_datagram(c->user, &c->server, c->request, c->request_size);
  return 0;
}

int turn_start(TurnClient* c, uint32_t now) {
  if (!c || c->state != TURN_NEW || !c->send_datagram || !c->random) return TURN_ERROR;
  c->state = TURN_ALLOCATING;
  c->challenges = 0;
  c->started_at = now;
  return begin_request(c, ALLOCATE, 0, now);
}

static int permission_live(const TurnPeer* p, uint32_t now) {
  return p->permission && now - p->permission_at < PERMISSION_MS;
}
static int channel_live(const TurnPeer* p, uint32_t now) {
  return p->bound && now - p->channel_at < CHANNEL_MS;
}
static int allocation_live(const TurnClient* c, uint32_t now) {
  return c->lifetime_ms && now - c->allocated_at < c->lifetime_ms;
}

void turn_poll(TurnClient* c, uint32_t now) {
  unsigned i;
  if (!c || c->state == TURN_NEW || c->state == TURN_CLOSED || c->state == TURN_FAILED) return;
  if (c->state == TURN_READY && !allocation_live(c, now)) { fail(c); return; }
  if (c->operation) {
    if (now - c->started_at >= REQUEST_TIMEOUT_MS) { fail(c); return; }
    if (c->attempts < 5 && now - c->sent_at >= (500u << (c->attempts - 1))) {
      c->send_datagram(c->user, &c->server, c->request, c->request_size);
      c->sent_at = now;
      c->attempts++;
    }
    return;
  }
  if (c->state != TURN_READY) return;
  c->challenges = 0;
  c->started_at = now;
  if (now - c->allocated_at >= c->lifetime_ms / 2) {
    begin_request(c, REFRESH, 0, now);
    return;
  }
  for (i = 0; i < TURN_MAX_PEERS; ++i) {
    TurnPeer* p = &c->peers[i];
    if (!p->used) continue;
    if (!p->permission || now - p->permission_at >= REFRESH_PEER_MS) {
      begin_request(c, PERMISSION, (uint8_t)i, now);
      return;
    }
    if (!p->bound || now - p->channel_at >= REFRESH_PEER_MS) {
      begin_request(c, CHANNEL_BIND, (uint8_t)i, now);
      return;
    }
  }
}

int turn_prepare_peer(TurnClient* c, const Address* peer, uint32_t now) {
  int i, free_slot = -1;
  if (!c || c->state != TURN_READY || !valid_address(peer)) return TURN_ERROR;
  if (!allocation_live(c, now)) return fail(c);
  for (i = 0; i < TURN_MAX_PEERS; ++i) {
    TurnPeer* p = &c->peers[i];
    if (!p->used) { if (free_slot < 0) free_slot = i; continue; }
    if (addr_equal(&p->address, peer)) {
      turn_poll(c, now);
      if (c->state != TURN_READY) return TURN_ERROR;
      return permission_live(p, now) && channel_live(p, now) ? 0 : TURN_AGAIN;
    }
  }
  if (free_slot < 0) return TURN_ERROR;
  c->peers[free_slot].used = 1;
  c->peers[free_slot].address = *peer;
  c->peers[free_slot].channel = (uint16_t)(0x4000 + free_slot);
  turn_poll(c, now);
  return c->state == TURN_READY ? TURN_AGAIN : TURN_ERROR;
}

int turn_send(TurnClient* c, const Address* peer, const uint8_t* data, size_t size, uint32_t now) {
  uint8_t packet[CONFIG_MTU + 4];
  int i, ret;
  if (!data || !size || size > CONFIG_MTU) return TURN_ERROR;
  ret = turn_prepare_peer(c, peer, now);
  if (ret) return ret;
  for (i = 0; i < TURN_MAX_PEERS; ++i) {
    if (c->peers[i].used && addr_equal(peer, &c->peers[i].address)) {
      put16(packet, c->peers[i].channel);
      put16(packet + 2, (uint16_t)size);
      memcpy(packet + 4, data, size);
      ret = c->send_datagram(c->user, &c->server, packet, size + 4);
      return ret == (int)size + 4 ? (int)size : TURN_AGAIN;
    }
  }
  return TURN_ERROR;
}

static int view(Attribute* a, const uint8_t* value, size_t size) {
  if (a->value) return TURN_ERROR; /* Reject ambiguous duplicate attributes. */
  a->value = value; a->size = size;
  return 0;
}

static int parse(const uint8_t* packet, size_t size, Response* r) {
  size_t pos = 20, end;
  uint16_t type, n;
  Attribute* a;
  memset(r, 0, sizeof(*r));
  if (size < 20 || (packet[0] & 0xc0) || get32(packet + 4) != COOKIE ||
      get16(packet + 2) % 4 || (size_t)get16(packet + 2) + 20 != size) return TURN_ERROR;
  while (pos < size) {
    if (size - pos < 4) return TURN_ERROR;
    type = get16(packet + pos); n = get16(packet + pos + 2);
    end = pos + 4 + padded(n);
    if (end > size || (r->integrity && type != FINGERPRINT)) return TURN_ERROR;
    a = NULL;
    switch (type) {
      case REALM: a = &r->realm; break;
      case NONCE: a = &r->nonce; break;
      case ERROR_CODE: a = &r->error; break;
      case LIFETIME: a = &r->lifetime; break;
      case XOR_RELAYED: a = &r->relayed; break;
      case XOR_MAPPED: a = &r->mapped; break;
      case XOR_PEER: a = &r->peer; break;
      case DATA: a = &r->data; break;
      case INTEGRITY:
        if (n != 20 || r->integrity) return TURN_ERROR;
        r->integrity = pos;
        break;
      case FINGERPRINT:
        if (n != 4 || end != size) return TURN_ERROR;
        break;
      default:
        /* Unknown comprehension-required attributes cannot safely be ignored. */
        if (type < 0x8000 && type != USERNAME && type != CHANNEL_NUMBER &&
            type != REQUESTED_TRANSPORT) return TURN_ERROR;
        break;
    }
    if (a && view(a, packet + pos + 4, n)) return TURN_ERROR;
    pos = end;
  }
  return 0;
}

static int verify(TurnClient* c, const uint8_t* packet, const Response* r) {
  uint8_t prefix[TURN_CONTROL_SIZE], signature[20], difference = 0;
  size_t i;
  int ret;
  if (!r->integrity || r->integrity > sizeof(prefix)) return TURN_ERROR;
  memcpy(prefix, packet, r->integrity);
  put16(prefix + 2, (uint16_t)(r->integrity + 24 - 20));
  ret = hmac(c->key, prefix, r->integrity, signature);
  if (ret == 0) {
    for (i = 0; i < sizeof(signature); ++i) difference |= signature[i] ^ packet[r->integrity + 4 + i];
    ret = difference ? TURN_ERROR : 0;
  }
  mbedtls_platform_zeroize(signature, sizeof(signature));
  mbedtls_platform_zeroize(prefix, sizeof(prefix));
  return ret;
}

static int deliver(const uint8_t* input, size_t size, const Address* address,
                    Address* peer, uint8_t* data, size_t capacity) {
  if (!peer || !data || !size || size > capacity || size > CONFIG_MTU) return TURN_ERROR;
  memmove(data, input, size);
  *peer = *address;
  return (int)size;
}

int turn_input(TurnClient* c, const Address* source, const uint8_t* packet,
               size_t size, Address* peer, uint8_t* data, size_t capacity, uint32_t now) {
  Response r;
  Address decoded, mapped;
  uint16_t type;
  uint32_t lifetime;
  unsigned i, code;
  if (!c || !source || !packet || !addr_equal(source, &c->server) || size < 4 ||
      c->state == TURN_NEW || c->state == TURN_FAILED || c->state == TURN_CLOSED) return TURN_ERROR;
  if ((c->state == TURN_READY && !allocation_live(c, now)) ||
      (c->operation && now - c->started_at >= REQUEST_TIMEOUT_MS)) return fail(c);
  type = get16(packet);
  if (type >= 0x4000 && type <= 0x7fff) {
    size_t n = get16(packet + 2);
    if (c->state != TURN_READY || (size != n + 4 && size != padded(n) + 4)) return TURN_ERROR;
    for (i = 0; i < TURN_MAX_PEERS; ++i) {
      TurnPeer* p = &c->peers[i];
      if (p->used && p->channel == type && permission_live(p, now) && channel_live(p, now))
        return deliver(packet + 4, n, &p->address, peer, data, capacity);
    }
    return TURN_ERROR;
  }
  if (parse(packet, size, &r)) return TURN_ERROR;
  if (type == 0x0017) { /* Data Indication has no long-term MESSAGE-INTEGRITY. */
    if (c->state != TURN_READY || decode_address(r.peer, &decoded) || !r.data.value) return TURN_ERROR;
    for (i = 0; i < TURN_MAX_PEERS; ++i) {
      TurnPeer* p = &c->peers[i];
      if (p->used && permission_live(p, now) &&
          p->address.sin.sin_addr.s_addr == decoded.sin.sin_addr.s_addr)
        return deliver(r.data.value, r.data.size, &decoded, peer, data, capacity);
    }
    return TURN_ERROR;
  }
  if (!c->operation || memcmp(packet + 8, c->transaction, 12) ||
      (type != (0x0100 | c->operation) && type != (0x0110 | c->operation))) return TURN_ERROR;
  if (c->authenticated) {
    /* A stale-nonce challenge may be unsigned (RFC 5389 10.2.3). It can only
     * trigger a bounded authenticated retry, never acknowledge an operation.
     * If a server includes MI, do verify it even on an error/challenge. */
    if (r.integrity) {
      if (verify(c, packet, &r)) return TURN_ERROR;
    } else if (type != (0x0110 | c->operation) || r.error.size < 4 ||
               r.error.value[2] != 4 || r.error.value[3] != 38) return TURN_ERROR;
  }
  if (type == (0x0110 | c->operation)) {
    if (r.error.size < 4 || r.error.value[0] || r.error.value[1] ||
        r.error.value[2] > 6 || r.error.value[3] > 99) return TURN_ERROR;
    code = r.error.value[2] * 100u + r.error.value[3];
    if ((!c->authenticated && code == 401 && c->operation == ALLOCATE) ||
        (c->authenticated && code == 438)) {
      if (!r.nonce.size || r.nonce.size > sizeof(c->nonce) || ++c->challenges > 2) return fail(c);
      if (!c->authenticated) {
        if (!r.realm.size || r.realm.size > sizeof(c->realm)) return fail(c);
        memcpy(c->realm, r.realm.value, r.realm.size); c->realm_size = r.realm.size;
        if (digest_key(c)) return fail(c);
      } else if (r.realm.value && (r.realm.size != c->realm_size ||
                 memcmp(r.realm.value, c->realm, c->realm_size))) return fail(c);
      memcpy(c->nonce, r.nonce.value, r.nonce.size); c->nonce_size = r.nonce.size;
      c->authenticated = 1;
      return begin_request(c, c->operation, c->pending_peer, now);
    }
    return fail(c);
  }
  if (!c->authenticated) return TURN_ERROR; /* Never accept unauthenticated success. */
  if (c->operation == ALLOCATE || c->operation == REFRESH) {
    if (r.lifetime.size != 4) return TURN_ERROR;
    lifetime = get32(r.lifetime.value);
    if (c->state == TURN_RELEASING) {
      if (lifetime != 0) return TURN_ERROR;
      wipe_credentials(c);
      c->state = TURN_CLOSED;
      return 0;
    }
    if (!lifetime || lifetime > 86400) return TURN_ERROR;
    if (c->operation == ALLOCATE) {
      if (decode_address(r.relayed, &decoded) || decode_address(r.mapped, &mapped)) return TURN_ERROR;
      c->relayed = decoded; c->mapped = mapped;
    }
    c->lifetime_ms = lifetime * 1000u;
    c->allocated_at = now;
    c->state = TURN_READY;
  } else {
    TurnPeer* p = &c->peers[c->pending_peer];
    p->permission = 1;
    p->permission_at = now; /* ChannelBind also refreshes the permission. */
    if (c->operation == CHANNEL_BIND) { p->bound = 1; p->channel_at = now; }
  }
  c->operation = 0;
  c->request_size = 0;
  return 0;
}

int turn_release(TurnClient* c, uint32_t now) {
  if (!c || c->state != TURN_READY) return TURN_ERROR;
  c->state = TURN_RELEASING;
  c->challenges = 0;
  c->started_at = now;
  return begin_request(c, REFRESH, 0, now);
}
