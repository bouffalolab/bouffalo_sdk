/*
 * Event loop based on rtos_queue
 * Copyright (c) 2002-2009
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include <assert.h>

#include "common.h"
#include "trace.h"
#include "list.h"
#include "os.h"
#include "eloop.h"
#include "eloop_rtos.h"
#include "rtos_def.h"

#undef wpa_printf
#define wpa_printf(l, f, ...) printf(f, ##__VA_ARGS__)

#define ELOOP_MSGQ_SIZE_MAX   (16)

struct eloop_timeout {
  struct dl_list entry;
  struct os_reltime time;
  void *eloop_data;
  void *user_data;
  eloop_timeout_handler handler;
};

struct eloop_event {
  struct dl_list entry;
  unsigned int event_id;
  void *eloop_data;
  void *user_data;
  eloop_sock_handler handler;
};

struct eloop_event_request {
  struct dl_list entry;
  unsigned int event_id;

  unsigned int body_len;
  void *body;
  void (*done)(void *);
};

struct eloop_data {
  rtos_mutex mtx;
  rtos_task_handle eloop_task;

  /* tirgger it when new event coming */
  rtos_semaphore event_trigger;

  /* the list for all kinds of events */
  struct dl_list event_handlers;

  /* the list for all pending events */
  struct dl_list event_requests;

  /* timer events */
  struct dl_list timeout;

#define ELOOP_STATE_STOP     0
#define ELOOP_STATE_RUNNING  1
#define ELOOP_STATE_STOPPING 2
  int state;
};

static struct eloop_data eloop;

int eloop_init(void)
{
  os_memset(&eloop, 0, sizeof(eloop));
  dl_list_init(&eloop.timeout);
  dl_list_init(&eloop.event_requests);
  dl_list_init(&eloop.event_handlers);

  if (rtos_mutex_create(&eloop.mtx)) {
    wpa_printf(MSG_ERROR, "failed to create mutex\r\n");
    return -1;
  }

  if (rtos_semaphore_create(&eloop.event_trigger, -1, 0) < 0) {
    wpa_printf(MSG_ERROR, "failed to create eloop_msg sema\r\n");
    rtos_mutex_delete(eloop.mtx);
    return -1;
  }

  return 0;
}

int eloop_sock_requeue(void)
{
  wpa_printf(MSG_ERROR, "eloop_msgq %s is NOT supported yet!\r\n", __func__);
  return 0;
}

int eloop_register_read_sock(int sock, eloop_sock_handler handler,
                             void *eloop_data, void *user_data)
{
  struct eloop_event *event = os_zalloc(sizeof(struct eloop_event));
  if (event == NULL) {
    wpa_printf(MSG_ERROR, "regist event %d failed\r\n", sock);
    return -1;
  }

  // sock is event id
  dl_list_init(&event->entry);
  event->event_id = sock;
  event->user_data = user_data;
  event->eloop_data = eloop_data;
  event->handler = handler;

  rtos_mutex_lock(eloop.mtx);
  dl_list_add_tail(&eloop.event_handlers, &event->entry);
  rtos_mutex_unlock(eloop.mtx);

  return 0;
}

void eloop_unregister_read_sock(int sock)
{
  struct eloop_event *item, *prev_item;

  rtos_mutex_lock(eloop.mtx);
  dl_list_for_each_safe(item, prev_item, &eloop.event_handlers,
                        struct eloop_event, entry) {
    if (item->event_id == sock) {
      dl_list_del(&item->entry);
      os_free(item);
    }
  }
  rtos_mutex_unlock(eloop.mtx);
}

