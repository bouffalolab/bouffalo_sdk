/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <time.h>
#include <stdint.h>
#include <xutils/xringbuffer.h>
#include <msp/record.h>
#include "msp/kernel.h"
#ifdef CONFIG_SHELL
#include <shell.h>
#else
#include <cli.h>
#endif

#define TAG "PCMREC"

#define PCM_PRIOD_MS 20
#define PCM_RATE     16000
#define PCM_CHN      1
#define PCM_FRAME    16
//#define PCM_PRIOD_SIZE ((PCM_RATE / 1000) * PCM_PRIOD_MS * (16 / 8 * PCM_CHN))
//#define RINBUF_SIZE  (PCM_FRAME * PCM_PRIOD_SIZE)

#define MAX_MIC_REC_COUNT PCM_CHN
#define CHUNK_SIZE (5120) // 5K

typedef struct _rec_mgr {
    x_ringbuf_t ringbuffer;
    char *        rbuffer;
    msp_event_t   wait_evt;
    rec_hdl_t     hdl;
    /* debug info */
    uint32_t  total_rec;
    uint32_t  rb_min_left;
    long long last_print_ms;
    uint32_t  total_drop;
} rec_mgr_t;

static rec_mgr_t *g_rec_mgrs[MAX_MIC_REC_COUNT];

static int      is_websocket_record_start = 0;
static int      is_memrecord_start        = 0;
static int      is_record_start           = 0;

void mic_rec_copy_data(int index, uint8_t *data, uint32_t size)
{
    if (index < MAX_MIC_REC_COUNT) {
        rec_mgr_t *rec_mgr = g_rec_mgrs[index];
        if (rec_mgr && rec_mgr->hdl && size > 0) {

            int write_space = xringbuffer_available_write_space(&rec_mgr->ringbuffer);
            if (write_space > size) {
                xringbuffer_write(&rec_mgr->ringbuffer, data, size);

                /* Debug info */
                rec_mgr->total_rec += size;
                if (write_space < rec_mgr->rb_min_left) {
                    rec_mgr->rb_min_left = write_space;
                } else if (rec_mgr->rb_min_left == 0) {
                    rec_mgr->rb_min_left = write_space;
                }

            } else {
                LOGW(TAG, "buffer full, drop %d", size);
                rec_mgr->total_drop += size;
            }

            // if (ringbuffer_available_read_space(&rec_mgr->ringbuffer) >= CHUNK_SIZE) {
            msp_event_set(&rec_mgr->wait_evt, 0x01, MSP_EVENT_OR);
            // }

            /* Debug info print */
            if (msp_now_ms() - rec_mgr->last_print_ms > 5000) {
                LOGW(TAG, "total rec=%u rb min left=%u drop=%u\r\n", rec_mgr->total_rec,
                     rec_mgr->rb_min_left, rec_mgr->total_drop);
                rec_mgr->last_print_ms = msp_now_ms();
            }
        }
    }
}

static int data_wait_ready(void *arg, void *data, size_t len)
{
    unsigned   flags;
    rec_mgr_t *rec_mgr = (rec_mgr_t *)arg;

    if (!is_websocket_record_start) {
        return 0;
    }
    if (len <= 0) {
        msp_event_get(&rec_mgr->wait_evt, 0x01, MSP_EVENT_OR_CLEAR, &flags, MSP_WAIT_FOREVER);
    }
    return 0;
}

static int mic_data_ready(void *arg, void *data, size_t len)
{
    if (!is_websocket_record_start) {
        return 0;
    }
    if (len <= 0) {
        return 0;
    }

    mic_rec_copy_data(0, data, len);
    return 0;
}

void mic_start()
{
    char buf1[64];
    static uint8_t mic_started = 0;

    if (mic_started) {
        return;
    }
    mic_started = 1;
    
    snprintf(buf1, sizeof(buf1), "mic://format=%u&sample=%u&chan=%u&frame_ms=%u",
                PCM_FRAME, PCM_RATE, PCM_CHN, PCM_PRIOD_MS);

    int frame_size = PCM_RATE/1000*(PCM_FRAME/8)*PCM_CHN*PCM_PRIOD_MS;

    rec_hdl_t hdl_mic = record_register(buf1, "null://");

    record_set_data_ready_cb(hdl_mic, mic_data_ready, (void *)NULL);
    record_set_chunk_size(hdl_mic, frame_size);
    record_start(hdl_mic);
}

