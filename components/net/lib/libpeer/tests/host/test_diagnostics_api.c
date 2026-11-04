/* Compile the fixture with library configuration; compile the consumer below
 * separately, without ANY library configuration macros, as C and C++. */
#ifdef DIAGNOSTICS_API_FIXTURE
#include "../../../webrtc_peer/src/webrtc_peer.c"
#include <assert.h>
static int token;
static int queried_video = -1;
static webrtc_peer_t fixture;
webrtc_peer_t *diagnostics_test_peer(int connected)
{
    fixture.connection = connected ? (PeerConnection *)&token : NULL;
    return &fixture;
}
int diagnostics_test_video(void) { return queried_video; }
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
int peer_connection_get_rtp_diagnostics(PeerConnection *pc, int video,
                                        RtpDiagnosticReport *report)
{
    assert(pc == (PeerConnection *)&token);
    queried_video = video;
    *report = (RtpDiagnosticReport){
        .ssrc=42, .clock_rate=8000, .received_packets=101, .expected_packets=102,
        .packets_lost=-7, .duplicates_recent=103, .reordered_packets=104,
        .untracked_old_packets=105, .sequence_rejects=106, .sequence_restarts=107,
        .jitter_us=108, .max_arrival_gap_ms=109, .timestamp_gaps=110,
        .timestamp_gap_samples=111, .sender_reports=112, .sender_packets=113,
        .sender_octets=114, .remote_reports=115, .remote_packets_lost=-3,
        .remote_fraction_lost_256=116, .remote_jitter_us=117};
    return 0;
}
#endif
#else
#include "webrtc_peer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#ifdef CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
#error This consumer must compile without library feature macros
#endif
#ifdef __cplusplus
extern "C" {
#endif
webrtc_peer_t *diagnostics_test_peer(int connected);
int diagnostics_test_video(void);
#ifdef __cplusplus
}
#endif
int main(void)
{
    struct { uint32_t prefix; webrtc_peer_rtp_diagnostics_t stats; uint8_t tail[8]; } box;
    unsigned char before[sizeof(box)];
    memset(&box, 0xa5, sizeof(box)); memcpy(before, &box, sizeof(box));
    webrtc_peer_t *peer = diagnostics_test_peer(1);
    int result = webrtc_peer_get_rtp_diagnostics(peer, 0, &box.stats,
                                                sizeof(box.stats) + sizeof(box.tail));
    if (result == WEBRTC_PEER_ERR_UNSUPPORTED) {
        assert(!memcmp(before, &box, sizeof(box)));
        assert(webrtc_peer_get_rtp_diagnostics(NULL, -1, NULL, 0) == result);
        assert(diagnostics_test_video() == -1);
        puts("Public diagnostic API: unconfigured consumer links to disabled library, no output writes");
        return 0;
    }
    assert(result == WEBRTC_PEER_OK && diagnostics_test_video() == 0);
    const webrtc_peer_rtp_diagnostics_t *r = &box.stats;
    assert(r->ssrc==42 && r->clock_rate==8000 && r->received_packets==101);
    assert(r->expected_packets==102 && r->packets_lost==-7 && r->duplicates_recent==103);
    assert(r->reordered_packets==104 && r->untracked_old_packets==105);
    assert(r->sequence_rejects==106 && r->sequence_restarts==107);
    assert(r->jitter_us==108 && r->max_arrival_gap_ms==109);
    assert(r->timestamp_gaps==110 && r->timestamp_gap_samples==111);
    assert(r->sender_reports==112 && r->sender_packets==113 && r->sender_octets==114);
    assert(r->remote_reports==115 && r->remote_packets_lost==-3);
    assert(r->remote_fraction_lost_256==116 && r->remote_jitter_us==117);
    assert(box.prefix==UINT32_C(0xa5a5a5a5));
    for (unsigned i=0; i<sizeof(box.tail); ++i) assert(box.tail[i]==0xa5);
    assert(!webrtc_peer_get_rtp_diagnostics(peer,1,&box.stats,sizeof(box.stats)));
    assert(diagnostics_test_video()==1);
    memcpy(before,&box,sizeof(box));
    assert(webrtc_peer_get_rtp_diagnostics(peer,0,&box.stats,sizeof(box.stats)-1)==WEBRTC_PEER_ERR_INVALID_ARG);
    assert(webrtc_peer_get_rtp_diagnostics(NULL,0,&box.stats,sizeof(box.stats))==WEBRTC_PEER_ERR_INVALID_ARG);
    assert(webrtc_peer_get_rtp_diagnostics(peer,0,NULL,sizeof(box.stats))==WEBRTC_PEER_ERR_INVALID_ARG);
    assert(webrtc_peer_get_rtp_diagnostics(peer,2,&box.stats,sizeof(box.stats))==WEBRTC_PEER_ERR_INVALID_ARG);
    assert(webrtc_peer_get_rtp_diagnostics(diagnostics_test_peer(0),0,&box.stats,sizeof(box.stats))==WEBRTC_PEER_ERR_INVALID_STATE);
    assert(!memcmp(before,&box,sizeof(box)));
    puts("Public diagnostic API: unconfigured consumer reads all fields, validates size/state, preserves canaries");
    return 0;
}
#endif