/* eloop timeouts */
int eloop_register_timeout(unsigned int secs, unsigned int usecs,
                           eloop_timeout_handler handler,
                           void *eloop_data, void *user_data)
{
  struct eloop_timeout *timeout, *tmp;
  os_time_t now_sec;

  timeout = os_zalloc(sizeof(*timeout));
  if (timeout == NULL) {
    return -1;
  }

  if (os_get_reltime(&timeout->time) < 0) {
    os_free(timeout);
    return -1;
  }

  now_sec = timeout->time.sec;
  timeout->time.sec += secs;
  if (timeout->time.sec < now_sec) {
    /*
     * Integer overflow - assume long enough timeout to be assumed
     * to be infinite, i.e., the timeout would never happen.
     */
    wpa_printf(MSG_DEBUG, "ELOOP: Too long timeout (secs=%u) to "
                          "ever happen - ignore it",
               secs);
    os_free(timeout);
    return 0;
  }

  timeout->time.usec += usecs;

  while (timeout->time.usec >= 1000000) {
    timeout->time.sec++;
    timeout->time.usec -= 1000000;
  }

  timeout->eloop_data = eloop_data;
  timeout->user_data = user_data;
  timeout->handler = handler;

  rtos_mutex_lock(eloop.mtx);
  /* Maintain timeouts in order of increasing time */
  dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, entry) {
    if (os_reltime_before(&timeout->time, &tmp->time)) {
      dl_list_add(tmp->entry.prev, &timeout->entry);
      rtos_mutex_unlock(eloop.mtx);
      return 0;
    }
  }
  dl_list_add_tail(&eloop.timeout, &timeout->entry);
  rtos_mutex_unlock(eloop.mtx);

  return 0;
}

static void eloop_remove_timeout(struct eloop_timeout *timeout)
{
  dl_list_del(&timeout->entry);
  os_free(timeout);
}

int eloop_cancel_timeout(eloop_timeout_handler handler,
                         void *eloop_data, void *user_data)
{
  struct eloop_timeout *timeout, *prev;
  int removed = 0;

  rtos_mutex_lock(eloop.mtx);
  dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                        struct eloop_timeout, entry)
  {
    if (timeout->handler == handler &&
      (timeout->eloop_data == eloop_data || eloop_data == ELOOP_ALL_CTX) &&
      (timeout->user_data == user_data || user_data == ELOOP_ALL_CTX)) {
      eloop_remove_timeout(timeout);
      removed++;
    }
  }
  rtos_mutex_unlock(eloop.mtx);

  return removed;
}

int eloop_cancel_timeout_one(eloop_timeout_handler handler,
                             void *eloop_data, void *user_data,
                             struct os_reltime *remaining)
{
  struct eloop_timeout *timeout, *prev;
  int removed = 0;
  struct os_reltime now;

  os_get_reltime(&now);
  remaining->sec = remaining->usec = 0;

  rtos_mutex_lock(eloop.mtx);
  dl_list_for_each_safe(timeout, prev, &eloop.timeout, struct eloop_timeout, entry)
  {
    if (timeout->handler == handler && (timeout->eloop_data == eloop_data) &&
      (timeout->user_data == user_data)) {
      removed = 1;

      if (os_reltime_before(&now, &timeout->time)) {
          os_reltime_sub(&timeout->time, &now, remaining);
      }

      eloop_remove_timeout(timeout);
      break;
    }
  }
  rtos_mutex_unlock(eloop.mtx);
  return removed;
}

int eloop_is_timeout_registered(eloop_timeout_handler handler,
                                void *eloop_data, void *user_data)
{
  struct eloop_timeout *tmp;

  rtos_mutex_lock(eloop.mtx);
  dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, entry) {
    if (tmp->handler == handler && tmp->eloop_data == eloop_data &&
      tmp->user_data == user_data)
    {
      rtos_mutex_unlock(eloop.mtx);
      return 1;
    }
  }
  rtos_mutex_unlock(eloop.mtx);

  return 0;
}

// Reduce the waiting time for one timeout
int eloop_deplete_timeout(unsigned int req_secs, unsigned int req_usecs,
                          eloop_timeout_handler handler, void *eloop_data,
                          void *user_data)
{
  struct os_reltime now, requested, remaining;
  struct eloop_timeout *tmp;

  rtos_mutex_lock(eloop.mtx);
  dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, entry)
  {
    if (tmp->handler == handler && tmp->eloop_data == eloop_data &&
      tmp->user_data == user_data)
    {
      requested.sec = req_secs;
      requested.usec = req_usecs;

      os_get_reltime(&now);
      os_reltime_sub(&tmp->time, &now, &remaining);

      if (os_reltime_before(&requested, &remaining)) {
        eloop_cancel_timeout(handler, eloop_data, user_data);

        eloop_register_timeout(requested.sec, requested.usec, handler, eloop_data,
                               user_data);

        rtos_mutex_unlock(eloop.mtx);
        return 1;
      }
      rtos_mutex_unlock(eloop.mtx);
      return 0;
    }
  }

  rtos_mutex_unlock(eloop.mtx);
  return -1;
}

