#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "address.h"
#include "peer_connection.h"
#include "rtp.h"
#include "utils.h"

typedef enum RtpH264Type {

  NALU = 23,
  FU_A = 28,

} RtpH264Type;

typedef struct NaluHeader {
  uint8_t type : 5;
  uint8_t nri : 2;
  uint8_t f : 1;
} NaluHeader;

typedef struct FuHeader {
  uint8_t type : 5;
  uint8_t r : 1;
  uint8_t e : 1;
  uint8_t s : 1;
} FuHeader;

#define RTP_PAYLOAD_SIZE (CONFIG_MTU - sizeof(RtpHeader))
#define FU_PAYLOAD_SIZE (CONFIG_MTU - sizeof(RtpHeader) - sizeof(FuHeader) - sizeof(NaluHeader))

int rtp_packet_validate(uint8_t* packet, size_t size) {
  size_t header_size;
  uint8_t payload_type;

  if (!packet || size < sizeof(RtpHeader) || (packet[0] >> 6) != 2)
    return 0;

  header_size = sizeof(RtpHeader) + (size_t)(packet[0] & 0x0f) * 4;
  if (header_size > size)
    return 0;

  if (packet[0] & 0x10) {
    size_t extension_words;

    if (size - header_size < 4)
      return 0;
    extension_words = ((size_t)packet[header_size + 2] << 8) |
                      packet[header_size + 3];
    if (extension_words > (size - header_size - 4) / 4)
      return 0;
  }

  payload_type = packet[1] & 0x7f;
  return payload_type < 64 || payload_type >= 96;
}

uint32_t rtp_get_ssrc(uint8_t* packet) {
  uint32_t ssrc;

  memcpy(&ssrc, packet + 8, sizeof(ssrc));
  return ntohl(ssrc);
}

static int rtp_get_payload(uint8_t* packet, size_t size, uint8_t** payload,
                           size_t* payload_size, uint16_t* sequence,
                           uint32_t* timestamp) {
  size_t header_size;
  size_t padding_size = 0;

  if (!payload || !payload_size || !rtp_packet_validate(packet, size))
    return -1;

  header_size = sizeof(RtpHeader) + (size_t)(packet[0] & 0x0f) * 4;
  if (packet[0] & 0x10) {
    size_t extension_words = ((size_t)packet[header_size + 2] << 8) |
                             packet[header_size + 3];
    header_size += 4 + extension_words * 4;
  }

  if (packet[0] & 0x20) {
    padding_size = packet[size - 1];
    if (padding_size == 0 || padding_size > size - header_size)
      return -1;
  }

  *payload = packet + header_size;
  *payload_size = size - header_size - padding_size;
  if (sequence)
    *sequence = ((uint16_t)packet[2] << 8) | packet[3];
  if (timestamp) {
    *timestamp = ((uint32_t)packet[4] << 24) |
                 ((uint32_t)packet[5] << 16) |
                 ((uint32_t)packet[6] << 8) | packet[7];
  }
  return 0;
}

static int rtp_encoder_encode_h264_single(RtpEncoder* rtp_encoder, const uint8_t* buf, size_t size) {
  RtpPacket* rtp_packet = (RtpPacket*)rtp_encoder->buf;

  if (!buf || size == 0 || size > RTP_PAYLOAD_SIZE || !rtp_encoder->on_packet) {
    return -1;
  }

  rtp_packet->header.version = 2;
  rtp_packet->header.padding = 0;
  rtp_packet->header.extension = 0;
  rtp_packet->header.csrccount = 0;
  rtp_packet->header.markerbit = 0;
  rtp_packet->header.type = rtp_encoder->type;
  rtp_packet->header.seq_number = htons(rtp_encoder->seq_number++);
  rtp_packet->header.timestamp = htonl(rtp_encoder->timestamp);
  rtp_packet->header.ssrc = htonl(rtp_encoder->ssrc);

  // I frame and P frame
  if ((*buf & 0x1f) == 0x05 || (*buf & 0x1f) == 0x01) {
    rtp_packet->header.markerbit = 1;
    rtp_encoder->timestamp += rtp_encoder->timestamp_increment;
  }
#if 0
  LOGI("markbit: %d, timestamp: %d, nalu type: %d", rtp_packet->header.markerbit, rtp_encoder->timestamp, buf[0] & 0x1f);
#endif

  memcpy(rtp_packet->payload, buf, size);
  rtp_encoder->on_packet(rtp_encoder->buf, size + sizeof(RtpHeader), rtp_encoder->user_data);
  return 0;
}

