#ifndef SCTP_H_
#define SCTP_H_

#include "buffer.h"
#include "config.h"
#include "dtls_srtp.h"
#include "utils.h"

typedef enum DecpMsgType {

  DATA_CHANNEL_OPEN = 0x03,
  DATA_CHANNEL_ACK = 0x02,

} DecpMsgType;

typedef enum DataChannelPpid {

  DATA_CHANNEL_PPID_CONTROL = 50,
  DATA_CHANNEL_PPID_DOMSTRING = 51,
  DATA_CHANNEL_PPID_BINARY_PARTIAL = 52,
  DATA_CHANNEL_PPID_BINARY = 53,
  DATA_CHANNEL_PPID_DOMSTRING_PARTIAL = 54

} DataChannelPpid;

#if !CONFIG_USE_USRSCTP

typedef struct SctpChunkParam {
  uint16_t type;
  uint16_t length;
  uint8_t value[0];

} SctpChunkParam;

typedef enum SctpParamType {

  SCTP_PARAM_STATE_COOKIE = 7,
  SCTP_PARAM_FORWARD_TSN_SUPPORTED = 0xc000,

} SctpParamType;

typedef enum SctpHeaderType {

  SCTP_DATA = 0,
  SCTP_INIT = 1,
  SCTP_INIT_ACK = 2,
  SCTP_SACK = 3,
  SCTP_HEARTBEAT = 4,
  SCTP_HEARTBEAT_ACK = 5,
  SCTP_ABORT = 6,
  SCTP_SHUTDOWN = 7,
  SCTP_SHUTDOWN_ACK = 8,
  SCTP_ERROR = 9,
  SCTP_COOKIE_ECHO = 10,
  SCTP_COOKIE_ACK = 11,
  SCTP_ECNE = 12,
  SCTP_CWR = 13,
  SCTP_SHUTDOWN_COMPLETE = 14,
  SCTP_AUTH = 15,
  SCTP_ASCONF_ACK = 128,
  SCTP_ASCONF = 130,
  SCTP_FORWARD_TSN = 192

} SctpHeaderType;

typedef struct SctpChunkCommon {
  uint8_t type;
  uint8_t flags;
  uint16_t length;

} SctpChunkCommon;

typedef struct SctpForwardTsnChunk {
  SctpChunkCommon common;
  uint32_t new_cumulative_tsn;
  uint8_t streams[0];

} SctpForwardTsnChunk;

typedef struct SctpGapAckBlock {
  uint16_t start;
  uint16_t end;
} SctpGapAckBlock;

typedef struct SctpHeader {
  uint16_t source_port;
  uint16_t destination_port;
  uint32_t verification_tag;
  uint32_t checksum;

} SctpHeader;

typedef struct SctpPacket {
  SctpHeader header;
  uint8_t chunks[0];

} SctpPacket;

typedef struct SctpSackChunk {
  SctpChunkCommon common;
  uint32_t cumulative_tsn_ack;
  uint32_t a_rwnd;
  uint16_t number_of_gap_ack_blocks;
  uint16_t number_of_dup_tsns;
  uint8_t blocks[0];

} SctpSackChunk;

typedef struct SctpDataChunk {
  uint8_t type;
  uint8_t iube;
  uint16_t length;
  uint32_t tsn;
  uint16_t sid;
  uint16_t sqn;
  uint32_t ppid;
  uint8_t data[0];

} SctpDataChunk;

typedef struct SctpInitChunk {
  SctpChunkCommon common;
  uint32_t initiate_tag;
  uint32_t a_rwnd;
  uint16_t number_of_outbound_streams;
  uint16_t number_of_inbound_streams;
  uint32_t initial_tsn;
  SctpChunkParam param[0];

} SctpInitChunk;

typedef struct SctpCookieEchoChunk {
  SctpChunkCommon common;
  uint8_t cookie[0];
} SctpCookieEchoChunk;

#endif

typedef enum SctpDataPpid {

  PPID_CONTROL = 50,
  PPID_STRING = 51,
  PPID_BINARY = 53,
  PPID_STRING_EMPTY = 56,
  PPID_BINARY_EMPTY = 57

} SctpDataPpid;

#define SCTP_MAX_STREAMS 5
#define SCTP_TX_HISTORY_SIZE 64

#if !CONFIG_USE_USRSCTP
typedef enum {
  SCTP_RETRY_NONE,
  SCTP_RETRY_INIT,
  SCTP_RETRY_COOKIE_ECHO,
  SCTP_RETRY_DCEP_OPEN
} SctpRetryKind;

typedef struct {
  uint8_t packet[CONFIG_MTU];
  size_t size;
  uint32_t started_ms;
  uint32_t last_sent_ms;
  uint16_t sid;
  uint8_t retries;
  SctpRetryKind kind;
} SctpRetryPacket;
#endif

