/* Production SDP serialization/parser with deterministic ICE/crypto boundaries.
 * Real DTLS and certificate handling are covered by test_dtls_transport.c. */
#include "peer_connection.c"
#include <assert.h>

static unsigned certificates;
static DtlsSrtp *active_transport;
static const char remote_fingerprint[] =
    "01:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF:"
    "01:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF";
int dtls_srtp_init(DtlsSrtp *dtls, DtlsSrtpRole role, void *context)
{
    dtls->role = role;
    dtls->user_data = context;
    snprintf(dtls->local_fingerprint, sizeof(dtls->local_fingerprint), "certificate-%u", ++certificates);
    return 0;
}
void dtls_srtp_deinit(DtlsSrtp *dtls) { (void)dtls; }
int dtls_srtp_set_role(DtlsSrtp *dtls, DtlsSrtpRole role) { dtls->role = role; return 0; }
void sctp_destroy_association(Sctp *sctp) { (void)sctp; }
int agent_recv(Agent *agent, uint8_t *data, int length) { (void)agent; (void)data; (void)length; return 0; }
int agent_send(Agent *agent, const uint8_t *data, int length) { (void)agent; (void)data; return length; }
int dtls_srtp_reassemble_client_hello(DtlsSrtp *dtls, uint8_t *data, size_t *length, size_t capacity)
{ (void)dtls; (void)data; (void)length; (void)capacity; return 0; }
void agent_clear_candidates(Agent *agent) { agent->local_candidates_count = 0; }
void agent_gather_candidate(Agent *agent, const char *url, const char *user, const char *password)
{ (void)url; (void)user; (void)password; agent->local_candidates_count = 1; }
void agent_update_candidate_pairs(Agent *agent) { (void)agent; }
void agent_set_remote_description(Agent *agent, char *description)
{
    (void)description;
    if (active_transport) assert(!strcmp(active_transport->remote_fingerprint, remote_fingerprint));
    strcpy(agent->remote_ufrag, "remote");
    strcpy(agent->remote_upwd, "remote-password");
    agent->candidate_pairs_num = 1;
}
void ice_candidate_to_description(IceCandidate *candidate, char *description, int length)
{
    (void)candidate;
    snprintf(description, length, "a=candidate:1 1 UDP 123 192.0.2.1 5000 typ host\r\n");
}
int utils_random_get_string(UtilsRandom *random, char *value, int length)
{
    (void)random; memset(value, 'x', length); value[length] = '\0'; return 0; }

static void check_media(const char *sdp, const char *setup)
{
    const char *section = strstr(sdp, "\r\nm=");
    unsigned sections = 0;
    assert(section);
    while (section) {
        const char *end = strstr(section + 2, "\r\nm=");
        if (!end) end = sdp + strlen(sdp);
        const char *required[] = {"a=ice-ufrag:", "a=ice-pwd:", "a=fingerprint:sha-256 ",
                                  setup, "a=candidate:"};
        for (unsigned i = 0; i < sizeof(required) / sizeof(required[0]); ++i) {
            const char *attribute = strstr(section, required[i]);
            assert(attribute && attribute < end);
        }
        ++sections;
        section = *end ? end : NULL;
    }
    assert(sections == 2);
    assert(strstr(sdp, "a=group:BUNDLE audio datachannel\r\n"));
    assert(strstr(sdp, "a=rtpmap:111 opus/48000/2\r\n"));
    char message_limit[64];
    snprintf(message_limit, sizeof(message_limit), "a=max-message-size:%u\r\n",
             (unsigned)CONFIG_SCTP_MESSAGE_SIZE);
    assert(strstr(sdp, message_limit));
}

