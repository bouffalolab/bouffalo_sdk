#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "agent.h"
#include "buffer.h"
#include "config.h"
#include "dtls_srtp.h"
#include "ice.h"
#include "peer_connection.h"
#include "ports.h"
#include "rtcp.h"
#include "rtp.h"
#include "sctp.h"
#include "sdp.h"
#include "utils.h"

#define STATE_CHANGED(pc, next_state)                                      \
  do {                                                                      \
    if ((pc)->state != (next_state)) {                                      \
      (pc)->state = (next_state);                                           \
      if ((pc)->oniceconnectionstatechange) {                               \
        (pc)->oniceconnectionstatechange((next_state), (pc)->config.user_data); \
      }                                                                     \
    }                                                                       \
  } while (0)

struct PeerConnection {
  PeerConfiguration config;
  PeerConnectionState state;
  Agent agent;
  DtlsSrtp dtls_srtp;
  Sctp sctp;

  char sdp[CONFIG_SDP_BUFFER_SIZE];

  void (*onicecandidate)(char* sdp, void* user_data);
  void (*oniceconnectionstatechange)(PeerConnectionState state, void* user_data);
  void (*on_connected)(void* userdata);
  void (*on_receiver_packet_loss)(float fraction_loss, uint32_t total_loss, void* user_data);

  uint8_t temp_buf[CONFIG_MTU];
  uint8_t agent_buf[CONFIG_MTU];
  int agent_ret;
  int b_local_description_created;
  int agent_created;
  int dtls_initialized;
  int has_local_description;
  SdpType local_sdp_type;

  Buffer* audio_rb;
  Buffer* video_rb;
  Buffer* data_rb;

  RtpEncoder artp_encoder;
  RtpEncoder vrtp_encoder;
  RtpDecoder vrtp_decoder;
  RtpDecoder artp_decoder;

  uint32_t remote_assrc;
  uint32_t remote_vssrc;
  uint32_t dtls_started_ms;
  DtlsSrtpRole answer_dtls_role;
};

static void peer_connection_outgoing_rtp_packet(uint8_t* data, size_t size, void* user_data) {
  PeerConnection* pc = (PeerConnection*)user_data;
  int packet_size;

  if (size > INT_MAX) {
    return;
  }
  packet_size = (int)size;
  if (dtls_srtp_encrypt_rtp_packet(&pc->dtls_srtp, data, &packet_size) !=
      srtp_err_status_ok) {
    LOGE("Failed to protect RTP packet");
    return;
  }
  agent_send(&pc->agent, data, packet_size);
}

static int peer_connection_is_dtls_record(const uint8_t* buf, int len) {
  return len > 0 && buf[0] >= 20 && buf[0] <= 63;
}

static int peer_connection_dtls_srtp_recv(void* ctx, unsigned char* buf, size_t len) {
  DtlsSrtp* dtls_srtp = (DtlsSrtp*)ctx;
  PeerConnection* pc = (PeerConnection*)dtls_srtp->user_data;
  int ret;

  if (pc->agent_ret > 0) {
    if ((size_t)pc->agent_ret > len) {
      return MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL;
    }
    ret = pc->agent_ret;
    memcpy(buf, pc->agent_buf, ret);
    pc->agent_ret = 0;
  } else {
    /* After the handshake the owner demultiplexes RTP/STUN first. A TLS read
     * must not consume an unrelated media packet while draining cached records. */
    if (pc->state != PEER_CONNECTION_CONNECTED) {
      return MBEDTLS_ERR_SSL_WANT_READ;
    }
    ret = agent_recv(&pc->agent, buf, len);
    if (ret <= 0) {
      return ret == 0 ? MBEDTLS_ERR_SSL_WANT_READ : MBEDTLS_ERR_SSL_INTERNAL_ERROR;
    }
  }
  if (!peer_connection_is_dtls_record(buf, ret)) {
    return MBEDTLS_ERR_SSL_WANT_READ;
  }
  size_t record_len = (size_t)ret;
  if (dtls_srtp_reassemble_client_hello(dtls_srtp, buf, &record_len, len) < 0) {
    return MBEDTLS_ERR_SSL_WANT_READ;
  }
  return (int)record_len;
}

static int peer_connection_dtls_srtp_send(void* ctx, const uint8_t* buf, size_t len) {
  DtlsSrtp* dtls_srtp = (DtlsSrtp*)ctx;
  PeerConnection* pc = (PeerConnection*)dtls_srtp->user_data;

  // LOGD("send %.4x %.4x, %ld", *(uint16_t*)buf, *(uint16_t*)(buf + 2), len);
  return agent_send(&pc->agent, buf, len);
}

static void peer_connection_incoming_rtcp(PeerConnection* pc, uint8_t* buf, size_t len) {
  RtcpHeader* rtcp_header;
  size_t pos = 0;

  while (pos < len) {
    rtcp_header = (RtcpHeader*)(buf + pos);

    switch (rtcp_header->type) {
      case RTCP_RR:
        LOGD("RTCP_PR");
        if (rtcp_header->rc > 0) {
// TODO: REMB, GCC ...etc
#if 0
          RtcpRr rtcp_rr = rtcp_parse_rr(buf);
          uint32_t fraction = ntohl(rtcp_rr.report_block[0].flcnpl) >> 24;
          uint32_t total = ntohl(rtcp_rr.report_block[0].flcnpl) & 0x00FFFFFF;
          if(pc->on_receiver_packet_loss && fraction > 0) {

            pc->on_receiver_packet_loss((float)fraction/256.0, total, pc->config.user_data);
          }
#endif
        }
        break;
      case RTCP_PSFB: {
        int fmt = rtcp_header->rc;
        LOGD("RTCP_PSFB %d", fmt);
        // PLI and FIR
        if ((fmt == 1 || fmt == 4) && pc->config.on_request_keyframe) {
          pc->config.on_request_keyframe(pc->config.user_data);
        }
      }
      default:
        break;
    }

    pos += 4 * ntohs(rtcp_header->length) + 4;
  }
}

