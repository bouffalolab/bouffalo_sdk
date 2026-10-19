/* Real SCTP receiver: Qwen begin/end/middle delivery, duplicates and bounds. */
#include "sctp.h"
#include <assert.h>
#include <limits.h>
#include <string.h>

extern uint32_t crc32c(uint32_t crc, const uint8_t *data, unsigned int length);
static DtlsSrtp transport;
static unsigned messages, closed;
static uint32_t ack;
static size_t received_length;
static char received[CONFIG_SCTP_MESSAGE_SIZE + 1];
static unsigned char ordered[32];

uint32_t ports_get_epoch_time(void) { return 100; }
int dtls_srtp_write(DtlsSrtp *dtls, const uint8_t *packet, size_t length)
{
    (void)dtls;
    assert(length >= sizeof(SctpHeader) + sizeof(SctpChunkCommon));
    if (packet[sizeof(SctpHeader)] == SCTP_SACK) {
        const SctpSackChunk *sack = (const SctpSackChunk *)(packet + sizeof(SctpHeader));
        ack = ntohl(sack->cumulative_tsn_ack);
    }
    return length;
}
static void on_message(char *data, size_t length, void *context, uint16_t sid)
{
    (void)context;
    assert(sid == 1 && length <= CONFIG_SCTP_MESSAGE_SIZE && messages < sizeof(ordered));
    ordered[messages++] = (unsigned char)data[0];
    memcpy(received, data, length);
    received_length = length;
}
static void on_close(void *context) { (void)context; ++closed; }
static void setup(Sctp *sctp, uint32_t first_tsn)
{
    memset(sctp, 0, sizeof(*sctp));
    sctp->dtls_srtp = &transport;
    sctp->connected = 1;
    sctp->rx_tsn_valid = 1;
    sctp->rx_tsn = first_tsn - 1;
    sctp->onmessage = on_message;
    sctp->onclose = on_close;
    sctp_set_stream_unordered(sctp, 1, 0);
    messages = closed = 0;
}
static void data(Sctp *sctp, uint32_t tsn, uint16_t sequence, uint8_t flags,
                 const char *payload, size_t length)
{
    uint8_t packet[2048] = {0};
    SctpHeader *header = (SctpHeader *)packet;
    SctpDataChunk *chunk = (SctpDataChunk *)(packet + sizeof(*header));
    chunk->type = SCTP_DATA;
    chunk->iube = flags;
    chunk->length = htons(sizeof(*chunk) + length);
    chunk->tsn = htonl(tsn);
    chunk->sid = htons(1);
    chunk->sqn = htons(sequence);
    chunk->ppid = htonl(PPID_STRING);
    memcpy(chunk->data, payload, length);
    size_t total = (sizeof(*header) + sizeof(*chunk) + length + 3) & ~(size_t)3;
    assert(total <= sizeof(packet));
    header->checksum = crc32c(0xffffffff, packet, total);
    sctp_incoming_data(sctp, (char *)packet, total);
}
static void test_fragment_order(uint32_t first)
{
    Sctp sctp;
    char expected[1700];
    memset(expected, 'a', 800);
    memset(expected + 800, 'b', 800);
    memset(expected + 1600, 'c', 100);
    setup(&sctp, first);
    data(&sctp, first, 0, 2, expected, 800);
    data(&sctp, first + 2, 0, 1, expected + 1600, 100);
    assert(messages == 0 && ack == first && sctp.rx_count == 2);
    data(&sctp, first + 2, 0, 1, "duplicate", 9);
    assert(messages == 0 && sctp.rx_count == 2);
    data(&sctp, first + 1, 0, 0, expected + 800, 800);
    assert(messages == 1 && ack == first + 2 && !closed);
    assert(received_length == sizeof(expected) && !memcmp(received, expected, sizeof(expected)));
    assert(sctp.rx_count == 0 && !sctp.rx_head);
    data(&sctp, first, 0, 2, expected, 800);
    data(&sctp, first + 1, 0, 0, expected + 800, 800);
    data(&sctp, first + 2, 0, 1, expected + 1600, 100);
    assert(messages == 1 && ack == first + 2);
    sctp_destroy_association(&sctp);
    assert(!sctp.rx_head && !sctp.rx_count && !sctp.rx_tsn_mask);
}
static void test_window(void)
{
    Sctp sctp;
    setup(&sctp, 1);
    char oversized[CONFIG_MTU] = {0};
    data(&sctp, 2, 2, 3, oversized, sizeof(oversized));
    assert(!sctp.rx_head && !sctp.rx_count && ack == 0);
    for (unsigned tsn = 17; tsn > 1; --tsn) {
        char value = tsn;
        data(&sctp, tsn, tsn - 1, 3, &value, 1);
    }
    assert(messages == 0 && sctp.rx_count == 15 && ack == 0);
    char value = 1;
    data(&sctp, 1, 0, 3, &value, 1);
    assert(messages == 16 && ack == 16 && !sctp.rx_head && !sctp.rx_count);
    for (unsigned i = 0; i < messages; ++i) assert(ordered[i] == i + 1);
    value = 17;
    data(&sctp, 17, 16, 3, &value, 1); /* Outside-window chunk can be retransmitted. */
    assert(messages == 17 && ack == 17 && !closed);
    data(&sctp, 19, 19, 3, &value, 1);
    assert(sctp.rx_count == 1);
    sctp_destroy_association(&sctp); /* Closing with a gap must free buffered data. */
    assert(!sctp.rx_head && !sctp.rx_count && !sctp.rx_tsn_mask);
    setup(&sctp, 20);
    data(&sctp, 20, 0, 3, &value, 1);
    assert(messages == 1 && ack == 20);
    sctp_destroy_association(&sctp);
}
int main(void)
{
    test_fragment_order(3310859764U); /* Captured Qwen session.updated TSNs. */
    test_fragment_order(UINT32_MAX); /* begin=max, middle=0, end=1. */
    test_window();
    puts("SCTP reorders bounded DATA fragments, delivers once and frees queued chunks");
    return 0;
}
