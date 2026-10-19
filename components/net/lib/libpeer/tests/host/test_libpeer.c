#include "sctp.h"
#include "buffer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint32_t crc32c(uint32_t crc, const uint8_t *data, unsigned int length);
static DtlsSrtp transport;
static unsigned received, closed, writes, opened;
static uint8_t last_type, expected_open_type, last_packet[CONFIG_MTU];
static size_t last_length;
static uint32_t now;
uint32_t ports_get_epoch_time(void) { return now; }
static size_t received_length;
static char received_data[CONFIG_SCTP_MESSAGE_SIZE + 1];
static uint32_t ack;
static uint16_t sent_sid, sent_sqn;
static uint8_t sent_flags;
int dtls_srtp_write(DtlsSrtp *ctx, const uint8_t *data, size_t len)
{
    (void)ctx;
    assert(len >= sizeof(SctpHeader));
    const SctpPacket *packet = (const SctpPacket *)data;
    const SctpChunkCommon *chunk = (const SctpChunkCommon *)packet->chunks;
    assert(len <= sizeof(last_packet));
    memcpy(last_packet, data, len);
    last_length = len;
    last_type = chunk->type;
    if (chunk->type == SCTP_SACK) ack = ntohl(((const SctpSackChunk *)chunk)->cumulative_tsn_ack);
    if (chunk->type == SCTP_DATA) {
        sent_sid = ntohs(((const SctpDataChunk *)chunk)->sid);
        sent_sqn = ntohs(((const SctpDataChunk *)chunk)->sqn);
        sent_flags = ((const SctpDataChunk *)chunk)->iube;
    }
    writes++;
    return len;
}
static void on_message(char *data, size_t len, void *ctx, uint16_t sid)
{
    (void)ctx;
    assert(sid == 3);
    received++;
    received_length = len;
    memcpy(received_data, data, len);
}
static void on_close(void *ctx) { (void)ctx; closed++; }
static void on_open(void *ctx)
{
    Sctp *sctp = ctx;
    opened++;
    /* Sending from onopen must not overwrite the COOKIE_ACK before it is sent. */
    assert(last_type == expected_open_type);
    assert(sctp->connected && sctp->retry_packet.kind == SCTP_RETRY_NONE);
    assert(sctp_outgoing_data(sctp, "open", 4, PPID_STRING, 0) == 4);
}

static size_t append_data(uint8_t *raw, size_t pos, uint32_t tsn, uint16_t sequence, uint8_t flags,
                          const char *data, size_t len)
{
    SctpDataChunk *chunk = (SctpDataChunk *)(raw + pos);
    memset(chunk, 0, sizeof(*chunk));
    chunk->type = SCTP_DATA;
    chunk->iube = flags;
    chunk->tsn = htonl(tsn);
    chunk->sid = htons(3);
    chunk->sqn = htons(sequence);
    chunk->ppid = htonl(DATA_CHANNEL_PPID_DOMSTRING);
    chunk->length = htons(sizeof(*chunk) + len);
    memcpy(chunk->data, data, len);
    return pos + ((sizeof(*chunk) + len + 3) & ~(size_t)3);
}
static void deliver(Sctp *sctp, uint8_t *raw, size_t len)
{
    SctpHeader *header = (SctpHeader *)raw;
    header->checksum = 0;
    header->checksum = crc32c(0xffffffff, raw, len);
    sctp_incoming_data(sctp, (char *)raw, len);
}

