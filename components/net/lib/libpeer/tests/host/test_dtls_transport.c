/* Real DTLS/SRTP over localhost UDP: negotiated roles retain certificates,
 * idle sockets yield, and SRTP keys work in both directions. */
#include "dtls_srtp.h"
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <mbedtls/sha256.h>

typedef struct {
    int fd;
    bool combine;
    unsigned char records[8192];
    size_t record_bytes;
} channel_t;

void ports_sleep_ms(int milliseconds) { usleep(milliseconds * 1000); }

static int send_packet(void *context, const unsigned char *data, size_t size)
{
    DtlsSrtp *dtls = context;
    channel_t *channel = dtls->user_data;
    if (channel->combine) {
        assert(size <= sizeof(channel->records) - channel->record_bytes);
        memcpy(channel->records + channel->record_bytes, data, size);
        channel->record_bytes += size;
        return size;
    }
    int result = send(channel->fd, data, size, 0);
    return result < 0 ? MBEDTLS_ERR_SSL_INTERNAL_ERROR : result;
}

static int receive_packet(void *context, unsigned char *data, size_t size)
{
    DtlsSrtp *dtls = context;
    channel_t *channel = dtls->user_data;
    int result = recv(channel->fd, data, size, MSG_DONTWAIT);
    if (result < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) return MBEDTLS_ERR_SSL_WANT_READ;
    return result < 0 ? MBEDTLS_ERR_SSL_INTERNAL_ERROR : result;
}

static uint64_t milliseconds(void)
{
    struct timespec now;
    assert(clock_gettime(CLOCK_MONOTONIC, &now) == 0);
    return now.tv_sec * 1000ULL + now.tv_nsec / 1000000U;
}

static void socket_pair(channel_t *a, channel_t *b)
{
    struct sockaddr_in addr_a = {.sin_family=AF_INET, .sin_addr.s_addr=htonl(INADDR_LOOPBACK)};
    struct sockaddr_in addr_b = addr_a;
    a->fd = socket(AF_INET, SOCK_DGRAM, 0);
    b->fd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(a->fd >= 0 && b->fd >= 0);
    assert(bind(a->fd, (struct sockaddr *)&addr_a, sizeof(addr_a)) == 0);
    assert(bind(b->fd, (struct sockaddr *)&addr_b, sizeof(addr_b)) == 0);
    socklen_t size = sizeof(addr_a);
    assert(getsockname(a->fd, (struct sockaddr *)&addr_a, &size) == 0);
    assert(getsockname(b->fd, (struct sockaddr *)&addr_b, &size) == 0);
    assert(connect(a->fd, (struct sockaddr *)&addr_b, size) == 0);
    assert(connect(b->fd, (struct sockaddr *)&addr_a, size) == 0);
}

static bool pending(int result)
{
    return result == MBEDTLS_ERR_SSL_WANT_READ || result == MBEDTLS_ERR_SSL_WANT_WRITE;
}

/* A pinned WebRTC certificate may use an old self-signature and dates. Its
 * SHA-256 SDP fingerprint and TLS proof of private-key possession authenticate it. */
static void legacy_certificate(DtlsSrtp *dtls)
{
    mbedtls_x509write_cert certificate;
    mbedtls_mpi serial;
    unsigned char pem[4096], digest[32];
    mbedtls_x509write_crt_init(&certificate);
    mbedtls_mpi_init(&serial);
    assert(mbedtls_mpi_lset(&serial, 1) == 0);
    mbedtls_x509write_crt_set_version(&certificate, MBEDTLS_X509_CRT_VERSION_3);
    mbedtls_x509write_crt_set_md_alg(&certificate, MBEDTLS_MD_SHA1);
    mbedtls_x509write_crt_set_subject_key(&certificate, &dtls->pkey);
    mbedtls_x509write_crt_set_issuer_key(&certificate, &dtls->pkey);
    assert(mbedtls_x509write_crt_set_subject_name(&certificate, "CN=legacy-webrtc") == 0);
    assert(mbedtls_x509write_crt_set_issuer_name(&certificate, "CN=legacy-webrtc") == 0);
    assert(mbedtls_x509write_crt_set_serial(&certificate, &serial) == 0);
    assert(mbedtls_x509write_crt_set_validity(&certificate, "20100101000000", "20110101000000") == 0);
    assert(mbedtls_x509write_crt_pem(&certificate, pem, sizeof(pem), mbedtls_ctr_drbg_random, &dtls->ctr_drbg) == 0);
    mbedtls_x509_crt_free(&dtls->cert);
    mbedtls_x509_crt_init(&dtls->cert);
    assert(mbedtls_x509_crt_parse(&dtls->cert, pem, strlen((char *)pem) + 1) == 0);
#if CONFIG_MBEDTLS_2_X
    assert(mbedtls_sha256_ret(dtls->cert.raw.p, dtls->cert.raw.len, digest, 0) == 0);
#else
    assert(mbedtls_sha256(dtls->cert.raw.p, dtls->cert.raw.len, digest, 0) == 0);
#endif
    for (unsigned i = 0; i < 32; ++i) snprintf(dtls->local_fingerprint + i * 3, 4, "%02X:", digest[i]);
    dtls->local_fingerprint[95] = '\0';
    mbedtls_mpi_free(&serial);
    mbedtls_x509write_crt_free(&certificate);
}

