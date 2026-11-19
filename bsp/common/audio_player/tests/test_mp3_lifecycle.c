/* Exercise the production MP3 task with controlled decoder and transport IO. */
#include "wave_player.h"
#include "pvmp3dec/pvmp3decoder_api.h"
#include <FreeRTOS.h>
#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define pdPASS                              1
#define INCLUDE_uxTaskGetStackHighWaterMark 0

static pthread_t worker;
static void (*worker_entry)(void *);
static void *worker_argument;
static pthread_mutex_t transport_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t transport_changed = PTHREAD_COND_INITIALIZER;
static atomic_bool running, hold_drain, drain_entered, stop_on_feed;
static atomic_uint queued, discarded, drains, allocations, running_queries;
static unsigned decoder_calls, read_calls, close_calls;
static size_t input_bytes, input_offset, first_read_bytes, frame_bytes;

static void *run_worker(void *unused)
{
    (void)unused;
    worker_entry(worker_argument);
    return NULL;
}

static int xTaskCreate(void (*entry)(void *), char *name, unsigned depth, void *argument, unsigned priority,
                       TaskHandle_t *task)
{
    assert(strcmp(name, "mp3_play") == 0 && depth > 0 && priority == 3);
    worker_entry = entry;
    worker_argument = argument;
    *task = &worker;
    assert(pthread_create(&worker, NULL, run_worker, NULL) == 0);
    return pdPASS;
}

static void vTaskDelay(uint32_t milliseconds)
{
    struct timespec delay = { .tv_nsec = (long)milliseconds * 1000000L };
    assert(nanosleep(&delay, NULL) == 0);
}

static void vTaskDelete(void *task)
{
    assert(task == NULL);
}

static void *pvPortMalloc(size_t bytes)
{
    void *allocation = malloc(bytes);
    assert(allocation != NULL);
    atomic_fetch_add(&allocations, 1);
    return allocation;
}

static void vPortFree(void *allocation)
{
    assert(allocation != NULL && atomic_fetch_sub(&allocations, 1) > 0);
    free(allocation);
}

void wave_player_config(uint32_t rate, uint32_t tx_bytes, uint32_t rx_bytes, uint8_t channels)
{
    assert(rate == 16000 && tx_bytes > 0 && rx_bytes == 0 && channels == 1);
}

int wave_player_start(void)
{
    atomic_store(&running, true);
    return 0;
}

bool wave_player_is_running(void)
{
    atomic_fetch_add(&running_queries, 1);
    return atomic_load(&running);
}

int wave_player_stop(void)
{
    assert(pthread_mutex_lock(&transport_lock) == 0);
    atomic_store(&running, false);
    atomic_fetch_add(&discarded, atomic_exchange(&queued, 0));
    assert(pthread_cond_broadcast(&transport_changed) == 0);
    assert(pthread_mutex_unlock(&transport_lock) == 0);
    return 0;
}

uint32_t wave_player_feed_blocking(uint8_t *data, uint32_t bytes)
{
    assert(data != NULL && atomic_load(&running));
    if (atomic_load(&stop_on_feed)) {
        atomic_store(&running, false);
        bytes /= 2U;
    }
    atomic_fetch_add(&queued, bytes);
    return bytes;
}

void wave_player_drain(void)
{
    assert(pthread_mutex_lock(&transport_lock) == 0);
    assert(atomic_load(&queued) != 0);
    atomic_fetch_add(&drains, 1);
    atomic_store(&drain_entered, true);
    assert(pthread_cond_broadcast(&transport_changed) == 0);
    /* Paused output makes no progress until stop cancels the transport. */
    while (atomic_load(&hold_drain) && atomic_load(&running)) {
        assert(pthread_cond_wait(&transport_changed, &transport_lock) == 0);
    }
    if (atomic_load(&running)) {
        atomic_store(&queued, 0);
    }
    assert(pthread_mutex_unlock(&transport_lock) == 0);
}

uint32 pvmp3_decoderMemRequirements(void)
{
    return 64;
}

void pvmp3_InitDecoder(tPVMP3DecoderExternal *decoder, void *memory)
{
    assert(decoder != NULL && memory != NULL);
}