const char* peer_connection_state_to_string(PeerConnectionState state) {
  switch (state) {
    case PEER_CONNECTION_NEW:
      return "new";
    case PEER_CONNECTION_CHECKING:
      return "checking";
    case PEER_CONNECTION_CONNECTED:
      return "connected";
    case PEER_CONNECTION_COMPLETED:
      return "completed";
    case PEER_CONNECTION_FAILED:
      return "failed";
    case PEER_CONNECTION_CLOSED:
      return "closed";
    case PEER_CONNECTION_DISCONNECTED:
      return "disconnected";
    default:
      return "unknown";
  }
}

PeerConnectionState peer_connection_get_state(PeerConnection* pc) {
  return pc->state;
}

void* peer_connection_get_sctp(PeerConnection* pc) {
  return &pc->sctp;
}

PeerConnection* peer_connection_create(PeerConfiguration* config) {
  if (!config) {
    return NULL;
  }

  PeerConnection* pc = calloc(1, sizeof(PeerConnection));
  if (!pc) {
    return NULL;
  }

  memcpy(&pc->config, config, sizeof(PeerConfiguration));
  pc->state = PEER_CONNECTION_NEW;

  pc->agent.udp_sockets[0].fd = -1;
  pc->agent.udp_sockets[1].fd = -1;
  if (agent_create(&pc->agent) < 0) {
    free(pc);
    return NULL;
  }
  pc->agent_created = 1;

  memset(&pc->sctp, 0, sizeof(pc->sctp));

  if (pc->config.datachannel) {
#if (CONFIG_DATA_BUFFER_SIZE) > 0
    LOGI("Datachannel allocates heap size: %d", CONFIG_DATA_BUFFER_SIZE);
    pc->data_rb = buffer_new(CONFIG_DATA_BUFFER_SIZE);
    if (!pc->data_rb) {
      peer_connection_destroy(pc);
      return NULL;
    }
#endif
  }

  if (pc->config.audio_codec) {
#if (CONFIG_AUDIO_BUFFER_SIZE) > 0
    LOGI("Audio allocates heap size: %d", CONFIG_AUDIO_BUFFER_SIZE);
    pc->audio_rb = buffer_new(CONFIG_AUDIO_BUFFER_SIZE);
    if (!pc->audio_rb) {
      peer_connection_destroy(pc);
      return NULL;
    }
#endif

    rtp_encoder_init(&pc->artp_encoder, pc->config.audio_codec,
                     peer_connection_outgoing_rtp_packet, (void*)pc);

    if (rtp_decoder_init(&pc->artp_decoder, pc->config.audio_codec,
                         pc->config.onaudiotrack, pc->config.user_data) != 0) {
      peer_connection_destroy(pc);
      return NULL;
    }
  }

  if (pc->config.video_codec) {
#if (CONFIG_VIDEO_BUFFER_SIZE) > 0
    LOGI("Video allocates heap size: %d", CONFIG_VIDEO_BUFFER_SIZE);
    pc->video_rb = buffer_new(CONFIG_VIDEO_BUFFER_SIZE);
    if (!pc->video_rb) {
      peer_connection_destroy(pc);
      return NULL;
    }
#endif
    rtp_encoder_init(&pc->vrtp_encoder, pc->config.video_codec,
                     peer_connection_outgoing_rtp_packet, (void*)pc);

    if (rtp_decoder_init(&pc->vrtp_decoder, pc->config.video_codec,
                         pc->config.onvideotrack, pc->config.user_data) != 0) {
      peer_connection_destroy(pc);
      return NULL;
    }
  }

  return pc;
}

void peer_connection_destroy(PeerConnection* pc) {
  if (pc) {
    rtp_decoder_deinit(&pc->artp_decoder);
    rtp_decoder_deinit(&pc->vrtp_decoder);
    sctp_destroy_association(&pc->sctp);
    if (pc->dtls_initialized) {
      dtls_srtp_deinit(&pc->dtls_srtp);
    }
    if (pc->agent_created) {
      agent_destroy(&pc->agent);
    }
    if (pc->data_rb) buffer_free(pc->data_rb);
    if (pc->audio_rb) buffer_free(pc->audio_rb);
    if (pc->video_rb) buffer_free(pc->video_rb);

    free(pc);
    pc = NULL;
  }
}

void peer_connection_close(PeerConnection* pc) {
  pc->state = PEER_CONNECTION_CLOSED;
}

int peer_connection_send_audio(PeerConnection* pc, const uint8_t* buf, size_t len) {
  if (!pc || !buf || len == 0 || pc->state != PEER_CONNECTION_COMPLETED) {
    // LOGE("dtls_srtp not connected");
    return -1;
  }
#if (CONFIG_AUDIO_BUFFER_SIZE) > 0
  return buffer_push_tail(pc->audio_rb, buf, len);
#else
  return rtp_encoder_encode(&pc->artp_encoder, buf, len);
#endif
}

int peer_connection_send_video(PeerConnection* pc, const uint8_t* buf, size_t len) {
  if (!pc || !buf || len == 0 || pc->state != PEER_CONNECTION_COMPLETED) {
    // LOGE("dtls_srtp not connected");
    return -1;
  }
#if (CONFIG_VIDEO_BUFFER_SIZE) > 0
  return buffer_push_tail(pc->video_rb, buf, len);
#else
  return rtp_encoder_encode(&pc->vrtp_encoder, buf, len);
#endif
}

