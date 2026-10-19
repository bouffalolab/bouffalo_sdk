#include <stdlib.h>
#include <string.h>

#include "dtls_srtp.h"
#include "ports.h"
#include "sctp.h"
#include "utils.h"
#if CONFIG_USE_USRSCTP
#include <usrsctp.h>
#endif

static const uint32_t crc32c_table[256] = {
    0x00000000L, 0xF26B8303L, 0xE13B70F7L, 0x1350F3F4L,
    0xC79A971FL, 0x35F1141CL, 0x26A1E7E8L, 0xD4CA64EBL,
    0x8AD958CFL, 0x78B2DBCCL, 0x6BE22838L, 0x9989AB3BL,
    0x4D43CFD0L, 0xBF284CD3L, 0xAC78BF27L, 0x5E133C24L,
    0x105EC76FL, 0xE235446CL, 0xF165B798L, 0x030E349BL,
    0xD7C45070L, 0x25AFD373L, 0x36FF2087L, 0xC494A384L,
    0x9A879FA0L, 0x68EC1CA3L, 0x7BBCEF57L, 0x89D76C54L,
    0x5D1D08BFL, 0xAF768BBCL, 0xBC267848L, 0x4E4DFB4BL,
    0x20BD8EDEL, 0xD2D60DDDL, 0xC186FE29L, 0x33ED7D2AL,
    0xE72719C1L, 0x154C9AC2L, 0x061C6936L, 0xF477EA35L,
    0xAA64D611L, 0x580F5512L, 0x4B5FA6E6L, 0xB93425E5L,
    0x6DFE410EL, 0x9F95C20DL, 0x8CC531F9L, 0x7EAEB2FAL,
    0x30E349B1L, 0xC288CAB2L, 0xD1D83946L, 0x23B3BA45L,
    0xF779DEAEL, 0x05125DADL, 0x1642AE59L, 0xE4292D5AL,
    0xBA3A117EL, 0x4851927DL, 0x5B016189L, 0xA96AE28AL,
    0x7DA08661L, 0x8FCB0562L, 0x9C9BF696L, 0x6EF07595L,
    0x417B1DBCL, 0xB3109EBFL, 0xA0406D4BL, 0x522BEE48L,
    0x86E18AA3L, 0x748A09A0L, 0x67DAFA54L, 0x95B17957L,
    0xCBA24573L, 0x39C9C670L, 0x2A993584L, 0xD8F2B687L,
    0x0C38D26CL, 0xFE53516FL, 0xED03A29BL, 0x1F682198L,
    0x5125DAD3L, 0xA34E59D0L, 0xB01EAA24L, 0x42752927L,
    0x96BF4DCCL, 0x64D4CECFL, 0x77843D3BL, 0x85EFBE38L,
    0xDBFC821CL, 0x2997011FL, 0x3AC7F2EBL, 0xC8AC71E8L,
    0x1C661503L, 0xEE0D9600L, 0xFD5D65F4L, 0x0F36E6F7L,
    0x61C69362L, 0x93AD1061L, 0x80FDE395L, 0x72966096L,
    0xA65C047DL, 0x5437877EL, 0x4767748AL, 0xB50CF789L,
    0xEB1FCBADL, 0x197448AEL, 0x0A24BB5AL, 0xF84F3859L,
    0x2C855CB2L, 0xDEEEDFB1L, 0xCDBE2C45L, 0x3FD5AF46L,
    0x7198540DL, 0x83F3D70EL, 0x90A324FAL, 0x62C8A7F9L,
    0xB602C312L, 0x44694011L, 0x5739B3E5L, 0xA55230E6L,
    0xFB410CC2L, 0x092A8FC1L, 0x1A7A7C35L, 0xE811FF36L,
    0x3CDB9BDDL, 0xCEB018DEL, 0xDDE0EB2AL, 0x2F8B6829L,
    0x82F63B78L, 0x709DB87BL, 0x63CD4B8FL, 0x91A6C88CL,
    0x456CAC67L, 0xB7072F64L, 0xA457DC90L, 0x563C5F93L,
    0x082F63B7L, 0xFA44E0B4L, 0xE9141340L, 0x1B7F9043L,
    0xCFB5F4A8L, 0x3DDE77ABL, 0x2E8E845FL, 0xDCE5075CL,
    0x92A8FC17L, 0x60C37F14L, 0x73938CE0L, 0x81F80FE3L,
    0x55326B08L, 0xA759E80BL, 0xB4091BFFL, 0x466298FCL,
    0x1871A4D8L, 0xEA1A27DBL, 0xF94AD42FL, 0x0B21572CL,
    0xDFEB33C7L, 0x2D80B0C4L, 0x3ED04330L, 0xCCBBC033L,
    0xA24BB5A6L, 0x502036A5L, 0x4370C551L, 0xB11B4652L,
    0x65D122B9L, 0x97BAA1BAL, 0x84EA524EL, 0x7681D14DL,
    0x2892ED69L, 0xDAF96E6AL, 0xC9A99D9EL, 0x3BC21E9DL,
    0xEF087A76L, 0x1D63F975L, 0x0E330A81L, 0xFC588982L,
    0xB21572C9L, 0x407EF1CAL, 0x532E023EL, 0xA145813DL,
    0x758FE5D6L, 0x87E466D5L, 0x94B49521L, 0x66DF1622L,
    0x38CC2A06L, 0xCAA7A905L, 0xD9F75AF1L, 0x2B9CD9F2L,
    0xFF56BD19L, 0x0D3D3E1AL, 0x1E6DCDEEL, 0xEC064EEDL,
    0xC38D26C4L, 0x31E6A5C7L, 0x22B65633L, 0xD0DDD530L,
    0x0417B1DBL, 0xF67C32D8L, 0xE52CC12CL, 0x1747422FL,
    0x49547E0BL, 0xBB3FFD08L, 0xA86F0EFCL, 0x5A048DFFL,
    0x8ECEE914L, 0x7CA56A17L, 0x6FF599E3L, 0x9D9E1AE0L,
    0xD3D3E1ABL, 0x21B862A8L, 0x32E8915CL, 0xC083125FL,
    0x144976B4L, 0xE622F5B7L, 0xF5720643L, 0x07198540L,
    0x590AB964L, 0xAB613A67L, 0xB831C993L, 0x4A5A4A90L,
    0x9E902E7BL, 0x6CFBAD78L, 0x7FAB5E8CL, 0x8DC0DD8FL,
    0xE330A81AL, 0x115B2B19L, 0x020BD8EDL, 0xF0605BEEL,
    0x24AA3F05L, 0xD6C1BC06L, 0xC5914FF2L, 0x37FACCF1L,
    0x69E9F0D5L, 0x9B8273D6L, 0x88D28022L, 0x7AB90321L,
    0xAE7367CAL, 0x5C18E4C9L, 0x4F48173DL, 0xBD23943EL,
    0xF36E6F75L, 0x0105EC76L, 0x12551F82L, 0xE03E9C81L,
    0x34F4F86AL, 0xC69F7B69L, 0xD5CF889DL, 0x27A40B9EL,
    0x79B737BAL, 0x8BDCB4B9L, 0x988C474DL, 0x6AE7C44EL,
    0xBE2DA0A5L, 0x4C4623A6L, 0x5F16D052L, 0xAD7D5351L};

uint32_t crc32c(uint32_t crc, const uint8_t* data, unsigned int length) {
  while (length--) {
    crc = crc32c_table[(crc ^ *data++) & 0xFFL] ^ (crc >> 8);
  }
  return crc ^ 0xffffffff;
}

static uint32_t sctp_get_checksum(Sctp* sctp, const uint8_t* buf, size_t len) {
  (void)sctp;
  uint32_t crc = crc32c(0xffffffff, buf, len);
  return crc;
}

static int sctp_send_packet(Sctp* sctp, const uint8_t* packet,
                            size_t packet_size) {
  if (sctp->send_packet) {
    return sctp->send_packet(sctp->send_packet_user_data,
                             packet, packet_size);
  }
  if (!sctp->dtls_srtp) {
    return -1;
  }
  return dtls_srtp_write(sctp->dtls_srtp, packet, packet_size);
}

#if !CONFIG_USE_USRSCTP
#define SCTP_TX_WINDOW ((unsigned)CONFIG_SCTP_TX_WINDOW)
#define SCTP_RX_WINDOW ((unsigned)CONFIG_SCTP_RX_WINDOW)
#define SCTP_RETRY_MS ((unsigned)CONFIG_SCTP_DATA_RETRY_MS)
#define SCTP_RETRY_LIMIT ((unsigned)CONFIG_SCTP_DATA_RETRY_LIMIT)

typedef struct SctpTxPacket {
  struct SctpTxPacket *next;
  uint32_t tsn;
  uint32_t sent_at;
  unsigned int retries;
  size_t length;
  uint8_t data[];
} SctpTxPacket;

typedef struct SctpRxChunk {
  struct SctpRxChunk *next;
  uint32_t tsn;
  size_t length;
  size_t capacity;
  int forward_ready;
  int discarded;
  uint8_t data[];
} SctpRxChunk;

static int sctp_drain_rx(Sctp* sctp);
static void sctp_compact_rx_prefix(Sctp* sctp);
static size_t sctp_build_sack(Sctp* sctp);

#define SCTP_RETRY_BASE_MS 500U
#define SCTP_RETRY_MAX_DELAY_MS 4000U
#define SCTP_RETRY_MAX_RETRANSMISSIONS 5U
#define SCTP_RETRY_DEADLINE_MS 20000U

static void sctp_retry_clear(Sctp* sctp) {
  sctp->retry_packet.kind = SCTP_RETRY_NONE;
  sctp->retry_packet.size = 0;
}

static void sctp_retry_clear_handshake(Sctp* sctp) {
  if (sctp->retry_packet.kind == SCTP_RETRY_INIT ||
      sctp->retry_packet.kind == SCTP_RETRY_COOKIE_ECHO) {
    sctp_retry_clear(sctp);
  }
}

static int sctp_retry_store(Sctp* sctp, SctpRetryKind kind,
                            const uint8_t* packet, size_t size,
                            uint16_t sid) {
  SctpRetryPacket* retry = &sctp->retry_packet;

  if (size > sizeof(retry->packet) ||
      (retry->kind != SCTP_RETRY_NONE && kind == SCTP_RETRY_DCEP_OPEN)) {
    return -1;
  }
  memcpy(retry->packet, packet, size);
  retry->size = size;
  retry->started_ms = ports_get_epoch_time();
  retry->last_sent_ms = retry->started_ms;
  retry->sid = sid;
  retry->retries = 0;
  retry->kind = kind;
  return 0;
}
#endif

