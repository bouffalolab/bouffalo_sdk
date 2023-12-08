#ifndef _BFLB_RTC_H
#define _BFLB_RTC_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup RTC
  * @{
  */

#define BFLB_RTC_SEC2TIME(s)    (s * 32768)
#define BFLB_RTC_TIME2SEC(time) (time / 32768)

/* This struct is the same with struct tm */
struct bflb_tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief clear and disable rtc.
 *
 * @param [in] dev device handle
 */
void bflb_rtc_disable(struct bflb_device_s *dev);

/**
 * @brief Set rtc alarming time.
 *
 * @param [in] dev device handle
 * @param [in] time alarming time, unit is (1/32768 s)
 */
void bflb_rtc_set_time(struct bflb_device_s *dev, uint64_t time);

/**
 * @brief Get rtc current time.
 *
 * @param [in] dev device handle
 * @return current rtc running time
 */
uint64_t bflb_rtc_get_time(struct bflb_device_s *dev);

/**
 * @brief Set current utc time.
 *
 * @param [in] time tm handle
 */
void bflb_rtc_set_utc_time(const struct bflb_tm *time);

/**
 * @brief Get current utc time.
 *
 * @param [out] time tm handle
 */
void bflb_rtc_get_utc_time(struct bflb_tm *time);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif