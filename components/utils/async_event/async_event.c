#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "async_event.h"

extern void async_rtos_lock(void);
extern void async_rtos_unlock(void);

/* Delete event filter, internal use */
#define EV_DELETE 0xFFFFFFFF

struct filter_list_node {
    SLIST_ENTRY(filter_list_node) node;
    async_event_cb cb;
    void *priv;
    uintptr_t type_filter;
};

static SLIST_HEAD(filter_list, filter_list_node) filters = SLIST_HEAD_INITIALIZER(&filters);
static SLIST_HEAD(event_list, input_event) events = SLIST_HEAD_INITIALIZER(&events);

static void (*notify_cb)(void);

void async_event_init(void (*notify)(void))
{
    assert(notify != NULL);

    notify_cb = notify;
}

void async_event_loop(void)
{
    struct filter_list_node *fn, *fn_tmp;
    struct input_event *ie;

    async_rtos_lock();
    while ((ie = SLIST_FIRST(&events)) != NULL) {
        SLIST_REMOVE_HEAD(&events, entry);
        SLIST_FOREACH_SAFE(fn, &filters, node, fn_tmp)
        {
            async_rtos_unlock();

            if (fn->type_filter == EV_DELETE) {
                async_rtos_lock();
                SLIST_REMOVE(&filters, fn, filter_list_node, node);
                async_rtos_unlock();
                (fn->cb)(NULL, fn->priv);

                free(fn);
            } else if (fn->type_filter == EV_ALL || fn->type_filter == ie->type) {
                (fn->cb)(ie, fn->priv);
            }

            async_rtos_lock();
        }
        async_rtos_unlock();
        if (ie->finish) {
            ie->finish(ie);
        }
        free(ie);
        async_rtos_lock();
    }
    async_rtos_unlock();
}

int async_post_general_event(input_event_t event)
{
    input_event_t event_new;
    assert(event != NULL);

    if (event->size < sizeof(struct input_event)) {
        printf("%s: invalid message length\n", __func__);
        return -1;
    }

    event_new = calloc(1, event->size);
    if (event_new == NULL) {
        printf("%s: calloc failed\n", __func__);
        return -1;
    }

    memcpy(event_new, event, event->size);

    async_rtos_lock();
    SLIST_INSERT_HEAD(&events, event_new, entry);
    async_rtos_unlock();

    /* notify async event loop */
    notify_cb();

    return 0;
}

int async_post_event(uintptr_t type, uint16_t code, unsigned long value)
{
    struct input_event event;

    memset(&event, 0, sizeof(struct input_event));
    event.size = sizeof(struct input_event);
    event.type = type;

    event.code = code;
    event.value = value;

    return async_post_general_event(&event);
}

int async_register_event_filter(uintptr_t type, async_event_cb cb, void *priv)
{
    struct filter_list_node *fn;

    assert(cb != NULL);

    if (type == EV_DELETE) {
        return -1;
    }

    fn = calloc(1, sizeof(struct filter_list_node));
    if (fn == NULL) {
        return -1;
    }

    fn->type_filter = type;
    fn->cb = cb;
    fn->priv = priv;

    async_rtos_lock();
    SLIST_INSERT_HEAD(&filters, fn, node);
    async_rtos_unlock();

    return 0;
}

int async_unregister_event_filter(uintptr_t type, async_event_cb cb, void *priv)
{
    struct filter_list_node *fn, *fn_tmp;

    assert(cb == NULL);

    async_rtos_lock();
    SLIST_FOREACH_SAFE(fn, &filters, node, fn_tmp)
    {
        if (fn->type_filter != type) {
            continue;
        }

        if (fn->cb != cb) {
            continue;
        }

        if (fn->priv != priv) {
            continue;
        }

        fn->type_filter = EV_DELETE;
        break;
    }
    async_rtos_unlock();

    return -1;
}
