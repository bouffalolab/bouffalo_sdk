/**
  ******************************************************************************
  * @file    at_main.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <task.h>
#include "queue.h"

#include "at_main.h"
#include "at_core.h"
#include "at_port.h"
#include "at_base_cmd.h"
#include "at_base_config.h"
#include "at_user_cmd.h"
#include "at_wifi_cmd.h"
#include "at_net_cmd.h"
#include "at_mqtt_cmd.h"
#include "at_http_cmd.h"
#include "at_through.h"
#include "at_ble_cmd.h"
#define ATCMD_TASK_STACK_SIZE (896)
#define ATCMD_TASK_PRIORITY 28
#define AT_CMD_PRINTF printf

#define AT_WORK_QUEUE 1

#if AT_WORK_QUEUE
static QueueHandle_t g_work_queue;
#endif

uint64_t at_current_ms_get()
{
    uint64_t current_ms;
    TimeOut_t xCurrentTime = {0};
    vTaskSetTimeOutState(&xCurrentTime);
    current_ms = ( uint64_t ) ( xCurrentTime.xOverflowCount ) << ( sizeof( TickType_t ) * 8 );
    current_ms += xCurrentTime.xTimeOnEntering;
    current_ms = current_ms * portTICK_PERIOD_MS;
    return current_ms;
}

void at_response_result(int result_code)
{
    int sub_code = 0;

    if (!at) {
        AT_CMD_PRINTF("ERROR: atcmd has not been initialized\r\n");
        return;
    }

    if (result_code > AT_RESULT_CODE_MAX) {
        sub_code = result_code >> 8;
        result_code &= 0x0f;
        at_cmd_syslog(AT_ERROR_NO(sub_code, 0x00));
    }

    if (AT_RESULT_CODE_OK == result_code)
        at->device_ops.write_data((uint8_t *)AT_CMD_MSG_OK, strlen(AT_CMD_MSG_OK));
    else if (AT_RESULT_CODE_ERROR == result_code)
        at->device_ops.write_data((uint8_t *)AT_CMD_MSG_ERROR, strlen(AT_CMD_MSG_ERROR));
    else if (AT_RESULT_CODE_FAIL == result_code)
        at->device_ops.write_data((uint8_t *)AT_CMD_MSG_FAIL, strlen(AT_CMD_MSG_FAIL));
    else if (AT_RESULT_CODE_SEND_OK == result_code)
        at->device_ops.write_data((uint8_t *)AT_CMD_MSG_SEND_OK, strlen(AT_CMD_MSG_SEND_OK));
    else if (AT_RESULT_CODE_SEND_FAIL == result_code)
        at->device_ops.write_data((uint8_t *)AT_CMD_MSG_SEND_FAIL, strlen(AT_CMD_MSG_SEND_FAIL));
}

void at_write(const char *format , ...)
{
    va_list ap;
    char outbuf[256];
    int outstr_len = 0;

    if (!at) {
        AT_CMD_PRINTF("ERROR: atcmd has not been initialized\r\n");
        return;
    }

    memset(outbuf, 0, sizeof(outbuf));
    va_start(ap, format);
    vsnprintf(outbuf, sizeof(outbuf), format, ap);
    va_end(ap);

    outstr_len = strlen(outbuf);
    at->device_ops.write_data((uint8_t *)outbuf, outstr_len);
}

void at_response_string(const char *format , ...)
{
    va_list ap;
    char outbuf[256];
    int outstr_len = 0;

    if (!at) {
        AT_CMD_PRINTF("ERROR: atcmd has not been initialized\r\n");
        return;
    }

    memset(outbuf, 0, sizeof(outbuf));
    va_start(ap, format);
    vsnprintf(outbuf, sizeof(outbuf), format, ap);
    va_end(ap);

    outstr_len = strlen(outbuf);
    if ((strncmp(&outbuf[outstr_len - 2], "\r\n", 2) != 0) && (outstr_len + 2 <= sizeof(outbuf))) {
        outbuf[outstr_len] = '\r';
        outbuf[outstr_len + 1] = '\n';
        outstr_len += 2;
    }

    at->device_ops.write_data((uint8_t *)outbuf, outstr_len);
}

int at_register_function(at_func restore, at_func stop)
{
    if (!at) {
        AT_CMD_PRINTF("ERROR: atcmd has not been initialized\r\n");
        return -1;
    }

    if (at->function_num < AT_CMD_MAX_FUNC) {
        at->function_ops[at->function_num].restore_func = restore;
        at->function_ops[at->function_num].stop_func = stop;
        at->function_num++;
        return 0;
    }
    else {
        AT_CMD_PRINTF("ERROR: register function failed\r\n");
        return -1;
    }
}

int at_set_work_mode(at_work_mode mode)
{
    at->incmd = mode;
    return 0;
}

at_work_mode at_get_work_mode(void)
{
    return at->incmd;
}

#if AT_WORK_QUEUE
int at_workq_send(int id, struct at_workq *q, int timeout)
{

    if (!g_work_queue) {
        return 0;
    }
    q->eventid = id;
    
    return xQueueSend(g_work_queue, q, timeout);
}

int at_workq_dowork(int eventid, int timeout)
{
    int ret;
    struct at_workq work;

    if (!g_work_queue) {
        return -1;
    }
    
    xQueuePeek(g_work_queue, &work, timeout);
    if (work.eventid != eventid) {
        return 0;
    }
    ret = xQueueReceive(g_work_queue, &work, timeout);
    if ((ret == pdTRUE) && work.pfunc) {
        work.pfunc(work.eventid, work.arg);
    }
    return ret;
}

static void at_workq_task(void *pvParameters)
{
    int ret;
    struct at_workq work;

    g_work_queue = xQueueCreate(1, sizeof(struct at_workq));

    while (1) {
        ret = xQueueReceive(g_work_queue, &work, portMAX_DELAY);
        if ((ret == pdTRUE) && work.pfunc) {
            work.pfunc(work.eventid, work.arg);
        }
    }
}
#endif 

static void at_main_task(void *pvParameters)
{
    int ret;
    int len = 0;
    at_work_mode cmd_mode = at->incmd;
    int recv_size = 0;

    at->device_ops.write_data((uint8_t *)AT_CMD_MSG_WEL, strlen(AT_CMD_MSG_WEL));
    while(!at->exit) {
        if (cmd_mode != at->incmd) {
            if (at->incmd == AT_WORK_MODE_CMD)
                ;
            else
                at->device_ops.write_data((uint8_t *)AT_CMD_MSG_WAIT_DATA, strlen(AT_CMD_MSG_WAIT_DATA));
            cmd_mode = at->incmd;
        }

        if (cmd_mode == AT_WORK_MODE_CMD) {
            ret  = at->device_ops.read_data((uint8_t *)(at->inbuf + len), AT_CMD_MAX_LEN - len);
            if (ret > 0) {
                len += ret;
                len = at_cmd_input((uint8_t *)at->inbuf, len);
            }
        }
        else {
            recv_size = at_through_max_length();
            if (recv_size == 0) {
                at->device_ops.write_data((uint8_t *)AT_CMD_MSG_SEND_OK, strlen(AT_CMD_MSG_SEND_OK));
                at_set_work_mode(AT_WORK_MODE_CMD);
                continue;
            }
            else if (recv_size < 0) {
                at->device_ops.write_data((uint8_t *)AT_CMD_MSG_SEND_FAIL, strlen(AT_CMD_MSG_SEND_FAIL));
                at_set_work_mode(AT_WORK_MODE_CMD);
                continue;
            }

            ret = at->device_ops.read_data((uint8_t *)(at->inbuf), recv_size);
            if (ret > 0) {
                ret = at_through_input((uint8_t *)at->inbuf, ret);
                len = 0;

                if (ret == -1) {
                	printf("at_through_input fail, exit throughput mode %d\r\n", ret);
                    at_set_work_mode(AT_WORK_MODE_CMD);
                    if (at_base_config->sysmsg_cfg.bit.quit_throughput_msg)
                        at->device_ops.write_data((uint8_t *)AT_CMD_MSG_QUIT_THROUGHPUT, strlen(AT_CMD_MSG_QUIT_THROUGHPUT));
                }
                else if (ret == -2) {
                	printf("at_through_input fail, exit throughput mode %d\r\n", ret);
                    at->device_ops.write_data((uint8_t *)AT_CMD_MSG_SEND_CANCELLED, strlen(AT_CMD_MSG_SEND_CANCELLED));
                    at_set_work_mode(AT_WORK_MODE_CMD);
                }
            }
        }
    }

    vTaskDelete(NULL);
}

int at_module_init(void)
{
    int ret = -1;
    int bufLen = (AT_THROUGH_MAX_LEN > AT_CMD_MAX_LEN) ? AT_THROUGH_MAX_LEN : AT_CMD_MAX_LEN;
 
    if (at) {
        AT_CMD_PRINTF( "ERROR: atcmd has been initialized\r\n");
        return -1;
    }

    at = (struct at_struct *)pvPortMalloc(sizeof(struct at_struct) + bufLen);
    if (at == NULL) {
        return -1;
    }

    memset((void *)at, 0, sizeof(struct at_struct));
    at->initialized = 0;
    at->echo = 0;
    at->syslog = 0;
    at->store = 1;
    at->exit = 0;
    at->incmd = AT_WORK_MODE_CMD;
    at->device_ops.init_device = at_port_init;
    at->device_ops.deinit_device = at_port_deinit;
    at->device_ops.read_data = at_port_read_data;
    at->device_ops.write_data = at_port_write_data;
    at->inbuf = (char *)((char *)at + sizeof(struct at_struct));

    ret = at->device_ops.init_device();
    if (ret < 0) {
        AT_CMD_PRINTF("ERROR: init at cmd device failed, ret = %d\r\n", ret);
        goto INIT_ERROR;
    }
#ifdef CONFIG_ATMODULE_NETWORK
    /* register network AT command */
    at_net_cmd_regist();
