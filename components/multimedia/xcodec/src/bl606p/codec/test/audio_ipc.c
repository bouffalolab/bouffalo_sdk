#include <stdio.h>
#include <stdint.h>
#include <ipc.h>
#include "audio_ipc.h"

#include <msp/kernel.h>

static void ipcrecv_process(ipc_t *ipc, message_t *msg, void *priv)
{
    struct ipc_audio *pdev = (struct ipc_audio *)priv;
    int ret;

    switch (msg->command) {
        case 110: {
        	ret = msp_queue_send(&pdev->queue, (void *)msg->req_data, msg->req_len);
        	if (ret != 0) {
        		//LOGD("err!!! cmd:%d lost_num:%d\r\n", msg->command, msg->req_len);
        	}
        } break;
        default:
            //LOGD("unknow cmd:%d\r\n", msg->command);
            break;
    }
}

int ipc_audio_receive(struct ipc_audio *pdev, void *buf, int buf_size, uint32_t timeout)
{
    size_t len = 0;

    msp_queue_recv(&pdev->queue, timeout, buf, &len);

    return (int)len;
}

int ipc_audio_send(struct ipc_audio *pdev, const void *buf, int size, uint32_t timeout)
{
    message_t ipcmsg = {
        .flag = MESSAGE_SYNC | SHM_CACHE,
        .service_id = 20,
        .command = 104,
    };

    ipcmsg.req_data = (int64_t)buf;
    ipcmsg.req_len = size;

    ipc_message_send(pdev->ipc, &ipcmsg, timeout);

    return size;
}

static void _ipc_audio_task(void *arg)
{
    struct ipc_audio *pdev = (struct ipc_audio *)arg;
    //int len;

    uint8_t *recv_buf = msp_malloc(INPUT_BUF_SIZE);

    if (recv_buf== NULL) {
        return;
    }

    while (1) {
        //memset(recv_buf, 0, INPUT_BUF_SIZE);
        //len =
        ipc_audio_receive(pdev, recv_buf, INPUT_BUF_SIZE, MSP_WAIT_FOREVER);
        //LOGD("ipc receive audio len:%lld %02x...%02x\r\n", len, recv_buf[0], recv_buf[len-1]);
    }
}

int ipc_audio_init(struct ipc_audio *pdev)
{
	pdev->ipc = ipc_get(1);

	msp_queue_create(&pdev->queue, INPUT_BUF_SIZE*2, INPUT_BUF_SIZE, 0);

    ipc_add_service(pdev->ipc, 20, ipcrecv_process, pdev);

    msp_task_new_ext(&pdev->task,
    		"ipc_audio_task",
			_ipc_audio_task,
			pdev,
			2048,
			10);

    return 0;
}


// test cli
void ipc_audio_recv_cmd(char *buf, int len, int argc, char **argv)
{
    static struct ipc_audio ipc_audio;
    ipc_audio_init(&ipc_audio);
}

void ipc_audio_send_cmd(char *buf, int len, int argc, char **argv)
{

}
