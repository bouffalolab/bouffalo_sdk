#include <sdiowifi_securec_wrap.h>
#include <rnm_server.h>
#include <stdio.h>
#include <string.h>
#include <sdio_port.h>
#include "sdiowifi_mgmr_type.h"

void rnms_handle_user_ext(rnms_t *rnm, rnm_msg_t *cmd)
{
    uint8_t msg_buf[256];
    rnm_user_ext_msg_t *rsp = (rnm_user_ext_msg_t *)msg_buf;
    rnm_user_ext_msg_t *msg = cmd->data;
    const char rsp_str[] = "response from device";

    // TODO Add your handler here
    HR_LOGI("Recv user ext\r\n");
    if (!msg) {
        return;
    }
    HR_LOGI("msg->payload:%s\r\n", msg->payload);

    MEMSET_SAFE(rsp,  sizeof(*rsp),  0,  sizeof(*rsp));
    // field cmd, session_id should be set
    // flag should be RNM_MSG_FLAG_ACK
    rnms_msg_fill_common(rnm, rsp, cmd);
    MEMCPY_SAFE(rsp->payload,  sizeof(rsp_str),  rsp_str,  sizeof(rsp_str));

    rnms_msg_output(rnm, rsp, sizeof(*rsp) + sizeof(rsp_str));
}

/* process the data received from host */
void rnms_handle_user_extension(rnms_t *rnm, rnm_msg_t *cmd, uint8_t is_response)
{
    rnm_user_ext_msg_t *msg = cmd->data;

    // TODO Add your handler here
    if (!msg) {
        return;
    }
    
    HR_LOGI("Recv message from host:%s\r\n", msg->payload);
    if (msg->hdr.flags & RNM_MSG_FLAG_ACK) {
        HR_LOGI("this msg is ack\r\n");
        return;
    }
    if (is_response) {
        HR_LOGI("device will response to host\r\n");
        uint8_t msg_buf[256];
        rnm_user_ext_msg_t *rsp = (rnm_user_ext_msg_t *)msg_buf;
        const char rsp_str[] = "response from device";

        MEMSET_SAFE(rsp,  sizeof(*rsp),  0,  sizeof(*rsp));
        // field cmd, session_id should be set
        // flag should be RNM_MSG_FLAG_ACK
        rnms_msg_fill_common(rnm, rsp, cmd);
        MEMCPY_SAFE(rsp->payload,  sizeof(rsp_str),  rsp_str,  sizeof(rsp_str));

        rnms_msg_output(rnm, rsp, sizeof(*rsp) + sizeof(rsp_str));
    }
}

/* send user data to host */
extern struct sdiowifi_mgmr g_sdiowifi_mgmr;
void rnms_user_send_extension(uint8_t is_response)
{
    rnms_t *rnm = g_sdiowifi_mgmr.rnm;
    
    uint8_t msg_buf[256];
    rnm_user_ext_msg_t *rsp = (rnm_user_ext_msg_t *)msg_buf;
    const char rsp_str[] = "send from device";

    MEMSET_SAFE(rsp,  sizeof(*rsp),  0,  sizeof(*rsp));
    // field cmd, session_id should be set
    // flag should be RNM_MSG_FLAG_ACK
    //rnms_msg_fill_common(rnm, rsp, cmd);
    MEMCPY_SAFE(rsp->payload,  sizeof(rsp_str),  rsp_str,  sizeof(rsp_str));
    if (is_response) {
        rsp->hdr.cmd = BF1B_CMD_USER_EXT_RSP;
    } else {
        rsp->hdr.cmd = BF1B_CMD_USER_EXT_NO_RSP;
    }
    rsp->hdr.flags = RNM_MSG_FLAG_ASYNC;
    rsp->hdr.msg_id = ++rnm->last_msg_id;

    rnms_msg_output(rnm, rsp, sizeof(*rsp) + sizeof(rsp_str));
}

