/**
  ******************************************************************************
  * @file    at_core.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "at_main.h"
#include "at_core.h"
#include "at_port.h"

#define END_CHAR_NUM 2
#define END_CHAR "\r\n"
#define IS_CHAR_CR(ch)				((ch)==0x0D)
#define IS_CHAR_LF(ch)				((ch)==0x0A)
#define IS_CHAR_BACKSLASH(ch)		((ch)==0x5C)
#define IS_CHAR_ESCAPE(ch)			((ch)==0x5C||(ch)==0x2C||(ch)==0x22)

typedef enum {
    AT_CMD_TYPE_TEST,
    AT_CMD_TYPE_QUERY,
    AT_CMD_TYPE_SETUP,
    AT_CMD_TYPE_EXE,
    AT_CMD_TYPE_ERROR
} at_cmd_type;


#define AT_CORE_PRINTF //printf

struct at_struct *at = NULL;
static uint32_t at_error = AT_SUB_OK;

void hex_dump(unsigned char *data, int len)
{
	int i;
	for(i=0; i<len; i++)
	{
		if(i%16 == 0)
			AT_CORE_PRINTF("\r\n%08x  ", i);
		
		if(data[i] < 0x10)
			AT_CORE_PRINTF("0%X ", data[i]);
		else
			AT_CORE_PRINTF("%X ", data[i]);
	}
    AT_CORE_PRINTF("\r\n");
}

uint32_t at_cmd_get_version(void)
{
    return AT_CORE_VERSION;
}

int at_cmd_get_compile_time(char *time, size_t buf_len)
{
    snprintf(time, buf_len, "%s %s", __DATE__, __TIME__);
    return 0;
}

/*int at_arg_is_number(const char *arg)
{
    int i;

    for (i=0; i<strlen(arg); i++) {
        if (arg[i] < '0' || arg[i] > '9')
            return 0;
    }

    return 1;
}*/

static int at_arg_is_string(const char *arg)
{
    int len = strlen(arg);

    if (arg[0] != '\"' || arg[len - 1] != '\"') {
        return 0;
    }

    return 1;
}

int at_arg_is_null(const char *arg)
{
    if (strlen(arg) <= 0)
        return 1;
    else
        return 0;
}

int at_arg_get_number(const char *arg, int *value)
{
    int i;

    if (!arg) {
        return 0;
    }
    for (i=0; i<strlen(arg); i++) {
        if (!((arg[i] >= '0' && arg[i] <= '9') || (i == 0 && arg[i] == '-')))
            return 0;
    }

    *value = atoi(arg);
    return 1;
}

int at_arg_get_string(const char *arg, char *string, int max)
{
    int len;

    if (!at_arg_is_string(arg))
        return 0;

    len = strlen(arg)-2;
    if (len >= max)
        return 0;

    strlcpy(string, arg+1, max);
    string[len] = '\0';
    return 1;
}

static int at_register_command(const at_cmd_struct *cmd)
{
    int i;

    if (!at) {
        return -1;
    }

    if (!cmd->at_name ) {
        return -1;
    }

    if (at->num_commands < AT_CMD_MAX_NUM) {
        /* Check if the command has already been registered.
        * Return 0, if it has been registered.
        */
        for (i = 0; i < at->num_commands; i++) {
            if (at->commands[i] == cmd) {
                return 0;
            }
        }
        at->commands[at->num_commands++] = cmd;
        return 0;
    }

    return -1;
}

static int at_unregister_command(const at_cmd_struct *cmd)
{
    int i;

    if (!cmd->at_name) {
        return -1;
    }

    for (i = 0; i < at->num_commands; i++) {
        if (at->commands[i] == cmd) {
            at->num_commands--;
  
            int remaining_cmds = at->num_commands - i;
            if (remaining_cmds > 0) {
                memmove(&at->commands[i], &at->commands[i + 1], (remaining_cmds * sizeof(at_cmd_struct *)));
            }
            at->commands[at->num_commands] = NULL;
            return 0;
        }
    }

    return -1;
}

