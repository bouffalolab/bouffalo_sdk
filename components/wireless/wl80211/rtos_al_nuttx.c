/**
 ****************************************************************************************
 *
 * @file rtos_al.c
 *
 * @brief RTOS abstraction layer implementation for wl80211 WiFi driver.
 *
 ****************************************************************************************
 */

#include <debug.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>

#include "wl80211_platform.h"

#include <sched.h>
#include <nuttx/clock.h>
#include <nuttx/semaphore.h>
#include <nuttx/mutex.h>
#include <nuttx/wqueue.h>
#include <nuttx/irq.h>
#include <nuttx/sched.h>
#include <nuttx/wdog.h>

#include <bflb_efuse.h>
#include <bflb_sec_trng.h>
#include "bl616_efuse.h"

#include "macsw.h"
#include "wl80211_mac.h"

/* Forward declarations for static functions */
static struct work_s g_timeout_work;
static void timeout_worker(void *arg);
static void timeout_watchdog_handler(wdparm_t arg);

/* Event queue structure */
struct event_node_s
{
  sq_entry_t node;
  uint32_t code1;
  uint32_t code2;
};

static sq_queue_t g_event_queue;
static mutex_t g_event_queue_mutex;

/* Event work structure */
static struct work_s g_event_work;

/* Timer management structure for high-precision timeouts */

struct rtos_timer_s
{
  struct wdog_s timer;      /* NuttX watchdog timer for precise timing */
  struct work_s work;       /* Work queue structure for callback execution */
  rtos_timer_cb cb;         /* User callback function */
  void *arg;                /* Callback argument */
  volatile bool is_pending; /* Work queue pending flag */
};

/* Global timeout timer instance */
static struct rtos_timer_s g_timeout_timer;

const struct platform_feature wl80211_platform_feature[] = { { 2, 1, 1 }, { 1, 0, 0 } };

/// RX Packet Reordering Buffer Size
#define MACSW_AMPDU_RX_BUF_SIZE CFG_REORD_BUF
#if (MACSW_AMPDU_RX && ((MACSW_AMPDU_RX_BUF_SIZE < 4) || (MACSW_AMPDU_RX_BUF_SIZE > 64)))
#error "Incorrect reordering buffer size"
#endif

// for station/ap mode
#define WL80211_RX_DESC_MPDU_LEN  (sizeof(struct wl80211_mac_rx_desc) + CO_ALIGN4_HI(RX_MAX_AMSDU_SUBFRAME_LEN + 1))
// for monitor mode
#define WL80211_RX_DESC_AMSDU_LEN (sizeof(struct wl80211_mac_rx_desc) + CO_ALIGN4_HI(MACSW_CONFIG(MACSW_MAX_AMSDU_RX)))

#define WL80211_RX_BUF_MEM_LEN    ((MACSW_MAX_BA_RX * MACSW_AMPDU_RX_BUF_SIZE + 2) * WL80211_RX_DESC_MPDU_LEN)
const unsigned int wl80211_rx_buf_mem_len = WL80211_RX_BUF_MEM_LEN;
uint32_t wl80211_rx_buf_mem[CO_ALIGN4_HI(WL80211_RX_BUF_MEM_LEN) / sizeof(uint32_t)] __SHAREDRAM;

/**
 ****************************************************************************************
 * @brief Convert milliseconds to RTOS ticks.
 *
 * @param[in] ms Time in milliseconds to convert.
 * @return Number of equivalent RTOS ticks.
 ****************************************************************************************
 */
uint32_t rtos_al_ms2tick(int ms)
{
  return MSEC2TICK(ms);
}

/**
 ****************************************************************************************
 * @brief Convert timeout in milliseconds to RTOS tick count.
 *
 * @param[in] timeout_ms Timeout in milliseconds (ERROR for infinite wait).
 * @return Number of ticks or TICK_WAIT for infinite wait.
 ****************************************************************************************
 */
static int rtos_timeout_2_tickcount(int timeout_ms)
{
  return MSEC2TICK(timeout_ms);
}

/**
 ****************************************************************************************
 * @brief Enter critical section to protect shared resources.
 ****************************************************************************************
 */
void rtos_lock(void)
{
  sched_lock();
}