static int rtp_encoder_encode_h264_fu_a(RtpEncoder* rtp_encoder, const uint8_t* buf, size_t size) {
  RtpPacket* rtp_packet = (RtpPacket*)rtp_encoder->buf;

  if (!buf || size <= 1 || !rtp_encoder->on_packet) {
    return -1;
  }

  rtp_packet->header.version = 2;
  rtp_packet->header.padding = 0;
  rtp_packet->header.extension = 0;
  rtp_packet->header.csrccount = 0;
  rtp_packet->header.markerbit = 0;
  rtp_packet->header.type = rtp_encoder->type;
  rtp_packet->header.timestamp = htonl(rtp_encoder->timestamp);
  rtp_packet->header.ssrc = htonl(rtp_encoder->ssrc);
  uint8_t type = buf[0] & 0x1f;
  uint8_t nri = (buf[0] & 0x60) >> 5;
  buf = buf + 1;
  size = size - 1;

  // increase timestamp if I, P frame
  if (type == 0x05 || type == 0x01) {
    rtp_encoder->timestamp += rtp_encoder->timestamp_increment;
  }

  NaluHeader* fu_indicator = (NaluHeader*)rtp_packet->payload;
  FuHeader* fu_header = (FuHeader*)rtp_packet->payload + sizeof(NaluHeader);
  fu_header->s = 1;

  while (size > 0) {
    fu_indicator->type = FU_A;
    fu_indicator->nri = nri;
    fu_indicator->f = 0;
    fu_header->type = type;
    fu_header->r = 0;
    rtp_packet->header.seq_number = htons(rtp_encoder->seq_number++);

    if (size <= FU_PAYLOAD_SIZE) {
      fu_header->e = 1;
      rtp_packet->header.markerbit = 1;
      memcpy(rtp_packet->payload + sizeof(NaluHeader) + sizeof(FuHeader), buf, size);
      rtp_encoder->on_packet(rtp_encoder->buf, size + sizeof(RtpHeader) + sizeof(NaluHeader) + sizeof(FuHeader), rtp_encoder->user_data);
      break;
    }

    fu_header->e = 0;

    memcpy(rtp_packet->payload + sizeof(NaluHeader) + sizeof(FuHeader), buf, FU_PAYLOAD_SIZE);
    rtp_encoder->on_packet(rtp_encoder->buf, CONFIG_MTU, rtp_encoder->user_data);
    size -= FU_PAYLOAD_SIZE;
    buf += FU_PAYLOAD_SIZE;

    fu_header->s = 0;
  }
  return 0;
}

static const uint8_t* h264_find_nalu(const uint8_t* buf_start, const uint8_t* buf_end) {
  const uint8_t* p;

  if ((size_t)(buf_end - buf_start) < 3) {
    return buf_end;
  }
  p = buf_start + 2;

  while (p < buf_end) {
    if (*(p - 2) == 0x00 && *(p - 1) == 0x00 && *p == 0x01)
      return p + 1;
    p++;
  }

  return buf_end;
}

static int rtp_encoder_encode_h264(RtpEncoder* rtp_encoder, const uint8_t* buf, size_t size) {
  const uint8_t* buf_end;
  const uint8_t *pstart, *pend;
  size_t nalu_size;

  if (!buf || size < 4) {
    return -1;
  }
  buf_end = buf + size;

  for (pstart = h264_find_nalu(buf, buf_end); pstart < buf_end; pstart = pend) {
    pend = h264_find_nalu(pstart, buf_end);
    nalu_size = pend - pstart;

    if (pend != buf_end)
      nalu_size--;

    while (nalu_size > 0 && pstart[nalu_size - 1] == 0x00)
      nalu_size--;

    if (nalu_size == 0) {
      continue;
    }

    if (nalu_size <= RTP_PAYLOAD_SIZE) {
      rtp_encoder_encode_h264_single(rtp_encoder, pstart, nalu_size);

    } else {
      rtp_encoder_encode_h264_fu_a(rtp_encoder, pstart, nalu_size);
    }
  }

  return 0;
}

static int rtp_encoder_encode_generic(RtpEncoder* rtp_encoder, const uint8_t* buf, size_t size) {
  RtpHeader* rtp_header = (RtpHeader*)rtp_encoder->buf;

  if (!buf || !rtp_encoder->on_packet ||
      size > sizeof(rtp_encoder->buf) - sizeof(RtpHeader)) {
    return -1;
  }
  rtp_header->version = 2;
  rtp_header->padding = 0;
  rtp_header->extension = 0;
  rtp_header->csrccount = 0;
  rtp_header->markerbit = 0;
  rtp_header->type = rtp_encoder->type;
  rtp_header->seq_number = htons(rtp_encoder->seq_number++);
  rtp_header->timestamp = htonl(rtp_encoder->timestamp);
  rtp_encoder->timestamp += rtp_encoder->timestamp_increment;
  rtp_header->ssrc = htonl(rtp_encoder->ssrc);
  memcpy(rtp_encoder->buf + sizeof(RtpHeader), buf, size);

  rtp_encoder->on_packet(rtp_encoder->buf, size + sizeof(RtpHeader), rtp_encoder->user_data);

  return 0;
}

