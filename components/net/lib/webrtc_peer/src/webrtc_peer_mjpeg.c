#include "webrtc_peer_mjpeg.h"

#include <limits.h>
#include <string.h>

static const uint8_t g_chunk_magic[4] = { 'J', 'P', 'G', 'C' };
static const uint8_t g_parity_magic[4] = { 'J', 'P', 'G', 'P' };
static const uint8_t g_legacy_magic[4] = { 'J', 'P', 'E', 'G' };

static int size_exceeds_wire_limit(size_t size)
{
#if SIZE_MAX > UINT32_MAX
    return size > UINT32_MAX;
#else
    (void)size;
    return 0;
#endif
}

static void write_be32(uint8_t *destination, uint32_t value)
{
    destination[0] = (uint8_t)(value >> 24);
    destination[1] = (uint8_t)(value >> 16);
    destination[2] = (uint8_t)(value >> 8);
    destination[3] = (uint8_t)value;
}

static uint32_t read_be32(const uint8_t *source)
{
    return ((uint32_t)source[0] << 24) |
           ((uint32_t)source[1] << 16) |
           ((uint32_t)source[2] << 8) |
           (uint32_t)source[3];
}

static int valid_jpeg(const uint8_t *jpeg, size_t size)
{
    return jpeg && size >= 4 &&
           jpeg[0] == 0xff && jpeg[1] == 0xd8 &&
           jpeg[size - 2] == 0xff && jpeg[size - 1] == 0xd9;
}

static int time_reached(uint32_t now_ms, uint32_t deadline_ms)
{
    return (int32_t)(now_ms - deadline_ms) >= 0;
}

static uint32_t chunk_interval_ms(size_t frame_size, size_t chunk_size,
                                  unsigned int target_fps)
{
    size_t chunk_count = frame_size / chunk_size;
    uint32_t frame_interval;
    uint32_t interval;

    if (target_fps == 0) {
        return 0;
    }
    if (frame_size % chunk_size != 0) {
        chunk_count++;
    }
    frame_interval = target_fps >= 1000U ? 1U :
        (1000U + target_fps / 2U) / target_fps;
    interval = (uint32_t)(frame_interval / chunk_count);
    return interval > 0 ? interval : 1;
}

int webrtc_peer_mjpeg_sender_init(
    webrtc_peer_mjpeg_sender_t *sender,
    const webrtc_peer_mjpeg_sender_config_t *config)
{
    if (!sender || !config || !config->chunk_buffer || !config->send ||
        config->chunk_payload_size == 0 || config->max_frame_size < 4 ||
        size_exceeds_wire_limit(config->max_frame_size) ||
        config->retry_timeout_ms > INT32_MAX ||
        config->chunk_buffer_size < WEBRTC_PEER_MJPEG_CHUNK_HEADER_SIZE ||
        config->chunk_payload_size > config->chunk_buffer_size -
            WEBRTC_PEER_MJPEG_CHUNK_HEADER_SIZE ||
        (config->enable_xor_parity &&
         size_exceeds_wire_limit(config->chunk_payload_size))) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }

    memset(sender, 0, sizeof(*sender));
    sender->config = *config;
    return WEBRTC_PEER_OK;
}

int webrtc_peer_mjpeg_sender_set_target_fps(
    webrtc_peer_mjpeg_sender_t *sender, unsigned int target_fps)
{
    if (!sender || !sender->config.send) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    sender->config.target_fps = target_fps;
    return WEBRTC_PEER_OK;
}

int webrtc_peer_mjpeg_sender_submit(
    webrtc_peer_mjpeg_sender_t *sender,
    const uint8_t *jpeg, size_t size, uint32_t now_ms)
{
    if (!sender || !sender->config.send || !jpeg || size == 0) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    if (sender->active) {
        return WEBRTC_PEER_ERR_WOULD_BLOCK;
    }
    if (size > sender->config.max_frame_size || size_exceeds_wire_limit(size) ||
        !valid_jpeg(jpeg, size)) {
        sender->stats.invalid_frames++;
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }

    sender->frame = jpeg;
    sender->frame_size = size;
    sender->frame_offset = 0;
    sender->frame_id = sender->next_frame_id++;
    sender->next_send_ms = now_ms;
    sender->chunk_interval_ms = chunk_interval_ms(
        size, sender->config.chunk_payload_size,
        sender->config.target_fps);
    sender->last_send_error = WEBRTC_PEER_OK;
    sender->parity_pending = 0;
    sender->chunk_retries = 0;
    sender->active = 1;
    sender->stats.frames_submitted++;
    return WEBRTC_PEER_OK;
}