static void mic_rec_start(const char *url, const char *save_name, int rb_size, int rec_cnt)
{
    for (int i = 0; i < rec_cnt; i++) {
        rec_mgr_t *rec_mgr = msp_zalloc_check(sizeof(rec_mgr_t));

        rec_mgr->rbuffer = msp_malloc_check(rb_size + 1);
        xringbuffer_create(&rec_mgr->ringbuffer, (uint8_t *)rec_mgr->rbuffer, rb_size + 1);
        msp_event_new(&rec_mgr->wait_evt, 0);
        char buf[64];
        snprintf(buf, sizeof(buf), "rbuf://handle=%u", (size_t)(uint64_t)(&rec_mgr->ringbuffer));
        /*
         * mic://format=16&sample=16000&chan=1&frame_ms=20
         */
        
        char   buf2[128];
        time_t tt = time(NULL);
        if (i == 0) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s", url, (int)tt, "mic", save_name);
        } else if (i == 1) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s", url, (int)tt, "ref", save_name);
        } else if (i == 2) {
            snprintf(buf2, sizeof(buf2), "%s/%d_%s_%s", url, (int)tt, "proc", save_name);
        }

        rec_hdl_t hdl_rbuf_ws = record_register(buf, buf2);
        record_set_data_ready_cb(hdl_rbuf_ws, data_wait_ready, (void *)rec_mgr);
        int frame_size = PCM_RATE/1000*(PCM_FRAME/8)*PCM_CHN*PCM_PRIOD_MS;
        record_set_chunk_size(hdl_rbuf_ws, frame_size*5); //Write up to 5 frames of data to ws 

        record_start(hdl_rbuf_ws);

        rec_mgr->hdl  = hdl_rbuf_ws;
        g_rec_mgrs[i] = rec_mgr;
        LOGD(TAG, "start mic rec [%d]\n", i);
    }
}

void mic_rec_stop(void)
{
    for (int i = 0; i < MAX_MIC_REC_COUNT; i++) {
        rec_mgr_t *rec_mgr = g_rec_mgrs[i];
        if (rec_mgr == NULL) {
            continue;
        }
        record_pre_stop(rec_mgr->hdl);
        msp_event_set(&rec_mgr->wait_evt, 0x01, MSP_EVENT_OR);
        record_stop(rec_mgr->hdl);
        msp_event_free(&rec_mgr->wait_evt);
        msp_free(rec_mgr->rbuffer);
        xringbuffer_destroy(&rec_mgr->ringbuffer);
        record_unregister(rec_mgr->hdl);
        rec_mgr->hdl = NULL;
        msp_free(rec_mgr);
        g_rec_mgrs[i] = NULL;
        LOGD(TAG, "mic rec stop over.");
    }
}

#ifdef CONFIG_SHELL
static void task_cmd(int argc, char **argv)
#else
static void task_cmd(char *buf, int len, int argc, char **argv)
#endif
{
    mic_start(); 

    if (argc >= 4 && strcmp(argv[1], "start") == 0) {
        if (is_record_start == 0) {

            // defaut: rec_cnt 1, rb_size 100K
            int rec_cnt = 1;
            int rb_size = 100 * 1024;

            if (argc >= 5) {
                rec_cnt = atoi(argv[4]);
                // rec_cnt: 1->cpu0, 2->cpu0+cpu1, 3->cpu0+cpu1+cpu2
                if (argc == 6) {
                    rb_size = atoi(argv[5]);
                }
            }

            is_record_start           = 1;
            is_websocket_record_start = 1;
            mic_rec_start(argv[2], argv[3], rb_size, rec_cnt);
            printf("record started\r\n");
        } else {
            printf("already started\r\n");
        }
    } else if (argc == 2 && strcmp(argv[1], "stop") == 0) {
        if (is_record_start == 1) {

            /* 停止数据 */
            is_record_start           = 0;
            is_websocket_record_start = 0;
            mic_rec_stop();

        }
        printf("record stop\r\n");
    } else if (argc == 2 && strcmp(argv[1], "memrecord") == 0) {
        is_record_start      = 1;
        is_memrecord_start   = 1;
        // mem_dump_mem_pcm_len = 0;
    } else {
        printf(
            "record start ws://192.168.50.216:8090 test.pcm [1(reccount)] [200000(rbsize)]\r\ntgsp stop\r\n \
                record memrecord\r\n");
    }
}

#ifdef CONFIG_SHELL
SHELL_CMD_EXPORT_ALIAS(task_cmd, record, tgspeech_debug);
#else
static const struct cli_command  rec_cli_cmd[] = {
    {"record", "record", task_cmd},
};

void tgsp_record_cli_init(void)
{
    //vendor_pcm_rb_hook(0, debug_pcm_mem_record);
    aos_cli_register_commands(&rec_cli_cmd[0], sizeof(rec_cli_cmd) / sizeof(rec_cli_cmd[0]));
}
#endif
