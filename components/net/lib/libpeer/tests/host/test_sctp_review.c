/* Production SCTP with allocation and transport faults at its boundaries. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

static unsigned allocation, fail_allocation, large_allocations;
static int validate_payload;
static void *test_malloc(size_t bytes)
{
    if (++allocation == fail_allocation) return NULL;
    if (bytes > CONFIG_MTU) ++large_allocations;
    return malloc(bytes);
}
static void *test_calloc(size_t count, size_t bytes)
{
    if (++allocation == fail_allocation) return NULL;
    return calloc(count, bytes);
}
#define malloc test_malloc
#define calloc test_calloc
#include "sctp.c"
#undef malloc
#undef calloc

static uint32_t now;
static unsigned sends, fail_send, messages, closes;
static unsigned sack_sends, sack_failures;
static int reenter_sack;
static int synchronous_ack;
static char delivered[32];
static size_t delivered_sizes[32];
static uint8_t last_packet[CONFIG_MTU];
static size_t last_size;
static Sctp *sender;
static void data(Sctp *sctp, uint32_t tsn, uint16_t sid, uint16_t ssn,
                 uint8_t flags, char value);

uint32_t ports_get_epoch_time(void) { return now; }
static void incoming(Sctp *sctp, uint8_t *packet, size_t bytes)
{
    SctpHeader *header = (SctpHeader *)packet;
    header->checksum = 0;
    header->checksum = crc32c(0xffffffff, packet, bytes);
    sctp_incoming_data(sctp, (char *)packet, bytes);
}
int dtls_srtp_write(DtlsSrtp *dtls, const uint8_t *packet, size_t bytes)
{
    (void)dtls;
    assert(bytes <= sizeof(last_packet));
    memcpy(last_packet, packet, bytes);
    last_size = bytes;
    const SctpDataChunk *chunk = (const SctpDataChunk *)(packet + sizeof(SctpHeader));
    if (chunk->type == SCTP_SACK) {
        ++sack_sends;
        if (sack_failures) {
            --sack_failures;
            return -1;
        }
        if (reenter_sack) {
            reenter_sack = 0;
            sack_failures = 1;
            data(sender, 101, 1, 1, 3, 'b');
        }
    }
    if (chunk->type != SCTP_DATA) return bytes;
    ++sends;
    if (sends == fail_send) return -1;
    if (synchronous_ack) {
        uint8_t ack[sizeof(SctpHeader) + sizeof(SctpSackChunk)] = {0};
        SctpSackChunk *sack = (SctpSackChunk *)(ack + sizeof(SctpHeader));
        sack->common.type = SCTP_SACK;
        sack->common.length = htons(sizeof(*sack));
        sack->cumulative_tsn_ack = chunk->tsn;
        incoming(sender, ack, sizeof(ack));
    }
    return bytes;
}
static void message(char *data, size_t bytes, void *context, uint16_t sid)
{
    (void)context;
    (void)sid;
    assert(bytes && messages < sizeof(delivered));
    if (validate_payload) {
        for (size_t i = 0; i < bytes; ++i) assert(data[i] == 'a');
    }
    delivered_sizes[messages] = bytes;
    delivered[messages++] = data[0];
}
static void closed(void *context) { (void)context; ++closes; }
static DtlsSrtp transport;
static void setup(Sctp *sctp)
{
    memset(sctp, 0, sizeof(*sctp));
    sctp->dtls_srtp = &transport;
    sctp->connected = 1;
    sctp->rx_tsn_valid = 1;
    sctp->rx_tsn = 99;
    sctp->tsn = 200;
    sctp->onmessage = message;
    sctp->onclose = closed;
    allocation = fail_allocation = sends = fail_send = messages = closes = 0;
    validate_payload = 0;
    sack_sends = sack_failures = 0;
    reenter_sack = 0;
    last_size = 0;
    synchronous_ack = 0;
    sender = sctp;
    now = 0;
}
static void data(Sctp *sctp, uint32_t tsn, uint16_t sid, uint16_t ssn,
                 uint8_t flags, char value)
{
    uint8_t packet[sizeof(SctpHeader) + sizeof(SctpDataChunk) + 4] = {0};
    SctpDataChunk *chunk = (SctpDataChunk *)(packet + sizeof(SctpHeader));
    chunk->type = SCTP_DATA;
    chunk->iube = flags;
    chunk->length = htons(sizeof(*chunk) + 1);
    chunk->tsn = htonl(tsn);
    chunk->sid = htons(sid);
    chunk->sqn = htons(ssn);
    chunk->ppid = htonl(PPID_STRING);
    chunk->data[0] = value;
    incoming(sctp, packet, sizeof(packet));
}
static void forward(Sctp *sctp, uint32_t tsn, int sid, uint16_t ssn)
{
    uint8_t packet[sizeof(SctpHeader) + sizeof(SctpForwardTsnChunk) + 4] = {0};
    SctpForwardTsnChunk *chunk = (SctpForwardTsnChunk *)(packet + sizeof(SctpHeader));
    chunk->common.type = SCTP_FORWARD_TSN;
    chunk->common.length = htons(sizeof(*chunk) + (sid >= 0 ? 4 : 0));
    chunk->new_cumulative_tsn = htonl(tsn);
    uint16_t pair[] = {htons(sid), htons(ssn)};
    if (sid >= 0) memcpy(chunk->streams, pair, sizeof(pair));
    incoming(sctp, packet, sizeof(packet) - (sid >= 0 ? 0 : 4));
}
static void test_forward_preserves_complete(void)
{
    Sctp sctp;
    setup(&sctp);
    /* SSN 1 is complete but waits for skipped SSN 0. FORWARD_TSN must
     * deliver the buffered message, even though its TSN is now cumulative. */
    data(&sctp, 101, 1, 1, 3, 'b');
    assert(!messages && sctp.rx_count == 1);
    forward(&sctp, 101, 1, 0);
    assert(messages == 1 && delivered[0] == 'b' && !sctp.rx_count);
    sctp_destroy_association(&sctp);
}
static void test_forward_preserves_skipped_complete(void)
{
    for (unsigned low_memory = 0; low_memory < 2; ++low_memory) {
        Sctp sctp;
        setup(&sctp);
        data(&sctp, 101, 1, 1, 3, 'b');
        data(&sctp, 102, 1, 2, 3, 'c');
        data(&sctp, 103, 1, 3, 3, 'd');
        if (low_memory) fail_allocation = allocation + 1;
        /* RFC 3758 3.6 also releases complete SSNs inside the skip range. */
        forward(&sctp, 103, 1, 2);
        if (low_memory) {
            assert(!messages && sctp.rx_count == 3);
            fail_allocation = 0;
            sctp_poll(&sctp, ++now);
        }
        assert(messages == 3 && !memcmp(delivered, "bcd", 3));
        assert(!sctp.rx_count && sctp.stream_table[0].rx_next_ssn == 4);
        data(&sctp, 104, 1, 4, 3, 'e');
        forward(&sctp, 103, 1, 2); /* A duplicate must not deliver twice. */
        assert(messages == 4 && !memcmp(delivered, "bcde", 4));
        sctp_destroy_association(&sctp);
    }
}
static void test_forward_preserves_fragmented_complete(void)
{
    Sctp sctp;
    setup(&sctp);
    data(&sctp, 101, 1, 1, 2, 'b');
    data(&sctp, 102, 1, 1, 1, 'c');
    data(&sctp, 103, 1, 3, 3, 'd');
    forward(&sctp, 102, 1, 2);
    assert(messages == 2 && !memcmp(delivered, "bd", 2));
    assert(delivered_sizes[0] == 2 && !sctp.rx_count);
    sctp_destroy_association(&sctp);
}
static void test_forward_skip_wrap(void)
{
    Sctp sctp;
    setup(&sctp);
    sctp_set_stream_unordered(&sctp, 1, 0);
    sctp.stream_table[0].rx_next_ssn = UINT16_MAX - 1U;
    data(&sctp, 101, 1, UINT16_MAX, 3, 'b');
    data(&sctp, 102, 1, 0, 3, 'c');
    data(&sctp, 103, 1, 1, 3, 'd');
    forward(&sctp, 103, 1, 0);
    assert(messages == 3 && !memcmp(delivered, "bcd", 3));
    assert(!sctp.rx_count && sctp.stream_table[0].rx_next_ssn == 2);
    sctp_destroy_association(&sctp);
}
static void test_forward_keeps_future_fragments(void)
{
    Sctp sctp;
    setup(&sctp);
    data(&sctp, 103, 1, 1, 2, 'b');
    data(&sctp, 105, 1, 1, 1, 'd');
    forward(&sctp, 101, 1, 0); /* Missing middle is still above the skip point. */
    assert(!messages && sctp.rx_count == 2);
    data(&sctp, 104, 1, 1, 0, 'c');
    assert(messages == 1 && delivered[0] == 'b' && delivered_sizes[0] == 3);
    assert(!sctp.rx_count);
    sctp_destroy_association(&sctp);
}
static void test_stream_order_and_gaps(void)
{
    Sctp sctp;
    setup(&sctp);
    data(&sctp, 101, 1, 1, 3, 'b');
    assert(!messages);
    const SctpSackChunk *sack = (const SctpSackChunk *)(last_packet + sizeof(SctpHeader));
    assert(ntohl(sack->cumulative_tsn_ack) == 99 && ntohs(sack->number_of_gap_ack_blocks) == 1);
    const SctpGapAckBlock *gap = (const SctpGapAckBlock *)sack->blocks;
    assert(ntohs(gap->start) == 2 && ntohs(gap->end) == 2);
    data(&sctp, 102, 2, 0, 3, 'x'); /* Another ordered stream progresses. */
    data(&sctp, 103, 3, 0, 7, 'u'); /* Unordered message passes the gap. */
    data(&sctp, 103, 3, 0, 7, 'u');
    assert(messages == 2 && !memcmp(delivered, "xu", 2));
    data(&sctp, 100, 1, 0, 3, 'a');
    assert(messages == 4 && !memcmp(delivered, "xuab", 4) && sctp.rx_tsn == 103);
    sctp_destroy_association(&sctp);
}
static void test_forward_orphan(void)
{
    Sctp sctp;
    setup(&sctp);
    data(&sctp, 100, 1, 0, 2, 'a');
    data(&sctp, 102, 1, 0, 1, 'c');
    forward(&sctp, 101, -1, 0); /* Skip missing middle and discard its orphan. */
    assert(!messages && !sctp.rx_count && sctp.rx_tsn == 101 && !sctp.rx_tsn_mask);
    data(&sctp, 103, 1, 1, 3, 'b');
    forward(&sctp, 102, 1, 0);
    assert(messages == 1 && delivered[0] == 'b');
    sctp_destroy_association(&sctp);
}
static void test_allocation_recovery(void)
{
    Sctp sctp;
    setup(&sctp);
    fail_allocation = 2; /* Receive chunk is stored, assembly allocation fails. */
    data(&sctp, 100, 1, 0, 3, 'a');
    assert(!messages && sctp.rx_count == 1 && sctp.rx_tsn == 100);
    fail_allocation = 0;
    sctp_poll(&sctp, ++now);
    assert(messages == 1 && delivered[0] == 'a' && !sctp.rx_count);
    assert(last_size >= sizeof(SctpHeader) + sizeof(SctpSackChunk));
    const SctpSackChunk *sack = (const SctpSackChunk *)(last_packet + sizeof(SctpHeader));
    assert(sack->common.type == SCTP_SACK && ntohl(sack->a_rwnd) == SCTP_RX_WINDOW * CONFIG_MTU);
    sctp_destroy_association(&sctp);
}
static void test_window_update_send_retry(void)
{
    Sctp sctp;
    setup(&sctp);
    for (unsigned i = 0; i < SCTP_RX_WINDOW; ++i) {
        if (i + 1 == SCTP_RX_WINDOW) fail_allocation = allocation + 2;
        data(&sctp, 100 + i, 1, 0,
             i == 0 ? 2 : (i + 1 == SCTP_RX_WINDOW ? 1 : 0), 'a');
    }
    assert(!messages && sctp.rx_count == SCTP_RX_WINDOW);
    const SctpSackChunk *sack = (const SctpSackChunk *)(last_packet + sizeof(SctpHeader));
    assert(ntohl(sack->a_rwnd) == 0); /* The peer may now remain silent. */
    unsigned initial_sends = sack_sends;
    fail_allocation = 0;
    sack_failures = 2;
    sctp_poll(&sctp, ++now);
    assert(messages == 1 && delivered_sizes[0] == SCTP_RX_WINDOW && !sctp.rx_count);
    assert(sack_sends == initial_sends + 1);
    sctp_poll(&sctp, ++now);
    assert(sack_sends == initial_sends + 2);
    sctp_poll(&sctp, ++now);
    assert(sack_sends == initial_sends + 3 && !sack_failures && !closes);
    sack = (const SctpSackChunk *)(last_packet + sizeof(SctpHeader));
    assert(ntohl(sack->a_rwnd) == SCTP_RX_WINDOW * CONFIG_MTU);
    sctp_poll(&sctp, ++now);
    assert(sack_sends == initial_sends + 3); /* Success clears the retry. */
    sctp_destroy_association(&sctp);
}
static void test_data_sack_send_retry(void)
{
    Sctp sctp;
    setup(&sctp);
    sack_failures = 1;
    data(&sctp, 100, 1, 0, 3, 'a');
    assert(messages == 1 && !sctp.rx_count && sack_sends == 1);
    sctp_poll(&sctp, ++now);
    assert(sack_sends == 2 && !closes);
    sctp_poll(&sctp, ++now);
    assert(sack_sends == 2);
    sack_failures = 1;
    data(&sctp, 101, 1, 1, 3, 'b');
    assert(sack_sends == 3);
    sctp_destroy_association(&sctp);
    sctp_poll(&sctp, ++now);
    assert(sack_sends == 3); /* Teardown cancels a pending SACK. */
}
static void test_forward_sack_send_retry(void)
{
    Sctp sctp;
    setup(&sctp);
    data(&sctp, 101, 1, 1, 3, 'b');
    sack_failures = 1;
    forward(&sctp, 101, 1, 0);
    assert(messages == 1 && !sctp.rx_count && sack_sends == 2);
    sctp_poll(&sctp, ++now);
    assert(sack_sends == 3 && !closes);
    const SctpSackChunk *sack = (const SctpSackChunk *)(last_packet + sizeof(SctpHeader));
    assert(ntohl(sack->cumulative_tsn_ack) == 101);
    sctp_destroy_association(&sctp);
}
static void test_sack_reentry_preserves_pending(void)
{
    Sctp sctp;
    setup(&sctp);
    sack_failures = 1;
    data(&sctp, 100, 1, 0, 3, 'a');
    reenter_sack = 1;
    sctp_poll(&sctp, ++now);
    /* The old SACK succeeds, while a synchronous callback receives new DATA
     * whose SACK fails. Completing the old send must not clear the new retry. */
    assert(messages == 2 && sack_sends == 3 && !closes);
    sctp_poll(&sctp, ++now);
    assert(sack_sends == 4);
    const SctpSackChunk *sack = (const SctpSackChunk *)(last_packet + sizeof(SctpHeader));
    assert(ntohl(sack->cumulative_tsn_ack) == 101);
    sctp_poll(&sctp, ++now);
    assert(sack_sends == 4);
    sctp_destroy_association(&sctp);
}
static void test_malformed_forward(void)
{
    Sctp sctp;
    setup(&sctp);
    data(&sctp, 101, 1, 1, 3, 'b');
    uint8_t packet[sizeof(SctpHeader) + sizeof(SctpForwardTsnChunk) + 4] = {0};
    SctpForwardTsnChunk *chunk = (SctpForwardTsnChunk *)(packet + sizeof(SctpHeader));
    chunk->common.type = SCTP_FORWARD_TSN;
    chunk->common.length = htons(sizeof(*chunk) + 2); /* Incomplete stream pair. */
    chunk->new_cumulative_tsn = htonl(101);
    incoming(&sctp, packet, sizeof(packet));
    assert(!closes && sctp.connected && sctp.rx_tsn == 99 && sctp.rx_count == 1);
    forward(&sctp, 101, 1, 0);
    assert(messages == 1 && delivered[0] == 'b');
    sctp_destroy_association(&sctp);
}
static void large_fragment(Sctp *sctp, uint32_t tsn, uint16_t ssn, uint8_t flags)
{
    uint8_t packet[sizeof(SctpHeader) + sizeof(SctpDataChunk) + 1200] = {0};
    SctpDataChunk *chunk = (SctpDataChunk *)(packet + sizeof(SctpHeader));
    chunk->type = SCTP_DATA;
    chunk->iube = flags;
    chunk->length = htons(sizeof(*chunk) + 1200);
    chunk->tsn = htonl(tsn);
    chunk->sid = htons(1);
    chunk->sqn = htons(ssn);
    chunk->ppid = htonl(PPID_STRING);
    incoming(sctp, packet, sizeof(packet));
}
static void test_forward_oversized_delivery(void)
{
    Sctp sctp;
    setup(&sctp);
    for (unsigned i = 0; i < 14; ++i)
        large_fragment(&sctp, 101 + i, 1, i == 0 ? 2 : (i == 13 ? 1 : 0));
    assert(!messages && sctp.rx_count == 14 && sctp.connected);
    forward(&sctp, 100, 1, 0);
    assert(!closes && sctp.connected && !sctp.rx_count && !messages);
    data(&sctp, 115, 1, 2, 3, 'b');
    assert(messages == 1 && delivered[0] == 'b');
    sctp_destroy_association(&sctp);
}
static void test_stream_limit_isolation(void)
{
    Sctp sctp;
    setup(&sctp);
    for (unsigned i = 0; i < SCTP_MAX_STREAMS; ++i)
        data(&sctp, 100 + i, i, 0, 3, 'a');
    data(&sctp, 105, 99, 0, 2, 'x');
    data(&sctp, 106, 99, 0, 1, 'x');
    assert(messages == SCTP_MAX_STREAMS && !closes && sctp.connected && !sctp.rx_count);
    data(&sctp, 107, 1, 1, 3, 'b');
    assert(messages == SCTP_MAX_STREAMS + 1 && delivered[messages - 1] == 'b');
    assert(sctp.stream_count == SCTP_MAX_STREAMS && sctp.rx_tsn == 107);
    sctp_destroy_association(&sctp);
}
static void test_oversized_window_recovery(void)
{
    for (unsigned unordered = 0; unordered < 2; ++unordered) {
        Sctp sctp;
        setup(&sctp);
        unsigned u = unordered ? 4 : 0;
        /* More than two windows of fragments must not fill the RX cache. */
        for (unsigned i = 0; i < 40; ++i) {
            large_fragment(&sctp, 100 + i, 0, u | (i == 0 ? 2 : (i == 39 ? 1 : 0)));
            assert(sctp.rx_count < SCTP_RX_WINDOW && !closes && sctp.connected);
        }
        assert(!messages && !sctp.rx_count && sctp.rx_tsn == 139);
        data(&sctp, 140, 2, 0, 3, 'b');
        data(&sctp, 141, 1, unordered ? 0 : 1, unordered ? 7 : 3, 'c');
        assert(messages == 2 && !memcmp(delivered, "bc", 2));
        const SctpSackChunk *sack = (const SctpSackChunk *)(last_packet + sizeof(SctpHeader));
        assert(ntohl(sack->a_rwnd) == SCTP_RX_WINDOW * CONFIG_MTU);
        sctp_destroy_association(&sctp);
    }
}
static void test_discard_marker_reordering(void)
{
    Sctp sctp;
    setup(&sctp);
    for (unsigned i = 0; i < 14; ++i)
        large_fragment(&sctp, 100 + i, 0, i == 0 ? 2 : 0);
    assert(sctp.rx_count == 1 && sctp.rx_head->discarded);
    large_fragment(&sctp, 116, 0, 1); /* E arrives before two middle fragments. */
    sctp_poll(&sctp, ++now);
    assert(sctp.rx_count == 2 && !messages && !closes);
    large_fragment(&sctp, 115, 0, 0);
    large_fragment(&sctp, 114, 0, 0);
    assert(!messages && !sctp.rx_count && sctp.rx_tsn == 116);
    data(&sctp, 117, 1, 1, 3, 'b');
    assert(messages == 1 && delivered[0] == 'b' && !closes);
    sctp_destroy_association(&sctp);
}
static void test_ssn_wrap(void)
{
    Sctp sctp;
    setup(&sctp);
    sctp_set_stream_unordered(&sctp, 1, 0);
    sctp.stream_table[0].rx_next_ssn = UINT16_MAX;
    data(&sctp, 101, 1, 0, 3, 'b');
    assert(!messages);
    data(&sctp, 100, 1, UINT16_MAX, 3, 'a');
    assert(messages == 2 && !memcmp(delivered, "ab", 2));
    assert(sctp.stream_table[0].rx_next_ssn == 1);
    sctp_destroy_association(&sctp);
}
static void test_atomic_send_and_retry(void)
{
    Sctp sctp;
    setup(&sctp);
    char payload[1800];
    memset(payload, 'p', sizeof(payload));
    fail_allocation = 2;
    assert(sctp_outgoing_data(&sctp, payload, sizeof(payload), PPID_STRING, 1) == -1);
    assert(!sends && !sctp.tx_count && sctp.tsn == 200);
    fail_allocation = 0;
    fail_send = 1;
    assert(sctp_outgoing_data(&sctp, payload, sizeof(payload), PPID_STRING, 1) == sizeof(payload));
    assert(sctp.tx_count == 2 && sctp.tsn == 202);
    synchronous_ack = 1;
    now = 800;
    sctp_poll(&sctp, now); /* Synchronous ACK frees each retransmission copy. */
    assert(!sctp.tx_count && !sctp.tx_head && !closes);
    synchronous_ack = 0;
    assert(sctp_outgoing_data(&sctp, "x", 1, PPID_STRING, 1) == 1);
    for (unsigned i = 0; i < 12; ++i) sctp_poll(&sctp, now += 800);
    assert(closes == 1 && !sctp.connected && !sctp.tx_head && !sctp.rx_head);
    sctp_destroy_association(&sctp);
}
static void test_full_window_gap_retransmission(void)
{
    Sctp sctp;
    setup(&sctp);
    data(&sctp, 100, 1, 0, 2, 'a'); /* First message waits for its end. */
    for (unsigned i = 2; i <= SCTP_RX_WINDOW; ++i)
        data(&sctp, 100 + i, 1, i - 1, 3, 'b');
    assert(!messages && sctp.rx_tsn == 100);
    data(&sctp, 101, 1, 0, 1, 'a'); /* Retransmit the missing end. */
    assert(messages && delivered[0] == 'a');
    assert(sctp.rx_count <= SCTP_RX_WINDOW && !closes);
    /* The last out-of-order packet may need retransmission after pressure. */
    data(&sctp, 100 + SCTP_RX_WINDOW, 1, SCTP_RX_WINDOW - 1, 3, 'b');
    assert(messages == SCTP_RX_WINDOW && !sctp.rx_count);
    assert(sctp.rx_tsn == 100 + SCTP_RX_WINDOW);
    const SctpSackChunk *sack = (const SctpSackChunk *)(last_packet + sizeof(SctpHeader));
    assert(ntohl(sack->a_rwnd) == SCTP_RX_WINDOW * CONFIG_MTU);
    sctp_destroy_association(&sctp);
}