typedef struct {
  char label[32];  // Stream label
  uint16_t sid;    // Stream ID
  uint16_t next_ssn;
  uint16_t rx_next_ssn;
  uint8_t open;
  uint8_t unordered;
  uint8_t drop_on_send_failure;
} SctpStreamEntry;

typedef struct {
  uint32_t tsn;
  uint8_t valid;
  uint8_t abandonable;
} SctpTxHistoryEntry;

typedef int (*SctpSendPacketCallback)(void* user_data,
                                      const uint8_t* packet,
                                      size_t packet_size);

typedef struct Sctp {
  struct socket* sock;

  int local_port;
  int remote_port;
  int connected;
  uint32_t verification_tag;
  uint32_t tsn;
  DtlsSrtp* dtls_srtp;
  SctpSendPacketCallback send_packet;
  void* send_packet_user_data;
  Buffer** data_rb;
  int stream_count;
  SctpStreamEntry stream_table[SCTP_MAX_STREAMS];
  uint16_t next_ssn_fallback;
#if !CONFIG_USE_USRSCTP
  uint32_t tx_cumulative_tsn_ack;
  /* Preserve abandonable history across ring-buffer eviction. Reliable
   * control TSNs stop this prefix until the peer acknowledges them. */
  uint32_t tx_abandonable_prefix_tsn;
  uint32_t tx_evicted_through_tsn;
  uint32_t tx_evicted_reliable_tsn;
  uint32_t last_forward_tsn;
  uint32_t forward_tsn_sent;
  uint32_t sack_gap_reports;
  uint32_t data_retransmissions;
  uint32_t data_retry_exhaustions;
  uint8_t tx_cumulative_tsn_ack_valid;
  uint8_t tx_abandonable_prefix_valid;
  uint8_t tx_evicted_through_valid;
  uint8_t tx_evicted_reliable_valid;
  uint8_t peer_supports_forward_tsn;
  uint8_t last_forward_tsn_valid;
  uint8_t forward_tsn_repeat_sacks;
  SctpTxHistoryEntry tx_history[SCTP_TX_HISTORY_SIZE];
  SctpRetryPacket retry_packet;
  struct SctpTxPacket* tx_head;
  struct SctpTxPacket* tx_tail;
  unsigned int tx_count;
  int cookie_issued;
  uint32_t rx_tsn;
  int rx_tsn_valid;
  struct SctpRxChunk* rx_head;
  unsigned int rx_count;
  uint32_t rx_tsn_mask;
  uint8_t rx_sack_pending;
#endif

  /* datachannel */
  void (*onmessage)(char* msg, size_t len, void* userdata, uint16_t sid);
  void (*onopen)(void* userdata);
  void (*onclose)(void* userdata);

  void* userdata;
  uint8_t buf[CONFIG_MTU];
} Sctp;

int sctp_create_association(Sctp* sctp, DtlsSrtp* dtls_srtp);

void sctp_destroy_association(Sctp* sctp);

int sctp_is_connected(Sctp* sctp);

/* Drive retransmission, queued receive delivery and receive-window updates. */
void sctp_poll(Sctp* sctp, uint32_t now_ms);

int sctp_is_stream_open(Sctp* sctp, uint16_t sid);

void sctp_set_stream_unordered(Sctp* sctp, uint16_t sid, int unordered);

void sctp_set_stream_drop_on_send_failure(Sctp* sctp, uint16_t sid,
                                          int drop_on_send_failure);

int sctp_stream_drops_on_send_failure(Sctp* sctp, uint16_t sid);

uint32_t sctp_get_forward_tsn_sent(const Sctp* sctp);

uint32_t sctp_get_sack_gap_reports(const Sctp* sctp);

uint32_t sctp_get_data_retransmissions(const Sctp* sctp);

uint32_t sctp_get_data_retry_exhaustions(const Sctp* sctp);

int sctp_peer_supports_forward_tsn(const Sctp* sctp);

void sctp_set_send_packet_callback(Sctp* sctp,
                                   SctpSendPacketCallback callback,
                                   void* user_data);

void sctp_incoming_data(Sctp* sctp, char* buf, size_t len);

int sctp_outgoing_data(Sctp* sctp, char* buf, size_t len, SctpDataPpid ppid, uint16_t sid);

void sctp_onmessage(Sctp* sctp, void (*onmessage)(char* msg, size_t len, void* userdata, uint16_t sid));

void sctp_onopen(Sctp* sctp, void (*onopen)(void* userdata));

void sctp_onclose(Sctp* sctp, void (*onclose)(void* userdata));

#endif  // SCTP_H_