static void test_sctp(void)
{
    Sctp sctp = {.dtls_srtp = &transport};
    sctp.connected = 1;
    sctp.onmessage = on_message;
    sctp.onclose = on_close;
    uint8_t raw[2048] = {0};
    char data[1800];
    memset(data, 'x', sizeof(data));
    size_t len = append_data(raw, sizeof(SctpHeader), 10, 0, 2, data, 1000);
    deliver(&sctp, raw, len);
    assert(received == 0 && ack == 10);
    memset(raw, 0, sizeof(raw));
    len = append_data(raw, sizeof(SctpHeader), 11, 0, 1, data + 1000, 800);
    deliver(&sctp, raw, len);
    assert(received == 1 && received_length == 1800 && ack == 11);
    assert(!memcmp(received_data, data, sizeof(data)));
    deliver(&sctp, raw, len); /* Duplicate must not execute again. */
    assert(received == 1);
    memset(raw, 0, sizeof(raw));
    len = append_data(raw, sizeof(SctpHeader), 13, 2, 3, "later", 5);
    deliver(&sctp, raw, len);
    assert(received == 1 && ack == 11);
    memset(raw, 0, sizeof(raw));
    len = append_data(raw, sizeof(SctpHeader), 12, 1, 3, "first", 5);
    len = append_data(raw, len, 13, 2, 3, "later", 5);
    deliver(&sctp, raw, len);
    assert(received == 3 && ack == 13 && received_length == 5);
    assert(!memcmp(received_data, "later", 5));
    memset(raw, 0, sizeof(raw));
    SctpChunkCommon *chunk = (SctpChunkCommon *)(raw + sizeof(SctpHeader));
    chunk->type = SCTP_DATA;
    chunk->length = htons(2); /* malformed chunk must not loop or read past input */
    deliver(&sctp, raw, sizeof(SctpHeader) + sizeof(*chunk));
    assert(received == 3);
    assert(sctp_outgoing_data(&sctp, "test", 4, PPID_STRING, 7) == 4);
    assert(sent_sid == 7 && sent_flags == 3 && sent_sqn == 0);
    assert(sctp_outgoing_data(&sctp, data, sizeof(data), PPID_STRING, 7) == sizeof(data));
    assert(sent_sid == 7 && sent_flags == 1 && sent_sqn == 1);
    assert(sctp_outgoing_data(&sctp, "other", 5, PPID_STRING, 9) == 5);
    assert(sent_sid == 9 && sent_sqn == 0);
    unsigned old_writes = writes;
    now += 800;
    sctp_poll(&sctp, now);
    assert(writes > old_writes); /* Lost outbound DATA is retransmitted. */
    assert(sctp_get_data_retransmissions(&sctp) > 0);
    memset(raw, 0, sizeof(raw));
    SctpSackChunk *sack = (SctpSackChunk *)(raw + sizeof(SctpHeader));
    sack->common.type = SCTP_SACK;
    sack->common.length = htons(sizeof(*sack));
    sack->cumulative_tsn_ack = htonl(sctp.tsn - 1);
    deliver(&sctp, raw, sizeof(SctpHeader) + sizeof(*sack));
    assert(sctp.tx_count == 0 && !sctp.tx_head);
    old_writes = writes;
    now += 800;
    sctp_poll(&sctp, now);
    assert(writes == old_writes);
    sctp_destroy_association(&sctp);
    assert(!sctp.rx_head && !sctp.rx_count && !sctp.rx_tsn_mask);
}

static size_t make_init(uint8_t *raw, uint8_t type, uint32_t tsn)
{
    memset(raw, 0, 128);
    SctpInitChunk *init = (SctpInitChunk *)(raw + sizeof(SctpHeader));
    init->common.type = type;
    size_t len = sizeof(*init);
    init->initiate_tag = htonl(0x87654321);
    init->initial_tsn = htonl(tsn);
    if (type == SCTP_INIT_ACK) {
        SctpChunkParam *cookie = init->param;
        cookie->type = htons(SCTP_PARAM_STATE_COOKIE);
        cookie->length = htons(8);
        uint32_t value = htonl(2);
        memcpy(cookie->value, &value, sizeof(value));
        len += 8;
    }
    init->common.length = htons(len);
    return sizeof(SctpHeader) + len;
}