static void test_abandonable_send_limit(void)
{
    Sctp sctp;
    setup(&sctp);
    sctp_set_stream_unordered(&sctp, 1, 1);
    sctp_set_stream_drop_on_send_failure(&sctp, 1, 1);
    char payload[CONFIG_SCTP_MESSAGE_SIZE + 1];
    memset(payload, 'x', sizeof(payload));
    assert(sctp_outgoing_data(&sctp, payload, sizeof(payload), PPID_STRING, 1) < 0);
    assert(!sends && sctp.tsn == 200 && sctp.stream_table[0].next_ssn == 0);
    size_t limit = SCTP_TX_WINDOW *
        (SCTP_MTU - sizeof(SctpPacket) - sizeof(SctpDataChunk));
    if (limit < CONFIG_SCTP_MESSAGE_SIZE) {
        assert(sctp_outgoing_data(&sctp, payload, limit + 1, PPID_STRING, 1) < 0);
        assert(!sends && sctp.tsn == 200);
    } else {
        limit = CONFIG_SCTP_MESSAGE_SIZE;
    }
    assert(sctp_outgoing_data(&sctp, payload, limit, PPID_STRING, 1) == limit);
    assert(sends && !sctp.tx_head && !sctp.tx_count);
    sctp_destroy_association(&sctp);
}