void sctp_poll(Sctp* sctp, uint32_t now_ms) {
#if CONFIG_USE_USRSCTP
  (void)sctp;
  (void)now_ms;
#else
  SctpRetryPacket* retry;
  uint32_t delay_ms;

  if (!sctp) {
    return;
  }
  if (sctp->connected && sctp->rx_head) {
    unsigned int queued = sctp->rx_count;
    sctp_compact_rx_prefix(sctp);
    if (sctp_drain_rx(sctp) < 0) {
      sctp_destroy_association(sctp);
      if (sctp->onclose) sctp->onclose(sctp->userdata);
      return;
    }
    if (sctp->connected && sctp->rx_count < queued) sctp->rx_sack_pending = 1;
  }
  if (sctp->connected && sctp->rx_sack_pending) {
    size_t length = sctp_build_sack(sctp);
    SctpHeader* header = (SctpHeader*)sctp->buf;
    header->source_port = htons(sctp->local_port);
    header->destination_port = htons(sctp->remote_port);
    header->verification_tag = sctp->verification_tag;
    header->checksum = sctp_get_checksum(sctp, sctp->buf, length);
    sctp->rx_sack_pending = 0;
    if (sctp_send_packet(sctp, sctp->buf, length) < 0 && sctp->connected)
      sctp->rx_sack_pending = 1;
  }
  unsigned int budget = 2;
  while (budget--) {
    SctpTxPacket* packet = sctp->tx_head;
    while (packet && (uint32_t)(now_ms - packet->sent_at) < SCTP_RETRY_MS)
      packet = packet->next;
    if (!packet) break;
    if (packet->retries++ >= SCTP_RETRY_LIMIT) {
      sctp->data_retry_exhaustions++;
      sctp_destroy_association(sctp);
      if (sctp->onclose) sctp->onclose(sctp->userdata);
      return;
    }
    packet->sent_at = now_ms;
    sctp->data_retransmissions++;
    /* A synchronous transport hook may retire this copy in its SACK callback. */
    sctp_send_packet(sctp, packet->data, packet->length);
  }
  if (sctp->retry_packet.kind == SCTP_RETRY_NONE) {
    return;
  }
  retry = &sctp->retry_packet;
  if (retry->retries >= SCTP_RETRY_MAX_RETRANSMISSIONS ||
      now_ms - retry->started_ms >= SCTP_RETRY_DEADLINE_MS) {
    int association_failed = retry->kind != SCTP_RETRY_DCEP_OPEN;
    LOGW("SCTP control retry exhausted kind=%u sid=%u",
         (unsigned)retry->kind, retry->sid);
    sctp_retry_clear(sctp);
    /* DCEP failure belongs to one stream. Other streams can already carry
     * traffic even if the peer has not acknowledged this stream. */
    if (association_failed) {
      sctp_destroy_association(sctp);
      if (sctp->onclose) sctp->onclose(sctp->userdata);
    }
    return;
  }
  delay_ms = SCTP_RETRY_BASE_MS << retry->retries;
  if (delay_ms > SCTP_RETRY_MAX_DELAY_MS) {
    delay_ms = SCTP_RETRY_MAX_DELAY_MS;
  }
  if (now_ms - retry->last_sent_ms < delay_ms) {
    return;
  }
  retry->last_sent_ms = now_ms;
  retry->retries++;
  if (sctp_send_packet(sctp, retry->packet, retry->size) < 0) {
    LOGW("SCTP control retry send failed kind=%u attempt=%u",
         (unsigned)retry->kind, (unsigned)retry->retries);
  } else {
    LOGI("SCTP control retransmitted kind=%u attempt=%u",
         (unsigned)retry->kind, (unsigned)retry->retries);
  }
#endif
}

static int sctp_outgoing_data_cb(void* userdata, void* buf, size_t len, uint8_t tos, uint8_t set_df) {
  (void)tos;
  (void)set_df;
  int ret = sctp_send_packet((Sctp*)userdata, buf, len);
  return ret < 0 ? ret : 0;
}

static uint16_t sctp_current_ssn(Sctp* sctp, uint16_t sid) {
  for (int i = 0; i < sctp->stream_count; i++) {
    if (sctp->stream_table[i].sid == sid) {
      return sctp->stream_table[i].next_ssn;
    }
  }

  if (sctp->stream_count < SCTP_MAX_STREAMS) {
    SctpStreamEntry* stream = &sctp->stream_table[sctp->stream_count++];
    stream->label[0] = '\0';
    stream->sid = sid;
    stream->next_ssn = 0;
    stream->rx_next_ssn = 0;
    stream->open = 0;
    stream->unordered = 0;
    stream->drop_on_send_failure = 0;
    return 0;
  }

  LOGW("SCTP stream table full, using fallback SSN for sid=%u", sid);
  return sctp->next_ssn_fallback;
}

static void sctp_commit_ssn(Sctp* sctp, uint16_t sid) {
  for (int i = 0; i < sctp->stream_count; i++) {
    if (sctp->stream_table[i].sid == sid) {
      sctp->stream_table[i].next_ssn++;
      return;
    }
  }

  sctp->next_ssn_fallback++;
}

void sctp_set_stream_unordered(Sctp* sctp, uint16_t sid, int unordered) {
  if (!sctp)
    return;

  for (int i = 0; i < sctp->stream_count; i++) {
    if (sctp->stream_table[i].sid == sid) {
      sctp->stream_table[i].unordered = unordered != 0;
      return;
    }
  }

  if (sctp->stream_count < SCTP_MAX_STREAMS) {
    SctpStreamEntry* stream = &sctp->stream_table[sctp->stream_count++];
    stream->label[0] = '\0';
    stream->sid = sid;
    stream->next_ssn = 0;
    stream->rx_next_ssn = 0;
    stream->open = 0;
    stream->unordered = unordered != 0;
    stream->drop_on_send_failure = 0;
  }
}

void sctp_set_stream_drop_on_send_failure(Sctp* sctp, uint16_t sid,
                                          int drop_on_send_failure) {
  if (!sctp)
    return;

  for (int i = 0; i < sctp->stream_count; i++) {
    if (sctp->stream_table[i].sid == sid) {
      sctp->stream_table[i].drop_on_send_failure =
          drop_on_send_failure != 0;
      return;
    }
  }

  if (sctp->stream_count < SCTP_MAX_STREAMS) {
    SctpStreamEntry* stream = &sctp->stream_table[sctp->stream_count++];
    stream->label[0] = '\0';
    stream->sid = sid;
    stream->next_ssn = 0;
    stream->rx_next_ssn = 0;
    stream->open = 0;
    stream->unordered = 0;
    stream->drop_on_send_failure = drop_on_send_failure != 0;
  }
}

int sctp_stream_drops_on_send_failure(Sctp* sctp, uint16_t sid) {
  if (!sctp)
    return 0;

  for (int i = 0; i < sctp->stream_count; i++) {
    if (sctp->stream_table[i].sid == sid)
      return sctp->stream_table[i].drop_on_send_failure;
  }
  return 0;
}

static int sctp_stream_is_unordered(Sctp* sctp, uint16_t sid) {
  for (int i = 0; i < sctp->stream_count; i++) {
    if (sctp->stream_table[i].sid == sid)
      return sctp->stream_table[i].unordered;
  }
  return 0;
}

#if !CONFIG_USE_USRSCTP
static int sctp_tsn_after(uint32_t left, uint32_t right) {
  return (int32_t)(left - right) > 0;
}

static int sctp_tsn_at_or_before(uint32_t left, uint32_t right) {
  return (int32_t)(left - right) <= 0;
}

static SctpTxHistoryEntry* sctp_find_tx_history(Sctp* sctp, uint32_t tsn) {
  SctpTxHistoryEntry* entry = &sctp->tx_history[tsn % SCTP_TX_HISTORY_SIZE];

  return entry->valid && entry->tsn == tsn ? entry : NULL;
}

static void sctp_record_tx(Sctp* sctp, uint32_t tsn, int abandonable) {
  SctpTxHistoryEntry* entry = &sctp->tx_history[tsn % SCTP_TX_HISTORY_SIZE];

  if (!sctp->tx_abandonable_prefix_valid) {
    sctp->tx_abandonable_prefix_tsn = tsn - 1U;
    sctp->tx_abandonable_prefix_valid = 1;
  }
  if (entry->valid) {
    /* DATA TSNs are recorded in order. Summarize overwritten entries so a
     * delayed SACK can still abandon lost video after any history rotation. */
    sctp->tx_evicted_through_tsn = entry->tsn;
    sctp->tx_evicted_through_valid = 1;
    if (!entry->abandonable) {
      sctp->tx_evicted_reliable_tsn = entry->tsn;
      sctp->tx_evicted_reliable_valid = 1;
    } else if (entry->tsn == sctp->tx_abandonable_prefix_tsn + 1U) {
      sctp->tx_abandonable_prefix_tsn = entry->tsn;
    }
  }
  entry->tsn = tsn;
  entry->valid = 1;
  entry->abandonable = abandonable != 0;
}

static void sctp_retire_tx_through(Sctp* sctp, uint32_t cumulative_tsn_ack) {
  size_t i;

  while (sctp->tx_head && sctp_tsn_at_or_before(sctp->tx_head->tsn, cumulative_tsn_ack)) {
    SctpTxPacket* packet = sctp->tx_head;
    sctp->tx_head = packet->next;
    free(packet);
    sctp->tx_count--;
  }
  if (!sctp->tx_head) sctp->tx_tail = NULL;

  if (!sctp->tx_abandonable_prefix_valid ||
      sctp_tsn_after(cumulative_tsn_ack, sctp->tx_abandonable_prefix_tsn)) {
    sctp->tx_abandonable_prefix_tsn = cumulative_tsn_ack;
    sctp->tx_abandonable_prefix_valid = 1;
  }
  /* The newest evicted reliable TSN is a conservative barrier for all
   * evicted control packets. Once it is acknowledged, the whole evicted
   * suffix is safe, including video overwritten while that ACK was delayed. */
  if (sctp->tx_evicted_through_valid &&
      (!sctp->tx_evicted_reliable_valid ||
       sctp_tsn_at_or_before(sctp->tx_evicted_reliable_tsn,
                             cumulative_tsn_ack)) &&
      sctp_tsn_after(sctp->tx_evicted_through_tsn,
                     sctp->tx_abandonable_prefix_tsn)) {
    sctp->tx_abandonable_prefix_tsn = sctp->tx_evicted_through_tsn;
  }
  for (i = 0; i < SCTP_TX_HISTORY_SIZE; ++i) {
    SctpTxHistoryEntry* entry = &sctp->tx_history[i];

    if (entry->valid &&
        sctp_tsn_at_or_before(entry->tsn, cumulative_tsn_ack)) {
      entry->valid = 0;
    }
  }
}