static void test_rejected_sdp(void)
{
    char valid[256];
    snprintf(valid, sizeof(valid), "v=0\r\na=setup:passive\r\na=fingerprint:sha-256 %s\r\n",
             remote_fingerprint);
    const char *invalid[] = {
        "a=setup:actpass\r\n", /* Illegal in an answer. */
        "a=setup:holdconn\r\n", "a=setup:active\r\n", /* Conflicting bundle role. */
        "a=fingerprint:sha-256 remote-cert\r\n",
        "a=fingerprint:sha-256 ZZ:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF\r\n",
        "a=fingerprint:sha-256 02:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF\r\n",
    };
    PeerConnection pc = {.dtls_initialized = 1};
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        char description[512];
        snprintf(description, sizeof(description), "%s%s", valid, invalid[i]);
        pc.state = PEER_CONNECTION_NEW;
        peer_connection_set_remote_description_with_type(&pc, description, SDP_TYPE_ANSWER);
        assert(pc.state == PEER_CONNECTION_FAILED);
    }
    /* Each independent SDP parse starts in the constructor's NEW state. */
    pc.state = PEER_CONNECTION_NEW;
    peer_connection_set_remote_description_with_type(&pc, valid, SDP_TYPE_ANSWER);
    assert(pc.state == PEER_CONNECTION_CHECKING);
    /* The preceding fingerprint must not authenticate a new SDP without one. */
    pc.state = PEER_CONNECTION_NEW;
    peer_connection_set_remote_description_with_type(&pc, "v=0\r\na=setup:passive\r\n", SDP_TYPE_ANSWER);
    assert(pc.state == PEER_CONNECTION_FAILED);
    assert(!strcmp(pc.dtls_srtp.remote_fingerprint, remote_fingerprint));
}

int main(void)
{
    PeerConnection pc = {0};
    pc.config.audio_codec = CODEC_OPUS;
    pc.config.datachannel = DATA_CHANNEL_STRING;
    const char *offer = peer_connection_create_offer(&pc);
    assert(offer);
    check_media(offer, "a=setup:actpass");
    char certificate[DTLS_SRTP_FINGERPRINT_LENGTH];
    strcpy(certificate, pc.dtls_srtp.local_fingerprint);
    char remote[256];
    snprintf(remote, sizeof(remote), "v=0\r\na=setup:passive\r\na=fingerprint:sha-256 %s\r\n",
             remote_fingerprint);
    peer_connection_set_remote_description(&pc, remote);
    assert(pc.state == PEER_CONNECTION_CHECKING);
    assert(pc.dtls_srtp.role == DTLS_SRTP_ROLE_CLIENT);
    assert(!strcmp(certificate, pc.dtls_srtp.local_fingerprint) && certificates == 1);
    assert(peer_connection_get_local_datachannel_sid(&pc) == 0);

    PeerConnection answerer = {.state = PEER_CONNECTION_NEW};
    answerer.config = pc.config;
    snprintf(remote, sizeof(remote), "v=0\na=setup:active\na=fingerprint:sha-256 %s\n",
             remote_fingerprint);
    peer_connection_set_remote_description(&answerer, remote);
    const char *answer = peer_connection_create_answer(&answerer);
    assert(answer);
    check_media(answer, "a=setup:passive");
    assert(answerer.dtls_srtp.role == DTLS_SRTP_ROLE_SERVER);
    assert(!strcmp(answerer.dtls_srtp.remote_fingerprint, remote_fingerprint));
    assert(peer_connection_get_local_datachannel_sid(&answerer) == 1);
    active_transport = &pc.dtls_srtp;
    peer_connection_set_remote_description_with_type(&pc, remote, SDP_TYPE_ANSWER);
    assert(pc.state == PEER_CONNECTION_CHECKING);
    active_transport = NULL;
    pc.dtls_srtp.state = DTLS_SRTP_STATE_CONNECTED;
    remote[0] = 'v';
    char *pin = strstr(remote, "a=fingerprint:sha-256 ") + strlen("a=fingerprint:sha-256 ");
    pin[0] = 'F';
    /* An unsupported replacement must preserve the active session and pin. */
    peer_connection_set_remote_description_with_type(&pc, remote, SDP_TYPE_ANSWER);
    assert(pc.state == PEER_CONNECTION_CHECKING);
    assert(!strcmp(pc.dtls_srtp.remote_fingerprint, remote_fingerprint));
    /* Explicit native close -> offer is a new negotiation, not an implicit
     * replacement of an active transport. Preserve that legacy entry point. */
    peer_connection_close(&pc);
    assert(peer_connection_create_offer(&pc));
    assert(pc.state == PEER_CONNECTION_NEW);
    check_media(pc.sdp, "a=setup:actpass");
    PeerConnection closed = {0};
    assert(peer_connection_create_answer(&closed) == NULL);
    assert(closed.state == PEER_CONNECTION_CLOSED);
    test_rejected_sdp();
    puts("Offer/Answer carry per-media transport attributes; Answer role retains advertised certificate");
    return 0;
}
