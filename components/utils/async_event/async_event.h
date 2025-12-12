#ifndef _ASYNC_EVENTS_H_
#define _ASYNC_EVENTS_H_

#include <stdint.h>
#include <sys/queue.h>

/* special event filter */
#define EV_ALL                      0

/* system event */
#define EV_SYS                      ((uintptr_t)async_event_init)
#define CODE_SYS_ON_STARTING        1
#define CODE_SYS_ON_START_COMPLETED 2
#define CODE_SYS_ON_START_FAILED    4
#define CODE_SYS_ON_IDLE            3
#define CODE_SYS_ON_START_UOTA      5
#define CODE_SYS_ON_ALINK_ONLINE    6
#define CODE_SYS_ON_ALINK_OFFLINE   7
#define CODE_SYS_ON_MQTT_READ       8
#define CODE_SYS_ON_COAP_AUTHED     9

typedef struct input_event {
    SLIST_ENTRY(input_event) entry;
    /* Event type */
    uintptr_t type;
    /* Called when message before free */
    void (*finish)(struct input_event *);
    /* Message size, according to type */
    uint16_t size;

    /* General data; most messages may use this. */
    uint16_t code;
    unsigned long value;
} *input_event_t;

typedef void (*async_event_cb)(input_event_t event, void *private_data);

void async_event_init(void (*notify)(void));
void async_event_loop(void);

int async_post_general_event(input_event_t event);
int async_post_event(uintptr_t type, uint16_t code, unsigned long value);
int async_register_event_filter(uintptr_t type, async_event_cb cb, void *priv);
int async_unregister_event_filter(uintptr_t type, async_event_cb cb, void *priv);

#endif
