/* Timestamp buffering and PLC on the production implementation, including
 * deterministic burst delivery, timestamp wrap and reconnect boundaries. */
#include "config.h"
#include "webrtc_peer.h"
#include "webrtc_peer_audio.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned allocations;
static int fail_alloc;
static void *allocate(size_t n, size_t size) { allocations++; return fail_alloc ? NULL : calloc(n, size); }
#define calloc allocate
#include "../../../webrtc_peer/src/webrtc_peer_audio.c"
#undef calloc
static int16_t input[320], output[320];
static webrtc_peer_audio_t *open_audio(unsigned rate, unsigned minimum, unsigned maximum)
{
    webrtc_peer_audio_t *s = NULL;
    webrtc_peer_audio_config_t c = {.sample_rate=rate, .min_delay_ms=minimum, .max_delay_ms=maximum};
    assert(webrtc_peer_audio_open(&c, &s) == 0 && s);
    return s;
}
static void push(webrtc_peer_audio_t *s, unsigned source, unsigned seq, uint32_t ts, int value)
{
    unsigned n=s->config.sample_rate/50;
    for (unsigned i=0;i<n;i++)input[i]=(int16_t)value;
    assert(!webrtc_peer_audio_push(s, source, (uint16_t)seq, ts, input, n));
}
static void expect(webrtc_peer_audio_t *s, int value)
{
    unsigned n=s->config.sample_rate/50;
    assert(webrtc_peer_audio_pull(s,output,n)==(int)n);
    for(unsigned i=0;i<n;i++) {if(output[i]!=value)fprintf(stderr,"unexpected sample %u: %d != %d at timestamp %u\n",i,output[i],value,s->timestamp);assert(output[i]==value);}
}
static void order_wrap(unsigned rate)
{
    unsigned n=rate/50, before=allocations;
    webrtc_peer_audio_t *s=open_audio(rate,60,240);
    uint32_t base=UINT32_MAX-n;
    push(s,3,65535,base+n,200); /* second packet arrives first */
    assert(!webrtc_peer_audio_pull(s,output,n));
    push(s,3,65534,base,100);
    push(s,3,0,base+2*n,300);
    push(s,3,0,base+2*n,999); /* duplicate cannot replace speech */
    expect(s,100);expect(s,200);expect(s,300);
    for(unsigned i=0;i<5000;i++) {
        push(s,3,(uint16_t)(i+1),base+(i+3)*n,(int)(i%1000));
        expect(s,(int)(i%1000));
    }
    assert(allocations==before+1); /* no per-packet allocations */
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
    webrtc_peer_audio_stats_t stats;
    assert(!webrtc_peer_audio_get_stats(s,&stats,sizeof(stats)));
    assert(stats.real_samples==(5003U*n) && stats.concealed_samples==0);
    assert(stats.duplicate_samples==n && stats.allocated_bytes<14000);
#endif
    webrtc_peer_audio_close(s);
}
static void loss_recovery(unsigned rate)
{
    unsigned n=rate/50;
    webrtc_peer_audio_t *s=open_audio(rate,60,240);
    /* Periodic reference signal with exactly 200 Hz pitch. */
    for(unsigned i=0;i<n;i++) input[i]=(int16_t)((int)(i%(rate/200))*100-2000);
    for(unsigned i=0;i<3;i++)assert(!webrtc_peer_audio_push(s,3,i,i*n,input,n));
    for(unsigned i=0;i<3;i++)assert(webrtc_peer_audio_pull(s,output,n)==(int)n);
    assert(webrtc_peer_audio_pull(s,output,n)==(int)n);
    assert(output[0]==input[0]);
    int64_t energy1=0,energy2=0;
    for(unsigned i=0;i<n;i++)energy1+=(int32_t)output[i]*output[i];
    assert(webrtc_peer_audio_pull(s,output,n)==(int)n);
    for(unsigned i=0;i<n;i++)energy2+=(int32_t)output[i]*output[i];
    assert(energy1>energy2 && energy2>0);
    for(unsigned j=0;j<8;j++)assert(webrtc_peer_audio_pull(s,output,n)==(int)n);
    for(unsigned i=0;i<n;i++)assert(output[i]==0); /* bounded fade, no buzz loop */
    for(unsigned j=0;j<4;j++)push(s,3,6+j,(6+j)*n,1200);
    assert(webrtc_peer_audio_pull(s,output,n)==(int)n);
    assert(output[0]==0 && output[n-1]==1200); /* recovery blend */
    expect(s,1200);
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
    assert(s->stats.concealed_samples==10*n && s->stats.rebuffer_samples==7*n);
    assert(s->target_ms==80);
#endif
    webrtc_peer_audio_reset(s);
    assert(!webrtc_peer_audio_pull(s,output,n));
    for(unsigned i=0;i<3;i++)push(s,4,i,800000+i*n,-300);
    expect(s,-300); /* previous source/history never leaks into next call */
    webrtc_peer_audio_close(s);
}
static void pull_contract(unsigned rate)
{
    unsigned n=rate/50;
    webrtc_peer_audio_t *s=open_audio(rate,60,240);
    for(unsigned i=0;i<n;i++)output[i]=1234;
    assert(webrtc_peer_audio_pull(s,output,n)==0);
    push(s,3,0,0,1200);
    assert(webrtc_peer_audio_pull(s,output,n)==0);
    for(unsigned i=0;i<n;i++)assert(output[i]==1234); /* zero writes no PCM */
    push(s,3,1,n,1200);push(s,3,2,2*n,1200);
    expect(s,1200);expect(s,1200);expect(s,1200);
    /* Once started, even an empty queue continues supplying the hardware.
     * The first 60 ms fades; re-priming then keeps returning silent PCM. */
    for(unsigned j=0;j<6;j++)assert(webrtc_peer_audio_pull(s,output,n)==(int)n);
    for(unsigned i=0;i<n;i++)assert(output[i]==0);
    push(s,3,6,6*n,1200);
    assert(webrtc_peer_audio_pull(s,output,n)==(int)n); /* not zero on re-prime */
    for(unsigned i=0;i<n;i++)assert(output[i]==0);
    for(unsigned j=7;j<10;j++)push(s,3,j,j*n,1200);
    assert(webrtc_peer_audio_pull(s,output,n)==(int)n);
    assert(output[0]==0 && output[n-1]==1200);
    expect(s,1200);
    /* A same-source forward discontinuity can re-prime before PLC has faded.
     * Re-priming therefore promises PLC, not unconditional silence. */
    push(s,3,10,1000*n,1200);
    assert(webrtc_peer_audio_pull(s,output,n)==(int)n);
    assert(output[0]==1200 && output[n-1]>0 && output[n-1]<1200);
    webrtc_peer_audio_reset(s);
    for(unsigned i=0;i<n;i++)output[i]=1234;
    assert(webrtc_peer_audio_pull(s,output,n)==0);
    for(unsigned i=0;i<n;i++)assert(output[i]==1234);
    webrtc_peer_audio_close(s);
}
static void late_restart(void)
{
    webrtc_peer_audio_t *s=open_audio(8000,40,240);
    for(unsigned i=0;i<5;i++)push(s,3,100+i,10000+i*160,123);
    for(unsigned i=0;i<3;i++)expect(s,123);
    push(s,3,101,10160,999); /* ordinary late packet */
    expect(s,123);
    for(unsigned i=0;i<320;i++)input[i]=888;
    assert(!webrtc_peer_audio_push(s,3,105,10640,input,320)); /* overlap keeps original prefix */
    expect(s,123);
    expect(s,888); /* late prefix clipped, tail used */
    push(s,3,300,0,456); /* new timestamp timeline needs confirmation */
    assert(s->timestamp==10960);
    push(s,3,301,160,456);
    assert(s->timestamp==160);
    push(s,3,302,320,456);
    assert(webrtc_peer_audio_pull(s,output,160)==160);
    assert(output[159]==456);
    push(s,9,0,80000,-1000);push(s,9,1,80160,-1000);
    expect(s,-1000);
    push(s,9,2,90000,2); /* forward discontinuity bounded re-prime */
    assert(s->timestamp==90000 && !s->primed);
    webrtc_peer_audio_close(s);
}
static void bounded_jitter(unsigned rate)
{
    unsigned n=rate/50, next=0;
    webrtc_peer_audio_t *s=open_audio(rate,160,240);
    /* Hold batches for 120 ms, then release in reverse order. Pull follows
     * the hardware clock, never the number of packets delivered in a tick. */
    unsigned output_frames=0;
    for(unsigned tick=0;tick<1500;tick++) {
        if(tick%6==5) {
            for(int j=5;j>=0;j--)push(s,7,next+(unsigned)j,(next+(unsigned)j)*n,100);
            next+=6;
        }
        int count=webrtc_peer_audio_pull(s,output,n);
        if(count) { output_frames++;for(unsigned i=0;i<n;i++)assert(output[i]==100); }
    }
    assert(output_frames>1400 && span(s)<=s->capacity);
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
    assert(s->stats.concealed_samples==0 && s->stats.resyncs==0);
#endif
    webrtc_peer_audio_close(s);
}
static void overflow_preserves_speech(void)
{
    webrtc_peer_audio_t *s=open_audio(8000,40,80);
    for(unsigned i=0;i<5;i++)push(s,3,i,160*i,(int)(100+i));
    /* Only frame 0 is evicted. Check the first frame after the recovery blend. */
    assert(webrtc_peer_audio_pull(s,output,160)==160);
    for(unsigned i=40;i<160;i++)assert(output[i]==101);
    expect(s,102);expect(s,103);expect(s,104);
#if CONFIG_LIBPEER_MEDIA_DIAGNOSTICS
    assert(s->stats.window_discard_samples==160 && s->stats.resyncs==0);
    assert(s->stats.concealed_samples==0 && s->stats.real_samples==640);
#endif
    webrtc_peer_audio_close(s);
}
int main(void)
{
    webrtc_peer_audio_t *s=(void*)1;
    webrtc_peer_audio_config_t c={8000,160,240};
    fail_alloc=1;assert(webrtc_peer_audio_open(&c,&s)==WEBRTC_PEER_ERR_NO_MEMORY && !s);fail_alloc=0;
    c.sample_rate=48000;assert(webrtc_peer_audio_open(&c,&s)==WEBRTC_PEER_ERR_INVALID_ARG && !s);
    c=(webrtc_peer_audio_config_t){8000,170,240};assert(webrtc_peer_audio_open(&c,&s)<0);
    assert(webrtc_peer_audio_push(NULL,0,0,0,input,160)<0);
    assert(webrtc_peer_audio_pull(NULL,output,160)<0);
    webrtc_peer_audio_reset(NULL);webrtc_peer_audio_close(NULL);
    for(unsigned rate=8000;rate<=16000;rate*=2) {order_wrap(rate);loss_recovery(rate);pull_contract(rate);bounded_jitter(rate);}
    late_restart();overflow_preserves_speech();
    puts("Audio playout: 8/16 kHz reorder/wrap, burst jitter, late/restart, PLC fade/recovery and allocation bounds passed");
}
