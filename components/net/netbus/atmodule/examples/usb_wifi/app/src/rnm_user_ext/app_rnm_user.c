#include <stdio.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <shell.h>

#define DBG_TAG "APP_RNM"
#include <log.h>

int cmd_rnm_sdio_no_rsp_test(int argc, char **argv)
{
    LOG_I("Starting cmd_rnm_test ...\r\n");
extern void rnms_user_send_extension(uint8_t is_response);
   
    printf("send no need reponse\r\n");
    rnms_user_send_extension(0);

    return 0;
}

int cmd_rnm_sdio_rsp_test(int argc, char **argv)
{
    LOG_I("Starting cmd_rnm_test ...\r\n");
extern void rnms_user_send_extension(uint8_t is_response);
    
    printf("send and need reponse\r\n");
    rnms_user_send_extension(1);
    
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_rnm_sdio_rsp_test, rnm_sdio_rsp, rnm rsp test.);
SHELL_CMD_EXPORT_ALIAS(cmd_rnm_sdio_no_rsp_test, rnm_sdio_no_rsp, rnm no rsp test.);

void app_rnm_sdio_init(void)
{
}