int at_cmd_set_error(uint32_t error)
{
    at_error = error;
    return 0;
}

void at_cmd_syslog(uint32_t error)
{
    char outbuf[64];

    if (at->syslog) {
        snprintf(outbuf, sizeof(outbuf), "ERR CODE:0x%08lx\r\n", error);
        at->device_ops.write_data((uint8_t *)outbuf, strlen(outbuf));
    }
}

int at_cmd_register(const at_cmd_struct *cmds, int num_cmds)
{
    int i;
    int err;

    if (!at) {
        return -1;
    }

    for (i = 0; i < num_cmds; i++) {
        if ((err = at_register_command(cmds++)) != 0) {
            return err;
        }
    }

    return 0;
}

int at_cmd_unregister(const at_cmd_struct *cmds, int num_cmds)
{
    int i;
    int err;

    if (!at) {
        return -1;
    }

    for (i = 0; i < num_cmds; i++) {
        if ((err = at_unregister_command(cmds++)) != 0) {
            return err;
        }
    }

    return 0;
}

static const at_cmd_struct *at_cmd_lookup(char *name)
{
    int i = 0;
    int n = 0;

    while (i < AT_CMD_MAX_NUM && n < at->num_commands) {
        if (at->commands[i]->at_name == NULL) {
            i++;
            continue;
        }

        if (!strcasecmp(at->commands[i]->at_name, name)) {
            return at->commands[i];
        }

        i++;
        n++;
    }

    return NULL;
}

static int at_cmd_proc(char *name, at_cmd_type type, int argc, char **argv)
{
    const at_cmd_struct *cmd = NULL;

    cmd = at_cmd_lookup(name);
    if (!cmd) {
        AT_CORE_PRINTF("not found  ' %s'!\r\n", name);
        at_cmd_syslog(AT_CMD_ERROR_CMD_UNSUPPORT);
        return AT_RESULT_CODE_ERROR;
    }

    if (type == AT_CMD_TYPE_TEST) {
        AT_CORE_PRINTF("test Cmd!\r\n");
        if (cmd->at_test_cmd == NULL)
            return AT_RESULT_CODE_ERROR;
        else
            return cmd->at_test_cmd(argc, (const char **)argv);
    }
    else if (type == AT_CMD_TYPE_QUERY) {
         AT_CORE_PRINTF("query Cmd!\r\n");
        if (cmd->at_query_cmd == NULL)
            return AT_RESULT_CODE_ERROR;
        else
            return cmd->at_query_cmd(argc, (const char **)argv);
    }
    else if (type == AT_CMD_TYPE_SETUP) {
        AT_CORE_PRINTF("setup Cmd!\r\n");
        if (cmd->at_setup_cmd == NULL)
            return AT_RESULT_CODE_ERROR;
        else {
            if (argc < cmd->para_num_min|| argc > cmd->para_num_max) {
                at_cmd_set_error(AT_CMD_ERROR_PARA_NUM(cmd->para_num_min, argc));
                AT_CORE_PRINTF("para num error!\r\n");
                return AT_RESULT_CODE_ERROR;
            }
            return cmd->at_setup_cmd(argc, (const char **)argv);
        }
    }
    else if (type == AT_CMD_TYPE_EXE) {
        AT_CORE_PRINTF("exe Cmd!\r\n");
        if (cmd->at_exe_cmd == NULL)
            return AT_RESULT_CODE_ERROR;
        else
            return cmd->at_exe_cmd(argc, (const char **)argv);
    }

    return AT_RESULT_CODE_ERROR;
}