static void sender_finish_frame(webrtc_peer_mjpeg_sender_t *sender)
{
    sender->stats.frames_sent++;
    sender->stats.frame_bytes_sent += sender->frame_size;
    sender->frame = NULL;
    sender->frame_size = 0;
    sender->frame_offset = 0;
    sender->parity_pending = 0;
    sender->active = 0;
}

static webrtc_peer_mjpeg_process_result_t sender_drop_frame(
    webrtc_peer_mjpeg_sender_t *sender)
{
    sender->stats.frames_dropped++;
    sender->frame = NULL;
    sender->frame_size = 0;
    sender->frame_offset = 0;
    sender->parity_pending = 0;
    sender->chunk_retries = 0;
    sender->active = 0;
    return WEBRTC_PEER_MJPEG_PROCESS_FRAME_DROPPED;
}

static void sender_build_xor_parity(webrtc_peer_mjpeg_sender_t *sender)
{
    uint8_t *chunk = sender->config.chunk_buffer;
    uint8_t *parity = chunk + WEBRTC_PEER_MJPEG_PARITY_HEADER_SIZE;
    size_t offset = 0;
    size_t chunk_size = sender->config.chunk_payload_size;

    memcpy(chunk, g_parity_magic, sizeof(g_parity_magic));
    write_be32(chunk + 4, sender->frame_id);
    write_be32(chunk + 8, (uint32_t)sender->frame_size);
    write_be32(chunk + 12, (uint32_t)chunk_size);
    memset(parity, 0, chunk_size);
    while (offset < sender->frame_size) {
        size_t remaining = sender->frame_size - offset;
        size_t length = remaining < chunk_size ? remaining : chunk_size;
        size_t i;

        for (i = 0; i < length; i++) {
            parity[i] ^= sender->frame[offset + i];
        }
        offset += length;
    }
}

webrtc_peer_mjpeg_process_result_t webrtc_peer_mjpeg_sender_process(
    webrtc_peer_mjpeg_sender_t *sender, uint32_t now_ms)
{
    size_t payload_size;
    uint8_t *chunk;
    int result;
    uint32_t retry_timeout;

    if (!sender || !sender->active ||
        !time_reached(now_ms, sender->next_send_ms)) {
        return WEBRTC_PEER_MJPEG_PROCESS_IDLE;
    }

    retry_timeout = sender->config.retry_timeout_ms ?
        sender->config.retry_timeout_ms : 20U;
    if (sender->chunk_retries &&
        (uint32_t)(now_ms - sender->retry_started_ms) >= retry_timeout) {
        return sender_drop_frame(sender);
    }

    if (sender->parity_pending) {
        sender_build_xor_parity(sender);
        result = sender->config.send(
            sender->config.chunk_buffer,
            WEBRTC_PEER_MJPEG_PARITY_HEADER_SIZE +
                sender->config.chunk_payload_size,
            sender->config.sid, sender->config.user_data);
        if (result == WEBRTC_PEER_OK) {
            sender->stats.parity_sent++;
        } else {
            /* The data frame is complete; parity loss must not hold it. */
            sender->last_send_error = result;
            sender->stats.parity_send_failures++;
        }
        sender_finish_frame(sender);
        return WEBRTC_PEER_MJPEG_PROCESS_FRAME_SENT;
    }

    payload_size = sender->frame_size - sender->frame_offset;
    if (payload_size > sender->config.chunk_payload_size) {
        payload_size = sender->config.chunk_payload_size;
    }
    chunk = sender->config.chunk_buffer;
    memcpy(chunk, g_chunk_magic, sizeof(g_chunk_magic));
    write_be32(chunk + 4, sender->frame_id);
    write_be32(chunk + 8, (uint32_t)sender->frame_size);
    write_be32(chunk + 12, (uint32_t)sender->frame_offset);
    memcpy(chunk + WEBRTC_PEER_MJPEG_CHUNK_HEADER_SIZE,
           sender->frame + sender->frame_offset, payload_size);

    result = sender->config.send(
        chunk, WEBRTC_PEER_MJPEG_CHUNK_HEADER_SIZE + payload_size,
        sender->config.sid, sender->config.user_data);
    if (result != WEBRTC_PEER_OK) {
        sender->last_send_error = result;
        sender->stats.send_failures++;
        if (result == WEBRTC_PEER_ERR_WOULD_BLOCK &&
            sender->chunk_retries < sender->config.max_chunk_retries) {
            if (sender->chunk_retries == 0) {
                sender->retry_started_ms = now_ms;
            }
            sender->chunk_retries++;
            sender->stats.send_retries++;
            sender->next_send_ms = now_ms + 1U;
            return WEBRTC_PEER_MJPEG_PROCESS_IDLE;
        }
        return sender_drop_frame(sender);
    }

    sender->chunk_retries = 0;
    sender->frame_offset += payload_size;
    sender->stats.chunks_sent++;
    if (sender->frame_offset == sender->frame_size) {
        if (!sender->config.enable_xor_parity) {
            sender_finish_frame(sender);
            return WEBRTC_PEER_MJPEG_PROCESS_FRAME_SENT;
        }
        sender->parity_pending = 1;
    }

    /* Missed deadlines must not become a burst into the UDP TX queue. */
    sender->next_send_ms = now_ms + sender->chunk_interval_ms;
    return WEBRTC_PEER_MJPEG_PROCESS_CHUNK_SENT;
}

