#ifndef _SYSINIT_H_
#define _SYSINIT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __SI_PRIO(s)    #s
#define SI_PRIO(s)      __SI_PRIO(s)

/* ---- Subsystem IDs (2-digit zero-padded hex strings) ---- */
#define SI_SUB_LOWEST    SI_PRIO(00)
#define SI_SUB_HW        SI_PRIO(10)
#define SI_SUB_CLOCK     SI_PRIO(20)
#define SI_SUB_INTR      SI_PRIO(30)
#define SI_SUB_DRIVERS   SI_PRIO(40)
/* Service subsystems layered on top of the drivers: filesystem (Flash MTD,
 * LittleFS, FATFS), key-value store, and other services such as low power. */
#define SI_SUB_SERVICES  SI_PRIO(50)
/* Records that must run after the service records but before the scheduler
 * barrier, such as the global IRQ enable. RC32K/RC32M trim and board DCDC
 * setup are NOT placed here: they run earlier, inside System_Post_Init()
 * before sysinit_run_pre_schedule(), because the board clock/RTC records
 * depend on trimmed clocks (same order as before SYSINIT). */
#define SI_SUB_PRE_SCHED SI_PRIO(52)
/* Separator between the pre-scheduler and post-scheduler SYSINIT records.
 * The FreeRTOS integration registers the only entry of this subsystem: its
 * callback creates the continuation task, starts the scheduler and never
 * returns. Entries sorted after this subsystem run in that task.
 *
 * Post-scheduler order:
 *   SI_SUB_POST_SCHED first post-scheduler phase (reserved)
 *   SI_SUB_EVENTS   async event loop ready
 *   SI_SUB_WIRELESS RF / Wi-Fi backend bring-up (synchronous)
 *   SI_SUB_NET      reserved for a future TCP/IP hand-off (no records today)
 *   SI_SUB_APP      application main(); SI_ORDER_MIDDLE enters main()
 * The Wi-Fi backend and TCP/IP finish synchronously before APP, so applications
 * can use both the network stack and the Wi-Fi APIs directly from main(). The
 * application-facing INIT_DONE/MGMR_DONE compatibility events are posted by the
 * backend's APP/LAST record after main() returns, not from the NET phase. */
#define SI_SUB_SCHED     SI_PRIO(55)
/* First post-scheduler phase: reserved for common records that must run in the
 * continuation task before the component phases below. */
#define SI_SUB_POST_SCHED SI_PRIO(56)
/* Async event loop initialization; only the enabled Wi-Fi backend registers
 * this phase (the FreeRTOS integration registers SCHED and APP records). It
 * runs before WIRELESS. */
#define SI_SUB_EVENTS    SI_PRIO(58)
/* RF and Wi-Fi backend bring-up; runs synchronously before the application
 * entry (NET is reserved). */
#define SI_SUB_WIRELESS  SI_PRIO(70)
/* Reserved for a future TCP/IP hand-off record; there are no registrations
 * today. TCP/IP is initialized by the WIRELESS backend records and the
 * application-facing compatibility events are posted by APP/LAST after
 * main() returns. */
#define SI_SUB_NET       SI_PRIO(80)
/* Application main(); the FreeRTOS integration registers the only entry at
 * SI_ORDER_MIDDLE. It runs after WIRELESS, so initialization is complete when
 * main() starts; APP/LAST records post the application-facing compatibility
 * events after main() returns. */
#define SI_SUB_APP       SI_PRIO(90)
#define SI_SUB_HIGHEST   SI_PRIO(ff)

/* ---- Order IDs (2-digit zero-padded hex strings) ---- */
#define SI_ORDER_FIRST  SI_PRIO(00)
#define SI_ORDER_SECOND SI_PRIO(01)
#define SI_ORDER_THIRD  SI_PRIO(02)
#define SI_ORDER_FOURTH SI_PRIO(03)
#define SI_ORDER_FIFTH  SI_PRIO(04)
#define SI_ORDER_MIDDLE SI_PRIO(80)
#define SI_ORDER_LAST   SI_PRIO(ff)

/* ---- Data structure ---- */
struct sysinit_item {
    void (*func)(const void *arg);
    const void *arg;
    const char *name;
};

/* ---- Registration macro ---- */
#ifdef CONFIG_SYSINIT
#define SYSINIT(_name, _sub, _ord, _fn, _arg)                                    \
    static const struct sysinit_item _sysinit_##_name                            \
        __attribute__((section(".sysinit." _sub "." _ord "." #_name), used)) = { \
            .func = (_fn),                                                       \
            .arg = (const void *)(_arg),                                         \
            .name = #_name,                                                      \
        }
#else
/* Keep callbacks referenced and type-checked without emitting a table entry. */
#define SYSINIT(_name, _sub, _ord, _fn, _arg)                    \
    static inline void __attribute__((unused))                  \
        _sysinit_disabled_##_name(void)                         \
    {                                                          \
        (_fn)((const void *)(_arg));                            \
    }
#endif

/* ---- Runtime API ---- */
/* Run the SYSINIT table from the beginning. When a scheduler barrier is
 * registered (CONFIG_SYSINIT && CONFIG_FREERTOS) this function does not
 * return: the barrier starts the scheduler and the remaining entries are
 * executed in task context. */
void sysinit_run_pre_schedule(void);

/* Run the SYSINIT entries that follow the scheduler barrier. Only the
 * continuation task created by the barrier integration may call this. */
void sysinit_run_post_schedule(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYSINIT_H_ */