#endif
   
#ifdef CONFIG_ATMODULE_FS
    /* register at fs */
    at_fs_register();
#endif  

    /* register base AT command */
    at_base_cmd_regist();

    at->syslog = at_base_config->sysmsg_cfg.syslog;
    
    /* register user AT command */
    at_user_cmd_regist();
#ifdef CONFIG_ATMODULE_WIFI
    /* register wifi AT command */
    at_wifi_cmd_regist();
#endif
#ifdef CONFIG_ATMODULE_MQTT
    /* register mqtt AT command */
    at_mqtt_cmd_regist();
#endif 
#ifdef CONFIG_ATMODULE_HTTP
    /* register http AT command */
    at_http_cmd_regist();
#endif
#ifdef CONFIG_ATMODULE_BLUETOOTH
    /* register ble AT command */
    at_ble_cmd_regist();
#endif
#if (defined LP_APP) && (defined CONFIG_ATMODULE_LP)
    /* register pwr AT command */
    at_pwr_cmd_regist();
#endif

    ret = xTaskCreate(at_main_task, (char*)"at_main_task", ATCMD_TASK_STACK_SIZE, NULL, ATCMD_TASK_PRIORITY, NULL);
    if (ret != pdPASS) {
        AT_CMD_PRINTF( "ERROR: create at_main_task failed, ret = %d\r\n", ret);
        goto INIT_ERROR;
    }

