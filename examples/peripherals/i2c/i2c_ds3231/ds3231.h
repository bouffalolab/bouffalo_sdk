#ifndef _DS3231_H
#define _DS3231_H

void ds3231_write_reg(uint8_t RegAddress, uint8_t Data);
uint8_t ds3231_read_reg(uint8_t RegAddress);
void ds3231_init(void);
void ds3231_set_time(uint8_t year,uint8_t mon,uint8_t data,uint8_t week,uint8_t hour,uint8_t min,uint8_t sec);
void ds3231_get_time(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week, uint8_t *hour, uint8_t *min, uint8_t *sec);

#endif
