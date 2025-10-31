/**
  ******************************************************************************
  * @file    at_core.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_CORE_H
#define AT_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CORE_VERSION 0x01000001

#define AT_CMD_HEAD "AT"

#define AT_CMD_MSG_WEL              "\r\nready\r\n"
#define AT_CMD_MSG_BUSY             "\r\nbusy p...\r\n"
#define AT_CMD_MSG_OK               "\r\nOK\r\n"
#define AT_CMD_MSG_ERROR            "\r\nERROR\r\n"
#define AT_CMD_MSG_FAIL             "\r\nERROR\r\n"
#define AT_CMD_MSG_SEND_OK          "\r\nSEND OK\r\n"
#define AT_CMD_MSG_SEND_FAIL        "\r\nSEND FAIL\r\n"
#define AT_CMD_MSG_SEND_CANCELLED   "\r\nSEND CANCELLED\r\n"
#define AT_CMD_MSG_WAIT_DATA        "\r\n>"
#define AT_CMD_MSG_QUIT_THROUGHPUT  "\r\n+QUITT\r\n"

/**
 * @brief module number,Now just AT module
 *
 */
typedef enum {
    AT_MODULE_NUM = 0x01   /*!< AT module */
} at_module;

/**
 * @brief subcategory number
 *
 */
typedef enum {
    AT_SUB_OK                       = 0x00,              /*!< OK */
    AT_SUB_COMMON_ERROR             = 0x01,              /*!< reserved */
    AT_SUB_NO_TERMINATOR            = 0x02,              /*!< terminator character not found ("\r\n" expected) */
    AT_SUB_NO_AT                    = 0x03,              /*!< Starting "AT" not found (or at, At or aT entered) */
    AT_SUB_PARA_LENGTH_MISMATCH     = 0x04,              /*!< parameter length mismatch */
    AT_SUB_PARA_TYPE_MISMATCH       = 0x05,              /*!< parameter type mismatch */
    AT_SUB_PARA_NUM_MISMATCH        = 0x06,              /*!< parameter number mismatch */
    AT_SUB_PARA_VALUE_INVALID       = 0x07,              /*!< the parameter value is out of range */
    AT_SUB_PARA_PARSE_FAIL          = 0x08,              /*!< parse parameter fail */
    AT_SUB_UNSUPPORT_CMD            = 0x09,              /*!< the command is not supported */
    AT_SUB_CMD_EXEC_FAIL            = 0x0A,              /*!< the command execution failed */
    AT_SUB_CMD_PROCESSING           = 0x0B,              /*!< processing of previous command is in progress */
    AT_SUB_CMD_OP_ERROR             = 0x0C,              /*!< the command operation type is error */
    AT_SUB_NO_MEMORY                = 0x0D,              /*!< Memory allocation error */
    AT_SUB_NO_RESOURCE              = 0x0E,              /*!< Resource allocation error */
    AT_SUB_TIMEOUT                  = 0x0F,              /*!< Operation timed out */
    AT_SUB_NOT_ALLOWED              = 0x10,              /*!< Operation not allowed */
    AT_SUB_NOT_INIT                 = 0x11,              /*!< Operation not initialized */
    AT_SUB_IO_ERROR                 = 0x12,              /*!< Device IO error */
    AT_SUB_OP_ADDR_ERROR            = 0x13,              /*!< Operation address error */
    AT_SUB_HANDLE_INVALID           = 0x14,              /*!< Invalid handle for operation */
} at_error_code;

#define AT_ERROR_NO(subcategory,extension)  \
        ((AT_MODULE_NUM << 24) | ((subcategory) << 16) | (extension))

