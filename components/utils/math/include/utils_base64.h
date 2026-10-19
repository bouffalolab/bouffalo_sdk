/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef _IOTX_COMMON_BASE64_H_
#define _IOTX_COMMON_BASE64_H_

#include <stdint.h>

#define UTILS_BASE64_TEST

typedef enum iotx_err
{
    FAIL_RETURN = 0,
    SUCCESS_RETURN
}iotx_err_t;

iotx_err_t utils_base64encode(const uint8_t *data, uint32_t inputLength, uint32_t outputLenMax,
                              uint8_t *encodedData, uint32_t *outputLength);
iotx_err_t utils_base64decode(const uint8_t *data, uint32_t inputLength, uint32_t outputLenMax,
                              uint8_t *decodedData, uint32_t *outputLength);

void utils_base64_encode_stream(int (*read_data)(uint8_t *data, void *opaque), void (*write_data)(const uint8_t data[4], void *opaque), void *opaque);
int base64_cli_init(void);

#endif