static void test_handshake(void)
{
    uint8_t raw[128] = {0};
    Sctp sctp = {.dtls_srtp = &transport};
    sctp.onclose = on_close;
    sctp.onopen = on_open;
    sctp.userdata = &sctp;
    expected_open_type = SCTP_COOKIE_ECHO;
    /* Client role: do not publish an open channel before COOKIE_ACK. */
    assert(sctp_create_association(&sctp, &transport) == 0);
    assert(last_type == SCTP_INIT && !sctp.connected && opened == 0);
    unsigned old_writes = writes;
    now += 800;
    sctp_poll(&sctp, now);
    assert(writes == old_writes + 1 && last_type == SCTP_INIT);
    size_t len = make_init(raw, SCTP_INIT_ACK, 100);
    deliver(&sctp, raw, len);
    assert(last_type == SCTP_COOKIE_ECHO && !sctp.connected && opened == 0);
    now += 800;
    sctp_poll(&sctp, now);
    assert(last_type == SCTP_COOKIE_ECHO);
    memset(raw, 0, sizeof(raw));
    SctpChunkCommon *chunk = (SctpChunkCommon *)(raw + sizeof(SctpHeader));
    chunk->type = SCTP_COOKIE_ACK;
    chunk->length = htons(sizeof(*chunk));
    deliver(&sctp, raw, sizeof(SctpHeader) + sizeof(*chunk));
    assert(sctp.connected && sctp.retry_packet.kind == SCTP_RETRY_NONE && opened == 1);
    sctp.rx_tsn = 150;
    len = make_init(raw, SCTP_INIT_ACK, 100);
    deliver(&sctp, raw, len);
    assert(sctp.rx_tsn == 150); /* Late INIT_ACK cannot rewind data delivery. */
    sctp_destroy_association(&sctp);

    /* Responder role: validate cookie and write reply before onopen sends DATA. */
    memset(&sctp, 0, sizeof(sctp));
    sctp.dtls_srtp = &transport;
    sctp.local_port = sctp.remote_port = 5000;
    sctp.onopen = on_open;
    sctp.userdata = &sctp;
    expected_open_type = SCTP_COOKIE_ACK;
    len = make_init(raw, SCTP_INIT, 200);
    deliver(&sctp, raw, len);
    assert(last_type == SCTP_INIT_ACK && !sctp.connected && opened == 1);
    memset(raw, 0, sizeof(raw));
    chunk = (SctpChunkCommon *)(raw + sizeof(SctpHeader));
    chunk->type = SCTP_COOKIE_ECHO;
    chunk->length = htons(sizeof(*chunk) + 4);
    uint32_t value = htonl(3);
    memcpy(raw + sizeof(SctpHeader) + sizeof(*chunk), &value, 4);
    deliver(&sctp, raw, sizeof(SctpHeader) + sizeof(*chunk) + 4);
    assert(!sctp.connected && opened == 1);
    value = htonl(2);
    memcpy(raw + sizeof(SctpHeader) + sizeof(*chunk), &value, 4);
    deliver(&sctp, raw, sizeof(SctpHeader) + sizeof(*chunk) + 4);
    assert(sctp.connected && opened == 2 && last_type == SCTP_DATA);
    deliver(&sctp, raw, sizeof(SctpHeader) + sizeof(*chunk) + 4);
    assert(opened == 2 && last_type == SCTP_COOKIE_ACK);
    len = make_init(raw, SCTP_INIT, 200);
    sctp.rx_tsn = 250;
    deliver(&sctp, raw, len);
    assert(sctp.rx_tsn == 250 && opened == 2);
    sctp_destroy_association(&sctp);

    /* Missing peer: retries are bounded and notify the application once. */
    memset(&sctp, 0, sizeof(sctp));
    sctp.dtls_srtp = &transport;
    sctp.onclose = on_close;
    assert(sctp_create_association(&sctp, &transport) == 0);
    unsigned old_closed = closed;
    for (int i = 0; i < 10; i++) { now += 4000; sctp_poll(&sctp, now); }
    assert(closed == old_closed + 1 && sctp.retry_packet.kind == SCTP_RETRY_NONE);
    sctp_destroy_association(&sctp);

    /* A DATA SACK confirms delivery, but a missing DCEP ACK only fails that
     * stream. Qwen's separate server txt stream must keep working. */
    memset(&sctp, 0, sizeof(sctp));
    sctp.dtls_srtp = &transport;
    sctp.connected = 1;
    sctp.onclose = on_close;
    char open[12] = {DATA_CHANNEL_OPEN};
    assert(sctp_outgoing_data(&sctp, open, sizeof(open), PPID_CONTROL, 0) == sizeof(open));
    memset(raw, 0, sizeof(raw));
    SctpSackChunk *sack = (SctpSackChunk *)(raw + sizeof(SctpHeader));
    sack->common.type = SCTP_SACK;
    sack->common.length = htons(sizeof(*sack));
    sack->cumulative_tsn_ack = htonl(sctp.tsn - 1);
    deliver(&sctp, raw, sizeof(SctpHeader) + sizeof(*sack));
    assert(!sctp.tx_head && sctp.retry_packet.kind == SCTP_RETRY_DCEP_OPEN);
    old_closed = closed;
    for (int i = 0; i < 10; i++) { now += 4000; sctp_poll(&sctp, now); }
    assert(closed == old_closed && sctp.connected && sctp.retry_packet.kind == SCTP_RETRY_NONE);
    assert(sctp_outgoing_data(&sctp, "txt", 3, PPID_STRING, 1) == 3);
    sctp_destroy_association(&sctp);
}