#define AT_CMD_ERROR_OK                           AT_ERROR_NO(AT_SUB_OK,0x00)                                       /*!< No Error */
#define AT_CMD_ERROR_NON_FINISH                   AT_ERROR_NO(AT_SUB_NO_TERMINATOR,0x00)                            /*!< terminator character not found ("\r\n" expected) */
#define AT_CMD_ERROR_NOT_FOUND_AT                 AT_ERROR_NO(AT_SUB_NO_AT,0x00)                                    /*!< Starting "AT" not found (or at, At or aT entered) */
#define AT_CMD_ERROR_PARA_LENGTH(which_para)      AT_ERROR_NO(AT_SUB_PARA_LENGTH_MISMATCH,which_para)               /*!< parameter length mismatch */
#define AT_CMD_ERROR_PARA_TYPE(which_para)        AT_ERROR_NO(AT_SUB_PARA_TYPE_MISMATCH,which_para)                 /*!< parameter type mismatch */
#define AT_CMD_ERROR_PARA_NUM(need,given)         AT_ERROR_NO(AT_SUB_PARA_NUM_MISMATCH,(((need) << 8) | (given)))   /*!< parameter number mismatch */
#define AT_CMD_ERROR_PARA_INVALID(which_para)     AT_ERROR_NO(AT_SUB_PARA_VALUE_INVALID,which_para)                 /*!< the parameter is invalid */
#define AT_CMD_ERROR_PARA_PARSE_FAIL(which_para)  AT_ERROR_NO(AT_SUB_PARA_PARSE_FAIL,which_para)                    /*!< parse parameter fail */
#define AT_CMD_ERROR_CMD_UNSUPPORT                AT_ERROR_NO(AT_SUB_UNSUPPORT_CMD,0x00)                            /*!< the command is not supported */
#define AT_CMD_ERROR_CMD_EXEC_FAIL(result)        AT_ERROR_NO(AT_SUB_CMD_EXEC_FAIL,result)                          /*!< the command execution failed */
#define AT_CMD_ERROR_CMD_PROCESSING               AT_ERROR_NO(AT_SUB_CMD_PROCESSING,0x00)                           /*!< processing of previous command is in progress */
#define AT_CMD_ERROR_CMD_OP_ERROR                 AT_ERROR_NO(AT_SUB_CMD_OP_ERROR,0x00)                             /*!< the command operation type is error */

#ifndef BIT 
#define BIT(bit)                  (1u << (bit))
#endif
#define BIT_ISSET(data, bit)      ((data) & BIT(bit))

#define AT_CMD_DATA_SEND(s,len) at->device_ops.write_data((uint8_t *)(s), len)
#define AT_CMD_DATA_RECV(s,len) at->device_ops.read_data((uint8_t *)(s), len)
#define AT_CMD_RESPONSE(s) at->device_ops.write_data((uint8_t *)(s), strlen(s))
  
#define AT_CMD_PARSE_STRING(i, string, max) do { \
    if (!at_arg_get_string(argv[i], string, max)) { \
        at_cmd_set_error(AT_CMD_ERROR_PARA_PARSE_FAIL(i)); \
        return AT_RESULT_CODE_ERROR; \
    } \
} while(0);

#define AT_CMD_PARSE_NUMBER(i, num) do {\
    if (!at_arg_get_number(argv[i], num)) { \
        at_cmd_set_error(AT_CMD_ERROR_PARA_PARSE_FAIL(i)); \
        return AT_RESULT_CODE_ERROR; \
    } \
} while(0);

#define AT_CMD_PARSE_OPT_STRING(i, string, max, valid) do { \
    if(argc > i && !at_arg_is_null(argv[i])) { \
        if (!at_arg_get_string(argv[i], string, max)) { \
            at_cmd_set_error(AT_CMD_ERROR_PARA_PARSE_FAIL(i)); \
            return AT_RESULT_CODE_ERROR; \
        } \
        valid = 1; \
    } \
} while(0);

#define AT_CMD_PARSE_OPT_NUMBER(i, num, valid) do {\
    if(argc > i && !at_arg_is_null(argv[i])) { \
        if (!at_arg_get_number(argv[i], num)) { \
            at_cmd_set_error(AT_CMD_ERROR_PARA_PARSE_FAIL(i)); \
            return AT_RESULT_CODE_ERROR; \
        } \
        valid = 1; \
    } \
} while(0);

#define AT_DEBUG_POINT(a) //at_port_debug_gpio_set(a)

uint32_t at_cmd_get_version(void);

int at_cmd_get_compile_time(char *time, size_t buf_len);

void at_cmd_syslog(uint32_t error);

int at_cmd_register(const at_cmd_struct *cmds, int num_cmds);

int at_cmd_unregister(const at_cmd_struct *cmds, int num_cmds);

int at_cmd_input(uint8_t *data, int32_t len);

int at_write_data(uint8_t *data, int32_t len);

void at_write_string(const char *format, va_list args);

int at_cmd_set_error(uint32_t error);

int at_arg_is_null(const char *arg);

int at_arg_get_number(const char *arg, int *value);

int at_arg_get_string(const char *arg, char *string, int max);

#ifdef __cplusplus
}
#endif

#endif/* AT_CORE_H */