static int at_cmd_gettype(int equal, int quest, int argc, char **argv)
{
    at_cmd_type type = AT_CMD_TYPE_ERROR;

    if (equal && quest) {
        if (argc > 0)
            return AT_RESULT_CODE_ERROR;
        type = AT_CMD_TYPE_TEST;
    }
    else if (!equal && quest) {
        if (argc > 0)
            return AT_RESULT_CODE_ERROR;
        type = AT_CMD_TYPE_QUERY;
    }
    else  if (equal && !quest) {
        if (argc <= 0)
            return AT_RESULT_CODE_ERROR;
        type = AT_CMD_TYPE_SETUP;
    }
    else {
        if (argc > 0)
            return AT_RESULT_CODE_ERROR;
        type = AT_CMD_TYPE_EXE;
    }
 
    return type;
}

static int at_cmd_parse(char *inbuf)
{
    struct
    {
        unsigned inArg : 1;
        unsigned inQuote : 1;
        unsigned done : 1;
        unsigned error : 1;
        unsigned haveEqual : 1;
        unsigned haveQuest : 1;
    } stat;
    int i = 0, len;
    int ret = 0;
    int argc = 0;
    char *argv[AT_CMD_MAX_PARA];
    at_cmd_type type;

    len = strlen(inbuf);
    if (len <= 0)
        return AT_RESULT_CODE_OK;

    memset((void *)&argv, 0, sizeof(argv));
    memset(&stat, 0, sizeof(stat));
    do {
        switch (inbuf[i]) {
            case '\0':
                stat.done = 1;
                break;

            case '=':
                if (!stat.inQuote) {
                    if (stat.haveEqual)  {
                        stat.error = 1;
                        stat.done = 1;
                    }
                    else if (stat.inArg) {
                        stat.inArg = 0;
                        stat.haveEqual = 1;
                        inbuf[i] = '\0';
                    }
                }
                break;

            case '?':
                if (!stat.inQuote) {
                    if (!stat.haveQuest) {
                        stat.haveQuest = 1;
                        inbuf[i] = '\0';
                    }
                    else {
                        stat.error = 1;
                        stat.done = 1;
                    }
                }
                break;

            case ',':
                if (!stat.inQuote) {
                    if (stat.inArg) {
                        stat.inArg = 0;
                        inbuf[i] = '\0';
                    } 
                    else if (!stat.inArg) {
                        argc++;
                        argv[argc - 1] = &inbuf[i];
                        inbuf[i] = '\0';
                    }
                }
                else {
                    stat.error = 1;
                    stat.done = 1;
                }
                break;

            case '"':
                if (!stat.inQuote && !stat.inArg) {
                    stat.inArg = 1;
                    stat.inQuote = 1;
                    argc++;
                    argv[argc - 1] = &inbuf[i];
                    break;
                }
                else {
                    if (inbuf[i + 1] != '\0' && inbuf[i + 1] != ',') {
                        stat.error = 1;
                        stat.done = 1;
                    }
                    else
                        stat.inQuote = 0;
                }
                break;

            case '\\':
                if (i < len-1 && stat.inQuote && stat.inArg) {
                    memcpy(&inbuf[i], &inbuf[i + 1], strlen(&inbuf[i + 1]) + 1);
                }
                break;

            default:
                if (!stat.inArg) {   
                    stat.inArg = 1;
                    argc++;
                    argv[argc - 1] = &inbuf[i];
                }
                break;
        }
    } while (!stat.done && i++ < AT_CMD_MAX_LEN && argc < AT_CMD_MAX_PARA);

    AT_CORE_PRINTF("\r\nargc = %d\r\n", argc);
    AT_CORE_PRINTF("inArg=%d, inQuote=%d, error=%d, Equal=%d, Quest=%d\r\n", stat.inArg, stat.inQuote, stat.error, stat.haveEqual, stat.haveQuest);
    for (i=0; i<argc; i++)
        AT_CORE_PRINTF("argv[%d] = %s\r\n", i, (argv[i]==NULL)?"NULL":argv[i]);

    if (stat.inQuote || stat.error) {
        AT_CORE_PRINTF("at cmd parse error\r\n");
        at_cmd_syslog(AT_CMD_ERROR_PARA_PARSE_FAIL(argc-1));
        return AT_RESULT_CODE_ERROR;
    }

    type = at_cmd_gettype(stat.haveEqual, stat.haveQuest, argc-1, argv+1);
    if (type == AT_CMD_TYPE_ERROR)
        return AT_RESULT_CODE_ERROR;

    ret = at_cmd_proc(argv[0], type, argc-1, argv+1);
    return ret;
}