void rtp_encoder_init(RtpEncoder* rtp_encoder, MediaCodec codec, RtpOnPacket on_packet, void* user_data) {
  memset(rtp_encoder, 0, sizeof(*rtp_encoder));
  rtp_encoder->on_packet = on_packet;
  rtp_encoder->user_data = user_data;
  rtp_encoder->timestamp = 0;
  rtp_encoder->seq_number = 0;

  switch (codec) {
    case CODEC_H264:
      rtp_encoder->type = PT_H264;
      rtp_encoder->ssrc = SSRC_H264;
      rtp_encoder->timestamp_increment = 90000 / 30;  // 30 FPS.
      rtp_encoder->encode_func = rtp_encoder_encode_h264;
      break;
    case CODEC_PCMA:
      rtp_encoder->type = PT_PCMA;
      rtp_encoder->ssrc = SSRC_PCMA;
      rtp_encoder->timestamp_increment = AUDIO_LATENCY * 8000 / 1000;
      rtp_encoder->encode_func = rtp_encoder_encode_generic;
      break;
    case CODEC_PCMU:
      rtp_encoder->type = PT_PCMU;
      rtp_encoder->ssrc = SSRC_PCMU;
      rtp_encoder->timestamp_increment = AUDIO_LATENCY * 8000 / 1000;
      rtp_encoder->encode_func = rtp_encoder_encode_generic;
      break;
    case CODEC_OPUS:
      rtp_encoder->type = PT_OPUS;
      rtp_encoder->ssrc = SSRC_OPUS;
      rtp_encoder->timestamp_increment = AUDIO_LATENCY * 48000 / 1000;
      rtp_encoder->encode_func = rtp_encoder_encode_generic;
      break;
    case CODEC_MJPEG:
      rtp_encoder->type = PT_H264;  // Reuse H264 payload type for MJPEG over RTP
      rtp_encoder->ssrc = SSRC_H264;
      rtp_encoder->timestamp_increment = 90000 / 30;  // 30 FPS
      rtp_encoder->encode_func = rtp_encoder_encode_generic;
      break;
    default:
      break;
  }
}

int rtp_encoder_encode(RtpEncoder* rtp_encoder, const uint8_t* buf, size_t size) {
  if (!rtp_encoder || rtp_encoder->encode_func == NULL)
    return -1;
  return rtp_encoder->encode_func(rtp_encoder, buf, size);
}