int peer_connection_datachannel_send(PeerConnection* pc, char* message, size_t len) {
  return peer_connection_datachannel_send_sid(pc, message, len, 0);
}

int peer_connection_datachannel_send_sid(PeerConnection* pc, char* message, size_t len, uint16_t sid) {
  if (!pc || !message || len == 0 || len > INT_MAX ||
      pc->state != PEER_CONNECTION_COMPLETED ||
      !sctp_is_connected(&pc->sctp)) {
    LOGD("sctp not connected");
    return -1;
  }

#if (CONFIG_DATA_BUFFER_SIZE) > 0
  return buffer_push_tail_with_prefix(pc->data_rb, (const uint8_t*)&sid,
                                      sizeof(sid), (uint8_t*)message,
                                      (int)len);
#else
  if (pc->config.datachannel == DATA_CHANNEL_STRING)
    return sctp_outgoing_data(&pc->sctp, message, len, PPID_STRING, sid);
  else
    return sctp_outgoing_data(&pc->sctp, message, len, PPID_BINARY, sid);
#endif
}

int peer_connection_get_data_buffered_amount(PeerConnection* pc) {
#if (CONFIG_DATA_BUFFER_SIZE) > 0
  return pc && pc->data_rb ? buffer_get_used(pc->data_rb) : 0;
#else
  (void)pc;
  return 0;
#endif
}

int peer_connection_get_data_buffer_capacity(PeerConnection* pc) {
#if (CONFIG_DATA_BUFFER_SIZE) > 0
  return pc && pc->data_rb ? buffer_get_capacity(pc->data_rb) : 0;
#else
  (void)pc;
  return 0;
#endif
}

uint32_t peer_connection_get_transport_send_failures(PeerConnection* pc) {
  if (!pc) {
    return 0;
  }
  return pc->agent.udp_sockets[0].send_error_count +
         pc->agent.udp_sockets[1].send_error_count;
}

uint32_t peer_connection_get_sctp_gap_reports(PeerConnection* pc) {
  return pc ? sctp_get_sack_gap_reports(&pc->sctp) : 0;
}

uint32_t peer_connection_get_data_retransmissions(PeerConnection* pc) {
  return pc ? sctp_get_data_retransmissions(&pc->sctp) : 0;
}

uint32_t peer_connection_get_data_retry_exhaustions(PeerConnection* pc) {
  return pc ? sctp_get_data_retry_exhaustions(&pc->sctp) : 0;
}

uint32_t peer_connection_get_sctp_forward_tsn_sent(PeerConnection* pc) {
  return pc ? sctp_get_forward_tsn_sent(&pc->sctp) : 0;
}

int peer_connection_sctp_peer_supports_forward_tsn(PeerConnection* pc) {
  return pc ? sctp_peer_supports_forward_tsn(&pc->sctp) : 0;
}

int peer_connection_create_datachannel(PeerConnection* pc, DecpChannelType channel_type, uint16_t priority, uint32_t reliability_parameter, char* label, char* protocol) {
  return peer_connection_create_datachannel_sid(pc, channel_type, priority, reliability_parameter, label, protocol, 0);
}

int peer_connection_create_datachannel_sid(PeerConnection* pc, DecpChannelType channel_type, uint16_t priority, uint32_t reliability_parameter, char* label, char* protocol, uint16_t sid) {
  int rtrn = -1;
  size_t label_len;
  size_t protocol_len;
  size_t msg_size;
  char* msg;

  if (!pc || !label || !protocol || !sctp_is_connected(&pc->sctp)) {
    LOGE("sctp not connected");
    return rtrn;
  }

  //  0                   1                   2                   3
  //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |  Message Type |  Channel Type |            Priority           |
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |                    Reliability Parameter                      |
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |         Label Length          |       Protocol Length         |
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |                                                               |
  // |                             Label                             |
  // |                                                               |
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |                                                               |
  // |                            Protocol                           |
  // |                                                               |
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  label_len = strlen(label);
  protocol_len = strlen(protocol);
  if (label_len > UINT16_MAX || protocol_len > UINT16_MAX ||
      label_len + protocol_len > SIZE_MAX - 12) {
    return -1;
  }
  msg_size = 12 + label_len + protocol_len;
  uint16_t priority_big_endian = htons(priority);
  uint32_t reliability_big_endian = htonl(reliability_parameter);
  uint16_t label_length = htons((uint16_t)label_len);
  uint16_t protocol_length = htons((uint16_t)protocol_len);
  msg = calloc(1, msg_size);
  if (!msg) {
    return -1;
  }

  msg[0] = DATA_CHANNEL_OPEN;
  msg[1] = (char)channel_type;
  memcpy(msg + 2, &priority_big_endian, sizeof(uint16_t));
  memcpy(msg + 4, &reliability_big_endian, sizeof(uint32_t));
  memcpy(msg + 8, &label_length, sizeof(uint16_t));
  memcpy(msg + 10, &protocol_length, sizeof(uint16_t));
  memcpy(msg + 12, label, label_len);
  memcpy(msg + 12 + label_len, protocol, protocol_len);

  sctp_set_stream_unordered(&pc->sctp, sid,
                            ((uint8_t)channel_type & 0x80) != 0);
  sctp_set_stream_drop_on_send_failure(
      &pc->sctp, sid,
      channel_type == DATA_CHANNEL_PARTIAL_RELIABLE_REXMIT_UNORDERED &&
          reliability_parameter == 0);
  rtrn = sctp_outgoing_data(&pc->sctp, msg, msg_size, PPID_CONTROL, sid);
  LOGI("DCEP OPEN sent sid=%u label=%s", sid, label);
  free(msg);
  return rtrn;
}