static void test_full_window_multiple_gaps(void)
{
    for (unsigned wrap = 0; wrap < 2; ++wrap) {
        Sctp sctp;
        setup(&sctp);
        uint32_t first = wrap ? UINT32_MAX - 1U : 100;
        sctp.rx_tsn = first - 1U;
        data(&sctp, first, 1, 0, 2, 'a');
        for (unsigned i = 3; i <= SCTP_RX_WINDOW; ++i)
            data(&sctp, first + i, 1, i - 2, 3, 'b');
        data(&sctp, first + 1U, 1, 0, 0, 'a'); /* Still missing the final fragment. */
        assert(!messages && sctp.rx_tsn == first + 1U);
        data(&sctp, first + 2U, 1, 0, 1, 'a');
        assert(messages && delivered[0] == 'a' && !closes);
        /* With these exact slot counts the prefix compacts before the final
         * repair, retaining all future messages. The independent gap-repair
         * tests cover legitimate reneging when compaction cannot free a slot. */
        assert(sctp.rx_tsn == first + SCTP_RX_WINDOW && !sctp.rx_count);
        assert(messages == SCTP_RX_WINDOW - 1);
        data(&sctp, first + SCTP_RX_WINDOW, 1, SCTP_RX_WINDOW - 2, 3, 'b');
        assert(messages == SCTP_RX_WINDOW - 1 && !sctp.rx_count);
        assert(sctp.rx_tsn == first + SCTP_RX_WINDOW);
        sctp_destroy_association(&sctp);
    }
}

