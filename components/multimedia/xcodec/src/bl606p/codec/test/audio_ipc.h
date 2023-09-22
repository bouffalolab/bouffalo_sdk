/*
 * audio_ipc.h
 *
 */

#ifndef __AUDIO_IPC_H__
#define __AUDIO_IPC_H__

#include "stdio.h"
#include <ipc.h>

#define INPUT_BUF_SIZE  2048

struct ipc_audio {
	ipc_t *ipc;
	uint8_t read_buf[INPUT_BUF_SIZE];
	msp_queue_t queue;
	msp_task_t task;
};

int ipc_audio_receive(struct ipc_audio *pdev, void *buf, int buf_size, uint32_t timeout);

int ipc_audio_send(struct ipc_audio *pdev, const void *buf, int size, uint32_t timeout);

int ipc_audio_init(struct ipc_audio *pdev);

#endif /* __AUDIO_IPC_H__ */
