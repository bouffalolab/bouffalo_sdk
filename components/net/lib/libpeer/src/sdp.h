#ifndef SDP_H_
#define SDP_H_

#include <stddef.h>

#ifndef ICE_LITE
#define ICE_LITE 0
#endif

typedef struct Sdp {
  char* content;
  size_t capacity;
  size_t length;
  int failed;
  const char* video_mid;
  const char* audio_mid;
  const char* data_mid;
} Sdp;

void sdp_init(Sdp* sdp, char* content, size_t capacity);

int sdp_append(Sdp* sdp, const char* format, ...);

int sdp_append_h264(Sdp* sdp);

int sdp_append_pcma(Sdp* sdp);

int sdp_append_pcmu(Sdp* sdp);

int sdp_append_opus(Sdp* sdp);

int sdp_append_datachannel(Sdp* sdp);

int sdp_create(Sdp* sdp, int b_video, int b_audio, int b_datachannel);
/* Ordered offer MIDs for an answer; NULL entries are omitted. */
int sdp_create_bundle(Sdp* sdp, const char* first, const char* second, const char* third);

#endif  // SDP_H_
