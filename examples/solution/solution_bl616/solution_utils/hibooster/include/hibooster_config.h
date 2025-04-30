#ifndef __HIBOOSTER_CONFIG_H__
#define __HIBOOSTER_CONFIG_H__
#ifdef CPU_D0
#include "FreeRTOS_POSIX.h"
#include <fcntl.h>
#include <unistd.h>
#include <FreeRTOS_POSIX/pthread.h>
#include <FreeRTOS_POSIX/mqueue.h>
#include <sample_common.h>

#include <semphr.h>
#include <mm_isp.h>
#include <mm_debug.h>
#include <ae_algo.h>
#include <awb2_algo.h>
#include <timers.h>
#endif
#ifdef CPU_M0
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#endif
#include <lwip/sockets.h>
#include <mjpeg_sw_buffer.h>
#include <sender.h>

#ifdef USE_STATIC_IMAGE_BUFFER
#ifdef CPU_M0
#define IMG_BUF_SECTION     __attribute__((section(".img_buf")))
#else
#define IMG_BUF_SECTION
#endif
#endif

#ifdef USE_STATIC_IMAGE_BUFFER
static uint8_t fifo_buffer[MJPEG_SW_FIFO_BUFFER_SIZE] IMG_BUF_SECTION;
static uint8_t frag_ack[FRAGS_ACK_BITMAP_BYTES_MAX] IMG_BUF_SECTION;
static uint8_t frag_buf[IMAGE_FRAG_SIZE_MAX] IMG_BUF_SECTION;
#endif

void bl_dcache_invalid_range(void *ptr, size_t len);
void bl_dcache_clean_range(void *ptr, size_t len);
long get_time_ms(void);
#endif
