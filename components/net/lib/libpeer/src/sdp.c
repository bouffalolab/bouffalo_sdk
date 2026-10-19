#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "sdp.h"

void sdp_init(Sdp* sdp, char* content, size_t capacity) {
  if (!sdp) {
    return;
  }

  sdp->content = content;
  sdp->capacity = capacity;
  sdp->length = 0;
  sdp->failed = content == NULL || capacity == 0;
  if (!sdp->failed) {
    content[0] = '\0';
  }
}

int sdp_append(Sdp* sdp, const char* format, ...) {
  va_list argptr;
  size_t available;
  int written;

  if (!sdp || sdp->failed || !format || sdp->length >= sdp->capacity) {
    return -1;
  }

  available = sdp->capacity - sdp->length;
  if (available <= 3) {
    sdp->failed = 1;
    return -1;
  }

  va_start(argptr, format);
  written = vsnprintf(sdp->content + sdp->length, available, format, argptr);
  va_end(argptr);

  if (written < 0 || (size_t)written + 2 >= available) {
    sdp->content[sdp->length] = '\0';
    sdp->failed = 1;
    return -1;
  }

  sdp->length += (size_t)written;
  sdp->content[sdp->length++] = '\r';
  sdp->content[sdp->length++] = '\n';
  sdp->content[sdp->length] = '\0';
  return 0;
}

int sdp_append_h264(Sdp* sdp) {
  return sdp_append(sdp, "m=video 9 UDP/TLS/RTP/SAVPF 96") ||
         sdp_append(sdp, "c=IN IP4 0.0.0.0") ||
         sdp_append(sdp, "a=rtcp-fb:96 nack") ||
         sdp_append(sdp, "a=rtcp-fb:96 nack pli") ||
         sdp_append(sdp, "a=fmtp:96 profile-level-id=42e01f;level-asymmetry-allowed=1") ||
         sdp_append(sdp, "a=rtpmap:96 H264/90000") ||
         sdp_append(sdp, "a=ssrc:1 cname:webrtc-h264") ||
         sdp_append(sdp, "a=sendrecv") ||
         sdp_append(sdp, "a=mid:video") ||
         sdp_append(sdp, "a=rtcp-mux");
}

int sdp_append_pcma(Sdp* sdp) {
  return sdp_append(sdp, "m=audio 9 UDP/TLS/RTP/SAVP 8") ||
         sdp_append(sdp, "c=IN IP4 0.0.0.0") ||
         sdp_append(sdp, "a=rtpmap:8 PCMA/8000") ||
         sdp_append(sdp, "a=ssrc:4 cname:webrtc-pcma") ||
         sdp_append(sdp, "a=sendrecv") ||
         sdp_append(sdp, "a=mid:audio") ||
         sdp_append(sdp, "a=rtcp-mux");
}

int sdp_append_pcmu(Sdp* sdp) {
  return sdp_append(sdp, "m=audio 9 UDP/TLS/RTP/SAVP 0") ||
         sdp_append(sdp, "c=IN IP4 0.0.0.0") ||
         sdp_append(sdp, "a=rtpmap:0 PCMU/8000") ||
         sdp_append(sdp, "a=ssrc:5 cname:webrtc-pcmu") ||
         sdp_append(sdp, "a=sendrecv") ||
         sdp_append(sdp, "a=mid:audio") ||
         sdp_append(sdp, "a=rtcp-mux");
}

int sdp_append_opus(Sdp* sdp) {
  return sdp_append(sdp, "m=audio 9 UDP/TLS/RTP/SAVP 111") ||
         sdp_append(sdp, "c=IN IP4 0.0.0.0") ||
         sdp_append(sdp, "a=rtpmap:111 opus/48000/2") ||
         sdp_append(sdp, "a=ssrc:6 cname:webrtc-opus") ||
         sdp_append(sdp, "a=sendrecv") ||
         sdp_append(sdp, "a=mid:audio") ||
         sdp_append(sdp, "a=rtcp-mux");
}

int sdp_append_datachannel(Sdp* sdp) {
  return sdp_append(sdp, "m=application 9 UDP/DTLS/SCTP webrtc-datachannel") ||
         sdp_append(sdp, "c=IN IP4 0.0.0.0") ||
         sdp_append(sdp, "a=mid:datachannel") ||
         sdp_append(sdp, "a=sctp-port:5000") ||
         sdp_append(sdp, "a=max-message-size:%u", (unsigned)CONFIG_SCTP_MESSAGE_SIZE);
}

int sdp_create(Sdp* sdp, int b_video, int b_audio, int b_datachannel) {
  if (sdp_append(sdp, "v=0") ||
      sdp_append(sdp, "o=- 1495799811084970 1495799811084970 IN IP4 0.0.0.0") ||
      sdp_append(sdp, "s=-") ||
      sdp_append(sdp, "t=0 0") ||
      sdp_append(sdp, "a=msid-semantic: iot")) {
    return -1;
  }
#if ICE_LITE
  if (sdp_append(sdp, "a=ice-lite")) {
    return -1;
  }
#endif

  return sdp_append(sdp, "a=group:BUNDLE%s%s%s",
                    b_video ? " video" : "",
                    b_audio ? " audio" : "",
                    b_datachannel ? " datachannel" : "");
}