/**
 ****************************************************************************************
 * @brief Exit critical section after protecting shared resources.
 ****************************************************************************************
 */
void rtos_unlock(void)
{
  sched_unlock();
}

/**
 ****************************************************************************************
 * @brief Get current system time in milliseconds.
 *
 * @param[in] isr Whether called from interrupt context.
 * @return Current time in milliseconds.
 ****************************************************************************************
 */
uint32_t rtos_now(bool isr)
{
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/**
 ****************************************************************************************
 * @brief Create a semaphore.
 *
 * @param[out] semaphore Pointer to semaphore handle.
 * @param[in] max_count Maximum count for counting semaphores.
 * @param[in] init_count Initial count value.
 * @return 0 on success, ERROR on failure.
 ****************************************************************************************
 */
int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count,
                          int init_count)
{
  sem_t *sem;
  int ret;

  /* Allocate semaphore structure */
  sem = (sem_t *)malloc(sizeof(sem_t));
  if (!sem)
    {
      return ERROR;
    }

  /* Initialize the semaphore */
  ret = nxsem_init(sem, 0, init_count);
  if (ret != 0)
    {
      free(sem);
      return ERROR;
    }

  *semaphore = (rtos_semaphore)sem;
  return 0;
}

/**
 ****************************************************************************************
 * @brief Delete a semaphore.
 *
 * @param[in] semaphore Semaphore handle to delete.
 ****************************************************************************************
 */
void rtos_semaphore_delete(rtos_semaphore semaphore)
{
  sem_t *sem = (sem_t *)semaphore;

  if (sem)
    {
      nxsem_destroy(sem);
      free(sem);
    }
}

/**
 ****************************************************************************************
 * @brief Wait for a semaphore with timeout.
 *
 * @param[in] semaphore Semaphore handle.
 * @param[in] timeout Timeout in milliseconds (ERROR for infinite wait).
 * @return 0 on success, ERROR on timeout.
 ****************************************************************************************
 */
int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout)
{
  sem_t *sem = (sem_t *)semaphore;
  int ret;

  if (timeout < 0)
    {
      /* Infinite wait */
      ret = nxsem_wait(sem);
    }
  else
    {
      /* Timed wait */
      ret = nxsem_tickwait(sem, rtos_timeout_2_tickcount(timeout));
    }

  if (ret != 0)
    {
      wlerr("rtos_semaphore_wait failed or timed out\n");
    }

  return (ret != 0) ? ERROR : OK;
}

/**
 ****************************************************************************************
 * @brief Signal/release a semaphore.
 *
 * @param[in] semaphore Semaphore handle.
 * @param[in] isr Whether called from interrupt context.
 * @return 0 on success, ERROR on failure.
 ****************************************************************************************
 */
int rtos_semaphore_signal(rtos_semaphore semaphore, bool isr)
{
  sem_t *sem = (sem_t *)semaphore;
  int ret;

  /* NuttX doesn't distinguish between ISR and task context for semaphores */
  ret = nxsem_post(sem);

  if (ret != 0)
    {
      wlerr("rtos_semaphore_signal failed\n");
    }

  return (ret != 0) ? ERROR : OK;
}

/**
 ****************************************************************************************
 * @brief Create a mutex.
 *
 * @param[out] mutex Pointer to mutex handle.
 * @return 0 on success, ERROR on failure.
 ****************************************************************************************
 */
int rtos_mutex_create(rtos_mutex *mutex)
{
  mutex_t *mtx;
  int ret;

  /* Allocate mutex structure */
  mtx = (mutex_t *)malloc(sizeof(mutex_t));
  if (!mtx)
    {
      return ERROR;
    }

  /* Initialize the mutex */
  ret = nxmutex_init(mtx);
  if (ret != 0)
    {
      free(mtx);
      return ERROR;
    }

  *mutex = (rtos_mutex)mtx;
  return OK;
}

/**
 ****************************************************************************************
 * @brief Delete a mutex.
 *
 * @param[in] mutex Mutex handle to delete.
 ****************************************************************************************
 */
void rtos_mutex_delete(rtos_mutex mutex)
{
  mutex_t *mtx = (mutex_t *)mutex;

  if (mtx)
    {
      /* Ensure mutex is not held before destroying */
      nxmutex_destroy(mtx);
      free(mtx);
    }
}