int webrtc_peer_mjpeg_sender_is_busy(
    const webrtc_peer_mjpeg_sender_t *sender)
{
    return sender && sender->active;
}

void webrtc_peer_mjpeg_sender_reset(webrtc_peer_mjpeg_sender_t *sender)
{
    if (!sender) {
        return;
    }
    if (sender->active) {
        sender->stats.frames_dropped++;
    }
    sender->frame = NULL;
    sender->frame_size = 0;
    sender->frame_offset = 0;
    sender->parity_pending = 0;
    sender->active = 0;
}

int webrtc_peer_mjpeg_sender_get_stats(
    const webrtc_peer_mjpeg_sender_t *sender,
    webrtc_peer_mjpeg_sender_stats_t *stats)
{
    if (!sender || !stats) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    *stats = sender->stats;
    return WEBRTC_PEER_OK;
}

int webrtc_peer_mjpeg_receiver_init(
    webrtc_peer_mjpeg_receiver_t *receiver,
    const webrtc_peer_mjpeg_receiver_config_t *config)
{
    if (!receiver || !config || !config->frame_buffer ||
        !config->on_frame || config->max_frame_size < 4 ||
        config->max_frame_size > UINT32_MAX ||
        config->frame_buffer_size < config->max_frame_size) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }

    memset(receiver, 0, sizeof(*receiver));
    receiver->config = *config;
    return WEBRTC_PEER_OK;
}

static int receive_complete_frame(webrtc_peer_mjpeg_receiver_t *receiver,
                                  size_t size, uint32_t frame_id)
{
    if (!valid_jpeg(receiver->config.frame_buffer, size)) {
        receiver->stats.invalid_messages++;
        receiver->stats.frames_dropped++;
        receiver->active = 0;
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }

    receiver->stats.frames_received++;
    receiver->stats.frame_bytes_received += size;
    receiver->config.on_frame(receiver->config.frame_buffer, size,
                              frame_id, receiver->config.user_data);
    receiver->active = 0;
    return WEBRTC_PEER_MJPEG_RECEIVE_FRAME;
}

