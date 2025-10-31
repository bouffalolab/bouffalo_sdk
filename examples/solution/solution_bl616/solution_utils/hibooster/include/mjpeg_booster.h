#ifndef __MJPEG_BOOSTER_H__
#define __MJPEG_BOOSTER_H__
#include <stdint.h>
#if defined ARCH_RISCV || defined ARCH_ARM
#ifdef CPU_M0
#include <FreeRTOS.h>
#include <queue.h>
#endif
#ifdef CPU_D0
#include <sample_common.h>
#include <FreeRTOS_POSIX/mqueue.h>
#endif
#else
#ifdef linux
#include <mqueue.h>
#endif
#endif
/*This header file is only used for export function for outlander*/

#define USER_MSG_PAYLOAD_SIZE_MAX 128

struct ctrl_msg_queue_item {
    uint32_t seq;
    uint16_t length;
    uint8_t data[USER_MSG_PAYLOAD_SIZE_MAX];
};

#define USED_MAGIC_FLAG     0xa5a55a5a
#define FRAME_BUF_ADDR      0x53800000
#define FRAME_BUF_SIZE      0x800000
#define FRAME_BIT_SIZE      4
#define FRAME_NUM           (1 << FRAME_BIT_SIZE)          //must 2^
#define PER_FRAME_SIZE      (FRAME_BUF_SIZE / FRAME_NUM)
typedef struct ctrl_frame_head {
    uint64_t used_flag;
    uint64_t seq;
    uint32_t length;
    uint32_t crc_val;
    uint8_t data[0];
} frame_head_t;

#define CTRL_MSG_QUEUE_LENGTH    4
#define CTRL_MSG_QUEUE_ITEM_SIZE sizeof(struct ctrl_msg_quue_item)

#ifdef CPU_D0
extern mqd_t ctrl_msg_queue;
#endif

#ifdef CPU_M0
extern QueueHandle_t ctrl_msg_queue;
#endif
int mjpeg_sender_config(uint16_t local_ctl_port, uint16_t send_port);

int mjpeg_sender_start();

int mjpeg_sender_stop();

void mjpeg_sender_ctl_task(void *_params);
void mjpeg_sender_task(void *_params);

//add
typedef int (*set_image_appendix_cb_t)(uint64_t seq, uint8_t *data, int *data_len);
int mjpeg_set_per_image_appendix_callback_register(set_image_appendix_cb_t cb);



void mjpeg_send_frame(void);
void mjpeg_resend_frame(void);

int mjpeg_is_idle(void);
int mjpeg_is_next_frame_ready(void);
void mjpeg_push_frame(uint8_t *image_data1, uint32_t image_len1);
int mjpeg_send_mcuctl_msg(uint8_t *data, uint16_t length, uint32_t *o_seq, void (*mcuctl_msg_callback)(uint32_t seq));
int mjpeg_sendto_opt_retry_control(int status);
int mjpeg_sendto_opt_cli_init(void);
#endif
