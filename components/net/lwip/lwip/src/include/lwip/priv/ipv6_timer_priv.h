#ifndef LWIP_HDR_IPV6_TIMER_PRIV_H
#define LWIP_HDR_IPV6_TIMER_PRIV_H

#include "lwip/opt.h"

#if LWIP_IPV6 && IPV6_TIMER_PRECISE_NEEDED

#include "lwip/def.h"
#include "lwip/sys.h"

/* sys_timeout() accepts delays up to LWIP_UINT32_MAX / 4. */
#define LWIP_IPV6_TIMER_MAX_DELAY_MS   (LWIP_UINT32_MAX / 4U)
#define LWIP_IPV6_TIMER_MAX_DELAY_SECS (LWIP_IPV6_TIMER_MAX_DELAY_MS / 1000U)

static inline u32_t
lwip_ipv6_timer_deadline_from_ms_at(u32_t now, u32_t delay_ms)
{
  delay_ms = LWIP_MIN(LWIP_MAX(delay_ms, 1U), LWIP_IPV6_TIMER_MAX_DELAY_MS);
  return now + delay_ms;
}

static inline u32_t
lwip_ipv6_timer_deadline_from_ms(u32_t delay_ms)
{
  return lwip_ipv6_timer_deadline_from_ms_at(sys_now(), delay_ms);
}

static inline u32_t
lwip_ipv6_timer_deadline_from_secs_at(u32_t now, u32_t delay_secs)
{
  u32_t step_secs = LWIP_MIN(LWIP_MAX(delay_secs, 1U),
                             LWIP_IPV6_TIMER_MAX_DELAY_SECS);

  return lwip_ipv6_timer_deadline_from_ms_at(now, step_secs * 1000U);
}

static inline u32_t
lwip_ipv6_timer_deadline_from_secs(u32_t delay_secs)
{
  return lwip_ipv6_timer_deadline_from_secs_at(sys_now(), delay_secs);
}

static inline int
lwip_ipv6_timer_deadline_due(u32_t deadline, u32_t now)
{
  return ((s32_t)(now - deadline) >= 0);
}

static inline u32_t
lwip_ipv6_timer_deadline_remaining(u32_t deadline, u32_t now)
{
  if (lwip_ipv6_timer_deadline_due(deadline, now)) {
    return 0;
  }

  return deadline - now;
}

static inline u32_t
lwip_ipv6_timer_deadline_sleeptime(u32_t deadline, u32_t now)
{
  return LWIP_MAX(lwip_ipv6_timer_deadline_remaining(deadline, now), 1U);
}

/* Return the effective remaining millisecond interval represented by a
 * chunked deadline. */
static inline u32_t
lwip_ipv6_timer_lifetime_remaining_ms(u32_t lifetime_ms,
                                      u32_t deadline, u32_t now)
{
  u32_t step_ms;

  if (lifetime_ms == 0) {
    return 0;
  }

  step_ms = LWIP_MIN(lifetime_ms, LWIP_IPV6_TIMER_MAX_DELAY_MS);
  if (!lwip_ipv6_timer_deadline_due(deadline, now)) {
    return (lifetime_ms - step_ms) + (deadline - now);
  } else {
    u32_t overdue_ms = now - deadline;
    u32_t elapsed_ms = step_ms + overdue_ms;

    return (lifetime_ms > elapsed_ms) ? (lifetime_ms - elapsed_ms) : 0;
  }
}

/* Return the effective remaining lifetime represented by a chunked deadline. */
static inline u32_t
lwip_ipv6_timer_lifetime_remaining_secs(u32_t lifetime_secs,
                                        u32_t deadline, u32_t now)
{
  u32_t step_secs;

  if (lifetime_secs == 0) {
    return 0;
  }

  step_secs = LWIP_MIN(lifetime_secs, LWIP_IPV6_TIMER_MAX_DELAY_SECS);
  if (!lwip_ipv6_timer_deadline_due(deadline, now)) {
    u32_t remaining_ms = deadline - now;
    u32_t remaining_step_secs = (remaining_ms + 999U) / 1000U;

    return (lifetime_secs - step_secs) + remaining_step_secs;
  } else {
    u32_t overdue_secs = (now - deadline) / 1000U;
    u32_t elapsed_secs = step_secs + overdue_secs;

    return (lifetime_secs > elapsed_secs) ?
           (lifetime_secs - elapsed_secs) : 0;
  }
}

#endif /* LWIP_IPV6 && IPV6_TIMER_PRECISE_NEEDED */

#endif /* LWIP_HDR_IPV6_TIMER_PRIV_H */