static void test_more_fragments_than_receive_slots(void)
{
    Sctp sctp;
    setup(&sctp);
    unsigned fragments = SCTP_RX_WINDOW * 3 + 1;
    for (unsigned i = 0; i < fragments; i++) {
        data(&sctp, 100 + i, 1, 0, (i == 0 ? 2 : 0) | (i + 1 == fragments ? 1 : 0), 'a');
        assert(sctp.rx_count <= SCTP_RX_WINDOW && !closes);
        assert(sctp_rx_bytes(&sctp) <= sctp_rx_byte_limit());
    }
    assert(messages == 1 && delivered_sizes[0] == fragments && !sctp.rx_count);
    data(&sctp, 100 + fragments, 1, 1, 3, 'b');
    assert(messages == 2 && delivered[1] == 'b');
    sctp_destroy_association(&sctp);

    setup(&sctp);
    large_allocations = 0;
    validate_payload = 1;
    for (unsigned offset = 0, index = 0; offset < CONFIG_SCTP_MESSAGE_SIZE; ++index) {
        uint8_t packet[sizeof(SctpHeader) + sizeof(SctpDataChunk) + 512] = {0};
        SctpDataChunk* chunk = (SctpDataChunk*)(packet + sizeof(SctpHeader));
        unsigned bytes = CONFIG_SCTP_MESSAGE_SIZE - offset;
        /* Exercise every padding length without charging padding as payload. */
        unsigned fragment_size = 509 + index % 4;
        if (bytes > fragment_size) bytes = fragment_size;
        chunk->type = SCTP_DATA;
        chunk->iube = (offset == 0 ? 2 : 0) | (offset + bytes == CONFIG_SCTP_MESSAGE_SIZE ? 1 : 0);
        chunk->length = htons(sizeof(*chunk) + bytes);
        chunk->tsn = htonl(100 + index); chunk->sid = htons(1);
        chunk->ppid = htonl(PPID_BINARY);
        memset(chunk->data, 'a', bytes);
        incoming(&sctp, packet, (sizeof(SctpHeader) + sizeof(*chunk) + bytes + 3) & ~(size_t)3);
        assert(!closes && sctp.rx_count <= SCTP_RX_WINDOW);
        assert(sctp_rx_bytes(&sctp) <= sctp_rx_byte_limit());
        if (sctp.rx_head) {
            const SctpDataChunk *head = (const SctpDataChunk *)sctp.rx_head->data;
            assert(ntohs(head->length) <= CONFIG_MTU);
        }
        offset += bytes;
    }
    assert(messages == 1 && delivered_sizes[0] == CONFIG_SCTP_MESSAGE_SIZE && !sctp.rx_count);
    /* At most one allocation per capacity doubling plus final delivery.
     * Derive the bound from the minimum fragment and configured message size. */
    unsigned allocation_bound = 1;
    for (size_t capacity = sizeof(SctpDataChunk) + 509;
         capacity < sizeof(SctpDataChunk) + CONFIG_SCTP_MESSAGE_SIZE;
         capacity *= 2) ++allocation_bound;
    assert(large_allocations <= allocation_bound);
    printf("SCTP max-message large allocations: %u (RX slots %u)\n", large_allocations, SCTP_RX_WINDOW);
    sctp_destroy_association(&sctp);

    /* A failed compaction allocation cannot retract cumulative ACKs or lose
     * payload. The owner poll must reopen the window when memory recovers. */
    setup(&sctp);
    for (unsigned i = 0; i < SCTP_RX_WINDOW; i++)
        data(&sctp, 100 + i, 1, 0, i == 0 ? 2 : 0, 'a');
    fail_allocation = allocation + 1;
    data(&sctp, 100 + SCTP_RX_WINDOW, 1, 0, 0, 'a');
    assert(sctp.rx_count == SCTP_RX_WINDOW && sctp.rx_tsn == 99 + SCTP_RX_WINDOW);
    fail_allocation = 0;
    sctp_poll(&sctp, ++now);
    assert(sctp.rx_count == 1 && !messages);
    data(&sctp, 100 + SCTP_RX_WINDOW, 1, 0, 0, 'a');
    data(&sctp, 101 + SCTP_RX_WINDOW, 1, 0, 1, 'a');
    assert(messages == 1 && delivered_sizes[0] == SCTP_RX_WINDOW + 2 && !sctp.rx_count);
    sctp_destroy_association(&sctp);
}

