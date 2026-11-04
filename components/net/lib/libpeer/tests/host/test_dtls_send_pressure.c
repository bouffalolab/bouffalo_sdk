/* Exercise the production DTLS BIO with actual encrypted UDP records. Queue
 * pressure loses a datagram without stranding ciphertext or poisoning TLS. */
#include "../../src/peer_connection.c"
#include <assert.h>
#include <stddef.h>
#include <sys/socket.h>
#include <time.h>

static PeerConnection peers[2];
static int sockets[2], fault, failures, actual_sends;
static int failure_result = -1;
void ports_sleep_ms(int ms) { usleep(ms * 1000); }
int agent_send(Agent *agent, const uint8_t *data, int size)
{
    unsigned index = agent == &peers[0].agent ? 0 : 1;
    assert(agent == &peers[index].agent);
    if (failures) {
        failures--;
        if (fault) errno = fault;
        return failure_result;
    }
    actual_sends++;
    return send(sockets[index], data, size, 0);
}
static int receive_record(void *ctx, uint8_t *data, size_t size)
{
    DtlsSrtp *dtls = ctx;
    unsigned index = dtls->user_data == &peers[0] ? 0 : 1;
    int result = recv(sockets[index], data, size, MSG_DONTWAIT);
    return result < 0 && (errno == EAGAIN || errno == EWOULDBLOCK) ?
        MBEDTLS_ERR_SSL_WANT_READ : result;
}
static uint64_t now_ms(void)
{
    struct timespec time;
    assert(clock_gettime(CLOCK_MONOTONIC, &time) == 0);
    return (uint64_t)time.tv_sec * 1000 + time.tv_nsec / 1000000;
}
static int pending(int result)
{
    return result == MBEDTLS_ERR_SSL_WANT_READ || result == MBEDTLS_ERR_SSL_WANT_WRITE;
}
static int read_application(DtlsSrtp *dtls, uint8_t *data, size_t size)
{
    uint64_t deadline = now_ms() + 1000;
    int result;
    do {
        result = dtls_srtp_read(dtls, data, size);
        if (!pending(result)) return result;
        usleep(1000);
    } while (now_ms() < deadline);
    return result;
}
int main(void)
{
    Address address[2] = {0};
    for (unsigned i = 0; i < 2; i++) {
        sockets[i] = socket(AF_INET, SOCK_DGRAM, 0);
        assert(sockets[i] >= 0);
        address[i].family = AF_INET;
        address[i].sin.sin_family = AF_INET;
        address[i].sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        assert(!bind(sockets[i], (struct sockaddr *)&address[i].sin, sizeof(address[i].sin)));
        socklen_t size = sizeof(address[i].sin);
        assert(!getsockname(sockets[i], (struct sockaddr *)&address[i].sin, &size));
        address[i].port = address[i].sin.sin_port;
    }
    assert(!connect(sockets[0], (struct sockaddr *)&address[1].sin, sizeof(address[1].sin)));
    assert(!connect(sockets[1], (struct sockaddr *)&address[0].sin, sizeof(address[0].sin)));
    assert(srtp_init() == srtp_err_status_ok);
    for (unsigned i = 0; i < 2; i++) {
        DtlsSrtp *dtls = &peers[i].dtls_srtp;
        assert(!dtls_srtp_init(dtls, i ? DTLS_SRTP_ROLE_SERVER : DTLS_SRTP_ROLE_CLIENT, &peers[i]));
        dtls->udp_send = peer_connection_dtls_srtp_send;
        dtls->udp_recv = receive_record;
    }
    DtlsSrtp *client = &peers[0].dtls_srtp, *server = &peers[1].dtls_srtp;
    strcpy(client->remote_fingerprint, server->local_fingerprint);
    strcpy(server->remote_fingerprint, client->local_fingerprint);
    const int temporary[] = {EAGAIN, EWOULDBLOCK, ENOMEM, ENOBUFS, ENOSPC, EINTR};
    for (unsigned i = 0; i < sizeof(temporary) / sizeof(*temporary); i++) {
        fault = temporary[i];failures = 2;
        assert(peer_connection_dtls_srtp_send(client, (const uint8_t *)"test", 4) == 4);
    }
    fault = EBADF;failures = 1;
    assert(peer_connection_dtls_srtp_send(client, (const uint8_t *)"test", 4) < 0);
    fault = 0;failures = 1;errno = ENOMEM;
    assert(peer_connection_dtls_srtp_send(client, (const uint8_t *)"test", 4) < 0);
    assert(actual_sends == 0);
    /* TURN's explicit retry status is independent of errno. It must not
     * poison TLS or spin waiting for permission/channel owner progress. */
    unsigned drops = peers[0].dtls_transport_drops;
    failure_result = AGENT_SEND_AGAIN;fault = 0;failures = 1;
    assert(peer_connection_dtls_srtp_send(client, (const uint8_t *)"test", 4) == 4);
    assert(!failures && !actual_sends && peers[0].dtls_transport_drops == drops + 1);
    peers[0].dtls_transport_drops = UINT32_MAX;
    fault = EBADF;failures = 1;
    assert(peer_connection_dtls_srtp_send(client, (const uint8_t *)"test", 4) == 4);
    assert(!failures && !actual_sends && peers[0].dtls_transport_drops == UINT32_MAX);
    failure_result = -1;fault = ENOBUFS;failures = 2;
    assert(peer_connection_dtls_srtp_send(client, (const uint8_t *)"test", 4) == 4);
    assert(!failures && !actual_sends && peers[0].dtls_transport_drops == UINT32_MAX);
    failures = 0;
    assert(peer_connection_dtls_srtp_send(client, (const uint8_t *)"test",
                                          (size_t)INT_MAX + 1) == MBEDTLS_ERR_SSL_BAD_INPUT_DATA);
    assert(!actual_sends);
    peers[0].dtls_transport_drops = 0;
    /* Short-lived pressure is retried once, sending this datagram once. */
    fault = ENOBUFS;failures = 1;
    assert(peer_connection_dtls_srtp_send(client, (const uint8_t *)"test", 4) == 4);
    uint8_t direct[4];
    assert(recv(sockets[1], direct, sizeof(direct), 0) == 4);
    assert(!memcmp(direct, "test", 4) && actual_sends == 1);
    /* Lose the first handshake flight through the production BIO. */
    fault = ENOBUFS;failures = 2;
    int a = MBEDTLS_ERR_SSL_WANT_READ, b = a;
    uint64_t start = now_ms();
    while ((pending(a) || pending(b)) && now_ms() - start < 6000) {
        if (pending(a)) a = dtls_srtp_handshake(client, NULL);
        if (pending(b)) b = dtls_srtp_handshake(server, &address[0]);
        assert(a == 0 || pending(a));assert(b == 0 || pending(b));
        usleep(1000);
    }
    assert(a == 0 && b == 0);
    for (unsigned i = 0; i < sizeof(temporary) / sizeof(*temporary); i++) {
        uint8_t received[32];
        fault = temporary[i];failures = 2;
        assert(dtls_srtp_write(client, (const uint8_t *)"lost", 4) == 4);
        assert(pending(dtls_srtp_read(server, received, sizeof(received))));
        assert(dtls_srtp_write(client, (const uint8_t *)"next", 4) == 4);
        assert(read_application(server, received, sizeof(received)) == 4);
        assert(!memcmp(received, "next", 4));
        assert(pending(dtls_srtp_read(server, received, sizeof(received))));
        /* An upper-layer retransmission arrives once, with no old TLS record. */
        assert(dtls_srtp_write(client, (const uint8_t *)"lost", 4) == 4);
        assert(read_application(server, received, sizeof(received)) == 4);
        assert(!memcmp(received, "lost", 4));
    }
    for (unsigned i = 0; i < 2; i++) {dtls_srtp_deinit(&peers[i].dtls_srtp);close(sockets[i]);}
    assert(srtp_shutdown() == srtp_err_status_ok);
    puts("DTLS direct/TURN pressure, saturated drops, handshake retry, clean records and stale errno rejection passed");
}