#if AT_WORK_QUEUE
    ret = xTaskCreate(at_workq_task, (char*)"at_workq", 512, NULL, 15, NULL);
    if (ret != pdPASS) {
        AT_CMD_PRINTF("ERROR: create net_main_task failed, ret = %d\r\n", ret);
        return -1;
    }
#endif
    at->initialized = 1;
    return 0;

INIT_ERROR:
    if (at) {
        vPortFree(at);
        at = NULL;
    }
    return -1;
}

int at_module_deinit(void)
{
    if (!at) {
        AT_CMD_PRINTF("ERROR: atcmd has not been initialized\r\n");
        return -1;
    }

    at->exit = 1;

    /* wait at_main exit*/
    vTaskDelay(1000);

    at->device_ops.deinit_device();
    vPortFree(at);
    at = NULL;
    return 0;
}

int at_module_func(char *cmd, int (*resp_func) (uint8_t *data, int len))
{
    int ret;
    void *func = NULL;

    if (!at) {
        AT_CMD_PRINTF("ERROR: atcmd has not been initialized\r\n");
        return -1;
    }

    func = (void *)at->device_ops.write_data;//store atcmd write function
    at->device_ops.write_data = resp_func;
    ret = at_cmd_input((uint8_t *)cmd, strlen(cmd));
    at->device_ops.write_data = func;//recovery atcmd write function
    return ret;
}

int at_output_redirect_register(int (*f_output_redirect) (void))
{
    at->device_ops.f_output_redirect = f_output_redirect;
}

int at_output_is_redirect()
{
    if (at && at->device_ops.f_output_redirect) {
        return at->device_ops.f_output_redirect();
    }
    return 0;
}

