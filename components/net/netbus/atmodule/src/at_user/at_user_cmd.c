/**
  ******************************************************************************
  * @file    at_user_cmd.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "at_main.h"
#include "at_core.h"

static int at_query_cmd_userram(int argc, const char **argv)
{
    if (argv == NULL) {
        AT_CMD_PRINTF("[AT_USER] Error: argv is NULL\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    // Add further query logic here if needed
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_userram(int argc, const char **argv)
{
    if (argv == NULL) {
        AT_CMD_PRINTF("[AT_USER] Error: argv is NULL\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    // Add further setup logic here if needed
    return AT_RESULT_CODE_OK;
}

static const at_cmd_struct at_user_cmd[] = {
    //{"+USERRAM", NULL, at_query_cmd_userram, at_setup_cmd_userram, NULL, 0, 0},
    {NULL,              NULL, NULL, NULL, 0, 0},
};

bool at_user_cmd_regist(void)
{
    if (at_cmd_register(at_user_cmd, sizeof(at_user_cmd) / sizeof(at_user_cmd[0])) == 0)
        return true;
    else
        return false;
}