/* H264 RTP depacketizer - handles single NALU and FU-A */
static int rtp_decode_h264(RtpDecoder* rtp_decoder, uint8_t* buf, size_t size) {
  uint8_t* payload;
  size_t payload_size;
  uint16_t sequence;
  uint32_t timestamp;
  uint8_t nalu_type;

  if (!rtp_decoder->h264_buffer ||
      rtp_get_payload(buf, size, &payload, &payload_size, &sequence,
                      &timestamp) != 0 ||
      payload_size < 1) {
    return -1;
  }

  nalu_type = payload[0] & 0x1f;
  if (nalu_type == FU_A) {
    size_t fragment_size;
    uint8_t fu_header;
    int is_start;
    int is_end;

    if (payload_size <= 2) {
      rtp_decoder->h264_assembling = 0;
      rtp_decoder->h264_offset = 0;
      return -1;
    }

    fu_header = payload[1];
    is_start = (fu_header & 0x80) != 0;
    is_end = (fu_header & 0x40) != 0;
    fragment_size = payload_size - 2;

    if (is_start) {
      rtp_decoder->h264_buffer[0] = 0x00;
      rtp_decoder->h264_buffer[1] = 0x00;
      rtp_decoder->h264_buffer[2] = 0x00;
      rtp_decoder->h264_buffer[3] = 0x01;
      rtp_decoder->h264_buffer[4] =
          (payload[0] & 0xe0) | (fu_header & 0x1f);
      rtp_decoder->h264_offset = 5;
      rtp_decoder->h264_timestamp = timestamp;
      rtp_decoder->h264_expected_seq = (uint16_t)(sequence + 1);
      rtp_decoder->h264_assembling = 1;
    } else if (!rtp_decoder->h264_assembling ||
               sequence != rtp_decoder->h264_expected_seq ||
               timestamp != rtp_decoder->h264_timestamp) {
      rtp_decoder->h264_assembling = 0;
      rtp_decoder->h264_offset = 0;
      return -1;
    } else {
      rtp_decoder->h264_expected_seq = (uint16_t)(sequence + 1);
    }

    if (fragment_size >
        rtp_decoder->h264_buffer_capacity - rtp_decoder->h264_offset) {
      rtp_decoder->h264_assembling = 0;
      rtp_decoder->h264_offset = 0;
      return -1;
    }

    memcpy(rtp_decoder->h264_buffer + rtp_decoder->h264_offset, payload + 2,
           fragment_size);
    rtp_decoder->h264_offset += fragment_size;

    if (is_end) {
      if (rtp_decoder->on_packet)
        rtp_decoder->on_packet(rtp_decoder->h264_buffer,
                               rtp_decoder->h264_offset,
                               rtp_decoder->user_data);
      rtp_decoder->h264_assembling = 0;
      rtp_decoder->h264_offset = 0;
    }
  } else if (nalu_type == 24) {
    size_t offset = 1;

    rtp_decoder->h264_assembling = 0;
    rtp_decoder->h264_offset = 0;
    while (offset < payload_size) {
      size_t nalu_size;

      if (payload_size - offset < 2)
        return -1;
      nalu_size = ((size_t)payload[offset] << 8) | payload[offset + 1];
      offset += 2;
      if (nalu_size == 0 || nalu_size > payload_size - offset ||
          nalu_size > rtp_decoder->h264_buffer_capacity - 4)
        return -1;

      rtp_decoder->h264_buffer[0] = 0x00;
      rtp_decoder->h264_buffer[1] = 0x00;
      rtp_decoder->h264_buffer[2] = 0x00;
      rtp_decoder->h264_buffer[3] = 0x01;
      memcpy(rtp_decoder->h264_buffer + 4, payload + offset, nalu_size);
      if (rtp_decoder->on_packet)
        rtp_decoder->on_packet(rtp_decoder->h264_buffer, nalu_size + 4,
                               rtp_decoder->user_data);
      offset += nalu_size;
    }
  } else if (nalu_type >= 1 && nalu_type <= NALU) {
    rtp_decoder->h264_assembling = 0;
    rtp_decoder->h264_offset = 0;
    if (payload_size > rtp_decoder->h264_buffer_capacity - 4)
      return -1;

    rtp_decoder->h264_buffer[0] = 0x00;
    rtp_decoder->h264_buffer[1] = 0x00;
    rtp_decoder->h264_buffer[2] = 0x00;
    rtp_decoder->h264_buffer[3] = 0x01;
    memcpy(rtp_decoder->h264_buffer + 4, payload, payload_size);
    if (rtp_decoder->on_packet)
      rtp_decoder->on_packet(rtp_decoder->h264_buffer, payload_size + 4,
                             rtp_decoder->user_data);
  } else {
    rtp_decoder->h264_assembling = 0;
    rtp_decoder->h264_offset = 0;
    return -1;
  }

  return (int)size;
}

static int rtp_decode_generic(RtpDecoder* rtp_decoder, uint8_t* buf, size_t size) {
  uint8_t* payload;
  size_t payload_size;

  if (rtp_get_payload(buf, size, &payload, &payload_size, NULL, NULL) != 0) {
    return -1;
  }
  if (rtp_decoder->on_packet != NULL)
    rtp_decoder->on_packet(payload, payload_size, rtp_decoder->user_data);
  return (int)size;
}

int rtp_decoder_init(RtpDecoder* rtp_decoder, MediaCodec codec,
                     RtpOnPacket on_packet, void* user_data) {
  if (!rtp_decoder)
    return -1;

  memset(rtp_decoder, 0, sizeof(*rtp_decoder));
  rtp_decoder->on_packet = on_packet;
  rtp_decoder->user_data = user_data;

  switch (codec) {
    case CODEC_H264:
      if (CONFIG_H264_REASSEMBLY_SIZE == 0 ||
          CONFIG_H264_REASSEMBLY_SIZE > SIZE_MAX - 4)
        return -1;
      rtp_decoder->h264_buffer_capacity = CONFIG_H264_REASSEMBLY_SIZE + 4;
      rtp_decoder->h264_buffer = malloc(rtp_decoder->h264_buffer_capacity);
      if (!rtp_decoder->h264_buffer) {
        rtp_decoder->h264_buffer_capacity = 0;
        return -1;
      }
      rtp_decoder->decode_func = rtp_decode_h264;
      break;
    case CODEC_PCMA:
    case CODEC_PCMU:
    case CODEC_OPUS:
    case CODEC_MJPEG:
      rtp_decoder->decode_func = rtp_decode_generic;
      break;
    default:
      return -1;
  }
  return 0;
}

void rtp_decoder_deinit(RtpDecoder* rtp_decoder) {
  if (!rtp_decoder)
    return;

  free(rtp_decoder->h264_buffer);
  memset(rtp_decoder, 0, sizeof(*rtp_decoder));
}

int rtp_decoder_decode(RtpDecoder* rtp_decoder, uint8_t* buf, size_t size) {
  if (!rtp_decoder || rtp_decoder->decode_func == NULL)
    return -1;
  return rtp_decoder->decode_func(rtp_decoder, buf, size);
}