/**
 ****************************************************************************************
 * @brief Lock a mutex (blocking).
 *
 * @param[in] mutex Mutex handle to lock.
 ****************************************************************************************
 */
void rtos_mutex_lock(rtos_mutex mutex)
{
  mutex_t *mtx = (mutex_t *)mutex;

  nxmutex_lock(mtx);
}

/**
 ****************************************************************************************
 * @brief Unlock a mutex.
 *
 * @param[in] mutex Mutex handle to unlock.
 ****************************************************************************************
 */
void rtos_mutex_unlock(rtos_mutex mutex)
{
  mutex_t *mtx = (mutex_t *)mutex;

  nxmutex_unlock(mtx);
}

/**
 ****************************************************************************************
 * @brief Watchdog timer handler - executed in interrupt context
 *
 * @param[in] arg Timer argument
 ****************************************************************************************
 */
static void timeout_watchdog_handler(wdparm_t arg)
{
  struct rtos_timer_s *timer = (struct rtos_timer_s *)arg;

  if (!timer)
    {
      return;
    }

  /* Check if work is already pending to avoid duplicate callbacks */

  if (timer->is_pending)
    {
      /* Already pending, skip to avoid duplicate callbacks */
      return;
    }

  /* Set pending flag */

  timer->is_pending = true;

  /* Queue work to LPWORK for safe callback execution */

  int ret = work_queue(HPWORK, &timer->work, timeout_worker, timer, 0);
  if (ret != 0)
    {
      wlerr("Failed to queue timeout work: %d\n", ret);
      timer->is_pending = false;
    }
}

/**
 ****************************************************************************************
 * @brief Timeout worker function executed in work queue context.
 *
 * @param[in] arg Pointer to timeout timer structure.
 ****************************************************************************************
 */
static void timeout_worker(void *arg)
{
  struct rtos_timer_s *timer = (struct rtos_timer_s *)arg;
  unsigned int next_delay;

  if (!timer)
    {
      return;
    }

  /* Call the external timeout handler */
  extern unsigned int timeouts_handler(void);
  next_delay = timeouts_handler();

  wlinfo("Timeout handler executed, next delay: %u ms\n", next_delay);

  /* Clear pending flag */
  timer->is_pending = false;

  /* Schedule the next timeout using watchdog for better precision */
  if (next_delay != (unsigned int)-1)
    {
      unsigned int delay_ticks = (next_delay == 0) ? 1 : MSEC2TICK(next_delay);
      wd_start(&timer->timer, delay_ticks, timeout_watchdog_handler, (wdparm_t)timer);
    }
}

/**
 ****************************************************************************************
 * @brief Initialize the timeout system.
 ****************************************************************************************
 */
void rtos_timeouts_init(void)
{
  int ret;

  /* Initialize the event queue */

  sq_init(&g_event_queue);

  /* Initialize the event queue mutex */

  ret = nxmutex_init(&g_event_queue_mutex);
  if (ret != 0)
    {
      wlerr("Failed to initialize event queue mutex\n");
    }

  /* Initialize the timeout timer structure */

  memset(&g_timeout_timer, 0, sizeof(struct rtos_timer_s));
  g_timeout_timer.cb = NULL;
  g_timeout_timer.arg = NULL;
  g_timeout_timer.is_pending = false;

  /* Initialize the work structure */

  memset(&g_timeout_work, 0, sizeof(struct work_s));
}

/**
 ****************************************************************************************
 * @brief Start timeout processing with specified delay.
 *
 * @param[in] delay Initial delay in milliseconds.
 ****************************************************************************************
 */
void rtos_timeouts_start(unsigned int delay)
{
  /* Cancel any existing timeout */
  wd_cancel(&g_timeout_timer.timer);

  if (delay == (unsigned int)-1)
    {
      /* No timeout needed */
      return;
    }

  unsigned int delay_ticks = delay == 0 ? 1 : MSEC2TICK(delay);

  wlinfo("Starting timeouts with delay: %u ms (%u ticks)\n", delay, delay_ticks);

  /* Start watchdog timer for high-precision timeout */

  wd_start(&g_timeout_timer.timer, delay_ticks, timeout_watchdog_handler, (wdparm_t)&g_timeout_timer);
}

