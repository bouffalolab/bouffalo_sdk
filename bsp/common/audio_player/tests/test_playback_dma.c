/* Run the production player loop with deterministic DMA/owner scheduling.
 * Exercise steady-state refills; startup priming is outside these checks. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WAVE_PLAYER_ENABLE_RESAMPLE 0
#define WAVE_PLAYER_ENABLE_REC 0
#define WAVE_PLAYER_TX_DONE_NOTIFY_BIT 1
#define DMA_CMD_GET_LLI_SRCADDR 1
#define portMAX_DELAY UINT32_MAX
#define pdMS_TO_TICKS(x) (x)
#define pdTRUE 1
#define WAVE_PLAYER_DIAG_TX_SEM_TAKE() ((void)0)
#define WAVE_PLAYER_DIAG_TX_FILL() ((void)0)
/* A diagnostic reset must not affect the functional DMA sequence. */
typedef struct { int unused; } Ring_Buffer_Type;
typedef struct {
    Ring_Buffer_Type *wave_rb;
    void *sem_wave_rb_space, *sem_tx_dma_done;
    uint32_t src_sample_rate_hz, output_sample_rate_hz, tx_block_bytes, logical_tx_block_bytes;
    uint8_t src_channels, out_channels;
    int16_t *pcm_ping, *pcm_pong;
    void (*pcm_fill)(int16_t *, const int16_t *, uint32_t, uint32_t, uint32_t, uint32_t);
    void (*audio_stop)(void);
} wave_player_cfg_t;
static struct {
    int16_t pcm_ping[640], pcm_pong[640];
    bool stop, io_started;
    void *wave_task, *dma_ch_tx;
    volatile uint32_t tx_dma_completions;
} g_audio;
static volatile uint32_t playback_flush_epoch;
static void (*playback_frame_cb)(const int16_t *,uint32_t,uint32_t,uint8_t,void *);
static void *playback_frame_user_data;
static unsigned fill_count, wakes, unsafe_writes, active, prepared;
static unsigned reference_wait_calls;
static bool reference_wait_continue = true;
static uint32_t block_bytes;
static struct { uint32_t tx_isr_cnt; } g_wave_diag;
static uint32_t diagnostic_max_delta;
typedef struct {unsigned physical, irqs;} completion_t;
static const completion_t *schedule;
static unsigned schedule_count;
static bool know_source;
static unsigned critical_depth;
static unsigned terminal_publications;
static unsigned next_owner;

static void test_enter_critical(void)
{
    assert(critical_depth == 0);
    critical_depth = 1;
}

static void test_exit_critical(void)
{
    assert(critical_depth == 1);
    critical_depth = 0;
    if (g_audio.wave_task == NULL) {
        assert(!g_audio.stop);
        terminal_publications++;
        /* A higher-priority owner can claim and stop immediately on handoff. */
        g_audio.wave_task = &next_owner;
        g_audio.stop = true;
    }
}