ERROR_CODE pvmp3_framedecoder(tPVMP3DecoderExternal *decoder, void *memory)
{
    assert(memory != NULL && ++decoder_calls <= 8);
    assert(decoder->inputBufferUsedLength == 0);
    if ((size_t)decoder->inputBufferCurrentLength < frame_bytes) {
        decoder->outputFrameSize = 0;
        return NO_ENOUGH_MAIN_DATA_ERROR;
    }
    decoder->inputBufferUsedLength = (int32)frame_bytes;
    decoder->outputFrameSize = 8;
    decoder->samplingRate = 16000;
    decoder->num_channels = 1;
    memset(decoder->pOutputBuffer, 1, 8 * sizeof(int16_t));
    return NO_DECODING_ERROR;
}

#include "../mp3_player.c"

static int read_input(void *context, uint8_t *buffer, uint32_t capacity)
{
    assert(context == &input_bytes);
    size_t bytes = input_bytes - input_offset;
    if (++read_calls == 1 && bytes > first_read_bytes) {
        bytes = first_read_bytes;
    }
    if (bytes > capacity) {
        bytes = capacity;
    }
    memset(buffer, 0, bytes);
    if (input_offset == 0 && bytes >= 2) {
        buffer[0] = 0xff;
        buffer[1] = 0xfb;
    }
    input_offset += bytes;
    return (int)bytes;
}

static void close_input(void *context)
{
    assert(context == &input_bytes);
    ++close_calls;
}

static void run_case(size_t available, size_t first_read, size_t required, bool cancel_drain, bool stop_during_feed)
{
    decoder_calls = read_calls = close_calls = 0;
    input_bytes = available;
    input_offset = 0;
    first_read_bytes = first_read;
    frame_bytes = required;
    atomic_store(&running, false);
    atomic_store(&hold_drain, cancel_drain);
    atomic_store(&drain_entered, false);
    atomic_store(&queued, 0);
    atomic_store(&discarded, 0);
    atomic_store(&drains, 0);
    atomic_store(&running_queries, 0);
    atomic_store(&stop_on_feed, stop_during_feed);
    audio_mp3_task_cfg_t config = {
        .ops = { .read = read_input, .close = close_input },
        .user_data = &input_bytes,
    };
    assert(mp3_player_start(&config) == 0);
    if (cancel_drain) {
        struct timespec deadline;
        assert(clock_gettime(CLOCK_REALTIME, &deadline) == 0);
        deadline.tv_sec += 2;
        assert(pthread_mutex_lock(&transport_lock) == 0);
        while (!atomic_load(&drain_entered)) {
            assert(pthread_cond_timedwait(&transport_changed, &transport_lock, &deadline) == 0);
        }
        assert(pthread_mutex_unlock(&transport_lock) == 0);
        assert(mp3_player_stop() == 0);
    }
    assert(pthread_join(worker, NULL) == 0);
    assert(!mp3_player_is_running() && close_calls == 1 && atomic_load(&allocations) == 0);
    if (available >= required) {
        assert(atomic_load(&drains) == 1);
        assert(cancel_drain || stop_during_feed || atomic_load(&discarded) == 0);
    } else {
        assert(atomic_load(&drains) == 0 && decoder_calls == 2);
    }
    if (stop_during_feed) {
        assert(atomic_load(&running_queries) == 1 && !atomic_load(&running));
        assert(decoder_calls == 1 && read_calls == 1 && atomic_load(&discarded) == 8);
    } else {
        assert(atomic_load(&running_queries) == 0);
    }
}

int main(void)
{
    alarm(5);
    run_case(100, 100, 1536, false, false);   /* Truncated EOF, decoder consumes nothing. */
    run_case(1536, 1200, 1536, false, false); /* Required frame exceeds refill watermark. */
    assert(decoder_calls == 2 && read_calls == 3);
    run_case(8, 8, 8, false, false); /* Natural EOF drains the queued final PCM. */
    run_case(8, 8, 8, true, false);  /* Stop releases an EOF drain while output is paused. */
    run_case(16, 16, 8, false, true); /* A stopped sink short-write stops decoder input consumption. */
    alarm(0);
    puts("MP3 EOF progress, refill, tail drain, paused-stop and stopped-sink short-write passed");
    return 0;
}