// Increase the waiting time for one timeout
int eloop_replenish_timeout(unsigned int req_secs, unsigned int req_usecs,
                            eloop_timeout_handler handler, void *eloop_data,
                            void *user_data)
{
  struct os_reltime now, requested, remaining;
  struct eloop_timeout *tmp;

  rtos_mutex_lock(eloop.mtx);
  dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, entry)
  {
    if (tmp->handler == handler && tmp->eloop_data == eloop_data &&
      tmp->user_data == user_data) {

      requested.sec = req_secs;
      requested.usec = req_usecs;

      os_get_reltime(&now);
      os_reltime_sub(&tmp->time, &now, &remaining);

      if (os_reltime_before(&remaining, &requested)) {
        eloop_cancel_timeout(handler, eloop_data, user_data);
        eloop_register_timeout(requested.sec, requested.usec, handler, eloop_data,
                               user_data);

        rtos_mutex_unlock(eloop.mtx);
        return 1;
      }
      rtos_mutex_unlock(eloop.mtx);
      return 0;
    }
  }

  rtos_mutex_unlock(eloop.mtx);
  return -1;
}

int eloop_register_signal_terminate(eloop_signal_handler handler,
                                    void *user_data)
{
  wpa_printf(MSG_WARNING, "SIGTERM is NOT supported yet!\r\n");
  return 0;
}

int eloop_register_signal_reconfig(eloop_signal_handler handler,
                                   void *user_data)
{
  wpa_printf(MSG_WARNING, "SIGHUP is NOT supported yet!\r\n");
  return 0;
}

/* post a event to eloop */
int eloop_post_event(int sock, unsigned int body_len, void *body, void (*done)(void *)) {
  struct eloop_event_request *evreq = NULL;

  evreq = os_zalloc(sizeof(struct eloop_event_request));
  if (evreq == NULL) {
    wpa_printf(MSG_WARNING, "failed to create request\r\n");
    return -1;
  }

  dl_list_init(&evreq->entry);
  evreq->event_id = sock;
  evreq->body = body;
  evreq->body_len = body_len;
  evreq->done = done;

  rtos_mutex_lock(eloop.mtx);
  dl_list_add_tail(&eloop.event_requests, &evreq->entry);
  rtos_mutex_unlock(eloop.mtx);

  /* trigger eloop */
  rtos_semaphore_signal(eloop.event_trigger, 0);
  return 0;
}

/* get current request body in handler */
unsigned int eloop_get_request_body(void ** body) {
  struct eloop_event_request *evreq;

  rtos_mutex_lock(eloop.mtx);
  evreq = dl_list_first(&eloop.event_requests, struct eloop_event_request, entry);
  rtos_mutex_unlock(eloop.mtx);

  assert(evreq != NULL);
  *body = evreq->body;
  return evreq->body_len;
}

/* sync done cb */
static void event_commit_sync_done(void *data) {
  struct event_sync_msg *msg = data;

  rtos_semaphore_signal(msg->done, 0);
}

/////////////////////////////////////////////////////////////////////////////
int eloop_event_commit_sync(int type, const char *request, int req_len,
                            char **resp, int *resp_len, int timeout_ms) {
  int ret = 0;
  struct event_sync_msg * data = os_malloc(sizeof(struct event_sync_msg) + req_len);

  memcpy(data + 1, request, req_len);

  rtos_semaphore_create(&data->done, 1, 0);
  data->resp = resp;
  data->resp_len = resp_len;

  ret = eloop_post_event(type, sizeof(struct event_sync_msg) + req_len, data, event_commit_sync_done);
  if (ret) {
    goto exit;
  }
  rtos_semaphore_wait(data->done, timeout_ms);

exit:
  rtos_semaphore_delete(data->done);
  os_free(data);

  return ret;
}