#define taskENTER_CRITICAL() test_enter_critical()
#define taskEXIT_CRITICAL()  test_exit_critical()
static void *pvPortMalloc(size_t n) { return malloc(n); }
static void vPortFree(void *p) { free(p); }
static void vTaskDelete(void *p)
{
    assert(p == NULL && critical_depth == 0 && terminal_publications == 1);
    assert(g_audio.wave_task == &next_owner && g_audio.stop);
}
static uint32_t Ring_Buffer_Get_Length(Ring_Buffer_Type *p) { (void)p;return 320; }
static uint32_t Ring_Buffer_Get_Size(Ring_Buffer_Type *p) { (void)p;return 5120; }
static void xSemaphoreGive(void *p) { (void)p; }
static void wave_player_diag_poll_playback_hw_xrun(void) {}
static void wave_player_diag_note_source_underrun(bool r,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f)
{(void)r;(void)a;(void)b;(void)c;(void)d;(void)e;(void)f;}
static void wave_player_diag_note_refill_miss(uint32_t a,bool b,uint32_t c,uint32_t d,uint32_t e)
{if(a>diagnostic_max_delta)diagnostic_max_delta=a;(void)b;(void)c;(void)d;(void)e;}
static uint32_t rb_read_block_aligned(Ring_Buffer_Type *p,uint8_t *dst,uint32_t bytes,uint32_t frame)
{
    (void)p;assert(frame==2);
    ++prepared;
    for(unsigned i=0;i<bytes/2;i++)((int16_t *)dst)[i]=(int16_t)prepared;
    return bytes;
}
static int xTaskNotifyWait(uint32_t a,uint32_t b,void *p,uint32_t timeout)
{
    (void)a;(void)b;(void)p;assert(timeout==portMAX_DELAY);
    assert(wakes<schedule_count);
    active ^= schedule[wakes].physical & 1;
    g_audio.tx_dma_completions += schedule[wakes].irqs;
    g_wave_diag.tx_isr_cnt += schedule[wakes].irqs;
    if(wakes==2)g_wave_diag.tx_isr_cnt=0; /* audio_diag clr while playback is running. */
    ++wakes;
    return pdTRUE;
}
static __attribute__((unused)) uint32_t bflb_dma_feature_control(void *dev,int cmd,int arg)
{
    (void)dev;(void)arg;assert(cmd==DMA_CMD_GET_LLI_SRCADDR);
    if(!know_source)return 0;
    return (uint32_t)(uintptr_t)(active?g_audio.pcm_pong:g_audio.pcm_ping)+block_bytes/2;
}
static void fill_pcm(int16_t *dst,const int16_t *src,uint32_t src_bytes,uint32_t src_ch,uint32_t out_ch,uint32_t tx_bytes)
{
    assert(src_bytes==320 && src_ch==1 && out_ch==2 && tx_bytes==block_bytes);
    for(unsigned i=0;i<src_bytes/2;i++)assert(src[i]==(int16_t)prepared);
    if(fill_count>=2 && dst==(active?g_audio.pcm_pong:g_audio.pcm_ping))unsafe_writes++;
    ++fill_count;
    if(fill_count==schedule_count+2)g_audio.stop=true;
}
static void audio_stop(void) {}
static bool audio_pcm_owned(void)
{
    return false;
}
static __attribute__((unused)) bool audio_pcm_player_wait_reference(void)
{
    assert(fill_count == 0 && wakes == 0);
    ++reference_wait_calls;
    return reference_wait_continue;
}
static void audio_pcm_player_started(void)
{
}
static void audio_pcm_player_failed(void)
{
}
static void audio_pcm_prepare_playback(int16_t *samples, size_t bytes)
{
    (void)samples;
    (void)bytes;
}
static void bflb_l1c_dcache_clean_range(void *samples, uint32_t bytes)
{
    (void)samples;
    (void)bytes;
}
#include "wave_player_loop_under_test.h"
static unsigned run(const completion_t *events,unsigned count,bool source,unsigned slots)
{
    memset(&g_audio,0,sizeof(g_audio));
    fill_count=wakes=unsafe_writes=active=prepared=0;
    reference_wait_calls = 0;
    critical_depth = terminal_publications = 0;
    g_wave_diag.tx_isr_cnt=diagnostic_max_delta=0;
    know_source=source;schedule=events;schedule_count=count;block_bytes=160*slots*2;
    Ring_Buffer_Type rb={0};
    wave_player_cfg_t cfg={
        .wave_rb=&rb,.sem_tx_dma_done=&rb,.src_sample_rate_hz=8000,.output_sample_rate_hz=8000,
        .tx_block_bytes=block_bytes,.logical_tx_block_bytes=640,.src_channels=1,.out_channels=2,
        .pcm_ping=g_audio.pcm_ping,.pcm_pong=g_audio.pcm_pong,.pcm_fill=fill_pcm,.audio_stop=audio_stop,
    };
    g_audio.wave_task = &cfg;
    wave_player_task(&cfg);
    assert(terminal_publications == 1 && critical_depth == 0);
    assert(g_audio.wave_task == &next_owner && g_audio.stop);
    assert(reference_wait_calls == 1);
    if (!reference_wait_continue) {
        assert(wakes == 0 && fill_count == 0);
        return 0;
    }
    assert(wakes==count && fill_count==count+2);
    assert(diagnostic_max_delta <= 4); /* No unsigned underflow on diagnostic reset. */
    return unsafe_writes;
}
int main(void)
{
    const completion_t normal[]={{1,1},{1,1},{1,1},{1,1}};
    const completion_t merged[]={{1,1},{2,2},{1,1},{1,1}};
    const completion_t masked_irq[]={{1,1},{2,1},{1,1},{1,1}};
    for(unsigned slots=2;slots<=4;slots+=2){
        assert(run(normal,4,true,slots)==0);
        assert(run(normal,4,false,slots)==0);
        unsigned merged_bad=run(merged,4,true,slots);
        unsigned counter_bad=run(merged,4,false,slots);
        unsigned masked_bad=run(masked_irq,4,true,slots);
#ifdef EXPECT_ORIGINAL_BUG
        assert(merged_bad && counter_bad && masked_bad);
#else
        assert(!merged_bad && !counter_bad && !masked_bad);
#endif
        printf("slots=%u: unsafe buffer writes coalesced=%u fallback=%u masked-irq=%u\n",slots,merged_bad,counter_bad,masked_bad);
    }
    reference_wait_continue = false;
    assert(run(normal, 4, true, 2) == 0);
}