uint16_t peer_connection_get_local_datachannel_sid(PeerConnection* pc) {
  return pc && pc->dtls_srtp.role == DTLS_SRTP_ROLE_SERVER ? 1 : 0;
}

static char* peer_connection_dtls_role_setup_value(DtlsSrtpRole d) {
  return d == DTLS_SRTP_ROLE_SERVER ? "a=setup:passive" : "a=setup:active";
}

/* ICE candidates belong to an SDP media section. With BUNDLE, browsers use
 * the first m-section as the transport, so each enabled section advertises
 * the shared UDP socket rather than leaving candidates only on the last one. */
static int peer_connection_append_candidates(Sdp* sdp, Agent* agent,
                                             char* description, size_t capacity) {
  for (int i = 0; i < agent->local_candidates_count; i++) {
    size_t description_len;

    ice_candidate_to_description(&agent->local_candidates[i], description,
                                 (int)capacity);
    description_len = strlen(description);
    while (description_len > 0 &&
           (description[description_len - 1] == '\r' ||
            description[description_len - 1] == '\n')) {
      description[--description_len] = '\0';
    }
    if (sdp_append(sdp, "%s", description) != 0) {
      return -1;
    }
  }
  return 0;
}

/* Some Realtime endpoints require transport attributes on each m-section
 * instead of inheriting them from the session. BUNDLE still shares one socket. */
static int peer_connection_append_transport(Sdp* sdp, PeerConnection* pc,
                                             SdpType type, DtlsSrtpRole role) {
  return sdp_append(sdp, "a=ice-ufrag:%s", pc->agent.local_ufrag) ||
         sdp_append(sdp, "a=ice-pwd:%s", pc->agent.local_upwd) ||
         sdp_append(sdp, "a=fingerprint:sha-256 %s", pc->dtls_srtp.local_fingerprint) ||
         sdp_append(sdp, "%s", type == SDP_TYPE_OFFER ?
                        "a=setup:actpass" : peer_connection_dtls_role_setup_value(role)) ||
         peer_connection_append_candidates(sdp, &pc->agent, (char*)pc->temp_buf,
                                            sizeof(pc->temp_buf));
}

static const char* peer_connection_create_sdp(PeerConnection* pc, SdpType sdp_type) {
  Sdp sdp;
  int sdp_error = 0;
  int audio_section = 0;
  DtlsSrtpRole role = DTLS_SRTP_ROLE_SERVER;

  if (!pc) {
    return NULL;
  }

  if (pc->state != PEER_CONNECTION_NEW) {
    LOGW("Local SDP replacement is unsupported; create a new connection");
    return NULL;
  }

  memset(pc->temp_buf, 0, sizeof(pc->temp_buf));
  pc->sctp.connected = 0;
  pc->sctp.stream_count = 0;
  pc->dtls_started_ms = 0;

  switch (sdp_type) {
    case SDP_TYPE_OFFER:
      role = DTLS_SRTP_ROLE_SERVER;
      agent_clear_candidates(&pc->agent);
      pc->agent.mode = AGENT_MODE_CONTROLLING;
      break;
    case SDP_TYPE_ANSWER:
      role = pc->answer_dtls_role;
      pc->agent.mode = AGENT_MODE_CONTROLLED;
      break;
    default:
      return NULL;
  }

  if (pc->dtls_initialized) {
    sctp_destroy_association(&pc->sctp);
    dtls_srtp_deinit(&pc->dtls_srtp);
    memset(&pc->dtls_srtp, 0, sizeof(pc->dtls_srtp));
    pc->dtls_initialized = 0;
  }

  pc->dtls_initialized = 1;
  if (dtls_srtp_init(&pc->dtls_srtp, role, pc) != 0) {
    LOGE("Failed to initialize DTLS-SRTP");
    return NULL;
  }
  pc->dtls_srtp.udp_recv = peer_connection_dtls_srtp_recv;
  pc->dtls_srtp.udp_send = peer_connection_dtls_srtp_send;

  /* --- Step 1: Session header --- */
  sdp_init(&sdp, pc->sdp, sizeof(pc->sdp));
  sdp_error = sdp_create(&sdp,
                         pc->config.video_codec != CODEC_NONE,
                         pc->config.audio_codec != CODEC_NONE,
                         pc->config.datachannel);

  /* --- Step 2: Gather ICE candidates --- */
  agent_gather_candidate(&pc->agent, NULL, NULL, NULL);  // host
  for (size_t i = 0; i < sizeof(pc->config.ice_servers) / sizeof(pc->config.ice_servers[0]); ++i) {
    if (pc->config.ice_servers[i].urls) {
      LOGI("ice server: %s", pc->config.ice_servers[i].urls);
      agent_gather_candidate(&pc->agent, pc->config.ice_servers[i].urls, pc->config.ice_servers[i].username, pc->config.ice_servers[i].credential);
    }
  }

  /* --- Step 3: Generate shared ICE credentials --- */
  memset(pc->agent.local_ufrag, 0, sizeof(pc->agent.local_ufrag));
  memset(pc->agent.local_upwd, 0, sizeof(pc->agent.local_upwd));
  if (utils_random_get_string(pc->agent.random, pc->agent.local_ufrag, 4) != 0 ||
      utils_random_get_string(pc->agent.random, pc->agent.local_upwd, 24) != 0) {
    LOGE("Failed to generate ICE credentials");
    memset(pc->agent.local_ufrag, 0, sizeof(pc->agent.local_ufrag));
    memset(pc->agent.local_upwd, 0, sizeof(pc->agent.local_upwd));
    return NULL;
  }

  /* --- Step 4: Media sections and their shared ICE candidates --- */
  if (pc->config.video_codec == CODEC_H264) {
    sdp_error = sdp_error || sdp_append_h264(&sdp);
    sdp_error = sdp_error ||
                peer_connection_append_transport(&sdp, pc, sdp_type, role);
  }

  switch (pc->config.audio_codec) {
    case CODEC_PCMA:
      sdp_error = sdp_error || sdp_append_pcma(&sdp);
      audio_section = 1;
      break;
    case CODEC_PCMU:
      sdp_error = sdp_error || sdp_append_pcmu(&sdp);
      audio_section = 1;
      break;
    case CODEC_OPUS:
      sdp_error = sdp_error || sdp_append_opus(&sdp);
      audio_section = 1;
      break;
    default:
      break;
  }
  if (audio_section) {
    sdp_error = sdp_error ||
                peer_connection_append_transport(&sdp, pc, sdp_type, role);
  }

  if (pc->config.datachannel) {
    sdp_error = sdp_error || sdp_append_datachannel(&sdp);
    sdp_error = sdp_error ||
                peer_connection_append_transport(&sdp, pc, sdp_type, role);
  }

  if (sdp_error || sdp.failed) {
    LOGE("Local SDP exceeds %u bytes", (unsigned int)sizeof(pc->sdp));
    return NULL;
  }

  pc->has_local_description = 1;
  pc->local_sdp_type = sdp_type;
  if (pc->onicecandidate) {
    pc->onicecandidate(pc->sdp, pc->config.user_data);
  }

  return pc->sdp;
}

