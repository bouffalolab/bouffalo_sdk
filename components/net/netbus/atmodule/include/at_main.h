/**
  ******************************************************************************
  * @file    at_main.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_MAIN_H
#define AT_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CMD_BIN_VERSION "1.0.0"

#define AT_CMD_MAX_NUM 256
#define AT_CMD_MAX_LEN 256
#define AT_CMD_MAX_PARA 16
#define AT_CMD_MAX_FUNC 8

typedef struct {
    char *at_name;                        
    int (*at_test_cmd)(int argc, const char **argv);      
    int (*at_query_cmd)(int argc, const char **argv);
    int (*at_setup_cmd)(int argc, const char **argv);
    int (*at_exe_cmd)(int argc, const char **argv);
    int para_num_min;
    int para_num_max;
} at_cmd_struct;

typedef enum {
    AT_RESULT_CODE_OK           = 0x00, /*!< "OK" */
    AT_RESULT_CODE_ERROR        = 0x01, /*!< "ERROR" */
    AT_RESULT_CODE_FAIL         = 0x02, /*!< "ERROR" */
    AT_RESULT_CODE_SEND_OK      = 0x03, /*!< "SEND OK" */
    AT_RESULT_CODE_SEND_FAIL    = 0x04, /*!< "SEND FAIL" */
    AT_RESULT_CODE_IGNORE       = 0x05, /*!< response nothing, just change internal status */
    AT_RESULT_CODE_PROCESS_DONE = 0x06, /*!< response nothing, just change internal status */
    AT_RESULT_CODE_MAX
} at_result_code_string_index;

#define AT_RESULT_WITH_SUB_CODE(sub_code) ((sub_code << 8) | AT_RESULT_CODE_ERROR)

typedef enum {
    AT_WORK_MODE_CMD = 0x00,
    AT_WORK_MODE_THROUGHPUT,
    AT_WORK_MODE_CMD_THROUGHPUT
} at_work_mode;

typedef struct {
    int (*init_device)(void);
    int (*deinit_device)(void); 
    int (*read_data) (uint8_t *data, int len);
    int (*write_data) (uint8_t *data, int len);
    int (*f_output_redirect) (void);
} at_device_ops;

typedef int (*at_func)(void);

typedef struct {
    at_func restore_func;
    at_func stop_func;
} at_function_ops;

typedef void (*at_work_handler_t)(int eventid, void *);

struct at_workq {
    at_work_handler_t pfunc;
    void *arg;
#define AT_EVENT_OTA            (10)
#define AT_EVENT_SOCKET_CLOSE   (11)
    uint16_t eventid;
};

struct at_struct {
    int initialized;
    int echo;
    int syslog;
    int store;
    int fakeoutput;
    int exit;
    at_work_mode incmd;
    at_device_ops device_ops;
    int function_num;
    at_function_ops function_ops[AT_CMD_MAX_FUNC];

    const at_cmd_struct *commands[AT_CMD_MAX_NUM];
    int num_commands;
    char *inbuf;
};

extern struct at_struct *at;

int at_module_init(void);

int at_module_deinit(void);

uint64_t at_current_ms_get();

int at_register_function(at_func restore, at_func stop);

void at_response_result(int result_code);

void at_response_string(const char *format, ... );

void at_write(const char *format , ...);

int at_set_work_mode(at_work_mode mode);

at_work_mode at_get_work_mode(void);

int at_module_func(char *cmd, int (*resp_func) (uint8_t *data, int len));

int at_output_redirect_register(int (*output_redirect) (void));

int at_output_is_redirect();

int at_workq_send(int eventid, struct at_workq *q, int timeout);

int at_workq_dowork(int eventid, int timeout);

#ifdef __cplusplus
}
#endif

#endif/* AT_MAIN_H */