static int sctp_send_forward_tsn(Sctp* sctp, uint32_t new_cumulative_tsn) {
  SctpPacket* packet = (SctpPacket*)sctp->buf;
  SctpForwardTsnChunk* chunk = (SctpForwardTsnChunk*)packet->chunks;
  const size_t chunk_size = sizeof(SctpChunkCommon) + sizeof(uint32_t);
  const size_t packet_size = sizeof(SctpHeader) + chunk_size;

  memset(sctp->buf, 0, sizeof(sctp->buf));
  packet->header.source_port = htons(sctp->local_port);
  packet->header.destination_port = htons(sctp->remote_port);
  packet->header.verification_tag = sctp->verification_tag;
  chunk->common.type = SCTP_FORWARD_TSN;
  chunk->common.flags = 0;
  chunk->common.length = htons(chunk_size);
  chunk->new_cumulative_tsn = htonl(new_cumulative_tsn);
  packet->header.checksum = sctp_get_checksum(
      sctp, (const uint8_t*)sctp->buf, packet_size);
  if (sctp_outgoing_data_cb(sctp, sctp->buf, packet_size, 0, 0) < 0) {
    return -1;
  }
  sctp->forward_tsn_sent++;
  LOGW("SCTP gap recovery: FORWARD_TSN=%lu",
       (unsigned long)new_cumulative_tsn);
  return 0;
}

static void sctp_handle_sack(Sctp* sctp, const SctpSackChunk* sack,
                             size_t chunk_len) {
  const SctpGapAckBlock* first_gap;
  uint32_t cumulative_tsn_ack;
  uint32_t forward_tsn;
  uint32_t tsn;
  uint16_t gap_count;
  uint16_t first_gap_start;
  size_t gap_bytes;

  if (chunk_len < sizeof(*sack)) {
    return;
  }
  gap_count = ntohs(sack->number_of_gap_ack_blocks);
  gap_bytes = (size_t)gap_count * sizeof(SctpGapAckBlock);
  if (gap_bytes > chunk_len - sizeof(*sack)) {
    LOGW("Ignoring malformed SCTP SACK gap list");
    return;
  }

  cumulative_tsn_ack = ntohl(sack->cumulative_tsn_ack);
  if (!sctp->tx_cumulative_tsn_ack_valid ||
      sctp_tsn_after(cumulative_tsn_ack, sctp->tx_cumulative_tsn_ack)) {
    sctp->tx_cumulative_tsn_ack = cumulative_tsn_ack;
    sctp->tx_cumulative_tsn_ack_valid = 1;
    sctp_retire_tx_through(sctp, cumulative_tsn_ack);
    if (sctp->last_forward_tsn_valid &&
        sctp_tsn_at_or_before(sctp->last_forward_tsn,
                              cumulative_tsn_ack)) {
      sctp->last_forward_tsn_valid = 0;
      sctp->forward_tsn_repeat_sacks = 0;
    }
  }
  if (gap_count == 0) {
    return;
  }

  sctp->sack_gap_reports++;
  first_gap = (const SctpGapAckBlock*)sack->blocks;
  first_gap_start = ntohs(first_gap->start);
  if (first_gap_start <= 1 || !sctp->peer_supports_forward_tsn) {
    return;
  }

  forward_tsn = cumulative_tsn_ack + first_gap_start - 1U;
  tsn = cumulative_tsn_ack + 1U;
  if (sctp->tx_abandonable_prefix_valid &&
      sctp_tsn_at_or_before(tsn, sctp->tx_abandonable_prefix_tsn)) {
    /* Skip the summarized prefix in constant time; a gap can cover up to
     * 65534 TSNs, while the remaining lookups are bounded by the history. */
    tsn = sctp_tsn_at_or_before(forward_tsn,
                                sctp->tx_abandonable_prefix_tsn)
              ? forward_tsn + 1U : sctp->tx_abandonable_prefix_tsn + 1U;
  }
  for (; sctp_tsn_at_or_before(tsn, forward_tsn); ++tsn) {
    SctpTxHistoryEntry* entry = sctp_find_tx_history(sctp, tsn);

    if (!entry || !entry->abandonable) {
      return;
    }
  }

  if (sctp->last_forward_tsn_valid &&
      sctp->last_forward_tsn == forward_tsn) {
    sctp->forward_tsn_repeat_sacks++;
    if (sctp->forward_tsn_repeat_sacks < 8U) {
      return;
    }
    sctp->forward_tsn_repeat_sacks = 0;
  }
  if (sctp_send_forward_tsn(sctp, forward_tsn) == 0) {
    sctp->last_forward_tsn = forward_tsn;
    sctp->last_forward_tsn_valid = 1;
    sctp->forward_tsn_repeat_sacks = 0;
  } else {
    sctp->last_forward_tsn_valid = 0;
    sctp->forward_tsn_repeat_sacks = 0;
  }
}

static int sctp_init_has_parameter(const SctpInitChunk* init,
                                   size_t chunk_len, uint16_t wanted_type) {
  const uint8_t* cursor;
  const uint8_t* end;

  if (chunk_len < sizeof(*init)) {
    return 0;
  }
  cursor = (const uint8_t*)init + sizeof(*init);
  end = (const uint8_t*)init + chunk_len;
  while (cursor + sizeof(SctpChunkParam) <= end) {
    const SctpChunkParam* param = (const SctpChunkParam*)cursor;
    size_t param_len = ntohs(param->length);

    if (param_len < sizeof(*param) ||
        param_len > (size_t)(end - cursor)) {
      return 0;
    }
    if (ntohs(param->type) == wanted_type) {
      return 1;
    }
    param_len = 4U * ((param_len + 3U) / 4U);
    if (param_len > (size_t)(end - cursor)) {
      return 0;
    }
    cursor += param_len;
  }
  return 0;
}

static size_t sctp_append_forward_tsn_parameter(uint8_t* destination,
                                                size_t capacity) {
  SctpChunkParam* param;

  if (capacity < sizeof(SctpChunkParam)) {
    return 0;
  }
  param = (SctpChunkParam*)destination;
  param->type = htons(SCTP_PARAM_FORWARD_TSN_SUPPORTED);
  param->length = htons(sizeof(*param));
  return sizeof(*param);
}
#endif

int sctp_outgoing_data(Sctp* sctp, char* buf, size_t len, SctpDataPpid ppid, uint16_t sid) {
  size_t original_len = len;
  int abandonable;
  int unordered;

  if (!sctp || !buf || len == 0 ||
      (!sctp->dtls_srtp && !sctp->send_packet)) {
    return -1;
  }
  unordered = ppid != PPID_CONTROL && sctp_stream_is_unordered(sctp, sid);
  abandonable = unordered && sctp_stream_drops_on_send_failure(sctp, sid);
#if CONFIG_USE_USRSCTP
  int res;
  struct sctp_sendv_spa spa = {0};

  spa.sendv_flags = SCTP_SEND_SNDINFO_VALID;

  spa.sendv_sndinfo.snd_sid = sid;
  spa.sendv_sndinfo.snd_flags = SCTP_EOR |
      (unordered ? SCTP_UNORDERED : 0);
  spa.sendv_sndinfo.snd_ppid = htonl(ppid);

  res = usrsctp_sendv(sctp->sock, buf, len, NULL, 0, &spa, sizeof(spa), SCTP_SENDV_SPA, 0);
  if (res < 0) {
    LOGE("sctp sendv error %d: %s", errno, strerror(errno));
  }
  return res;
#else
  const size_t payload_max = SCTP_MTU - sizeof(SctpPacket) - sizeof(SctpDataChunk);
  int retry_open = ppid == PPID_CONTROL && (uint8_t)buf[0] == DATA_CHANNEL_OPEN;
  size_t fragments = (len - 1U) / payload_max + 1U;
  if (retry_open && fragments != 1) return -1;
  if (!retry_open &&
      (len > CONFIG_SCTP_MESSAGE_SIZE || fragments > SCTP_TX_WINDOW)) return -1;
  if (!abandonable && !retry_open &&
      (sctp->tx_count > SCTP_TX_WINDOW ||
       fragments > SCTP_TX_WINDOW - sctp->tx_count)) return -1;

  /* Reserve every fragment before sending any part of a reliable message.
   * Memory/window pressure therefore rejects the entire message atomically. */
  SctpTxPacket* pending = NULL;
  SctpTxPacket** tail = &pending;
  if (!abandonable && !retry_open) {
    for (size_t i = 0; i < fragments; ++i) {
      size_t bytes = len - i * payload_max;
      if (bytes > payload_max) bytes = payload_max;
      size_t packet_size = (sizeof(SctpHeader) + sizeof(SctpDataChunk) + bytes + 3U) & ~(size_t)3U;
      SctpTxPacket* packet = calloc(1, sizeof(*packet) + packet_size);
      if (!packet) {
        while (pending) {
          SctpTxPacket* next = pending->next;
          free(pending);
          pending = next;
        }
        return -1;
      }
      packet->length = packet_size;
      *tail = packet;
      tail = &packet->next;
    }
  }
  uint16_t ssn = sctp_current_ssn(sctp, sid);
  uint32_t first_tsn = sctp->tsn;
  SctpTxPacket* saved = pending;
  size_t offset = 0;
  for (size_t i = 0; i < fragments; ++i) {
    size_t bytes = len - offset;
    if (bytes > payload_max) bytes = payload_max;
    size_t packet_size = (sizeof(SctpHeader) + sizeof(SctpDataChunk) + bytes + 3U) & ~(size_t)3U;
    SctpPacket* packet = (SctpPacket*)(saved ? saved->data : sctp->buf);
    memset(packet, 0, packet_size);
    packet->header.source_port = htons(sctp->local_port);
    packet->header.destination_port = htons(sctp->remote_port);
    packet->header.verification_tag = sctp->verification_tag;
    SctpDataChunk* chunk = (SctpDataChunk*)packet->chunks;
    chunk->type = SCTP_DATA;
    chunk->iube = (i == 0 ? 2 : 0) | (i + 1U == fragments ? 1 : 0) | (unordered ? 4 : 0);
    chunk->length = htons(sizeof(*chunk) + bytes);
    chunk->tsn = htonl(first_tsn + i);
    chunk->sid = htons(sid);
    chunk->sqn = htons(ssn);
    chunk->ppid = htonl(ppid);
    memcpy(chunk->data, buf + offset, bytes);
    packet->header.checksum = sctp_get_checksum(sctp, (uint8_t*)packet, packet_size);
    if (saved) {
      saved->tsn = first_tsn + i;
      saved->sent_at = ports_get_epoch_time();
      sctp_record_tx(sctp, saved->tsn, 0);
      saved = saved->next;
    } else {
      if (retry_open && sctp_retry_store(sctp, SCTP_RETRY_DCEP_OPEN,
                                          (uint8_t*)packet, packet_size, sid) < 0) return -1;
      if (sctp_send_packet(sctp, (uint8_t*)packet, packet_size) < 0 && !retry_open) {
        if (i > 0) sctp_commit_ssn(sctp, sid);
        return i > 0 ? -2 : -1;
      }
      sctp_record_tx(sctp, sctp->tsn++, abandonable);
    }
    offset += bytes;
  }
  sctp_commit_ssn(sctp, sid);
  if (pending) {
    if (sctp->tx_tail) sctp->tx_tail->next = pending;
    else sctp->tx_head = pending;
    sctp->tx_tail = pending;
    while (sctp->tx_tail->next) sctp->tx_tail = sctp->tx_tail->next;
    sctp->tx_count += fragments;
    sctp->tsn += fragments;
    /* Accepted copies remain owned by SCTP until cumulatively acknowledged.
     * A failed first UDP send is retried with the same TSN/SSN and payload. */
    for (size_t i = 0; i < fragments; ++i) {
      saved = sctp->tx_head;
      while (saved && saved->tsn != first_tsn + i) saved = saved->next;
      if (saved) sctp_send_packet(sctp, saved->data, saved->length);
    }

  }

#endif
  return (int)original_len;
}