const char* peer_connection_create_offer(PeerConnection* pc) {
  /* Check before STATE_CHANGED: resetting to NEW first bypasses the guard
   * in create_sdp and silently destroys an established transport. Keep the
   * legacy explicit close -> offer path available to native signaling users. */
  if (!pc || (pc->state != PEER_CONNECTION_NEW &&
              pc->state != PEER_CONNECTION_CLOSED)) {
    return NULL;
  }
  STATE_CHANGED(pc, PEER_CONNECTION_NEW);
  return peer_connection_create_sdp(pc, SDP_TYPE_OFFER);
}

const char* peer_connection_create_answer(PeerConnection* pc) {
  if (!pc) {
    return NULL;
  }
  const char* sdp = peer_connection_create_sdp(pc, SDP_TYPE_ANSWER);
  if (!sdp) {
    return NULL;
  }
  agent_update_candidate_pairs(&pc->agent);
  STATE_CHANGED(pc, PEER_CONNECTION_CHECKING);
  return sdp;
}

void peer_connection_set_local_description(PeerConnection* pc, const char* sdp, SdpType sdp_type) {
  if (pc && sdp && strlen(sdp) < CONFIG_SDP_BUFFER_SIZE) {
    strncpy(pc->sdp, sdp, CONFIG_SDP_BUFFER_SIZE - 1);
    pc->sdp[CONFIG_SDP_BUFFER_SIZE - 1] = '\0';
    pc->has_local_description = 1;
    pc->local_sdp_type = sdp_type;
  }
}

static int peer_connection_check_consent(PeerConnection* pc) {
  if (!pc || pc->state != PEER_CONNECTION_COMPLETED) return 0;
  int consent = agent_consent_check(&pc->agent);
  if (consent >= 0) return consent;
  /* Stop producers and release transport queues before notifying the owner.
   * Reconnection requires fresh ICE credentials, never a late old response. */
  sctp_destroy_association(&pc->sctp);
  if (pc->audio_rb) buffer_clear(pc->audio_rb);
  if (pc->video_rb) buffer_clear(pc->video_rb);
  if (pc->data_rb) buffer_clear(pc->data_rb);
  if (consent == AGENT_CONSENT_LOST) {
    LOGW("ICE consent lost; create a new connection to reconnect");
    STATE_CHANGED(pc, PEER_CONNECTION_DISCONNECTED);
  } else {
    LOGE("Completed ICE connection has no selected path");
    STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
  }
  return -1;
}