static void at_cmd_echo(char *cmd, int cmdLen, int mark)
{
    if (cmdLen <= 0)
        return;

    if (at->echo) {
        at->device_ops.write_data((uint8_t *)(cmd), cmdLen);
        if (mark)
            at->device_ops.write_data((uint8_t *)END_CHAR, END_CHAR_NUM);
    }
}

static int at_cmd_is_valid(char *cmd)
{
    int i, headLen;
    char head[16];

    snprintf((char *)head, sizeof(head), AT_CMD_HEAD);
    headLen = strlen(AT_CMD_HEAD);

    if (strlen(cmd) < headLen)
        return 0;

    for (i=0; i<headLen; i++) {
        if (toupper(head[i]) != toupper(cmd[i]))
            return 0;
    }

    return 1;
}

int at_cmd_input(uint8_t *data, int32_t len)
{
    int i, ret;
    int find = 0;
    int readLen = 0;
    int remainLen = 0;
    char *cmd = (char *)data;
    static int echoLen = 0;

    for (i=0; i<len; i++) {
        if (i > 0 && IS_CHAR_CR(data[i-1]) && IS_CHAR_LF(data[i])) {
            data[i-1] = '\0';
            data[i] = '\0';
            readLen = i+1;

            if (!at_cmd_is_valid(cmd)) {
                at_cmd_echo(cmd+echoLen, strlen(cmd)-echoLen, 1);
                echoLen = 0;
                at_cmd_syslog(AT_CMD_ERROR_NOT_FOUND_AT);
                at->device_ops.write_data((uint8_t *)AT_CMD_MSG_ERROR, strlen(AT_CMD_MSG_ERROR));
            }
            else {
                //AT_DEBUG_POINT(0);
                find = 1;
                break;
            }
            cmd = (char *)(&data[i+1]);
        }
    }

    if (find) {
        at_cmd_echo(cmd+echoLen, strlen(cmd)-echoLen, 1);
        echoLen = 0;
        if (readLen < len) {
            at_cmd_syslog(AT_CMD_ERROR_CMD_PROCESSING);
            at->device_ops.write_data((uint8_t *)AT_CMD_MSG_BUSY, strlen(AT_CMD_MSG_BUSY));
        }

        ret = at_cmd_parse(cmd + strlen(AT_CMD_HEAD));
        if (at_error != AT_SUB_OK) {
            at_cmd_syslog(at_error);
            at_error = AT_SUB_OK;
        }
        else if (ret == AT_RESULT_CODE_FAIL) {
            at_cmd_syslog(AT_CMD_ERROR_CMD_EXEC_FAIL(0));
        }
        at_response_result(ret);
        return 0;
    }

    remainLen = len - readLen;
    if (readLen > 0 && remainLen > 0) {
        memcpy(&data[0], &data[readLen], remainLen);
    }

    if (remainLen > 0) {
        at_cmd_echo((char *)&data[0]+echoLen, remainLen-echoLen, 0);
        echoLen = remainLen;
        if (remainLen >= AT_CMD_MAX_LEN)
            return 0;
        else
            return remainLen;
    }
    else {
        return 0;
    }
}

int at_write_data(uint8_t *data, int32_t len)
{
    return at->device_ops.write_data(data, len);
}

void at_write_string(const char *format, va_list args)
{
    char outbuf[128];

    if (!at) {
        return;
    }

    memset(outbuf, 0, sizeof(outbuf));
    vsnprintf(outbuf, sizeof(outbuf), format, args);

    at->device_ops.write_data((uint8_t *)outbuf, strlen(outbuf));
}