void sctp_add_stream_mapping(Sctp* sctp, const char* label, uint16_t sid) {
  for (int i = 0; i < sctp->stream_count; i++) {
    if (sctp->stream_table[i].sid == sid) {
      strncpy(sctp->stream_table[i].label, label, sizeof(sctp->stream_table[i].label) - 1);
      sctp->stream_table[i].label[sizeof(sctp->stream_table[i].label) - 1] = '\0';
      return;
    }
  }

  if (sctp->stream_count < SCTP_MAX_STREAMS) {
    strncpy(sctp->stream_table[sctp->stream_count].label, label,
            sizeof(sctp->stream_table[sctp->stream_count].label) - 1);
    sctp->stream_table[sctp->stream_count].label[sizeof(sctp->stream_table[sctp->stream_count].label) - 1] = '\0';
    sctp->stream_table[sctp->stream_count].sid = sid;
    sctp->stream_table[sctp->stream_count].next_ssn = 0;
    sctp->stream_table[sctp->stream_count].rx_next_ssn = 0;
    sctp->stream_table[sctp->stream_count].open = 0;
    sctp->stream_table[sctp->stream_count].unordered = 0;
    sctp->stream_table[sctp->stream_count].drop_on_send_failure = 0;
    sctp->stream_count++;
  } else
    LOGE("Stream table full. Cannot add more streams.");
}

static void sctp_mark_stream_open(Sctp* sctp, uint16_t sid) {
  for (int i = 0; i < sctp->stream_count; i++) {
    if (sctp->stream_table[i].sid == sid) {
      sctp->stream_table[i].open = 1;
      return;
    }
  }

  if (sctp->stream_count < SCTP_MAX_STREAMS) {
    SctpStreamEntry* stream = &sctp->stream_table[sctp->stream_count++];
    stream->label[0] = '\0';
    stream->sid = sid;
    stream->next_ssn = 0;
    stream->rx_next_ssn = 0;
    stream->open = 1;
    stream->unordered = 0;
    stream->drop_on_send_failure = 0;
  }
}

static int sctp_map_data_channel_open(Sctp* sctp, uint16_t sid,
                                      const uint8_t* data, size_t length) {
  uint16_t label_length;
  uint16_t protocol_length;
  uint32_t reliability_parameter;
  char label[sizeof(sctp->stream_table[0].label)];
  size_t copied_label_length;

  if (!sctp || !data || length < 12 || data[0] != DATA_CHANNEL_OPEN)
    return -1;

  memcpy(&label_length, data + 8, sizeof(label_length));
  memcpy(&protocol_length, data + 10, sizeof(protocol_length));
  memcpy(&reliability_parameter, data + 4, sizeof(reliability_parameter));
  label_length = ntohs(label_length);
  protocol_length = ntohs(protocol_length);
  reliability_parameter = ntohl(reliability_parameter);
  if (length < 12 + (size_t)label_length + (size_t)protocol_length)
    return -1;

  copied_label_length = label_length;
  if (copied_label_length >= sizeof(label))
    copied_label_length = sizeof(label) - 1;
  memcpy(label, data + 12, copied_label_length);
  label[copied_label_length] = '\0';
  sctp_add_stream_mapping(sctp, label, sid);
  sctp_set_stream_unordered(sctp, sid, (data[1] & 0x80) != 0);
  sctp_set_stream_drop_on_send_failure(
      sctp, sid, (data[1] & 0x7f) == 0x01 && reliability_parameter == 0);
  sctp_mark_stream_open(sctp, sid);
  return 0;
}

void sctp_parse_data_channel_open(Sctp* sctp, uint16_t sid, char* data, size_t length) {
  if (sctp_map_data_channel_open(sctp, sid, (const uint8_t*)data, length) == 0) {
    char ack = DATA_CHANNEL_ACK;
    sctp_outgoing_data(sctp, &ack, 1, PPID_CONTROL, sid);
  }
}

void sctp_handle_sctp_packet(Sctp* sctp, char* buf, size_t len) {
  uint16_t sid;
  uint32_t ppid;

  if (!sctp || !buf || len < sizeof(SctpHeader) + sizeof(SctpDataChunk) + 1)
    return;

  if (buf[12] != 0)  // if chunk_type is no zero, it's not data
    return;

  memcpy(&sid, buf + 20, sizeof(sid));
  memcpy(&ppid, buf + 24, sizeof(ppid));
  sid = ntohs(sid);
  ppid = ntohl(ppid);

  if (ppid == DATA_CHANNEL_PPID_CONTROL)
    sctp_parse_data_channel_open(sctp, sid, buf + 28, len - 28);
}

#if !CONFIG_USE_USRSCTP
/* TSNs acknowledge reception across the association. SSNs order messages
 * within one stream; an unrelated TSN gap must not block unordered delivery. */
static SctpStreamEntry* sctp_rx_stream(Sctp* sctp, uint16_t sid) {
  for (int i = 0; i < sctp->stream_count; ++i) {
    if (sctp->stream_table[i].sid == sid) return &sctp->stream_table[i];
  }
  if (sctp->stream_count == SCTP_MAX_STREAMS) return NULL;
  SctpStreamEntry* stream = &sctp->stream_table[sctp->stream_count++];
  memset(stream, 0, sizeof(*stream));
  stream->sid = sid;
  return stream;
}

static void sctp_rx_advance(Sctp* sctp) {
  while (sctp->rx_tsn_mask & 1U) {
    ++sctp->rx_tsn;
    sctp->rx_tsn_mask >>= 1;
  }
}

/* Classic DATA fragments belonging to one complete message are consecutive. */
static SctpRxChunk* sctp_rx_message_end(SctpRxChunk* first, size_t* size,
                                       SctpRxChunk** tail) {
  const SctpDataChunk* begin = (const SctpDataChunk*)first->data;
  const SctpDataChunk* end = begin;
  SctpRxChunk* last = first;
  *size = first->discarded ? CONFIG_SCTP_MESSAGE_SIZE + 1U :
                            first->length - sizeof(*begin);
  if (tail) *tail = first;
  if (!(begin->iube & 2)) return NULL;
  while (!(end->iube & 1)) {
    SctpRxChunk* next = last->next;
    if (!next || next->tsn != last->tsn + 1U) break;
    const SctpDataChunk* part = (const SctpDataChunk*)next->data;
    if (part->sid != begin->sid || part->sqn != begin->sqn ||
        part->ppid != begin->ppid || (part->iube & 2) ||
        (part->iube & 4) != (begin->iube & 4)) break;
    *size += next->length - sizeof(*part);
    last = next;
    end = part;
  }
  if (tail) *tail = last;
  return (end->iube & 1) ? last : NULL;
}

/* A peer may split a supported message into more DATA chunks than our slot
 * count (for example after a path-MTU change). Keep its cumulatively ACKed,
 * contiguous prefix in one bounded allocation so the receive window can
 * reopen before E arrives. Never discard acknowledged bytes on allocation
 * failure, merge gaps, or compact a message awaiting an earlier stream SSN. */