static void test_fragment_allocation_recovery(void)
{
    Sctp sctp;
    setup(&sctp);
    for (unsigned i = 0; i + 1 < SCTP_RX_WINDOW; ++i)
        data(&sctp, 100 + i, 1, 0, i == 0 ? 2 : 0, 'a');
    /* Accept E, but fail complete-message delivery. Poll must retain E and
     * deliver without inventing another fragment for the same message. */
    fail_allocation = allocation + 2;
    data(&sctp, 99 + SCTP_RX_WINDOW, 1, 0, 1, 'a');
    assert(!messages && sctp.rx_count == SCTP_RX_WINDOW);
    fail_allocation = 0;
    sctp_poll(&sctp, ++now);
    assert(messages == 1 && delivered_sizes[0] == SCTP_RX_WINDOW && !sctp.rx_count);
    data(&sctp, 100 + SCTP_RX_WINDOW, 1, 1, 3, 'b');
    assert(messages == 2 && delivered[1] == 'b' && !sctp.rx_count && !closes);
    sctp_destroy_association(&sctp);

    setup(&sctp);
    for (unsigned i = 0; i < SCTP_RX_WINDOW; ++i)
        data(&sctp, 100 + i, 1, 0, i == 0 ? 2 : 0, 'a');
    fail_allocation = allocation + 1;
    data(&sctp, 100 + SCTP_RX_WINDOW, 1, 0, 1, 'a');
    /* Failed prefix growth must preserve every cumulative byte and leave E
     * unacknowledged so the sender can retransmit that exact TSN. */
    assert(!messages && sctp.rx_count == SCTP_RX_WINDOW);
    assert(sctp.rx_tsn == 99 + SCTP_RX_WINDOW);
    fail_allocation = 0;
    sctp_poll(&sctp, ++now);
    assert(sctp.rx_count == 1 && !messages);
    fail_allocation = allocation + 2;
    data(&sctp, 100 + SCTP_RX_WINDOW, 1, 0, 1, 'a');
    assert(!messages && sctp.rx_tsn == 100 + SCTP_RX_WINDOW);
    fail_allocation = 0;
    sctp_poll(&sctp, ++now);
    assert(messages == 1 && delivered_sizes[0] == SCTP_RX_WINDOW + 1 && !sctp.rx_count);
    data(&sctp, 101 + SCTP_RX_WINDOW, 1, 1, 3, 'b');
    assert(messages == 2 && delivered[1] == 'b' && !sctp.rx_count && !closes);
    sctp_destroy_association(&sctp);
    puts("SCTP E-fragment allocation faults retain cumulative bytes and recover delivery before the next SSN");
}

