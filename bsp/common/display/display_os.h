/**
 * @file display_os.h
 * @brief Platform adaptation: bus lock, GPIO, critical section and delay.
 *
 * The SDK implementation is display_os.c (FreeRTOS or bare metal);
 * host tests provide their own implementation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_OS_H
#define DISPLAY_OS_H

#include "display_types.h"

#ifdef CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_OS Platform adaptation
 * @{
 */

/** @brief Upper bound of waiting for a bus, in milliseconds. */
#ifndef CONFIG_DISPLAY_BUS_LOCK_TIMEOUT_MS
#define CONFIG_DISPLAY_BUS_LOCK_TIMEOUT_MS 1000
#endif

/** @brief Timeout value of display_os_sem_take(): wait forever. */
#define DISPLAY_OS_WAIT_FOREVER UINT32_MAX

/**
 * @brief Binary semaphore used as the bus lock: available = the bus is idle.
 *
 * Taken in a task; given in a task or in an ISR (a DMA transfer releases the bus from its completion
 * interrupt), therefore it is not a mutex. The same semaphore tells a synchronous caller that its DMA
 * transfer is done, see display_bus_driver.h.
 */
typedef struct {
#ifdef CONFIG_FREERTOS
    StaticSemaphore_t storage; /**< Static storage */
    SemaphoreHandle_t handle;  /**< Handle */
#else
    volatile uint8_t available; /**< 1: can be taken */
#endif
} display_os_sem_t;

/**
 * @brief Initialize a semaphore in the available state.
 *
 * @param [out] sem semaphore, storage provided by the caller
 * @retval 0 success
 * @retval -ENOMEM creation failed
 */
int display_os_sem_init(display_os_sem_t *sem);

/**
 * @brief Delete a semaphore.
 *
 * @param [in,out] sem semaphore
 */
void display_os_sem_deinit(display_os_sem_t *sem);

/**
 * @brief Take a semaphore, task context only.
 *
 * @param [in,out] sem        semaphore
 * @param [in]     timeout_ms upper bound of waiting, or DISPLAY_OS_WAIT_FOREVER
 * @retval 0 success
 * @retval -ETIMEDOUT not available in time
 */
int display_os_sem_take(display_os_sem_t *sem, uint32_t timeout_ms);

/**
 * @brief Give a semaphore from a task or an ISR.
 *
 * @param [in,out] sem semaphore
 */
void display_os_sem_give(display_os_sem_t *sem);

/**
 * @brief Configure a GPIO as output without glitch: the level is written before the output is enabled.
 *
 * @param [in] pin   GPIO number
 * @param [in] level initial level
 */
void display_os_gpio_output(uint8_t pin, bool level);

/**
 * @brief Write a GPIO level. ISR safe.
 *
 * @param [in] pin   GPIO number
 * @param [in] level level
 */
void display_os_gpio_write(uint8_t pin, bool level);

/**
 * @brief Enter a critical section by disabling interrupts. Sections must be short and may nest when the
 *        saved state is restored in reverse order.
 *
 * @return saved interrupt state, to be passed to display_os_irq_restore()
 */
uintptr_t display_os_irq_save(void);

/**
 * @brief Leave a critical section.
 *
 * @param [in] flags value returned by display_os_irq_save()
 */
void display_os_irq_restore(uintptr_t flags);

/**
 * @brief Write back the data cache lines of a buffer that a DMA or a display engine is going to read. The data
 *        stays in the cache. Does nothing for an address that is not cached.
 *        May be used in an ISR; large ranges increase interrupt latency.
 *
 * @param [in] addr start of the buffer
 * @param [in] len  size in bytes
 */
void display_os_dcache_clean(const void *addr, size_t len);

/**
 * @brief Check whether the caller runs in an ISR.
 *
 * @return true in an ISR; always false on bare metal
 */
bool display_os_in_isr(void);

/**
 * @brief Delay. Yields the CPU when a scheduler is running.
 *
 * @param [in] ms milliseconds
 */
void display_os_delay_ms(uint32_t ms);

/**
 * @brief Drive a pin to its active or inactive level. No effect when the pin is not wired.
 *
 * @param [in] pin    pin description
 * @param [in] active true for the active level
 */
static inline void display_pin_set(const display_pin_t *pin, bool active)
{
    if (display_pin_used(pin)) {
        display_os_gpio_write(pin->num, (pin->flags & DISPLAY_PIN_ACTIVE_LOW) ? !active : active);
    }
}

/**
 * @brief Configure a pin as output at its active or inactive level. No effect when the pin is not wired.
 *
 * @param [in] pin    pin description
 * @param [in] active true for the active level
 */
static inline void display_pin_output(const display_pin_t *pin, bool active)
{
    if (display_pin_used(pin)) {
        display_os_gpio_output(pin->num, (pin->flags & DISPLAY_PIN_ACTIVE_LOW) ? !active : active);
    }
}

/** @} */ /* DISPLAY_OS */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_OS_H */