static void sctp_compact_rx_prefix(Sctp* sctp) {
  if (sctp->rx_count < SCTP_RX_WINDOW || !sctp->rx_head) return;
  SctpRxChunk* first = sctp->rx_head;
  const SctpDataChunk* begin = (const SctpDataChunk*)first->data;
  SctpStreamEntry* stream = sctp_rx_stream(sctp, ntohs(begin->sid));
  if (!stream || first->discarded || !(begin->iube & 2) ||
      (!(begin->iube & 4) && ntohs(begin->sqn) != stream->rx_next_ssn)) return;
  size_t size;
  SctpRxChunk* tail;
  if (sctp_rx_message_end(first, &size, &tail) || tail == first ||
      size > CONFIG_SCTP_MESSAGE_SIZE || sctp_tsn_after(tail->tsn, sctp->rx_tsn)) return;
  uint32_t last_tsn = tail->tsn;
  size_t required = sizeof(*begin) + size;
  SctpRxChunk* merged = first;
  if (first->capacity < required) {
    /* Only this acknowledged head prefix grows. Geometric capacity bounds
     * aggregate copying to O(message size), even with a two-slot window.
     * Slack is < one message; never reserve a full message for ordinary DATA. */
    size_t capacity = first->capacity;
    size_t maximum = sizeof(*begin) + CONFIG_SCTP_MESSAGE_SIZE;
    while (capacity < required) {
      capacity = capacity > maximum / 2 ? maximum : capacity * 2;
    }
    merged = malloc(sizeof(*merged) + capacity);
    if (!merged) return;
    memcpy(merged, first, sizeof(*merged) + first->length);
    merged->capacity = capacity;
  }
  size_t copied = first->length;
  SctpRxChunk* part = first->next;
  SctpRxChunk* after = tail->next;
  while (part != after) {
    SctpRxChunk* next = part->next;
    size_t bytes = part->length - sizeof(*begin);
    memcpy(merged->data + copied, part->data + sizeof(*begin), bytes);
    copied += bytes;
    free(part);
    --sctp->rx_count;
    part = next;
  }
  if (merged != first) free(first);
  merged->next = after;
  merged->tsn = last_tsn;
  ((SctpDataChunk*)merged->data)->tsn = htonl(last_tsn);
  /* The prefix size lives in size_t metadata, not a 16-bit wire length.
   * Retain the original DATA header length; this storage is never sent. */
  merged->length = required;
  sctp->rx_head = merged;
}

static size_t sctp_rx_byte_limit(void) {
  size_t slots = SCTP_RX_WINDOW * (size_t)CONFIG_MTU;
  size_t message = CONFIG_SCTP_MESSAGE_SIZE + SCTP_RX_WINDOW * sizeof(SctpDataChunk);
  return slots > message ? slots : message;
}

static size_t sctp_rx_bytes(const Sctp* sctp) {
  size_t bytes = 0;
  for (const SctpRxChunk* chunk = sctp->rx_head; chunk; chunk = chunk->next)
    bytes += chunk->length;
  return bytes;
}

static int sctp_drain_rx(Sctp* sctp) {
  SctpRxChunk** position = &sctp->rx_head;
  while (*position) {
    SctpRxChunk* first = *position;
    const SctpDataChunk* begin = (const SctpDataChunk*)first->data;
    SctpStreamEntry* stream = sctp_rx_stream(sctp, ntohs(begin->sid));
    int unordered = (begin->iube & 4) != 0;
    uint16_t ssn = ntohs(begin->sqn);
    if (!stream) {
      /* Exceeding the bounded stream table must not close healthy channels. */
      if (begin->iube & 2) LOGW("Dropping DATA for unsupported SID %u", ntohs(begin->sid));
      *position = first->next;
      free(first);
      --sctp->rx_count;
      continue;
    }
    if (!(begin->iube & 2) ||
        (!unordered && ssn != stream->rx_next_ssn && !first->forward_ready)) {
      position = &first->next;
      continue;
    }
    size_t size;
    SctpRxChunk* tail;
    SctpRxChunk* last = sctp_rx_message_end(first, &size, &tail);
    int discard = size > CONFIG_SCTP_MESSAGE_SIZE;
    if (discard && !first->discarded)
      LOGW("Dropping oversized DATA message on SID %u", ntohs(begin->sid));
    if (discard && !last) {
      /* Retain one header as a discard marker until E arrives. Collapsing
       * each contiguous prefix reopens the RX window even for messages much
       * larger than that window, without ever delivering a truncated body. */
      SctpDataChunk marker;
      memcpy(&marker, begin, sizeof(marker));
      int forward_ready = first->forward_ready;
      while (first != tail) {
        SctpRxChunk* next = first->next;
        free(first);
        --sctp->rx_count;
        first = next;
      }
      marker.tsn = htonl(tail->tsn);
      memcpy(tail->data, &marker, sizeof(marker));
      tail->length = sizeof(marker);
      tail->discarded = 1;
      tail->forward_ready = forward_ready;
      *position = tail;
      position = &tail->next;
      continue;
    }
    if (!last) {
      position = &first->next;
      continue;
    }
    char* message = discard ? NULL : malloc(size + 1);
    if (!discard && !message) return 0; /* Keep acknowledged payload until memory recovers. */
    size_t copied = 0;
    uint32_t ppid = ntohl(begin->ppid);
    uint16_t sid = ntohs(begin->sid);
    SctpRxChunk* after = last->next;
    while (first != after) {
      SctpRxChunk* next = first->next;
      const SctpDataChunk* part = (const SctpDataChunk*)first->data;
      size_t bytes = first->length - sizeof(*part);
      if (message) memcpy(message + copied, part->data, bytes);
      copied += bytes;
      free(first);
      --sctp->rx_count;
      first = next;
    }
    *position = after;
    uint16_t next_ssn = ssn + 1U;
    if (!unordered && (int16_t)(next_ssn - stream->rx_next_ssn) > 0)
      stream->rx_next_ssn = next_ssn;
    if (discard) {
      position = &sctp->rx_head;
      continue;
    }
    message[size] = '\0';
    if (ppid == DATA_CHANNEL_PPID_CONTROL && size &&
        (uint8_t)message[0] == DATA_CHANNEL_OPEN) {
      sctp_parse_data_channel_open(sctp, sid, message, size);
    } else if (ppid == DATA_CHANNEL_PPID_CONTROL && size == 1 &&
               (uint8_t)message[0] == DATA_CHANNEL_ACK) {
      sctp_mark_stream_open(sctp, sid);
      if (sctp->retry_packet.kind == SCTP_RETRY_DCEP_OPEN &&
          sctp->retry_packet.sid == sid) sctp_retry_clear(sctp);
    } else if ((ppid == DATA_CHANNEL_PPID_DOMSTRING ||
                ppid == DATA_CHANNEL_PPID_BINARY) && sctp->onmessage) {
      sctp->onmessage(message, size, sctp->userdata, sid);
    }
    free(message);
    position = &sctp->rx_head;
  }
  return 0;
}

static int sctp_receive_data(Sctp* sctp, const SctpDataChunk* chunk, size_t length) {
  uint32_t tsn = ntohl(chunk->tsn);
  if (!sctp->rx_tsn_valid) {
    sctp->rx_tsn = tsn - 1U;
    sctp->rx_tsn_valid = 1;
  }
  int32_t distance = (int32_t)(tsn - sctp->rx_tsn);
  if (distance <= 0 || (uint32_t)distance > SCTP_RX_WINDOW) return 0;
  uint32_t bit = 1U << (distance - 1);
  if (sctp->rx_tsn_mask & bit) return 0;
  if (length > CONFIG_MTU) return 0;
  sctp_compact_rx_prefix(sctp);
  size_t buffered = sctp_rx_bytes(sctp);
  size_t byte_limit = sctp_rx_byte_limit();
  while (sctp->rx_count >= SCTP_RX_WINDOW || buffered > byte_limit ||
         length > byte_limit - buffered) {
    if (distance != 1) return 0;
    /* Reneging a gap ACK is allowed; cumulative ACKs must never be retracted.
     * Replace the farthest gap fragment to admit a cumulative-gap repair,
     * including consecutive repairs that cannot complete the message yet. */
    SctpRxChunk** farthest = &sctp->rx_head;
    while (*farthest && (*farthest)->next) farthest = &(*farthest)->next;
    SctpRxChunk* victim = *farthest;
    if (!victim || !sctp_tsn_after(victim->tsn, tsn)) return 0;
    uint32_t gap = victim->tsn - sctp->rx_tsn;
    if (gap <= 1 || gap > SCTP_RX_WINDOW) return 0;
    sctp->rx_tsn_mask &= ~(1U << (gap - 1U));
    *farthest = NULL;
    buffered -= victim->length;
    free(victim);
    --sctp->rx_count;
  }
  SctpRxChunk* saved = malloc(sizeof(*saved) + length);
  if (!saved) return 0;
  saved->tsn = tsn;
  saved->length = length;
  saved->capacity = length;
  saved->forward_ready = 0;
  saved->discarded = 0;
  memcpy(saved->data, chunk, length);
  SctpRxChunk** position = &sctp->rx_head;
  while (*position && (int32_t)(tsn - (*position)->tsn) > 0) position = &(*position)->next;
  saved->next = *position;
  *position = saved;
  ++sctp->rx_count;
  sctp->rx_tsn_mask |= bit;
  sctp_rx_advance(sctp);
  return sctp_drain_rx(sctp);
}

static size_t sctp_build_sack(Sctp* sctp) {
  SctpSackChunk* sack = (SctpSackChunk*)((SctpPacket*)sctp->buf)->chunks;
  memset(sctp->buf, 0, sizeof(sctp->buf));
  sack->common.type = SCTP_SACK;
  sack->cumulative_tsn_ack = htonl(sctp->rx_tsn);
  size_t available = (SCTP_RX_WINDOW - sctp->rx_count) * CONFIG_MTU;
  size_t buffered = sctp_rx_bytes(sctp);
  size_t limit = sctp_rx_byte_limit();
  size_t byte_space = buffered < limit ? limit - buffered : 0;
  if (available > byte_space) available = byte_space;
  sack->a_rwnd = htonl((uint32_t)available);
  SctpGapAckBlock* blocks = (SctpGapAckBlock*)sack->blocks;
  unsigned count = 0;
  for (unsigned i = 0; i < SCTP_RX_WINDOW; ++i) {
    if (!(sctp->rx_tsn_mask & (1U << i))) continue;
    blocks[count].start = htons(i + 1U);
    while (i + 1U < SCTP_RX_WINDOW && (sctp->rx_tsn_mask & (1U << (i + 1U)))) ++i;
    blocks[count++].end = htons(i + 1U);
  }
  size_t size = sizeof(*sack) + count * sizeof(*blocks);
  sack->number_of_gap_ack_blocks = htons(count);
  sack->common.length = htons(size);
  return sizeof(SctpHeader) + size;
}

