#ifndef __PHYRF_ARCH_COMPAT_H__
#define __PHYRF_ARCH_COMPAT_H__

#include <stdint.h>

#include "compiler/compiler_gcc.h"
#include "csi_rv32_gcc.h"
#include "bflb_irq.h"

/*
 * CPU WORD SIZE
 ****************************************************************************************
 */
#ifndef CPU_WORD_SIZE
#define CPU_WORD_SIZE 4
#endif

/*
 * CPU Endianness
 ****************************************************************************************
 */
#ifndef CPU_LE
#define CPU_LE 1
#endif

/*
 * ASSERTION CHECK
 ****************************************************************************************
 */
void phy_assert_rec(const char *condition, const char *file, int line);
void phy_assert_err(const char *condition, const char *file, int line);
void phy_assert_warn(const char *condition, const char *file, int line);

#ifndef ASSERT
#define ASSERT(type, cond) phy_assert_##type(cond, "module", __LINE__)
#endif

#ifndef ASSERT_ERR
#if PHYRF_ASSERT_EN
#define ASSERT_ERR(cond)                              \
    do {                                              \
        if (!(cond)) {                                \
            ASSERT(err, #cond);                       \
        }                                             \
    } while (0)
#else
#define ASSERT_ERR(cond) do { } while (0)
#endif
#endif

#ifndef ASSERT_WARN
#if PHYRF_ASSERT_EN
#define ASSERT_WARN(cond)                             \
    do {                                              \
        if (!(cond)) {                                \
            ASSERT(warn, #cond);                      \
        }                                             \
    } while (0)
#else
#define ASSERT_WARN(cond) do { } while (0)
#endif
#endif

#ifndef ASSERT_REC
#if PHYRF_ASSERT_EN
#define ASSERT_REC(cond)                              \
    do {                                              \
        if (!(cond)) {                                \
            ASSERT(rec, #cond);                       \
            return;                                   \
        }                                             \
    } while (0)
#else
#define ASSERT_REC(cond) do { } while (0)
#endif
#endif

#ifndef ASSERT_REC_VAL
#if PHYRF_ASSERT_EN
#define ASSERT_REC_VAL(cond, ret)                     \
    do {                                              \
        if (!(cond)) {                                \
            ASSERT(rec, #cond);                       \
            return (ret);                             \
        }                                             \
    } while (0)
#else
#define ASSERT_REC_VAL(cond, ret) do { } while (0)
#endif
#endif

#ifndef ASSERT_REC_NO_RET
#if PHYRF_ASSERT_EN
#define ASSERT_REC_NO_RET(cond)                       \
    do {                                              \
        if (!(cond)) {                                \
            ASSERT(rec, #cond);                       \
        }                                             \
    } while (0)
#else
#define ASSERT_REC_NO_RET(cond) do { } while (0)
#endif
#endif

/* The two halves form one split do { ... } while (0) critical section and
 * must be defined - or platform-suppressed - as a unit: with independent
 * #ifndef gates, a platform that predefines only one half leaves the other
 * expanding to nothing, breaking the brace balance (or referencing an
 * undeclared irq_rest) on every call site. */
#ifndef GLOBAL_INT_DISABLE
#define GLOBAL_INT_DISABLE()                                                  \
    do {                                                                      \
        uintptr_t irq_rest = bflb_irq_save();

#define GLOBAL_INT_RESTORE()                                                  \
        bflb_irq_restore(irq_rest);                                           \
    } while (0);
#endif

#ifndef __SHAREDRAM
#define __SHAREDRAM __attribute__((section("SHAREDRAM")))
#endif

#ifndef BTBLE_IF_OPT_NONE
#define BTBLE_IF_OPT_NONE        0x00
#define BTBLE_IF_OPT_BLE         0x10
#define BTBLE_IF_OPT_BT          0x20
#define BTBLE_IF_OPT_BTBLE       0x30
#define BTBLE_IF_OPT_BTBLE_MASK  0x30

#define BTBLE_IF_OPT_UART        0x01
#define BTBLE_IF_OPT_USB         0x02
#define BTBLE_IF_OPT_SDU         0x03
#define BTBLE_IF_OPT_IF_MASK     0x03

#define BTBLE_IF_OPT_BLE_UART    (BTBLE_IF_OPT_BLE | BTBLE_IF_OPT_UART)
#define BTBLE_IF_OPT_BLE_USB     (BTBLE_IF_OPT_BLE | BTBLE_IF_OPT_USB)
#define BTBLE_IF_OPT_BLE_SDU     (BTBLE_IF_OPT_BLE | BTBLE_IF_OPT_SDU)
#define BTBLE_IF_OPT_BT_UART     (BTBLE_IF_OPT_BT | BTBLE_IF_OPT_UART)
#define BTBLE_IF_OPT_BT_USB      (BTBLE_IF_OPT_BT | BTBLE_IF_OPT_USB)
#define BTBLE_IF_OPT_BT_SDU      (BTBLE_IF_OPT_BT | BTBLE_IF_OPT_SDU)
#define BTBLE_IF_OPT_BTBLE_UART  (BTBLE_IF_OPT_BTBLE | BTBLE_IF_OPT_UART)
#define BTBLE_IF_OPT_BTBLE_USB   (BTBLE_IF_OPT_BTBLE | BTBLE_IF_OPT_USB)
#define BTBLE_IF_OPT_BTBLE_SDU   (BTBLE_IF_OPT_BTBLE | BTBLE_IF_OPT_SDU)
#endif

#ifndef CFG_BTBLE_HCI_ENABLED
#define CFG_BTBLE_HCI_ENABLED(x)       ((x) & BTBLE_IF_OPT_BTBLE_MASK)
#define CFG_BTBLE_HCI_BLE_ENABLED(x)   (((x) & BTBLE_IF_OPT_BTBLE_MASK) == BTBLE_IF_OPT_BLE)
#define CFG_BTBLE_HCI_BT_ENABLED(x)    (((x) & BTBLE_IF_OPT_BTBLE_MASK) == BTBLE_IF_OPT_BT)
#define CFG_BTBLE_HCI_BTBLE_ENABLED(x) (((x) & BTBLE_IF_OPT_BTBLE_MASK) == BTBLE_IF_OPT_BTBLE)

#define CFG_BTBLE_HCI_UART_ENABLED(x)  (((x) & BTBLE_IF_OPT_IF_MASK) == BTBLE_IF_OPT_UART)
#define CFG_BTBLE_HCI_USB_ENABLED(x)   (((x) & BTBLE_IF_OPT_IF_MASK) == BTBLE_IF_OPT_USB)
#define CFG_BTBLE_HCI_SDU_ENABLED(x)   (((x) & BTBLE_IF_OPT_IF_MASK) == BTBLE_IF_OPT_SDU)
#endif

/// Macro checking if a pointer is part of the shared RAM
#ifndef TST_SHRAM_PTR
#define TST_SHRAM_PTR(ptr) ((((uint32_t)(ptr)) < (uint32_t)&_sshram) ||                   \
                            (((uint32_t)(ptr)) >= (uint32_t)&_eshram))
#endif

/// Macro checking if a pointer is part of the shared RAM
#ifndef CHK_SHRAM_PTR
#define CHK_SHRAM_PTR(ptr) \
    do {                   \
        if (TST_SHRAM_PTR(ptr)) { \
            return;        \
        }                  \
    } while (0)
#endif

#endif
