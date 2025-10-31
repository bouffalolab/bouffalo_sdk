#ifndef PLATFORM_AL_H_
#define PLATFORM_AL_H_

#include <stdint.h>
#include "rtos_def.h"

#if defined(CFG_BL_WIFI_PS_ENABLE) || defined(CFG_WIFI_PDS_RESUME)
#include <bl_lp.h>
#else
enum PSM_EVENT {
    PSM_EVENT_SCAN = 0,
    PSM_EVENT_CONNECT,
    PSM_EVENT_DISCONNECT,
    PSM_EVENT_PS,
    PSM_EVENT_APP,
};
#endif

/* forward decl */
typedef struct bcn_param bcn_param_t;

typedef void *  platform_event_handler_t;
typedef void (*platform_event_func_t)( platform_event_handler_t xTimer );

/**
 ****************************************************************************************
 * @brief Get MAC Address
 *
 * @param[out] mac Buffer of hold mac address.
 * @return 0 on success and OTHERS if error occurred.
 ****************************************************************************************
 */

int platform_get_mac(uint8_t *mac);

/**
 ****************************************************************************************
 * @brief Get Random
 *
 * @param[out] Get random value.
 * @return 0 on success and OTHERS if error occurred.
 ****************************************************************************************
 */
int platform_get_random(unsigned char *buf, size_t len);

/**
****************************************************************************************
* @brief Wrapper of platform malloc/free
****************************************************************************************
*/
void *platform_malloc(uint32_t size);
void platform_free(void* mem_ptr);

/**
****************************************************************************************
* @brief Add and delete schedule event.
****************************************************************************************
*/
platform_event_handler_t platform_add_schedule_event(platform_event_func_t handler, void *arg, uint32_t ms, uint8_t repeat);
int platform_delete_schedule_event(platform_event_handler_t handler);

/**
****************************************************************************************
* @brief Post Event to upper layer
*
* @param[in] catalogue Type of event.
* @param[in] code Code of event.
****************************************************************************************
*/

void platform_post_event(int catalogue, int code1, int code2);

#define PLATFORM_HOOK(x, ...) if( &platform_hook_##x ) {platform_hook_##x(__VA_ARGS__);}
/**
****************************************************************************************
* @brief hook for receive beacon
****************************************************************************************
*/
void platform_hook_beacon(uint32_t rhd,uint32_t tim_offset,bcn_param_t* param) __attribute__((weak));

/**
****************************************************************************************
* @brief hook for prevent sleep
****************************************************************************************
*/
void platform_hook_prevent_sleep(enum PSM_EVENT event, uint8_t prevent) __attribute__((weak));

/**
****************************************************************************************
* @brief hook for tcpip rx event
****************************************************************************************
*/
void platform_hook_tcpip_rx(void) __attribute__((weak));

#ifdef CFG_LPM
uint8_t platform_get_chip_version(void);
#endif
#endif
