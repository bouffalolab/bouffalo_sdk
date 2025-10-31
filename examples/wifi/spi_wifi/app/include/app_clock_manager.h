#ifndef APP_CLOCK_MGMT_H
#define APP_CLOCK_MGMT_H

#include <stdint.h>

// Clock source definitions
#define CLOCK_SOURCE_RC       1  // Internal RC oscillator
#define CLOCK_SOURCE_PASSIVE  2  // External passive crystal
#define CLOCK_SOURCE_ACTIVE   3  // External active crystal

// Read clock source from flash
int clock_source_read(uint8_t *clock_source);

// Write clock source to flash
int clock_source_write(uint8_t clock_source);

// Read RC calibration data from flash
int rc_cal_data_read(uint32_t *rc_cal_value);

// Write RC calibration data to flash
int rc_cal_data_write(uint32_t rc_cal_value);

int app_clock_init(void);
int app_set_clock_source(uint8_t source);
int app_get_clock_source(uint8_t *source);

#endif /* APP_CLOCK_MGMT_H */