int peer_connection_loop(PeerConnection* pc) {
#if (CONFIG_VIDEO_BUFFER_SIZE) > 0 || (CONFIG_AUDIO_BUFFER_SIZE) > 0 || \
    (CONFIG_DATA_BUFFER_SIZE) > 0
  int bytes;
  uint8_t* data = NULL;
#endif
  uint32_t ssrc = 0;
  if (!pc) {
    return -1;
  }

  memset(pc->agent_buf, 0, sizeof(pc->agent_buf));
  pc->agent_ret = 0;
  unsigned receive_budget = 32;
  unsigned media_budget = 2;
#if !CONFIG_PEER_NONBLOCKING_RX
  int receive_draining = 0;
#endif

  switch (pc->state) {
    case PEER_CONNECTION_CHECKING: {
      /* A candidate-free SDP is valid with Trickle ICE. Keep waiting for a
       * compatible pair; the application owns the overall signaling timeout. */
      if (pc->agent.candidate_pairs_num == 0) break;
      if (agent_select_candidate_pair(&pc->agent) < 0) {
        STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
      } else if (agent_connectivity_check(&pc->agent) == 0) {
        pc->dtls_started_ms = ports_get_epoch_time();
        STATE_CHANGED(pc, PEER_CONNECTION_CONNECTED);
      }
      break;
    }

    case PEER_CONNECTION_CONNECTED: {
      if ((uint32_t)(ports_get_epoch_time() - pc->dtls_started_ms) >= CONFIG_DTLS_HANDSHAKE_TIMEOUT_MS) {
        LOGE("DTLS handshake timed out");
        STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
        break;
      }
      int result = dtls_srtp_handshake(&pc->dtls_srtp,
                                       &pc->agent.nominated_pair->remote->addr);
      if (result == 0) {
        if (pc->config.datachannel) {
          if (sctp_create_association(&pc->sctp, &pc->dtls_srtp) < 0) {
            STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
            break;
          }
          pc->sctp.userdata = pc->config.user_data;
        }
        STATE_CHANGED(pc, PEER_CONNECTION_COMPLETED);
      } else if (result != MBEDTLS_ERR_SSL_WANT_READ && result != MBEDTLS_ERR_SSL_WANT_WRITE) {
        LOGE("DTLS handshake failed: %d", result);
        STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
      }
      break;
    }
    case PEER_CONNECTION_COMPLETED: {
      int consent = peer_connection_check_consent(pc);
      if (consent < 0) break;
      /* Pending consent gates outbound application traffic, not receipt of
       * authenticated records. Drain cached DTLS as well as fresh UDP so a
       * quiet socket cannot strand already buffered control/application data. */
      if (consent == AGENT_CONSENT_PENDING) goto read_next;
      if (pc->config.datachannel) {
        sctp_poll(&pc->sctp, ports_get_epoch_time());
      }

#if (CONFIG_VIDEO_BUFFER_SIZE) > 0
      if (pc->video_rb) {
        data = buffer_peak_head(pc->video_rb, &bytes);
        if (data) {
          rtp_encoder_encode(&pc->vrtp_encoder, data, bytes);
          buffer_pop_head(pc->video_rb);
        }
      }
#endif

#if (CONFIG_AUDIO_BUFFER_SIZE) > 0
      if (pc->audio_rb) {
        data = buffer_peak_head(pc->audio_rb, &bytes);
        if (data) {
          rtp_encoder_encode(&pc->artp_encoder, data, bytes);
          buffer_pop_head(pc->audio_rb);
        }
      }
#endif

#if (CONFIG_DATA_BUFFER_SIZE) > 0
      if (pc->data_rb) {
        unsigned int batch;

        for (batch = 0; batch < CONFIG_DATA_TX_BATCH_SIZE; batch++) {
          uint16_t sid;
          int send_result = -2;

          data = buffer_peak_head(pc->data_rb, &bytes);
          if (!data) {
            break;
          }

          if (bytes <= (int)sizeof(sid)) {
            LOGE("corrupt datachannel buffer record");
          } else {
            memcpy(&sid, data, sizeof(sid));
            data += sizeof(sid);
            bytes -= sizeof(sid);
            if (pc->config.datachannel == DATA_CHANNEL_STRING)
              send_result =
                  sctp_outgoing_data(&pc->sctp, (char*)data, bytes,
                                     PPID_STRING, sid);
            else
              send_result =
                  sctp_outgoing_data(&pc->sctp, (char*)data, bytes,
                                     PPID_BINARY, sid);
          }
          /*
           * -1 means no SCTP fragment reached UDP, so preserving the record
           * provides an atomic retry on reliable channels. A real-time
           * maxRetransmits=0 stream discards that stale record instead.
           * -2 means a multi-fragment message failed after partial progress;
           * drop it because replaying from the start would duplicate data.
           */
          if (send_result != -1 ||
              sctp_stream_drops_on_send_failure(&pc->sctp, sid)) {
            buffer_pop_head(pc->data_rb);
          }
          /* Stop the batch on transport pressure, including a discarded
           * PR message. Reliable records remain queued for the next pass. */
          if (send_result < 0) {
            break;
          }
        }
      }
#endif

read_next:
      if (receive_budget-- == 0) {
        break;
      }
      if (mbedtls_ssl_check_pending(&pc->dtls_srtp.ssl)) {
        int result = dtls_srtp_read(&pc->dtls_srtp, pc->temp_buf, sizeof(pc->temp_buf));
        if (result > 0) {
          sctp_incoming_data(&pc->sctp, (char*)pc->temp_buf, result);
          goto read_next;
        }
        if (result != MBEDTLS_ERR_SSL_WANT_READ && result != MBEDTLS_ERR_SSL_WANT_WRITE) {
          STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
          break;
        }
      }
#if CONFIG_PEER_NONBLOCKING_RX
      pc->agent_ret = agent_recv_nonblocking(&pc->agent, pc->agent_buf,
                                              sizeof(pc->agent_buf));
#else
      /* Only the first poll may wait. Once draining a burst, waiting again
       * can keep this owner inside RX and starve outgoing microphone audio. */
      pc->agent_ret = receive_draining ?
          agent_recv_nonblocking(&pc->agent, pc->agent_buf, sizeof(pc->agent_buf)) :
          agent_recv(&pc->agent, pc->agent_buf, sizeof(pc->agent_buf));
      receive_draining = 1;
#endif
      if (pc->agent_ret > 0) {
        LOGD("agent_recv %d", pc->agent_ret);

        if (rtcp_probe(pc->agent_buf, pc->agent_ret)) {
          LOGD("Got RTCP packet");
          if (dtls_srtp_decrypt_rtcp_packet(&pc->dtls_srtp, pc->agent_buf,
                                            &pc->agent_ret) != srtp_err_status_ok) {
            LOGW("Discarding RTCP packet that failed SRTP authentication");
            goto read_next;
          }
          peer_connection_incoming_rtcp(pc, pc->agent_buf, pc->agent_ret);
          goto read_next;
        } else if (dtls_srtp_probe(pc->agent_buf)) {
          int ret = dtls_srtp_read(&pc->dtls_srtp, pc->temp_buf, sizeof(pc->temp_buf));
          LOGD("Got DTLS data %d", ret);

          if (ret > 0) {
            sctp_incoming_data(&pc->sctp, (char*)pc->temp_buf, ret);
          } else if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
            break;
          }
          goto read_next;
        } else if (rtp_packet_validate(pc->agent_buf, pc->agent_ret)) {
          LOGD("Got RTP packet");

          if (dtls_srtp_decrypt_rtp_packet(&pc->dtls_srtp, pc->agent_buf,
                                           &pc->agent_ret) != srtp_err_status_ok) {
            LOGW("Discarding RTP packet that failed SRTP authentication");
            goto read_next;
          }

          ssrc = rtp_get_ssrc(pc->agent_buf);
          if (ssrc == pc->remote_assrc) {
            rtp_decoder_decode(&pc->artp_decoder, pc->agent_buf, pc->agent_ret);
          } else if (ssrc == pc->remote_vssrc) {
            rtp_decoder_decode(&pc->vrtp_decoder, pc->agent_buf, pc->agent_ret);
          }
          /* Catch up media bursts while returning frequently enough for the
           * owner to send microphone audio and service other tasks. */
          if (--media_budget) goto read_next;

        } else {
          LOGW("Unknown data");
        }
      }

      /* Recheck after receiving: a matched 403 must close this same tick,
       * and the receive budget may have crossed the expiry deadline. The
       * paced tick does not generate an extra probe here. */
      if (pc->state == PEER_CONNECTION_COMPLETED)
        peer_connection_check_consent(pc);

      break;
    }
    case PEER_CONNECTION_NEW:
    case PEER_CONNECTION_FAILED:
    case PEER_CONNECTION_DISCONNECTED:
    case PEER_CONNECTION_CLOSED:
      break;
    default:
      break;
  }

  return 0;
}

