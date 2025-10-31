#ifndef __TIMEOUT_H__
#define __TIMEOUT_H__

#include <sys/queue.h>

struct timeout_s;

typedef void (*timeout_cb_t)(struct timeout_s *);

typedef struct timeout_s {
  TAILQ_ENTRY(timeout_s) ent;
  unsigned int wakeup;

  timeout_cb_t callback;
  void *opaque;
} *timeout_t;

void timeout_start(timeout_t e, unsigned int delay);
void timeout_stop(timeout_t e);

timeout_t timeout_start_new(timeout_cb_t cb, void *opaque, unsigned int delay);

void timeout_init(void);
#endif
