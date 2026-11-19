/**
 * @file display_os.c
 * @brief Platform adaptation for the Bouffalo SDK (FreeRTOS or bare metal).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "display_os.h"
#include "bflb_gpio.h"
#include "bflb_irq.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#ifdef CONFIG_FREERTOS
#include "task.h"
#endif

/**
 * @brief Get the GPIO device, looked up once on first use.
 */
static struct bflb_device_s *gpio_dev(void)
{
    static struct bflb_device_s *gpio;

    if (!gpio) {
        gpio = bflb_device_get_by_name("gpio");
    }
    return gpio;
}

void display_os_gpio_write(uint8_t pin, bool level)
{
    if (level) {
        bflb_gpio_set(gpio_dev(), pin);
    } else {
        bflb_gpio_reset(gpio_dev(), pin);
    }
}

void display_os_gpio_output(uint8_t pin, bool level)
{
    /* Write the output register first, then enable the output, then write again
     * because bflb_gpio_init() may reset the output value. */
    display_os_gpio_write(pin, level);
    bflb_gpio_init(gpio_dev(), pin, GPIO_OUTPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_1);
    display_os_gpio_write(pin, level);
}

uintptr_t display_os_irq_save(void)
{
    return bflb_irq_save();
}

void display_os_irq_restore(uintptr_t flags)
{
    bflb_irq_restore(flags);
}

void display_os_dcache_clean(const void *addr, size_t len)
{
    bflb_l1c_dcache_clean_range((void *)addr, (uint32_t)len);
}

void display_os_delay_ms(uint32_t ms)
{
#ifdef CONFIG_FREERTOS
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING && !display_os_in_isr()) {
        TickType_t ticks = pdMS_TO_TICKS(ms);

        /* Never round a short delay down to zero ticks, which would not wait at all */
        vTaskDelay(ticks ? ticks : 1);
        return;
    }
#endif
    bflb_mtimer_delay_ms(ms);
}

#ifdef CONFIG_FREERTOS

bool display_os_in_isr(void)
{
    return xPortIsInsideInterrupt() != pdFALSE;
}

int display_os_sem_init(display_os_sem_t *sem)
{
    sem->handle = xSemaphoreCreateBinaryStatic(&sem->storage);
    if (!sem->handle) {
        return -ENOMEM;
    }
    xSemaphoreGive(sem->handle);
    return 0;
}

void display_os_sem_deinit(display_os_sem_t *sem)
{
    vSemaphoreDelete(sem->handle);
    sem->handle = NULL;
}

int display_os_sem_take(display_os_sem_t *sem, uint32_t timeout_ms)
{
    TickType_t ticks = timeout_ms == DISPLAY_OS_WAIT_FOREVER ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);

    return xSemaphoreTake(sem->handle, ticks) == pdTRUE ? 0 : -ETIMEDOUT;
}

void display_os_sem_give(display_os_sem_t *sem)
{
    if (display_os_in_isr()) {
        BaseType_t woken = pdFALSE;

        xSemaphoreGiveFromISR(sem->handle, &woken);
        portYIELD_FROM_ISR(woken); /* switch at once if a higher priority task was waiting */
    } else {
        xSemaphoreGive(sem->handle);
    }
}

#else /* bare metal */

bool display_os_in_isr(void)
{
    return false;
}

int display_os_sem_init(display_os_sem_t *sem)
{
    sem->available = 1;
    return 0;
}

void display_os_sem_deinit(display_os_sem_t *sem)
{
    sem->available = 0;
}

int display_os_sem_take(display_os_sem_t *sem, uint32_t timeout_ms)
{
    uint64_t start = bflb_mtimer_get_time_ms();

    /* Bare metal: poll; the flag may be given from an ISR, so test-and-clear with interrupts off */
    for (;;) {
        uintptr_t flags = display_os_irq_save();

        if (sem->available) {
            sem->available = 0;
            display_os_irq_restore(flags);
            return 0;
        }
        display_os_irq_restore(flags);

        if (timeout_ms != DISPLAY_OS_WAIT_FOREVER && bflb_mtimer_get_time_ms() - start >= timeout_ms) {
            return -ETIMEDOUT;
        }
    }
}

void display_os_sem_give(display_os_sem_t *sem)
{
    sem->available = 1;
}

#endif /* CONFIG_FREERTOS */