int webrtc_peer_mjpeg_receiver_consume(
    webrtc_peer_mjpeg_receiver_t *receiver,
    const uint8_t *data, size_t size)
{
    uint32_t frame_id;
    size_t frame_size;
    size_t offset;
    size_t payload_size;

    if (!receiver || !data || size < 4) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }

    if (memcmp(data, g_legacy_magic, sizeof(g_legacy_magic)) == 0) {
        if (size < WEBRTC_PEER_MJPEG_LEGACY_HEADER_SIZE) {
            receiver->stats.invalid_messages++;
            return WEBRTC_PEER_ERR_INVALID_ARG;
        }
        frame_size = read_be32(data + 4);
        if (frame_size == 0 || frame_size > receiver->config.max_frame_size ||
            frame_size != size - WEBRTC_PEER_MJPEG_LEGACY_HEADER_SIZE) {
            receiver->stats.invalid_messages++;
            return WEBRTC_PEER_ERR_INVALID_ARG;
        }
        if (receiver->active) {
            receiver->stats.frames_dropped++;
        }
        memcpy(receiver->config.frame_buffer,
               data + WEBRTC_PEER_MJPEG_LEGACY_HEADER_SIZE, frame_size);
        receiver->active = 1;
        receiver->frame_size = frame_size;
        receiver->next_offset = frame_size;
        receiver->frame_id++;
        return receive_complete_frame(receiver, frame_size,
                                      receiver->frame_id);
    }

    /* Older C receivers do not reconstruct parity. Ignore a well-formed
     * parity record without losing their current JPEG assembly. */
    if (memcmp(data, g_parity_magic, sizeof(g_parity_magic)) == 0) {
        size_t parity_size;

        if (size < WEBRTC_PEER_MJPEG_PARITY_HEADER_SIZE) {
            receiver->stats.invalid_messages++;
            return WEBRTC_PEER_ERR_INVALID_ARG;
        }
        frame_size = read_be32(data + 8);
        parity_size = read_be32(data + 12);
        if (frame_size == 0 || frame_size > receiver->config.max_frame_size ||
            parity_size == 0 ||
            parity_size > size - WEBRTC_PEER_MJPEG_PARITY_HEADER_SIZE ||
            size != WEBRTC_PEER_MJPEG_PARITY_HEADER_SIZE + parity_size) {
            receiver->stats.invalid_messages++;
            return WEBRTC_PEER_ERR_INVALID_ARG;
        }
        return WEBRTC_PEER_MJPEG_RECEIVE_CHUNK;
    }

    if (size <= WEBRTC_PEER_MJPEG_CHUNK_HEADER_SIZE ||
        memcmp(data, g_chunk_magic, sizeof(g_chunk_magic)) != 0) {
        receiver->stats.invalid_messages++;
        return WEBRTC_PEER_ERR_UNSUPPORTED;
    }

    frame_id = read_be32(data + 4);
    frame_size = read_be32(data + 8);
    offset = read_be32(data + 12);
    payload_size = size - WEBRTC_PEER_MJPEG_CHUNK_HEADER_SIZE;
    if (frame_size == 0 || frame_size > receiver->config.max_frame_size ||
        offset > frame_size || payload_size > frame_size - offset) {
        receiver->stats.invalid_messages++;
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }

    if (offset == 0) {
        if (receiver->active) {
            receiver->stats.frames_dropped++;
        }
        receiver->frame_id = frame_id;
        receiver->frame_size = frame_size;
        receiver->next_offset = 0;
        receiver->active = 1;
    } else if (!receiver->active || receiver->frame_id != frame_id ||
               receiver->frame_size != frame_size ||
               receiver->next_offset != offset) {
        if (receiver->active) {
            receiver->stats.frames_dropped++;
            receiver->active = 0;
        }
        receiver->stats.invalid_messages++;
        return WEBRTC_PEER_ERR_INVALID_STATE;
    }

    memcpy(receiver->config.frame_buffer + offset,
           data + WEBRTC_PEER_MJPEG_CHUNK_HEADER_SIZE, payload_size);
    receiver->next_offset += payload_size;
    receiver->stats.chunks_received++;
    if (receiver->next_offset == receiver->frame_size) {
        return receive_complete_frame(receiver, receiver->frame_size,
                                      receiver->frame_id);
    }
    return WEBRTC_PEER_MJPEG_RECEIVE_CHUNK;
}

void webrtc_peer_mjpeg_receiver_reset(
    webrtc_peer_mjpeg_receiver_t *receiver)
{
    if (!receiver) {
        return;
    }
    if (receiver->active) {
        receiver->stats.frames_dropped++;
    }
    receiver->frame_size = 0;
    receiver->next_offset = 0;
    receiver->active = 0;
}

int webrtc_peer_mjpeg_receiver_get_stats(
    const webrtc_peer_mjpeg_receiver_t *receiver,
    webrtc_peer_mjpeg_receiver_stats_t *stats)
{
    if (!receiver || !stats) {
        return WEBRTC_PEER_ERR_INVALID_ARG;
    }
    *stats = receiver->stats;
    return WEBRTC_PEER_OK;
}