int eloop_event_commit(int type, const char *request, int req_len) {
  unsigned char * data = os_malloc(req_len);
  memcpy(data, request, req_len);

  return eloop_post_event(type, req_len, data, os_free);
}

/////////////////////////////////////////////////////////////////////////////

static void eloop_process_events(void) {
  struct eloop_event_request *evreq;
  struct eloop_event *tmp;

retry:
  rtos_mutex_lock(eloop.mtx);
  evreq = dl_list_first(&eloop.event_requests, struct eloop_event_request, entry);
  if (evreq == NULL) {
    goto exit;
  }

  dl_list_for_each(tmp, &eloop.event_handlers, struct eloop_event, entry) {
    if (tmp->event_id == evreq->event_id) {
      rtos_mutex_unlock(eloop.mtx);
      // invoke handler
      tmp->handler(tmp->event_id, tmp->eloop_data, tmp->user_data);
      rtos_mutex_lock(eloop.mtx);
      break; /* yes, we break loop, it means one event only have one handler */
    }
  }
  // remove req
  dl_list_del(&evreq->entry);
  rtos_mutex_unlock(eloop.mtx);

  // do the req done cb
  if (evreq->done) evreq->done(evreq->body);
  os_free(evreq);

  goto retry;

exit:
  rtos_mutex_unlock(eloop.mtx);
  return;
}

static unsigned int eloop_process_timeouts(void) {
  struct eloop_timeout *timeout;
  struct os_reltime now;

  unsigned int timeout_ms = -1;

  rtos_mutex_lock(eloop.mtx);
retry:
  /* check if some registered timeouts have occurred */
  timeout = dl_list_first(&eloop.timeout, struct eloop_timeout, entry);

  /* handle timeouts */
  if (timeout) {
    os_get_reltime(&now);
    if (!os_reltime_before(&now, &timeout->time)) {
      void *eloop_data = timeout->eloop_data;
      void *user_data = timeout->user_data;
      eloop_timeout_handler handler = timeout->handler;

      // XXX why delete before invoke handle?
      eloop_remove_timeout(timeout);

      rtos_mutex_unlock(eloop.mtx);
      handler(eloop_data, user_data);
      rtos_mutex_lock(eloop.mtx);

      goto retry;
    } else {
      struct os_reltime tv;
      os_reltime_sub(&timeout->time, &now, &tv);
      timeout_ms = tv.sec * 1000 + tv.usec / 1000;
    }
  }
  rtos_mutex_unlock(eloop.mtx);

  return timeout_ms;
}

void eloop_run(void)
{
  unsigned int timeout_ms;

  eloop.state = ELOOP_STATE_RUNNING;
  eloop.eloop_task = rtos_task_get_handle(NULL);

  while (eloop.state == ELOOP_STATE_RUNNING) {
    // dispatch events
    eloop_process_events();

    timeout_ms = eloop_process_timeouts();

    /* waiting events before timeouts arrive */
    rtos_semaphore_wait(eloop.event_trigger, timeout_ms);
  }

  eloop.state = ELOOP_STATE_STOP;
  rtos_mutex_unlock(eloop.mtx);
  wpa_printf(MSG_INFO, "eloop exits\r\n");
}

void eloop_terminate(void)
{
  eloop.state = ELOOP_STATE_STOPPING;
}

void eloop_destroy(void)
{
  struct eloop_timeout *timeout, *prev;
  dl_list_for_each_safe(timeout, prev, &eloop.timeout,
                        struct eloop_timeout, entry)
  {
    eloop_remove_timeout(timeout);
  }

  rtos_semaphore_delete(eloop.event_trigger);
  rtos_mutex_delete(eloop.mtx);
}

int eloop_terminated(void)
{
  return eloop.state == ELOOP_STATE_STOP;
}

void eloop_wait_for_read_sock(int sock)
{
  wpa_printf(MSG_WARNING, "%s is NOT supported yet\r\n", __func__);
}
