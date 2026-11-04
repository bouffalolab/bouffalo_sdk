/* Library fixture and independently compiled C/C++ consumer. */
#ifdef AUDIO_API_FIXTURE
#include "../../../webrtc_peer/src/webrtc_peer.c"
#include "../../../webrtc_peer/src/webrtc_peer_audio.c"
static webrtc_peer_t fixture;
static int connection_token, callback_active;
webrtc_peer_t *audio_api_test_peer(int connected, int callback)
{
    fixture.connection = connected ? (PeerConnection *)&connection_token : NULL;
    callback_active = callback;
    return &fixture;
}
#if CONFIG_LIBPEER_RTP_PACKET_INFO
int peer_connection_get_audio_packet_info(PeerConnection *pc, PeerRtpPacketInfo *info)
{
    if (pc != (PeerConnection *)&connection_token || !callback_active) return -1;
    *info = (PeerRtpPacketInfo){.ssrc=42, .timestamp=1234, .sequence=65535, .payload_type=8};
    return 0;
}
#endif
#else
#include "webrtc_peer.h"
#include "webrtc_peer_audio.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#if defined(CONFIG_LIBPEER_MEDIA_DIAGNOSTICS) || defined(CONFIG_LIBPEER_RTP_PACKET_INFO)
#error Consumer must not depend on library configuration macros
#endif
#ifdef __cplusplus
extern "C" {
#endif
webrtc_peer_t *audio_api_test_peer(int connected, int callback);
#ifdef __cplusplus
}
#endif
int main(void)
{
    struct { uint32_t prefix; webrtc_peer_audio_packet_info_t info; uint8_t tail[8]; } box;
    unsigned char before[sizeof(box)];
    memset(&box, 0xa5, sizeof(box)); memcpy(before, &box, sizeof(box));
    int result = webrtc_peer_get_audio_packet_info(audio_api_test_peer(1, 1),
                                                  &box.info, sizeof(box.info));
    if (result == WEBRTC_PEER_ERR_UNSUPPORTED) {
        assert(!memcmp(before, &box, sizeof(box)));
        assert(webrtc_peer_get_audio_packet_info(NULL, NULL, 0) == result);
    } else {
        assert(result == WEBRTC_PEER_OK);
        assert(box.info.ssrc==42 && box.info.timestamp==1234);
        assert(box.info.sequence==65535 && box.info.payload_type==8);
        assert(box.prefix==UINT32_C(0xa5a5a5a5));
        for (unsigned i=0; i<sizeof(box.tail); ++i) assert(box.tail[i]==0xa5);
        memcpy(before, &box, sizeof(box));
        assert(webrtc_peer_get_audio_packet_info(audio_api_test_peer(1,0), &box.info,
                                                sizeof(box.info))==WEBRTC_PEER_ERR_INVALID_STATE);
        assert(webrtc_peer_get_audio_packet_info(audio_api_test_peer(0,1), &box.info,
                                                sizeof(box.info))==WEBRTC_PEER_ERR_INVALID_STATE);
        assert(webrtc_peer_get_audio_packet_info(NULL,&box.info,sizeof(box.info))==WEBRTC_PEER_ERR_INVALID_ARG);
        assert(webrtc_peer_get_audio_packet_info(audio_api_test_peer(1,1),&box.info,
                                                sizeof(box.info)-1)==WEBRTC_PEER_ERR_INVALID_ARG);
        assert(!memcmp(before, &box, sizeof(box)));
    }
    struct { uint32_t prefix; webrtc_peer_audio_stats_t stats; uint8_t tail[8]; } stats;
    unsigned char stats_before[sizeof(stats)];
    memset(&stats,0xa5,sizeof(stats)); memcpy(stats_before,&stats,sizeof(stats));
    webrtc_peer_audio_config_t config = {8000,160,240};
    webrtc_peer_audio_t *audio = NULL;
    assert(!webrtc_peer_audio_open(&config,&audio));
    result=webrtc_peer_audio_get_stats(audio,&stats.stats,sizeof(stats.stats));
    if (result==WEBRTC_PEER_ERR_UNSUPPORTED) {
        assert(!memcmp(stats_before,&stats,sizeof(stats)));
        assert(webrtc_peer_audio_get_stats(NULL,NULL,0)==result);
    } else {
        assert(result==WEBRTC_PEER_OK && stats.stats.real_samples==0);
        assert(stats.stats.target_delay_ms==160 && stats.stats.allocated_bytes>0);
        assert(stats.prefix==UINT32_C(0xa5a5a5a5));
        for(unsigned i=0;i<sizeof(stats.tail);++i) assert(stats.tail[i]==0xa5);
        memcpy(stats_before,&stats,sizeof(stats));
        assert(webrtc_peer_audio_get_stats(audio,&stats.stats,sizeof(stats.stats)-1)==WEBRTC_PEER_ERR_INVALID_ARG);
        assert(webrtc_peer_audio_get_stats(NULL,&stats.stats,sizeof(stats.stats))==WEBRTC_PEER_ERR_INVALID_ARG);
        assert(!memcmp(stats_before,&stats,sizeof(stats)));
    }
    webrtc_peer_audio_close(audio);
    puts("Audio API: unconfigured consumer, optional metadata/statistics, bounds and canaries passed");
    return 0;
}
#endif