static void test_missing_abandonable_first_fragment(void)
{
    Sctp tx, rx;
    setup(&tx);
    tx.tsn = 100;
    tx.peer_supports_forward_tsn = 1;
    sctp_set_stream_unordered(&tx, 1, 1);
    sctp_set_stream_drop_on_send_failure(&tx, 1, 1);
    setup(&rx);
    char payload[2000];
    memset(payload, 'x', sizeof(payload));
    assert(sctp_outgoing_data(&tx, payload, sizeof(payload), PPID_STRING, 1) == sizeof(payload));
    assert(tx.tsn == 102);
    data(&rx, 101, 1, 0, 5, 'x'); /* Lose the first fragment, TSN 100. */
    assert(rx.rx_tsn == 99 && rx.rx_count == 1);
    const SctpSackChunk *sack = (const SctpSackChunk *)(last_packet + sizeof(SctpHeader));
    const SctpGapAckBlock *gap = (const SctpGapAckBlock *)sack->blocks;
    assert(ntohs(sack->number_of_gap_ack_blocks) == 1 && ntohs(gap->start) == 2);
    uint8_t packet[CONFIG_MTU];
    memcpy(packet, last_packet, last_size);
    incoming(&tx, packet, last_size);
    assert(tx.forward_tsn_sent == 1);
    memcpy(packet, last_packet, last_size);
    incoming(&rx, packet, last_size);
    assert(!rx.rx_count && rx.rx_tsn == 100);
    data(&rx, 102, 3, 0, 3, 'r'); /* Another reliable stream still delivers. */
    assert(messages == 1 && delivered[0] == 'r' && !rx.rx_count);
    memcpy(packet, last_packet, last_size);
    incoming(&tx, packet, last_size);
    assert(tx.forward_tsn_sent == 2);
    memcpy(packet, last_packet, last_size);
    incoming(&rx, packet, last_size);
    assert(rx.rx_tsn == 102 && !rx.rx_count && messages == 1);
    sctp_destroy_association(&tx);
    sctp_destroy_association(&rx);
}

