/* Drive the actual receive loop and RTP depacketizer with queued datagrams. */
#include "peer_connection.c"
#include <assert.h>

static unsigned queued, received, delivered, polls, blocking_polls, nonblocking_polls;
static uint32_t packet_ssrc = 42;

uint32_t ports_get_epoch_time(void) { return 1000; }
/* Consent behavior is covered by ice_transport; this test isolates RTP draining. */
int agent_consent_check(Agent *agent) { (void)agent; return AGENT_CONSENT_VALID; }
void sctp_destroy_association(Sctp *sctp) { (void)sctp; assert(0); }
void buffer_clear(Buffer *buffer) { (void)buffer; assert(0); }
int agent_select_candidate_pair(Agent *agent) { (void)agent; assert(0); return -1; }
int agent_connectivity_check(Agent *agent) { (void)agent; assert(0); return -1; }
int dtls_srtp_handshake(DtlsSrtp *dtls, Address *addr)
{ (void)dtls; (void)addr; assert(0); return -1; }
int sctp_create_association(Sctp *sctp, DtlsSrtp *dtls)
{ (void)sctp; (void)dtls; assert(0); return -1; }
void sctp_poll(Sctp *sctp, uint32_t now) { (void)sctp; (void)now; }
void sctp_incoming_data(Sctp *sctp, char *buf, size_t len)
{ (void)sctp; (void)buf; (void)len; assert(0); }
int mbedtls_ssl_check_pending(const mbedtls_ssl_context *ssl) { (void)ssl; return 0; }
int dtls_srtp_read(DtlsSrtp *dtls, uint8_t *buf, size_t len)
{ (void)dtls; (void)buf; (void)len; assert(0); return -1; }
int dtls_srtp_probe(uint8_t *buf) { return buf[0] >= 20 && buf[0] < 64; }
int dtls_srtp_decrypt_rtp_packet(DtlsSrtp *dtls, uint8_t *buf, int *len)
{ (void)dtls; (void)buf; (void)len; return srtp_err_status_ok; }
int dtls_srtp_decrypt_rtcp_packet(DtlsSrtp *dtls, uint8_t *buf, int *len)
{ (void)dtls; (void)buf; (void)len; assert(0); return -1; }

static int receive_packet(Agent *agent, uint8_t *buf, int capacity)
{
    (void)agent;
    polls++;
    if (!queued) return 0;
    assert(capacity >= 13);
    memset(buf, 0, 13);
    buf[0] = 0x80;
    buf[1] = PT_OPUS;
    uint16_t seq = htons(received);
    uint32_t stamp = htonl(received * 960), ssrc = htonl(packet_ssrc);
    memcpy(buf + 2, &seq, sizeof(seq));
    memcpy(buf + 4, &stamp, sizeof(stamp));
    memcpy(buf + 8, &ssrc, sizeof(ssrc));
    buf[12] = received++;
    queued--;
    return 13;
}
int agent_recv(Agent *agent, uint8_t *buf, int capacity)
{ blocking_polls++; return receive_packet(agent, buf, capacity); }
int agent_recv_nonblocking(Agent *agent, uint8_t *buf, int capacity)
{ nonblocking_polls++; return receive_packet(agent, buf, capacity); }

static void audio(uint8_t *data, size_t length, void *context)
{
    (void)context;
    assert(length == 1 && data[0] == (uint8_t)delivered);
    delivered++;
}

int main(void)
{
    PeerConnection pc = {0};
    pc.state = PEER_CONNECTION_COMPLETED;
    pc.remote_assrc = 42;
    assert(rtp_decoder_init(&pc.artp_decoder, CODEC_OPUS, audio, NULL) == 0);
    queued = 4;
    assert(peer_connection_loop(&pc) == 0);
    assert(delivered == 2 && queued == 2 && polls == 2);
#if CONFIG_PEER_NONBLOCKING_RX
    assert(blocking_polls == 0 && nonblocking_polls == 2);
#else
    assert(blocking_polls == 1 && nonblocking_polls == 1);
#endif
    assert(peer_connection_loop(&pc) == 0);
    assert(delivered == 4 && queued == 0);
    /* Return regularly under sustained media, so the owner can send uplink. */
    queued = 40;
    for (unsigned i = 0; i < 20; i++) {
        unsigned before = delivered;
        assert(peer_connection_loop(&pc) == 0);
        assert(delivered == before + 2 && queued == 38 - i * 2);
    }
    assert(delivered == 44 && queued == 0);
    /* Unknown SSRC must consume budget and allow subsequent media to drain. */
    packet_ssrc = 99;
    queued = 2;
    assert(peer_connection_loop(&pc) == 0);
    assert(delivered == 44 && queued == 0);
    puts("RTP bursts drain in order within the bounded receive budget");
}
