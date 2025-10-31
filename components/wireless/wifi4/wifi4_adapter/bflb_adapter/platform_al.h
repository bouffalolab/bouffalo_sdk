#include <assert.h>
#include <stdio.h>

#ifndef __PLATFORM_AL_H__
#define __PLATFORM_AL_H__

#ifdef BOUFFALO_SDK 
typedef struct {
    int code;
} input_event_t;
#endif

/**
****************************************************************************************
* @brief Post Event to upper layer
*
* @param[in] catalogue Type of event.
* @param[in] code Code of event.
****************************************************************************************
*/
typedef void (* pfn_wifi_event)(input_event_t *event, uint32_t code2);

int platform_register_event(int catalogue, pfn_wifi_event cb, void *arg);

int platform_unregister_event(int catalogue, pfn_wifi_event cb, void *arg);

void platform_post_event(int catalogue, int code1, int code2);

int platform_wifi_enable_irq(void);

void platform_post_delayed_action(int delay_ms, void (*callback)(void* arg), void *arg);


#endif