static void test_window_and_heartbeat(void)
{
    Sctp sctp = {.dtls_srtp = &transport};
    sctp.connected = 1;
    for (int i = 0; i < 16; i++) assert(sctp_outgoing_data(&sctp, "x", 1, PPID_STRING, 0) == 1);
    uint32_t tsn = sctp.tsn;
    unsigned old_writes = writes;
    assert(sctp_outgoing_data(&sctp, "x", 1, PPID_STRING, 0) < 0);
    assert(sctp.tsn == tsn && writes == old_writes && sctp.tx_count == 16);
    uint8_t raw[128] = {0};
    SctpChunkCommon *chunk = (SctpChunkCommon *)(raw + sizeof(SctpHeader));
    chunk->type = SCTP_HEARTBEAT;
    chunk->length = htons(12);
    memcpy(raw + sizeof(SctpHeader) + 4, "abcdefgh", 8);
    deliver(&sctp, raw, sizeof(SctpHeader) + 12);
    assert(last_type == SCTP_HEARTBEAT_ACK && last_length == sizeof(SctpHeader) + 12);
    assert(!memcmp(last_packet + sizeof(SctpHeader) + 4, "abcdefgh", 8));
    sctp_destroy_association(&sctp);
    assert(!sctp.connected && !sctp.tx_head && sctp.tx_count == 0);
}

static void test_retry_counters(void)
{
    Sctp sctp = {.dtls_srtp = &transport, .connected = 1, .onclose = on_close};
    unsigned old_closed = closed;
    assert(sctp_outgoing_data(&sctp, "lost", 4, PPID_BINARY, 0) == 4);
    for (unsigned i = 0; i <= CONFIG_SCTP_DATA_RETRY_LIMIT; i++) {
        now += CONFIG_SCTP_DATA_RETRY_MS;
        sctp_poll(&sctp, now);
    }
    assert(sctp_get_data_retransmissions(&sctp) == CONFIG_SCTP_DATA_RETRY_LIMIT);
    assert(sctp_get_data_retry_exhaustions(&sctp) == 1);
    assert(!sctp.connected && !sctp.tx_head && closed == old_closed + 1);
    assert(sctp_create_association(&sctp, &transport) == 0);
    assert(!sctp_get_data_retransmissions(&sctp) && !sctp_get_data_retry_exhaustions(&sctp));
    sctp_destroy_association(&sctp);
    assert(!sctp_get_data_retransmissions(NULL) && !sctp_get_data_retry_exhaustions(NULL));
}

static void test_buffer(void)
{
    Buffer *buffer = buffer_new(256);
    assert(buffer);
    unsigned char expected[1000][80];
    int lengths[1000], head = 0, tail = 0;
    srand(31);
    for (int i = 0; i < 100000; i++) {
        if (tail - head < 1000 && rand() % 2) {
            int len = 1 + rand() % 80;
            unsigned char data[80];
            memset(data, (unsigned char)i, len);
            if (buffer_push_tail(buffer, data, len) == len) {
                memcpy(expected[tail % 1000], data, len);
                lengths[tail++ % 1000] = len;
            }
        } else {
            int size;
            unsigned char *data = buffer_peak_head(buffer, &size);
            if (data) {
                assert(head < tail && size == lengths[head % 1000]);
                assert(!memcmp(data, expected[head++ % 1000], size));
                buffer_pop_head(buffer);
            } else assert(head == tail);
        }
    }
    buffer_free(buffer);
    assert(!buffer_new(0));
    assert(!buffer_new(4));
}
int main(void)
{
    test_sctp();
    test_handshake();
    test_window_and_heartbeat();
    test_retry_counters();
    test_buffer();
    puts("SCTP handshake, retry, fragmentation, backpressure, heartbeat and buffer tests passed");
}