void peer_connection_set_remote_description_with_type(PeerConnection* pc,
                                                      const char* sdp_text,
                                                      SdpType sdp_type) {
  const char* start = sdp_text;
  const char* line = NULL;
  char buf[256];
  char* val_start = NULL;
  uint32_t* ssrc = NULL;
  char remote_fingerprint[DTLS_SRTP_FINGERPRINT_LENGTH] = {0};
  DtlsSrtpRole role = sdp_type == SDP_TYPE_OFFER ? DTLS_SRTP_ROLE_CLIENT : DTLS_SRTP_ROLE_SERVER;
  int setup_seen = 0;

  if (!pc || !sdp_text) {
    return;
  }
  /* Renegotiation/ICE restart needs coordinated SDP, DTLS, SCTP and queue
   * state. Reject in-place replacement before modifying any live state.
   * The facade reports INVALID_STATE; direct users must create a new session. */
  if (pc->state != PEER_CONNECTION_NEW) {
    LOGW("Remote SDP replacement is unsupported; create a new connection");
    return;
  }

  /* Validate a fresh pin per description without erasing the active pin while
   * parsing. Invalid/missing pins fail closed; they never inherit an old pin. */
  while (*start) {
    size_t line_len;
    int skip = 2;

    line = strstr(start, "\r\n");
    if (!line) {
      line = strchr(start, '\n');
      skip = 1;
    }
    if (!line) {
      line = start + strlen(start);
      skip = 0;
    }

    line_len = (size_t)(line - start);
    if (line_len < sizeof(buf)) {
      memcpy(buf, start, line_len);
      buf[line_len] = '\0';
      if (strncmp(buf, "a=setup:", 8) == 0) {
        DtlsSrtpRole selected;
        if (strcmp(buf + 8, "passive") == 0) selected = DTLS_SRTP_ROLE_CLIENT;
        else if (strcmp(buf + 8, "active") == 0) selected = DTLS_SRTP_ROLE_SERVER;
        else if (sdp_type == SDP_TYPE_OFFER && strcmp(buf + 8, "actpass") == 0)
          selected = DTLS_SRTP_ROLE_CLIENT;
        else {
          STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
          return;
        }
        if (setup_seen && selected != role) {
          STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
          return;
        }
        setup_seen = 1;
        role = selected;
      }

      if (strncmp(buf, "a=fingerprint:sha-256 ",
                  strlen("a=fingerprint:sha-256 ")) == 0) {
        const char* fingerprint = buf + strlen("a=fingerprint:sha-256 ");
        size_t fingerprint_length = strlen(fingerprint);
        if (fingerprint_length != 95) {
          STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
          return;
        }
        for (size_t i = 0; i < fingerprint_length; ++i) {
          if ((i % 3 == 2) ? fingerprint[i] != ':' :
              !isxdigit((unsigned char)fingerprint[i])) {
            STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
            return;
          }
        }
        if (remote_fingerprint[0] &&
            strcasecmp(remote_fingerprint, fingerprint) != 0) {
          STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
          return;
        }
        memcpy(remote_fingerprint, fingerprint, fingerprint_length + 1);
      }

      if (strncmp(buf, "m=video", strlen("m=video")) == 0) {
        ssrc = &pc->remote_vssrc;
      } else if (strncmp(buf, "m=audio", strlen("m=audio")) == 0) {
        ssrc = &pc->remote_assrc;
      }

      if ((val_start = strstr(buf, "a=ssrc:")) && ssrc) {
        *ssrc = strtoul(val_start + 7, NULL, 10);
        LOGD("SSRC: %" PRIu32, *ssrc);
      }
    } else {
      LOGW("Ignoring overlong SDP line (%u bytes)", (unsigned int)line_len);
    }

    start = line + skip;
  }

  if (!remote_fingerprint[0] ||
      (pc->dtls_srtp.state == DTLS_SRTP_STATE_CONNECTED &&
       strcasecmp(pc->dtls_srtp.remote_fingerprint, remote_fingerprint) != 0)) {
    LOGE("Remote SDP has no fingerprint or attempts to replace a connected peer");
    STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
    return;
  }

  if (sdp_type == SDP_TYPE_OFFER) {
    pc->answer_dtls_role = role;
  } else if (!pc->dtls_initialized || dtls_srtp_set_role(&pc->dtls_srtp, role) != 0) {
    STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
    return;
  }

  agent_set_remote_description(&pc->agent, (char*)sdp_text);
  LOGI("remote ufrag=%s candidates=%d pairs=%d",
       pc->agent.remote_ufrag,
       pc->agent.remote_candidates_count,
       pc->agent.candidate_pairs_num);

  if (pc->agent.remote_ufrag[0] == '\0' ||
      pc->agent.remote_upwd[0] == '\0') {
    LOGE("Remote SDP is missing DTLS or ICE attributes");
    STATE_CHANGED(pc, PEER_CONNECTION_FAILED);
  } else {
    memcpy(pc->dtls_srtp.remote_fingerprint, remote_fingerprint, sizeof(remote_fingerprint));
    if (sdp_type == SDP_TYPE_ANSWER) STATE_CHANGED(pc, PEER_CONNECTION_CHECKING);
  }
}

