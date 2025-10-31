#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/queue.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "timeout.h"
#include "rtos_al.h"

static TAILQ_HEAD(timeout_head, timeout_s) timeouts = TAILQ_HEAD_INITIALIZER(timeouts);

static timeout_t insert(struct timeout_s *item) {
  assert(item != NULL);
  struct timeout_s *e = NULL;

  rtos_lock();
  // insert sorted list
  TAILQ_FOREACH(e, &timeouts, ent) {
    if (e->wakeup > item->wakeup) {
      TAILQ_INSERT_BEFORE(e, item, ent);
      goto done;
    }
  }

  if (TAILQ_EMPTY(&timeouts)) {
    TAILQ_INSERT_HEAD(&timeouts, item, ent);
  } else {
    TAILQ_INSERT_TAIL(&timeouts, item, ent);
  }

done:
  e = TAILQ_FIRST(&timeouts);
  rtos_unlock();
  return e;
}

void timeout_start(timeout_t e, unsigned int delay) {
  assert(e->callback != NULL);
  unsigned int now;

  e->wakeup = rtos_now(0) + delay;

  /* enqueue and get first timer */
  e = insert(e);

  now = rtos_now(0);

  if (e->wakeup > now) {
    rtos_timeouts_start(e->wakeup - now);
  } else {
    rtos_timeouts_start(1);
  }
}

void timeout_stop(timeout_t e) {
  timeout_t v, tv;
  unsigned int now, t;

  rtos_lock();
  TAILQ_FOREACH_SAFE(v, &timeouts, ent, tv) {
    if (v == e) {
      TAILQ_REMOVE(&timeouts, e, ent);
      e = TAILQ_FIRST(&timeouts);
      rtos_unlock();

      if (e == NULL){
        t = -1;
      } else {
        now = rtos_now(0);

        if (e->wakeup > now) {
          t = e->wakeup - now;
        } else {
          t = 1;
        }
      }
      rtos_timeouts_start(t);
      return;
    }
  }
  rtos_unlock();
}

// add timeout timer
timeout_t timeout_start_new(timeout_cb_t cb, void *opaque, unsigned int delay) {
  struct timeout_s *e;

  e = malloc(sizeof(*e));
  if (e == NULL) {
    abort();
  }

  e->callback = cb;
  e->opaque = opaque;
  timeout_start(e, delay);
  return e;
}

unsigned int timeouts_handler(void) {
  timeout_t e;
  unsigned int t;

  rtos_lock();
  /* get first timer */
  e = TAILQ_FIRST(&timeouts);

  /* check timeout expires */
  if (e->wakeup <= rtos_now(0)) {
    /* remove element before invoke callback */
    TAILQ_REMOVE(&timeouts, e, ent);
    rtos_unlock();

    e->callback(e);

    rtos_lock();
    e = TAILQ_FIRST(&timeouts);
  }
  rtos_unlock();

  if (e != NULL) {
    t = rtos_now(0);
    if (e->wakeup > t) {
      t = e->wakeup - t;
    } else {
      t = 1;
    }
  } else {
    t = -1;
  }

  return t;
}

void timeout_init(void) {
  rtos_timeouts_init();
}
