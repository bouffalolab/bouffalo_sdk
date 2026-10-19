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
} Sdp;

void sdp_init(Sdp* sdp, char* content, size_t capacity);

int sdp_append(Sdp* sdp, const char* format, ...);

int sdp_append_h264(Sdp* sdp);

int sdp_append_pcma(Sdp* sdp);

int sdp_append_pcmu(Sdp* sdp);

int sdp_append_opus(Sdp* sdp);

int sdp_append_datachannel(Sdp* sdp);

int sdp_create(Sdp* sdp, int b_video, int b_audio, int b_datachannel);

#endif  // SDP_H_