void peer_connection_set_remote_description(PeerConnection* pc,
                                            const char* sdp_text) {
  SdpType remote_type = SDP_TYPE_OFFER;

  if (pc && pc->has_local_description &&
      pc->local_sdp_type == SDP_TYPE_OFFER) {
    remote_type = SDP_TYPE_ANSWER;
  }
  peer_connection_set_remote_description_with_type(pc, sdp_text, remote_type);
}

int peer_connection_send_rtcp_pil(PeerConnection* pc, uint32_t ssrc) {
  int ret = -1;
  uint8_t plibuf[128];
  (void)pc;
  rtcp_get_pli(plibuf, 12, ssrc);

  // TODO: encrypt rtcp packet
  // guint size = 12;
  // dtls_transport_encrypt_rctp_packet(pc->dtls_transport, plibuf, &size);
  // ret = nice_agent_send(pc->nice_agent, pc->stream_id, pc->component_id, size, (gchar*)plibuf);

  return ret;
}

// callbacks
void peer_connection_on_connected(PeerConnection* pc, void (*on_connected)(void* userdata)) {
  pc->on_connected = on_connected;
}

void peer_connection_on_receiver_packet_loss(PeerConnection* pc,
                                             void (*on_receiver_packet_loss)(float fraction_loss, uint32_t total_loss, void* userdata)) {
  pc->on_receiver_packet_loss = on_receiver_packet_loss;
}

void peer_connection_onicecandidate(PeerConnection* pc, void (*onicecandidate)(char* sdp_text, void* userdata)) {
  pc->onicecandidate = onicecandidate;
}

void peer_connection_oniceconnectionstatechange(PeerConnection* pc,
                                                void (*oniceconnectionstatechange)(PeerConnectionState state, void* userdata)) {
  pc->oniceconnectionstatechange = oniceconnectionstatechange;
}

void peer_connection_ondatachannel(PeerConnection* pc,
                                   void (*onmessage)(char* msg, size_t len, void* userdata, uint16_t sid),
                                   void (*onopen)(void* userdata),
                                   void (*onclose)(void* userdata)) {
  if (pc) {
    sctp_onopen(&pc->sctp, onopen);
    sctp_onclose(&pc->sctp, onclose);
    sctp_onmessage(&pc->sctp, onmessage);
  }
}

int peer_connection_lookup_sid(PeerConnection* pc, const char* label, uint16_t* sid) {
  for (int i = 0; i < pc->sctp.stream_count; i++) {
    if (strncmp(pc->sctp.stream_table[i].label, label, sizeof(pc->sctp.stream_table[i].label)) == 0) {
      *sid = pc->sctp.stream_table[i].sid;
      return 0;
    }
  }
  return -1;  // Not found
}

char* peer_connection_lookup_sid_label(PeerConnection* pc, uint16_t sid) {
  for (int i = 0; i < pc->sctp.stream_count; i++) {
    if (pc->sctp.stream_table[i].sid == sid) {
      return pc->sctp.stream_table[i].label;
    }
  }
  return NULL;  // Not found
}

int peer_connection_add_ice_candidate(PeerConnection* pc, char* candidate) {
  IceCandidate parsed;
  Agent* agent;

  if (!pc || !candidate || pc->state == PEER_CONNECTION_CLOSED ||
      pc->state == PEER_CONNECTION_FAILED ||
      pc->state == PEER_CONNECTION_DISCONNECTED) {
    return -1;
  }

  agent = &pc->agent;
  if (ice_candidate_from_description(&parsed, candidate,
                                     candidate + strlen(candidate)) != 0) {
    return -1;
  }

  for (int i = 0; i < agent->remote_candidates_count; i++) {
    if (agent->remote_candidates[i].component == parsed.component &&
        addr_equal(&agent->remote_candidates[i].addr, &parsed.addr)) {
      return 0;
    }
  }
  if (agent->remote_candidates_count >= AGENT_MAX_CANDIDATES) {
    return -1;
  }

  agent->remote_candidates[agent->remote_candidates_count++] = parsed;
  agent_update_candidate_pairs(agent);
  return 0;
}