static int sctp_receive_forward_tsn(Sctp* sctp, const SctpForwardTsnChunk* chunk,
                                    size_t length) {
  if (length < sizeof(*chunk) || (length - sizeof(*chunk)) % 4U) return -1;
  uint32_t tsn = ntohl(chunk->new_cumulative_tsn);
  if (!sctp->rx_tsn_valid || !sctp_tsn_after(tsn, sctp->rx_tsn)) return 0;
  uint32_t advance = tsn - sctp->rx_tsn;
  sctp->rx_tsn_mask = advance >= SCTP_RX_WINDOW ? 0 : sctp->rx_tsn_mask >> advance;
  sctp->rx_tsn = tsn;
  /* Drop incomplete skipped messages. If a future orphan is discarded,
   * retract its gap ACK so a reliable peer can retransmit that fragment. */
  SctpRxChunk** position = &sctp->rx_head;
  int keep_parts = 0;
  uint16_t keep_sid = 0, keep_ssn = 0;
  uint32_t keep_ppid = 0;
  while (*position) {
    SctpRxChunk* saved = *position;
    const SctpDataChunk* data = (const SctpDataChunk*)saved->data;
    size_t message_size;
    SctpRxChunk* complete = sctp_rx_message_end(saved, &message_size, NULL);
    if (complete) {
      /* Complete stranded messages survive TSN advancement. The stream list
       * also releases completed SSNs at/below its skip point (RFC 3758 3.6). */
      for (size_t i = sizeof(*chunk); i < length; i += 4) {
        uint16_t pair[2];
        memcpy(pair, (const uint8_t*)chunk + i, sizeof(pair));
        if (data->sid == pair[0] &&
            (int16_t)(ntohs(data->sqn) - ntohs(pair[1])) <= 0)
          saved->forward_ready = 1;
      }
      position = &complete->next;
      keep_parts = 0;
      continue;
    }
    int keep = sctp_tsn_after(saved->tsn, tsn) &&
        ((data->iube & 2) || (keep_parts && data->sid == keep_sid &&
          data->sqn == keep_ssn && data->ppid == keep_ppid));
    if (keep) {
      keep_sid = data->sid;
      keep_ssn = data->sqn;
      keep_ppid = data->ppid;
      keep_parts = !(data->iube & 1);
      position = &saved->next;
      continue;
    }
    uint32_t distance = saved->tsn - tsn;
    if (distance > 0 && distance <= SCTP_RX_WINDOW)
      sctp->rx_tsn_mask &= ~(1U << (distance - 1U));
    *position = saved->next;
    free(saved);
    --sctp->rx_count;
  }
  for (size_t i = sizeof(*chunk); i < length; i += 4) {
    uint16_t pair[2];
    memcpy(pair, (const uint8_t*)chunk + i, sizeof(pair));
    SctpStreamEntry* stream = sctp_rx_stream(sctp, ntohs(pair[0]));
    uint16_t next = ntohs(pair[1]) + 1U;
    if (stream && (int16_t)(next - stream->rx_next_ssn) > 0) stream->rx_next_ssn = next;
  }
  sctp_rx_advance(sctp);
  return sctp_drain_rx(sctp);
}
#endif

void sctp_incoming_data(Sctp* sctp, char* buf, size_t len) {
  if (!sctp || !buf || len < sizeof(SctpHeader) + sizeof(SctpChunkCommon))
    return;

#if CONFIG_USE_USRSCTP
  sctp_handle_sctp_packet(sctp, buf, len);
  usrsctp_conninput(sctp, buf, len, 0);
#else
  size_t length = 0;
  size_t pos = sizeof(SctpHeader);
  int stop_processing = 0;
  SctpChunkCommon* chunk_common;
  SctpPacket* in_packet = (SctpPacket*)buf;
  SctpPacket* out_packet = (SctpPacket*)sctp->buf;

  // Header
#if 0
  LOGD("source_port %d", ntohs(in_packet->header.source_port));
  LOGD("destination_port %d", ntohs(in_packet->header.destination_port));
  LOGD("verification_tag %ld", ntohl(in_packet->header.verification_tag));
  LOGD("checksum %d", ntohs(in_packet->header.checksum));
#endif
  uint32_t crc32c = in_packet->header.checksum;

  in_packet->header.checksum = 0;

  if (crc32c != sctp_get_checksum(sctp, (const uint8_t*)buf, len)) {
    LOGE("checksum error");
    return;
  }

  // prepare outgoing packet
  memset(sctp->buf, 0, sizeof(sctp->buf));
  while (pos + sizeof(SctpChunkCommon) <= len) {
    size_t chunk_len;
    size_t padded_chunk_len;

    chunk_common = (SctpChunkCommon*)(buf + pos);
    chunk_len = ntohs(chunk_common->length);
    if (chunk_len < sizeof(SctpChunkCommon) || chunk_len > len - pos) {
      LOGE("invalid SCTP chunk length %zu", chunk_len);
      return;
    }
    padded_chunk_len = 4 * ((chunk_len + 3) / 4);
    if (padded_chunk_len > len - pos) {
      LOGE("truncated SCTP chunk padding");
      return;
    }

    memset(sctp->buf, 0, sizeof(sctp->buf));
    length = 0;
    stop_processing = 0;
    int notify_open = 0;

    switch (chunk_common->type) {
      case SCTP_DATA: {
        const SctpDataChunk* data_chunk = (const SctpDataChunk*)(buf + pos);
        if (chunk_len < sizeof(*data_chunk)) return;
        if (sctp_receive_data(sctp, data_chunk, chunk_len) < 0) {
          sctp_destroy_association(sctp);
          if (sctp->onclose) sctp->onclose(sctp->userdata);
          return;
        }
        length = sctp_build_sack(sctp);
      } break;
      case SCTP_FORWARD_TSN:
        /* Reject malformed input before changing receive state. Errors from
         * delivery still require the same fatal cleanup as the DATA path. */
        if (chunk_len < sizeof(SctpForwardTsnChunk) ||
            (chunk_len - sizeof(SctpForwardTsnChunk)) % 4U) return;
        if (sctp_receive_forward_tsn(sctp, (const SctpForwardTsnChunk*)chunk_common,
                                      chunk_len) < 0) {
          sctp_destroy_association(sctp);
          if (sctp->onclose) sctp->onclose(sctp->userdata);
          return;
        }
        length = sctp_build_sack(sctp);
        break;
      case SCTP_INIT: {
        LOGI("SCTP_INIT");

        SctpInitChunk* init_chunk = (SctpInitChunk*)(buf + pos);
        if (chunk_len < sizeof(SctpInitChunk)) {
          return;
        }
        sctp->peer_supports_forward_tsn = sctp_init_has_parameter(
            init_chunk, chunk_len, SCTP_PARAM_FORWARD_TSN_SUPPORTED);
        sctp->verification_tag = init_chunk->initiate_tag;
        sctp->cookie_issued = 1;
        if (!sctp->rx_tsn_valid) {
          sctp->rx_tsn = ntohl(init_chunk->initial_tsn) - 1U;
          sctp->rx_tsn_valid = 1;
        }

        SctpInitChunk* init_ack = (SctpInitChunk*)out_packet->chunks;
        init_ack->common.type = SCTP_INIT_ACK;
        init_ack->common.flags = 0x00;
        init_ack->common.length = htons(sizeof(SctpInitChunk) + 8);
        init_ack->initiate_tag = htonl(0x12345678);
        init_ack->a_rwnd = htonl(0x100000);
        init_ack->number_of_outbound_streams = 0xffff;
        init_ack->number_of_inbound_streams = 0xffff;
        init_ack->initial_tsn = htonl(sctp->tsn);

        SctpChunkParam* param = init_ack->param;

        param->type = htons(SCTP_PARAM_STATE_COOKIE);
        param->length = htons(8);
        uint32_t cookie_value = htonl(0x02);
        memcpy(param->value, &cookie_value, sizeof(cookie_value));
        sctp_append_forward_tsn_parameter(
            (uint8_t*)param + ntohs(param->length),
            sizeof(sctp->buf) - sizeof(SctpHeader) -
                sizeof(SctpInitChunk) - ntohs(param->length));
        init_ack->common.length = htons(
            ntohs(init_ack->common.length) + sizeof(SctpChunkParam));
        length = ntohs(init_ack->common.length) + sizeof(SctpHeader);

      } break;
      case SCTP_INIT_ACK: {
        LOGI("SCTP_INIT_ACK");
        SctpInitChunk* init_ack = (SctpInitChunk*)(buf + pos);
        SctpCookieEchoChunk* cookie_echo = (SctpCookieEchoChunk*)out_packet->chunks;
        SctpChunkParam* param = NULL;
        size_t init_ack_len = ntohs(init_ack->common.length);
        uint8_t* cookie;
        uint8_t* cookie_end;

        if (init_ack_len < sizeof(SctpInitChunk) || init_ack_len != chunk_len) {
          LOGE("invalid SCTP_INIT_ACK length %zu", init_ack_len);
          length = 0;
          pos = len;
          break;
        }
        sctp->verification_tag = init_ack->initiate_tag;
        if (!sctp->rx_tsn_valid) {
          sctp->rx_tsn = ntohl(init_ack->initial_tsn) - 1U;
          sctp->rx_tsn_valid = 1;
        }
        sctp->peer_supports_forward_tsn = sctp_init_has_parameter(
            init_ack, chunk_len, SCTP_PARAM_FORWARD_TSN_SUPPORTED);
        cookie = (uint8_t*)&init_ack->param[0];
        cookie_end = (uint8_t*)init_ack + init_ack_len;
        while (cookie + sizeof(SctpChunkParam) <= cookie_end) {
          SctpChunkParam* candidate = (SctpChunkParam*)cookie;
          size_t param_len = ntohs(candidate->length);

          if (param_len < sizeof(SctpChunkParam) || cookie + param_len > cookie_end) {
            break;
          }
          if (ntohs(candidate->type) == SCTP_PARAM_STATE_COOKIE) {
            param = candidate;
            break;
          }
          cookie += 4 * ((param_len + 3) / 4);
        }

        if (!param) {
          LOGE("SCTP_INIT_ACK has no valid state cookie");
          length = 0;
          pos = len;
          break;
        }

        if (ntohs(param->length) > sizeof(sctp->buf) - sizeof(SctpHeader)) {
          LOGE("SCTP state cookie is too large");
          return;
        }

        cookie_echo->common.type = SCTP_COOKIE_ECHO;
        cookie_echo->common.flags = 0x00;
        // cookie echo: type + flag + length (4 bytes) + cookie
        cookie_echo->common.length = htons(ntohs(param->length));
        // param: type + length (4 bytes) + cookie
        memcpy(cookie_echo->cookie, param->value, ntohs(param->length) - 4);
        length = ntohs(cookie_echo->common.length) + sizeof(SctpHeader);

      } break;
      case SCTP_SACK:
        sctp_handle_sack(sctp, (const SctpSackChunk*)chunk_common,
                         chunk_len);
#if 0
        LOGD("SCTP_SACK");
        sack = (SctpSackChunk*)in_packet->chunks;
        LOGD("cumulative_tsn_ack %d", ntohl(sack->cumulative_tsn_ack));
        LOGD("a_rwnd %d", ntohl(sack->a_rwnd));
        LOGD("number_of_gap_ack_blocks %d", sack->number_of_gap_ack_blocks);
        LOGD("number_of_dup_tsns %d", sack->number_of_dup_tsns);
#endif
// XXX: unordered sequence
#if 0
        if (sack->number_of_gap_ack_blocks > 0) {

          int blocks = ntohs(sack->number_of_gap_ack_blocks);
          LOGW("cumulative_tsn_ack: %ld, number_of_gap_ack_blocks: %d",
           ntohl(sack->cumulative_tsn_ack), blocks);

          for (int i = 0; i < blocks; i++) {

            uint16_t *start = (uint16_t*)sack->blocks + i*2;
            uint16_t *end = (uint16_t*)sack->blocks + i*2 + 1;
            LOGW("start: %d, end: %d", ntohs(*start), ntohs(*end));
            sctp->tsn = ntohl(sack->cumulative_tsn_ack) + 1;// + (*start) - 1;
          }
        } else if (sack->number_of_dup_tsns > 0) {

          LOGW("cumulative_tsn_ack: %ld, number_of_dup_tsns: %d",
           ntohl(sack->cumulative_tsn_ack),
           ntohs(sack->number_of_dup_tsns));
        }
#endif
        break;
      case SCTP_HEARTBEAT: {
        SctpChunkCommon* heartbeat_ack = (SctpChunkCommon*)out_packet->chunks;

        if (chunk_len > sizeof(sctp->buf) - sizeof(SctpHeader)) {
          LOGE("SCTP_HEARTBEAT exceeds output buffer");
          return;
        }
        memcpy(heartbeat_ack, chunk_common, chunk_len);
        heartbeat_ack->type = SCTP_HEARTBEAT_ACK;
        length = sizeof(SctpHeader) + chunk_len;
        LOGI("SCTP_HEARTBEAT");
      } break;
      case SCTP_HEARTBEAT_ACK:
        LOGD("SCTP_HEARTBEAT_ACK");
        break;
      case SCTP_COOKIE_ECHO: {
        LOGI("SCTP_COOKIE_ECHO");
        uint32_t cookie;
        if (chunk_len != sizeof(SctpChunkCommon) + sizeof(cookie)) return;
        memcpy(&cookie, (const uint8_t*)chunk_common + sizeof(SctpChunkCommon), sizeof(cookie));
        if (!sctp->cookie_issued || ntohl(cookie) != 2) return;
        sctp_retry_clear_handshake(sctp);
        SctpChunkCommon* common = (SctpChunkCommon*)out_packet->chunks;
        common->type = SCTP_COOKIE_ACK;
        common->length = htons(4);
        length = ntohs(common->length) + sizeof(SctpHeader);
        if (!sctp->connected) {
          sctp->connected = 1;
          notify_open = 1;
        }
      } break;
      case SCTP_COOKIE_ACK: {
        LOGI("SCTP_COOKIE_ACK");
        if (!sctp->connected && sctp->retry_packet.kind == SCTP_RETRY_COOKIE_ECHO) {
          sctp_retry_clear_handshake(sctp);
          sctp->connected = 1;
          notify_open = 1;
        }
        break;
      }
      case SCTP_ABORT: {
        uint16_t cause_code = 0;
        uint16_t cause_length = 0;
        char cause_reason[65] = {0};

        if (chunk_len >= 8) {
          memcpy(&cause_code, buf + pos + sizeof(SctpChunkCommon), sizeof(cause_code));
          memcpy(&cause_length,
                 buf + pos + sizeof(SctpChunkCommon) + sizeof(uint16_t),
                 sizeof(cause_length));
          cause_code = ntohs(cause_code);
          cause_length = ntohs(cause_length);
          if (cause_length > 4 && cause_length <= chunk_len) {
            size_t reason_length = cause_length - 4;
            if (reason_length > sizeof(cause_reason) - 1) {
              reason_length = sizeof(cause_reason) - 1;
            }
            memcpy(cause_reason, buf + pos + sizeof(SctpChunkCommon) + 4, reason_length);
            for (size_t i = 0; i < reason_length; i++) {
              if ((unsigned char)cause_reason[i] < 0x20 || (unsigned char)cause_reason[i] > 0x7e) {
                cause_reason[i] = '.';
              }
            }
          }
        }
        if (cause_code == 12) {
          LOGI("SCTP peer closed association: %s",
               cause_reason[0] ? cause_reason : "user initiated abort");
        } else {
          LOGE("SCTP_ABORT flags=0x%02x cause=%u cause_length=%u reason=%s",
               chunk_common->flags, cause_code, cause_length,
               cause_reason[0] ? cause_reason : "<none>");
        }
        sctp_destroy_association(sctp);
        if (sctp->onclose) {
          sctp->onclose(sctp->userdata);
        }
        stop_processing = 1;
        break;
      }
      default:
        LOGI("Unknown chunk type %d", chunk_common->type);
        length = 0;
        break;
    }

    out_packet->header.source_port = htons(sctp->local_port);
    out_packet->header.destination_port = htons(sctp->remote_port);
    out_packet->header.verification_tag = sctp->verification_tag;
    out_packet->header.checksum = 0x00;

    if (length > 0) {
      // padding 4
      length = (4 * ((length + 3) / 4));
      if (length > sizeof(sctp->buf)) {
        LOGE("SCTP response exceeds output buffer");
        return;
      }
      out_packet->header.checksum = sctp_get_checksum(sctp, sctp->buf, length);
      if (chunk_common->type == SCTP_INIT_ACK && !sctp->connected &&
          sctp_retry_store(sctp, SCTP_RETRY_COOKIE_ECHO,
                           sctp->buf, length, 0) < 0) {
        return;
      }
      int sack_response = chunk_common->type == SCTP_DATA ||
                          chunk_common->type == SCTP_FORWARD_TSN;
      /* Clear before sending: a synchronous callback may queue a newer SACK. */
      if (sack_response) sctp->rx_sack_pending = 0;
      if (sctp_send_packet(sctp, sctp->buf, length) < 0) {
        if (sack_response && sctp->connected) sctp->rx_sack_pending = 1;
        if (chunk_common->type == SCTP_INIT_ACK) {
          LOGW("SCTP COOKIE_ECHO queued for retry after send failure");
        }
        return;
      }
      // sctp_outgoing_data_cb(sctp, sctp->buf, SCTP_MTU, 0, 0);
    }
    if (notify_open && sctp->onopen) sctp->onopen(sctp->userdata);
    if (stop_processing) {
      break;
    }
    pos += padded_chunk_len;
  }
#endif
}

