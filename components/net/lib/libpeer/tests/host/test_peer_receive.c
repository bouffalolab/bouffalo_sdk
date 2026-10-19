/* Exercise the production DTLS BIO boundary without ICE/network mocks above it. */
#include "peer_connection.c"
#include <assert.h>

static int polled_result;
static unsigned polls;
static unsigned audio_packets;
static void on_packet(uint8_t *packet, size_t length, void *context)
{
    (void)context;
    const RtpHeader *header = (const RtpHeader *)packet;
    assert(length == sizeof(*header) + 6);
    assert(header->version == 2 && header->type == PT_OPUS);
    assert(ntohs(header->seq_number) == audio_packets);
    assert(ntohl(header->timestamp) == audio_packets * 960);
    assert(!memcmp(packet + sizeof(*header), "packet", 6));
    audio_packets++;
}

int agent_recv(Agent *agent, uint8_t *buffer, int size)
{
    (void)agent;
    ++polls;
    if (polled_result > 0) {
        assert(size >= polled_result);
        memset(buffer, 0x17, polled_result);
    }
    return polled_result;
}

int main(void)
{
    PeerConnection pc = {0};
    pc.state = PEER_CONNECTION_CONNECTED;
    pc.dtls_srtp.user_data = &pc;
    unsigned char buffer[16];
    assert(peer_connection_dtls_srtp_recv(&pc.dtls_srtp, buffer, sizeof(buffer)) == MBEDTLS_ERR_SSL_WANT_READ);
    assert(polls == 1);
    memcpy(pc.agent_buf, "\x17TLS", 4);
    pc.agent_ret = 4;
    assert(peer_connection_dtls_srtp_recv(&pc.dtls_srtp, buffer, 2) == MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL);
    assert(polls == 1);
    assert(peer_connection_dtls_srtp_recv(&pc.dtls_srtp, buffer, sizeof(buffer)) == 4);
    assert(!memcmp(buffer, "\x17TLS", 4) && pc.agent_ret == 0 && polls == 1);
    assert(peer_connection_dtls_srtp_recv(&pc.dtls_srtp, buffer, sizeof(buffer)) == MBEDTLS_ERR_SSL_WANT_READ);
    assert(polls == 2);
    polled_result = 3;
    assert(peer_connection_dtls_srtp_recv(&pc.dtls_srtp, buffer, sizeof(buffer)) == 3);
    assert(buffer[0] == 0x17 && polls == 3);
    polled_result = -1;
    assert(peer_connection_dtls_srtp_recv(&pc.dtls_srtp, buffer, sizeof(buffer)) == MBEDTLS_ERR_SSL_INTERNAL_ERROR);
    pc.dtls_srtp.role = DTLS_SRTP_ROLE_CLIENT;
    assert(peer_connection_get_local_datachannel_sid(&pc) == 0);
    pc.dtls_srtp.role = DTLS_SRTP_ROLE_SERVER;
    assert(peer_connection_get_local_datachannel_sid(&pc) == 1);
    /* A producer burst must become RTP immediately, without waiting for one
     * peer_connection_loop per frame or adding an unpaced second queue. */
    rtp_encoder_init(&pc.artp_encoder, CODEC_OPUS, on_packet, NULL);
    static const uint8_t payload[] = "packet";
    assert(peer_connection_send_audio(&pc, payload, 6) < 0);
    pc.state = PEER_CONNECTION_COMPLETED;
    for (unsigned i = 0; i < 4; i++) assert(peer_connection_send_audio(&pc, payload, 6) == 0);
    assert(audio_packets == 4 && !pc.audio_rb && !memcmp(payload, "packet", 6));
    puts("Peer DTLS BIO idle/error/cached-datagram and role-dependent SID passed");
    puts("Direct audio burst becomes ordered 20 ms RTP without a second queue");
    return 0;
}
