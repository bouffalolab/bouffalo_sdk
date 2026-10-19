#ifndef _SYSINIT_H_
#define _SYSINIT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __SI_PRIO(s)    #s
#define SI_PRIO(s)      __SI_PRIO(s)

/* ---- Subsystem IDs (2-digit zero-padded hex strings) ---- */
#define SI_SUB_LOWEST   SI_PRIO(00)
#define SI_SUB_HW       SI_PRIO(10)
#define SI_SUB_CLOCK    SI_PRIO(20)
#define SI_SUB_INTR     SI_PRIO(30)
#define SI_SUB_DRIVERS  SI_PRIO(40)
#define SI_SUB_FS       SI_PRIO(50)
#define SI_SUB_NET      SI_PRIO(60)
#define SI_SUB_WIRELESS SI_PRIO(70)
#define SI_SUB_APP      SI_PRIO(80)
#define SI_SUB_HIGHEST  SI_PRIO(ff)

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
#define SYSINIT(_name, _sub, _ord, _fn, _arg)                                    \
    static const struct sysinit_item _sysinit_##_name                            \
        __attribute__((section(".sysinit." _sub "." _ord "." #_name), used)) = { \
            .func = (_fn),                                                       \
            .arg = (const void *)(_arg),                                         \
            .name = #_name,                                                      \
        }

/* ---- Runtime API ---- */
void sysinit_run_all(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYSINIT_H_ */