#if CONFIG_USE_USRSCTP
static int sctp_handle_incoming_data(Sctp* sctp, char* data, size_t len, uint32_t ppid, uint16_t sid, int flags) {
  switch (ppid) {
    case DATA_CHANNEL_PPID_CONTROL:
      break;

    case DATA_CHANNEL_PPID_DOMSTRING:
    case DATA_CHANNEL_PPID_BINARY:
    case DATA_CHANNEL_PPID_DOMSTRING_PARTIAL:
    case DATA_CHANNEL_PPID_BINARY_PARTIAL:

      LOGD("Got message (size = %ld)", len);
      if (sctp->onmessage) {
        sctp->onmessage(data, len, sctp->userdata, sid);
      }
      break;

    default:
      break;
  }
  return 0;
}

static void sctp_process_notification(Sctp* sctp, union sctp_notification* notification, size_t len) {
  if (notification->sn_header.sn_length != (uint32_t)len) {
    return;
  }

  switch (notification->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE:

      switch (notification->sn_assoc_change.sac_state) {
        case SCTP_COMM_UP:

          sctp->connected = 1;
          if (sctp->onopen) {
            sctp->onopen(sctp->userdata);
          }

          break;

        case SCTP_COMM_LOST:
        case SCTP_SHUTDOWN_COMP:
          sctp->connected = 0;
          if (sctp->onclose) {
            sctp->onclose(sctp->userdata);
          }
        default:
          break;
      }
      break;
    default:
      break;
  }
}

static int sctp_incoming_data_cb(struct socket* sock, union sctp_sockstore addr, void* data, size_t len, struct sctp_rcvinfo recv_info, int flags, void* userdata) {
  Sctp* sctp = (Sctp*)userdata;
  LOGD("Data of length %u received on stream %u with SSN %u, TSN %u, PPID %u",
       (uint32_t)len,
       recv_info.rcv_sid,
       recv_info.rcv_ssn,
       recv_info.rcv_tsn,
       ntohl(recv_info.rcv_ppid));
  if (flags & MSG_NOTIFICATION) {
    sctp_process_notification(sctp, (union sctp_notification*)data, len);
  } else {
    sctp_handle_incoming_data(sctp, data, len, ntohl(recv_info.rcv_ppid), recv_info.rcv_sid, flags);
  }
  free(data);  // we need to free the memory that usrsctp allocates
  return 0;
}
#endif

