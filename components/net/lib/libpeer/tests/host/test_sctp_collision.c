/* Crossed INIT/INIT_ACK/COOKIE_ECHO, as observed with the real Qwen server. */
#include "sctp.h"
#include <assert.h>
#include <string.h>

extern uint32_t crc32c(uint32_t crc, const uint8_t *data, unsigned int length);
static DtlsSrtp transport;
static unsigned opened;
static uint8_t last_chunk;
uint32_t ports_get_epoch_time(void) { return 100; }
int dtls_srtp_write(DtlsSrtp *dtls, const uint8_t *packet, size_t length)
{
    (void)dtls;
    assert(length >= sizeof(SctpHeader) + sizeof(SctpChunkCommon));
    last_chunk = packet[sizeof(SctpHeader)];
    return length;
}
static void on_open(void *context) { (void)context; ++opened; }
static void deliver(Sctp *sctp, uint8_t *packet, size_t length)
{
    SctpHeader *header = (SctpHeader *)packet;
    header->checksum = 0;
    header->checksum = crc32c(0xffffffff, packet, length);
    sctp_incoming_data(sctp, (char *)packet, length);
}
static size_t make_init(uint8_t *packet, uint8_t type)
{
    memset(packet, 0, 128);
    SctpInitChunk *init = (SctpInitChunk *)(packet + sizeof(SctpHeader));
    init->common.type = type;
    init->common.length = htons(sizeof(*init) + 8);
    init->initiate_tag = htonl(123);
    init->initial_tsn = htonl(456);
    init->param->type = htons(SCTP_PARAM_STATE_COOKIE);
    init->param->length = htons(8);
    uint32_t cookie = htonl(2);
    memcpy(init->param->value, &cookie, sizeof(cookie));
    return sizeof(SctpHeader) + sizeof(*init) + 8;
}
int main(void)
{
    Sctp sctp = {0};
    uint8_t packet[128];
    sctp.onopen = on_open;
    assert(sctp_create_association(&sctp, &transport) == 0 && last_chunk == SCTP_INIT);
    size_t length = make_init(packet, SCTP_INIT);
    deliver(&sctp, packet, length);
    assert(sctp.cookie_issued && last_chunk == SCTP_INIT_ACK);
    length = make_init(packet, SCTP_INIT_ACK);
    deliver(&sctp, packet, length);
    assert(last_chunk == SCTP_COOKIE_ECHO && !sctp.connected);
    memset(packet, 0, sizeof(packet));
    SctpChunkCommon *chunk = (SctpChunkCommon *)(packet + sizeof(SctpHeader));
    chunk->type = SCTP_COOKIE_ECHO;
    chunk->length = htons(sizeof(*chunk) + 4);
    uint32_t cookie = htonl(2);
    memcpy(packet + sizeof(SctpHeader) + sizeof(*chunk), &cookie, sizeof(cookie));
    deliver(&sctp, packet, sizeof(SctpHeader) + sizeof(*chunk) + 4);
    assert(sctp.connected && opened == 1 && last_chunk == SCTP_COOKIE_ACK);
    deliver(&sctp, packet, sizeof(SctpHeader) + sizeof(*chunk) + 4);
    assert(opened == 1 && last_chunk == SCTP_COOKIE_ACK);
    memset(packet, 0, sizeof(packet));
    chunk->type = SCTP_COOKIE_ACK;
    chunk->length = htons(sizeof(*chunk));
    deliver(&sctp, packet, sizeof(SctpHeader) + sizeof(*chunk));
    assert(opened == 1 && sctp.connected);
    sctp_destroy_association(&sctp);
    assert(!sctp.cookie_issued);
    puts("Simultaneous SCTP INIT and crossed cookies establish one association");
    return 0;
}
