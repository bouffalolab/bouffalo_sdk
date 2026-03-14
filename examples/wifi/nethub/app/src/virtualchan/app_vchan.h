/**
 * @file app_vchan.h
 * @brief NetHub Virtual Channel demo code header file for user
 */

#ifndef _APP_VCHAN_H_
#define _APP_VCHAN_H_

#ifdef CONFIG_MR_VIRTUALCHAN

/**
 * @brief Initialize Virtual Channel application
 * @note This function does not register any callback, only prepares for operation
 *       Actual callback registration is triggered by vchan recv on command
 */
void app_vchan_init(void);

#endif /* CONFIG_MR_VIRTUALCHAN */

#endif /* _APP_VCHAN_H_ */
