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

#if defined(CONFIG_BLUETOOTH_APP) || defined(CONFIG_ATMODULE_FULL_FEAT)
#define AT_THROUGH_MAX_LEN (1*1536)
#else
#define AT_THROUGH_MAX_LEN (256)
#endif
#define AT_THROUGH_EXIT_CMD "+++"

void at_through_escape_reset(void);

/* Read timeout in RTOS ticks. */
uint32_t at_through_read_timeout(void);

/* len == 0 polls buffered data and the escape guard; remaining is the unconsumed input length. */
int at_through_input(uint8_t *data, int32_t len, int *remaining);

int at_through_max_length(void);

int at_through_set_length(int linkid, int max_size, int report_size, int transmit_size);

#ifdef __cplusplus
}
#endif

#endif/* AT_THROUGH_H */