/**
 ****************************************************************************************
 * @brief Async event handler executed in work queue context.
 *
 * @param[in] arg Pointer to event data (not used, data stored in globals).
 ****************************************************************************************
 */
static void async_event_handler(void *arg)
{
  struct event_node_s *event;
  sq_entry_t *node;
  bool more_events;

  /* Prevent blocking operations in work queue context by using scheduler lock */
  sched_lock();

  /* User defined wifi event handler */
  extern void bl616_wifi_event_handler(void *arg1, uint32_t arg2);

  do
    {
      /* Lock the queue */
      nxmutex_lock(&g_event_queue_mutex);

      /* Check if there are events in queue */
      if (!sq_empty(&g_event_queue))
        {
          /* Get first event from queue */
          node = sq_remfirst(&g_event_queue);
          nxmutex_unlock(&g_event_queue_mutex);

          /* Get event structure from node */
          event = (struct event_node_s *)node;

          /* Call the user-defined event handler */
          bl616_wifi_event_handler((void *)event->code1, event->code2);

          /* Free event node */
          free(event);

          /* Check if there are more events */
          nxmutex_lock(&g_event_queue_mutex);
          more_events = !sq_empty(&g_event_queue);
          nxmutex_unlock(&g_event_queue_mutex);
        }
      else
        {
          nxmutex_unlock(&g_event_queue_mutex);
          more_events = false;
        }
    }
  while (more_events);

  sched_unlock();
}

/**
 ****************************************************************************************
 * @brief Post Event to upper layer.
 *
 * @param[in] code1 Event code 1.
 * @param[in] code2 Event code 2.
 ****************************************************************************************
 */
void wl80211_post_event(int code1, int code2)
{
  struct event_node_s *event;
  int ret;

  /* Allocate event node */
  event = (struct event_node_s *)malloc(sizeof(struct event_node_s));
  if (!event)
    {
      wlerr("Failed to allocate event node\n");
      return;
    }

  /* Initialize event node */
  event->code1 = (uint32_t)code1;
  event->code2 = (uint32_t)code2;

  /* Lock the queue */
  nxmutex_lock(&g_event_queue_mutex);

  /* Add event to queue */
  sq_addlast(&event->node, &g_event_queue);

  /* Unlock the queue */
  nxmutex_unlock(&g_event_queue_mutex);

  /* Queue the event handler if work is available */
  ret = work_queue(LPWORK, &g_event_work, async_event_handler, NULL, 0);
  if (ret != 0)
    {
      wlerr("Failed to queue event handler work\n");
    }
}

/**
 ****************************************************************************************
 * @brief Get MAC Address
 *
 * @param[in] vif VIF type (STA/AP) - for future multi-MAC support
 * @param[out] mac Buffer of hold mac address.
 * @return 0 on success and OTHERS if error occurred.
 ****************************************************************************************
 */
int platform_get_mac(enum wl80211_vif_type vif, uint8_t mac[6])
{
  uint8_t tmp[8];
  UNUSED(vif);

#ifdef CONFIG_BL616_WLAN_BACKUPMAC
  bflb_efuse_get_chipid(tmp);
#else
  bl616_efuse_read_mac_address(tmp);
#endif

  mac[0] = tmp[0];
  mac[1] = tmp[1];
  mac[2] = tmp[2];
  mac[3] = tmp[3];
  mac[4] = tmp[4];
  mac[5] = tmp[5];

  return 0;
}

/**
 ****************************************************************************************
 * @brief Get platform feature array index
 *
 * @return Index into platform feature array based on chip type
 ****************************************************************************************
 */
int platform_feature_index(void)
{
#if defined(BL618DG)
    return 0;
#elif defined(BL616CL) || defined(BL616)
    return 1;
#endif
}

/**
 ****************************************************************************************
 * @brief WiFi driver printf function (redirects to syslog)
 *
 * @param[in] fmt Printf-style format string
 * @param[in] ... Variable arguments
 * @return 0 on success
 ****************************************************************************************
 */
int wl80211_printf(const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  vsyslog(LOG_DEBUG, fmt, args);
  va_end(args);

  return 0;
}