int sctp_create_association(Sctp* sctp, DtlsSrtp* dtls_srtp) {
  if (!sctp || (!dtls_srtp && !sctp->send_packet)) {
    return -1;
  }
#if !CONFIG_USE_USRSCTP
  sctp_destroy_association(sctp);
#endif
  sctp->dtls_srtp = dtls_srtp;
  sctp->local_port = 5000;
  sctp->remote_port = 5000;
  sctp->tsn = 1234;
  sctp->stream_count = 0;
  sctp->next_ssn_fallback = 0;
#if CONFIG_USE_USRSCTP
  int ret = -1;
  usrsctp_init(0, sctp_outgoing_data_cb, NULL);
  usrsctp_sysctl_set_sctp_ecn_enable(0);
  usrsctp_register_address(sctp);

  struct socket* sock = usrsctp_socket(AF_CONN, SOCK_STREAM, IPPROTO_SCTP,
                                       sctp_incoming_data_cb, NULL, 0, sctp);

  if (!sock) {
    LOGE("usrsctp_socket failed");
    return -1;
  }

  do {
    if (usrsctp_set_non_blocking(sock, 1) < 0) {
      LOGE("usrsctp_set_non_blocking failed");
      break;
    }

    struct linger lopt;
    lopt.l_onoff = 1;
    lopt.l_linger = 0;
    usrsctp_setsockopt(sock, SOL_SOCKET, SO_LINGER, &lopt, sizeof(lopt));

#if 0
    struct sctp_paddrparams peer_param;
    memset(&peer_param, 0, sizeof peer_param);
    peer_param.spp_flags = SPP_PMTUD_DISABLE;
    peer_param.spp_pathmtu = 1200;
    usrsctp_setsockopt(s, IPPROTO_SCTP, SCTP_PEER_ADDR_PARAMS, &peer_param, sizeof peer_param);
#endif

    struct sctp_assoc_value av;
    av.assoc_id = SCTP_ALL_ASSOC;
    av.assoc_value = SCTP_ENABLE_RESET_STREAM_REQ | SCTP_ENABLE_CHANGE_ASSOC_REQ;
    usrsctp_setsockopt(sock, IPPROTO_SCTP, SCTP_ENABLE_STREAM_RESET, &av, sizeof(av));

    uint32_t nodelay = 1;
    usrsctp_setsockopt(sock, IPPROTO_SCTP, SCTP_NODELAY, &nodelay, sizeof(nodelay));

    static uint16_t event_types[] = {
        SCTP_ASSOC_CHANGE,
        SCTP_PEER_ADDR_CHANGE,
        SCTP_REMOTE_ERROR,
        SCTP_SHUTDOWN_EVENT,
        SCTP_ADAPTATION_INDICATION,
        SCTP_SEND_FAILED_EVENT,
        SCTP_SENDER_DRY_EVENT,
        SCTP_STREAM_RESET_EVENT,
        SCTP_STREAM_CHANGE_EVENT};

    struct sctp_event event;
    memset(&event, 0, sizeof(event));
    event.se_assoc_id = SCTP_ALL_ASSOC;
    event.se_on = 1;
    for (int i = 0; i < sizeof(event_types) / sizeof(uint16_t); i++) {
      event.se_type = event_types[i];
      usrsctp_setsockopt(sock, IPPROTO_SCTP, SCTP_EVENT, &event, sizeof(event));
    }

    struct sctp_initmsg init_msg;
    memset(&init_msg, 0, sizeof init_msg);
    init_msg.sinit_num_ostreams = 300;
    init_msg.sinit_max_instreams = 300;
    usrsctp_setsockopt(sock, IPPROTO_SCTP, SCTP_INITMSG, &init_msg, sizeof init_msg);

    struct sockaddr_conn sconn;
    memset(&sconn, 0, sizeof(sconn));
    sconn.sconn_family = AF_CONN;
    sconn.sconn_port = htons(sctp->local_port);
    sconn.sconn_addr = (void*)sctp;
    ret = usrsctp_bind(sock, (struct sockaddr*)&sconn, sizeof(sconn));

    struct sockaddr_conn rconn;

    memset(&rconn, 0, sizeof(struct sockaddr_conn));
    rconn.sconn_family = AF_CONN;
    rconn.sconn_port = htons(sctp->remote_port);
    rconn.sconn_addr = (void*)sctp;
    ret = usrsctp_connect(sock, (struct sockaddr*)&rconn, sizeof(struct sockaddr_conn));

    if (ret < 0 && errno != EINPROGRESS) {
      LOGE("connect error");
      break;
    }

    ret = 0;

  } while (0);

  if (ret < 0) {
    sctp_destroy_association(sctp);
    return -1;
  }

  sctp->sock = sock;
#else
  sctp->tx_cumulative_tsn_ack = 0;
  sctp->last_forward_tsn = 0;
  sctp->forward_tsn_sent = 0;
  sctp->sack_gap_reports = 0;
  sctp->data_retransmissions = 0;
  sctp->data_retry_exhaustions = 0;
  sctp->tx_cumulative_tsn_ack_valid = 0;
  sctp->tx_abandonable_prefix_tsn = 0;
  sctp->tx_evicted_through_tsn = 0;
  sctp->tx_evicted_reliable_tsn = 0;
  sctp->tx_abandonable_prefix_valid = 0;
  sctp->tx_evicted_through_valid = 0;
  sctp->tx_evicted_reliable_valid = 0;
  sctp->peer_supports_forward_tsn = 0;
  sctp->last_forward_tsn_valid = 0;
  sctp->forward_tsn_repeat_sacks = 0;
  memset(sctp->tx_history, 0, sizeof(sctp->tx_history));
  memset(&sctp->retry_packet, 0, sizeof(sctp->retry_packet));

  // send SCTP_INIT
  int length = 0;
  SctpInitChunk* init_chunk;
  SctpHeader* header;
  SctpPacket* out_packet = (SctpPacket*)sctp->buf;
  size_t parameter_size;
  memset(sctp->buf, 0, sizeof(sctp->buf));
  header = &out_packet->header;
  init_chunk = (SctpInitChunk*)out_packet->chunks;

  header->source_port = htons(sctp->local_port);
  header->destination_port = htons(sctp->remote_port);
  header->verification_tag = 0x0;
  init_chunk->common.type = SCTP_INIT;
  init_chunk->common.flags = 0x00;
  init_chunk->common.length = htons(sizeof(SctpInitChunk));
  init_chunk->initiate_tag = htonl(0x12345678);
  init_chunk->a_rwnd = htonl(0x100000);
  init_chunk->number_of_outbound_streams = 0xffff;
  init_chunk->number_of_inbound_streams = 0xffff;
  init_chunk->initial_tsn = htonl(sctp->tsn);
  parameter_size = sctp_append_forward_tsn_parameter(
      (uint8_t*)init_chunk + sizeof(SctpInitChunk),
      sizeof(sctp->buf) - sizeof(SctpHeader) - sizeof(SctpInitChunk));
  if (parameter_size == 0) {
    return -1;
  }
  init_chunk->common.length = htons(
      sizeof(SctpInitChunk) + parameter_size);
  length = ntohs(init_chunk->common.length) + sizeof(SctpHeader);
  length = (4 * ((length + 3) / 4));
  header->checksum = 0;
  header->checksum = sctp_get_checksum(sctp, sctp->buf, length);
  if (sctp_retry_store(sctp, SCTP_RETRY_INIT,
                       sctp->buf, length, 0) < 0) {
    return -1;
  }
  if (sctp_send_packet(sctp, sctp->buf, length) < 0) {
    LOGW("SCTP INIT queued for retry after send failure");
  }
#endif

  return 0;
}

void sctp_destroy_association(Sctp* sctp) {
#if CONFIG_USE_USRSCTP
  if (sctp && sctp->sock) {
    usrsctp_shutdown(sctp->sock, SHUT_RDWR);
    usrsctp_close(sctp->sock);
    usrsctp_finish();
    sctp->sock = NULL;
  }
#else
  if (sctp) {
    sctp_retry_clear(sctp);
    while (sctp->tx_head) {
      SctpTxPacket* packet = sctp->tx_head;
      sctp->tx_head = packet->next;
      free(packet);
    }
    sctp->tx_tail = NULL;
    sctp->tx_count = 0;
    while (sctp->rx_head) {
      SctpRxChunk* chunk = sctp->rx_head;
      sctp->rx_head = chunk->next;
      free(chunk);
    }
    sctp->rx_count = 0;
    sctp->rx_sack_pending = 0;
    sctp->rx_tsn_mask = 0;
    sctp->rx_tsn_valid = 0;
    sctp->cookie_issued = 0;
    sctp->connected = 0;
    sctp->stream_count = 0;
    memset(sctp->stream_table, 0, sizeof(sctp->stream_table));
  }
#endif
}

int sctp_is_connected(Sctp* sctp) {
  return sctp->connected;
}

int sctp_is_stream_open(Sctp* sctp, uint16_t sid) {
  for (int i = 0; i < sctp->stream_count; i++) {
    if (sctp->stream_table[i].sid == sid) {
      return sctp->stream_table[i].open;
    }
  }
  return 0;
}

uint32_t sctp_get_forward_tsn_sent(const Sctp* sctp) {
#if CONFIG_USE_USRSCTP
  (void)sctp;
  return 0;
#else
  return sctp ? sctp->forward_tsn_sent : 0;
#endif
}

uint32_t sctp_get_sack_gap_reports(const Sctp* sctp) {
#if CONFIG_USE_USRSCTP
  (void)sctp;
  return 0;
#else
  return sctp ? sctp->sack_gap_reports : 0;
#endif
}

int sctp_peer_supports_forward_tsn(const Sctp* sctp) {
#if CONFIG_USE_USRSCTP
  (void)sctp;
  return 1;
#else
  return sctp && sctp->peer_supports_forward_tsn;
#endif
}

uint32_t sctp_get_data_retransmissions(const Sctp* sctp) {
#if CONFIG_USE_USRSCTP
  (void)sctp;
  return 0;
#else
  return sctp ? sctp->data_retransmissions : 0;
#endif
}

uint32_t sctp_get_data_retry_exhaustions(const Sctp* sctp) {
#if CONFIG_USE_USRSCTP
  (void)sctp;
  return 0;
#else
  return sctp ? sctp->data_retry_exhaustions : 0;
#endif
}

void sctp_set_send_packet_callback(Sctp* sctp,
                                   SctpSendPacketCallback callback,
                                   void* user_data) {
  if (!sctp) {
    return;
  }
  sctp->send_packet = callback;
  sctp->send_packet_user_data = user_data;
}

void sctp_onmessage(Sctp* sctp, void (*onmessage)(char* msg, size_t len, void* userdata, uint16_t sid)) {
  sctp->onmessage = onmessage;
}

void sctp_onopen(Sctp* sctp, void (*onopen)(void* userdata)) {
  sctp->onopen = onopen;
}

void sctp_onclose(Sctp* sctp, void (*onclose)(void* userdata)) {
  sctp->onclose = onclose;
}