int main(void)
{
#ifdef SCTP_RX_COMPACTION_ONLY
    test_more_fragments_than_receive_slots();
    test_fragment_allocation_recovery();
    puts("SCTP small receive-window compaction and allocation recovery passed");
    return 0;
#endif
    test_abandonable_send_limit();
#ifdef SCTP_SEND_LIMITS_ONLY
    return 0;
#endif
    test_full_window_gap_retransmission();
    test_full_window_multiple_gaps();
    test_missing_abandonable_first_fragment();
    test_more_fragments_than_receive_slots();
    test_fragment_allocation_recovery();
    test_forward_preserves_complete();
    test_forward_preserves_skipped_complete();
    test_forward_preserves_fragmented_complete();
    test_forward_skip_wrap();
    test_forward_keeps_future_fragments();
    test_stream_order_and_gaps();
    test_forward_orphan();
    test_allocation_recovery();
    test_window_update_send_retry();
    test_data_sack_send_retry();
    test_forward_sack_send_retry();
    test_sack_reentry_preserves_pending();
    test_malformed_forward();
    test_forward_oversized_delivery();
    test_stream_limit_isolation();
    test_oversized_window_recovery();
    test_discard_marker_reordering();
    test_ssn_wrap();
    test_atomic_send_and_retry();
    puts("SCTP stream order, gap ACK, FORWARD_TSN, allocation faults and synchronous retry passed");
    return 0;
}