static void rtp_roundtrip(DtlsSrtp *sender, DtlsSrtp *receiver)
{
    const uint8_t plain[] = {0x80, 111, 0, 1, 0, 0, 0, 1, 0x11, 0x22, 0x33, 0x44, 1, 2, 3, 4};
    uint8_t encrypted[128];
    memcpy(encrypted, plain, sizeof(plain));
    int length = sizeof(plain);
    assert(srtp_protect(sender->srtp_out, encrypted, &length) == srtp_err_status_ok);
    assert(length > (int)sizeof(plain));
    assert(srtp_unprotect(receiver->srtp_in, encrypted, &length) == srtp_err_status_ok);
    assert(length == sizeof(plain) && !memcmp(encrypted, plain, sizeof(plain)));
}

static void handshake_pair(bool bad_fingerprint)
{
    DtlsSrtp client = {0}, server = {0};
    channel_t a = {0}, b = {0};
    socket_pair(&a, &b);
    /* Offer's initial endpoint may differ from the Answer's selected role. */
    assert(dtls_srtp_init(&client, DTLS_SRTP_ROLE_SERVER, &a) == 0);
    assert(dtls_srtp_init(&server, DTLS_SRTP_ROLE_CLIENT, &b) == 0);
    legacy_certificate(&server);
    char fingerprint[DTLS_SRTP_FINGERPRINT_LENGTH];
    strcpy(fingerprint, client.local_fingerprint);
    assert(dtls_srtp_set_role(&client, DTLS_SRTP_ROLE_CLIENT) == 0);
    assert(!strcmp(fingerprint, client.local_fingerprint));
    assert(dtls_srtp_set_role(&server, DTLS_SRTP_ROLE_SERVER) == 0);
    strcpy(client.remote_fingerprint, server.local_fingerprint);
    strcpy(server.remote_fingerprint, client.local_fingerprint);
    if (bad_fingerprint) client.remote_fingerprint[0] = '?';
    client.udp_send = server.udp_send = send_packet;
    client.udp_recv = server.udp_recv = receive_packet;
    Address client_address = {0};
    socklen_t address_length = sizeof(client_address.sin);
    assert(getsockname(a.fd, (struct sockaddr *)&client_address.sin, &address_length) == 0);
    client_address.family = AF_INET;
    client_address.port = client_address.sin.sin_port;
    assert(dtls_srtp_handshake(NULL, NULL) == MBEDTLS_ERR_SSL_BAD_INPUT_DATA);
    assert(dtls_srtp_handshake(&server, NULL) == MBEDTLS_ERR_SSL_BAD_INPUT_DATA);
    int client_result = MBEDTLS_ERR_SSL_WANT_READ, server_result = client_result;
    uint64_t start = milliseconds();
    /* No server packet exists yet; absence must yield immediately, not EOF. */
    assert(pending(dtls_srtp_handshake(&client, NULL)));
    assert(milliseconds() - start < 100);
    while (milliseconds() - start < 5000 && (pending(client_result) || pending(server_result))) {
        if (pending(server_result)) server_result = dtls_srtp_handshake(&server, &client_address);
        if (pending(client_result)) client_result = dtls_srtp_handshake(&client, NULL);
        if ((client_result && !pending(client_result)) || (server_result && !pending(server_result))) break;
        usleep(1000);
    }
    if (bad_fingerprint) {
        assert(client_result < 0 && !pending(client_result));
        assert(client.state != DTLS_SRTP_STATE_CONNECTED);
    } else {
        assert(client_result == 0 && server_result == 0);
        assert(!memcmp(client.local_policy_key, server.remote_policy_key, sizeof(client.local_policy_key)));
        assert(!memcmp(server.local_policy_key, client.remote_policy_key, sizeof(server.local_policy_key)));
        rtp_roundtrip(&client, &server);
        rtp_roundtrip(&server, &client);
        /* Both records arrive in one UDP packet. Polling UDP again after the
         * first read would strand the second record until a future packet. */
        a.combine = true;
        assert(dtls_srtp_write(&client, (const unsigned char *)"first", 5) == 5);
        assert(dtls_srtp_write(&client, (const unsigned char *)"second", 6) == 6);
        assert(send(a.fd, a.records, a.record_bytes, 0) == (int)a.record_bytes);
        unsigned char message[32];
        int read_result;
        uint64_t read_started = milliseconds();
        do {
            read_result = dtls_srtp_read(&server, message, sizeof(message));
            if (pending(read_result)) usleep(1000);
        } while (pending(read_result) && milliseconds() - read_started < 100);
        assert(read_result == 5 && !memcmp(message, "first", 5));
        assert(mbedtls_ssl_check_pending(&server.ssl));
        assert(dtls_srtp_read(&server, message, sizeof(message)) == 6 && !memcmp(message, "second", 6));
        assert(dtls_srtp_read(&server, message, sizeof(message)) == MBEDTLS_ERR_SSL_WANT_READ);
        dtls_srtp_reset_session(&client);
        assert(!client.srtp_in && !client.srtp_out);
    }
    dtls_srtp_deinit(&client);
    dtls_srtp_deinit(&server);
    close(a.fd);
    close(b.fd);
}

int main(void)
{
    assert(srtp_init() == srtp_err_status_ok);
    handshake_pair(false);
    handshake_pair(true);
    assert(srtp_shutdown() == srtp_err_status_ok);
    puts("Real DTLS role negotiation, fingerprint rejection, idle yield and bidirectional SRTP passed");
    return 0;
}
