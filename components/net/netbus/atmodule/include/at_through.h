/**
  ******************************************************************************
  * @file    at_through.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_THROUGH_H
#define AT_THROUGH_H

#ifdef __cplusplus
extern "C" {
#endif

#define AT_THROUGH_MAX_LEN (2*1536)
#define AT_THROUGH_EXIT_CMD "+++"

int at_through_input(uint8_t *data, int32_t len);

int at_through_max_length(void);

int at_through_set_length(int linkid, int max_size, int report_size, int transmit_size);

#ifdef __cplusplus
}
#endif

#endif/* AT_THROUGH_H */

